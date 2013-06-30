/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_chatlog.c
*/

//#define NODEBUG

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingscolor.h"
#include "muiclass_chatlog.h"
#include "muiclass_network.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
WID_SETTINGS = 0,
GID_LAST
};

/*
** data used by this class
*/

#define DISPLAYBUFFER_SIZEOF 0x400

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	LONG                   mcc_MUIPen[ PEN_NUMBEROF ];
	LONG                   mcc_Pen[ PEN_NUMBEROF ];
	ULONG                  mcc_PenRGB[ PEN_NUMBEROF ];
	char                   mcc_DisplayBuffer[ DISPLAYBUFFER_SIZEOF ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *)DoSuperNew( cl, obj,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// OM_Setup()
**
*/

/*************************************************************************/

static ULONG OM_Setup( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (Object*) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWSETTINGS );

	DoMethod( obj, MM_CHATLOG_PENSOBTAIN  );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// OM_Cleanup()
**
*/

/*************************************************************************/

static ULONG OM_Cleanup( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MM_CHATLOG_PENSRELEASE );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatLogEntry *cle;

	if( ( cle = msg->entry ) ) {
		sprintf( mccdata->mcc_DisplayBuffer, "\033P[%ld]%s", mccdata->mcc_Pen[ cle->cle_Pen ], cle->cle_Message );
		*msg->strings = (STRPTR) mccdata->mcc_DisplayBuffer;
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ShowLastLine()
**
*/

/*************************************************************************/

static ULONG MM_ShowLastLine( struct IClass *cl, Object *obj, struct MP_CHATLOG_SHOWLASTLINE *msg )
{
IPTR visible, top, total;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	/* this magic code checks if the last line was shown before adding our entry, if
	** yes we move it down to make the new line visible, if not, we leave the list
	** position as the user may wants to read something up there
	**
	** The second to last line cannot be fixuated, but that shouldn t be a problem
	*/

	visible = MUIGetVar( obj, MUIA_NList_Visible );
	top     = MUIGetVar( obj, MUIA_NList_First   );
	total   = MUIGetVar( obj, MUIA_NList_Entries );

	if( msg->Force || ( ( top + visible ) >= total - 1 ) ) {
		SetAttrs( obj, MUIA_NList_First, total - visible, TAG_DONE );
	}
	return( 0 );
}
/* /// MM_PensObtain()
**
*/

/*************************************************************************/

static ULONG MM_PensObtain( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct MUI_PenSpec *penspec;
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; i < PEN_NUMBEROF ; i++ ) {
		if( ( penspec = (APTR) LRC( OID_SETTINGSCOLOR + i ) ) ) {
			mccdata->mcc_MUIPen[ i ] = MUI_ObtainPen( muiRenderInfo( obj ), penspec, 0 );
			mccdata->mcc_Pen[ i ]    = MUIPEN( mccdata->mcc_MUIPen[ i ] );
			mccdata->mcc_PenRGB[ i ] = MUIPenSpecToRGB( obj, penspec );
		}
	}
	#if ENABLE_NLIST /* NList is not using the standard background tag */
		SetAttrs( obj, MUIA_NList_ListBackground, LRC( OID_SETTINGSCOLOR + PEN_LOGLISTBACKGROUND ), TAG_DONE );
	#else
		SetAttrs( obj, MUIA_Background, LRC( OID_SETTINGSCOLOR + PEN_LOGLISTBACKGROUND ), TAG_DONE );
	#endif

	return( 0 );
}
/* \\\ */
/* /// MM_PensRelease()
**
*/

/*************************************************************************/

static ULONG MM_PensRelease( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; i < PEN_NUMBEROF ; i++ ) {
		MUI_ReleasePen( muiRenderInfo( obj ), mccdata->mcc_MUIPen[ i ] );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_PensUpdate()
**
*/

/*************************************************************************/

static ULONG MM_PensUpdate( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MM_CHATLOG_PENSOBTAIN  );
	DoMethod( obj, MM_CHATLOG_PENSRELEASE );

	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_ChatLog_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ChatLog_Dispatcher)
{
	switch ( msg->MethodID ) {
		case OM_NEW                          : return( OM_New          ( cl, obj, (APTR) msg ) );
		case MUIM_Setup                      : return( OM_Setup        ( cl, obj, (APTR) msg ) );
		case MUIM_Cleanup                    : return( OM_Cleanup      ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display      ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_SHOWLASTLINE         : return( MM_ShowLastLine ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_PENSOBTAIN           : return( MM_PensObtain   ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_PENSRELEASE          : return( MM_PensRelease  ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_PENSUPDATE           : return( MM_PensUpdate   ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ChatLog_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ChatLog_InitClass( void )
{
	appclasses[ CLASSID_CHATLOG ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, sizeof( struct mccdata ), (APTR) ENTRY(MCC_ChatLog_Dispatcher) );
	return( appclasses[ CLASSID_CHATLOG ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ChatLog_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ChatLog_DisposeClass( void )
{
	if( appclasses[ CLASSID_CHATLOG ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_CHATLOG ] );
		appclasses[ CLASSID_CHATLOG ] = NULL;
    }
}
/* \\\ */

