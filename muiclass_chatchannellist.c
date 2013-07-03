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
#include <libraries/gadtools.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "functions.h"
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
MID_CMENU_SERVERSHOW,
MID_CMENU_SERVERHIDE,
MID_CMENU_CHANNELREMOVE,
MID_CMENU_CHANNELLEAVE,
};

/*
** data used by this class
*/

#define DISPLAYBUFFER_SIZEOF 0x400

struct mccdata
{
	Object                  *mcc_ClassObjects[ GID_LAST ];
	LONG                     mcc_Pen[ PEN_NUMBEROF ];
	LONG                     mcc_MUIPen[ PEN_NUMBEROF ];
	ULONG                    mcc_PenRGB[ PEN_NUMBEROF ];
	char                     mcc_DisplayBuffer[ DISPLAYBUFFER_SIZEOF ];
	struct ChatChannelEntry *mcc_SelectedContextEntry;

};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( obj = DoSuperNew( cl, obj,
						MUIA_ShortHelp  , LGS( MSG_MUICLASS_CHATCHANNELLIST_HELP ),
						MUIA_ContextMenu, 1,
						TAG_DONE ) ) ) {
		DoMethod( obj, MUIM_Notify, MUIA_NList_Entries, 0, obj, 1, MM_CHATCHANNELLIST_WINDOWCLOSE );
	}
	return( (IPTR) obj );
}
/* \\\ */
/* /// OM_Setup()
**
*/

/*************************************************************************/

static IPTR OM_Setup( struct IClass *cl, Object *obj, Msg *msg )
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

static IPTR OM_Cleanup( struct IClass *cl, Object *obj, Msg *msg )
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

static IPTR MM_ContextMenuBuild( struct IClass *cl, Object *obj, struct MUIP_ContextMenuBuild *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct MUI_NList_TestPos_Result res;
struct ChatChannelEntry *cce;
struct Channel *c;

	if( mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] ) {
		MUI_DisposeObject( mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] );
		mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] = NULL;
	}
	mccdata->mcc_SelectedContextEntry = NULL;
	DoMethod( obj, MUIM_NList_TestPos, msg->mx, msg->my, &res );
	if( res.entry != -1 ) {
		DoMethod( obj, MUIM_NList_GetEntry, res.entry, &mccdata->mcc_SelectedContextEntry );
		if( ( cce = mccdata->mcc_SelectedContextEntry ) ) { /* paranoia */
			if( ( c = cce->cce_Channel ) ) { /* paranoia */
				IPTR servervisible = DoMethod(      obj , MM_CHATCHANNELLIST_SERVERISVISIBLE, c );
				IPTR channellast   = DoMethod( _app(obj), MM_APPLICATION_CHANNELISVISIBLE, c, _win(obj) );
				IPTR isserver      = c->c_Flags & CHANNELF_SERVER;
/* now build the tree */

				if( ( mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] = MenustripObject,
								Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_CMENU ),
									(( channellast || isserver ) ? TAG_IGNORE :	Child ), (( channellast || isserver ) ? TAG_IGNORE : (IPTR)	MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_CHANNELLEAVE_CMENU     ), MUIA_UserData, MID_CMENU_CHANNELLEAVE , End ),
									((!channellast || isserver ) ? TAG_IGNORE :	Child ), ((!channellast || isserver ) ? TAG_IGNORE : (IPTR)	MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_CHANNELREMOVE_CMENU    ), MUIA_UserData, MID_CMENU_CHANNELREMOVE, End ),
									((                isserver ) ? TAG_IGNORE : Child ), ((                isserver ) ? TAG_IGNORE : (IPTR)	MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0) ),
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_MOVETONEWWINDOW_CMENU  ), MUIA_UserData, MID_CMENU_MOVETONEWWINDOW, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_CLONETONEWWINDOW_CMENU ), MUIA_UserData, MID_CMENU_CLONETONEWWINDOW, End,
									Child, MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0),
									( servervisible ? TAG_IGNORE : Child ), ( servervisible ? TAG_IGNORE : (IPTR) MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_SERVERSHOW_CMENU       ), MUIA_UserData, MID_CMENU_SERVERSHOW   , End ),
									(!servervisible ? TAG_IGNORE : Child ), (!servervisible ? TAG_IGNORE : (IPTR) MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATCHANNELLIST_SERVERHIDE_CMENU       ), MUIA_UserData, MID_CMENU_SERVERHIDE   , End ),
								End,
							End ) ) {
				}
			}
		}
	}
	return( (IPTR) mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] );
}
/* \\\ */
/* /// MM_ContextMenuSelect()
*/

/*************************************************************************/

static IPTR MM_ContextMenuSelect( struct IClass *cl, Object *obj, struct  MUIP_ContextMenuChoice *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannelEntry *cce;
struct Channel          *c;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( cce = mccdata->mcc_SelectedContextEntry ) && ( c = cce->cce_Channel ) ) { /* paranoia */
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

static IPTR OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannelEntry *cce;

	if( ( cce = msg->entry ) ) {
		sprintf( mccdata->mcc_DisplayBuffer, "\033P[%ld]%s", mccdata->mcc_Pen[ cce->cce_Pen ], cce->cce_Channel->c_Name );
		*msg->strings = (STRPTR) mccdata->mcc_DisplayBuffer;
	}
	return( 0 );
}
/* \\\ */
/* /// OM_Construct()
**
*/

/*************************************************************************/

static IPTR OM_Construct( struct IClass *cl, Object *obj, struct MUIP_NList_Construct *msg )
{
struct ChatChannelEntry *cce;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( cce = AllocVec( sizeof( struct ChatChannelEntry ), MEMF_ANY ) ) ) {
		cce->cce_Pen     = PEN_CHANNELLISTTEXT;
		cce->cce_Channel = msg->entry;
    }
	return( (IPTR) cce );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static IPTR OM_Destruct( struct IClass *cl, Object *obj, struct MUIP_NList_Destruct *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( msg->entry ) {
		FreeVec( msg->entry );
    }
	return( 0 );
}
/* \\\ */

/* /// MM_PensObtain()
**
*/

/*************************************************************************/

static IPTR MM_PensObtain( struct IClass *cl, Object *obj, Msg *msg )
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

static IPTR MM_PensRelease( struct IClass *cl, Object *obj, Msg *msg )
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

static IPTR MM_PensUpdate( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MM_CHATCHANNELLIST_PENSOBTAIN  );
	DoMethod( obj, MM_CHATCHANNELLIST_PENSRELEASE );

	return( 0 );
}
/* \\\ */

/* /// MM_WindowClose()
**
*/

/*************************************************************************/

static IPTR MM_WindowClose( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	/* this is special. The window object gets disposed. Calling the method directly
	** would dispose the object from within a notification of the very same object,
	** which is quite bad.
	*/
	DoMethod( _app(obj), MUIM_Application_PushMethod, _app(obj), 2, MM_APPLICATION_WINDOWDISPOSE, _win(obj) );

	return( 0 );
}
/* \\\ */

/* /// MM_ServerIsVisible()
**
*/

/*************************************************************************/

static IPTR MM_ServerIsVisible( struct IClass *cl, Object *obj, struct MP_CHATCHANNELLIST_SERVERISVISIBLE *msg )
{
struct ChatChannelEntry *cce;
struct Channel *c;
struct Server  *s;
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	/* pointer magic */
	s = (APTR) ( ( (IPTR) List_GetListFromNode( msg->Channel ) ) - (IPTR) offsetof( struct Server, s_ChannelList ) );
	/* find server channel by flag */
	for( c = (APTR) s->s_ChannelList.lh_Head ; c->c_Succ ; c = c->c_Succ ) {
		if( c->c_Flags & CHANNELF_SERVER ) {
			break;
		}
	}
	if( c->c_Succ ) { /* did we find the server channel? */
		/* now see if the channel we found is in visible list */
		for( i = 0 ;  ; i++ ) {
			cce = NULL;
			DoMethod( obj, MUIM_NList_GetEntry, i, &cce );
			if( cce ) {
				if( ( c == cce->cce_Channel ) ) {
					return( (IPTR) c );
				}
			} else {
				break;
			}
		}
	} else {
		debug("####### ERROR: There is no server channel for server '%s'! ########\n", s->s_Name );
	}
	return( (IPTR) NULL );
}
/* \\\ */
/* /// MM_ChannelIsVisible()
**
*/

/*************************************************************************/

static IPTR MM_ChannelIsVisible( struct IClass *cl, Object *obj, struct MP_CHATCHANNELLIST_CHANNELISVISIBLE *msg )
{
struct ChatChannelEntry *cce;
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	/* now see if the channel we found is in visible list */
	for( i = 0 ;  ; i++ ) {
		cce = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &cce );
		if( cce ) {
			if( ( msg->Channel == cce->cce_Channel ) ) {
				return( (IPTR) msg->Channel );
			}
		} else {
			break;
		}
	}

	return( (IPTR) NULL );
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
		case OM_NEW                             : return( OM_New                ( cl, obj, (APTR) msg ) );
		case MUIM_Setup                         : return( OM_Setup              ( cl, obj, (APTR) msg ) );
		case MUIM_Cleanup                       : return( OM_Cleanup            ( cl, obj, (APTR) msg ) );
		case MUIM_ContextMenuChoice             : return( MM_ContextMenuSelect  ( cl, obj, (APTR) msg ) );
		case MUIM_ContextMenuBuild              : return( MM_ContextMenuBuild   ( cl, obj, (APTR) msg ) );

		case MUIM_NList_Display                 : return( OM_Display            ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct                : return( OM_Destruct           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Construct               : return( OM_Construct          ( cl, obj, (APTR) msg ) );
		case MM_CHATCHANNELLIST_PENSOBTAIN      : return( MM_PensObtain         ( cl, obj, (APTR) msg ) );
		case MM_CHATCHANNELLIST_PENSRELEASE     : return( MM_PensRelease        ( cl, obj, (APTR) msg ) );
		case MM_CHATCHANNELLIST_PENSUPDATE      : return( MM_PensUpdate         ( cl, obj, (APTR) msg ) );

		case MM_CHATCHANNELLIST_WINDOWCLOSE     : return( MM_WindowClose        ( cl, obj, (APTR) msg ) );
		case MM_CHATCHANNELLIST_SERVERISVISIBLE : return( MM_ServerIsVisible    ( cl, obj, (APTR) msg ) );
		case MM_CHATCHANNELLIST_CHANNELISVISIBLE: return( MM_ChannelIsVisible   ( cl, obj, (APTR) msg ) );

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

