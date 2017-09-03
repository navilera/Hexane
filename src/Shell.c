/*
 * Shell.c
 *
 *  Created on: Aug 20, 2017
 *      Author: maanu
 */

#include "Main.h"
#include "Shell.h"

#include <unistd.h>
#include <termios.h>

static ShellCmdHistory_t* cmdHistory = NULL;
static ShellCmdHistory_t* lastHistory = NULL;

static char lineBuffer[SHELL_LINE_LEN_LIMIT];

static void addCmdHistory(void);
static void keyBoardControl(void);
static char* getHistory(char updn);
static void inArrayAdjustment(int pos, bool del_ins, char ch);

#define ESCAPE '\33'
#define BACKSPACE '\177'
#define DELETE '~'

static void debuglinebuf(void)
{
	printf("DD");
	for(int i = 0 ; i < 16 ; i++)
	{
		printf(" %02x|", lineBuffer[i]);
	}
	printf("\n");
}

char* Shell_GetLine(void)
{
	fputs(SHELL_PROMPT, stdout);
	memset(lineBuffer, 0, sizeof(lineBuffer));

	//lineBufStartIdx = strlen(lineBuffer);

	//fgets(&lineBuffer[lineBufStartIdx], sizeof(lineBuffer), stdin);
	keyBoardControl();
	addCmdHistory();

	debuglinebuf();

	return lineBuffer;
}

static void keyBoardControl(void)
{
	int lineBufferLen = 0;
	int currentCursor = 0;
	struct termios termios_p;
	struct termios termios_save;

	fflush(stdout);

	tcgetattr(0, &termios_p);
	termios_save = termios_p;

	termios_p.c_lflag &= ~(ICANON|ECHO);
	tcsetattr(0,TCSANOW, &termios_p);
	char buff;
	while(read(0, &buff, 1) >= 0)
	{
		if(buff > 0)
		{
			if(buff == ESCAPE)
			{
				char* history = NULL;

				read(0, &buff, 1);
				read(0, &buff, 1);
				if(buff == 'A')
				{
					history = getHistory('A');
				}
				else if(buff == 'B')
				{
					history = getHistory('B');
				}
				else if(buff == 'C')
				{
					if(currentCursor < lineBufferLen)
					{
						write(2, "\33[C", 3);
						currentCursor++;
					}
				}
				else if(buff == 'D')
				{
					if(currentCursor > 0)
					{
						write(2, "\10", 2);
						currentCursor--;
					}
				}

				if(history != NULL)
				{
					if(lineBufferLen > 0)
					{
						while(lineBufferLen--)
						{
							// delete current input line
							write(2, "\10\33[1P", 5);
						}
					}
					//write(2, "\10\33[1P", 5); // current cursor position
					memset(lineBuffer, 0, SHELL_LINE_LEN_LIMIT);
					lineBufferLen = 0;
					for(char* ch = history ; *ch != '\0' ; ch++)
					{
						write(2,ch,1);
						lineBuffer[lineBufferLen++] = *ch;
						currentCursor = lineBufferLen;
					}
				}
			}
			else if(buff == BACKSPACE)
			{
				if(currentCursor > 0)
				{
					write(2, "\10\33[1P", 5);
					inArrayAdjustment(--currentCursor, true, 0);
					lineBufferLen--;
				}
			}
			else if(buff == DELETE)
			{
				write(2, "\33[1P", 4);
				inArrayAdjustment(currentCursor, true, 0);
				lineBufferLen--;
			}
			else
			{
				write(2,&buff,1);
				if((buff == '\n') || (currentCursor == lineBufferLen))
				{
					lineBuffer[lineBufferLen++] = buff;
					currentCursor = lineBufferLen;
				}
				else
				{
					if(buff != '\n')
					{
						inArrayAdjustment(currentCursor, false, buff);
						lineBufferLen++;
					}
				}
			}
			if(buff == '\n')
			{
				lineBuffer[lineBufferLen] = '\0';
				break;
			}
		}
	}
	tcsetattr(0,TCSANOW, &termios_save);
}

static void inArrayAdjustment(int pos, bool del_ins, char ch)
{
	if(del_ins == true)
	{
		// delete
		for(int i = pos ; i < (SHELL_LINE_LEN_LIMIT - 1) ; ++i)
		{
			lineBuffer[i] = lineBuffer[i+1];
		}
	}
	else
	{
		// insert
		for(int i = (SHELL_LINE_LEN_LIMIT - 2) ; i >= pos  ; --i)
		{
			lineBuffer[i+1] = lineBuffer[i];
		}
		lineBuffer[pos] = ch;
	}
}

static void addCmdHistory(void)
{
	ShellCmdHistory_t* node = (ShellCmdHistory_t*)malloc(sizeof(ShellCmdHistory_t));
	memset(node, 0, sizeof(ShellCmdHistory_t));
	memcpy(node->cmd, lineBuffer, SHELL_LINE_LEN_LIMIT);

	char* enterPtr = strchr(node->cmd, '\n');
	*enterPtr = '\0';

	if(cmdHistory == NULL)
	{
		cmdHistory = node;
		node->prev = NULL;
		node->next = NULL;
	}
	else
	{
		ShellCmdHistory_t* itr = cmdHistory;
		while(itr->next != NULL)
		{
			itr = itr->next;
		}

		itr->next = node;
		node->prev = itr;
		node->next = NULL;
	}

	lastHistory = node;
}

static char* getHistory(char updn)
{
	char* ret = NULL;
	if (cmdHistory)
	{
		ret = lastHistory->cmd;

		if (updn == 'A')
		{
			lastHistory = (lastHistory->prev == NULL) ? lastHistory : lastHistory->prev;
		}
		else if(updn == 'B')
		{
			lastHistory = (lastHistory->next == NULL) ? lastHistory : lastHistory->next;
		}
		else
		{
			return NULL;
		}
	}

	return ret;
}
