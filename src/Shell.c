/*
 * Shell.c
 *
 *  Created on: Aug 20, 2017
 *      Author: maanu
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "Shell.h"

static char lineBuffer[SHELL_LINE_LEN_LIMIT];

char* Shell_GetLine(void)
{
	fputs(SHELL_PROMPT, stdout);

	memset(lineBuffer, 0, sizeof(lineBuffer));

	fgets(lineBuffer, sizeof(lineBuffer), stdin);

	return lineBuffer;
}
