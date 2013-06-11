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

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <string.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_nicklist.h"
#include "version.h"

/*************************************************************************/

#define ABOUT_LOGONAME "progdir:wookiechat_logo.gif" 

/*
** gadgets used by this class
*/

enum
{
GID_AREXXPORT = 0,
GID_LAST
};

/*
** data used by this class
*/

#define AREXXPORT_SIZEOF 0x100

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	if( (obj = (Object *)DoSuperNew( cl, obj,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );

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
struct TagItem *tstate;

	for( tstate = msg->ops_AttrList ; ( tag = NextTagItem( &tstate ) ) ; ) {
		ULONG tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
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
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                      : return( OM_Dispose                       ( cl, obj, (APTR) msg ) );

		case OM_SET                          : return( OM_Set                           ( cl, obj, (APTR) msg ) );

/* application specific methods */

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
	appclasses[ CLASSID_NICKLIST ] = MUI_CreateCustomClass( NULL, MUIC_NList, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_NickList_Dispatcher) );
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




