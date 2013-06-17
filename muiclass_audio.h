/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_AUDIO_H
#define MUICLASS_AUDIO_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define AudioObject NEWOBJECT( appclasses[ CLASSID_AUDIO ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_AUDIO_PLAY =  0xFED00340,
/* Attributes */
};

struct MP_AUDIO_PLAY    { ULONG MethodID; ULONG SID; char *Name; };

/*************************************************************************/

enum{
SID_TABOPENING = 0,
SID_HIGHLIGHT,
SID_PRIVMSG,
SID_CTCP,
SID_LAST
};

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_Audio_InitClass( void );
void    MCC_Audio_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_AUDIO_H */

