/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_chatchannellist.c
*/

#define NODEBUG

#include <libraries/mui.h>
#include <intuition/classusr.h>
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
#include "muiclass_chatchannellist.h"
#include "muiclass_settingscolor.h"
#include "muiclass_windowsettings.h"
#include "muiclass_windowchat.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
WID_SETTINGS = 0,
MID_CONTEXTMENU,
GID_LAST,
/* these need no storage, so defined after GID_LAST */
MID_CMENU_MOVETONEWWINDOW,
MID_CMENU_CLONETONEWWINDOW,
};

/*
** data used by this class
*/

#define DISPLAYBUFFER_SIZEOF 0x400

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	LONG                   mcc_Pen[ PEN_NUMBEROF ];
	LONG                   mcc_MUIPen[ PEN_NUMBEROF ];
	ULONG                  mcc_PenRGB[ PEN_NUMBEROF ];
	char                   mcc_DisplayBuffer[ DISPLAYBUFFER_SIZEOF ];
	struct ChatChannel    *mcc_SelectedContextEntry;

};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	return( (IPTR) DoSuperNew( cl, obj,
						MUIA_ShortHelp  , LGS( MSG_MUICLASS_CHATCHANNELLIST_HELP ),
						MUIA_ContextMenu, 1,
						TAG_DONE ) );
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

	DoMethod( obj, MM_CHATCHANNELLIST_PENSOBTAIN  );

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

	DoMethod( obj, MM_CHATCHANNELLIST_PENSRELEASE );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// MM_ContextMenuBuild()
**
*/

/*************************************************************************/

static ULONG MM_ContextMenuBuild( struct IClass *cl, Object *obj, struct MUIP_ContextMenuBuild *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct MUI_NList_TestPos_Result res;

	if( mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] ) {
		MUI_DisposeObject( mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] );
		mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] = NULL;
	}
	mccdata->mcc_SelectedContextEntry = NULL;
	DoMethod( obj, MUIM_NList_TestPos, msg->mx, msg->my, &res );
	if( res.entry != -1 ) {
		DoMethod( obj, MUIM_NList_GetEntry, res.entry, &mccdata->mcc_SelectedContextEntry );
		if( mccdata->mcc_SelectedContextEntry ) { /* paranoia */

/* now build the tree */
			if( ( mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] = MenustripObject,
							Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_CMENU ),
								Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_MOVETONEWWINDOW_CMENU ), MUIA_UserData, MID_CMENU_MOVETONEWWINDOW, End,
								Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_CLONETONEWWINDOW_CMENU ), MUIA_UserData, MID_CMENU_CLONETONEWWINDOW, End,
							End,
						End ) ) {

			}
		}
	}
	return( (ULONG) mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] );
}
/* \\\ */
/* /// MM_ContextMenuSelect()
*/

/*************************************************************************/

static ULONG MM_ContextMenuSelect( struct IClass *cl, Object *obj, struct  MUIP_ContextMenuChoice *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannel   *cc;
struct Channel       *c;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( cc = mccdata->mcc_SelectedContextEntry ) && ( c = cc->cc_Channel ) ) { /* paranoia */
		if( msg->item ) {
			switch( MUIGetVar( msg->item, MUIA_UserData ) ) {
				case MID_CMENU_MOVETONEWWINDOW:
					{ Object *windowchat;

					if( ( windowchat = (APTR) DoMethod( _app(obj), MM_APPLICATION_WINDOWCHATNEW ) ) ) {
						DoMethod( _win( obj ), MM_WINDOWCHAT_CHANNELREMOVE, c );
						DoMethod( windowchat , MM_WINDOWCHAT_CHANNELADD, c );
					}
					}
					break;
				case MID_CMENU_CLONETONEWWINDOW:
					{ Object *windowchat;

					if( ( windowchat = (APTR) DoMethod( _app(obj), MM_APPLICATION_WINDOWCHATNEW ) ) ) {
						DoMethod( windowchat, MM_WINDOWCHAT_CHANNELADD, c );
					}
					}
					break;
			}
		}
	}
	return( 0 );
}
/* \\\ */

/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannel *cc;

	if( ( cc = msg->entry ) ) {
		sprintf( mccdata->mcc_DisplayBuffer, "\033P[%ld]%s", mccdata->mcc_Pen[ cc->cc_Pen ], cc->cc_Channel->c_Name );
		*msg->strings = (STRPTR) mccdata->mcc_DisplayBuffer;
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
struct ChatChannel *cc;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( cc = AllocPooled( msg->pool, sizeof( struct ChatChannel ) ) ) ) {
		cc->cc_Pen     = PEN_CHANNELLISTTEXT;
		cc->cc_Channel = msg->entry;
    }
	return( (IPTR) cc );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Destruct( struct IClass *cl, Object *obj, struct MUIP_NList_Destruct *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( msg->entry ) {
		FreePooled( msg->pool, msg->entry, sizeof( struct ChatChannel ) );
    }
	return( 0 );
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

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; i < PEN_NUMBEROF ; i++ ) {
		if( ( penspec = (APTR) LRC( OID_SETTINGSCOLOR + i ) ) ) {
			mccdata->mcc_MUIPen[ i ] = MUI_ObtainPen( muiRenderInfo( obj ), penspec, 0 );
			mccdata->mcc_Pen[ i ]    = MUIPEN( mccdata->mcc_MUIPen[ i ] );
			mccdata->mcc_PenRGB[ i ] = MUIPenSpecToRGB( obj, penspec );
		}
	}
	#if ENABLE_NLIST /* NList is not using the standard background tag */
		SetAttrs( obj, MUIA_NList_ListBackground, LRC( OID_SETTINGSCOLOR + PEN_CHANNELLISTBACKGROUND ), TAG_DONE );
	#else
		SetAttrs( obj, MUIA_Background, LRC( OID_SETTINGSCOLOR + PEN_CHANNELLISTBACKGROUND ), TAG_DONE );
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

	DoMethod( obj, MM_CHATCHANNELLIST_PENSOBTAIN  );
	DoMethod( obj, MM_CHATCHANNELLIST_PENSRELEASE );

	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_ChatChannelList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ChatChannelList_Dispatcher)
{
	switch (msg->MethodID) {
		case OM_NEW                         : return( OM_New                ( cl, obj, (APTR) msg ) );
		case MUIM_Setup                     : return( OM_Setup              ( cl, obj, (APTR) msg ) );
		case MUIM_Cleanup                   : return( OM_Cleanup            ( cl, obj, (APTR) msg ) );
		case MUIM_ContextMenuChoice         : return( MM_ContextMenuSelect  ( cl, obj, (APTR) msg ) );
		case MUIM_ContextMenuBuild          : return( MM_ContextMenuBuild   ( cl, obj, (APTR) msg ) );

		case MUIM_NList_Display             : return( OM_Display            ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct            : return( OM_Destruct           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Construct           : return( OM_Construct          ( cl, obj, (APTR) msg ) );
		case MM_CHATCHANNELLIST_PENSOBTAIN  : return( MM_PensObtain         ( cl, obj, (APTR) msg ) );
		case MM_CHATCHANNELLIST_PENSRELEASE : return( MM_PensRelease        ( cl, obj, (APTR) msg ) );
		case MM_CHATCHANNELLIST_PENSUPDATE  : return( MM_PensUpdate         ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ChatChannelList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ChatChannelList_InitClass( void )
{
	appclasses[ CLASSID_CHATCHANNELLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, sizeof( struct mccdata ), (APTR) ENTRY(MCC_ChatChannelList_Dispatcher) );
	return( appclasses[ CLASSID_CHATCHANNELLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ChatChannelList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ChatChannelList_DisposeClass( void )
{
	if( appclasses[ CLASSID_CHATCHANNELLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_CHATCHANNELLIST ] );
		appclasses[ CLASSID_CHATCHANNELLIST ] = NULL;
    }
}
/* \\\ */

