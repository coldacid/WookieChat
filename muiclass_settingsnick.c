/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingsnick.c
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

/*
** configitem structure
*/

struct ConfigItem {
	ULONG GadgetID;
	ULONG ObjectID;
	ULONG Attr;
	LONG  Default;
};

static struct ConfigItem TAB_CONFIGITEMS[] = {
#ifdef __MORPHOS__
	{ GID_NICK             , OID_NIK_NICK    , MUIA_String_Contents, (LONG) "MorphOSUser" },
	{ GID_NICK1            , OID_NIK_NICK1   , MUIA_String_Contents, (LONG) "MorphOSUser_" },
	{ GID_NICK2            , OID_NIK_NICK2   , MUIA_String_Contents, (LONG) "MorphOSUser__" },
	{ GID_USERNAME         , OID_NIK_USERNAME, MUIA_String_Contents, (LONG) "MorphOSUser__" },
#elif __AROS__
	{ GID_NICK             , OID_NIK_NICK    , MUIA_String_Contents, (LONG) "AROSUser" },
	{ GID_NICK1            , OID_NIK_NICK1   , MUIA_String_Contents, (LONG) "AROSUser_" },
	{ GID_NICK2            , OID_NIK_NICK2   , MUIA_String_Contents, (LONG) "AROSUser__"  },
	{ GID_USERNAME         , OID_NIK_USERNAME, MUIA_String_Contents, (LONG) "AROSUser" },
#elif __amigaos4__
	{ GID_NICK             , OID_NIK_NICK    , MUIA_String_Contents, (LONG) "AmigaOS4User"  },
	{ GID_NICK1            , OID_NIK_NICK1   , MUIA_String_Contents, (LONG) "AmigaOS4User_"  },
	{ GID_NICK2            , OID_NIK_NICK2   , MUIA_String_Contents, (LONG) "AmigaOS4User__"  },
	{ GID_USERNAME         , OID_NIK_USERNAME, MUIA_String_Contents, (LONG) "AmigaOS4User"  },
#else
	{ GID_NICK             , OID_NIK_NICK    , MUIA_String_Contents, (LONG) "AmigaUser"   },
	{ GID_NICK1            , OID_NIK_NICK1   , MUIA_String_Contents, (LONG) "AmigaUser_"  },
	{ GID_NICK2            , OID_NIK_NICK2   , MUIA_String_Contents, (LONG) "AmigaUser__" },
	{ GID_USERNAME         , OID_NIK_USERNAME, MUIA_String_Contents, (LONG) "AmigaUser"   },
#endif
	{ GID_REALNAME         , OID_NIK_REALNAME, MUIA_String_Contents, (LONG) "John Wookie" },
	{ -1,0,0,0 },
};

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

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, TRUE,
				Child, HVSpace,
				Child, VGroup,
					Child, HVSpace,
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
					Child, HVSpace,
				End,
				Child, HVSpace,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( obj, MM_SETTINGSNICK_RESETTODEFAULTS );

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// MM_ResetToDefaults()
**
*/

/*************************************************************************/

static ULONG MM_ResetToDefaults( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		SetAttrs( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ],
								TAB_CONFIGITEMS[ i ].Attr, TAB_CONFIGITEMS[ i ].Default,
								MUIA_ObjectID            , TAB_CONFIGITEMS[ i ].ObjectID,
								TAG_DONE );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSNICK_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].ObjectID == msg->ObjectID ) {
			return( (ULONG) MUIGetVar( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr ) );
		}
	}
	return( 0 );
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
		case OM_NEW                              : return( OM_New                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSNICK_RESETTODEFAULTS     : return( MM_ResetToDefaults         ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSNICK_READCONFIG          : return( MM_ReadConfig              ( cl, obj, (APTR) msg ) );
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
	appclasses[ CLASSID_SETTINGSNICK ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsNick_Dispatcher) );
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



