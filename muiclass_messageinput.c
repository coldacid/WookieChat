/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_messageinput.c
*/

#define NODEBUG

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>

#include <devices/rawkeycodes.h>
#include <devices/inputevent.h>

#include <string.h>
#include <stdio.h>

#include "functions.h"
#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_chatuserlist.h"
#include "muiclass_settingsgeneral.h"
#include "muiclass_windowsettings.h"
#include "muiclass_messageinput.h"
#include "muiclass_network.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
WID_SETTINGS = 0,
GID_USERLIST,
GID_LAST
};

/*
** data used by this class
*/
#define NICKBUFFER_SIZEOF 0x200

struct mccdata
{
	Object                      *mcc_ClassObjects[ GID_LAST ];
	struct MUI_EventHandlerNode  mcc_EventHandlerNode;
	struct List                  mcc_HistoryList;
	char                         mcc_NickPostBuffer[ NICKBUFFER_SIZEOF ];
	char                         mcc_NickPreBuffer[ NICKBUFFER_SIZEOF ];
	char                         mcc_NickBuffer[ NICKBUFFER_SIZEOF ];
	char                         mcc_ComposeBuffer[ NICKBUFFER_SIZEOF ];
	ULONG                        mcc_NickStart;
	ULONG                        mcc_NickLength;
};

struct History {
	struct History *h_Succ;
	struct History *h_Pred;
	char            h_Message[1];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = DoSuperNew( cl, obj, TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		NEWLIST( &mccdata->mcc_HistoryList );

		DoMethod( obj, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, obj, 2, MM_MESSAGEINPUT_ADD, NULL );
	}
	return( (IPTR) obj );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct History *history;

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	while( ( history = (APTR) ( mccdata->mcc_HistoryList.lh_Head ) )->h_Succ ) {
		DoMethod( obj, MM_MESSAGEINPUT_REMOVE, history );
	}

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Setup()
**
*/

/*************************************************************************/

static ULONG OM_Setup( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG rc;

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( rc = DoSuperMethodA( cl, obj, msg ) ) ) {

		mccdata->mcc_EventHandlerNode.ehn_Events = (IDCMP_RAWKEY);
		mccdata->mcc_EventHandlerNode.ehn_Object = obj;
		mccdata->mcc_EventHandlerNode.ehn_Class  = cl;
		DoMethod( obj, MUIM_Window_AddEventHandler, &mccdata->mcc_EventHandlerNode );
	}
	return( rc );
}

/* \\\ */
/* /// OM_Cleanup()
**
*/

/*************************************************************************/

static ULONG OM_Cleanup( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MUIM_Window_RemEventHandler, &mccdata->mcc_EventHandlerNode );

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_HandleEvent()
**
*/

/*************************************************************************/

static ULONG OM_HandleEvent( struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( msg->imsg ) {
		struct History *history;
		char *str;
		ULONG pos, max;
		switch( msg->imsg->Class ) {
			case IDCMP_RAWKEY:
				{
					switch( msg->imsg->Code ) {
						default:
							if( !( msg->imsg->Code & IECODE_UP_PREFIX ) ) { /* tab release must not kill tab handling */
								mccdata->mcc_NickStart = 0;
							}
							break;
						case RAWKEY_TAB:
							/* get current string */
							str	= (char *) MUIGetVar( obj, MUIA_String_Contents );
							pos	=          MUIGetVar( obj, MUIA_String_BufferPos );
							max	= MUIGetVar( mccdata->mcc_ClassObjects[ GID_USERLIST ], MUIA_NList_Entries );

							if( pos ) {
								struct ChatNick *cn;
								ULONG count = 0;
								char chr, *tmp;
								if( !mccdata->mcc_NickStart ) {
									/* we found a matching nick, so prepare buffers and enable */
									strcpy( mccdata->mcc_NickPostBuffer, &str[pos] );
									strncpy( mccdata->mcc_NickPreBuffer, str, pos );
									mccdata->mcc_NickPreBuffer[ pos ] = 0x00;

									while( pos && ( ( chr = mccdata->mcc_NickPreBuffer[pos] ) != 0x20 ) && ( chr != ',' ) && ( chr != '.' ) ) {
										pos--;
									}
									if( pos ) {
										pos++;
									}
									strcpy( mccdata->mcc_NickBuffer, &mccdata->mcc_NickPreBuffer[ pos ] );
									mccdata->mcc_NickLength = strlen( mccdata->mcc_NickBuffer );
									mccdata->mcc_NickPreBuffer[ pos ] = 0x00;
									mccdata->mcc_NickStart = 1;
								}
								debug("pre: '%s'\n", mccdata->mcc_NickPreBuffer );
								debug("nick: '%s'\n", mccdata->mcc_NickBuffer );
								debug("post: '%s'\n", mccdata->mcc_NickPostBuffer );
								if( mccdata->mcc_NickStart ) {
									do{
										cn = NULL;
										DoMethod( mccdata->mcc_ClassObjects[ GID_USERLIST ], MUIM_NList_GetEntry, mccdata->mcc_NickStart - 1, &cn );
										if( cn ) {
											tmp = cn->cn_ChatNickEntry->cne_Nick;
											if( ( *tmp == '@' ) || ( *tmp == '+' ) || ( *tmp == '%' ) ) {
												tmp++;
											}
											if( !Strnicmp( (CONST_STRPTR) tmp, (CONST_STRPTR) mccdata->mcc_NickBuffer, mccdata->mcc_NickLength ) ) {
												strcpy( mccdata->mcc_ComposeBuffer, mccdata->mcc_NickPreBuffer );
												strcat( mccdata->mcc_ComposeBuffer, tmp );
												switch( LRC( OID_GEN_NICKCOMPLETITION ) ) {
													case NICKCOMPLETITIONMODE_MIRCSTYLE:
														break;
													case NICKCOMPLETITIONMODE_AMIRCSTYLE:
														strcat( mccdata->mcc_ComposeBuffer, ": " );
														break;
													default:
													case NICKCOMPLETITIONMODE_WOOKIESTYLE:
														strcat( mccdata->mcc_ComposeBuffer, " " );
														break;
												}
												pos = strlen( mccdata->mcc_ComposeBuffer ); /* cursor behind nick, so the user can continue writing */
												strcat( mccdata->mcc_ComposeBuffer, mccdata->mcc_NickPostBuffer );
												SetAttrs( obj, MUIA_NoNotify, TRUE, MUIA_String_Contents, mccdata->mcc_ComposeBuffer, MUIA_String_BufferPos, pos, TAG_DONE );
												SetAttrs( _win(obj), MUIA_Window_ActiveObject, obj, TAG_DONE );
												cn = NULL; /* terminate loop */
											}
										}
										mccdata->mcc_NickStart++;
										count++;
										if( count > max ) {
											mccdata->mcc_NickStart = 0;
											break;
										}
										if( max < mccdata->mcc_NickStart ) {
											mccdata->mcc_NickStart = 1;
										}
									} while( cn );
								}
							} else {
								mccdata->mcc_NickStart = 0; /* turn off */
							}
							return(0);
							break;
						case RAWKEY_UP:
							mccdata->mcc_NickStart = 0;
							/* get current string */
							str	= (char *) MUIGetVar( obj, MUIA_String_Contents );
							pos	=          MUIGetVar( obj, MUIA_String_BufferPos );

							if( strlen( str ) < pos ) {
								pos = strlen( str );
							}
							/* get current strings history entry (if exists) */
							history = (APTR) DoMethod( obj, MM_MESSAGEINPUT_FIND, NULL, str, strlen(str), MV_MESSAGEINPUT_MODE_EXACT, MV_MESSAGEINPUT_DIRECTION_DOWN );

							if( ( msg->imsg->Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT | IEQUALIFIER_CAPSLOCK ) ) ) {
								/* find next entry maching */
								history = (APTR) DoMethod( obj, MM_MESSAGEINPUT_FIND, history, str, pos, MV_MESSAGEINPUT_MODE_PARTIAL, MV_MESSAGEINPUT_DIRECTION_DOWN );
							} else {
								if( msg->imsg->Qualifier & IEQUALIFIER_CONTROL ) {
									/* find last entry maching */
									history = (APTR) mccdata->mcc_HistoryList.lh_Head;
								} else {
									/* find next entry */
									if( history ) {
										history = history->h_Succ;
									} else {
										history = (APTR) mccdata->mcc_HistoryList.lh_Head;
									}
								}
							}
							if( history && history->h_Succ ) {
								SetAttrs( obj, MUIA_NoNotify, TRUE, MUIA_String_Contents, history->h_Message, MUIA_String_BufferPos, pos, TAG_DONE );
							}

							break;
						case RAWKEY_DOWN:
							mccdata->mcc_NickStart = 0;
							/* get current string */
							str	= (char *) MUIGetVar( obj, MUIA_String_Contents );
							pos	=          MUIGetVar( obj, MUIA_String_BufferPos );
							if( strlen( str ) < pos ) {
								pos = strlen( str );
							}
							/* get current strings history entry (if exists) */
							history = (APTR) DoMethod( obj, MM_MESSAGEINPUT_FIND, NULL, str, strlen(str), MV_MESSAGEINPUT_MODE_EXACT, MV_MESSAGEINPUT_DIRECTION_UP );

							if( ( msg->imsg->Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_LSHIFT | IEQUALIFIER_CAPSLOCK ) ) ) {
								/* find next entry from this one matching */
								history = (APTR) DoMethod( obj, MM_MESSAGEINPUT_FIND, history, str, pos, MV_MESSAGEINPUT_MODE_PARTIAL, MV_MESSAGEINPUT_DIRECTION_UP );
							} else {
								/* find next last entry */
								if( ( msg->imsg->Qualifier & ( IEQUALIFIER_CONTROL ) ) ) {
									history = (APTR) mccdata->mcc_HistoryList.lh_TailPred;
								} else {
									/* find next entry */
									if( history ) {
										history = history->h_Pred;
									} else {
										history = (APTR) mccdata->mcc_HistoryList.lh_TailPred;
									}
								}
							}
							if( history && history->h_Pred ) {
								SetAttrs( obj, MUIA_NoNotify, TRUE, MUIA_String_Contents, history->h_Message, MUIA_String_BufferPos, pos, TAG_DONE );
							}
							break;
					}
				}
		}
	}
	return( DoSuperMethodA( cl, obj, msg ) );
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
NTIP struct TagItem *tstate;

	for( tstate = msg->ops_AttrList ; ( tag = NextTagItem( &tstate ) ) ; ) {
		ULONG tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
			case MA_MESSAGEINPUT_OBJECTSETTINGS:
				mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (Object *) tidata;
				break;
			case MA_MESSAGEINPUT_OBJECTUSERLIST:
				mccdata->mcc_ClassObjects[ GID_USERLIST ] = (Object *) tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// MM_Find()
**
*/

/*************************************************************************/

static ULONG MM_Find( struct IClass *cl, Object *obj, struct MP_MESSAGEINPUT_FIND *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct History *history = msg->First;
ULONG length;

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( !IsListEmpty( &mccdata->mcc_HistoryList ) ) {
		if( ( length = msg->Length ) ) {
			if( msg->Direction == MV_MESSAGEINPUT_DIRECTION_DOWN ) {
				if( !history ) {
					history = (APTR) &mccdata->mcc_HistoryList;
				}
				for( history = (APTR) history->h_Succ ; history->h_Succ ; history = history->h_Succ ) {
//					  debug("#1 compare '%s' '%s' (%ld)\n", history->h_Message, msg->Message, length );
					if( !Strnicmp( (CONST_STRPTR) history->h_Message, (CONST_STRPTR) msg->Message, length ) ) {
						if( msg->Mode == MV_MESSAGEINPUT_MODE_EXACT ) {
							if( strlen( history->h_Message ) != length ) {
								continue;
							}
						}
//						  debug("#1 found\n");
						return( (IPTR) history );
					}
				}
			} else {
				if( !history ) {
					history = (APTR) &mccdata->mcc_HistoryList.lh_Tail;
				}
				for( history = (APTR) history->h_Pred ; history->h_Pred ; history = history->h_Pred ) {
//					  debug("#2 compare '%s' '%s' (%ld)\n", history->h_Message, msg->Message, length );
					if( !Strnicmp( (CONST_STRPTR) history->h_Message, (CONST_STRPTR) msg->Message, length ) ) {
						if( msg->Mode == MV_MESSAGEINPUT_MODE_EXACT ) {
							if( strlen( history->h_Message ) != length ) {
								continue;
							}
						}
//						  debug("#2 found\n");
						return( (IPTR) history );
					}
				}
			}
		}
	}
	return( (IPTR) NULL );
}
/* \\\ */
/* /// MM_Add()
**
*/

/*************************************************************************/

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_MESSAGEINPUT_ADD *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct History *history;
char *str;
ULONG len;

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( !( str = msg->Message ) ) {
		str = (char*) MUIGetVar( obj, MUIA_String_Contents );
	}
	len = strlen( str );

	if( ( history = (APTR) DoMethod( obj, MM_MESSAGEINPUT_FIND, NULL, str, len, MV_MESSAGEINPUT_MODE_EXACT, MV_MESSAGEINPUT_DIRECTION_DOWN ) ) ) {
		Remove( (struct Node *) history );
		AddHead( &mccdata->mcc_HistoryList, (struct Node *) history );
		return( (IPTR) history );
	}
	if( ( history = AllocVec( len + sizeof( struct History ) + 1, MEMF_ANY ) ) ) {
		strcpy( history->h_Message, str );
		AddHead( &mccdata->mcc_HistoryList, (struct Node *) history );
		debug("adding %s\n", history->h_Message );
	}
	return( (IPTR) history );
}
/* \\\ */
/* /// MM_Remove()
**
*/

/*************************************************************************/

static ULONG MM_Remove( struct IClass *cl, Object *obj, struct MP_MESSAGEINPUT_REMOVE *msg )
{
struct History *history;

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( history = msg->History ) ) {
		if( history->h_Succ ) {
			Remove( (APTR) history );
		}
		FreeVec( history );
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_MessageInput_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_MessageInput_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                 : return( OM_New               ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE             : return( OM_Dispose           ( cl, obj, (APTR) msg ) );
		case OM_SET                 : return( OM_Set               ( cl, obj, (APTR) msg ) );

		case MUIM_HandleEvent       : return( OM_HandleEvent       ( cl, obj, (APTR) msg ) );
		case MUIM_Setup             : return( OM_Setup             ( cl, obj, (APTR) msg ) );
		case MUIM_Cleanup           : return( OM_Cleanup           ( cl, obj, (APTR) msg ) );

		case MM_MESSAGEINPUT_FIND   : return( MM_Find              ( cl, obj, (APTR) msg ) );
		case MM_MESSAGEINPUT_ADD    : return( MM_Add               ( cl, obj, (APTR) msg ) );
		case MM_MESSAGEINPUT_REMOVE : return( MM_Remove            ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_MessageInput_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_MessageInput_InitClass( void )
{
	appclasses[ CLASSID_MESSAGEINPUT ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_String, NULL, sizeof(struct mccdata), (APTR) ENTRY(MCC_MessageInput_Dispatcher) );
	return( appclasses[ CLASSID_MESSAGEINPUT ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_MessageInput_DisposeClass()
**
*/

/*************************************************************************/

void MCC_MessageInput_DisposeClass( void )
{
	if( appclasses[ CLASSID_MESSAGEINPUT ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_MESSAGEINPUT ] );
		appclasses[ CLASSID_MESSAGEINPUT ] = NULL;
    }
}
/* \\\ */

