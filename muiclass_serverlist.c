/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_serverlist.c
*/

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>

#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <string.h>
#include <stdio.h>

#include "functions.h"
#include "intern.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_serverlist.h"
#include "muiclass_channellist.h"
#include "muiclass_nicklist.h"
#include "version.h"

/*************************************************************************/

#define LINEBUFFER_SIZEOF 0x2000

/*
** gadgets used by this class
*/

enum
{
GID_NICKLIST = 0,
GID_CHANNELLIST,
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

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
	if( ( obj = (Object *) DoSuperNew( cl, obj,
													MUIA_NList_Title         , TRUE,
													MUIA_CycleChain          , 1,
													MUIA_NList_Format        , "BAR,BAR,BAR,BAR",
													MUIA_ObjectID            , OID_SVR_LIST,
													TAG_DONE ) ) ) {

		DoMethod( obj, MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime, obj, 1, MM_SERVERLIST_ACTIVECHANGE );

	}
	return( (ULONG) obj );
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
			case MA_SERVERLIST_CHANNELLISTOBJ:
				mccdata->mcc_ClassObjects[ GID_CHANNELLIST ] = (Object *) tidata;
				break;
			case MA_SERVERLIST_NICKLISTOBJ:
				mccdata->mcc_ClassObjects[ GID_NICKLIST    ] = (Object *) tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
STRPTR *array = msg->strings;
struct ServerEntry *se;
static char displayport[10];

	if( ( se = msg->entry ) ) {
		sprintf( displayport, (char*) "%ld", se->se_Port );
		*array++ = (STRPTR) se->se_Name;
		*array++ = (STRPTR) displayport;
		*array++ = (STRPTR) ( se->se_Flags & SERVERENTRYF_AUTOCONNECT ) ? LGS( MSG_LV_YES ) : LGS( MSG_LV_NO );
		*array++ = (STRPTR) se->se_Charset;
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_SERVER  );
		*array++ = (STRPTR) LGS( MSG_LV_PORT    );
		*array++ = (STRPTR) LGS( MSG_LV_CONNECT );
		*array++ = (STRPTR) LGS( MSG_LV_CHARSET );
    }
	return( 0 );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Destruct( struct IClass *cl, Object *obj, struct MUIP_NList_Destruct *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se;
struct ChannelEntry *ce, *tce;
struct NickEntry *ne, *tne;
ULONG i;

	if( ( se = msg->entry ) ) {
		while( ( ce = (APTR) se->se_ChannelList.lh_Head )->ce_Succ ) {
			for( i = 0 ; ; i++ ) {
				tce = NULL;
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_GetEntry, i, &tce );
				if( tce ) {
					if( ( tce == ce ) ) {
						/* if channel is in list, remove or we will display trashed data */
						DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_Remove, i );
						break;
					}
				} else {
					break;
				}
			}
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MM_CHANNELLIST_REMOVE, ce );
		}
		while( ( ne = (APTR) se->se_NickList.lh_Head )->ne_Succ ) {
			for( i = 0 ; ; i++ ) {
				tne = NULL;
				DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_NList_GetEntry, i, &tne );
				if( tne ) {
					if( ( tne == ne ) ) {
						/* if channel is in list, remove or we will display trashed data */
						DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_Remove, i );
						break;
					}
				} else {
					break;
				}
			}
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MM_CHANNELLIST_REMOVE, ne );
		}
		FreeMem( se, sizeof( struct ServerEntry ) );
    }
	return( 0 );
}
/* \\\ */

/* /// OM_Import()
**
*/

/*************************************************************************/

static ULONG OM_Import( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
	DoMethod( obj, MM_SERVERLIST_IMPORTLISTASTEXT, DEFAULT_SETTINGSSERVER_NAME );

	DoMethod( obj, MM_SERVERLIST_ACTIVECHANGE );

	return( DoSuperMethodA( cl, obj, (Msg) msg ) );
}
/* \\\ */
/* /// OM_Export()
**
*/

/*************************************************************************/

static ULONG OM_Export( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{

	DoMethod( obj, MM_SERVERLIST_EXPORTLISTASTEXT, DEFAULT_SETTINGSSERVER_NAME );

	return( DoSuperMethodA( cl, obj, (Msg) msg ) );
}
/* \\\ */

/* /// MM_Add()
**
*/

/*************************************************************************/

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_SERVERLIST_ADD *msg )
{
struct ServerEntry *se;

	if( ( se = AllocVec( sizeof( struct ServerEntry ), MEMF_ANY|MEMF_CLEAR ) ) ) {
		NEWLIST( &se->se_ChannelList );
		NEWLIST( &se->se_NickList );
		strncpy( (char *) se->se_Name    , (char *) ( msg->Name     ? msg->Name     : LGS( MSG_MUICLASS_SERVERLIST_DEFAULTNAME )), SERVERENTRY_NAME_SIZEOF     );
		strncpy( (char *) se->se_Address , (char *) ( msg->Address  ? msg->Address  : LGS( MSG_MUICLASS_SERVERLIST_DEFAULTADDRESS )), SERVERENTRY_ADDRESS_SIZEOF  );
		strncpy( (char *) se->se_Password, (char *) ( msg->Password ? msg->Password : (STRPTR) ""      ), SERVERENTRY_PASSWORD_SIZEOF );
		strncpy( (char *) se->se_Charset , (char *) ( msg->Charset  ? msg->Charset  : (STRPTR) "UTF-8" ), SERVERENTRY_CHARSET_SIZEOF  );
		se->se_Port  = msg->Port;
		se->se_Flags = msg->Flags;
		DoMethod( obj, MUIM_NList_InsertSingle, se, MUIV_NList_Insert_Bottom );
	}
	return( (ULONG) se );
}
/* \\\ */
/* /// MM_ActiveChange()
**
*/

/*************************************************************************/

static ULONG MM_ActiveChange( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se;
struct ChannelEntry *ce;
struct NickEntry *ne;

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_Clear );
	DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST    ], MUIM_NList_Clear );

	se = NULL;
	DoMethod( obj, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &se );
	if( se ) {
		for( ce = (APTR) se->se_ChannelList.lh_Head ; ce->ce_Succ ; ce = ce->ce_Succ ) {
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_InsertSingle, ce, MUIV_NList_Insert_Bottom );
		}
		for( ne = (APTR) se->se_NickList.lh_Head ; ne->ne_Succ ; ne = ne->ne_Succ ) {
			DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_NList_InsertSingle, ne, MUIV_NList_Insert_Bottom );
		}
	}
	return( 0 );
}
/* \\\ */

/* /// MM_ExportListAsText()
*/

/*************************************************************************/

static ULONG MM_ExportListAsText( struct IClass *cl, Object *obj, struct MP_SERVERLIST_EXPORTLISTASTEXT *msg )
{
BPTR handle;
ULONG i;
struct ServerEntry *se;

	if( ( handle = Open( (_ub_cs) msg->Name, MODE_NEWFILE ) ) ) {
		for( i = 0 ;  ; i++ ) {
			se = NULL;
			DoMethod( obj, MUIM_NList_GetEntry, i, &se );
			if( se ) {
				IPTR args[10];
				args[0] = (IPTR) se->se_Name;
				args[1] = (IPTR) se->se_Address;
				args[2] = (IPTR) se->se_Port;
				args[3] = (IPTR) se->se_Password;
				args[4] = (IPTR) se->se_Charset;
				args[5] = (IPTR) ( ( se->se_Flags & SERVERENTRYF_AUTOCONNECT ) ? " CONNECT" : "" );
				VPrintf( (CONST_STRPTR)  "SERVER \"%s\" ADDRESS \"%s\" PORT %ld PASSWORD \"%s\" CHARSET \"%s\"%s\n", &args );
				#if 0
				sprintf( buffer, "SERVER \"%s\" ADDRESS \"%s\" PORT %ld PASSWORD \"%s\" CHARSET \"%s\"%s",
													se->se_Name,
													se->se_Address,
													se->se_Port,
													se->se_Password,
													se->se_Charset,
													( ( se->se_Flags & SERVERENTRYF_AUTOCONNECT ) ? " CONNECT" : "" )
													);
				FPuts( handle, (CONST_STRPTR) buffer );
				#endif
			} else {
	            break;
			}
		}
		Close( handle );
    }
	return( 0 );
}
/* \\\ */
/* /// MM_ImportListAsText()
*/


/*************************************************************************/

static ULONG MM_ImportListAsText( struct IClass *cl, Object *obj, struct MP_SERVERLIST_IMPORTLISTASTEXT *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
BPTR handle;
char *linebuffer;
struct ServerEntry *se = NULL;

	DoMethod( obj, MUIM_NList_Clear );

	if( ( linebuffer = AllocVec( LINEBUFFER_SIZEOF, MEMF_ANY ) ) ) {
		if( ( handle = Open( (_ub_cs) msg->Name, MODE_OLDFILE ) ) ) {
			while( FGets( handle, (STRPTR) linebuffer, LINEBUFFER_SIZEOF - 1 ) ) {
				struct SimpleReadArgsData *srad;
				if( *linebuffer && *linebuffer != ';' ) {
					if( ( srad = SimpleReadArgsParse( "SERVER/A,ADDRESS/A,PORT/N/A,PASSWORD/A,CHARSET/A,CONNECT/S", linebuffer ) ) ) {
						enum{ SERVERARG_SERVER = 0, SERVERARG_ADDRESS, SERVERARG_PORT, SERVERARG_PASSWORD, SERVERARG_CHARSET, SERVERARG_CONNECT };
						se = (APTR) DoMethod( obj, MM_SERVERLIST_ADD, srad->srad_ArgArray[SERVERARG_SERVER],
																		srad->srad_ArgArray[SERVERARG_ADDRESS],
																		*((ULONG*)srad->srad_ArgArray[SERVERARG_PORT]),
																		srad->srad_ArgArray[SERVERARG_PASSWORD],
																		srad->srad_ArgArray[SERVERARG_CHARSET],
																		( srad->srad_ArgArray[SERVERARG_CONNECT] ? SERVERENTRYF_AUTOCONNECT : 0 ) );
						SimpleReadArgsFree( srad );
					} else {
						if( se ) {
							if( ( srad = SimpleReadArgsParse( "CHANNEL/A,PASSWORD/A", linebuffer ) ) ) {
								DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MM_CHANNELLIST_ADD, se, srad->srad_ArgArray[0], srad->srad_ArgArray[1] );
								SimpleReadArgsFree( srad );
							} else {
								if( ( srad = SimpleReadArgsParse( "NICK/A,PASSWORD/A", linebuffer ) ) ) {
									DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MM_NICKLIST_ADD, se, srad->srad_ArgArray[0], srad->srad_ArgArray[1] );
								}
							}
						}
					}
				}
			}
			Close( handle );
        }
		FreeVec( linebuffer );
    }
	return( 0 );
}
/* \\\ */


/*
** Dispatcher, init and dispose
*/

/* /// MCC_ServerList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ServerList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_SET                          : return( OM_Set                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct             : return( OM_Destruct                      ( cl, obj, (APTR) msg ) );

		case MUIM_Import                     : return( OM_Import                        ( cl, obj, (APTR) msg ) );
		case MUIM_Export                     : return( OM_Export                        ( cl, obj, (APTR) msg ) );

		case MM_SERVERLIST_ADD               : return( MM_Add                           ( cl, obj, (APTR) msg ) );
		case MM_SERVERLIST_ACTIVECHANGE      : return( MM_ActiveChange                  ( cl, obj, (APTR) msg ) );
		case MM_SERVERLIST_IMPORTLISTASTEXT  : return( MM_ImportListAsText              ( cl, obj, (APTR) msg ) );
		case MM_SERVERLIST_EXPORTLISTASTEXT  : return( MM_ExportListAsText              ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ServerList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ServerList_InitClass( void )
{
	appclasses[ CLASSID_SERVERLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, sizeof( struct mccdata ), (APTR) ENTRY(MCC_ServerList_Dispatcher) );
	return( appclasses[ CLASSID_SERVERLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ServerList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ServerList_DisposeClass( void )
{
	if( appclasses[ CLASSID_SERVERLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SERVERLIST ] );
		appclasses[ CLASSID_SERVERLIST ] = NULL;
    }
}
/* \\\ */




