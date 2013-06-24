/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_chatuserlist.c
*/

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
#include "muiclass_network.h"
#include "muiclass_chatuserlist.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingscolor.h"

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

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	LONG                   mcc_Pen[PEN_NUMBEROF];
	ULONG                  mcc_PenRGB[PEN_NUMBEROF];
};


/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

	debug( "%s (%ld) %s - Class: 0x00004530x Object: 0x00004530x \n", __FILE__, __LINE__, __func__, cl, obj );

	return( (IPTR) DoSuperNew( cl, obj, TAG_DONE ) );
}
/* \\\ */
/* /// OM_Setup()
**
*/

/*************************************************************************/

static ULONG OM_Setup( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s - Class: 0x00007935x Object: 0x00007935x \n", __FILE__, __LINE__, __func__, cl, obj );

	mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (Object*) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWSETTINGS );

	DoMethod( obj, MM_CHATUSERLIST_PENSOBTAIN  );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// OM_Cleanup()
**
*/

/*************************************************************************/

static ULONG OM_Cleanup( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s - Class: 0x00007935x Object: 0x00007935x \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MM_CHATUSERLIST_PENSRELEASE );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
	if( ( msg->entry ) ) {
		*msg->strings = (STRPTR) ((struct ChatNick *) msg->entry)->cn_ChatNickEntry->cne_Nick;
	}
	return( 0 );
}
/* \\\ */
/* /// OM_Construct()
**
*/

/*************************************************************************/

static ULONG OM_Construct( struct IClass *cl, Object *obj, struct MUIP_NList_Construct *msg )
{
struct ChatNick *cn;

	debug( "%s (%ld) %s - Class: 0x00004530x Object: 0x00004530x \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( cn = AllocPooled( msg->pool, sizeof( struct ChatNick ) ) ) ) {
		cn->cn_ChatNickEntry = msg->entry;
    }
	return( (IPTR) cn );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Destruct( struct IClass *cl, Object *obj, struct MUIP_NList_Destruct *msg )
{

	debug( "%s (%ld) %s - Class: 0x00004530x Object: 0x00004530x \n", __FILE__, __LINE__, __func__, cl, obj );

	if( msg->entry ) {
		FreePooled( msg->pool, msg->entry, sizeof( struct ChatNick ) );
    }
	return( 0 );
}
/* \\\ */
/* /// OM_Compare()
**
*/

/*************************************************************************/

static ULONG OM_Compare( struct IClass *cl, Object *obj, struct MUIP_NList_Compare *msg )
{
STRPTR nick1 = (STRPTR) ((struct ChatNick *) msg->entry1)->cn_ChatNickEntry->cne_Nick;
STRPTR nick2 = (STRPTR) ((struct ChatNick *) msg->entry2)->cn_ChatNickEntry->cne_Nick;

	debug( "%s (%ld) %s - Class: 0x00004530x Object: 0x00004530x \n", __FILE__, __LINE__, __func__, cl, obj );

	return( Stricmp( (CONST_STRPTR) nick1, (CONST_STRPTR) nick2 ) );
}
/* \\\ */

/* /// MM_PensObtain()
**
*/

/*************************************************************************/

static ULONG MM_PensObtain( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct MUI_PenSpec *penspec;
ULONG i;

	debug( "%s (%ld) %s - Class: 0x00007935x Object: 0x00007935x \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; i < PEN_NUMBEROF ; i++ ) {
		if( ( penspec = (APTR) LRC( OID_SETTINGSCOLOR + i ) ) ) {
			mccdata->mcc_Pen[ i ]    = MUI_ObtainPen( muiRenderInfo( obj ), penspec, 0 );
			mccdata->mcc_PenRGB[ i ] = MUIPenSpecToRGB( obj, penspec );
		}
	}
	#if ENABLE_NLIST /* NList is not using the standard background tag */
		SetAttrs( obj, MUIA_NList_ListBackground, LRC( OID_SETTINGSCOLOR + PEN_USERLISTBACKGROUND ), TAG_DONE );
	#else
		SetAttrs( obj, MUIA_Background, LRC( OID_SETTINGSCOLOR + PEN_USERLISTBACKGROUND ), TAG_DONE );
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

	debug( "%s (%ld) %s - Class: 0x00007935x Object: 0x00007935x \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; i < PEN_NUMBEROF ; i++ ) {
		MUI_ReleasePen( muiRenderInfo( obj ), mccdata->mcc_Pen[ i ] );
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

	debug( "%s (%ld) %s - Class: 0x00007935x Object: 0x00007935x \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MM_CHATUSERLIST_PENSOBTAIN  );
	DoMethod( obj, MM_CHATUSERLIST_PENSRELEASE );

	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_ChatUserList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ChatUserList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                      : return( OM_New       ( cl, obj, (APTR) msg ) );
		case MUIM_Setup                  : return( OM_Setup      ( cl, obj, (APTR) msg ) );
		case MUIM_Cleanup                : return( OM_Cleanup    ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display          : return( OM_Display   ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct         : return( OM_Destruct  ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Construct        : return( OM_Construct ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Compare          : return( OM_Compare   ( cl, obj, (APTR) msg ) );
		case MM_CHATUSERLIST_PENSOBTAIN  : return( MM_PensObtain ( cl, obj, (APTR) msg ) );
		case MM_CHATUSERLIST_PENSRELEASE : return( MM_PensRelease( cl, obj, (APTR) msg ) );
		case MM_CHATUSERLIST_PENSUPDATE  : return( MM_PensUpdate ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ChatUserList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ChatUserList_InitClass( void )
{
	appclasses[ CLASSID_CHATUSERLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, sizeof( struct mccdata ),  (APTR) ENTRY(MCC_ChatUserList_Dispatcher) );
	return( appclasses[ CLASSID_CHATUSERLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ChatUserList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ChatUserList_DisposeClass( void )
{
	if( appclasses[ CLASSID_CHATUSERLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_CHATUSERLIST ] );
		appclasses[ CLASSID_CHATUSERLIST ] = NULL;
    }
}
/* \\\ */

