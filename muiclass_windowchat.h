/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_WINDOWCHAT_H
#define MUICLASS_WINDOWCHAT_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"
#include "muiclass_network.h"


#define WindowChatObject NEWOBJECT( appclasses[ CLASSID_WINDOWCHAT ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_WINDOWCHAT_DUMMY =  0xFED00020,
MM_WINDOWCHAT_MENUSELECT,
MM_WINDOWCHAT_COLORCHANGE,

MM_WINDOWCHAT_MESSAGERECEIVED,
MM_WINDOWCHAT_MESSAGEENTERED,
MM_WINDOWCHAT_CHANNELADD,
MM_WINDOWCHAT_CHANNELREMOVE,
/* Attributes */
};

struct MP_WINDOWCHAT_MENUSELECT { ULONG MethodID; ULONG MenuID; };

struct MP_WINDOWCHAT_MESSAGERECEIVED { ULONG MethodID; struct Channel *Channel; char *Message; ULONG Flags; };
struct MP_WINDOWCHAT_MESSAGEENTERED  { ULONG MethodID; };
struct MP_WINDOWCHAT_CHANNELREMOVE   { ULONG MethodID; struct Channel *Channel; };
struct MP_WINDOWCHAT_CHANNELADD      { ULONG MethodID; struct Channel *Channel; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowChat_InitClass( void );
void    MCC_WindowChat_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_WINDOWCHAT_H */

