/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef VERSION_H
#define VERSION_H 1

/***************************************************************************/

#include <exec/types.h>

/***************************************************************************/

#define VERSION  3
#define REVISION 0
#define DAY      05
#define MONTH    06
#define YEAR     2013

#define APPLICATIONNAME "WookieChat"
#define AUTHORNAME "WookieChat Team"

/***************************************************************************/

/*
** Macros
*/

#define I2S(x) I2S2(x)
#define I2S2(x) #x

/*
** system information
*/

#ifdef __amigaos4__
#define VERSION_SYSTEM "(AmigaOS 4.x binary)"
#elif __MORPHOS__
#define VERSION_SYSTEM "(MorphOS binary)"
#elif __AROS__
#define VERSION_SYSTEM "(AROS binary)"
#else
#define VERSION_SYSTEM "(AmigaOS 3.x binary)"
#endif

/***************************************************************************/

#define VERSION_MUI   &version[6]

#define VERSION_CTCP          APPLICATIONNAME " " I2S(VERSION) "." I2S(REVISION) " (" I2S(DAY) "." I2S(MONTH) "." I2S(YEAR) ") - http://sourceforge.net/projects/wookiechat/ - " VERSION_SYSTEM

#define VERSION_ABOUT "\033c" APPLICATIONNAME " " I2S(VERSION) "." I2S(REVISION) " (" I2S(DAY) "." I2S(MONTH) "." I2S(YEAR) ") - http://sourceforge.net/projects/wookiechat/\n" VERSION_SYSTEM

#define VERSION_COPYRIGHT "© " I2S(YEAR) " " AUTHORNAME

extern UBYTE *version;

/***************************************************************************/

#endif /* VERSION_H */

