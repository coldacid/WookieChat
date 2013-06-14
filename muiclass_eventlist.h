/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_EVENTLIST_H
#define MUICLASS_EVENTLIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define EventListObject NEWOBJECT( appclasses[ CLASSID_EVENTLIST ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_EVENTLIST_ADD =  0xFED00350,
/* Attributes */
};

struct MP_EVENTLIST_ADD { ULONG MethodID; ULONG Type; ULONG Mode; STRPTR Script; STRPTR Text; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_EventList_InitClass( void );
void    MCC_EventList_DisposeClass( void );


/*************************************************************************/

/*
** event entry structure
*/

#define EVENTENTRY_SCRIPT_SIZEOF    256
#define EVENTENTRY_TEXT_SIZEOF       64

struct EventEntry {
	ULONG         ee_Type;
	ULONG         ee_Mode;
	char          ee_Script[ EVENTENTRY_SCRIPT_SIZEOF + 2 ];
	char          ee_Text[ EVENTENTRY_TEXT_SIZEOF + 2 ];
};

enum{ /* sort order must be equal to locales */
EVENTTYPE_CONNECT = 0,
EVENTTYPE_DISCONNECT,
EVENTTYPE_JOIN,
EVENTTYPE_PART,
EVENTTYPE_QUIT,
EVENTTYPE_DCCRECEIVEOFFERED,
EVENTTYPE_DCCRECEIVEDONE,
EVENTTYPE_DCCSENDEDONE,
EVENTTYPE_CHANNELMESSAGE,
EVENTTYPE_PRIVATEMESSAGE,
EVENTTYPE_NOTICE,
EVENTTYPE_CTCPREQUEST,
EVENTTYPE_KICK,
EVENTTYPE_MODECHANGE,
EVENTTYPE_INVITE,
EVENTTYPE_LAST,
};

enum{ /* sort order must be equal to locales */
EVENTMODE_NEVER = 0,
EVENTMODE_ALWAYS,
EVENTMODE_WINDOWISACTIVE,
EVENTMODE_WINDOWISINACTIVE,
};
/*************************************************************************/

#endif /* MUICLASS_EVENTLIST_H */

