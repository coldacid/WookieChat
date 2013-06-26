/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowdccincoming.c
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

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowdccincoming.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_LIST = 0,
GID_PATH,
GID_ACCEPT,
GID_REJECT,
GID_REMOVE,
GID_LAST
};

/*
** data used by this class
*/
#define DCC_PATH_SIZEOF 0x200
#define LINEBUFFER_SIZEOF 0x2000

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
};

/*************************************************************************/
/* /// display_hook()
*/

  #ifdef __amigaos4__
static void display_func(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct dcc_entry *entry)) {
    #elif __MORPHOS__
	static void display_func(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct dcc_entry *entry)) {
   #elif __AROS__
	static void display_func(struct Hook *hook, char **array,struct dcc_entry *entry) {
  #elif __GNUC__
static void display_func(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct dcc_entry *a1 __asm("a1");        struct dcc_entry *entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

	if(entry)
    {
		array[0]=entry->status;
		array[1]=entry->nick;
		array[2]=entry->filename;
		array[3]=entry->cps;
		array[4]=entry->bytes_transferred;
		array[5]=entry->filesize;
		array[6]=entry->percentage;
		array[7]=entry->timeleft;
	} else {
		array[0]= (char*) LGS( MSG_LV_STATUS );
		array[1]= (char*) LGS( MSG_LV_SENDER );
		array[2]= (char*) LGS( MSG_LV_FILENAME );
        array[3]= (char*)"kB/s";
		array[4]= (char*) LGS( MSG_LV_RECIEVED );
		array[5]= (char*) LGS( MSG_LV_SIZE );
        array[6]= (char*)"%";
		array[7]= (char*) LGS( MSG_LV_TIMEREMAINING );
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
#if 0
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
#endif

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *)DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWDCCINCOMING_TITLE ),
			MUIA_Window_ID               , MAKE_ID('D','C','C','I'),
			MUIA_Window_NoMenus	       	 , TRUE,

			WindowContents, VGroup,
					Child, HGroup,
						Child, Label2( LGS( MSG_MUICLASS_WINDOWDCCINCOMING_DOWNLOADPATH_GAD )),
						Child, objs[ GID_PATH ] = StringObject,
															MUIA_Frame        , MUIV_Frame_String,
															MUIA_String_MaxLen, DCC_PATH_SIZEOF,
															End,
					End,
					Child, NListviewObject, MUIA_NListview_NList, objs[ GID_LIST ] = NListObject,
					        MUIA_Frame, MUIV_Frame_InputList,
							MUIA_NList_ConstructHook, &ConstructDCC_TextHook,
							MUIA_NList_DestructHook, &DestructDCC_TextHook,

							MUIA_NList_DisplayHook, &display_hook,
							//MUIA_NList_DestructHook, &destruct_hook,
							//MUIA_NList_ConstructHook, &construct_hook,
					        MUIA_NList_Title, TRUE,
							MUIA_NList_Format,"WEIGHT=19 BAR,WEIGHT=15 BAR,WEIGHT=55 BAR,WEIGHT=10 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
							End,
					End,
					Child, HGroup,
						Child, objs[ GID_ACCEPT ] = SimpleButton( LGS( MSG_MUICLASS_WINDOWDCCINCOMING_ACCEPT_GAD )),
						Child, objs[ GID_REJECT ] = SimpleButton( LGS( MSG_MUICLASS_WINDOWDCCINCOMING_REJECT_GAD )),
						Child, objs[ GID_REMOVE ] = SimpleButton( LGS( MSG_MUICLASS_WINDOWDCCINCOMING_REMOVE_GAD )),
					End,
				End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		//DoMethod( objs[ GID_LIST       ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj, 1, MM_WINDOWDCCINCOMING_LISTTOGADGETS );
		//DoMethod( objs[ GID_PATTERN    ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_WINDOWDCCINCOMING_GADGETSTOLIST );
		//DoMethod( objs[ GID_PRIVMSG    ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj, 1, MM_WINDOWDCCINCOMING_GADGETSTOLIST );
		//DoMethod( objs[ GID_CTCP       ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj, 1, MM_WINDOWDCCINCOMING_GADGETSTOLIST );
		//DoMethod( objs[ GID_DCC        ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj, 1, MM_WINDOWDCCINCOMING_GADGETSTOLIST );


		//DoMethod( objs[ GID_ADD       ], MUIM_Notify, MUIA_Pressed            , FALSE, obj, 1, MM_WINDOWDCCINCOMING_ADD );
		DoMethod( objs[ GID_REMOVE ], MUIM_Notify, MUIA_Pressed            , FALSE, objs[ GID_LIST ], 2, MUIM_NList_Remove, MUIV_NList_Remove_Selected );

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// MM_Add()
**
*/

/*************************************************************************/

static ULONG MM_Add( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	//DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_InsertSingle, &ignore, MUIV_NList_Insert_Bottom );
	//SetAttrs( mccdata->mcc_ClassObjects[ GID_LIST ], MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );

	return( 0 );
}
/* \\\ */

/* /// MM_DCCAccept()
**
*/

/*************************************************************************/

static ULONG MM_DCCAccept( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
STRPTR path = MUIGetVar( mccdata->mcc_ClassObjects[ GID_PATH ], MUIA_String_Contents );
struct dcc_entry *find = NULL;

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find );
    LONG a = 0;

    dcc_conductor = dcc_root->next;

    if (DEBUG)
        printf("trying to accept\n");

    while (find && dcc_conductor)
    {

        //if(DEBUG) printf("dcc 2\n");

        if (!dcc_conductor->removed)
        {

            //if(DEBUG) printf("dcc 3\n");

            if (find && dcc_conductor->entry)
            {
                if (!stricmp(find->status, dcc_conductor->entry->status)
                        && !stricmp(find->nick, dcc_conductor->entry->nick)
                        && !stricmp(find->filename, dcc_conductor->entry->filename)
                        && !stricmp(find->port, dcc_conductor->entry->port)) //experimental, adding port string to entry struct
                {
                    //if(DEBUG) printf("dcc 4\n");

                    if (dcc_conductor)
                    {

                        if (DEBUG)
                            printf("found the dcc conductor entry\n");
                        //if(dcc_conductor->accepted==TRUE) { printf("aborting accept_dcc()\n"); return; }

                        if (dcc_conductor->entry)
                        {
							strcpy(dcc_conductor->entry->status, (char *) LGS( MSG_CONNECTING ));
                            strcpy(dcc_conductor->entry->percentage, "0%");
                        }
                        //if(DEBUG) printf("dcc 5\n");

                        DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle, dcc_conductor->entry, a,
                                NOWRAP, ALIGN_LEFT);

                        //if(DEBUG) printf("dcc 6\n");

                        //open file, see if it exists, check filesize for resume option
						if (path[strlen(path) - 1] != '/' && path[strlen(path) - 1] != ':')
							sprintf(dcc_conductor->full_filename_with_path, "%s/%s", path, dcc_conductor->filename);
                        else
							sprintf(dcc_conductor->full_filename_with_path, "%s%s", path, dcc_conductor->filename);
                        BPTR examined_file = Open((_s_cs)dcc_conductor->full_filename_with_path, MODE_READWRITE);

						strcpy(dcc_conductor->filepath, path);

						if (path[strlen(path) - 1] != '/' && path[strlen(path) - 1] != ':')
                            strcat(dcc_conductor->filepath, "/");

                        if (!dcc_conductor->completed)
                        {
                            if (DEBUG)
                                printf("examined file for dcc resume:%s\n", dcc_conductor->full_filename_with_path);
                            if (examined_file)
                            {
                                //if(DEBUG) printf("dcc 7\n");

                                struct FileInfoBlock *a_fib;
                                a_fib = (struct FileInfoBlock*) AllocDosObject(DOS_FIB, NULL);

                                if (ExamineFH(examined_file, a_fib))
                                {
                                    if (a_fib->fib_Size > 0)
                                    {
                                        //printf("file already exists\n");
                                        char c[100], d[100], e[1100];
										sprintf(e, "\"%s\" %s", dcc_conductor->full_filename_with_path, LGS( MSG_FILE_ALREADY_EXISTS ) );
										sprintf(c, "%s: %lu bytes", LGS( MSG_INCOMING_FILE_SIZE	), a_fib->fib_Size);
										sprintf(d, "%s: %lu bytes", LGS( MSG_FILE_SIZE_OFFERED ), dcc_conductor->filesize);
                                        setmacro((Object*) WookieChat->TX_oldname, MUIA_Text_Contents, e);
                                        setmacro((Object*) WookieChat->TX_oldsize, MUIA_Text_Contents, c);
                                        setmacro((Object*) WookieChat->TX_newsize, MUIA_Text_Contents, d);

                                        dcc_conductor->total_recv = a_fib->fib_Size;

                                        //if(DEBUG) printf("dcc 8\n");

                                        if (my_settings.dcc_recv_auto_accept_if_exists_action == ASK)
                                            setmacro((Object*) WookieChat->WI_dcc_file_exists, MUIA_Window_Open, TRUE);

                                        else if (my_settings.dcc_recv_auto_accept_if_exists_action == OVERWRITE)
                                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 71);

                                        else if (my_settings.dcc_recv_auto_accept_if_exists_action == RESUME)
                                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 70);

                                        else if (my_settings.dcc_recv_auto_accept_if_exists_action == ABORT)
                                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 66);

                                        else if (my_settings.dcc_recv_auto_accept_if_exists_action == RENAME)
                                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 60);

                                        else
                                            setmacro((Object*) WookieChat->WI_dcc_file_exists, MUIA_Window_Open, TRUE);

                                        //if(DEBUG) printf("dcc 9\n");

                                    }
                                    else
                                    {
                                        //dcc_conductor->accepted=TRUE;
                                        if (DEBUG)
                                            printf("connect() attempted on dcc socket..\n");

                                        connect(dcc_conductor->dcc_socket, (struct sockaddr*) &dcc_conductor->test,
                                                sizeof(struct sockaddr));
                                        //if(DEBUG) printf("dcc 10\n");
                                    }

                                }
                                Close(examined_file);
                                FreeDosObject(DOS_FIB, a_fib);
                            }
                        }

                        return;
                        break;

                    }
                }

            }

            a++;
        }

        dcc_conductor = dcc_conductor->next;

    }

    //if(DEBUG) printf("dcc 11\n");
    if (DEBUG)
        printf("\n");

}
/* \\\ */

/* /// DCC_ReceiveCloseSocket()
**
*/

/*************************************************************************/

void DCC_ReceiveCloseSocket( struct dcc *dcc )
{
	if (dcc->dcc_socket != -1 ) {
		if( FD_ISSET( dcc->dcc_socket, &read_master ) ) {
			shutdown( (int) dcc->dcc_socket, (int) 2 );
		}
		if( FD_ISSET( dcc->dcc_socket, &read_master ) ) {
			FD_CLR( dcc->dcc_socket, &read_master );
		}
		if( FD_ISSET( dcc->dcc_socket, &write_master ) ) {
			FD_CLR( dcc->dcc_socket, &write_master);
		}
		dcc->dcc_socket = -1;

		if( dcc->dcc_file ) {
			Close( dcc->dcc_file );
			dcc->dcc_file = 0;
        }
    }
}
/* \\\ */
/* /// DCC_SendCloseSocket()
**
*/

/*************************************************************************/

void DCC_SendCloseSocket( struct dcc *dcc )
{
	if( dcc->dcc_listen_socket != -1)
    {
		if( FD_ISSET( dcc->dcc_listen_socket, &read_master ) ) {
			shutdown((int) dcc->dcc_listen_socket, (int) 2);
		}
		if( FD_ISSET( dcc->dcc_listen_socket, &read_master ) ) {
			FD_CLR( dcc->dcc_listen_socket, &read_master );
		}
		if( FD_ISSET(dcc->dcc_listen_socket, &write_master ) ) {
			FD_CLR( dcc->dcc_listen_socket, &write_master );
		}
		dcc->dcc_listen_socket = -1;
    }

	if( dcc->dcc_socket != -1 ) {
		if( FD_ISSET( dcc->dcc_socket, &read_master ) ) {
			shutdown( (int) dcc->dcc_socket, (int) 2);
		}
		if( FD_ISSET( dcc->dcc_socket, &read_master ) ) {
			FD_CLR( dcc->dcc_socket, &read_master );
		}
		if( FD_ISSET( dcc->dcc_socket, &write_master ) ) {
			FD_CLR( dcc->dcc_socket, &write_master );
		}
		dcc->dcc_socket = -1;
    }
}
/* \\\ */

/* /// MM_CreateReceiveDCC()
**
*/

/*************************************************************************/

static ULONG DCC_CreateReceiveDCC( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct dcc *dcc_work;
ULONG result = 0;

	if( ( dcc = AllocVec( sizeof( struct dcc ), MEMF_ANY|MEMF_CLEAR ) ) ) {
		if( ( dcc->entry = AllocVec( sizeof( struct dcc_entry ), MEMF_ANY|MEMF_CLEAR ) ) ) {

		//if (!strcmp(nick, "0") && !strcmp(filename, "0"))
		//	  return; /* WTF */

			dcc->total_recv = 0;
			dcc->connected  = 0;
			dcc->dcc_file   = 0;
			dcc->removed    = 0;
			dcc->accepted   = FALSE;
			dcc->completed  = 0;
			dcc->cancelled  = 0;

			// This is the Entry position for when we edit the DCC NLIST Info ENTRY
			//dcc->pos        = dcc_transfers;

			strcpy( dcc->networkname, status_conductor->networkname );
			strcpy( dcc->address, msg->Address );
			sprintf( dcc->port  , "%i", msg->Port );
			if( msg->Nick ) {
				strcpy( dcc->nick, nick );
			}
			if( msg->Filename ) {
				strcpy(dcc->filename, msg->Filename );
			}
			if( msg->Filesize ) {
				dcc->filesize = atoi( msg->Filesize );
			}
			strcpy( dcc->full_filename_with_path, "" );


			strcpy( dcc->entry->port, dcc_conductor->port );
			strcpy( dcc->entry->nick, ( msg->Nick ? msg->Nick : "Unknown" ) );
			strcpy( dcc->entry->filename, ( msg->Filename ? msg->Filename : "Unknown" ) );
			if( msg->Filesize ) {
				sprintf( dcc->entry->filesize, "%dkB", atoi(msg->Filesize) / 1024);
			}
			strcpy( dcc->entry->status, (char *) LGS( MSG_DCC_WAITING ) );
			strcpy( dcc->entry->percentage, "0%" );
			strcpy( dcc->entry->bytes_transferred, "0" );
			strcpy( dcc->entry->cps, "0" );
			strcpy( dcc->entry->timeleft, "" );

			dcc->dcc_socket = socket(AF_INET, SOCK_STREAM, 0);

			if( fdmax < dcc->dcc_socket ) {
				fdmax = dcc->dcc_socket;
			}

			FD_SET( dcc->dcc_socket, &read_master);

		    struct in_addr test2;
		#ifdef __amigaos4__
			test2.s_addr = inet_addr( msg->Address );
		#elif __AROS__
			test2.s_addr = inet_addr( msg->Address );
		#elif __MORPHOS__
			test2.s_addr = inet_addr((UBYTE*) msg->Address );
		#else
			test2.s_addr = inet_addr((u_char*) msg->Address );
		#endif

			dcc->test.sin_addr.s_addr = test2.s_addr;
			dcc->test.sin_port = msg->Port;
			dcc->test.sin_family = AF_INET;
			memset(&(dcc->test.sin_zero), '\0', 8);

		    long i = 1;

			IoctlSocket(dcc->dcc_socket, FIONBIO, (char*) &i );

			DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_InsertSingle, dcc->entry, MUIV_NList_Insert_Bottom );

			IoctlSocket(dcc->dcc_socket, FIOASYNC, (char*) &i );

			if (my_settings.dcc_recv_auto_accept) {
				DoMethod( obj, MM_WINDOWDCCINCOMING_ACCEPT );
			}
		} else {
			FreeVec( dcc );
		}
	}
	return( result );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowDCCIncoming_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowDCCIncoming_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                                : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MM_WINDOWDCCINCOMING_ADD              : return( MM_Add                           ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowDCCIncoming_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowDCCIncoming_InitClass( void )
{
	appclasses[ CLASSID_WINDOWDCCINCOMING ] = MUI_CreateCustomClass( NULL, MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowDCCIncoming_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWDCCINCOMING ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowDCCIncoming_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowDCCIncoming_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWDCCINCOMING ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWDCCINCOMING ] );
		appclasses[ CLASSID_WINDOWDCCINCOMING ] = NULL;
    }
}
/* \\\ */

