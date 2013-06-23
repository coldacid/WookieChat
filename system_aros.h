/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** system_aros.h
*/

#ifndef SYSTEM_AROS_H
#define SYSTEM_AROS_H

#ifdef SYSTEM_GENERAL_H /* we do not want this to be used directly */

/*************************************************************************/

#include <exec/types.h>

#include "debug.h"

/*************************************************************************/

/*
** typedefs
** ub   - UBYTE *
** s    - STRPTR
** cs   - CONST_STRTR
*/

typedef char *          c_in;
typedef unsigned int *  i_in;
typedef CONST_STRPTR    l_in;
typedef CONST_STRPTR    loc_in;
typedef CONST_STRPTR    _ub_cs;
typedef CONST_STRPTR    _s_cs;
#define NTIP const

#ifndef offsetof
#define offsetof(type, member)  __builtin_offsetof(type, member)
#endif

/*
** missign mui defines
*/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d)  ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#ifndef MUIC_DTPIC
#define MUIC_DTPIC "Dtpic.mui"
#endif

#ifndef MUIA_Dtpic_Name
#define MUIA_Dtpic_Name 0x80423d72
#endif

#ifndef MUIA_Application_IconifyTitle
#define MUIA_Application_IconifyTitle 0x80422cb8 /* V18 isg STRPTR            */
#endif

#ifndef MUIA_Imagedisplay_Spec
#define MUIA_Imagedisplay_Spec 0x8042a547
#endif

#ifndef MUIA_Window_DisableKeys
#define MUIA_Window_DisableKeys 0x80424c36
#endif

#ifndef MUIA_Text_HiIndex
 #define MUIA_Text_HiIndex 0x804214f5
#endif

/*************************************************************************/

#endif /* SYSTEM_GENERAL_H */

#endif /* SYSTEM_AROS_H */

