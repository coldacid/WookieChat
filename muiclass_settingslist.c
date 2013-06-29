/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingslist.c
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

#include <string.h>
#include <stdio.h>

#include "functions.h"
#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_settingslist.h"

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );
	
	return( (ULONG) DoSuperNew( cl, obj,
									MUIA_Frame               , MUIV_Frame_InputList,
									MUIA_NList_Input         , TRUE,
									TAG_DONE ) );
}
/* \\\ */
/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
	*msg->strings = msg->entry;
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsList_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0,  (APTR) ENTRY(MCC_SettingsList_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsList_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSLIST ] );
		appclasses[ CLASSID_SETTINGSLIST ] = NULL;
    }
}
/* \\\ */




