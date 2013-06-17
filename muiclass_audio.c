/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_audio.c
*/

#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <string.h>
#include <stdio.h>

#include "functions.h"
#include "intern.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowsettings.h"
#include "muiclass_audio.h"
#include "version.h"

#define LINEBUFFER_SIZEOF 0x2000

/*************************************************************************/

enum{
WID_SETTINGS = 0,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	Object                *mcc_SampleObjects[ SID_LAST ];
};

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
	return( (ULONG) DoSuperNew( cl, obj,
							MUIA_NList_Title         , TRUE,
							MUIA_CycleChain          , 1,
							MUIA_NList_Format        , "BAR,",
							MUIA_ObjectID            , OID_ALI_LIST,
							TAG_DONE ) );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; i < SID_LAST ; i++ ) {
		if( mccdata->mcc_SampleObjects[ i ] ) {
			DisposeDTObject( mccdata->mcc_SampleObjects[ i ] );
			mccdata->mcc_SampleObjects[ i ] = NULL;
		}
	}

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// MM_Play()
**
*/

/*************************************************************************/

static ULONG MM_Play( struct IClass *cl, Object *obj, struct MP_AUDIO_PLAY *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	if( !mccdata->mcc_ClassObjects[ WID_SETTINGS ] ) {
		mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (Object *) MUIGetVar( _app( obj ), MA_APPLICATION_WINDOWSETTINGS );
	}
	/* no playback if disabled */
	if( LocalReadConfig( OID_SND_ENABLE ) ) {
		char *extplayer = (char *) LRC( OID_SND_USEEXTERNALPLAYER );
		/* we need to load sample */
		if( !mccdata->mcc_SampleObjects[ msg->SID ] || msg->Name || extplayer ) {
			char *path;
			BPTR lock = 0;

			path = (char *) LocalReadConfig( OID_SND_CTCPSAMPLES );
			if( !path || !path[0] ) {
				path = DEFAULT_SETTINGSSAMPLEPATH;
			}
			if( !(lock = Lock( (_s_cs) path, ACCESS_READ ) ) ) {
				lock = CurrentDir( lock );
			}
			/* if name is specified we need to load sample, so free old one, if present */
			if( mccdata->mcc_SampleObjects[ msg->SID ] ) {
				DisposeDTObject( mccdata->mcc_SampleObjects[ msg->SID ] );
			}

			if( !( path = msg->Name ) ) { /* no name, then obtain using purpose and settings */
				switch( msg->SID ) {
					case SID_TABOPENING:
						path = (char *) LocalReadConfig( OID_SND_SPLONTABOPENING );
						break;
					case SID_HIGHLIGHT:
						path = (char *) LocalReadConfig( OID_SND_SPLONHIGHLIGHT );
						break;
					case SID_PRIVMSG:
						path = (char *) LocalReadConfig( OID_SND_SPLONPRIVMSG );
						break;
				}
			}

			if( path ) {
				if( !extplayer ) {
					mccdata->mcc_SampleObjects[ msg->SID ] = NewDTObject( path, DTA_GroupID, GID_SOUND, TAG_END );
				} else { /* external player does not use objects */
					char *filename;
					if( ( extplayer = (char *) LocalReadConfig( OID_SND_EXTERNALPLAYER ) ) && *extplayer ) {
						if( ( filename = AllocVec( strlen( path ) + strlen( extplayer ) + 32, MEMF_ANY ) ) ) {
							sprintf( filename, "run >nil: %s %s", extplayer, filename );
#ifdef __amigaos4__
							SystemTags( (_s_cs) filename, TAG_DONE);
#else
							Execute(    (_s_cs) filename, 0, 0 );
#endif
							FreeVec( filename );
						}
					}
				}
			}
			if( lock ) {
				UnLock( CurrentDir( lock ) );
			}
		}
		if( mccdata->mcc_SampleObjects[ msg->SID ] ) {
			DoMethod( mccdata->mcc_SampleObjects[ msg->SID ], DTM_TRIGGER, NULL, STM_PLAY, NULL );
		}
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_Audio_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_Audio_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                        : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                    : return( OM_Dispose                       ( cl, obj, (APTR) msg ) );
		case MM_AUDIO_PLAY                 : return( MM_Play                          ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_Audio_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_Audio_InitClass( void )
{
	appclasses[ CLASSID_AUDIO ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_Area, NULL, sizeof( struct mccdata ), (APTR) ENTRY(MCC_Audio_Dispatcher) );
	return( appclasses[ CLASSID_AUDIO ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_Audio_DisposeClass()
**
*/

/*************************************************************************/

void MCC_Audio_DisposeClass( void )
{
	if( appclasses[ CLASSID_AUDIO ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_AUDIO ] );
		appclasses[ CLASSID_AUDIO ] = NULL;
    }
}
/* \\\ */




