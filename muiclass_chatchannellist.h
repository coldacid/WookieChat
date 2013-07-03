/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_CHATCHANNELLIST_H
#define MUICLASS_CHATCHANNELLIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"
#include "muiclass_network.h"


#define ChatChannelListObject NEWOBJECT( appclasses[ CLASSID_CHATCHANNELLIST ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_CHATCHANNELLIST_DUMMY =  0xFED00600,
MM_CHATCHANNELLIST_WINDOWCLOSE,
MM_CHATCHANNELLIST_PENSUPDATE,
MM_CHATCHANNELLIST_PENSOBTAIN,
MM_CHATCHANNELLIST_PENSRELEASE,
MM_CHATCHANNELLIST_ISSERVERVISIBLE,
/* Attributes */
};

struct MP_CHATCHANNELLIST_ISSERVERVISIBLE { ULONG MethodID; struct Channel *Channel; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_ChatChannelList_InitClass( void );
void    MCC_ChatChannelList_DisposeClass( void );

/*************************************************************************/

/*
** ChatChannel structure
*/

struct ChatChannelEntry {
	ULONG                  cce_Pen;
	struct Channel        *cce_Channel;
};

/*************************************************************************/

#endif /* MUICLASS_CHATCHANNELLIST_H */

