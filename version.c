
/*
** version.c
*/

#include <exec/types.h>

#include "version.h"

/***************************************************************************/

UBYTE *version =  "$VER: " APPLICATIONNAME " " I2S(VERSION) "." I2S(REVISION) " (" I2S(DAY) "." I2S(MONTH) "." I2S(YEAR) ") © " I2S(YEAR) " " AUTHORNAME VERSION_SYSTEM;

/***************************************************************************/

