/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_CHANNEL_H
#define MUICLASS_CHANNEL_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define ChannelObject NEWOBJECT( appclasses[ CLASSID_CHANNEL ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_CHANNEL_DUMMY =  0xFED00300,
/* Attributes */
};

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_Channel_InitClass( void );
void    MCC_Channel_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_CHANNEL_H */

