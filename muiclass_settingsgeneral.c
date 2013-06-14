/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingsgeneral.c
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

#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsgeneral.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

#define MESSAGE_SIZEOF     0x80
#define EDITOR_SIZEOF      0x200
#define BROWSER_SIZEOF     0x200
#define HIGHLIGHT_SIZEOF   0x200

/*
** gadgets used by this class
*/

enum
{
GID_EDITOR = 0,
GID_BROWSER,
GID_MSGKICK,
GID_MSGQUIT,
GID_MSGPART,
GID_OPENPRIVATEQUERY,
GID_REQUESTWHOIS,
GID_REJOIN,
GID_OPENQUERY,
GID_HIGHLIGHTPATTERN,
GID_NICKCOMPLETITION,
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
	{ GID_EDITOR           , OID_GEN_EDITOR           , MUIA_String_Contents, (LONG) "Ed" },
#ifdef __MORPHOS__
	{ GID_BROWSER          , OID_GEN_BROWSER          , MUIA_String_Contents, (LONG) "Open" },
#else
	{ GID_BROWSER          , OID_GEN_BROWSER          , MUIA_String_Contents, (LONG) "" },
#endif
	{ GID_MSGKICK          , OID_GEN_MSGKICK          , MUIA_String_Contents, (LONG) "Pfffffffffffffffffft" },
	{ GID_MSGQUIT          , OID_GEN_MSGQUIT          , MUIA_String_Contents, (LONG) "Heroes never die...They just reload!" },
	{ GID_MSGPART          , OID_GEN_MSGPART          , MUIA_String_Contents, (LONG) "Heroes never die...They just reload!" },
	{ GID_OPENPRIVATEQUERY , OID_GEN_OPENPRIVATEQUERY , MUIA_Selected       , -1 },
	{ GID_REQUESTWHOIS     , OID_GEN_REQUESTWHOIS     , MUIA_Selected       ,  0 },
	{ GID_REJOIN           , OID_GEN_REJOIN           , MUIA_Selected       ,  0 },
	{ GID_OPENQUERY        , OID_GEN_OPENQUERY        , MUIA_Selected       , -1 },
	{ GID_HIGHLIGHTPATTERN , OID_GEN_HIGHLIGHTPATTERN , MUIA_String_Contents, (LONG) "" },
	{ GID_NICKCOMPLETITION , OID_GEN_NICKCOMPLETITION , MUIA_Cycle_Active   , 0 },
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
static STRPTR TAB_CYCLE_NICKCOMPLETITION[ MSG_CY_AMIRC - MSG_CY_MIRC + 2 ];

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, TRUE,
				//Child, HVSpace,
				Child, VGroup,
					Child, HVSpace,
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGENERAL_MESSAGEWHEN_INFO ),
					Child, ColGroup(2),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_MESSAGEKICK_GAD ),
						Child, objs[ GID_MSGKICK ] = MUICreateString( MSG_MUICLASS_SETTINGSGENERAL_MESSAGEKICK_GAD, MESSAGE_SIZEOF ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_MESSAGEQUIT_GAD ),
						Child, objs[ GID_MSGQUIT ] = MUICreateString( MSG_MUICLASS_SETTINGSGENERAL_MESSAGEQUIT_GAD, MESSAGE_SIZEOF ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_MESSAGEPART_GAD ),
						Child, objs[ GID_MSGPART ] = MUICreateString( MSG_MUICLASS_SETTINGSGENERAL_MESSAGEPART_GAD, MESSAGE_SIZEOF ),
					End,
					Child, VSpace(5),
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGENERAL_DOUBLECLICKONNICKLIST_INFO ),
					Child, ColGroup(2),
						Child, objs[ GID_OPENPRIVATEQUERY ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGENERAL_OPENPRIVATEQUERY_GAD, FALSE ),
						Child, HGroup,
							Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_OPENPRIVATEQUERY_GAD ),
							Child, HVSpace,
						End,
						Child, objs[ GID_REQUESTWHOIS     ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGENERAL_REQUESTWHOIS_GAD, FALSE ),
						Child, HGroup,
							Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_REQUESTWHOIS_GAD ),
							Child, HVSpace,
						End,
					End,
					Child, VSpace(5),
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGENERAL_AUTOMATIC_INFO ),
					Child, ColGroup(2),
						Child, objs[ GID_REJOIN            ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGENERAL_REJOIN_GAD, FALSE ),
						Child, HGroup,
							Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_REJOIN_GAD ),
							Child, HVSpace,
						End,
						Child, objs[ GID_OPENQUERY         ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGENERAL_OPENQUERY_GAD, FALSE ),
						Child, HGroup,
							Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_OPENQUERY_GAD ),
							Child, HVSpace,
						End,
					End,
					Child, VSpace(5),
					Child, ColGroup(2),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_HIGHLIGHTPATTERN_GAD ),
						Child, objs[ GID_HIGHLIGHTPATTERN ] = MUICreateString( MSG_MUICLASS_SETTINGSGENERAL_HIGHLIGHTPATTERN_GAD, HIGHLIGHT_SIZEOF ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_NICKCOMPLETITION_GAD ),
						Child, objs[ GID_NICKCOMPLETITION ] = MUICreateCycle( MSG_MUICLASS_SETTINGSGENERAL_NICKCOMPLETITION_GAD, &TAB_CYCLE_NICKCOMPLETITION, MSG_CY_MIRC, MSG_CY_AMIRC ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_EDITOR_GAD ),
						Child, MUICreatePopASL( MSG_MUICLASS_SETTINGSGENERAL_EDITOR_GAD, EDITOR_SIZEOF, MUII_PopFile, &objs[ GID_EDITOR ], TAG_DONE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGENERAL_BROWSER_GAD ),
						Child, MUICreatePopASL( MSG_MUICLASS_SETTINGSGENERAL_BROWSER_GAD, BROWSER_SIZEOF, MUII_PopFile, &objs[ GID_BROWSER ], TAG_DONE ),
					End,
					Child, HVSpace,
				End,
				//Child, HVSpace,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( obj, MM_SETTINGSGENERAL_RESETTODEFAULTS );

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
		SetAttrs( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr, TAB_CONFIGITEMS[ i ].Default, MUIA_ObjectID, TAB_CONFIGITEMS[ i ].ObjectID, TAG_DONE );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSGENERAL_READCONFIG *msg )
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

/* /// MCC_SettingsGeneral_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsGeneral_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                              : return( OM_New                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSGENERAL_RESETTODEFAULTS  : return( MM_ResetToDefaults         ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSGENERAL_READCONFIG       : return( MM_ReadConfig              ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsGeneral_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsGeneral_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSGENERAL ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsGeneral_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSGENERAL ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsGeneral_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsGeneral_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSGENERAL ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSGENERAL ] );
		appclasses[ CLASSID_SETTINGSGENERAL ] = NULL;
    }
}
/* \\\ */



