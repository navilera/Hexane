/*
 * Main.h
 *
 *  Created on: Aug 23, 2017
 *      Author: maanu
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include <stddef.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define STR_CMP(strA, strB)		(strncmp(strA, strB, strlen(strB)) == 0) && (strlen(strA) == strlen(strB))


#endif /* SRC_MAIN_H_ */
