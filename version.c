/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** version.c
*/

#include <exec/types.h>

#include "version.h"

/***************************************************************************/

UBYTE *version =  "$VER: " APPLICATIONNAME " " I2S(VERSION) "." I2S(REVISION) " (" I2S(DAY) "." I2S(MONTH) "." I2S(YEAR) ") © " I2S(YEAR) " " AUTHORNAME VERSION_SYSTEM;

/***************************************************************************/

