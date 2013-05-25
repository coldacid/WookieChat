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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <devices/timer.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
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


#ifdef __amigaos4__
#include <unistd.h>
#include <intuition/bitmapshare.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/filio.h>
#include <arpa/inet.h>
#include <proto/bsdsocket.h>
#include <proto/timer.h>

typedef char *p_in;
typedef char *l_in;
typedef char *i_in;

typedef UBYTE *a_in;
typedef char *b_in;
typedef char *c_in;

#else

#include <clib/asl_protos.h>
#include <clib/graphics_protos.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/utility_protos.h>
#include <clib/dos_protos.h>


typedef char *p_in;
typedef char *l_in;
typedef unsigned char *i_in;
typedef UBYTE *a_in;
typedef char *b_in;
typedef UBYTE *c_in;


#endif

#include <MUI/NListview_mcc.h>
#include <MUI/NListtree_mcc.h>
#include <MUI/BetterString_mcc.h>

#ifndef __amigaos4__
#include <inline/alib.h>
#include <inline/muimaster.h>
#endif


#include <SDI_compiler.h>
#include <SDI_hook.h>

#define MUIA_Window_DisableKeys 0x80424c36 /* V15 isg ULONG */ /* private */
#define STRING_BUFFERSIZE 800
#define SMALLSTRING_BUFFERSIZE 100
#define HOSTNAME_STRING_SIZE 85
#define MUIM_CustomBackfill  0x80428d73 // muiundoc.h stuff to do undocumented MUI stuff
#define MUIA_CustomBackfill  0x80420a63 // muiundoc.h stuff to do undocumented MUI stuff
#define set(obj,attr,value) SetAttrs(obj,attr,value,TAG_DONE)
#define get(obj,attr,store) GetAttr(attr,obj,(ULONG *)store)
#define DCC_RECV_BUFFERSIZE 10000
#define BUFFERSIZE 4000
#define STDIN 0
#define SIGNAL_TIMER 16
#define listview_format "COL=2, COL=1 WEIGHT=-1 BAR, COL=0 WEIGHT=-1"
#define OVERWRITE 0
#define RESUME 1
#define ABORT 2
#define ASK 3
#define ACTIVITY 1
#define ACTIVITY_CHAT 2
#define ACTIVITY_HIGHLIGHT 3

Library * GfxBase = NULL;
Library * UtilityBase = NULL;
Library * MUIMasterBase = NULL;
Library * IntuitionBase = NULL;
Library * SocketBase = NULL;

#ifdef __amigaos4__
#include "os4.h"

#elif __GNUC__

//bottom code is experimental work for e-uae on os4
//#include <sys/clib2_net.h>
/*#include <sys/types.h>
#include <sys/socket.h>
#include <sys/filio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
*/
/*extern "C"
{
#include "gg:netinclude/sys/types.h"
#include "gg:netinclude/netdb.h"
//#include "gg:NETINCLUDE/SYS/FILIO.H"
#include "gg:netinclude/bsdsocket.h"
#include "gg:netinclude/sys/ioctl.h"
#include "gg:netinclude/sys/socket.h"
//#include "gg:NETINCLUDE/BSDSOCKET/SOCKETBASETAGS.H"
#include "gg:netinclude/clib/socket_protos.h"

}*/

extern "C"
{
#include "gg:NETINCLUDE/SYS/TYPES.H"
#include "gg:NETINCLUDE/NETDB.H"
#include "gg:NETINCLUDE/SYS/FILIO.H"
#include "gg:netinclude/sys/ioctl.h"
#include "gg:NETINCLUDE/SYS/SOCKET.H"
#include "gg:NETINCLUDE/BSDSOCKET/SOCKETBASETAGS.H"
#include "gg:NETINCLUDE/CLIB/SOCKET_PROTOS.H"
}

//bottom code works perfectly with winuaes gcc setup
/*extern "C"
{
#include "gg:NETINCLUDE/SYS/TYPES.H"
#include "gg:NETINCLUDE/NETDB.H"
#include "gg:NETINCLUDE/SYS/FILIO.H"
#include "GCC-AmigaOS:ADE/netinclude/sys/ioctl.h"
#include "gg:NETINCLUDE/SYS/SOCKET.H"
#include "gg:NETINCLUDE/BSDSOCKET/SOCKETBASETAGS.H"
#include "GCC-AmigaOS:ADE/NETINCLUDE/CLIB/SOCKET_PROTOS.H"
}*/


#endif



BPTR urlgrabber_file;
BPTR newbptr_file;
BOOL aslresult;
LONG char_read_storage;
LONG colour=8; //default colour for listview text
LONG custom_pen_colours[22];
int nickcomp_state=0;
int nickcomp_count=0;
int a,b;
int fdmax=-1;        // maximum file descriptor number
int count_the_linefeeds=0;
int delay_b4_ping_server_count=0;
int how_many_lines=15;
int sort_method=1; //which sorting method is used for nick lists - only 1 presently
char buffer_text[600];
char urlvisit_str[0][2000];
char urlgrabber_str[2000];
char list_found_nicks[5000][50]; // Nick completion function variables
char file_name[800];
char wookie_folder[300]; //the pathname wookiechat is located in
char *pch;
char *pch3;
char *nick=new char[30];
char *nick2=new char[30];
char *nick3=new char[30];
char *string1;
char *string2;
char *string3;
char *string4;
char *string5;
char *tabwork_string=new char[900]; //original string gadget contents
char *tabwork2_string=new char[900]; //text before cursor position
char *tabwork3_string=new char[900]; //text after cursor position
char *tabwork4_string=new char[100]; //nick text to complete
char *tabwork5_string=new char[100]; //completed nick text to insert
char string7[900];
char string8[900];
char string9[900];
char string10[900];
char string11[900];
char uhostname[50] = "222-153-241-57.jetstream.xtra.co.nz"; // Default Hostname
char uservername[50] = "uk.amigaworld.net"; // Default Server of the User
char realname[100];
char username[20];
char background[64];
char background2[64];
char activity[64];
char activity_chat[64];
char activity_highlight[64];
char *background3=new char[64];
char *nickcolour=new char[40];
struct ClockData *clockdata;
struct MUI_NList_TestPos_Result *last_clicked_res;
struct FileRequester *filerequester;
Object *o;


//select stuff
fd_set read_master;   // master file descriptor list
fd_set read_fds; // temp file descriptor list for select()
fd_set exception_master;   // master file descriptor list
fd_set exception_fds; // temp file descriptor list for select()
fd_set write_master;   // master file descriptor list
fd_set write_fds; // temp file descriptor list for select()



//struct Task *MainTask;

struct XYMessage
{
    struct Message xy_Msg;
    int quit;

};



struct Settings
{
    int auto_rejoin_on_kick;
    int open_query_on_privmsg;
    LONG max_lines_for_buffers;
    int log_channels;
    int log_private;
    char default_kickmsg[500];
    char quit_msg[800];
    char part_msg[800];

    //when doubleclicking on a nick in the nick list do you want to..
    int request_whois_info;
    int open_private_tab;

    char time_stamp_string[20];
    int timestamp_config;
    char text_editor[250];
    char browser[250];

    char download_path[200];

    int dcc_recv_auto_accept;
    int dcc_recv_auto_accept_if_exists_action; // 1 == overwrite
                                               // 2 == resume
                                               // 3 == abort
                                               // 4 == ask
    int autogethostid;
    char ipaddr[100];
    char start_port[100];
    char end_port[100];
    int current_port;

    int how_many_lines_to_reload_channel;
    int how_many_lines_to_reload_private;

    char highlight[800];

    char samples_path[800];
    char sound_sample_player[800];
    char sound_sample_tabopen[800];
    int play_sound_when_tabopen;  // 0 == never
                                  // 1 == always
                                  // 2 == only when window is inactive

    char sound_sample_highlight[800];
    int play_sound_when_highlight; // 0 == never
                                   // 1 == always
                                   // 2 == only when window is inactive


    char command_alias[100][600];
    int number_of_command_aliases;
    int use_column_display;
    int nick_completion;
} my_settings, temp_settings;

enum { NEVER=0, ALWAYS, WHEN_WIN_ACTIVE };
enum { AMIRC_STYLE=0, MIRC_STYLE };

/* PREFS */



// below are some variables for working with NextObject() loops
struct MinList *list;
APTR object_state, member_object;

static struct NewMenu MenuData1[] =
{
    { NM_TITLE, "Nicklist Options"       , 0 ,0 ,0          ,(APTR)0  },
	{ NM_ITEM, "Whois"                   ,0,0 ,0             ,(APTR)65     },
	{ NM_ITEM, "Open Query"              ,0,0 ,0             ,(APTR)66     },
	{ NM_ITEM, "Open Global Query"              ,0,0 ,0             ,(APTR)80     },
	{ NM_ITEM, "CTCP"                     ,0,0, 0             ,(APTR)0            },
	{ NM_SUB,  "Ping"                    ,0,0 ,0             ,(APTR)75    },
	{ NM_SUB,  "Version"                 ,0,0 ,0             ,(APTR)76    },
	{ NM_SUB,  "Time"                    ,0,0 ,0             ,(APTR)77    },
    { NM_ITEM, "Direct Connection"                  ,0,0, 0             ,(APTR)0            },
	{ NM_SUB,  "Send file"                      ,0,0 ,0             ,(APTR)79     },
	{ NM_SUB,  "Chat"                      ,0,0 ,0             ,(APTR)81     },
    { NM_ITEM, "Control"                  ,0,0, 0             ,(APTR)0            },
	{ NM_SUB,  "Op"                      ,0,0 ,0             ,(APTR)67     },
	{ NM_SUB,  "DeOp"                    ,0,0 ,0             ,(APTR)68     },
	{ NM_SUB,  "HalfOp"                  ,0,0 ,0             ,(APTR)69     },
	{ NM_SUB,  "DeHalfOp"                ,0,0 ,0             ,(APTR)70     },
	{ NM_SUB,  "Voice"                   ,0,0 ,0             ,(APTR)71     },
	{ NM_SUB,  "DeVoice"                   ,0,0 ,0             ,(APTR)72     },
	{ NM_SUB,  "Kick"                    ,0,0 ,0             ,(APTR)73     },
	 { NM_SUB,  "Ban"                     ,0,0 ,0             ,(APTR)83    },

	{ NM_END,NULL,0,0,0,(APTR)0 },
};

static struct NewMenu MenuData2[] =
{
    { NM_TITLE, "Clipboard"       , 0 ,0 ,0          ,(APTR)0  },
	{ NM_ITEM, "Copy to clipboard"                   ,0,0 ,0             ,(APTR)78     },
	{ NM_END,NULL,0,0,0,(APTR)0 },

};

struct MUI_CustomClass *NLI_Class;

#include "version_info.cpp"


struct ObjApp
{
	APTR	App;
	APTR	WI_main;

    //quit requester
	APTR	WI_quit;
    APTR    BT_quit_yes;
    APTR    BT_quit_no;

    //about window objects
    APTR    WI_about;
    APTR    LV_about;
    APTR    TX_about;
    APTR    TX_about2;
    APTR    wookiechat_img_obj;
    APTR    guru_meditation_img_obj;
    APTR    SETTINGS_ROOT;
	
    //This window pops up if you try to recieve a file that already exists
    APTR    WI_dcc_file_exists;
	APTR    TX_oldname;
    APTR    TX_newname;
    APTR    TX_oldsize;
    APTR    TX_newsize;
    APTR    TX_olddate;
    APTR    BT_dcc_resume;
	APTR    BT_dcc_overwrite;
	APTR    BT_dcc_rename;
	APTR    BT_dcc_abort;


    //DCC WINDOW BUTTONS AND OBJECTS
    APTR    WI_dcc;
    APTR    WI_dcc2;
	APTR	LV_dcc;
	APTR    BT_dcc_accept;
	APTR    BT_dcc_cancel;
	APTR    BT_dcc_remove;
    APTR    STR_dcc_PA_label_0;
	APTR    PA_dcc_label_0;
    APTR    DCC_Register_Grp;
	APTR	LV_send_dcc;
    APTR    BT_dcc_send_new;
	APTR    BT_dcc_send_reoffer;
	APTR    BT_dcc_send_cancel;
	APTR    BT_dcc_send_remove;

	APTR	MN_;

	APTR	STR_usertext;
	APTR	BT_query;

	APTR	WI_label_2;
	APTR	STR_nickname1;
	APTR	STR_nickname2;
	APTR	STR_nickname3;
	APTR	STR_realname;
	APTR	STR_username;
	APTR	BT_addserver;
	APTR	BT_addgroup;
	APTR	BT_delete_server;
	APTR	BT_connect;
	APTR	BT_connect_tab;
    APTR    NLT_Servers;

   //edit window stuff follows
	APTR	WI_edit_server;
	APTR	STR_servername;
	APTR	STR_port;
	APTR	STR_pass;
	APTR	BT_acceptserver;
	APTR	BT_cancelserver;
	APTR	WI_edit_group;
	APTR	STR_groupname;
    APTR    BT_perform;
	APTR	BT_acceptgroup;
	APTR	BT_cancelgroup;

   //colour settings window prefs
    APTR    WI_colour_settings;
    APTR    PP_CSW_background;
    APTR    PP_CSW_nicklistbackground;
    APTR    PP_CSW_join;
    APTR    PP_CSW_part;
    APTR    PP_CSW_quit;
    APTR    PP_CSW_modes;
    APTR    PP_CSW_ctcp;
    APTR    PP_CSW_actions;
    APTR    PP_CSW_normal;
    APTR    PP_CSW_info;
    APTR    PP_CSW_owntext;
    APTR    PP_CSW_highlight;
    APTR    PP_CSW_notices;
    APTR    PP_CSW_invite; //13
    APTR    PP_CSW_kick;    //14
    APTR    PP_CSW_nickchange;  //15
    APTR    PP_CSW_topic;     //16
    APTR    PP_CSW_wallops;    //17
    APTR    PP_CSW_activity;     //18
    APTR    PP_CSW_chatactivity;  //19
    APTR    PP_CSW_highlightactivity; //20
    APTR    PP_CSW_nicklisttext; //21

   //main settings window
    APTR    SETTINGS_Register_Grp;
	APTR	WI_mainsettings;
	APTR	CH_label_6;
	APTR	CH_label_1;
	APTR	STR_label_3;
	APTR	CH_label_2;
	APTR	CH_label_3;
	APTR	STR_label_1;
	APTR	STR_label_2;
	APTR	STR_label_4;
	APTR	STR_label_5;
	APTR	CH_label_4;
	APTR	CH_label_5;
	APTR	STR_label_0;
	APTR	CH_label_0;
	APTR	PA_label_0;
	APTR	STR_PA_label_0;
	APTR	BT_label_0;
	APTR	BT_label_1;
	APTR	BT_label_2;
    APTR    TX_highlight_label;
	APTR    STR_highlight_label;
    APTR    TX_how_many_lines_to_reload;
    APTR    STR_how_many_lines_to_reload_channel;
    APTR    STR_how_many_lines_to_reload_private;
    APTR    LA_texteditor;
    APTR    PA_browser_label;
    APTR    PA_browser;
	APTR	STR_browser;
	APTR    RA_nick_completion_style_Label;
    APTR    RA_nick_completion_style;

    //dcc part of the settings window
	APTR	CH_autoaccept;
    APTR    LV_trusted_nicks;
    APTR    RA_autoaccept;
    APTR    CH_gethostid;
    APTR    STR_ipaddr_dcc;
    APTR    STR_port1_dcc;
    APTR    STR_port2_dcc;

    //events part of settings window
    APTR    PA_sound_sample_player_label;
    APTR    PA_sound_sample_player;
    APTR    LA_sound_sample_player;
    APTR    STR_sound_sample_player;
    APTR    PA_sound_sample_tabopen_label;
    APTR    PA_sound_sample_tabopen;
    APTR    LA_sound_sample_tabopen;
    APTR    STR_sound_sample_tabopen;
    APTR    RA_sound_sample_tabopen;
    APTR    PA_sound_sample_highlight_label;
    APTR    PA_sound_sample_highlight;
    APTR    LA_sound_sample_highlight;
    APTR    STR_sound_sample_highlight;
    APTR    RA_sound_sample_highlight;
    APTR    LA_samples_path;
    APTR    STR_samples_path;
    APTR    PA_samples_path;

    //aliases register group in settings window
    APTR    LV_alias;
    APTR    BT_add_alias;
    APTR    BT_remove_alias;
    APTR    BT_edit_alias;
    APTR    STR_add_alias;

    //initiate dcc send window
	APTR	WI_dcc_send;
	APTR	PA_dcc_send_file;
	APTR	STR_PA_dcc_send_file;
	APTR	STR_dcc_send_nick;
	APTR	BT_dcc_send;


    //url grabber window
    APTR    WI_urlgrabber;
    APTR    LV_urlgrabber;
    APTR    BT_urlgrabber_clear;
    APTR    BT_urlgrabber_clearall;

};


struct MUI_PenSpec *background_pens[23];

struct MUI_PenSpec *background_pen=new MUI_PenSpec;

struct MUI_PenSpec *background_pen1 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen2 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen3 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen4 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen5 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen6 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen7 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen8 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen9 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen10 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen11 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen12 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen13 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen14 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen15 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen16 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen17 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen18 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen19 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen20 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen21 = new MUI_PenSpec;
struct MUI_PenSpec *background_pen22 = new MUI_PenSpec;


#define MYATTR_PEN  0x8022
#define MYATTR_PEN2 0x8023
#define MYATTR_PEN3 0x8024
#define MYATTR_PEN4 0x8025
#define MYATTR_PEN5 0x8026
#define MYATTR_PEN6 0x8027
#define MYATTR_PEN7 0x8028
#define MYATTR_PEN8 0x8029
#define MYATTR_PEN9 0x8030
#define MYATTR_PEN10 0x8031
#define MYATTR_PEN11 0x8032
#define MYATTR_PEN12 0x8033
#define MYATTR_PEN13 0x8034
#define MYATTR_PEN14 0x8035
#define MYATTR_PEN15 0x8036
#define MYATTR_PEN16 0x8037
#define MYATTR_PEN17 0x8038
#define MYATTR_PEN18 0x8039
#define MYATTR_PEN19 0x8040
#define MYATTR_PEN20 0x8041
#define MYATTR_PEN21 0x8045
#define MYATTR_PEN22 0x8046



struct ObjApp *James;

struct list_entry
{
    char modes[3];
    char name[32];
    char *hostname;
};

struct history
{
	char buffer_history[1000];
	struct history *next;
    struct history *previous;
} *work_history;


struct channel_entry
{
    char entry[800];
    LONG colour;
};


struct query_window
{
    struct channel_entry waiting_to_be_displayed_buffer[50];
    int waiting_to_be_displayed_count;
    BPTR log_file;

	APTR GR_conductor, GR_channel_modes, GR_listviews;
	APTR	BT_mode_X;
	APTR	STR_topic;
	APTR	BT_mode_T;
	APTR	BT_mode_N;
	APTR	BT_mode_S;
	APTR	BT_mode_I;
	APTR	BT_mode_P;
	APTR	BT_mode_M;
	APTR	BT_mode_B;
	APTR	BT_mode_K;
	APTR	STR_keyword;
	APTR	BT_mode_L;
	APTR	STR_limit;
	APTR	LV_channel;
	APTR	LV_nicklist;
    APTR    strip;
    APTR    strip2;

	BOOL attached;
	BOOL deleted;
	APTR	BT_querybutton;
    char name[60];
    char displayed_name[60];

    query_window *previous;

    query_window *next;
    int chanquery;
    struct history *string_root;
    struct history *string_conductor;
    int nicks;
    struct list_entry nicklist[5000];
    int mode_T;
    int mode_N;
    int mode_S;
    int mode_I;
    int mode_P;
    int mode_M;
    int mode_K;
    int mode_L;

    LONG entries_count;

    int activity;

} *current_query, *work_query;

struct status_window
{
    int socket_number;
    int connection_active;
    APTR GR_server1_buttons;
    APTR my_hvspace;

    int iv;

    long a_socket;

    char *str;
    char *buffer;

    struct query_window *root, *conductor, *current_query, *previous_query;
    status_window *next;
    char *nick;
    char pass[100];
    char groupname[100];
    char servername[100];
    char shortserver[50];
    char networkname[100];

    struct irc_user_modes_struct
    {
        char mode[3];
        char symbol[3];

    } user_modes[15];

    int max_modes;

    int filter_swear_words;

} *work_status, *status_root, *status_conductor, *status_current;

int socket_global=0;
    char rawservername[100];

APTR pen;

struct MUI_PenSpec *startpen;




struct list_entry *work_entry = new list_entry;

struct MUI_NList_GetSelectInfo *res=new struct MUI_NList_GetSelectInfo;
ULONG entries,visible,first;

struct channel_entry *centry=new channel_entry;
struct channel_entry *wentry=new channel_entry;

#include "subclasses.cpp"

MUI_CustomClass *mcc, *mcc2, *mcc3, *mcc4, *mcc5;
#define BS_Object NewObject(mcc->mcc_Class,NULL


ULONG ev_type;
long ev_sock;
char buffer3[BUFFERSIZE*2];
char buffer2[BUFFERSIZE*2];
int last_a=0;
int last_c=0;


char list_channels_work[900];


//THE HOOK BELOW WORKS PERFECTLY BUT ADDS /n'S AUTOMATICALLY, CANT GET AROUND THAT..

#ifdef __amigaos4__
static void Custom_Clipboard_Func(REG(a0, struct Hook *hook),REG(a2, char **array),REG(a1, char *new_entry)) {
//char ** Custom_Clipboard_Func(REG(a0, struct Hook *hook),REG(a2, char **array),REG(a1, char *new_entry)) {
#elif __GNUC__
static void Custom_Clipboard_Func(void) {

  register char **a2 __asm("a2");                         char **array = a2;
  //register struct NList_CopyColumnToClipMessage *a1 __asm("a1");  struct NList_CopyColumnToClipMessage *new_entry = a1;
  register char *a1 __asm("a1");                         char *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

    DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_GetEntry,array[-1], &buffer3);
    if(!buffer3) return;

    //if(new_entry) printf("new_entry:%s\n",new_entry);
    //if(array) printf("array[-1]:%i\narray[1]:%i\narray[2]:%i\n",array[-1],array[1],array[2]);
    //if(array) printf("array[-1]:%i\narray[0]:%i\narray[1]:%i\narray[2]:%i\narray[3]:%i\narray[4]:%i\n",array[-1],array[0],array[1],array[2],array[3],array[4]);

    char **new_array;
    new_array[0]=new char[500];
    new_array[1]=new char[10];

    int a=(int)array[1],b=0,c=(int)array[2];
    //printf("a=%i, c=%i\n",a,c);

    if(a==0) a=11;

    while(a<c)
    {
        new_array[0][b]=new_entry[a];
         a++; b++;
    }

    new_array[0][b]='\0';

    sprintf(new_array[1],"%i",strlen(new_array[0]));

    array[0]=new_array[0];
    array[1]="5"; //new_array[1];


}


const struct Hook Custom_Clipboard_Hook = { { NULL,NULL }, (ULONG(*)())Custom_Clipboard_Func, NULL,NULL };




#ifdef __amigaos4__
struct list_entry * ConstructLI_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, list_entry *new_entry)) {
#elif __GNUC__
struct list_entry * ConstructLI_TextFunc(void) {

  register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct list_entry *a1 __asm("a1");             struct list_entry *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

    struct list_entry *work_entry = (struct list_entry*) AllocMem(sizeof(struct list_entry),MEMF_ANY);
    *work_entry=*new_entry;

    return work_entry;
}

const struct Hook ConstructLI_TextHook = { { NULL,NULL }, (ULONG(*)())ConstructLI_TextFunc, NULL,NULL };





  #ifdef __amigaos4__
static void DestructLI_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, list_entry *new_entry)) {
  #elif __GNUC__
static void DestructLI_TextFunc(void)
{

 register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct list_entry *a1 __asm("a1");             struct list_entry *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

  if (new_entry)
    FreeMem((struct list_entry*) new_entry, sizeof(struct list_entry));
}


const struct Hook DestructLI_TextHook = { { NULL,NULL },(ULONG(*)())DestructLI_TextFunc, NULL,NULL };

/* *********************************************** */


char pen_number[10];
char new_array[20];
char new_array2[900];



//multi column display hook for channels

char display_hook_breakup[900];
char display_hook_column1[900];
char display_hook_column2[900];
char display_hook_column3[900];
char *display_string1;


  #ifdef __amigaos4__
static void DisplayLI2_channel_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
  #elif __GNUC__
static void DisplayLI2_channel_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct channel_entry *a1 __asm("a1");        struct channel_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {

        if(a_entry->colour >= 0 && a_entry->colour <= 22)
        {

            sprintf(new_array,"\033P[%ld]",custom_pen_colours[a_entry->colour]);
            strncpy(a_entry->entry,new_array,strlen(new_array));

            strcpy(display_hook_breakup, a_entry->entry);
            sprintf(display_hook_column1,"\033l");
            display_string1=strtok(display_hook_breakup," ");
            if(display_string1) strcat(display_hook_column1,display_string1);
            array[2]=display_hook_column1;

            sprintf(display_hook_column2,"\033r\%s",new_array);
            display_string1=strtok(NULL," ");
            if(display_string1) strcat(display_hook_column2,display_string1);
            array[1]=display_hook_column2;

            sprintf(display_hook_column3,"\033P%s",new_array);
            display_string1=strtok(NULL,"");
            if(display_string1) strcat(display_hook_column3,display_string1);
            array[0]=display_hook_column3;

        }
        else
        {
            sprintf(new_array,"\033P[%ld]",custom_pen_colours[1]);

            strncpy(a_entry->entry,new_array,strlen(new_array));

            array[0]=a_entry->entry;
            array[1]="";
            array[2]="";
        }


    }

}


const struct Hook DisplayLI2_channel_TextHook = { { NULL,NULL },(ULONG(*)())DisplayLI2_channel_TextFunc, NULL,NULL };

//single column displayhook for channels

  #ifdef __amigaos4__
static void DisplayLI_channel_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
  #elif __GNUC__
static void DisplayLI_channel_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct channel_entry *a1 __asm("a1");        struct channel_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {

        if(a_entry->colour >= 0 && a_entry->colour <= 22)
            sprintf(new_array,"\033P[%ld]",custom_pen_colours[a_entry->colour]);
        else
            sprintf(new_array,"\033P[%ld]",custom_pen_colours[1]);

        strncpy(a_entry->entry,new_array,strlen(new_array));

        //removed this block of code for testing stability

        array[0]=a_entry->entry;

    }

}

const struct Hook DisplayLI_channel_TextHook = { { NULL,NULL },(ULONG(*)())DisplayLI_channel_TextFunc, NULL,NULL };

// CONSTRUCTER FOR CHANNEL LISTVIEWS

#ifdef __amigaos4__
struct channel_entry * ConstructLI_channel_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct channel_entry *anew_entry)) {
#elif __GNUC__
struct channel_entry * ConstructLI_channel_TextFunc(void) {

  register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct channel_entry *a1 __asm("a1");             struct channel_entry *anew_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

    struct channel_entry *work_entry = (struct channel_entry*) AllocMem(sizeof(struct channel_entry),MEMF_ANY);
    *work_entry=*anew_entry;

    return work_entry;
}

const struct Hook ConstructLI_channel_TextHook = { { NULL,NULL }, (ULONG(*)())ConstructLI_channel_TextFunc, NULL,NULL };



//DESTRUCTER FOR CHANNEL LISTVIEWS

  #ifdef __amigaos4__
static void DestructLI_channel_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, channel_entry *new_entry)) {
  #elif __GNUC__
static void DestructLI_channel_TextFunc(void)
{

 register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct channel_entry *a1 __asm("a1");             struct channel_entry *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

  if (new_entry)
    FreeMem((struct channel_entry*) new_entry, sizeof(struct channel_entry));
}


const struct Hook DestructLI_channel_TextHook = { { NULL,NULL },(ULONG(*)())DestructLI_channel_TextFunc, NULL,NULL };

/* *********************************************** */


static char buf[20];

  #ifdef __amigaos4__
static void DisplayLI_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, list_entry *a_entry)) {
  #elif __GNUC__
static void DisplayLI_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct list_entry *a1 __asm("a1");             struct list_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {
        array[0] = a_entry->modes;
        array[1] = a_entry->name;
        //array[2] = a_entry->hostname;
    }

}

const struct Hook DisplayLI_TextHook = { { NULL,NULL },(ULONG(*)())DisplayLI_TextFunc, NULL,NULL };

/* *********************************************** */


  #ifdef __amigaos4__
static void DisplayTREE_TextFunc(  REG(a0, struct IClass *cl),  REG(a2, char **array),  REG(a1, MUIP_NListtree_DisplayMessage *a_entry)) {
  #elif __GNUC__
static void DisplayTREE_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register MUIP_NListtree_DisplayMessage *a1 __asm("a1");    MUIP_NListtree_DisplayMessage *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
  #endif

    if(a_entry)
    {
        char *work;

        work=strtok(a_entry->Array[0]," ");
        work=strtok(NULL," ");
        strcpy(a_entry->Array[0],work);

    }

}

const struct Hook DisplayTREE_TextHook = { { NULL,NULL },(ULONG(*)())DisplayTREE_TextFunc, NULL,NULL };


struct hostent *he;
struct sockaddr_in slapshot_in;
struct sockaddr addr_buffer, addr_buffer2;

char *PreParse_NewText="\033c\0333";
char *NewPreParse_NewText=new char[100];
char *channel_display=new char[100];

int iv=0;

APTR BT_blank;

struct query_window *init_conductor(int a)
{


    #define FW 13
	struct query_window *test;

	test = (struct query_window *) AllocVec(sizeof(struct query_window), MEMF_PUBLIC|MEMF_CLEAR);

    test->entries_count=0;

    for(count=0; count<4000; count++)
        test->nicklist[count].hostname=NULL;

    test->next=NULL;

    test->mode_T=0;
    test->mode_N=0;
    test->mode_S=0;
    test->mode_I=0;
    test->mode_P=0;
    test->mode_M=0;
    test->mode_K=0;
    test->mode_L=0;

    test->nicks=0;

    iv++;

	if(a==1)
	{


	test->BT_mode_X = (Object*)TextObject,
		ButtonFrame,
		MUIA_Weight, 0,
        MUIA_FixWidth, FW,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "X",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "Close the current tab",
        MUIA_InputMode, MUIV_InputMode_RelVerify,
		//MUIA_InputMode, MUIV_InputMode_Toggle,
	End;



    if(string_default)
    {
        test->STR_topic = (Object*)StringObject,
		MUIA_Weight, 80,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "This is the channels current topic",
		MUIA_String_MaxLen, 500,
    	End;
    }
    else
    {
        test->STR_topic = (Object*)BetterStringObject,
		MUIA_Weight, 80,
		//MUIA_Textinput_Multiline, FALSE,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "This is the channels current topic",
		//MUIA_String_MaxLen, 400,
    	End;

    }

	test->BT_mode_T = (Object*)TextObject,
		ButtonFrame,
        MUIA_FixWidth, FW,
		MUIA_Weight, 0,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "T",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "Topic protection. This setting prevents regular users from changing the channel topic",
        MUIA_InputMode, MUIV_InputMode_Toggle,
		//MUIA_InputMode, MUIV_InputMode_RelVerify,
	End;

	test->BT_mode_N = (Object*)TextObject,
		ButtonFrame,
        MUIA_FixWidth, FW,
		MUIA_Weight, 0,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "N",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "No messaging. This setting forces users to be in the channel to be able to talk in it",
        MUIA_InputMode, MUIV_InputMode_Toggle,
		//MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

	test->BT_mode_S = (Object*)TextObject,
		ButtonFrame,
        MUIA_FixWidth, FW,
		MUIA_Weight, 0,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "S",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "Secret mode. If this is set, the channel is invisible until you have joined",
        MUIA_InputMode, MUIV_InputMode_Toggle,
		//MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

	test->BT_mode_I = (Object*)TextObject,
		ButtonFrame,
        MUIA_FixWidth, FW,
		MUIA_Weight, 0,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "I",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "Invite only. Only users who have been invited to the channel may join",
		MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

	test->BT_mode_P = (Object*)TextObject,
		ButtonFrame,
        MUIA_FixWidth, FW,
		MUIA_Weight, 0,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "P",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "Private mode. If set, channel topic is not shown on global channel lists.",
		MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

	test->BT_mode_M = (Object*)TextObject,
		ButtonFrame,
        MUIA_FixWidth, FW,
		MUIA_Weight, 0,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "M",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "Moderated mode. Only users with operator, halfop or voice status (thats @ % or +) may talk in channel when set",
		MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

	test->BT_mode_B = (Object*)TextObject,
		ButtonFrame,
        MUIA_FixWidth, FW,
		MUIA_Weight, 0,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "B",
		MUIA_Text_PreParse, "\033c",
		//MUIA_ShortHelp, "BT_mode_B",
		MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

	test->BT_mode_K = (Object*)TextObject,
		ButtonFrame,
        MUIA_FixWidth, FW,
		MUIA_Weight, 0,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "K",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "Keyword mode. When this is set, users need to supply a keyword (password) to enter.",
		MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

    if(string_default)
    {
    	test->STR_keyword = (Object*)StringObject,
		MUIA_Weight, 10,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "Current channel keyword",
        MUIA_String_Secret, TRUE,
    	End;
    }
    else
    {
        test->STR_keyword = (Object*)BetterStringObject,
		MUIA_Weight, 10,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "Current channel keyword",
        MUIA_String_Secret, TRUE,
    	End;
    }

	test->BT_mode_L = (Object*)TextObject,
		ButtonFrame,
        MUIA_FixWidth, FW,
		MUIA_Weight, 0,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "L",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "Enforce a limit to how many users can currently join the channel.",
		MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

    if(string_default)
    {
    	test->STR_limit = (Object*)StringObject,
		MUIA_Weight, 10,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "Current user limit",
    	End;
    }
    else
    {
        test->STR_limit = (Object*)BetterStringObject,
		MUIA_Weight, 10,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "Current user limit",
 	    MUIA_String_Accept, "0123456789",
    	End;
    }

	test->GR_channel_modes = (Object*)GroupObject,
		MUIA_HelpNode, "GR_channel_modes",
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 2,
		Child, test->BT_mode_X,
		Child, test->STR_topic,
		Child, test->BT_mode_T,
		Child, test->BT_mode_N,
		Child, test->BT_mode_S,
		Child, test->BT_mode_I,
		Child, test->BT_mode_P,
		Child, test->BT_mode_M,
		//Child, test->BT_mode_B,
		Child, test->BT_mode_K,
		Child, test->STR_keyword,
		Child, test->BT_mode_L,
		Child, test->STR_limit,
	End;

    // Below:- Get the configured colour and copy it to a string for use in our background attribute
    get(James->PP_CSW_background,MUIA_Pendisplay_Spec,&background_pen);
    strcpy(background2," "); strcat(background2,background_pen->buf);
    background2[0]='2'; background2[1]=':';
    //printf("background:\n%s\n",background2);

    if(my_settings.use_column_display==0) //make a one columned display
    {
    	test->LV_channel = (Object*)NewObject(mcc2->mcc_Class,NULL,// NListObject,
            //MUIA_ContextMenu, test->strip2 = MUI_MakeObject(MUIO_MenustripNM,MenuData2,0),
    		MUIA_Frame, MUIV_Frame_InputList,
            MUIA_NList_ConstructHook, &ConstructLI_channel_TextHook,
            MUIA_NList_DestructHook, &DestructLI_channel_TextHook,
            MUIA_NList_DisplayHook, &DisplayLI_channel_TextHook,
            //MUIA_NList_DisplayHook, &DisplayLI2_channel_TextHook,
            //MUIA_NList_Format,listview_format,
            MUIA_NList_AutoCopyToClip, TRUE,
            MUIA_NList_Input, FALSE,
            MUIA_NList_ListBackground, background2,
            MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
    	End;
    }
    else //make a multi columned display for time/nick/text
    {
    	test->LV_channel = (Object*)NewObject(mcc2->mcc_Class,NULL,// NListObject,
            //MUIA_ContextMenu, test->strip2 = MUI_MakeObject(MUIO_MenustripNM,MenuData2,0),
    		MUIA_Frame, MUIV_Frame_InputList,
            MUIA_NList_ConstructHook, &ConstructLI_channel_TextHook,
            MUIA_NList_DestructHook, &DestructLI_channel_TextHook,
            //MUIA_NList_DisplayHook, &DisplayLI_channel_TextHook,
            MUIA_NList_DisplayHook, &DisplayLI2_channel_TextHook,
            MUIA_NList_Format,listview_format,
            MUIA_NList_AutoCopyToClip, TRUE,
            MUIA_NList_Input, FALSE,
            MUIA_NList_ListBackground, background2,
            MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
    	End;
    }


    /*	  test->LV_channel = (Object*)NewObject(mcc2->mcc_Class,NULL,// NListObject,
            //MUIA_ContextMenu, test->strip2 = MUI_MakeObject(MUIO_MenustripNM,MenuData2,0),
    		MUIA_Frame, MUIV_Frame_InputList,
            MUIA_NList_ConstructHook, &ConstructLI_channel_TextHook,
            MUIA_NList_DestructHook, &DestructLI_channel_TextHook,
            MUIA_NList_DisplayHook, &DisplayLI_channel_TextHook,
            //MUIA_NList_DisplayHook, &DisplayLI2_channel_TextHook,
            //MUIA_NList_Format,listview_format,
            MUIA_NList_AutoCopyToClip, TRUE,
            MUIA_NList_Input, FALSE,
            MUIA_NList_ListBackground, background2,
            MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
    	End;
    */
    if(!test->LV_channel) cleanexit("Please download and install the NList MUI Class from aminet\n");

	test->LV_channel = (Object*)NListviewObject,
		MUIA_HelpNode, "LV_channel",
		MUIA_Weight, 80,
		//MUIA_NListview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_NListview_NList, test->LV_channel,
	End;



// MENUSTRIP STUFF

    // Below:- Get the configured colour and copy it to a string for use in our background attribute
    get(James->PP_CSW_nicklistbackground,MUIA_Pendisplay_Spec,&background_pen);
    strcpy(background," "); strcat(background,background_pen->buf);
    background[0]='2'; background[1]=':';

    get(James->PP_CSW_nicklisttext,MUIA_Pendisplay_Spec,&background_pen22);


	test->LV_nicklist = (Object*)NewObject(mcc2->mcc_Class,NULL,  //NListObject,
        MUIA_ContextMenu, test->strip = (Object*)MUI_MakeObject(MUIO_MenustripNM,MenuData1,0),
        MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Default,
		MUIA_NList_ConstructHook, &ConstructLI_TextHook,
		MUIA_NList_DisplayHook, &DisplayLI_TextHook,
        MUIA_NList_DestructHook, &DestructLI_TextHook,
        MUIA_NList_ListBackground, background,
        MUIA_NList_ListPen, background_pen22,
        //MUIA_NList_ListBackground, "2:00000000,2EEF2EEF,AC23AC23", //MUIPEN(custom_pen_colours[1]),
		MUIA_Frame, MUIV_Frame_InputList,
	End;


	test->LV_nicklist = (Object*)NListviewObject,
	//test->LV_nicklist = (Object*)NewObject(mcc5->mcc_Class,NULL, //NListviewObject,
        MUIA_HelpNode, "LV_nicklist",
		//MUIA_Weight, 20,
		MUIA_Width, 150,
		MUIA_MaxWidth, 150,
        //MUIA_CustomBackfill, TRUE, //Custom Backfill!!!!!!!!!

        //MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Default,
		//MUIA_NListview_Input, FALSE,
		MUIA_NListview_NList, test->LV_nicklist,
	End;

	test->GR_listviews = (Object*)GroupObject,
		MUIA_HelpNode, "GR_listviews",
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 0,
		MUIA_Group_VertSpacing, 0,
		Child, test->LV_channel,
		Child, test->LV_nicklist,
	End;

	test->GR_conductor = (Object*)GroupObject,
		MUIA_HelpNode, "GR_conductor",
		Child, test->GR_channel_modes,
		Child, test->GR_listviews,
	End;

    set((Object*)test->LV_nicklist,MUIA_NList_Format,"COLWIDTH=1.5, WEIGHT=90");
    set((Object*)test->LV_nicklist,MUIA_NListview_Horiz_ScrollBar,MUIV_NListview_HSB_None);

    //set((Object*)test->LV_nicklist,MUIA_FillArea, TRUE);

    DoMethod((Object*)test->LV_nicklist,MUIM_Notify,MUIA_NList_DoubleClick,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,9);

    DoMethod((Object*)test->BT_mode_T,MUIM_Notify,MUIA_Selected,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,10);
    DoMethod((Object*)test->BT_mode_N,MUIM_Notify,MUIA_Selected,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,11);
    DoMethod((Object*)test->BT_mode_S,MUIM_Notify,MUIA_Selected,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,12);
    DoMethod((Object*)test->BT_mode_I,MUIM_Notify,MUIA_Selected,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,13);
    DoMethod((Object*)test->BT_mode_P,MUIM_Notify,MUIA_Selected,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,14);
    DoMethod((Object*)test->BT_mode_M,MUIM_Notify,MUIA_Selected,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,15);

    DoMethod((Object*)test->BT_mode_L,MUIM_Notify,MUIA_Selected,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,16);
    DoMethod((Object*)test->BT_mode_K,MUIM_Notify,MUIA_Selected,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,17);

    DoMethod((Object*)test->BT_mode_T,MUIM_Notify,MUIA_Selected,FALSE,(Object*)James->App,2,MUIM_Application_ReturnID,20);
    DoMethod((Object*)test->BT_mode_N,MUIM_Notify,MUIA_Selected,FALSE,(Object*)James->App,2,MUIM_Application_ReturnID,21);
    DoMethod((Object*)test->BT_mode_S,MUIM_Notify,MUIA_Selected,FALSE,(Object*)James->App,2,MUIM_Application_ReturnID,22);
    DoMethod((Object*)test->BT_mode_I,MUIM_Notify,MUIA_Selected,FALSE,(Object*)James->App,2,MUIM_Application_ReturnID,23);
    DoMethod((Object*)test->BT_mode_P,MUIM_Notify,MUIA_Selected,FALSE,(Object*)James->App,2,MUIM_Application_ReturnID,24);
    DoMethod((Object*)test->BT_mode_M,MUIM_Notify,MUIA_Selected,FALSE,(Object*)James->App,2,MUIM_Application_ReturnID,25);

    DoMethod((Object*)test->BT_mode_L,MUIM_Notify,MUIA_Selected,FALSE,(Object*)James->App,2,MUIM_Application_ReturnID,27);
    DoMethod((Object*)test->BT_mode_K,MUIM_Notify,MUIA_Selected,FALSE,(Object*)James->App,2,MUIM_Application_ReturnID,28);

    DoMethod((Object*)test->STR_limit,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)James->App,2,MUIM_Application_ReturnID,16);
    DoMethod((Object*)test->STR_keyword,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)James->App,2,MUIM_Application_ReturnID,17);

    DoMethod((Object*)test->STR_topic,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)James->App,2,MUIM_Application_ReturnID,29);

	}

	if(a==2)
	{

		test->BT_mode_T=NULL;
		test->BT_mode_N=NULL;
		test->BT_mode_S=NULL;
		test->BT_mode_I=NULL;
		test->BT_mode_P=NULL;
		test->BT_mode_M=NULL;
		test->BT_mode_B=NULL;
		test->BT_mode_K=NULL;
		test->BT_mode_L=NULL;

	test->BT_mode_X = (Object*)TextObject,
		ButtonFrame,
		MUIA_Weight, 0,
        MUIA_FixWidth, FW,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "X",
		MUIA_Text_PreParse, "\033c",
		MUIA_ShortHelp, "Close the current tab",
		MUIA_InputMode, MUIV_InputMode_RelVerify,
	End;

    if(string_default)
    {
    	test->STR_topic = (Object*)StringObject,
		MUIA_Weight, 80,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "This is the hostname of the user you are currently talking to",
		MUIA_String_MaxLen, 400,
    	End;
    }
    else
    {
        test->STR_topic = (Object*)BetterStringObject,
		MUIA_Weight, 80,
		//MUIA_Textinput_Multiline, FALSE,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "This is the hostname of the user you are currently talking to",
		//MUIA_ShortHelp, "STR_topic",
		//MUIA_String_MaxLen, 400,
    	End;
    }

	test->GR_channel_modes = (Object*)GroupObject,
		MUIA_ShortHelp, "GR_channel_modes",
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 2,
		Child, test->BT_mode_X,
		Child, test->STR_topic,
	End;

    // Below:- Get the configured colour and copy it to a string for use in our background attribute
    get(James->PP_CSW_background,MUIA_Pendisplay_Spec,&background_pen);
    strcpy(background2," "); strcat(background2,background_pen->buf);
    background2[0]='2'; background2[1]=':';

    if(my_settings.use_column_display==0) //make a one columned display
    {
    	test->LV_channel = (Object*)NewObject(mcc2->mcc_Class,NULL,// NListObject,
    		MUIA_Frame, MUIV_Frame_InputList,
            MUIA_NList_ConstructHook, &ConstructLI_channel_TextHook,
            MUIA_NList_DestructHook, &DestructLI_channel_TextHook,
            MUIA_NList_DisplayHook, &DisplayLI_channel_TextHook,
            MUIA_NList_AutoCopyToClip, TRUE,
            MUIA_NList_Input, FALSE,
            MUIA_NList_ListBackground, background2,
            MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
    	End;
    }
    else //make a multi columned display for time/nick/text
    {
    	test->LV_channel = (Object*)NewObject(mcc2->mcc_Class,NULL,// NListObject,
    		MUIA_Frame, MUIV_Frame_InputList,
            MUIA_NList_ConstructHook, &ConstructLI_channel_TextHook,
            MUIA_NList_DestructHook, &DestructLI_channel_TextHook,
            MUIA_NList_DisplayHook, &DisplayLI2_channel_TextHook,
            MUIA_NList_Format,listview_format,
            MUIA_NList_AutoCopyToClip, TRUE,
            MUIA_NList_Input, FALSE,
            MUIA_NList_ListBackground, background2,
            MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
    	End;
    }

	/*test->LV_channel = (Object*)NewObject(mcc2->mcc_Class,NULL, // NListObject,
		MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, &ConstructLI_channel_TextHook,
        MUIA_NList_DestructHook, &DestructLI_channel_TextHook,
        MUIA_NList_DisplayHook, &DisplayLI2_channel_TextHook,
        MUIA_NList_Format,listview_format, //"COL=2 WEIGHT=10 BAR,COL=1 WEIGHT=15 BAR,COL=0 WEIGHT=75 BAR",
        MUIA_NList_ListBackground, background2,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input, FALSE,
		MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
	End;*/

    if(!test->LV_channel) cleanexit("Please download and install the NList MUI Class from aminet\n");

	test->LV_channel = (Object*)NListviewObject,
		MUIA_HelpNode, "LV_channel",
		MUIA_Weight, 80,
		//MUIA_NListview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_NListview_NList, test->LV_channel,
	End;

	test->GR_listviews = (Object*)GroupObject,
		MUIA_HelpNode, "GR_listviews",
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 0,
		MUIA_Group_VertSpacing, 0,
		Child, test->LV_channel,
	End;

	test->GR_conductor = (Object*)GroupObject,
		MUIA_HelpNode, "GR_conductor",
		Child, test->GR_channel_modes,
		Child, test->GR_listviews,
	End;

    test->LV_nicklist=NULL;
	}

	test->BT_querybutton = (Object*)TextObject,
		ButtonFrame,
		//MUIA_Weight, 0,
		MUIA_MaxWidth, 150,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "new button",
		MUIA_Text_PreParse, "\033c\0332",
		MUIA_HelpNode, "BT_query2",
        MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

    set((Object*)test->LV_channel,MUIA_NListview_Horiz_ScrollBar,MUIV_NListview_HSB_None);

    DoMethod((Object*)test->BT_mode_X,MUIM_Notify,MUIA_Selected,FALSE,(Object*)James->App,2,MUIM_Application_ReturnID,73);

    test->string_root=new history;
    test->string_conductor=new history;
    strcpy(test->string_conductor->buffer_history,"");
    test->string_conductor->next=NULL;
    test->string_conductor->previous=NULL;
    test->string_root=test->string_conductor;

    DoMethod((Object*)test->LV_channel,MUIM_Notify, MUIA_NList_DoubleClick, TRUE,(Object*)James->App,2,MUIM_Application_ReturnID, 96);

	return test;



}




char wscreent[200];

int previous_b=0;
int a_socket=0;
ULONG portsig, portsig2, portsig3, portsig4;
char string_to_send[BUFFERSIZE];
int nick_length;
char sendstuff[1500];
int len;
ULONG test, test2;
ULONG socksigbit,dccsigbit,timer_signal;
char windowtitlestring[110];
char server[50];



// timer device variables
struct timerequest *TimerIO;
struct MsgPort *TimerMP;
struct Message *TimerMSG;
struct MsgPort *xyreplyport;

// second timer device variables for the 2 second delays
struct timerequest *Timer2IO;
struct MsgPort *Timer2MP;
struct Message *Timer2MSG;
struct timeval *systime;


LONG error;
ULONG days,hrs,secs,mins,mics;
char timestamp_hrs[4]; char timestamp_mins[4]; char timestamp_secs[4]; char timestamp[12];

//ctcp ping variables
char pingtimestamp_hrs[4]; char pingtimestamp_mins[4]; char pingtimestamp_secs[4]; char pingtimestamp[12];

//dcc time variables
char dcctimestamp_secs[1000]; char dcctimestamp[12];

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

LONG dcc_transfers=0;

//dcc send/recieve transfer structure
struct dcc
{
    char networkname[100];
    LONG pos;
    char address[50];
    char port[20];
    char nick[150];
    char filename[500];
    char filepath[500];
    char recv_buffer[DCC_RECV_BUFFERSIZE];
    char buffer[DCC_RECV_BUFFERSIZE];
    long dcc_socket;
    long dcc_listen_socket;
    struct dcc_entry *entry;
    ULONG total_recv;
    ULONG start_recv; //not used yet
    ULONG filesize;
    struct dcc *next;
    BPTR dcc_file;
    int completed;
    int connected;
    int removed;
    int cancelled;
    struct sockaddr_in test;
    struct sockaddr_in their_addr;
    char timestarted[1000];
} *dcc_root,*dcc_conductor,*dcc_send_root,*dcc_send_conductor;


struct dcc_chat
{
    struct query_window *conductor;
    char networkname[100];
    char address[50];
    char port[20];
    char nick[150];
    char own_nick[150];
    char recv_buffer[BUFFERSIZE];
    char buffer[BUFFERSIZE];
    char *str;
    long dcc_socket;
    long dcc_listen_socket;
    struct dcc_chat *next;
    int connected;
    int removed;
    struct sockaddr_in test;
    struct sockaddr_in their_addr;
} *dcc_chat_root,*dcc_chat_conductor; //,*dcc_chat_send_root,*dcc_chat_send_conductor;


long dcc_socket;
long dcc_listen_socket;
struct sockaddr_in their_addr;

struct dcc_entry *find=new dcc_entry;


struct List temp_list;

int string_history_number;

int string_id=2;

#define GAD_STRING 1;




/* GUI */

void DisposeApp(struct ObjApp * MBObj)
{
	MUI_DisposeObject((Object*)MBObj->App);
	FreeVec(MBObj);
}


APTR	MN1_Main, MN1_SelectServer, MN1_NewTAB, MN1_NewGTAB,MN1_CloseTAB, MNmenuBarLabel0,MNmenuBarLabel1, MN_ClearHistory, MN_SaveHistory, MN_MainSettings;
APTR	MN_Settings, MN_MUISettings, MN_ColourSettings, MN_windows_dcc2,  MN_windows_dcc, MN_windows, MN_urlgrabber, MN_about, MN_quit, MN_MultiColumnDisplay;



APTR	GROUP_ROOT_0, GROUP_ROOT_1, GR_top;
APTR	GR_string_gadget, GR_bottom_group, GR_buttons, GR_virtual_buttons, GR_samples_path;
APTR	LA_space;

	APTR	GROUP_ROOT_8;

	APTR	GROUP_ROOT_3, GR_grp_0, LA_nicknames, GR_grp_1, LA_realname, GR_grp_2;
	APTR	LA_username, GR_grp_3, REC_label_0, GR_grp_8, GR_grp_7, REC_label_0CC;
	APTR	GR_grp_9, BT_edit, GR_grp_6;
    APTR    GR_send_dcc, GR_dcc, GR_dcc_file_exists;

    // BELOW are edit window variables
	APTR	GROUP_ROOT_4, GR_grp_13, LA_servername, GR_grp_11, LA_port, LA_pass, GR_grp_12;
	APTR	GROUP_ROOT_5, GR_grp_15, LA_groupname, GR_grp_16, GR_grp_17;

    // BELOW are colour setting window variables
    APTR    GROUP_ROOT_6;
    APTR    LA_CSW_background;
    APTR    LA_CSW_nicklistbackground;
    APTR    LA_CSW_join;
    APTR    LA_CSW_part;
    APTR    LA_CSW_quit;
    APTR    LA_CSW_modes;
    APTR    LA_CSW_ctcp;
    APTR    LA_CSW_actions;
    APTR    LA_CSW_normal;
    APTR    LA_CSW_info;
    APTR    LA_CSW_owntext;
    APTR    LA_CSW_highlight;
    APTR    LA_CSW_notices;
    APTR    LA_CSW_invite; //13
    APTR    LA_CSW_kick;    //14
    APTR    LA_CSW_nickchange;  //15
    APTR    LA_CSW_topic;     //16
    APTR    LA_CSW_wallops;    //17
    APTR    LA_CSW_activity;
    APTR    LA_CSW_chatactivity;
    APTR    LA_CSW_highlightactivity;
    APTR    LA_CSW_nicklisttext;
    APTR    PenMasterObject;


	APTR	URLGRABBER_ROOT, URLGRABBER_GRP1, ABOUT_ROOT, GROUP_ROOT_2, GROUP_ROOT_2_SECOND, GROUP_ROOT_2_DCC,GROUP_ROOT_2_ACTIONS, GR_grp_20, GR_grp_21, GR_grp_22, obj_aux0, obj_aux1;
	APTR	obj_aux2, obj_aux3, REC_label_1, GR_grp_23, obj_aux4, obj_aux5, obj_aux6;
	APTR	obj_aux7, REC_label_2, GR_grp_24, obj_aux8, obj_aux9, obj_aux10, obj_aux11;
	APTR	obj_aux12, obj_aux13, obj_aux14, obj_aux15, REC_label_3, GR_grp_25;
	APTR	obj_aux16, obj_aux17, obj_aux18, obj_aux19, REC_label_4, GR_grp_26, GR_grp_reload_log;
	APTR	obj_aux20, obj_aux21, GR_grp_27, obj_aux22, obj_aux23, REC_label_5;
	APTR	GR_grp_28, GR_grp_31, GR_dcc_grp_28, GR_dcc_grp_29,REC_label_6,REC_label_7, GR_grp_29;
    APTR    obj_aux24, obj_aux25, GR_grp_32;
    APTR    obj_settings1_dcc, obj_settings2_dcc, obj_gethostid_dcc, obj_ipaddr_dcc, obj_port1_dcc, obj_port2_dcc;
    APTR    grp_dcc_settings1, grp_dcc_settings2,grp_dcc_settings3, grp_dcc_settings4, grp_dcc_settings5;
    APTR    GROUP_ROOT_2_ALIASES, GR_alias1, GR_alias2;
    APTR    GROUP_ROOT_7, GROUP_ROOT_9;

	APTR	GROUP_ROOT_10, GR_grp_19, LA_dcc_send_file, GR_grp_30, LA_dcc_send_nick;
    APTR    QUIT_ROOT, quit_label;

    APTR source, instance;
    APTR source2, instance2;
	struct BitMap *bitmap2, *bitmap, *bitmap3;
	ULONG width, height;


struct graphical_smilies_struct
{
    APTR    icon_smile;
    APTR    icon_smile_num;
} graphical_smilies;

APTR icon_smile;

struct ObjApp *CreateApp(void)
{

	BT_blank = (Object*)TextObject,
		ButtonFrame,
		//MUIA_Weight, 0,
		MUIA_MaxWidth, 150,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Text_Contents, "new button",
		MUIA_Text_PreParse, "\033c\0332",
		MUIA_HelpNode, "BT_query2",
        MUIA_InputMode, MUIV_InputMode_Toggle,
	End;

	struct ObjApp *MBObj;

	if (!(MBObj = (struct ObjApp *) AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
		return(NULL);

    static const char *titles2[] = { "General", "Logging", "DCC", "Sounds", "Aliases", NULL };
	
    /*MBObj->LV_about = (Object*)NListObject,
		MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
    End;

    MBObj->LV_about = (Object*)NListviewObject,
		MUIA_HelpNode, "LV_channel",
		MUIA_Weight, 80,
		MUIA_NListview_NList, MBObj->LV_about,
	End;
    */


    char about_text[2000];
    //sprintf(about_text,"WookieChat %s (c) 2004-2005\nReleased 5th November 2005\nAuthor: James Carroll\ne-mail: james.carroll@xnet.co.nz\nhttp://wookiechat.amigarevolution.com\n",verno);
    sprintf(about_text,VERSION_ABOUT);

    MBObj->TX_about = TextObject,
        MUIA_Text_Contents,about_text,
        MUIA_Weight, 100,
    End;

    MBObj->TX_about2 = TextObject,
        MUIA_Text_Contents,"Proudly sponsored by Guru Meditation\nAmiga reseller and Linux consulting\nhttp://gurumeditation.se/",
        MUIA_Weight, 100,
	End;

#ifdef __amigaos4__

    struct Screen * my_screen = LockPubScreen(NULL);
    UnlockPubScreen(NULL, my_screen);


	if (source = ObtainBitMapSource("progdir:guru_logo2.png",TAG_END))
	{

	   if (instance = ObtainBitMapInstance(source,my_screen,TAG_END))
	   {
	      BitMapInstanceControl(instance,BMICTRL_GetBitMap,&bitmap,
	                                     BMICTRL_GetWidth,&width,
	                                     BMICTRL_GetHeight,&height,
	                                     TAG_END);

	   }
	}

    MBObj->guru_meditation_img_obj = (Object*)BodychunkObject,
        MUIA_Bitmap_Bitmap, bitmap,
        MUIA_Bitmap_Height, height,
        MUIA_Bitmap_Width, width,
        MUIA_Weight, 100,
    End;


	if (source2 = ObtainBitMapSource("progdir:wookiechat_logo.gif",TAG_END))
	{

	   if (instance2 = ObtainBitMapInstance(source2,my_screen,TAG_END))
	   {
	      BitMapInstanceControl(instance2,BMICTRL_GetBitMap,&bitmap2,
	                                     BMICTRL_GetWidth,&width,
	                                     BMICTRL_GetHeight,&height,
	                                     TAG_END);

	   }
	}

    MBObj->wookiechat_img_obj = (Object*)BodychunkObject,
        MUIA_Bitmap_Bitmap, bitmap2,
        MUIA_Bitmap_Height, height,
        MUIA_Bitmap_Width, width,
        MUIA_Weight, 100,
    End;


	/*if (source = ObtainBitMapSource("progdir:icon_smile.gif",TAG_END))
	{

	   if (instance = ObtainBitMapInstance(source,my_screen,TAG_END))
	   {
	      BitMapInstanceControl(instance,BMICTRL_GetBitMap,&bitmap3,
	                                     BMICTRL_GetWidth,&width,
	                                     BMICTRL_GetHeight,&height,
	                                     TAG_END);

	   }
	}

    icon_smile = (Object*)BodychunkObject,
        MUIA_Bitmap_Bitmap, bitmap3,
        MUIA_Bitmap_Height, height,
        MUIA_Bitmap_Width, width,
        MUIA_Weight, 100,
    End;*/



    APTR about_group1 = GroupObject,
		Child, MBObj->wookiechat_img_obj,
        Child, MBObj->TX_about,
        //Child, icon_smile,
        MUIA_Weight,100,

    End;

    APTR about_group2 = GroupObject,
		Child, MBObj->guru_meditation_img_obj,
        Child, MBObj->TX_about2,
        MUIA_Weight,100,

    End;

#else

    APTR about_group1 = GroupObject,
		//Child, MBObj->wookiechat_img_obj,
        Child, MBObj->TX_about,
        MUIA_Weight,100,

    End;

    APTR about_group2 = GroupObject,
		//Child, MBObj->guru_meditation_img_obj,
        Child, MBObj->TX_about2,
        MUIA_Weight,100,

    End;

#endif

    /*
    APTR    WI_urlgrabber;
    APTR    LV_urlgrabber;
    APTR    BT_urlgrabber_clear;
    */


	MBObj->LV_urlgrabber = (Object*) NListObject,
		MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        //MUIA_NList_DisplayHook, &DisplayLI_channel_TextHook,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input,TRUE,
        MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
	End;

    MBObj->LV_urlgrabber = (Object*)NListviewObject,
		MUIA_Weight, 100,
		MUIA_NListview_NList, MBObj->LV_urlgrabber,
	End;

    MBObj->BT_urlgrabber_clear=SimpleButton("Remove URL(s) From List");
    MBObj->BT_urlgrabber_clearall=SimpleButton("Clear All");

    URLGRABBER_GRP1 = GroupObject,
		MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_urlgrabber_clear,
        Child, MBObj->BT_urlgrabber_clearall,
    End;

    URLGRABBER_ROOT = GroupObject,
		MUIA_Group_Horiz, FALSE,
        Child, MBObj->LV_urlgrabber,
        Child, URLGRABBER_GRP1,
    End;

	MBObj->WI_urlgrabber = WindowObject,
		MUIA_Window_Title, "URL Grabber - Double Click to Open",
		MUIA_Window_CloseGadget, TRUE,
		MUIA_Window_ID, MAKE_ID('7', '1', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, URLGRABBER_ROOT,
	End;

    ABOUT_ROOT = GroupObject,
        MUIA_Background, "2:ffffffff,ffffffff,ffffffff", // MUII_SHINE,
        Child, about_group1,
        Child, about_group2,
    End;

	MBObj->WI_about = WindowObject,
		MUIA_Window_Title, "About",
		MUIA_Window_CloseGadget, TRUE,
		MUIA_Window_ID, MAKE_ID('7', 'W', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, ABOUT_ROOT,
	End;


    // quit yes/no window

	quit_label = Label("Quit Program?");
	
    MBObj->BT_quit_yes = SimpleButton("Yes");
    MBObj->BT_quit_no = SimpleButton("No");

    QUIT_ROOT = GroupObject,
		MUIA_Group_Columns, 3,
        Child, quit_label,
        Child, MBObj->BT_quit_yes,
        Child, MBObj->BT_quit_no,
    End;

	MBObj->WI_quit = WindowObject,
		MUIA_Window_Title, "Quit",
		MUIA_Window_CloseGadget, FALSE,
		MUIA_Window_ID, MAKE_ID('9', 'W', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, QUIT_ROOT,
	End;


    //dcc send initiate window

	LA_dcc_send_file = Label("File to send");

	MBObj->STR_PA_dcc_send_file = String("", 800);

	MBObj->PA_dcc_send_file = PopButton(MUII_PopFile);

	MBObj->PA_dcc_send_file = PopaslObject,
		MUIA_HelpNode, "PA_dcc_send_file",
		MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,"Choose filename to send",
        ASLFR_InitialDrawer,"ram:",
		MUIA_Popstring_String, MBObj->STR_PA_dcc_send_file,
		MUIA_Popstring_Button, MBObj->PA_dcc_send_file,
	End;

	GR_grp_19 = GroupObject,
		MUIA_HelpNode, "GR_grp_19",
		MUIA_Group_Columns, 2,
		Child, LA_dcc_send_file,
		Child, MBObj->PA_dcc_send_file,
	End;

	LA_dcc_send_nick = Label("Nick");


    MBObj->STR_dcc_send_nick = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_dcc_send_nick",
	End;

    /*
    MBObj->STR_dcc_send_nick = (Object*)BS_Object,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_String_MaxLen, 100,
    	End;
    */

	GR_grp_30= GroupObject,
		MUIA_HelpNode, "GR_grp_1",
		MUIA_Group_Columns, 2,
		Child, LA_dcc_send_nick,
		Child, MBObj->STR_dcc_send_nick,
	End;

	MBObj->BT_dcc_send = SimpleButton("Send File");

	GROUP_ROOT_10 = GroupObject,
		Child, GR_grp_19,
		Child, GR_grp_30,
		Child, MBObj->BT_dcc_send,
	End;

	MBObj->WI_dcc_send = WindowObject,
		MUIA_Window_Title, "DCC Send File",
		MUIA_Window_ID, MAKE_ID('9', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_10,
	End;

//


    MBObj->LV_send_dcc = (Object*)NListObject,
		MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, &ConstructDCC_TextHook,
        MUIA_NList_DestructHook, &DestructDCC_TextHook,
        MUIA_NList_DisplayHook, &DisplayDCC_send_TextHook,
        MUIA_NList_Exports, MUIV_NList_Exports_ColWidth,
        MUIA_NList_Imports, MUIV_NList_Imports_ColWidth,
        MUIA_NList_Format,"WEIGHT=19 BAR,WEIGHT=15 BAR,WEIGHT=55 BAR,WEIGHT=10 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        MUIA_NList_Title, TRUE,
	End;
	
    MBObj->LV_send_dcc = (Object*)NListviewObject,
		MUIA_HelpNode, "LV_channel",
		MUIA_Weight, 80,
		MUIA_NListview_NList, MBObj->LV_send_dcc,
	End;

    MBObj->BT_dcc_send_reoffer = SimpleButton("Reoffer file");
	MBObj->BT_dcc_send_cancel = SimpleButton("Cancel Send");
    MBObj->BT_dcc_send_remove = SimpleButton("Remove");
    MBObj->BT_dcc_send_new = SimpleButton("Send File");

	GR_send_dcc = GroupObject,
		MUIA_Group_Columns, 4,
        Child, MBObj->BT_dcc_send_new,
		Child, MBObj->BT_dcc_send_reoffer,
		Child, MBObj->BT_dcc_send_cancel,
        Child, MBObj->BT_dcc_send_remove,
	End;

	MBObj->BT_dcc_resume = SimpleButton("Resume");
	MBObj->BT_dcc_overwrite = SimpleButton("Overwrite");
	MBObj->BT_dcc_abort = SimpleButton("Abort");

	GR_dcc_file_exists = GroupObject,
		MUIA_Group_Columns, 4,
		Child, MBObj->BT_dcc_resume,
		Child, MBObj->BT_dcc_overwrite,
		//Child, MBObj->BT_dcc_rename,
		Child, MBObj->BT_dcc_abort,
	End;

    MBObj->TX_oldname = TextObject,End;
    MBObj->TX_oldsize = TextObject,End;
    MBObj->TX_newsize = TextObject,End;

	GR_dcc_grp_29 = GroupObject,
		MUIA_HelpNode, "GR_dcc_grp_29",
		Child, MBObj->TX_oldname,
		Child, MBObj->TX_oldsize,
		Child, MBObj->TX_newsize,
	End;

	GROUP_ROOT_8 = GroupObject,
        Child, GR_dcc_grp_29,
	    Child, GR_dcc_file_exists,
    End;

	MBObj->WI_dcc_file_exists = WindowObject,
		MUIA_Window_Title, "Incoming File already exists",
		MUIA_Window_ID, MAKE_ID('6', 'W', 'I', 'N'),
		MUIA_Window_CloseGadget, FALSE,
		MUIA_Window_NoMenus, FALSE,
		WindowContents, GROUP_ROOT_8,
	End;

	MBObj->STR_dcc_PA_label_0 = String("", 200);

	MBObj->PA_dcc_label_0 = PopButton(MUII_PopDrawer);

	MBObj->PA_dcc_label_0 = PopaslObject,
        ASLFR_DrawersOnly, TRUE,
		MUIA_HelpNode, "Choose a path to save your downloads to",
		MUIA_Popasl_Type, 0,
		MUIA_Popstring_String, MBObj->STR_dcc_PA_label_0,
		MUIA_Popstring_Button, MBObj->PA_dcc_label_0,
	End;


	GR_dcc_grp_28 = GroupObject,
		MUIA_HelpNode, "GR_dcc_grp_28",
		Child, MBObj->PA_dcc_label_0,
	End;



	MBObj->BT_dcc_accept = SimpleButton("Accept");
	MBObj->BT_dcc_cancel = SimpleButton("Cancel");
	MBObj->BT_dcc_remove = SimpleButton("Remove");

	GR_dcc = GroupObject,
		MUIA_Group_Columns, 3,
		Child, MBObj->BT_dcc_accept,
		Child, MBObj->BT_dcc_cancel,
		Child, MBObj->BT_dcc_remove,
	End;


	MBObj->LV_dcc = (Object*)NListObject,// NListObject,
		MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, &ConstructDCC_TextHook,
        MUIA_NList_DestructHook, &DestructDCC_TextHook,
        MUIA_NList_DisplayHook, &DisplayDCC_recv_TextHook,
        MUIA_NList_Exports, MUIV_NList_Exports_ColWidth,
        MUIA_NList_Imports, MUIV_NList_Imports_ColWidth,
        MUIA_NList_Format,"WEIGHT=19 BAR,WEIGHT=15 BAR,WEIGHT=55 BAR,WEIGHT=10 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        MUIA_NList_Title, TRUE,
	End;

    MBObj->LV_dcc = (Object*)NListviewObject,
		MUIA_HelpNode, "LV_channel",
		MUIA_Weight, 80,
		MUIA_NListview_NList, MBObj->LV_dcc,
	End;

	GROUP_ROOT_7 = GroupObject,
        //MUIA_Frame, MUIV_Frame_Group,
        //MUIA_FrameTitle, "Incoming File Transfers",
        Child, GR_dcc_grp_28,
		Child, MBObj->LV_dcc,
	    Child, GR_dcc,
    End;

	GROUP_ROOT_9 = GroupObject,
        //MUIA_Frame, MUIV_Frame_Group,
        //MUIA_FrameTitle, "Outgoing File Transfers",
		Child, MBObj->LV_send_dcc,
	    Child, GR_send_dcc,
    End;

    /*MBObj->DCC_Register_Grp = GroupObject, //RegisterGroup(titles),
        Child,GROUP_ROOT_7, //incoming dcc group
        Child,GROUP_ROOT_9, //outgoing dcc group
    End;*/

	MBObj->WI_dcc = WindowObject,
		MUIA_Window_Title, "Incoming File Transfers",
		MUIA_Window_ID, MAKE_ID('5', 'W', 'I', 'N'),
		MUIA_Window_NoMenus, FALSE,
		WindowContents, GROUP_ROOT_7, //incoming dcc group
        //MBObj->DCC_Register_Grp,
	End;

	MBObj->WI_dcc2 = WindowObject,
		MUIA_Window_Title, "Outgoing DCC File Transfers",
		MUIA_Window_ID, MAKE_ID('8', 'W', 'I', 'N'),
		MUIA_Window_NoMenus, FALSE,
		WindowContents, GROUP_ROOT_9, //outgoing dcc group
        //MBObj->DCC_Register_Grp,
	End;


    //below is the main settings window and objects

	MBObj->RA_nick_completion_style_Label= Label2("Nick Completion Style");

    static const char *RA_nick_completion_style_Titles[] =
	{
	   "AmIRC", "mIRC", NULL
	};

    MBObj->RA_nick_completion_style = (Object*)RadioObject,
	   MUIA_Radio_Entries, RA_nick_completion_style_Titles,
		MUIA_ShortHelp, "AmIRC style puts a colon and a space\nafter users nicks if used at the beginning\nof a line, or just a space afterwards\nanywhere else.\nmIRC style: No colon\nor spaces are used, only the nick is\ndisplayed",
    End;

    GR_grp_32=(Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, MBObj->RA_nick_completion_style_Label,
        Child, MBObj->RA_nick_completion_style,
    End;

    static const char *RA_GroupTitleColor[] =
	{
	   "Overwrite", "Resume", "Abort", "Ask", NULL
	};

    MBObj->RA_autoaccept = (Object*)RadioObject,
	   MUIA_Radio_Entries, RA_GroupTitleColor,
    End;

	MBObj->CH_autoaccept = CheckMark(FALSE);

	MBObj->LV_trusted_nicks = (Object*)NewObject(mcc2->mcc_Class,NULL,// NListObject,
		MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_ListBackground, background2,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
	End;

	MBObj->CH_label_6 = CheckMark(FALSE);

	obj_aux24 = Label2("Auto-rejoin channels when kicked");

	MBObj->CH_label_1 = CheckMark(FALSE);

	obj_aux1 = Label2("Auto-open private query tabs when messaged");

    obj_aux25 = GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux24,
        Child, MBObj->CH_label_6,
    End;

	obj_aux0 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux1,
		Child, MBObj->CH_label_1,
	End;

	MBObj->STR_label_3 = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_label_3",
		MUIA_String_Accept, "-0123456789",
		MUIA_String_MaxLen, 10,
	End;

	obj_aux3 = Label2("Maximum number of lines to keep in buffer");

	obj_aux2 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux3,
		Child, MBObj->STR_label_3,
	End;

	GR_grp_22 = GroupObject,
		MUIA_HelpNode, "GR_grp_22",
		Child, obj_aux25,
        Child, obj_aux0,
		Child, obj_aux2,
        Child, GR_grp_32, //nick completion style group
	End;

	REC_label_1 = RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	MBObj->CH_label_2 = CheckMark(FALSE);

	obj_aux5 = Label2("Log channels");

	obj_aux4 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux5,
		Child, MBObj->CH_label_2,
	End;

	MBObj->CH_label_3 = CheckMark(FALSE);

	obj_aux7 = Label2("Log private chats");

	obj_aux6 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux7,
		Child, MBObj->CH_label_3,
	End;

	GR_grp_23 = GroupObject,
		MUIA_HelpNode, "GR_grp_23",
		MUIA_FrameTitle, "Logging",
		MUIA_Group_Columns, 2,
		Child, obj_aux4,
		Child, obj_aux6,
	End;

	REC_label_2 = RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	MBObj->STR_label_1 = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_label_1",
		MUIA_String_MaxLen, 800,
	End;

	obj_aux9 = Label2("Default Kick Message");

	obj_aux8 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux9,
		Child, MBObj->STR_label_1,
	End;

	MBObj->STR_label_2 = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_label_2",
		MUIA_String_MaxLen, 800,
	End;

	obj_aux11 = Label2("Default Ban Message");

	obj_aux10 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux11,
		Child, MBObj->STR_label_2,
	End;

	MBObj->STR_label_4 = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_label_4",
		MUIA_String_MaxLen, 400,
	End;

	obj_aux13 = Label2("Quit Message");

	obj_aux12 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux13,
		Child, MBObj->STR_label_4,
	End;

	MBObj->STR_label_5 = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_label_5",
		MUIA_String_MaxLen, 400,
	End;

	obj_aux15 = Label2("Part Message");

	obj_aux14 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux15,
		Child, MBObj->STR_label_5,
	End;

	GR_grp_24 = GroupObject,
		MUIA_HelpNode, "GR_grp_24",
		MUIA_FrameTitle, "Messages",
		Child, obj_aux8,
		Child, obj_aux12,
		Child, obj_aux14,
	End;

	REC_label_3 = RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	MBObj->CH_label_4 = CheckMark(FALSE);

	obj_aux17 = Label2("Request Whois information");

	obj_aux16 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux17,
		Child, MBObj->CH_label_4,
	End;

	MBObj->CH_label_5 = CheckMark(FALSE);

	obj_aux19 = Label2("Open private query tab");

	obj_aux18 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux19,
		Child, MBObj->CH_label_5,
	End;

	GR_grp_25 = GroupObject,
		MUIA_HelpNode, "GR_grp_25",
		MUIA_FrameTitle, "Nicklist doubleclick action(s)",
		MUIA_Group_Columns, 2,
		Child, obj_aux16,
		Child, obj_aux18,
	End;

	REC_label_4 = RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	MBObj->STR_label_0 = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp,"Customise your timestamps! use %h for the hour, %m for the minute, and %s for the second. This is useful to see what time an event happened (such as a user joining, or speaking)",
        MUIA_HelpNode, "STR_label_0",
		MUIA_String_MaxLen, 50,
	End;

	obj_aux21 = Label2("Timestamp Format");

    MBObj->TX_highlight_label = Label2("Words to Highlight");

	MBObj->STR_highlight_label = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp,"Enter words here for WookieChat to watch for. Lines of text will appear in the highlight colour if one of these words is found. Enter the words you want to highlight, separated by spaces",
		MUIA_String_MaxLen, 800,
	End;

	//obj_aux20 = GroupObject,
	
    GR_grp_26 = GroupObject,
		MUIA_Group_Columns, 2,
		MUIA_FrameTitle, "Highlight",
		Child, MBObj->TX_highlight_label,
		Child, MBObj->STR_highlight_label,
	End;

	/*GR_grp_26 = GroupObject,
		MUIA_HelpNode, "GR_grp_26",
		Child, obj_aux20,
	End;*/

	MBObj->CH_label_0 = CheckMark(FALSE);

	obj_aux23 = Label2("Use timestamps");

	obj_aux22 = GroupObject,
        MUIA_FrameTitle, "Timestamps",
		MUIA_Group_Columns, 4,
		Child, obj_aux23,
		Child, MBObj->CH_label_0,
		Child, obj_aux21,
		Child, MBObj->STR_label_0,
	End;

	GR_grp_27 = GroupObject,
		MUIA_HelpNode, "GR_grp_27",
		Child, obj_aux22,
	End;

	REC_label_5 = RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	MBObj->STR_PA_label_0 = String("", 400);

	MBObj->PA_label_0 = PopButton(MUII_PopFile);

	MBObj->PA_label_0 = PopaslObject,
		MUIA_HelpNode, "PA_label_0",
        MUIA_ShortHelp, "This text editor is used when you edit your \"Perform on Connect\" scripts (from the Server Selection window)",
		MUIA_Popstring_String, MBObj->STR_PA_label_0,
		MUIA_Popstring_Button, MBObj->PA_label_0,        
	End;

	REC_label_6 = RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	MBObj->LA_texteditor = Label2("Text Editor");

	GR_grp_28 = GroupObject,
		MUIA_HelpNode, "GR_grp_28",
		MUIA_Group_Horiz, TRUE,
		//MUIA_FrameTitle, "Text Editor",
		Child, MBObj->LA_texteditor,
		Child, MBObj->PA_label_0,
	End;


	MBObj->PA_browser_label = Label2("Web Browser");

	MBObj->PA_browser = PopButton(MUII_PopFile);

	MBObj->STR_browser = (Object*)StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "Choose a web browser to use with WookieChat's \"URL Grabber\" Window. Double clicking an address there will load in this chosen browser",
		MUIA_String_MaxLen, 1000,
	End;

	MBObj->PA_browser = PopaslObject,
		//MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_DrawersOnly, TRUE,
        ASLFR_TitleText,"Choose browser to use",
        ASLFR_InitialDrawer,"progdir:",
		MUIA_Popstring_String, MBObj->STR_browser,
		MUIA_Popstring_Button, MBObj->PA_browser,
	End;

	GR_grp_31 = GroupObject,
		MUIA_HelpNode, "GR_grp_31",
		MUIA_Group_Horiz, TRUE,
		//MUIA_FrameTitle, "Web Browser",
        Child, MBObj->PA_browser_label,
		Child, MBObj->PA_browser,
	End;

	MBObj->BT_label_0 = SimpleButton("Save");

	MBObj->BT_label_1 = SimpleButton("Use");

	MBObj->BT_label_2 = SimpleButton("Cancel");

	GR_grp_29 = GroupObject,
		MUIA_HelpNode, "GR_grp_29",
		MUIA_Group_Columns, 3,
		Child, MBObj->BT_label_0,
		Child, MBObj->BT_label_1,
		Child, MBObj->BT_label_2,
	End;

	GR_grp_21 = GroupObject,
		MUIA_HelpNode, "GR_grp_21",
		Child, GR_grp_22,
		Child, GR_grp_24,
		Child, GR_grp_25,
        Child, GR_grp_26,
		Child, GR_grp_27,
		Child, GR_grp_28,
        Child, GR_grp_31,
	End;

	GR_grp_20 = GroupObject,
		MUIA_HelpNode, "GR_grp_20",
		Child, GR_grp_21,
	End;

	obj_settings1_dcc = Label2("Auto accept incoming DCC File Transfers");
	obj_settings2_dcc = Label2("Action to take if file exists");

    obj_gethostid_dcc = Label2("Use local address for DCC connections");
	MBObj->CH_gethostid = CheckMark(FALSE);

    obj_ipaddr_dcc = Label2("IP Address");
	MBObj->STR_ipaddr_dcc = (Object*)StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "If WookieChat is not obtaining the correct address automatically with the above option, then specify your IP address here",
	End;

    obj_port1_dcc = Label2("DCC port range from ");
    obj_port2_dcc = Label2(" to ");

	MBObj->STR_port1_dcc = (Object*)StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "This is the start of the port range for incoming DCC connections. This is configurable so you can choose which ports to use and open up on any firewall you might have.",
	End;

	MBObj->STR_port2_dcc = (Object*)StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "This is the end of the port range for incoming DCC connections. This is configurable so you can choose which ports to use and open up on any firewall you might have.",
	End;


	REC_label_7 = RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

    grp_dcc_settings1 = GroupObject,
		MUIA_Group_Columns, 2,
        Child, obj_settings1_dcc,
    	Child, MBObj->CH_autoaccept,
    End;

    grp_dcc_settings2 = GroupObject,
		MUIA_Group_Columns, 1,
        Child, obj_settings2_dcc,
        Child, MBObj->RA_autoaccept,
    End;

    grp_dcc_settings3 = GroupObject,
		MUIA_Group_Columns, 2,
        Child, obj_gethostid_dcc,
        Child, MBObj->CH_gethostid,
    End;

    grp_dcc_settings4 = GroupObject,
		MUIA_Group_Columns, 2,
        Child, obj_ipaddr_dcc,
        Child, MBObj->STR_ipaddr_dcc,
    End;

    grp_dcc_settings5 = GroupObject,
		MUIA_Group_Columns, 4,
        Child,obj_port1_dcc,
        Child,MBObj->STR_port1_dcc,
        Child,obj_port2_dcc,
        Child,MBObj->STR_port2_dcc,
    End;

    GROUP_ROOT_2_DCC = GroupObject,
		//MUIA_Group_Columns, 2,
    	//Child, MBObj->LV_trusted_nicks,
        Child, grp_dcc_settings1,
        Child, grp_dcc_settings2,
        Child, grp_dcc_settings3,
        //Child, grp_dcc_settings4,
        Child, grp_dcc_settings5,
    End;

	GROUP_ROOT_2 = GroupObject,
	    MUIA_Weight, 100,
    	Child, GR_grp_20,
	End;


    //tabopen action entries

    static const char *RA_sound_sample_tabopen_entries[] =
	{
	   "Never", "Always", "Only when Window is inactive", NULL
	};


	MBObj->LA_sound_sample_tabopen = Label2("Sample to play when a tab opens");

    MBObj->STR_sound_sample_tabopen = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, " ",
	End;

	MBObj->PA_sound_sample_tabopen_label = Label2("Play sample");

	MBObj->PA_sound_sample_tabopen = PopButton(MUII_PopFile);

	MBObj->PA_sound_sample_tabopen = PopaslObject,
		MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,"Choose filename",
        ASLFR_InitialDrawer,"progdir:",
		MUIA_Popstring_String, MBObj->STR_sound_sample_tabopen,
		MUIA_Popstring_Button, MBObj->PA_sound_sample_tabopen,
	End;

    MBObj->RA_sound_sample_tabopen = (Object*)RadioObject,
	   MUIA_Radio_Entries, RA_sound_sample_tabopen_entries,
    End;


    //highlight action objects

    static const char *RA_sound_sample_highlight_entries[] =
	{
	   "Never", "Always", "Only when Window is inactive", NULL
	};

	MBObj->LA_sound_sample_highlight = Label2("Sample to play on highlight");

    MBObj->STR_sound_sample_highlight = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, " ",
	End;
	
	MBObj->PA_sound_sample_highlight_label = Label2("Play sample");

    MBObj->PA_sound_sample_highlight = PopButton(MUII_PopFile);

	MBObj->PA_sound_sample_highlight = PopaslObject,
		MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,"Choose filename",
        ASLFR_InitialDrawer,"progdir:",
		MUIA_Popstring_String, MBObj->STR_sound_sample_highlight,
		MUIA_Popstring_Button, MBObj->PA_sound_sample_highlight,
	End;

    MBObj->RA_sound_sample_highlight = (Object*)RadioObject,
	   MUIA_Radio_Entries, RA_sound_sample_highlight_entries,
    End;


	MBObj->LA_samples_path = Label2("Samples Path for CTCP SOUND");
	
    MBObj->STR_samples_path = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "When you recieve a CTCP SOUND request, WookieChat will\nlook in this path for sound samples",
		MUIA_String_MaxLen, 400,
	End;

    MBObj->PA_samples_path = PopButton(MUII_PopDrawer);

	MBObj->PA_samples_path = PopaslObject,
		MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,"Choose directory",
        ASLFR_InitialDrawer,"progdir:",
		MUIA_Popstring_String, MBObj->STR_samples_path,
		MUIA_Popstring_Button, MBObj->PA_samples_path,
	End;


//
    /*GR_samples_path = GroupObject,
		MUIA_Group_Columns, 2,
        Child, MBObj->LA_samples_path,
        Child, MBObj->STR_samples_path,
    End;
    */

    GROUP_ROOT_2_ACTIONS = GroupObject,
		MUIA_Group_Columns, 2,
        Child, MBObj->LA_sound_sample_tabopen,
        Child, MBObj->PA_sound_sample_tabopen,
        Child, MBObj->PA_sound_sample_tabopen_label,
        Child, MBObj->RA_sound_sample_tabopen,
        Child, MBObj->LA_sound_sample_highlight,
        Child, MBObj->PA_sound_sample_highlight,
        Child, MBObj->PA_sound_sample_highlight_label,
        Child, MBObj->RA_sound_sample_highlight,
        Child, MBObj->LA_samples_path,
        Child, MBObj->PA_samples_path,
    End;

    // log reloading gui objects


    MBObj->TX_how_many_lines_to_reload = TextObject,
        MUIA_Text_Contents, "# of lines to display from the end of logfiles when opening a tab\n(Put 0 to disable this feature)",
	End;

	APTR obj_how_many_lines_to_reload_channel = Label2("channel");

    MBObj->STR_how_many_lines_to_reload_channel = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "How many lines to reload from the end of log files when you join a channel",
		MUIA_String_Accept, "0123456789",
		MUIA_String_MaxLen, 4,
	End;

	APTR obj_how_many_lines_to_reload_private = Label2("private");

    MBObj->STR_how_many_lines_to_reload_private = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, "How many lines to reload from the end of log files when you open a private query tab to another user",
		MUIA_String_Accept, "0123456789",
		MUIA_String_MaxLen, 4,
	End;

    APTR GR_grp_reload_log2 = GroupObject,
		MUIA_Group_Columns, 4,
        Child, obj_how_many_lines_to_reload_channel,
        Child, MBObj->STR_how_many_lines_to_reload_channel,
        Child, obj_how_many_lines_to_reload_private,
        Child, MBObj->STR_how_many_lines_to_reload_private,
    End;

    GR_grp_reload_log = GroupObject,
		MUIA_FrameTitle, "Log reloading",
        Child, MBObj->TX_how_many_lines_to_reload,
        Child, GR_grp_reload_log2,
    End;

	GROUP_ROOT_2_SECOND = GroupObject,
		Child, GR_grp_23,
	    Child, GR_grp_reload_log,
    End;

    MBObj->LV_alias = (Object*) NListObject,
		MUIA_ShortHelp, "Enter an alias name, then the command you want it to use\nE.g. With this line added to the alias list..\n/j /join\n..then the \"/j\" command will be replaced with \"/join\"",
		MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input,TRUE,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
	End;

    GR_alias1 = GroupObject,
        Child, MBObj->LV_alias,
    End;

	MBObj->BT_add_alias = (Object*)SimpleButton("Add alias >>");
	MBObj->BT_remove_alias = (Object*)SimpleButton("Remove alias");

	MBObj->STR_add_alias =(Object*) StringObject,
		MUIA_Frame, MUIV_Frame_String,
		//MUIA_ShortHelp, "",
	End;

    GR_alias2 = GroupObject,
		MUIA_Group_Columns, 3,
        Child, MBObj->BT_add_alias,
        Child, MBObj->STR_add_alias,
        Child, MBObj->BT_remove_alias,
    End;


    GROUP_ROOT_2_ALIASES = GroupObject,
		MUIA_Group_Horiz, FALSE,
        Child, GR_alias2,
        Child, GR_alias1,
    End;

    MBObj->SETTINGS_Register_Grp = RegisterGroup(titles2),
        Child, GROUP_ROOT_2,
        Child, GROUP_ROOT_2_SECOND,
        Child, GROUP_ROOT_2_DCC,
        Child, GROUP_ROOT_2_ACTIONS,
        Child, GROUP_ROOT_2_ALIASES,
    End;

    MBObj->SETTINGS_ROOT = GroupObject,
        Child, MBObj->SETTINGS_Register_Grp,
		Child, GR_grp_29,
    End;

	MBObj->WI_mainsettings = WindowObject,
		MUIA_Window_Title, "Settings",
		MUIA_Window_ID, MAKE_ID('4', 'W', 'I', 'N'),
		MUIA_Window_NoMenus, FALSE,
		MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_SizeGadget, TRUE,
        WindowContents, MBObj->SETTINGS_ROOT,
	End;



    //below is the colour settings window objects
	

    LA_CSW_background = (Object*)Label("background");

    MBObj->PP_CSW_background = (Object*)NewObject(mcc3->mcc_Class,NULL,
    	MUIA_Window_Title, "Custom Class Color",
	End;


	LA_CSW_nicklistbackground = (Object*)Label("nicklistbackground");

    MBObj->PP_CSW_nicklistbackground = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_join = (Object*)Label("join");

    MBObj->PP_CSW_join = (Object*)NewObject(mcc3->mcc_Class,NULL,
    	MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_part = (Object*)Label("part");

    MBObj->PP_CSW_part = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_quit = (Object*)Label("quit");

    MBObj->PP_CSW_quit = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_modes = (Object*)Label("modes");

    MBObj->PP_CSW_modes = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_ctcp = (Object*)Label("ctcp");

    MBObj->PP_CSW_ctcp = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_actions = (Object*)Label("actions");

    MBObj->PP_CSW_actions = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_normal = (Object*)Label("normal");

    MBObj->PP_CSW_normal = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_info = (Object*)Label("info");

    MBObj->PP_CSW_info = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_owntext = (Object*)Label("owntext");

    MBObj->PP_CSW_owntext = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_highlight = (Object*)Label("highlight");

    MBObj->PP_CSW_highlight = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_invite = (Object*)Label("invite");

    MBObj->PP_CSW_invite = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_kick = (Object*)Label("kick");

    MBObj->PP_CSW_kick = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_nickchange = (Object*)Label("nickchange");

    MBObj->PP_CSW_nickchange = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_topic = (Object*)Label("topic");

    MBObj->PP_CSW_topic = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_wallops = (Object*)Label("wallops");

    MBObj->PP_CSW_wallops = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_notices = (Object*)Label("notices");

    MBObj->PP_CSW_notices = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_activity = (Object*)Label("activity");

    MBObj->PP_CSW_activity = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_chatactivity = (Object*)Label("chatactivity");

    MBObj->PP_CSW_chatactivity = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_highlightactivity = (Object*)Label("highlightactivity");

    MBObj->PP_CSW_highlightactivity = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	LA_CSW_nicklisttext = (Object*)Label("nicklisttext");

    MBObj->PP_CSW_nicklisttext = (Object*)NewObject(mcc3->mcc_Class,NULL,
		MUIA_Window_Title, "Custom Class Color",
	End;

	GROUP_ROOT_6 = (Object*)GroupObject,
		MUIA_Group_Columns, 4,
        Child,    LA_CSW_background,
        Child,    MBObj->PP_CSW_background,
        Child,    LA_CSW_nicklistbackground,
        Child,    MBObj->PP_CSW_nicklistbackground,
        Child,    LA_CSW_nicklisttext,
        Child,    MBObj->PP_CSW_nicklisttext,
        Child,    LA_CSW_join,
        Child,    MBObj->PP_CSW_join,
        Child,    LA_CSW_part,
        Child,    MBObj->PP_CSW_part,
        Child,    LA_CSW_quit,
        Child,    MBObj->PP_CSW_quit,
        Child,    LA_CSW_modes,
        Child,    MBObj->PP_CSW_modes,
        Child,    LA_CSW_ctcp,
        Child,    MBObj->PP_CSW_ctcp,
        Child,    LA_CSW_actions,
        Child,    MBObj->PP_CSW_actions,
        Child,    LA_CSW_normal,
        Child,    MBObj->PP_CSW_normal,
        Child,    LA_CSW_info,
        Child,    MBObj->PP_CSW_info,
        Child,    LA_CSW_owntext,
        Child,    MBObj->PP_CSW_owntext,
        Child,    LA_CSW_highlight,
        Child,    MBObj->PP_CSW_highlight,
        Child,    LA_CSW_notices,
        Child,    MBObj->PP_CSW_notices,
        Child,    LA_CSW_invite,
        Child,    MBObj->PP_CSW_invite,
        Child,    LA_CSW_kick,
        Child,    MBObj->PP_CSW_kick,
        Child,    LA_CSW_nickchange,
        Child,    MBObj->PP_CSW_nickchange,
        Child,    LA_CSW_topic,
        Child,    MBObj->PP_CSW_topic,
        Child,    LA_CSW_wallops,
        Child,    MBObj->PP_CSW_wallops,
        Child,    LA_CSW_activity,
        Child,    MBObj->PP_CSW_activity,
        Child,    LA_CSW_chatactivity,
        Child,    MBObj->PP_CSW_chatactivity,
        Child,    LA_CSW_highlightactivity,
        Child,    MBObj->PP_CSW_highlightactivity,
    End;


	MBObj->WI_colour_settings = (Object*)WindowObject,
		MUIA_Window_Title, "Colour Settings",
		MUIA_Window_ID, MAKE_ID('3', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_6,
	End;


    //below are menu items

	MN1_SelectServer = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Select server",
		MUIA_Menuitem_Shortcut, "S",
	End;

	MN1_CloseTAB = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Close current tab",
		MUIA_Menuitem_Shortcut, "Z",
	End;

	MN1_NewTAB = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "New server tab",
		MUIA_Menuitem_Shortcut, "T",
	End;

	MN1_NewGTAB = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Open global query tab",
	End;

	MNmenuBarLabel0 = (Object*)MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

	MNmenuBarLabel1 = (Object*)MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

    MN_about = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "About",
	End;

    MN_quit = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Quit",
		MUIA_Menuitem_Shortcut, "Q",
	End;

	MN_ClearHistory = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Clear buffer",
	End;

	MN_SaveHistory = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Save buffer as",
	End;

	MN1_Main = (Object*)MenuObject,
		MUIA_Menu_Title, "Main",
		MUIA_Family_Child, MN1_SelectServer,
		MUIA_Family_Child, MN1_NewTAB,
		MUIA_Family_Child, MN1_NewGTAB,
		MUIA_Family_Child, MN1_CloseTAB,
		MUIA_Family_Child, MNmenuBarLabel0,
		MUIA_Family_Child, MN_ClearHistory,
		MUIA_Family_Child, MN_SaveHistory,
		MUIA_Family_Child, MNmenuBarLabel1,
		MUIA_Family_Child, MN_about,
		MUIA_Family_Child, MN_quit,
	End;

    MN_MainSettings  = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Main Settings",
		//MUIA_Menuitem_Shortcut, "S",
	End;

	MN_ColourSettings = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Colour Settings",
		//MUIA_Menuitem_Shortcut, "S",
	End;

	MN_MUISettings = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "MUI Settings",
		//MUIA_Menuitem_Shortcut, "S",
	End;

    MN_MultiColumnDisplay  = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Multi-column chat display",
        MUIA_Menuitem_Checkit, TRUE,
		//MUIA_Menuitem_Shortcut, "S",
	End;

	MN_Settings = (Object*)MenuObject,
		MUIA_Menu_Title, "Settings",
        MUIA_Family_Child, MN_MainSettings,
	    MUIA_Family_Child, MN_ColourSettings,
        MUIA_Family_Child, MN_MUISettings,
	    MUIA_Family_Child, MN_MultiColumnDisplay,
    End;

	MN_windows_dcc = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Incoming DCC File Transfers",
		MUIA_Menuitem_Shortcut, "D",
	End;

	MN_windows_dcc2 = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "Outgoing DCC File Transfers",
		MUIA_Menuitem_Shortcut, "R",
	End;

	MN_urlgrabber = (Object*)MenuitemObject,
		MUIA_Menuitem_Title, "URL Grabber",
		MUIA_Menuitem_Shortcut, "U",
	End;

	MN_windows = (Object*)MenuObject,
		MUIA_Menu_Title, "Windows",
        MUIA_Family_Child, MN_windows_dcc,
        MUIA_Family_Child, MN_windows_dcc2,
        MUIA_Family_Child, MN_urlgrabber,
    End;

	MBObj->MN_ = (Object*)MenustripObject,
		MUIA_Family_Child, MN1_Main,
		MUIA_Family_Child, MN_Settings,
	    MUIA_Family_Child, MN_windows,
    End;

    //below are edit window stuff

	LA_servername = (Object*)Label("Servername");

	MBObj->STR_servername = (Object*)StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_servername",
	End;

	GR_grp_13 = (Object*)GroupObject,
		MUIA_HelpNode, "GR_grp_13",
		MUIA_Group_Horiz, TRUE,
		Child, LA_servername,
		Child, MBObj->STR_servername,
	End;

	LA_port = (Object*)Label("Port number");

	MBObj->STR_port = (Object*)StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_port",
	End;

	GR_grp_11 = (Object*)GroupObject,
		MUIA_HelpNode, "GR_grp_11",
		MUIA_Group_Horiz, TRUE,
		Child, LA_port,
		Child, MBObj->STR_port,
	End;

    //
	LA_pass = (Object*)Label("Server password");

	MBObj->STR_pass = (Object*)StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_pass",
	End;

	GR_grp_17 = (Object*)GroupObject,
		MUIA_HelpNode, "GR_grp_17",
		MUIA_Group_Horiz, TRUE,
		Child, LA_pass,
		Child, MBObj->STR_pass,
	End;
    //

	MBObj->BT_acceptserver = (Object*)SimpleButton("Accept");

	MBObj->BT_cancelserver = (Object*)SimpleButton("Cancel");

	GR_grp_12 = (Object*)GroupObject,
		MUIA_HelpNode, "GR_grp_12",
		MUIA_Group_Horiz, TRUE,
		Child, MBObj->BT_acceptserver,
		Child, MBObj->BT_cancelserver,
	End;

	GROUP_ROOT_4 = (Object*)GroupObject,
		Child, GR_grp_13,
		Child, GR_grp_11,
        Child, GR_grp_17,
		Child, GR_grp_12,
	End;

	MBObj->WI_edit_server = (Object*)WindowObject,
		MUIA_Window_Title, "Edit Server",
		MUIA_Window_ID, MAKE_ID('2', 'W', 'I', 'N'),
		MUIA_Window_CloseGadget, FALSE,
		WindowContents, GROUP_ROOT_4,
	End;

	LA_groupname = (Object*)Label("Group name");

    MBObj->BT_perform = (Object*)SimpleButton("Perform on connect");

	MBObj->STR_groupname = (Object*)StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_groupname",
	End;

	GR_grp_15 =(Object*) GroupObject,
		MUIA_HelpNode, "GR_grp_15",
		MUIA_Group_Horiz, TRUE,
		Child, LA_groupname,
		Child, MBObj->STR_groupname,
	End;

	MBObj->BT_acceptgroup =(Object*) SimpleButton("Accept");

	MBObj->BT_cancelgroup =(Object*) SimpleButton("Cancel");

	GR_grp_16 =(Object*) GroupObject,
		MUIA_HelpNode, "GR_grp_16",
		MUIA_Group_Horiz, TRUE,
		Child, MBObj->BT_acceptgroup,
		Child, MBObj->BT_cancelgroup,
	End;

	GROUP_ROOT_5 =(Object*) GroupObject,
		Child, GR_grp_15,
		Child, GR_grp_16,
	End;

	MBObj->WI_edit_group =(Object*) WindowObject,
		MUIA_Window_Title, "Edit Group",
		MUIA_Window_ID, MAKE_ID('3', 'W', 'I', 'N'),
		MUIA_Window_CloseGadget, FALSE,
		WindowContents, GROUP_ROOT_5,
	End;

// above is edit window stuff

	LA_nicknames =(Object*) Label("Nicknames");

	MBObj->STR_nickname1 =(Object*) StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_nickname1",
        MUIA_String_Reject,"\\/#.,?*@!",
	End;

	MBObj->STR_nickname2 =(Object*) StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_nickname2",
        MUIA_String_Reject,"\\/#.,?*@!",
	End;

	MBObj->STR_nickname3 =(Object*) StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_nickname3",
        MUIA_String_Reject,"\\/#.,?*@!",
	End;

	GR_grp_0 =(Object*) GroupObject,
		MUIA_HelpNode, "GR_grp_0",
		MUIA_Group_Horiz, TRUE,
		Child, LA_nicknames,
		Child, MBObj->STR_nickname1,
		Child, MBObj->STR_nickname2,
		Child, MBObj->STR_nickname3,
	End;

	LA_realname =(Object*) Label("Real Name");

	MBObj->STR_realname =(Object*) StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_realname",
	End;

	GR_grp_1 =(Object*) GroupObject,
		MUIA_HelpNode, "GR_grp_1",
		MUIA_Group_Horiz, TRUE,
		Child, LA_realname,
		Child, MBObj->STR_realname,
	End;

	LA_username =(Object*) Label("User Name");

	MBObj->STR_username =(Object*) StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_username",
	End;

	GR_grp_2 =(Object*) GroupObject,
		MUIA_HelpNode, "GR_grp_2",
		MUIA_Group_Horiz, TRUE,
		Child, LA_username,
		Child, MBObj->STR_username,
	End;

	REC_label_0 =(Object*) RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	GR_grp_3 =(Object*) GroupObject,
		MUIA_HelpNode, "GR_grp_3",
		MUIA_Group_Horiz, TRUE,
		Child, REC_label_0,
	End;


    MBObj->NLT_Servers =(Object*) NListtreeObject,
        MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_On,
    End;

    if(!MBObj->NLT_Servers) cleanexit("Please download and install the NListtree and NList MUI Classes from aminet\n");


	GR_grp_8 =(Object*) GroupObject,
        Child, MBObj->NLT_Servers,
		MUIA_HelpNode, "GR_grp_8",
	End;


	REC_label_0CC =(Object*) RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	GR_grp_7 =(Object*) GroupObject,
		MUIA_HelpNode, "GR_grp_7",
		Child, REC_label_0CC,
	End;

	MBObj->BT_addserver =(Object*) SimpleButton("New Server");

	MBObj->BT_addgroup =(Object*) SimpleButton("New Group");

	MBObj->BT_delete_server =(Object*) SimpleButton("Delete Entry");

	BT_edit =(Object*) SimpleButton("Edit Entry");

	GR_grp_9 =(Object*) GroupObject,
		MUIA_HelpNode, "GR_grp_9",
		MUIA_Group_Horiz, TRUE,
		Child, MBObj->BT_addserver,
		Child, MBObj->BT_addgroup,
		Child, MBObj->BT_delete_server,
		Child, BT_edit,
	End;

	MBObj->BT_connect =(Object*) SimpleButton("Connect to Server");

	MBObj->BT_connect_tab =(Object*) SimpleButton("Connect in New TAB");

	GR_grp_6 =(Object*) GroupObject,
		MUIA_HelpNode, "GR_grp_6",
		MUIA_Group_Horiz, TRUE,
		Child, MBObj->BT_connect,
		Child, MBObj->BT_connect_tab,
        Child, MBObj->BT_perform,
	End;

	GROUP_ROOT_3 =(Object*) GroupObject,
		MUIA_Group_HorizSpacing, 2,
		MUIA_Group_VertSpacing, 2,
		Child, GR_grp_0,
		Child, GR_grp_1,
		Child, GR_grp_2,
		Child, GR_grp_3,
		Child, GR_grp_8,
		Child, GR_grp_7,
		Child, GR_grp_9,
		Child, GR_grp_6,
	End;


	MBObj->WI_label_2 =(Object*) WindowObject,
		MUIA_Window_Title, "Select a server",
		MUIA_Window_ID, MAKE_ID('1', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_3,
	End;





	GR_top =(Object*) GroupObject,
		MUIA_HelpNode, "GR_top",
		MUIA_Group_HorizSpacing, 0,
		MUIA_Group_VertSpacing, 0,
		//Child, GR_conductor,
	End;

	if(string_default)
    {
    	MBObj->STR_usertext =(Object*) StringObject,
		//MUIA_Textinput_Multiline, FALSE,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_usertext",
		MUIA_String_MaxLen, 3000,
    	End;
    }
    else
    {

        MBObj->STR_usertext =(Object*)BS_Object,
		MUIA_Frame, MUIV_Frame_String,
        MUIA_BetterString_StayActive, TRUE,
		MUIA_HelpNode, "STR_usertext",
		MUIA_String_MaxLen, 3000,
    	End;

    }

	GR_string_gadget =(Object*) GroupObject,
		MUIA_HelpNode, "GR_string_gadget",
		Child, MBObj->STR_usertext,
	End;


    GR_buttons =(Object*) VirtgroupObject,//GroupObject,
		MUIA_HelpNode, "GR_buttons",
        MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 2,
        //MUIA_Group_Rows,2,
	End;

	GR_virtual_buttons =(Object*) ScrollgroupObject,
		MUIA_Scrollgroup_Contents, GR_buttons,
        MUIA_HelpNode, "GR_buttons",
		//MUIA_Group_Horiz, TRUE,
        //MUIA_Scrollgroup_UseWinBorder, TRUE,
        MUIA_Scrollgroup_FreeVert, FALSE,
        MUIA_Scrollgroup_FreeHoriz, TRUE,
		MUIA_Group_HorizSpacing, 2,
	End;


                /*
                  LA_space =(Object*) TextObject,
		MUIA_Weight, 1,
		MUIA_Text_PreParse, "\033r",
		MUIA_Text_Contents, "",
		MUIA_InnerLeft, 0,
		MUIA_InnerRight, 0,
	End;
                */

	LA_space =(Object*) TextObject,
		MUIA_Weight, 1,
		MUIA_Text_PreParse, NULL, //"\033r",
		MUIA_Text_Contents, NULL,
		MUIA_InnerLeft, 0,
		MUIA_InnerRight, 0,
	End;


	GR_bottom_group =(Object*) GroupObject,
		MUIA_HelpNode, "GR_bottom_group",
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 2,
		Child, GR_buttons,
		//Child, GR_virtual_buttons,
        Child, LA_space,
	End;

	GROUP_ROOT_0 =(Object*) GroupObject,
		MUIA_Group_HorizSpacing, 0,
		MUIA_Group_VertSpacing, 2,
		//Child, PenMasterObject,
        Child, GR_top,
		Child, GR_string_gadget,
		Child, GR_bottom_group,
	End;

	MBObj->WI_main =(Object*)NewObject(mcc4->mcc_Class,NULL, // WindowObject,
	//MBObj->WI_main =(Object*)WindowObject,
		MUIA_Window_Title, "Wookiechat",
		MUIA_Window_Menustrip, MBObj->MN_,
		//MUIA_Window_Menustrip, strip  =(Object*) MUI_MakeObject(MUIO_MenustripNM,MenuData1,0),
		MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_0,
	End;



	MBObj->App =(Object*) ApplicationObject,
		MUIA_Application_Author, "James Carroll",
		MUIA_Application_Base, "WookieChat",
		MUIA_Application_Title, "WookieChat",
		MUIA_Application_Version, VERSION_MUI,
		MUIA_Application_Copyright, "James Carroll",
		MUIA_Application_Description, "IRC Client",
		SubWindow, MBObj->WI_main,
		SubWindow, MBObj->WI_label_2,
		SubWindow, MBObj->WI_edit_server,
		SubWindow, MBObj->WI_edit_group,
        SubWindow, MBObj->WI_colour_settings,
		SubWindow, MBObj->WI_mainsettings,
		SubWindow, MBObj->WI_dcc,  //incoming file transfers
		SubWindow, MBObj->WI_dcc2, //outgoing file transfers
        SubWindow, MBObj->WI_dcc_file_exists,
        SubWindow, MBObj->WI_dcc_send,
        SubWindow, MBObj->WI_about,
        SubWindow, MBObj->WI_quit,
	    SubWindow, MBObj->WI_urlgrabber,
    End;


	if (!MBObj->App)
	{
		FreeVec(MBObj);
        printf("unable to create app\n");
		return(NULL);
	}


	return(MBObj);
}




