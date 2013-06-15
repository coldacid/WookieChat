/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_CHANNELLIST_H
#define MUICLASS_CHANNELLIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define ChannelListObject NEWOBJECT( appclasses[ CLASSID_CHANNELLIST ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_CHANNELLIST_DUMMY =  0xFED00310,
/* Attributes */
};

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_ChannelList_InitClass( void );
void    MCC_ChannelList_DisposeClass( void );

/*
** channel entry structure
*/

#define CHANNELENTRY_CHANNEL_SIZEOF     30
#define CHANNELENTRY_PASSWORD_SIZEOF 30

struct ChannelEntry {
	struct ChannelEntry    *ce_Succ;
	struct ChannelEntry    *ce_Pred;
	char                    ce_Channel[ CHANNELENTRY_CHANNEL_SIZEOF + 2 ];
	char                    ce_Password[ CHANNELENTRY_PASSWORD_SIZEOF + 2 ];
};

/*************************************************************************/

#endif /* MUICLASS_CHANNELLIST_H */

