/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_nicklist.c
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

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_nicklist.h"
#include "muiclass_serverlist.h"
#include "version.h"

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	return( (IPTR) (obj = (Object *) DoSuperNew( cl, obj,
													MUIA_NList_Title         , TRUE,
													MUIA_NList_Format        , "BAR,",
													MUIA_CycleChain          , 1,
													TAG_DONE ) ) );
}
/* \\\ */
/* /// OM_Display()
**
*/

/*************************************************************************/

static IPTR OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
STRPTR *array = msg->strings;
struct NickEntry *ne;

	if( ( ne = msg->entry ) ) {
		*array++ = (STRPTR) ne->ne_Name;
		*array++ = (STRPTR) ne->ne_Password;
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_NICK     );
		*array++ = (STRPTR) LGS( MSG_LV_PASSWORD );
    }
	return( 0 );
}
/* \\\ */

/* /// MM_Add()
**
*/

/*************************************************************************/

static IPTR MM_Add( struct IClass *cl, Object *obj, struct MP_NICKLIST_ADD *msg )
{
struct NickEntry *ne;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( ne = AllocVec( sizeof( struct NickEntry ), MEMF_ANY|MEMF_CLEAR ) ) ) {
		strncpy( (char *) ne->ne_Name    , (char *) ( msg->Nick     ? msg->Nick     : LGS( MSG_MUICLASS_NICKLIST_DEFAULTNAME )), NICKENTRY_NICK_SIZEOF     );
		strncpy( (char *) ne->ne_Password, (char *) ( msg->Password ? msg->Password : (STRPTR) "" ), NICKENTRY_PASSWORD_SIZEOF  );
		if( msg->ServerEntry ) {
			AddTail( &((struct ServerEntry *) msg->ServerEntry)->se_NickList, (struct Node *) ne );
		}
	}
	return( (IPTR) ne );
}
/* \\\ */
/* /// MM_Remove()
**
*/

/*************************************************************************/

static IPTR MM_Remove( struct IClass *cl, Object *obj, struct MP_NICKLIST_REMOVE *msg )
{
struct NickEntry *ne;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( ne = msg->NickEntry ) ) {
		if( ne->ne_Succ && ne->ne_Pred ) {
			Remove( (struct Node *) ne );
		}
		FreeVec( ne );
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_NickList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_NickList_Dispatcher)
{
	switch( msg->MethodID )
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );

		case MM_NICKLIST_ADD                 : return( MM_Add                           ( cl, obj, (APTR) msg ) );
		case MM_NICKLIST_REMOVE              : return( MM_Remove                        ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_NickList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_NickList_InitClass( void )
{
	appclasses[ CLASSID_NICKLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0 ,  (APTR) ENTRY(MCC_NickList_Dispatcher) );
	return( appclasses[ CLASSID_NICKLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_NickList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_NickList_DisposeClass( void )
{
	if( appclasses[ CLASSID_NICKLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_NICKLIST ] );
		appclasses[ CLASSID_NICKLIST ] = NULL;
    }
}
/* \\\ */




