/*
 * Shell.h
 *
 *  Created on: Aug 20, 2017
 *      Author: maanu
 */

#ifndef SRC_SHELL_H_
#define SRC_SHELL_H_

#define SHELL_LINE_LEN_LIMIT	1024

#define SHELL_PROMPT			">> "

char* Shell_GetLine(void);

#endif /* SRC_SHELL_H_ */