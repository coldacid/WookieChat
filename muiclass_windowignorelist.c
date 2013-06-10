/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowignorelist.c
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

#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowignorelist.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

#define DEFAULT_IGNORELIST_NAME "PROGDIR:ignorelist.txt"


/*
** gadgets used by this class
*/

enum
{
GID_LIST = 0,
GID_PATTERN,
GID_PRIVMSG,
GID_CTCP,
GID_DCC,
GID_ADD,
GID_REMOVE,
GID_LAST
};

/*
** data used by this class
*/

#define LINEBUFFER_SIZEOF 0x2000

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
};

/*************************************************************************/

/* /// display_hook()
*/

  #ifdef __amigaos4__
static void display_func(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct Ignore *entry)) {
#elif __MORPHOS__
static void display_func(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct Ignore *entry)) {
#elif __AROS__
static void display_func(struct Hook *hook, char **array, struct Ignore *entry) {
  #elif __GNUC__
static void display_func(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct Ignore *a1 __asm("a1");                struct Ignore *entry = a1;
  register struct Hook *a0 __asm("a0");                struct Hook *hook = a0;
#endif

	if( entry ) {
		array[0] = entry->Ignore_Pattern;
		array[1] = (char*) (( entry->Ignore_Flags & IGNOREF_PRIVMSG ) ? LGS( MSG_LV_IGNORE ) : LGS( MSG_LV_ALLOW ) );
		array[2] = (char*) (( entry->Ignore_Flags & IGNOREF_CTCP    ) ? LGS( MSG_LV_IGNORE ) : LGS( MSG_LV_ALLOW ) );
		array[3] = (char*) (( entry->Ignore_Flags & IGNOREF_DCC     ) ? LGS( MSG_LV_IGNORE ) : LGS( MSG_LV_ALLOW ) );
	} else {
		array[0] = (char*) LGS( MSG_LV_PATTERN );
		array[1] = (char*) LGS( MSG_LV_TEXT );
		array[2] = (char*) LGS( MSG_LV_CTCP );
		array[3] = (char*) LGS( MSG_LV_DCC  );
    }
}

/* hook define */
#ifdef __MORPHOS__
static struct Hook display_hook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC) display_func, NULL };
#elif __AROS__
static struct Hook display_hook = { { NULL,NULL },(IPTR(*)()) display_func, NULL,NULL };
#else
static struct Hook display_hook = { { NULL,NULL },(ULONG(*)()) display_func, NULL,NULL };
#endif
/* \\\ */
/* /// destruct_hook()
*/

  #ifdef __amigaos4__
static void destruct_func(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct Ignore *entry)) {
#elif __MORPHOS__
static void destruct_func(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct Ignore *entry)) {
#elif __AROS__
static void destruct_func(struct Hook *hook, char **array, struct Ignore *entry) {
  #elif __GNUC__
static void destruct_func(void)
{
  register char **a2 __asm("a2");                      char **array = a2;
  register struct Ignore *a1 __asm("a1");              struct Ignore *entry = a1;
  register struct Hook *a0 __asm("a0");                struct Hook *hook = a0;
#endif

	if( entry ) {
		FreeVec( entry );
	}
}

/* hook define */
#ifdef __MORPHOS__
static struct Hook destruct_hook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC) destruct_func, NULL };
#elif __AROS__
static struct Hook destruct_hook = { { NULL,NULL },(IPTR(*)()) destruct_func, NULL,NULL };
#else
static struct Hook destruct_hook = { { NULL,NULL },(ULONG(*)()) destruct_func, NULL,NULL };
#endif
/* \\\ */
/* /// construct_hook()
*/

  #ifdef __amigaos4__
static APTR construct_func(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct Ignore *entry)) {
#elif __MORPHOS__
static APTR construct_func(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct Ignore *entry)) {
#elif __AROS__
static APTR construct_func(struct Hook *hook, char **array, struct Ignore *entry) {
  #elif __GNUC__
static APTR construct_func(void)
{
  register char **a2 __asm("a2");                      char **array = a2;
  register struct Ignore *a1 __asm("a1");              struct Ignore *entry = a1;
  register struct Hook *a0 __asm("a0");                struct Hook *hook = a0;
#endif
	struct Ignore *nentry;

	if( ( nentry = AllocVec( sizeof( struct Ignore ), MEMF_ANY ) ) ) {
		CopyMem( entry, nentry, sizeof( struct Ignore ) );
	}
	return( nentry );
}

/* hook define */
#ifdef __MORPHOS__
static struct Hook construct_hook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC) construct_func, NULL };
#elif __AROS__
static struct Hook construct_hook = { { NULL,NULL },(IPTR(*)()) construct_func, NULL,NULL };
#else
static struct Hook construct_hook = { { NULL,NULL },(ULONG(*)()) construct_func, NULL,NULL };
#endif
/* \\\ */

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	if( (obj = (Object *)DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWIGNORELIST_TITLE ),
			MUIA_Window_ID               , MAKE_ID('I','G','N','O'),
			MUIA_Window_NoMenus	       	 , TRUE,

			WindowContents, VGroup,
					Child, NListviewObject, MUIA_NListview_NList, objs[ GID_LIST ] = NListObject,
					        MUIA_Frame, MUIV_Frame_InputList,
							MUIA_NList_DisplayHook, &display_hook,
							MUIA_NList_DestructHook, &destruct_hook,
							MUIA_NList_ConstructHook, &construct_hook,
					        MUIA_NList_AutoCopyToClip, TRUE,
					        MUIA_NList_Input,TRUE,
					        MUIA_NList_Title, TRUE,
					        MUIA_NList_Format,"COL=0 WEIGHT=80 BAR, COL=1 WEIGHT=-1 BAR, COL=2 WEIGHT=-1 BAR, COL=3 WEIGHT=-1 BAR",
					        MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted,
					        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
							End,
					End,
					Child, HGroup,
						Child, objs[ GID_PATTERN ] = StringObject,
															MUIA_Frame        , MUIV_Frame_String,
															MUIA_String_Reject, " ",
															MUIA_String_MaxLen, IGNORE_PATTERN_SIZEOF,
															End,
						Child, objs[ GID_PRIVMSG ] = CheckMark(TRUE),
						Child, Label2( LGS( MSG_MUICLASS_WINDOWIGNORELIST_PRIVMSG_GAD )),
						Child, objs[ GID_CTCP    ] = CheckMark(TRUE),
						Child, Label2( LGS( MSG_MUICLASS_WINDOWIGNORELIST_CTCP_GAD )),
						Child, objs[ GID_DCC     ] = CheckMark(TRUE),
						Child, Label2( LGS( MSG_MUICLASS_WINDOWIGNORELIST_DCC_GAD )),
					End,
					Child, TextObject, MUIA_Text_PreParse, NULL,
									MUIA_Text_Contents, LGS( MSG_IGNORE_LONG_DETAILED_HELP ),
									MUIA_InnerLeft, 0,
									MUIA_InnerRight, 0,
									End,
					Child, HGroup,
						Child, objs[ GID_ADD    ] = SimpleButton( LGS( MSG_MUICLASS_WINDOWIGNORELIST_ADD_GAD    )),
						Child, objs[ GID_REMOVE ] = SimpleButton( LGS( MSG_MUICLASS_WINDOWIGNORELIST_REMOVE_GAD )),
					End,
				End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );
		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 1, MM_WINDOWIGNORELIST_SAVELIST );

		DoMethod( objs[ GID_LIST       ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj, 1, MM_WINDOWIGNORELIST_LISTTOGADGETS );
		DoMethod( objs[ GID_PATTERN    ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_WINDOWIGNORELIST_GADGETSTOLIST );
		DoMethod( objs[ GID_PRIVMSG    ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj, 1, MM_WINDOWIGNORELIST_GADGETSTOLIST );
		DoMethod( objs[ GID_CTCP       ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj, 1, MM_WINDOWIGNORELIST_GADGETSTOLIST );
		DoMethod( objs[ GID_DCC        ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj, 1, MM_WINDOWIGNORELIST_GADGETSTOLIST );


		DoMethod( objs[ GID_ADD       ], MUIM_Notify, MUIA_Pressed            , FALSE, obj, 1, MM_WINDOWIGNORELIST_ADD );
		DoMethod( objs[ GID_REMOVE    ], MUIM_Notify, MUIA_Pressed            , FALSE, objs[ GID_LIST ], 2, MUIM_NList_Remove, MUIV_NList_Remove_Selected );

		DoMethod( obj, MM_WINDOWIGNORELIST_LOADLIST );

		DoMethod( obj, MM_WINDOWIGNORELIST_LISTTOGADGETS );

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// MM_DisEnable()
**
*/

/*************************************************************************/

static ULONG MM_DisEnable( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG pos;
BOOL disabled = TRUE;

	GetAttr( MUIA_NList_Active, mccdata->mcc_ClassObjects[ GID_LIST ], (ULONG*) &pos );
	if( pos >= 0 ) {
		disabled = FALSE;
	}

	SetAttrs( mccdata->mcc_ClassObjects[ GID_PATTERN ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_PRIVMSG ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_CTCP    ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_DCC     ], MUIA_Disabled, disabled, TAG_DONE );
	
	return( 0 );
}
/* \\\ */

/* /// MM_GadgetsToList()
**
*/

/*************************************************************************/

static ULONG MM_GadgetsToList( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Ignore *ignore = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ignore );

	if( ignore ) {

		STRPTR val;
		if( ( val = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_PATTERN ], MUIA_String_Contents ) ) ) {
			strcpy( ignore->Ignore_Pattern, (char *) val );
		}

		ignore->Ignore_Flags  = ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_PRIVMSG ], MUIA_Selected ) ) ? IGNOREF_PRIVMSG : 0;
		ignore->Ignore_Flags |= ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_CTCP    ], MUIA_Selected ) ) ? IGNOREF_CTCP    : 0;
		ignore->Ignore_Flags |= ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_DCC     ], MUIA_Selected ) ) ? IGNOREF_DCC     : 0;

		DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ListToGadgets()
**
*/

/*************************************************************************/

static ULONG MM_ListToGadgets( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Ignore *ignore = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ignore );

	if( ignore ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_PATTERN ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ignore->Ignore_Pattern, TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_PRIVMSG ], MUIA_NoNotify, TRUE, MUIA_Selected       , ( ( ignore->Ignore_Flags & IGNOREF_PRIVMSG ) ? TRUE : FALSE ), TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_CTCP    ], MUIA_NoNotify, TRUE, MUIA_Selected       , ( ( ignore->Ignore_Flags & IGNOREF_CTCP    ) ? TRUE : FALSE ), TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_DCC     ], MUIA_NoNotify, TRUE, MUIA_Selected       , ( ( ignore->Ignore_Flags & IGNOREF_DCC     ) ? TRUE : FALSE ), TAG_DONE );
	}

	DoMethod( obj, MM_WINDOWIGNORELIST_DISENABLE );

	return( 0 );
}
/* \\\ */

/* /// MM_SaveList()
**
*/

/*************************************************************************/

static ULONG MM_SaveList( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
BPTR handle;
ULONG i;
struct Ignore *ignore;

	if( ( handle = Open( (_ub_cs) DEFAULT_IGNORELIST_NAME, MODE_NEWFILE ) ) ) {

		for( i = 0 ;  ; i++ ) {
			ignore = NULL;
			DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_GetEntry, i, &ignore );
			if( ignore ) {
				FPuts( handle, (CONST_STRPTR)   ignore->Ignore_Pattern );
				FPuts( handle, (CONST_STRPTR) ( ( ignore->Ignore_Flags & IGNOREF_PRIVMSG ) ? " ignore" : " allow" ) );
				FPuts( handle, (CONST_STRPTR) ( ( ignore->Ignore_Flags & IGNOREF_CTCP    ) ? " ignore" : " allow" ) );
				FPuts( handle, (CONST_STRPTR) ( ( ignore->Ignore_Flags & IGNOREF_DCC     ) ? " ignore" : " allow" ) );
				FPutC( handle,        '\n'  );
			} else {
	            break;
			}
		}
		Close( handle );
    }
	return( 0 );
}
/* \\\ */
/* /// MM_LoadList()
**
*/


/*************************************************************************/

static ULONG MM_LoadList( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Ignore ignore;
BPTR handle;
LONG i, length;
char *linebuffer, *text;

	DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_Clear );

	if( ( linebuffer = AllocVec( LINEBUFFER_SIZEOF, MEMF_ANY ) ) ) {

		if( ( handle = Open( (_ub_cs) DEFAULT_IGNORELIST_NAME, MODE_OLDFILE ) ) ) {

			while( FGets( handle, (STRPTR) linebuffer, LINEBUFFER_SIZEOF - 1 ) ) {
				length = strlen( linebuffer );
				for( i = 0; i < length ; i++ ) {
					linebuffer[ i ] = ToLower( linebuffer[ i ] );
				}

				ignore.Ignore_Flags = 0;
				text = linebuffer;
				if( ( text = strchr( text, 0x20 ) ) ) {
					*text++ = 0x00;
					strncpy( ignore.Ignore_Pattern, linebuffer, IGNORE_PATTERN_SIZEOF );
					if( !Strnicmp( (_ub_cs) text, (_ub_cs) "ignore", 6 ) ) {
						ignore.Ignore_Flags |= IGNOREF_PRIVMSG;
					}
					if( ( text = strchr( text, 0x20 ) ) ) {
						text++;
						if( !Strnicmp( (_ub_cs) text, (_ub_cs) "ignore", 6 ) ) {
							ignore.Ignore_Flags |= IGNOREF_CTCP;
						}
						if( ( text = strchr( text, 0x20 ) ) ) {
							text++;
							if( !Strnicmp( (_ub_cs) text, (_ub_cs) "ignore", 6 ) ) {
								ignore.Ignore_Flags |= IGNOREF_DCC;
							}
							DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_InsertSingle, &ignore, MUIV_NList_Insert_Bottom );
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

/* /// MM_Add()
**
*/

/*************************************************************************/

static ULONG MM_Add( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Ignore ignore;

	strcpy( &ignore.Ignore_Pattern[0], "<mask>" );
	ignore.Ignore_Flags = 1;

	DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_InsertSingle, &ignore, MUIV_NList_Insert_Bottom );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_LIST ], MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );

	return( 0 );
}
/* \\\ */
/* /// MM_CheckIgnore()
**
*/

/*************************************************************************/

static ULONG MM_CheckIgnore( struct IClass *cl, Object *obj, struct MP_WINDOWIGNORELIST_CHECKIGNORE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Ignore *ignore;
char pattern[ IGNORE_PATTERN_SIZEOF * 3 ];
char *nick;
ULONG i, length, result = FALSE;

	length = strlen( (char *) msg->Nick ) + strlen( (char *) msg->HostName ) + 2;

	if( ( nick = AllocVec( length, MEMF_ANY ) ) ) {

		sprintf( nick, "%s!%s", msg->Nick, msg->HostName );

		for( i = 0 ; ; i++ ) {
			ignore = NULL;
			DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_GetEntry, i, &ignore );
			if( ignore ) {
				ParsePatternNoCase( (_ub_cs) ignore->Ignore_Pattern, (_ub_cs)  pattern, IGNORE_PATTERN_SIZEOF * 3 );
				if( MatchPatternNoCase( (_ub_cs) pattern, (_ub_cs) nick ) ) {
					/* match */
					switch( msg->Type ) {
						case PRIVMSG:
							if( ignore->Ignore_Flags & IGNOREF_PRIVMSG ) {
								result = TRUE;
							}
							break;
						case CTCP:
							if( ignore->Ignore_Flags & IGNOREF_CTCP ) {
								result = TRUE;
							}
							break;
						case DCC:
							if( ignore->Ignore_Flags & IGNOREF_DCC ) {
								result = TRUE;
							}
							break;
					}
					break;
				}
			} else {
				break;
			}
		}
		FreeVec( nick );
	}
	return( result );
}
/* \\\ */
 
/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowIgnoreList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowIgnoreList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                                : return( OM_New                           ( cl, obj, (APTR) msg ) );

		case MM_WINDOWIGNORELIST_DISENABLE         : return( MM_DisEnable                     ( cl, obj, (APTR) msg ) );
		case MM_WINDOWIGNORELIST_GADGETSTOLIST     : return( MM_GadgetsToList                 ( cl, obj, (APTR) msg ) );
		case MM_WINDOWIGNORELIST_LISTTOGADGETS     : return( MM_ListToGadgets                 ( cl, obj, (APTR) msg ) );
		case MM_WINDOWIGNORELIST_LOADLIST          : return( MM_LoadList                      ( cl, obj, (APTR) msg ) );
		case MM_WINDOWIGNORELIST_SAVELIST          : return( MM_SaveList                      ( cl, obj, (APTR) msg ) );
		case MM_WINDOWIGNORELIST_ADD               : return( MM_Add                           ( cl, obj, (APTR) msg ) );
		case MM_WINDOWIGNORELIST_CHECKIGNORE       : return( MM_CheckIgnore                   ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowIgnoreList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowIgnoreList_InitClass( void )
{
	appclasses[ CLASSID_WINDOWIGNORELIST ] = MUI_CreateCustomClass( NULL, MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowIgnoreList_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWIGNORELIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowIgnoreList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowIgnoreList_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWIGNORELIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWIGNORELIST ] );
		appclasses[ CLASSID_WINDOWIGNORELIST ] = NULL;
    }
}
/* \\\ */

