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
#include <sys/types.h>

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

#define SH_LEFTARROW "\10"
#define SH_RIGHTARROW "\33[C"
#define SH_BACKSPACE "\10\33[1P"
#define SH_DELETE "\33[1P"

char* Shell_GetLine(void)
{
	fputs(SHELL_PROMPT, stdout);
	memset(lineBuffer, 0, sizeof(lineBuffer));

	keyBoardControl();
	addCmdHistory();

	return lineBuffer;
}

static void clearPrompt(void)
{
	write(1, "\r", 1);
	for(int i = 0 ; i < SHELL_LINE_LEN_LIMIT ; i++)
	{
		// delete current input line
		write(1, SH_DELETE, 4);
	}
	write(1, SHELL_PROMPT, 3);
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
					// Arrow Up
					history = getHistory('A');
				}
				else if(buff == 'B')
				{
					// Arrow Down
					history = getHistory('B');
				}
				else if(buff == 'C')
				{
					// Arrow Right
					if(currentCursor < lineBufferLen)
					{
						write(1, SH_RIGHTARROW, 3);
						currentCursor++;
					}
				}
				else if(buff == 'D')
				{
					// Arrow Left
					if(currentCursor > 0)
					{
						write(1, SH_LEFTARROW, 2);
						currentCursor--;
					}
				}

				if(history != NULL)
				{
					// clear current input
					clearPrompt();
					memset(lineBuffer, 0, SHELL_LINE_LEN_LIMIT);
					lineBufferLen = 0;

					// replace current input to history
					strncpy(lineBuffer, history, strlen(history));
					lineBufferLen = strlen(history);
					currentCursor = lineBufferLen;

					// show display
					write(1, lineBuffer, lineBufferLen);
				}
			}
			else if(buff == BACKSPACE)
			{
				if(currentCursor > 0)
				{
					write(1, SH_BACKSPACE, 5);
					inArrayAdjustment(--currentCursor, true, 0);
					lineBufferLen--;
				}
			}
			else if(buff == DELETE)
			{
				write(1, SH_DELETE, 4);
				inArrayAdjustment(currentCursor, true, 0);
				lineBufferLen--;
			}
			else
			{
				write(1,&buff,1);
				if((buff == '\n') || (currentCursor == lineBufferLen))
				{
					// add input character into end of line buffer
					lineBuffer[lineBufferLen++] = buff;
					currentCursor = lineBufferLen;
				}
				else
				{
					if(buff != '\n')
					{
						// add input character into middle of line buffer
						inArrayAdjustment(currentCursor++, false, buff);
						lineBufferLen++;

						// clear current input
						clearPrompt();

						// show updated line buffer
						write(1, lineBuffer, lineBufferLen);

						// adjust current cursor position
						write(1, "\r", 1);
						write(1, SHELL_PROMPT, 3);
						for(int i = 0 ; i < currentCursor ; i++) write(1, SH_RIGHTARROW, 3);
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
