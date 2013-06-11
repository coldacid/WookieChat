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
#include <intuition/intuition.h>

/*************************************************************************/

/*
** Class ID Enumeration
*/

enum
{
CLASSID_APPLICATION = 0,
CLASSID_WINDOWMAIN,
CLASSID_WINDOWABOUT,
CLASSID_WINDOWQUIT,
CLASSID_WINDOWURLGRABBER,
CLASSID_WINDOWIGNORELIST,
CLASSID_WINDOWCOLORSETTINGS,
CLASSID_NICKLIST,
CLASSID_CHANNELLIST,
CLASSID_CHANNEL,
CLASSID_LAST
};

extern struct MUI_CustomClass *appclasses[];
extern Object *application;

/*
** Prototypes
*/

ULONG MUIClass_Open ( void );
void  MUIClass_Close( void );

ULONG MUIGetVar( Object *obj, ULONG attr );

TEXT MUIGetUnderScore( ULONG text);
APTR MUICreatePoppen( ULONG text, ULONG poptitle );
APTR MUICreateButton( ULONG text );
APTR MUICreateSmallButton( ULONG text );
APTR MUICreateLabel( ULONG text );
APTR MUICreateCheckbox( ULONG text, ULONG defstate );
APTR MUICreateString( ULONG text, ULONG maxchars );

/*************************************************************************/

#endif /* MUICLASS_H */

