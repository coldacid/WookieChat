/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowabout.c
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
#include "muiclass_application.h"
#include "muiclass_windowabout.h"
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
	char                   mcc_ArexxPort[ AREXXPORT_SIZEOF + 2 ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( obj = (Object *) DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWABOUT_TITLE ),
			MUIA_Window_ID               , MAKE_ID('A','B','O','U'),
			MUIA_Window_NoMenus	       	 , TRUE,

			WindowContents, VGroup,
#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
						Child, GroupObject, MUIA_FixHeight, 78,
							Child, MUI_NewObject( (char*) MUIC_Dtpic, MUIA_Dtpic_Name, ABOUT_LOGONAME, TAG_DONE ),
						End,
#endif
						Child, TextObject, MUIA_Text_Contents, VERSION_ABOUT, MUIA_Weight, 100, End,
						Child, TextObject,MUIA_Text_Contents, "\n\033cThanks go out to Guru Meditation for past sponsorship!\n\033cAmiga reseller and Linux consulting\n\033chttp://gurumeditation.se/\n", MUIA_Weight, 100, End,
						Child, objs[ GID_AREXXPORT ] = TextObject,	MUIA_Weight, 100, End,
					 End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		return( (ULONG) obj );
    }
	return( (IPTR) NULL );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static IPTR OM_Dispose( struct IClass *cl, Object *obj, Msg msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Set()
**
*/

/*************************************************************************/

static IPTR OM_Set( struct IClass *cl, Object *obj, struct opSet *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct TagItem *tag;
struct TagItem *tstate;

	for( tstate = msg->ops_AttrList ; ( tag = NextTagItem( &tstate ) ) ; ) {
		ULONG tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
			case MA_WINDOWABOUT_AREXXPORT:
				strncpy( mccdata->mcc_ArexxPort, (char *) LGS( MSG_MUICLASS_WINDOWABOUT_AREXXPORT_GAD ), AREXXPORT_SIZEOF );
				strncat( mccdata->mcc_ArexxPort, (char *) tidata, AREXXPORT_SIZEOF );
				SetAttrs( mccdata->mcc_ClassObjects[ GID_AREXXPORT ], MUIA_Text_Contents, mccdata->mcc_ArexxPort, TAG_DONE );
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static IPTR OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_CLASSID: *msg->opg_Storage = CLASSID_WINDOWABOUT ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowAbout_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowAbout_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New      ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                      : return( OM_Dispose  ( cl, obj, (APTR) msg ) );

		case OM_SET                          : return( OM_Set      ( cl, obj, (APTR) msg ) );
		case OM_GET                          : return( OM_Get      ( cl, obj, (APTR) msg ) );

/* application specific methods */

    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowAbout_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowAbout_InitClass( void )
{
	appclasses[ CLASSID_WINDOWABOUT ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowAbout_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWABOUT ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowAbout_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowAbout_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWABOUT ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWABOUT ] );
		appclasses[ CLASSID_WINDOWABOUT ] = NULL;
    }
}
/* \\\ */




