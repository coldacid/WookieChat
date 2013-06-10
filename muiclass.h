/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_H
#define MUICLASS_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

/*************************************************************************/

/*
** Class ID Enumeration
*/

enum
{
CLASSID_WINDOWABOUT = 0,
CLASSID_WINDOWURLGRABBER,
CLASSID_WINDOWIGNORELIST,
CLASSID_LAST
};

extern struct MUI_CustomClass *appclasses[];

/*
** Prototypes
*/

ULONG MUIClass_Open ( void );
void  MUIClass_Close( void );

ULONG MUIGetVar( Object *obj, ULONG attr );

/*************************************************************************/

#endif /* MUICLASS_H */

