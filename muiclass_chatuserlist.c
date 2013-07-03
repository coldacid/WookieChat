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

#define NODEBUG

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
#include "functions.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_network.h"
#include "muiclass_chatuserlist.h"
#include "muiclass_chatchannellist.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingscolor.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
MID_CONTEXTMENU = 0,
WID_SETTINGS,
GID_CHATCHANNELLIST,
GID_NETWORK,
GID_LAST,
/* these need no storage, so defined after GID_LAST */
MID_CMENU_WHOIS,
MID_CMENU_OPENQUERY,
MID_CMENU_OPENGLOBALQUERY,
MID_CMENU_CTCPPING,
MID_CMENU_CTCPVERSION,
MID_CMENU_CTCPTIME,
MID_CMENU_CONTROLOP,
MID_CMENU_CONTROLDEOP,
MID_CMENU_CONTROLHALFOP,
MID_CMENU_CONTROLDEHALFOP,
MID_CMENU_CONTROLVOICE,
MID_CMENU_CONTROLDEVOICE,
MID_CMENU_CONTROLKICK,
MID_CMENU_CONTROLBAN,
};

/*
** data used by this class
*/

#define DISPLAYBUFFER_SIZEOF 0x100
#define IMAGEBUFFER_SIZEOF   0x100
#define COMPOSEBUFFER_SIZEOF 0x100
struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	LONG                   mcc_MUIPen[ PEN_NUMBEROF ];
	LONG                   mcc_Pen[ PEN_NUMBEROF ];
	ULONG                  mcc_PenRGB[ PEN_NUMBEROF ];
	char                   mcc_ImageBuffer[ IMAGEBUFFER_SIZEOF ];
	char                   mcc_DisplayBuffer[ DISPLAYBUFFER_SIZEOF ];
	struct ChatNick       *mcc_SelectedContextEntry;
	char                   mcc_ComposeBuffer[ COMPOSEBUFFER_SIZEOF ];
};


/* /// OM_New()
**
*/

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	return( (IPTR) DoSuperNew( cl, obj, MUIA_NList_Format, ",",
				MUIA_ShortHelp    , LGS( MSG_MUICLASS_CHATUSERLIST_HELP ),
				MUIA_ContextMenu  , 1,
				TAG_DONE ) );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] ) {
		MUI_DisposeObject( mccdata->mcc_ClassObjects[ MID_CONTEXTMENU ] );
	}

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// OM_Setup()
**
*/

/*************************************************************************/

static ULONG OM_Setup( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

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

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MM_CHATUSERLIST_PENSRELEASE );

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

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( tstate = msg->ops_AttrList ; ( tag = NextTagItem( &tstate ) ) ; ) {
		ULONG tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
			case MA_CHATUSERLIST_OBJECTCHATCHANNELLIST:
				mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ] = (APTR) tidata;
				break;
			case MA_CHATUSERLIST_OBJECTNETWORK:
				mccdata->mcc_ClassObjects[ GID_NETWORK ] = (APTR) tidata;
				break;
			case MA_CHATUSERLIST_OBJECTSETTINGS:
				mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (APTR) tidata;
				break;
		}
    }
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
							Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CMENU ),
								Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_WHOIS_CMENU ), MUIA_UserData, MID_CMENU_WHOIS, End,
								Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_OPENQUERY_CMENU ), MUIA_UserData, MID_CMENU_OPENQUERY, End,
								Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_OPENGLOBALQUERY_CMENU ), MUIA_UserData, MID_CMENU_OPENGLOBALQUERY, End,
								Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CTCP_CMENU ),
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CTCPPING_CMENU ), MUIA_UserData, MID_CMENU_CTCPPING, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CTCPVERSION_CMENU ), MUIA_UserData, MID_CMENU_CTCPVERSION, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CTCPTIME_CMENU ), MUIA_UserData, MID_CMENU_CTCPTIME, End,
								End,
								Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CONTROL_CMENU ),
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CONTROLOP_CMENU ), MUIA_UserData, MID_CMENU_CONTROLOP, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CONTROLDEOP_CMENU ), MUIA_UserData, MID_CMENU_CONTROLDEOP, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CONTROLHALFOP_CMENU ), MUIA_UserData, MID_CMENU_CONTROLHALFOP, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CONTROLDEHALFOP_CMENU ), MUIA_UserData, MID_CMENU_CONTROLDEHALFOP, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CONTROLVOICE_CMENU ), MUIA_UserData, MID_CMENU_CONTROLVOICE, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CONTROLDEVOICE_CMENU ), MUIA_UserData, MID_CMENU_CONTROLDEVOICE, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CONTROLKICK_CMENU ), MUIA_UserData, MID_CMENU_CONTROLKICK, End,
									Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_CHATUSERLIST_CONTROLBAN_CMENU ), MUIA_UserData, MID_CMENU_CONTROLBAN, End,
								End,
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
struct ChatChannelEntry *cce;
struct ChatNick         *cn;
struct ChatNickEntry    *cne;
struct Channel          *c;
struct Server           *s;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cce );

	if( cce && ( c = cce->cce_Channel ) &&  ( cn = mccdata->mcc_SelectedContextEntry ) && (cne = cn->cn_ChatNickEntry) ) { /* paranoia */
		/* pointer magic */
		s = (APTR) ( ( (IPTR) List_GetListFromNode( c ) ) - (IPTR) offsetof( struct Server, s_ChannelList ) );

		mccdata->mcc_ComposeBuffer[0] = '\0'; /* send detection marker */
		if( msg->item ) {
			switch( MUIGetVar( msg->item, MUIA_UserData ) ) {
				case MID_CMENU_WHOIS:
					sprintf( mccdata->mcc_ComposeBuffer, "/WHOIS %s", cne->cne_Nick );
					break;
				case MID_CMENU_CTCPPING:
					sprintf( mccdata->mcc_ComposeBuffer, "/PRIVMSG %s :\001PING\001", cne->cne_Nick );
					break;
				case MID_CMENU_CTCPVERSION:
					sprintf( mccdata->mcc_ComposeBuffer, "/PRIVMSG %s :\001VERSION\001", cne->cne_Nick );
					break;
				case MID_CMENU_CTCPTIME:
					sprintf( mccdata->mcc_ComposeBuffer, "/PRIVMSG %s :\001TIME\001", cne->cne_Nick );
					break;
				case MID_CMENU_CONTROLOP:
					sprintf( mccdata->mcc_ComposeBuffer, "/MODE %s +o %s", c->c_Name, cne->cne_Nick );
					break;
				case MID_CMENU_CONTROLDEOP:
					sprintf( mccdata->mcc_ComposeBuffer, "/MODE %s -o %s", c->c_Name, cne->cne_Nick );
					break;
				case MID_CMENU_CONTROLHALFOP:
					sprintf( mccdata->mcc_ComposeBuffer, "/MODE %s +h %s", c->c_Name, cne->cne_Nick );
					break;
				case MID_CMENU_CONTROLDEHALFOP:
					sprintf( mccdata->mcc_ComposeBuffer, "/MODE %s -h %s", c->c_Name, cne->cne_Nick );
					break;
				case MID_CMENU_CONTROLVOICE:
					sprintf( mccdata->mcc_ComposeBuffer, "/MODE %s +v %s", c->c_Name, cne->cne_Nick );
					break;
				case MID_CMENU_CONTROLDEVOICE:
					sprintf( mccdata->mcc_ComposeBuffer, "/MODE %s -v %s", c->c_Name, cne->cne_Nick );
					break;
				case MID_CMENU_CONTROLBAN:
					sprintf( mccdata->mcc_ComposeBuffer, "/MODE %s +b %s", c->c_Name, cne->cne_Nick );
					break;
				case MID_CMENU_CONTROLKICK:
					sprintf( mccdata->mcc_ComposeBuffer, "/KICK %s %s :%s", c->c_Name, cne->cne_Nick, LRC( OID_GEN_MSGKICK ) );
					break;
			}
		}
		/* send command if needed */
		if( mccdata->mcc_ComposeBuffer[0] ) {
			DoMethod( mccdata->mcc_ClassObjects[ GID_NETWORK ], MM_NETWORK_SERVERMESSAGESENDMSG, s, NULL, mccdata->mcc_ComposeBuffer );
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
char *nick   =  ((struct ChatNick *) msg->entry)->cn_ChatNickEntry->cne_Nick;
char *status = NULL;
char statusstr[2];

	statusstr[1] = '\0';
	switch( ( statusstr[0] = *nick++ ) ) {
		case '@':
			status = "ops";
			break;
		case '%':
			status = "half_ops";
			break;
		case '+':
			status = "voice";
			break;
		default:
			nick--;
			statusstr[0] = '\0';
			break;
	}
	mccdata->mcc_ImageBuffer[0] = '\0';
	if( status ) {
		if( ( LRC( OID_GUI_NICKLISTGFXINFO ) ) ) {
			sprintf( mccdata->mcc_ImageBuffer, "\033I[5:PROGDIR:smilies/nicklist/%s]", status );
		} else {
			sprintf( mccdata->mcc_ImageBuffer, "\033P[%ld]%s", mccdata->mcc_Pen[ PEN_USERLISTTEXT ], statusstr );
		}
	}
	sprintf( mccdata->mcc_DisplayBuffer, "\033P[%ld]%s", mccdata->mcc_Pen[ PEN_USERLISTTEXT ], nick );
	msg->strings[0] = (STRPTR) mccdata->mcc_ImageBuffer;
	msg->strings[1] = (STRPTR) mccdata->mcc_DisplayBuffer;

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

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

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

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

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

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	return( Stricmp( (_ub_cs) nick1, (_ub_cs) nick2 ) );
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
		case OM_NEW                      : return( OM_New                ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                  : return( OM_Dispose            ( cl, obj, (APTR) msg ) );
		case OM_SET                      : return( OM_Set                ( cl, obj, (APTR) msg ) );
		case MUIM_Setup                  : return( OM_Setup              ( cl, obj, (APTR) msg ) );
		case MUIM_Cleanup                : return( OM_Cleanup            ( cl, obj, (APTR) msg ) );
		case MUIM_ContextMenuChoice      : return( MM_ContextMenuSelect  ( cl, obj, (APTR) msg ) );
		case MUIM_ContextMenuBuild       : return( MM_ContextMenuBuild   ( cl, obj, (APTR) msg ) );

		case MUIM_NList_Display          : return( OM_Display            ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct         : return( OM_Destruct           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Construct        : return( OM_Construct          ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Compare          : return( OM_Compare            ( cl, obj, (APTR) msg ) );

		case MM_CHATUSERLIST_PENSOBTAIN  : return( MM_PensObtain         ( cl, obj, (APTR) msg ) );
		case MM_CHATUSERLIST_PENSRELEASE : return( MM_PensRelease        ( cl, obj, (APTR) msg ) );
		case MM_CHATUSERLIST_PENSUPDATE  : return( MM_PensUpdate         ( cl, obj, (APTR) msg ) );
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

