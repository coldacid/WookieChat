/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowcolorsettings.c
*/

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>
#include <string.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsnick.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

#define LINEBUFFER_SIZEOF 0x2000

#define NICK_SIZEOF     0x10
#define USERNAME_SIZEOF 0x20
#define REALNAME_SIZEOF 0x20
 
/*
** gadgets used by this class
*/

enum
{
GID_NICK = 0,
GID_NICK1,
GID_NICK2,
GID_USERNAME,
GID_REALNAME,
GID_LAST
};

#define GID_LASTNICK   GID_REALNAME
#define GID_FIRSTNICK  GID_NICK
#define NICK_NUMBEROF ( GID_LASTNICK - GID_FIRSTNICK + 1 )

/*
** data used by this class
*/

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
ULONG i;

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, TRUE,
				Child, ColGroup(2),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSNICK_NICK_GAD ),
					Child, HGroup,
						Child, objs[ GID_NICK     ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSNICK_NICK_GAD, NICK_SIZEOF ),
						Child, HVSpace,
					End,
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSNICK_NICKALT1_GAD ),
					Child, HGroup,
						Child, objs[ GID_NICK1    ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSNICK_NICKALT1_GAD, NICK_SIZEOF ),
						Child, HVSpace,
					End,
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSNICK_NICKALT2_GAD ),
					Child, HGroup,
						Child, objs[ GID_NICK2    ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSNICK_NICKALT2_GAD, NICK_SIZEOF ),
						Child, HVSpace,
					End,
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSNICK_USERNAME_GAD ),
					Child, HGroup,
						Child, objs[ GID_USERNAME ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSNICK_USERNAME_GAD, USERNAME_SIZEOF ),
						Child, HVSpace,
					End,
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSNICK_REALNAME_GAD ),
					Child, HGroup,
						Child, objs[ GID_REALNAME ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSNICK_REALNAME_GAD, REALNAME_SIZEOF ),
						Child, HVSpace,
					End,
				End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		for( i = 0 ; i < NICK_NUMBEROF ; i++ ) {
			SetAttrs( objs[ GID_FIRSTNICK + i ], MUIA_ObjectID, OID_SETTINGSNICK + 1 + i, TAG_DONE );
		}

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

/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsNick_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsNick_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                      : return( OM_New                     ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                  : return( OM_Dispose                 ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsNick_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsNick_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSNICK ] = MUI_CreateCustomClass( NULL, MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsNick_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSNICK ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsNick_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsNick_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSNICK ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSNICK ] );
		appclasses[ CLASSID_SETTINGSNICK ] = NULL;
    }
}
/* \\\ */


