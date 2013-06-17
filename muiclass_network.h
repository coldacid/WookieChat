/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_NETWORK_H
#define MUICLASS_NETWORK_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define NetworkObject NEWOBJECT( appclasses[ CLASSID_NETWORK ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_NETWORK_PLAY =  0xFED00350,
/* Attributes */
MA_NETWORK_OBJECTSETTINGS,
MA_NETWORK_OBJECTAUDIO,
};

struct MP_NETWORK_PLAY    { ULONG MethodID; ULONG SID; char *Name; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_Network_InitClass( void );
void    MCC_Network_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_NETWORK_H */

