/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_network.c
*/

#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "functions.h"
#include "intern.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowsettings.h"
#include "muiclass_network.h"

/*************************************************************************/

enum{
WID_SETTINGS = 0,
GID_AUDIO,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
};

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg )
{
	if( ( obj = (Object*) DoSuperNew( cl, obj, TAG_DONE ) ) ) {
		SetAttrs( obj, TAG_MORE, msg->ops_AttrList );
	}
	return( (ULON) obj );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg *msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// OM_Set()
**
*/

/*************************************************************************/

static ULONG OM_Set( struct IClass *cl, Object *obj, struct opSet *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct TagItem *tag;
struct TagItem *tstate;

	for( tstate = msg->ops_AttrList ; ( tag = NextTagItem( &tstate ) ) ; ) {
		ULONG tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
			case MA_NETWORK_OBJECTSETTINGS:
				mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (APTR) tidata;
				break;
			case MA_NETWORK_OBJECTAUDIO:
				mccdata->mcc_ClassObjects[ GID_AUDIO    ] = (APTR) tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// MM_Play()
**
*/

/*************************************************************************/

static ULONG MM_Play( struct IClass *cl, Object *obj, struct MP_NETWORK_PLAY *msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_Network_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_Network_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                        : return( OM_New                      ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                    : return( OM_Dispose                  ( cl, obj, (APTR) msg ) );
		case OM_SET                        : return( OM_Set                      ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_PLAY                 : return( MM_Play                     ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_Network_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_Network_InitClass( void )
{
	appclasses[ CLASSID_NETWORK ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_Area, NULL, sizeof( struct mccdata ), (APTR) ENTRY(MCC_Network_Dispatcher) );
	return( appclasses[ CLASSID_NETWORK ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_Network_DisposeClass()
**
*/

/*************************************************************************/

void MCC_Network_DisposeClass( void )
{
	if( appclasses[ CLASSID_NETWORK ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_NETWORK ] );
		appclasses[ CLASSID_NETWORK ] = NULL;
    }
}
/* \\\ */




