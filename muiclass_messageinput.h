/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_MESSAGEINPUT_H
#define MUICLASS_MESSAGEINPUT_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define MessageInputObject NEWOBJECT( appclasses[ CLASSID_MESSAGEINPUT ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_MESSAGEINPUT_ =  0xFED00120,
MM_MESSAGEINPUT_FIND,
MM_MESSAGEINPUT_ADD,
MM_MESSAGEINPUT_REMOVE,
/* Attributes */
MA_MESSAGEINPUT_OBJECTSETTINGS,
MA_MESSAGEINPUT_OBJECTNETWORK,
MA_MESSAGEINPUT_OBJECTCHATUSERLIST,
MA_MESSAGEINPUT_OBJECTCHATCHANNELLIST,
};

struct MP_MESSAGEINPUT_FIND   { ULONG MethodID; APTR First; char *Message; ULONG Length; ULONG Mode; ULONG Direction; };
struct MP_MESSAGEINPUT_ADD    { ULONG MethodID; char *Message; };
struct MP_MESSAGEINPUT_REMOVE { ULONG MethodID; APTR History; };

#define MV_MESSAGEINPUT_MODE_EXACT   1
#define MV_MESSAGEINPUT_MODE_PARTIAL 0

#define MV_MESSAGEINPUT_DIRECTION_DOWN 0
#define MV_MESSAGEINPUT_DIRECTION_UP   1

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_MessageInput_InitClass( void );
void    MCC_MessageInput_DisposeClass( void );


/*************************************************************************/

#endif /* MUICLASS_MESSAGEINPUT_H */

