/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

/* =================================================================== 
                               Includes
   =================================================================== */

//fixed width size



#define  __USE_OLD_TIMEVAL__ 1

#include "intern.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <exec/lists.h>
#include <devices/timer.h>

#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <intuition/intuition.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>
#include <exec/memory.h>
#include <proto/datatypes.h>
#include <datatypes/soundclass.h>

int stccpy(char *p, const char *q, int n)
{
   char *t = p;
   while ((*p++ = *q++) && --n > 0);
   p[-1] = '\0';
   return p - t;
}

#ifdef __amigaos4__

#include <unistd.h>
#include <intuition/bitmapshare.h>
#include <clib/alib_protos.h>
#include <proto/codesets.h>
//#include <proto/rexxsyslib.h>
#ifndef   EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef   REXX_RXSLIB_H
#include <rexx/rxslib.h>
#endif
#ifndef   REXX_REXXIO_H
#include <rexx/rexxio.h>
#endif

#include <proto/rexxsyslib.h>

struct SocketIFace * ISocket;
Library * RexxSysBase = NULL;
RexxSysIFace *IRexxSys;
struct CodesetsIFace  *ICodesets = NULL;
struct IntuitionIFace * IIntuition = NULL;
struct LocaleIFace * ILocale = NULL;
struct UtilityIFace * IUtility = NULL;
struct DataTypesIFace * IDataTypes = NULL;
struct IconIFace * IIcon = NULL;
struct AslIFace * IAsl = NULL;

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/icon.h>

     // && select_struct.vnum==clipboard_struct.pos+1)
#include <proto/timer.h>
typedef UBYTE *a_in;

#include "os4.h"

#elif __MORPHOS__
extern "C"
{
#include <proto/asl.h>
#include <proto/graphics.h>
#include <proto/alib.h>
#define __STRICT_ANSI__ 1
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/locale.h>
#include <proto/icon.h>
#include <proto/datatypes.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/codesets.h>

//#include <clib/codesets_protos.h>
//#include <libraries/codesets.h>


/*#include <exec/types.h>
#include <utility/tagitem.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/filio.h>
*/
}

//#include <proto/bsdsocket.h>
/*
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/filio.h>
*/

#include <dos/dostags.h>


typedef UBYTE *a_in;


#elif __AROS__

#define MUI_OBSOLETE

extern "C"
{
#include <clib/alib_protos.h>

#include <proto/datatypes.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/icon.h>
#include <proto/codesets.h>
#include <proto/muimaster.h>
#include <proto/socket.h>
#include <proto/codesets.h>

#include <dos/dostags.h>

#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bsdsocket/socketbasetags.h>
#include <clib/socket_protos.h>
}


typedef UBYTE *a_in;


#else
#include <proto/codesets.h>
#include <clib/asl_protos.h>
#include <clib/graphics_protos.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/utility_protos.h>
#include <clib/dos_protos.h>
#include <clib/locale_protos.h>
#include <clib/icon_protos.h>
#include <inline/alib.h>
#include <inline/muimaster.h>
#include <dos/exall.h>
#include <dos/dostags.h>



typedef UBYTE *a_in;


extern "C"
{

}

#endif

#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/BetterString_mcc.h>

#ifdef __amigaos4__
#include <MUI/NBitmap_mcc.h>
#endif




//automatically reconnect options


#define QUEUED_MESSAGES_DELAY_IN_SECONDS 0
#define QUEUED_MESSAGES_DELAY_IN_MICROSECONDS 750000

#define LEFT 0
#define RIGHT 1
 

#define SMALLSTRING_BUFFERSIZE 100




#define STDIN 0
#define SIGNAL_TIMER 16

// prototypes
int do_waitselect_code(void);
BOOL FindUTF8Chars(char*);
void remove_tab_listview2(void);
void add_tabs_to_nicklist_group(void);

int create_new_query_window(char*,int);
int compare_func(char*,char*);
void LoadAllLibs(void);
void CloseQueryWindow(void);









struct MsgPort *app_process_port;


struct SignalSemaphore *WookieChat_semaphore;



//APTR BT_blank;
APTR pen;
BOOL user_ignore_privmsg, user_ignore_ctcp, user_ignore_dcc;
BPTR arexx_dir_lock;
struct codeset *local_charsets_list[45];

char *PreParse_NewText=(char*)"\033c\0333";
char *text2;
char background[64];
char pen_number[10];
char new_array2[900];
char list_channels_work[900];
char wscreent[200];




int a_socket=0;
int last_a=0;
int last_c=0;
int previous_b=0;
int socket_global=0;
int sort_method=1; //which sorting method is used for nick lists - only 1 presently

long ev_sock;


struct MUI_PenSpec *startpen;
struct sockaddr addr_buffer, addr_buffer2;
struct sockaddr_in slapshot_in;
ULONG ev_type;
ULONG portsig, portsig2, portsig3, portsig4;
ULONG socksigbit,dccsigbit,reconnect_delay_timer_signal;
STRPTR work_incoming2;

ULONG test, test2;
enum { NEVER=0, ALWAYS, WHEN_WIN_ACTIVE };

char *ignore1,*ignore2,*ignore3,*ignore4,*ignore5,*ignore6,*ignore7,*ignore8,*ignore9,*ignore10;








// below are some variables for working with NextObject() loops




struct MUI_CustomClass *NLI_Class;

#include "version_info.h"





char num_to_string[100];

    struct test_tree
    {
        struct MUI_NListtree_TreeNode *t;
        struct test_tree *next;

    } *tree_conductor, *tree_root;

int total_servers;
char last_group[50];













struct MUI_NList_GetSelectInfo *res=new struct MUI_NList_GetSelectInfo;

struct channel_entry *wentry=new channel_entry;




#ifdef __AROS__
IPTR NewObjectAROS( struct IClass *classPtr, UBYTE *classID, ULONG tag1, ... )
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = (IPTR) NewObject(classPtr, classID, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
}

IPTR DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1, ...)
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = (IPTR)DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
}
#endif

#ifdef __AROS__
#define BS_Object NewObjectAROS(mcc->mcc_Class,NULL //nlist subclass#else
#else
#define BS_Object NewObject(mcc->mcc_Class,NULL
#endif


char clipboard_string[5000];
char *buffer4;
char string20[800];
char string21[800];
int last_count, last_count2;

struct MUI_NList_GetEntryInfo clipboard_struct2;

int  col1,col2,pos1,pos2;
int was_last_one_four;


int lastvstart;
int lastvend;

// timer device variables

struct Message *TimerMSG;
//struct MsgPort *xyreplyport;


struct Message *Timer2MSG;




//struct timerequest *Timer3IO;
//struct MsgPort *Timer3MP;
//struct MsgPort *xyreplyport2;










//ctcp ping variables

//dcc time variables
char dcctimestamp_secs[1000];

int pingcount=0;

struct pingstruct
{
  int code;
  char time;
}pings;

int change_list=0;
ULONG *selected_pointer;

char buffer[BUFFERSIZE];

ULONG socket_signal;
ULONG dcc_signal;

long own_address_socket;
struct sockaddr_in own_addr;

//int lastentrynum, lastpos1, lastpos2, vcount=1,lastentry_pos=-1;
int vcount=0
,lastentry_pos=-1;
//char lastentry[800];
int clip_count=1;
//int vcount=0;












long dcc_socket;
long dcc_listen_socket;
struct sockaddr_in their_addr;

struct List temp_list;

int string_history_number;

int string_id=2;

#define GAD_STRING 1;

/* GUI */



        









    APTR    PenMasterObject;







    APTR source, instance;
    APTR source2, instance2;
    struct BitMap *bitmap2, *bitmap, *bitmap3;
    ULONG width, height;





    APTR STR_events_highlight;
    APTR STR_events_highlight2;
    APTR CH_events_highlight;
    APTR CH_events_highlight2;

    APTR STR_events_connect;
    APTR STR_events_connect2;
    APTR CH_events_connect;
    APTR CH_events_connect2;

    APTR STR_events_disconnect;
    APTR STR_events_disconnect2;
    APTR CH_events_disconnect;
    APTR CH_events_disconnect2;

    APTR STR_events_join;
    APTR STR_events_join2;
    APTR CH_events_join;
    APTR CH_events_join2;

    APTR STR_events_part;
    APTR STR_events_part2;
    APTR CH_events_part;
    APTR CH_events_part2;

    APTR STR_events_quit;
    APTR STR_events_quit2;
    APTR CH_events_quit;
    APTR CH_events_quit2;

    APTR STR_events_dcc_send_finished;
    APTR STR_events_dcc_send_finished2;
    APTR CH_events_dcc_send_finished;
    APTR CH_events_dcc_send_finished2;

    APTR STR_events_dcc_recv_offered;
    APTR STR_events_dcc_recv_offered2;
    APTR CH_events_dcc_recv_offered;
    APTR CH_events_dcc_recv_offered2;

    APTR STR_events_dcc_recv_finished;
    APTR STR_events_dcc_recv_finished2;
    APTR CH_events_dcc_recv_finished;
    APTR CH_events_dcc_recv_finished2;


//    struct event_settings dcc_send_finished;
//    struct event_settings dcc_send_offered;
//    struct event_settings dcc_recv_finished;

#include "objapp.h"



