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
#define FW 13
#define SECOND_SET_OF_RETURNIDS 400000

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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/filio.h>
#include <arpa/inet.h>
#include <proto/bsdsocket.h>
     // && select_struct.vnum==clipboard_struct.pos+1)
#include <proto/timer.h>
typedef char *p_in;
typedef UBYTE *a_in;
typedef char **c2_in;
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
#include <proto/socket.h>


//#include <clib/socket_protos.h>    
#include <netinet/in.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <sys/filio.h>
//#include <arpa/inet.h>
#include <netdb.h>

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

typedef char *p_in;
typedef UBYTE *a_in;
typedef char **c2_in;

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

typedef char *p_in;
typedef UBYTE *a_in;
typedef char **c2_in;

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


typedef char *p_in;
typedef UBYTE *a_in;
typedef UBYTE **c2_in;

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

#endif

#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/BetterString_mcc.h>

#ifdef __amigaos4__
#include <MUI/NBitmap_mcc.h>
#endif

#ifndef __AROS__
#include <SDI_compiler.h>
#include <SDI_hook.h>
#endif

#ifndef MUIA_Dtpic_Name
#define MUIA_Dtpic_Name 0x80423d72
#endif

//automatically reconnect options
#define MAX_CLONE_FIELDS 25

#define QUEUED_MESSAGES_DELAY_IN_SECONDS 0
#define QUEUED_MESSAGES_DELAY_IN_MICROSECONDS 750000

#define LEFT 0
#define RIGHT 1
 

#define STRING_BUFFERSIZE 800
#define SMALLSTRING_BUFFERSIZE 100
#define HOSTNAME_STRING_SIZE 85
#define USERLIST_NAME_SIZE 100
#define USERLIST_COMMAND_SIZE 800


#ifndef __AROS__

#ifndef MUIA_Application_IconifyTitle
#define MUIA_Application_IconifyTitle 0x80422cb8 /* V18 isg STRPTR            */
#endif

#ifndef MUIA_Imagedisplay_Spec
#define MUIA_Imagedisplay_Spec 0x8042a547
#endif
        /*
#ifndef MUIM_CustomBackfill
#define MUIM_CustomBackfill  0x80428d73 // muiundoc.h stuff to do undocumented MUI stuff
#endif

#ifndef MUIA_CustomBackfill
#define MUIA_CustomBackfill  0x80420a63 // muiundoc.h stuff to do undocumented MUI stuff
#endif
          */

#ifndef MUIA_Window_DisableKeys
#define MUIA_Window_DisableKeys 0x80424c36
#endif



#endif

#define STDIN 0
#define SIGNAL_TIMER 16

#define ACTIVITY_HIGHLIGHT 3

#ifdef __AROS__
Library * IconBase = NULL;
struct LocaleBase * LocaleBase = NULL;
struct GfxBase * GfxBase = NULL;
struct UtilityBase * UtilityBase = NULL;
Library * MUIMasterBase = NULL;
struct IntuitionBase * IntuitionBase = NULL;
Library * SocketBase = NULL;
Library * DataTypesBase = NULL;
Library * CodesetsBase = NULL;
#elif __amigaos4__
Library * AslBase = NULL;
Library * IconBase = NULL;
Library * LocaleBase = NULL;
Library * GfxBase = NULL;
Library * UtilityBase = NULL;
Library * MUIMasterBase = NULL;
Library * IntuitionBase = NULL;
Library * SocketBase = NULL;
Library * DataTypesBase = NULL;
Library * CodesetsBase = NULL;
#elif __MORPHOS__
Library * AslBase = NULL;
Library * IconBase = NULL;
Library * LocaleBase = NULL;
struct GfxBase * GfxBase = NULL;
Library * UtilityBase = NULL;
Library * MUIMasterBase = NULL;
struct IntuitionBase * IntuitionBase = NULL;
Library * SocketBase = NULL;
Library * DataTypesBase = NULL;
Library * CodesetsBase = NULL;
#else
Library * AslBase = NULL;
Library * IconBase = NULL;
Library * LocaleBase = NULL;
Library * GfxBase = NULL;
Library * UtilityBase = NULL;
Library * MUIMasterBase = NULL;
Library * IntuitionBase = NULL;
Library * SocketBase = NULL;
Library * DataTypesBase = NULL;
Library * CodesetsBase = NULL;
#endif


// prototypes
int search_for_highlight_words(char *, char *);
void give_each_tab_a_listview_number_for_switching_tabs(void);
void give_each_tab_a_page_group_number(void);
int update_your_nickname_text_label(void);
int update_TX_nicklist(void);
int do_waitselect_code(void);
void sort_linked_list(void);
BOOL FindUTF8Chars(char*);
void DisplayNicks(void);
void activate_tab_button(void);
void remove_tab_listview(void);
void add_tabs_to_nicklist_group(void);
void acquire_connect_details(char*);
void save_colours_choice(void);
void load_colours_choice(void);
void set_channel_clipboard_hook(void);
void check_column_size(void);
void shutdown_my_dcc_send_socket(void);
void remove_tab_listview2(void);
void remove_tab_listview(void);
void add_tabs_to_nicklist_group(void);
void setup_notifys(void);
void kill_my_notifys(void);
char *doubleclick_url_action(char*, int, int);
void change_window_titlebar_text(void);
void initiate_outgoing_dcc_chat(char*);
void create_recv_dcc_chat(char *, char *, char *);
int offer_dcc(char*,char*,char*,char*,int);
int create_new_query_window(char*,int);
void cleanexit(char*);
void send_current(char*);
void send_dcc_chat(char*);
int compare_func(char*,char*);
int sort_nicks(int);
void LoadAllLibs(void);
void SWhoIs(char *selnick);
void SQuery(char *selnick);
void ChangeMyNick(char *mynewnick);
void SJoin(char *selchannel);
void SMsg(char *selnick, char *message);
void SNames(char *selchannel);
void SNotice(char *selnick, char *massage);
void ChangeTopic(char *newtopic);
void SPart(char *message);
void SQuit(char *message);
void SAction(char *message);
void CloseQueryWindow(void);
void SQuote(char *message);
void SPing(char *message);
void ping_time(void);
void save_settings(void);
void save_nick_settings(void);
void load_nick_settings(void);
void retrieve_settings(void);
void close_server_select_window(void);
void switch_between_tabs(int);
int switch_between_tabs_with_keys(int);
void load_colours(char*);
int display_last_few_lines_of_logfile_conductor(void);
int load_graphical_smilies(void);
int free_graphical_smilies(struct query_window*);
int use_graphical_smilies(struct query_window*);
int find_themes(void);
char * convert_graphical_smilies_2_text(char*);


struct TagItem blank_taglist[] = { {TAG_DONE, 0} };

struct MsgPort *arexx_quit_replyport;
struct MsgPort *app_process_port;
struct MsgPort *app_process_replyport;
struct MsgPort *send_text_replyport;
//struct Message *my_message;


struct SignalSemaphore *WookieChat_semaphore;
struct SharedList {
    struct SignalSemaphore sl_Semaphore;
    struct List sl_List;
} *slist;


//APTR BT_blank;
APTR pen;
BOOL RECENTLY_CREATED_A_TAB=FALSE;
BOOL is_chooser_window_open;
BOOL user_ignore_privmsg, user_ignore_ctcp, user_ignore_dcc;
BOOL aslresult;
BOOL using_a_proxy;
BPTR urlgrabber_file;
BPTR arexx_dir_lock;
BOOL dont_open_colours;
struct codeset *charsets[45];
struct codeset *local_charsets_list[45];
struct codeset *cs;
char *background3;//=new char[64];
char *banmask_tokens[6];
char *channel_display;//=new char[100];
char *NewPreParse_NewText;//=new char[100];
char *nickcolour;//=new char[40];
char *pch3;
char *pch;
char *PreParse_NewText=(char*)"\033c\0333";
char *string2,*string3,*string4,*string5;
char *tabwork2_string;//=new char[900]; //text before cursor position
char *tabwork3_string;//=new char[900]; //text after cursor position
char *tabwork4_string;//=new char[100]; //nick text to complete
char *tabwork5_string;//=new char[100]; //completed nick text to insert
char *tabwork_string;//=new char[900]; //original string gadget contents
char *text, *text2;
STRPTR text3;
char activity[64];
char activity_chat[64];
char activity_highlight[64];
char background4[64];
char background[64];
char banmask[200];
char ban_window_title[200];
char pen_number[10];
char new_array2[900];
char new_array3[20];
char listview_format[100];
char list_channels_work[900];
char list_found_nicks[5000][50]; // Nick completion function variables
char output_string[800];
char old_ignore_entry[800];
char old_alias_entry[800];
char rawservername[100];
char server[50];
char *string123;
char string8[900],string9[900],string11[900];
char string_to_send[BUFFERSIZE];
char urlgrabber_str[2000];
char urlvisit_str[1][2000];
char windowtitlestring[110];
char wookie_dir[400]; //the pathname wookiechat is located in
char wscreent[200];
                                        char orig_filename[1000];
                                        char filename[1000];
                                        char new_filename[1000];

int b;
int a_socket=0;
int delay_b4_ping_server_count=0;
int iv=0;
int last_a=0;
int last_c=0;
int nickcomp_count=0;
int nickcomp_state=0;
int nick_length;
int previous_b=0;
int socket_global=0;
int sort_method=1; //which sorting method is used for nick lists - only 1 presently
ULONG arexx_wants_to_send_signal;
ULONG iconified_and_new_text;
long ev_sock;
Object *o, *o3;
struct ClockData *clockdata;
struct ClockData *clockdata2;
struct DiskObject *dobj; //for the iconified icon
struct FileRequester *filerequester;
struct MUI_NList_TestPos_Result *last_clicked_res;

struct MUI_PenSpec *startpen;
struct sockaddr addr_buffer, addr_buffer2;
struct sockaddr_in slapshot_in;
ULONG ev_type;
ULONG portsig, portsig2, portsig3, portsig4;
ULONG socksigbit,dccsigbit,timer_signal,reconnect_delay_timer_signal;
STRPTR work_incoming2;

ULONG test, test2;
enum { NEVER=0, ALWAYS, WHEN_WIN_ACTIVE };
enum { AMIRC_STYLE=0, MIRC_STYLE };
//enum { BLACK_BACKGROUND=0, WHITE_BACKGROUND };

char *ignore1,*ignore2,*ignore3,*ignore4,*ignore5,*ignore6,*ignore7,*ignore8,*ignore9,*ignore10;

char clone_results[MAX_CLONE_FIELDS][900];
int count_clones;






// below are some variables for working with NextObject() loops



//the menu is left blank so we can use localised strings from a catalog.
//the code for inserting the localised text is in LoadAllLibs() in
//startexit.cpp
struct NewMenu MenuData1[] =
{
    { NM_TITLE, (char*)""   ,0,0,0  ,(APTR)0      },
    { NM_ITEM,  (char*)""   ,0,0,0  ,(APTR)65     },
    { NM_ITEM,  (char*)""   ,0,0,0  ,(APTR)66     },
    { NM_ITEM,  (char*)""   ,0,0,0  ,(APTR)80     },
    { NM_ITEM,  (char*)""   ,0,0,0  ,(APTR)0      },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)75     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)76     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)77     },
    { NM_ITEM,  (char*)""   ,0,0,0  ,(APTR)0      },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)79     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)81     },
    { NM_ITEM,  (char*)""   ,0,0,0  ,(APTR)0      },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)67     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)68     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)69     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)70     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)71     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)72     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)73     },
    { NM_SUB,   (char*)""   ,0,0,0  ,(APTR)83     },

    { NM_END,NULL,0,0,0,(APTR)0 },
};

struct MUI_CustomClass *NLI_Class;

#include "version_info.h"



APTR LV_events;
APTR STR_events_script;
APTR STR_events_args;
APTR CYCLE_events;

char num_to_string[100];

    struct test_tree
    {
        struct MUI_NListtree_TreeNode *t;
        struct test_tree *next;

    } *tree_conductor, *tree_root;

int total_servers;
char last_group[50];


void read_list_of_servers(void)
{
    count=0;

    char *work_buffer=new char[1000];
    char *work_buffer2=new char[1000];
    char *work1=new char[100];
    char *len2; //variable used for file access
    char work_buffer3[600];
    newbptr_file = Open("progdir:servers.txt",MODE_OLDFILE);

    int running3=1;
    len2=(char*)FGets(newbptr_file,(b_in)work_buffer,600);
    if(len2)
    {
        if(stricmp(work_buffer,"WOOKIECHAT_SERVERS_FILE_2\n"))
        {
            printf("Old servers.txt file detected!\nRenaming servers.txt to servers2.txt\nNow generating a new servers.txt in the new format\n(Keep servers2.txt as a backup in case something goes wrong!\n");

            Rename("progdir:servers.txt","progdir:servers2.txt");

            BPTR new_servers_file = Open("progdir:servers.txt",MODE_NEWFILE);
            if(new_servers_file)
            {
                FPuts(new_servers_file,(b_in)"WOOKIECHAT_SERVERS_FILE_2\n");

                FPuts(new_servers_file,(b_in)work_buffer);

                while(running3)
                {
                    //read the line from the servers file
                    len2=(char*)FGets(newbptr_file,(b_in)work_buffer,600);

                    if(!len2) { running3=0; }

                    if(DEBUG) printf("running3 = %d\n",running3);


                    //work_buffer[strlen(work_buffer)]='\0';

                    strcpy(work_buffer2,work_buffer);
                    strtok(work_buffer2," ");
                    work1=strtok(NULL," \n");

                    //its a group name! lets create the initial group entry
                    if(!work1)
                    {
                        //if(!first) FPuts(new_servers_file,(b_in)"ENDGROUP\n");
                        //first=FALSE;

                        //FPuts(new_servers_file,(b_in)"GROUP");
                        FPuts(new_servers_file,(b_in)work_buffer);
                        //FPuts(new_servers_file,(b_in)"\n");
                        //printf("putting group name:%s",work_buffer);

                    }
                    else
                    {
                        strcpy(work_buffer2,work_buffer);
                        //printf("1 work_buffer :%s",work_buffer);
                        //printf("1 work_buffer2:%s",work_buffer2);

                        string3=strtok(work_buffer2," "); //servername
                        string2=strtok(NULL,"\n ");     //port number
                        string1=strtok(NULL,"\n ");    //connect automatically, yes or not?
                        string5=strtok(NULL,"\n ");     //charset
                        string4=strtok(NULL,"\n ");   //password

                        if(string3)
                        {
                            sprintf(work_buffer3,"SERVER=%s ",string3);
                            FPuts(new_servers_file,(b_in)work_buffer3);

                            if(string2)
                            {
                                sprintf(work_buffer3,"PORT=%s",string2);
                                FPuts(new_servers_file,(b_in)work_buffer3);

                            }
                        }

                        if(string1)
                            sprintf(work_buffer3," AUTOCONNECT=%s",string1);
                        else
                            sprintf(work_buffer3," AUTOCONNECT=");

                        FPuts(new_servers_file,(b_in)work_buffer3);

                        if(string5)
                            sprintf(work_buffer3," SERVER_CHARSET=%s",string5);
                        else
                            sprintf(work_buffer3," SERVER_CHARSET=");

                        FPuts(new_servers_file,(b_in)work_buffer3);

                        if(string4)
                            sprintf(work_buffer3," SERVER_PASSWORD=%s",string4);
                        else
                            sprintf(work_buffer3," SERVER_PASSWORD=");
                        FPuts(new_servers_file,(b_in)work_buffer3);


                        sprintf(work_buffer3," AUTOJOINS=");
                        FPuts(new_servers_file,(b_in)work_buffer3);

                        FPuts(new_servers_file,(b_in)"\n");

                    }


                }

                Close(new_servers_file);


            }



        }

    }

    Close(newbptr_file);


    return;

/*
    newbptr_file = Open("progdir:servers.txt",MODE_OLDFILE);

    while(running3)
    {
        if(!newbptr_file) break;

        //read the line from the servers file
        len2=(char*)FGets(newbptr_file,(b_in)work_buffer,600);

        if(!len2) { running3=0; }

        work_buffer[strlen(work_buffer)]='\0';

        strcpy(work_buffer2,work_buffer);
        work1=strtok(work_buffer2," =\n");

        if(!work1) { running3=0; break; }


        //if(!stricmp(work1,"GROUP"))

        if(!work1)
        {
            strcpy(work_buffer2,work_buffer);
            work1=strtok(work_buffer2," \n");

            //this is the name of the group
            if(work1)
            {
                tree_conductor->next=new test_tree;
                tree_conductor=tree_conductor->next;
                tree_conductor->next=NULL;

                tree_conductor->t = (struct MUI_NListtree_TreeNode *)DoMethod((Object*)WookieChat->NLT_Servers, MUIM_NListtree_Insert, work1, NULL,
                    tree_root->t, MUIV_NListtree_Insert_PrevNode_Tail, TNF_LIST);

                strcpy(last_group,work1);
                //strcpy(server_list[count].group_name,last_group);

                //count++;
                total_servers=count;

            }


        }
        else if(!stricmp(work1,"AUTOCONNECT"))
        {

            work1=strtok(NULL," \n");

            if(work1)
            {
                server_list[count].auto_connect=atoi(work1);
            }
            else
                server_list[count].auto_connect=0;


        }
        else if(!stricmp(work1,"SERVER"))
        {

            strcpy(server_list[count].group_name,last_group);

            work1=strtok(NULL," \n");

            if(work1)
            {
                strcpy(server_list[count].server_name,work1);
            }
            else
                strcpy(server_list[count].server_name,"error");

        }
        else if(!stricmp(work1,"PORT"))
        {
            work1=strtok(NULL," \n");

            if(work1)
            {
                strcpy(server_list[count].port_number,work1);
            }
            else
                strcpy(server_list[count].port_number,"6667");



        }
        else if(!stricmp(work1,"SERVER_PASSWORD"))
        {

            work1=strtok(NULL," \n");

            if(work1)
            {
                strcpy(server_list[count].server_password,work1);
            }
            else
                strcpy(server_list[count].server_password,"");

        }
        else if(!stricmp(work1,"NICK_PASSWORD"))
        {

            work1=strtok(NULL," \n");

            if(work1)
            {
                strcpy(server_list[count].nick_password,work1);
            }
            else
                strcpy(server_list[count].nick_password,"");

        }
        else if(!stricmp(work1,"SERVER_CHARSET"))
        {

            work1=strtok(NULL," \n");

            if(work1)
            {
                strcpy(server_list[count].server_charset,work1);
            }
            else
                strcpy(server_list[count].server_charset,"");

        }
        else if(!stricmp(work1,"AUTOJOINS"))
        {

            work1=strtok(NULL," \n");

            if(work1)
            {
                strcpy(server_list[count].auto_joins,work1);
            }
            else
                strcpy(server_list[count].auto_joins,"");

        }
        else if(!stricmp(work1,"ENDSERVER"))
        {


            strcpy(work_buffer,"");
            if(server_list[count].server_name) { strcat(work_buffer,server_list[count].server_name); strcat(work_buffer," "); }
            if(server_list[count].port_number) { strcat(work_buffer,server_list[count].port_number); strcat(work_buffer," "); }
            if(server_list[count].auto_connect) { sprintf(num_to_string,"%i",server_list[count].auto_connect); strcat(work_buffer,num_to_string); strcat(work_buffer," ");  }
            if(server_list[count].server_charset) { strcat(work_buffer,server_list[count].server_charset); strcat(work_buffer,"\n"); }

            DoMethod((Object*)WookieChat->NLT_Servers, MUIM_NListtree_Insert, work_buffer, NULL,
                tree_conductor->t, MUIV_NListtree_Insert_PrevNode_Tail, NULL );

            //count++;
            total_servers=count;

        }



    }

    if(newbptr_file) Close(newbptr_file);

    DoMethod((Object*)WookieChat->NLT_Servers, MUIM_NListtree_Open,MUIV_NListtree_Open_ListNode_Root,MUIV_NListtree_Open_ListNode_Root);
  */

}

struct list_entry *new_entry; //=new list_entry;
struct list_entry *work_entry3; // = new list_entry;
struct list_entry work_entry4;


struct history
{
    char buffer_history[800];
    struct history *next;
    struct history *previous;
} *work_history, *work_history2;





char number_of_lines_unread[20];

struct MUI_NList_GetSelectInfo *res=new struct MUI_NList_GetSelectInfo;

struct channel_entry *wentry=new channel_entry;

#include "subclasses.cpp"

MUI_CustomClass *mcc, *mcc2, *mcc3, *mcc4, *mcc5, *mcc6;

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
char string22[800];
int last_count, last_count2;
struct MUI_NList_GetEntryInfo clipboard_struct;
struct MUI_NList_GetEntryInfo clipboard_struct2;

int  col1,col2,pos1,pos2;
int was_last_one_four;

struct MUI_NList_GetSelectInfo select_struct;
int lastvstart;
int lastvend;

// timer device variables
struct timerequest *TimerIO;
struct MsgPort *TimerMP;
struct Message *TimerMSG;
//struct MsgPort *xyreplyport;

// second timer device variables for the 2 second delays
struct timerequest *Timer2IO;
struct MsgPort *Timer2MP;
struct Message *Timer2MSG;




//struct timerequest *Timer3IO;
//struct MsgPort *Timer3MP;
//struct MsgPort *xyreplyport2;

//check if its midnight yet, if it is then inform the user in all open tabs
struct timerequest *Timer4IO;
struct MsgPort *Timer4MP;

// signal us when its time to send another line of our paste
struct timerequest *Timer5IO;
struct MsgPort *Timer5MP;

LONG error;
ULONG mics;
char timestamp_hrs[4]; char timestamp_mins[4]; char timestamp_secs[4];

//ctcp ping variables
char pingtimestamp_hrs[4]; char pingtimestamp_mins[4]; char pingtimestamp_secs[4]; char pingtimestamp[12];

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



int last_clipboardline=999;

int last_line=999999;
int last_pos1=999,last_pos2=999;

enum { COLUMNS=1, NORMAL };

int which_clipboard_style()
{

    if(my_settings.which_clipboard_style==COLUMNS)
    {
        if(DEBUG) printf("columns code used\n");
        return 0;

    }
    else
    {
        if(DEBUG) printf("normal code used\n");
        return 1;

    }
    return 1;

}

int first_pos1=0;

#include "hooks.cpp"



char *CYA_GroupTitleColor[20];


struct query_window *init_conductor(int a)
{

    struct query_window *test;

    test = (struct query_window *) AllocVec(sizeof(struct query_window), MEMF_PUBLIC|MEMF_CLEAR);

    test->entries_count=0;

    for(count=0; count<2500; count++)
        test->nicklist[count].hostname=NULL;

    test->next=NULL;
    test->removed=0;
    test->nicks=0;
    work_query=NULL;
    //test->queued_messages_current=0;
    test->queued_messages_total=0;

    for(count=0; count<MAX_QUEUED_MESSAGES; count++) test->queued_messages[count]=NULL; //strcpy(test->queued_messages[count],"");



    strcpy(test->limit,"");
    strcpy(test->keyword,"");
    strcpy(test->away_message,"");

    iv++;
    //printf("init_conductor iv:%d\n",iv);

    test->BT_mode_X = (Object*)TextObject,
        ButtonFrame,
        MUIA_Weight, 0,
        MUIA_FixWidth, FW,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, (char*)"X",
        MUIA_Text_PreParse, (char*)"\033c",
        MUIA_ShortHelp, (char*)GetCatalogStr(catalog,4,(char*)"Close the current tab"),
        MUIA_InputMode, MUIV_InputMode_RelVerify,
    End;

    test->BT_mode_T = (Object*)TextObject,
        ButtonFrame,
        MUIA_FixWidth, FW,
        MUIA_Weight, 0,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, (char*)"T",
        MUIA_Text_PreParse, (char*)"\033c",
        MUIA_ShortHelp, GetCatalogStr(catalog,6,"Topic protection. This setting prevents regular users from changing the channel topic"),
        MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

    test->BT_mode_N = (Object*)TextObject,
        ButtonFrame,
        MUIA_FixWidth, FW,
        MUIA_Weight, 0,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "N",
        MUIA_Text_PreParse, "\033c",
        MUIA_ShortHelp, GetCatalogStr(catalog,7,"No messaging. This setting forces users to be in the channel to be able to talk in it"),
        MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

    test->BT_mode_S = (Object*)TextObject,
        ButtonFrame,
        MUIA_FixWidth, FW,
        MUIA_Weight, 0,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "S",
        MUIA_Text_PreParse, "\033c",
        MUIA_ShortHelp, GetCatalogStr(catalog,8,"Secret mode. If this is set, the channel is invisible until you have joined"),
        MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

    test->BT_mode_I = (Object*)TextObject,
        ButtonFrame,
        MUIA_FixWidth, FW,
        MUIA_Weight, 0,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "I",
        MUIA_Text_PreParse, "\033c",
        MUIA_ShortHelp, GetCatalogStr(catalog,9,"Invite only. Only users who have been invited to the channel may join"),
        MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

    test->BT_mode_P = (Object*)TextObject,
        ButtonFrame,
        MUIA_FixWidth, FW,
        MUIA_Weight, 0,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "P",
        MUIA_Text_PreParse, "\033c",
        MUIA_ShortHelp, GetCatalogStr(catalog,10,"Private mode. If set, channel topic is not shown on global channel lists"),
        MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

    test->BT_mode_M = (Object*)TextObject,
        ButtonFrame,
        MUIA_FixWidth, FW,
        MUIA_Weight, 0,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "M",
        MUIA_Text_PreParse, "\033c",
        MUIA_ShortHelp, GetCatalogStr(catalog,11,"Moderated mode. Only users with operator, halfop or voice status (thats @ % or +) may talk in channel when set"),
        MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

    test->BT_mode_B = (Object*)TextObject,
        ButtonFrame,
        MUIA_FixWidth, FW,
        MUIA_Weight, 0,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "B",
        MUIA_Text_PreParse, "\033c",
        MUIA_ShortHelp, GetCatalogStr(catalog,12,"Click to view a list of all banmasks for this channel"),
        MUIA_InputMode, MUIV_InputMode_RelVerify,
    End;

    test->BT_mode_K = (Object*)TextObject,
        ButtonFrame,
        MUIA_FixWidth, FW,
        MUIA_Weight, 0,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "K",
        MUIA_Text_PreParse, "\033c",
        MUIA_ShortHelp, GetCatalogStr(catalog,13,"Keyword mode. When this is set, users need to supply a keyword (password) to enter"),
        MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

        test->STR_keyword = (Object*)BetterStringObject,
        MUIA_Weight, 10,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GetCatalogStr(catalog,14,"Current channel keyword"),
        MUIA_String_Secret, TRUE,
        End;

    test->BT_mode_L = (Object*)TextObject,
        ButtonFrame,
        MUIA_FixWidth, FW,
        MUIA_Weight, 0,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "L",
        MUIA_Text_PreParse, "\033c",
        MUIA_ShortHelp, GetCatalogStr(catalog,15,"Enforce a limit to how many users can currently join the channel"),
        MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

        test->STR_limit = (Object*)BetterStringObject,
        MUIA_Weight, 10,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GetCatalogStr(catalog,16,"Current user limit"),
         MUIA_String_Accept, "0123456789",
        End;

    //setup_background_colours();

    if(my_settings.use_column_display==0) //make a one columned display
    {
        #ifdef __AROS__
        test->LV_channel = (Object*)NewObjectAROS(mcc2->mcc_Class, NULL,
        #else
        test->LV_channel = (Object*)NewObject(mcc2->mcc_Class, NULL,
        #endif
            MUIA_Frame, MUIV_Frame_InputList,
            //undo
            MUIA_NList_ConstructHook, &ConstructLI_channel_TextHook,
            MUIA_NList_DestructHook, &DestructLI_channel_TextHook,
            //MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
            //MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
            MUIA_NList_DisplayHook, &DisplayLI_channel_TextHook,
            MUIA_NList_DisplayRecall, TRUE,
            MUIA_NList_CopyEntryToClipHook, &Custom_Clipboard2_Hook,
            MUIA_NList_AutoCopyToClip, TRUE,
            MUIA_NList_Input, FALSE,
            MUIA_NList_ListBackground, popimage_background,
            MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
        End;
    }
    else //make a multi columned display for time/nick/text
    {
            #ifdef __AROS__
            test->LV_channel = (Object*)NewObjectAROS(mcc2->mcc_Class, NULL,
            #else
            test->LV_channel = (Object*)NewObject(mcc2->mcc_Class,NULL,// NListObject,
            #endif
            MUIA_Frame, MUIV_Frame_InputList,
            //undo
            MUIA_NList_ConstructHook, &ConstructLI_channel_TextHook,
            MUIA_NList_DestructHook, &DestructLI_channel_TextHook,
            //MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
            //MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
            MUIA_NList_DisplayHook, &DisplayLI2_channel_TextHook,
            MUIA_NList_DisplayRecall, TRUE,
            MUIA_NList_CopyEntryToClipHook, &Custom_Clipboard2_Hook,
            MUIA_NList_Format,listview_format,
            MUIA_NList_AutoCopyToClip, TRUE,
            MUIA_NList_Input, FALSE,
            MUIA_NList_ListBackground, popimage_background,
            MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
        End;

    }

    if(!test->LV_channel) cleanexit((char*)GetCatalogStr(catalog,18,"Please download and install the NList MUI Class from aminet\n"));

    test->LV_channel = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 100,
        MUIA_NListview_NList, test->LV_channel,
    End;



    if(a==1)
    {

        test->STR_topic = (Object*)BetterStringObject,
            MUIA_Weight, 80,
            //MUIA_Textinput_Multiline, FALSE,
            MUIA_Frame, MUIV_Frame_String,
            MUIA_ShortHelp, GetCatalogStr(catalog,5,"This is the channels current topic"),
            MUIA_String_MaxLen, 500,
        End;

        test->GR_channel_modes = (Object*)GroupObject,
            MUIA_HelpNode, "GR_channel_modes",
            MUIA_Group_Horiz, TRUE,
            MUIA_Group_HorizSpacing, 1,
            MUIA_Weight,100,
            Child, test->BT_mode_X,
            Child, test->STR_topic,
            Child, test->GR_mode_gadgets_sub_group=(Object*)GroupObject,
                MUIA_Group_Horiz, TRUE,
                MUIA_Group_HorizSpacing, 1,
                MUIA_Weight,40,
                MUIA_ShowMe,TRUE,
                Child, test->BT_mode_T,
                Child, test->BT_mode_N,
                Child, test->BT_mode_S,
                Child, test->BT_mode_I,
                Child, test->BT_mode_P,
                Child, test->BT_mode_M,
                Child, test->BT_mode_B,
                Child, test->BT_mode_K,
                Child, test->STR_keyword,
                Child, test->BT_mode_L,
                Child, test->STR_limit,
            End,
        End;

        #ifdef __AROS__
        test->LV_nicklist = (Object*)NewObjectAROS(mcc2->mcc_Class,NULL,  //NListObject,
        #else
        test->LV_nicklist = (Object*)NewObject(mcc2->mcc_Class,NULL,  //NListObject,
            #endif
            MUIA_ContextMenu, test->strip = (Object*)MUI_MakeObject(MUIO_MenustripNM,MenuData1,0),
            MUIA_ShortHelp,GetCatalogStr(catalog,319,"Graphical Usermodes Key:\nGreen = Channel Operators or @\nBlue = HalfOps or \%\nYellow = Voiced Users or +"),
            MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Default,
            MUIA_NList_ConstructHook, &ConstructLI_TextHook,
            MUIA_NList_DisplayHook, &DisplayLI_TextHook,
            MUIA_NList_DestructHook, &DestructLI_TextHook,
            MUIA_NList_ListBackground, popimage_nicklistbackground,
            MUIA_NList_ListPen, pendisplay_specs[22],
            MUIA_NList_Format,"PIXELWIDTH=15, WEIGHT=90",
            MUIA_Frame, MUIV_Frame_InputList,
        End;

        test->LV_nicklist = (Object*)NListviewObject,
            //MUIA_Weight, 20,
            MUIA_HelpNode, "LV_nicklist",
            MUIA_ObjectID, MAKE_ID('L', 'V', '0', '1'),
            MUIA_Width, my_settings.nicklist_horizontal_weight,
            MUIA_MaxWidth, my_settings.nicklist_horizontal_weight,
            MUIA_NListview_Horiz_ScrollBar,MUIV_NListview_HSB_None,
            MUIA_NListview_NList, test->LV_nicklist,
        End;

        DoMethod((Object*)test->LV_nicklist,MUIM_Notify,MUIA_NList_DoubleClick,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,9);

        DoMethod((Object*)test->BT_mode_B,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,18);


        DoMethod((Object*)test->STR_topic,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,29);

    }
    else
    {

        test->STR_topic = (Object*)BetterStringObject,
            MUIA_Weight, 80,
            //MUIA_Textinput_Multiline, FALSE,
            MUIA_String_MaxLen, 500,
            MUIA_Frame, MUIV_Frame_String,
            MUIA_ShortHelp, GetCatalogStr(catalog,17,"This is the hostname of the user you are currently talking to"),
            //MUIA_ShortHelp, "This is the hostname of the user you are currently talking to",
        End;

        /*if(ZUNE_SYSTEM==TRUE)
        {
            test->GR_channel_modes = (Object*)GroupObject,
                MUIA_HelpNode, "GR_channel_modes",
                MUIA_Group_Horiz, TRUE,
                MUIA_Group_HorizSpacing, 1,
                MUIA_Group_HorizSpacing, 0,
                MUIA_Weight,100,
                Child, test->BT_mode_X,
                Child, test->STR_topic,
                Child, test->BT_mode_T,
                Child, test->BT_mode_N,
                Child, test->BT_mode_S,
                Child, test->BT_mode_I,
                Child, test->BT_mode_P,
                Child, test->BT_mode_M,
                Child, test->BT_mode_B,
                Child, test->BT_mode_K,
                Child, test->STR_keyword,
                Child, test->BT_mode_L,
                Child, test->STR_limit,
            End;

        }
        else*/
        {
            test->GR_channel_modes = (Object*)GroupObject,
                MUIA_ShortHelp, "GR_channel_modes",
                MUIA_Group_Horiz, TRUE,
                MUIA_Group_HorizSpacing, 1,
                //MUIA_Group_HorizSpacing, 2,
                MUIA_Weight,100,
                Child, test->BT_mode_X,
                Child, test->STR_topic,
            End;
        }

        #ifdef __AROS__
        test->LV_nicklist = (Object*)NewObjectAROS(mcc2->mcc_Class,NULL,  //NListObject,
        #else
        test->LV_nicklist = (Object*)NewObject(mcc2->mcc_Class,NULL,  //NListObject,
            #endif
            MUIA_ContextMenu, test->strip = (Object*)MUI_MakeObject(MUIO_MenustripNM,MenuData1,0),
            //MUIA_ShortHelp,"Graphical Usermodes Key:\nGreen = Channel Operators or @\nBlue = HalfOps or \%\nYellow = Voiced Users or +",
            MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Default,
            MUIA_NList_ConstructHook, &ConstructLI_TextHook,
            MUIA_NList_DisplayHook, &DisplayLI_TextHook,
            MUIA_NList_DestructHook, &DestructLI_TextHook,
            MUIA_NList_ListBackground, popimage_nicklistbackground,
            MUIA_NList_ListPen, pendisplay_specs[22],
            MUIA_NList_Format,"COLWIDTH=1.5, WEIGHT=90",
            //MUIA_NList_Format,"PIXELWIDTH=15, WEIGHT=90",
            MUIA_Frame, MUIV_Frame_InputList,
        End;

        test->LV_nicklist = (Object*)NListviewObject,
            MUIA_HelpNode, "LV_nicklist",
            //MUIA_Weight, 20,
            MUIA_Width, my_settings.nicklist_horizontal_weight,
            MUIA_MaxWidth, my_settings.nicklist_horizontal_weight,
            MUIA_ObjectID, MAKE_ID('L', 'V', '0', '2'),
            MUIA_NListview_Horiz_ScrollBar,MUIV_NListview_HSB_None,
            MUIA_NListview_NList, test->LV_nicklist,
        End;

        DoMethod((Object*)test->LV_nicklist,MUIM_Notify,MUIA_NList_DoubleClick,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,9);



    }

    test->GR_tabs = (Object*)GroupObject,
        MUIA_HelpNode, "GR_listviews",
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_HorizSpacing, 1,
        MUIA_Group_VertSpacing, 1,
        Child, WookieChat->GR_click_user_list_buttons,
        Child, WookieChat->LV_tabs,
    End;

    test->GR_nicklist_and_tabs = (Object*)GroupObject,
        MUIA_HelpNode, "GR_listviews",
        //MUIA_HorizWeight,20,
        //MUIA_Weight, 20,
        MUIA_Width, my_settings.nicklist_horizontal_weight,
        MUIA_MaxWidth, my_settings.nicklist_horizontal_weight,
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_HorizSpacing, 1,
        MUIA_Group_VertSpacing, 1,
        Child, test->LV_nicklist,
    End;

    test->GR_listviews = (Object*)GroupObject,
        MUIA_HelpNode, "GR_listviews",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_HorizSpacing, 1,
        MUIA_Group_VertSpacing, 1,
        Child, test->LV_channel,
            Child, test->GR_listviews_sub= (Object*)GroupObject,
                MUIA_HelpNode, "GR_listviews",
                MUIA_Group_Horiz, FALSE,
                MUIA_Group_HorizSpacing, 1,
                MUIA_Group_VertSpacing, 1,
                Child, test->TX_nicklist=TextObject,MUIA_Text_Contents,"                 ",MUIA_ShowMe,TRUE,MUIA_Background,MUII_TextBack,MUIA_Frame,MUIV_Frame_Text, End,
                Child, test->GR_nicklist_and_tabs,
            End,
    End;

    test->GR_conductor = (Object*)GroupObject,
        MUIA_HelpNode, "GR_conductor",
        MUIA_Group_HorizSpacing, 1,
        //MUIA_Group_VertSpacing, 1,
        MUIA_Group_VertSpacing, 1,
        
        Child, test->GR_channel_modes,
        Child, test->GR_listviews,
    End;

    if(SMALLTABS)
    {

        test->BT_querybutton = (Object*)TextObject,
            ButtonFrame,
            MUIA_Weight, 0,
            MUIA_Background, MUII_ButtonBack,
            MUIA_Text_Contents, "new button",
            MUIA_Text_PreParse, "\033c\0332",
            MUIA_HelpNode, "BT_query2",
            MUIA_InputMode, MUIV_InputMode_Immediate,
            MUIA_Text_SetMin,FALSE,
        End;

    }
    else
    {
        test->BT_querybutton = (Object*)TextObject,
            ButtonFrame,
            MUIA_MaxWidth, 150,  //testing smaller buttons
            MUIA_Background, MUII_ButtonBack,
            MUIA_Text_Contents, "new button",
            MUIA_Text_PreParse, "\033c\0332",
            MUIA_HelpNode, "BT_query2",
            MUIA_InputMode, MUIV_InputMode_Immediate,
            MUIA_Text_SetMin,FALSE,
        End;
    }

    setmacro((Object*)test->LV_channel,MUIA_NListview_Horiz_ScrollBar,MUIV_NListview_HSB_None);


    DoMethod((Object*)test->BT_mode_X,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,73);

    test->removed=0;

    test->string_root=new history;
    test->string_conductor=new history;
    strcpy(test->string_conductor->buffer_history,"");
    test->string_conductor->next=NULL;
    test->string_conductor->previous=NULL;
    test->string_root=test->string_conductor;

    DoMethod((Object*)test->LV_channel,MUIM_Notify, MUIA_NList_DoubleClick, TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID, 96);

    strcpy(test->your_current_mode,"");

    
    use_graphical_smilies(test);

    return test;

}


long dcc_socket;
long dcc_listen_socket;
struct sockaddr_in their_addr;

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


        

APTR    GR_ban, GR_ban_subgroup, GR_logging_splitup, GR_logging_splitup2;

APTR    GROUP_ROOT_0, GROUP_ROOT_1, GR_top;
APTR    GR_bottom_group, GR_buttons, GR_virtual_buttons, GR_samples_path;
APTR    LA_space;

    APTR    GROUP_ROOT_8;
    APTR    GR_graphical_smileys;
    APTR    DISPLAY_SERVER_WIN_GRP;

    APTR    GROUP_ROOT_3, GR_grp_0, LA_nicknames, GR_grp_1, GR_local_charset,LA_realname, GR_grp_2;
    APTR    LA_username, GR_grp_3, REC_label_0, GR_grp_8, GR_grp_7, REC_label_0CC;
    APTR    GR_grp_9, GR_grp_6;
    APTR    GR_send_dcc, GR_dcc, GR_dcc_file_exists;
    APTR    grp_auto_open_query_tabs_when_msged;
    // BELOW are edit window variables
    APTR    GROUP_ROOT_4, GR_grp_13, LA_servername, GR_grp_11, LA_port, LA_pass, GR_grp_12;
    APTR    GROUP_ROOT_5, GR_grp_15, LA_groupname, GR_grp_16, GR_grp_17;

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

    APTR    LA_CSW_listviewtabs_background; //22
    APTR    LA_CSW_listviewtabs_normaltext; //33

    APTR    URLGRABBER_ROOT, URLGRABBER_GRP1, ABOUT_ROOT, GROUP_ROOT_2_SERVER, GROUP_ROOT_2, GROUP_ROOT_2_SECOND, GROUP_ROOT_2_DCC,GROUP_ROOT_2_ACTIONS, GR_grp_20, GR_grp_21, GR_grp_22, obj_aux0, obj_auto_open_query_tabs_when_msged_label;
    APTR    IGNORE_ROOT, IGNORE_GRP1, GR_autoconnect;
    APTR    ADDIGNORE_ROOT, ADDIGNORE_GRP1, ADDIGNORE_GRP2;
    APTR    obj_aux2, obj_aux3, REC_label_1, GR_grp_23, obj_aux4, obj_aux5, obj_aux6;
    APTR    obj_aux7, REC_label_2, GR_grp_24, obj_aux8, obj_aux9, obj_aux10, obj_aux11;
    APTR    obj_aux12, obj_aux13, obj_aux14, obj_aux15, REC_label_3, GR_grp_doubleclickactions;
    APTR    obj_aux16, obj_aux17, obj_aux18, obj_aux19, REC_label_4, GR_grp_26, GR_grp_reload_log;
    APTR    obj_aux20, obj_aux21, GR_grp_27, obj_aux22, obj_aux23, REC_label_5;
    APTR    GR_grp_28, GR_grp_31, GR_dcc_grp_28,GR_dcc_grp_282, GR_dcc_grp_29,REC_label_6,REC_label_7, GR_grp_29;
    APTR    obj_autojoin_channels_when_kicked_label, grp_autojoin_channels_when_kicked, GR_grp_32, GR_grp_33;
    APTR    obj_settings1_dcc, obj_settings2_dcc, obj_gethostid_dcc, obj_ipaddr_dcc, obj_port1_dcc, obj_port2_dcc;
    APTR    grp_dcc_settings1, grp_dcc_settings2,grp_dcc_settings3, grp_dcc_settings4, grp_dcc_settings5;
    APTR    GROUP_ROOT_2_ALIASES, GR_alias1, GR_alias2, EDIT_ALIAS_ROOT, EDIT_ALIAS_1;
    APTR    GROUP_ROOT_7, GROUP_ROOT_9;

    APTR    GROUP_ROOT_10, GR_grp_19, LA_dcc_send_file, GR_grp_30, LA_dcc_send_nick;
    APTR    QUIT_ROOT, quit_label;

    APTR source, instance;
    APTR source2, instance2;
    struct BitMap *bitmap2, *bitmap, *bitmap3;
    ULONG width, height;


    APTR about_group1, about_group2, about_group3,about_group4,about_group_old;
    APTR PICTURE_LOGO;

int AMIX=0;

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

struct ObjApp *CreateApp(void)
{

    //if(AMIX) printf("1\n");

    struct ObjApp *MBObj;

    if (!(MBObj = (struct ObjApp *) AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
        return(NULL);

if(AMIX) printf("2\n");
    static const char *titles2[] = { GetCatalogStr(catalog,19,"General"),
                                     "GUI",
                                     GetCatalogStr(catalog,260,"Servers"),
                                     GetCatalogStr(catalog,20,"Logging"),
                                     "DCC",
                                     GetCatalogStr(catalog,21,"Sounds"),
                                     GetCatalogStr(catalog,22,"Aliases"),
                                     GetCatalogStr(catalog,374,"Nick List Buttons"),
                                     GetCatalogStr(catalog,800,"Events"),
                                     NULL };
    

if(AMIX) printf("3\n");
        #ifdef __AROS__
        MBObj->LV_tabs = (Object*)NewObjectAROS(mcc2->mcc_Class,NULL, //nlist subclass
        #else
        MBObj->LV_tabs = (Object*)NewObject(mcc2->mcc_Class,NULL, //nlist subclass
        #endif
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, &ConstructLI_channel2_TextHook,
        MUIA_NList_DestructHook, &DestructLI_channel2_TextHook,
        MUIA_NList_DisplayHook, &Display_listviewtabs_TextHook,
        MUIA_NList_AutoCopyToClip, FALSE,
        MUIA_NList_Input, TRUE,
        MUIA_NList_ListBackground, background4,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
    End;

    MBObj->LV_tabs = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_nicklist",
        MUIA_NListview_Horiz_ScrollBar,MUIV_NListview_HSB_None,
        MUIA_NListview_Vert_ScrollBar,MUIV_NListview_VSB_Always,
        MUIA_Width, 150,
        //MUIA_MaxWidth, 150,
        MUIA_ObjectID, MAKE_ID('L', 'V', '0', '3'),
        MUIA_NListview_NList, MBObj->LV_tabs,
    End;

if(AMIX) printf("4\n");

    char about_text[2000];
    strcpy(about_text,VERSION_ABOUT);

    MBObj->TX_about = (Object*)TextObject,
        MUIA_Text_Contents,about_text,
        MUIA_Weight, 100,
    End;


    MBObj->TX_about2 = (Object*)TextObject,
        MUIA_Text_Contents,"\033cNew sponsor coming soon..",
        MUIA_Weight, 100,
    End;

    MBObj->TX_about_old = (Object*)TextObject,
        MUIA_Text_Contents,"\n\033cThanks go out to Guru Meditation for past sponsorship!\n\033cAmiga reseller and Linux consulting\n\033chttp://gurumeditation.se/\n",
        MUIA_Weight, 100,
    End;

    MBObj->TX_about3 = (Object*)TextObject,
        MUIA_Text_Contents,GetCatalogStr(catalog,333,"AREXX port: "),
        MUIA_Weight, 100,
    End;
if(AMIX) printf("5\n");
#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)

    about_group1 = GroupObject,
        MUIA_FixHeight, 78,
        //Child, PICTURE_LOGO = (Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "wookiechat_logo.gif", End,
        Child, PICTURE_LOGO = (Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:wookiechat_logo.gif", End,
        MUIA_Weight,100,

    End;

    about_group2 = GroupObject,
        Child, MBObj->TX_about,
        MUIA_Weight,100,                                                                                                                               \
    End;

    APTR about_group5 = GroupObject,
        Child, MBObj->TX_about_old,
        MUIA_Weight,100,
    End;


    about_group3 = GroupObject,
        MUIA_FixHeight, 114,
        Child, MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:avatar2.jpg", End,
        //Child, MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "avatar2.jpg", End,
        MUIA_Weight,100,
    End;


    about_group4 = GroupObject,
        Child, MBObj->TX_about2,
        MUIA_Weight,100,
    End;

    ABOUT_ROOT = GroupObject,
        MUIA_Background, "2:ffffffff,ffffffff,ffffffff",
        Child, about_group1,
        Child, about_group2,//amix sponsorship text
        Child, about_group3,//amix new logo
        Child, about_group4,
        Child, about_group5, //thanks go to Guru Meditation
        Child, MBObj->TX_about3,
    End;
if(AMIX) printf("6\n");
#else

    about_group2 = (Object*)GroupObject,
        Child, MBObj->TX_about,
        MUIA_Weight,100,

    End;

    about_group4 = (Object*)GroupObject,
        Child, MBObj->TX_about2,
        MUIA_Weight,100,

    End;
if(AMIX) printf("7\n");
    if(my_settings.os3_about_window_gfx)
    {

        about_group1 = GroupObject,
            MUIA_FixHeight, 78,
            Child, MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:wookiechat_logo.gif", End,
            MUIA_Weight,100,

        End;

        about_group3 = GroupObject,
            MUIA_FixHeight, 114,
            Child, MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:avatar2.jpg", End,
            MUIA_Weight,100,
        End;

        ABOUT_ROOT = (Object*)GroupObject,
            MUIA_Background, "2:ffffffff,ffffffff,ffffffff",
            Child, about_group1,
            Child, about_group2,
            Child, about_group3,
            Child, about_group4,
            Child, MBObj->TX_about3,
        End;


    }
    else
    {

        ABOUT_ROOT = (Object*)GroupObject,
            MUIA_Background, "2:ffffffff,ffffffff,ffffffff",
            Child, about_group2,
            Child, about_group4,
            Child, MBObj->TX_about3,
        End;

    }

#endif

if(AMIX) printf("8\n");
    //load graphical smilies

    MBObj->LA_local_charset=(Object*)Label(GetCatalogStr(catalog,328,"Local Charset:"));

    STRPTR *array;
if(AMIX) printf("9\n");
    if((array = CodesetsSupportedA(blank_taglist)))
    {
        int i, ii;

        for(i=0, ii=0; array[i] != NULL; i++)
        {
            charsets[i] = CodesetsFindA((char*)array[i],NULL);
            if(!strstr(array[i], "UTF"))
            {
                //local_selectable_charsets[ii] = CodesetsFindA((char*)array[i],NULL);
                local_charsets[ii]=new char[30];
                strcpy(local_charsets[ii],array[i]);
                ii++;
                //local_selectable_charsets[ii]=NULL;

            }

            remote_charsets[i]=new char[30];
            strcpy(remote_charsets[i],array[i]);
            
            if(DEBUG) printf("charset:%d %s\n",i,remote_charsets[i]);
        }


            charsets[i]=NULL;
            total_charsets=i-1;

            local_charsets[i+1]=NULL;
            remote_charsets[i+1]=NULL;

        //CodesetsFreeA(array, NULL);
    }
if(AMIX) printf("10\n");
    MBObj->CYCLE_local_charset = CycleObject,
       MUIA_Weight,0,
       MUIA_Cycle_Entries, local_charsets,
    End;
    
    MBObj->LV_urlgrabber = (Object*) NListObject,
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input, TRUE,
        MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
        #ifndef __AROS__
        //#ifndef __MORPHOS__
        MUIA_NList_ActiveObjectOnClick,TRUE,
        //#endif
        #endif
    End;

    MBObj->LV_urlgrabber = (Object*)NListviewObject,
        MUIA_Weight, 100,
        MUIA_NListview_NList, MBObj->LV_urlgrabber,
    End;
if(AMIX) printf("11\n");
    MBObj->BT_urlgrabber_clear=(Object*)SimpleButton(GetCatalogStr(catalog,23,"Remove URL(s) From List"));
    MBObj->BT_urlgrabber_clearall=(Object*)SimpleButton(GetCatalogStr(catalog,24,"Clear All"));
if(AMIX) printf("12\n");
    URLGRABBER_GRP1 =(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_urlgrabber_clear,
        Child, MBObj->BT_urlgrabber_clearall,
    End;

    URLGRABBER_ROOT = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        Child, MBObj->LV_urlgrabber,
        Child, URLGRABBER_GRP1,
    End;
if(AMIX) printf("13\n");
    MBObj->WI_urlgrabber = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,25,"URL Grabber - Double Click to Open"),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('7', '1', '1', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, URLGRABBER_ROOT,
        //MUIA_Window_ActiveObject, MBObj->LV_urlgrabber,
        //MUIA_Window_DefaultObject, WookieChat->LV_urlgrabber,
    End;


    MBObj->STR_addignore = (Object*)StringObject,
        MUIA_Weight, 80,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_MaxLen, 600,
        End;

    MBObj->BT_addignore = (Object*)TextObject,
        ButtonFrame,
        MUIA_Text_Contents,"blah", //GetCatalogStr(catalog,812,"blah"),
        MUIA_Text_PreParse, "\033c",
        MUIA_InputMode, MUIV_InputMode_RelVerify,
        //MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

    MBObj->LA_addignore_privmsg = (Object*)Label2(GetCatalogStr(catalog,265,"Text"));
    MBObj->CH_addignore_privmsg = (Object*)CheckMark(FALSE);
    
    MBObj->LA_addignore_ctcp = (Object*)Label2("CTCP");
    MBObj->CH_addignore_ctcp = (Object*)CheckMark(FALSE);


    MBObj->LA_addignore_dcc = (Object*)Label2("DCC");
    MBObj->CH_addignore_dcc = (Object*)CheckMark(FALSE);

    MBObj->TX_addignore =(Object*)TextObject,
        MUIA_Weight, 100,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, GetCatalogStr(catalog,270, "Hostmarks look like this: nick!userid@hostname\ni.e.«Join» jahc (~jahc00@jahc.Amigaworld.Net)\njahc is the nick, jahc00 is the userid, and jahc.Amigaworld.Net is the hostname\nTo ignore this person, you could insert an ignore entry of: jahc*!*@*\nThis will ignore all people with a nick starting with the letters jahc.\nThe use of wildcards makes this ignore command very powerful.\nThings like this are possible: guru*editatio*!*userid@*.se\nor if you want to keep it simple, you could just use: roberto*"),
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;
if(AMIX) printf("14\n");
    MBObj->LA_ignore_hostmask=(Object*)Label(GetCatalogStr(catalog,271,"Ignore entry:"));
    MBObj->LA_ignore_actions=(Object*)Label(GetCatalogStr(catalog,272,"Which events to ignore:"));


    ADDIGNORE_GRP1 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->LA_ignore_hostmask,
        Child, MBObj->STR_addignore,
        Child, MBObj->LA_addignore_privmsg,
        Child, MBObj->CH_addignore_privmsg,
        Child, MBObj->LA_addignore_ctcp,
        Child, MBObj->CH_addignore_ctcp,
        Child, MBObj->LA_addignore_dcc,
        Child, MBObj->CH_addignore_dcc,
    End;

if(AMIX) printf("15\n");
    ADDIGNORE_ROOT = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        Child, ADDIGNORE_GRP1,
        Child, MBObj->BT_addignore,
        Child, MBObj->TX_addignore,
    End;

    MBObj->WI_addignore = (Object*)WindowObject,
        MUIA_Window_Title, "jahc was here", //this window serves two purposes, "add ignore", and "edit ignore",
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('7', '2', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, ADDIGNORE_ROOT,
    End;
if(AMIX) printf("16\n");
        //MUIA_Background, MUII_BACKGROUND,

    MBObj->BT_ignore_add=(Object*)SimpleButton(GetCatalogStr(catalog,273,"Add"));
    MBObj->BT_ignore_edit=(Object*)SimpleButton(GetCatalogStr(catalog,274,"Edit"));
    MBObj->BT_ignore_remove=(Object*)SimpleButton(GetCatalogStr(catalog,275,"Remove"));

    MBObj->LV_ignore = (Object*) NListObject,
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook,  MUIV_NList_DestructHook_String,
        MUIA_NList_DisplayHook, &Display_ignore_TextHook,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input,TRUE,
        MUIA_NList_Title, TRUE,
        MUIA_NList_Format,"COL=0 WEIGHT=80 BAR, COL=1 WEIGHT=-1 BAR, COL=2 WEIGHT=-1 BAR, COL=3 WEIGHT=-1 BAR",
        MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
    End;

    MBObj->LV_ignore = (Object*)NListviewObject,
        MUIA_Weight, 100,
        MUIA_NListview_NList, MBObj->LV_ignore,
    End;
if(AMIX) printf("17\n");
    IGNORE_GRP1 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_ignore_add,
        Child, MBObj->BT_ignore_edit,
        Child, MBObj->BT_ignore_remove,
    End;

    IGNORE_ROOT = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        Child, MBObj->LV_ignore,
        Child, IGNORE_GRP1,
    End;
if(AMIX) printf("18\n");
    MBObj->WI_ignore = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,276,"Ignore List"),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('7', '1', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, IGNORE_ROOT,
    End;
if(AMIX) printf("19\n");
    //end of ignore stuff

    MBObj->WI_about = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,26,"About"),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('7', 'W', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, ABOUT_ROOT,
    End;

    // quit yes/no window

    quit_label = (Object*)Label(GetCatalogStr(catalog,60,"Quit Program?"));
    
    MBObj->BT_quit_yes = (Object*)SimpleButton(GetCatalogStr(catalog,27,"Yes"));
    MBObj->BT_quit_no = (Object*)SimpleButton(GetCatalogStr(catalog,28,"No"));

    QUIT_ROOT = (Object*)GroupObject,
        MUIA_Group_Columns, 3,
        Child, quit_label,
        Child, MBObj->BT_quit_yes,
        Child, MBObj->BT_quit_no,
    End;
if(AMIX) printf("20\n");
    MBObj->WI_quit = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,60,"Quit Program?"), //"Quit",
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_ID, MAKE_ID('9', 'W', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, QUIT_ROOT,
    End;


    MBObj->WI_change_nick = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,800,"Change nick"), //"Quit",
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', 'z', 'z', 'z'),
        //MUIA_Background, MUII_SHINE,
        WindowContents, GroupObject,
            MUIA_Group_Columns, 2,
            Child,MBObj->LA_change_nick=Label(GetCatalogStr(catalog,800,"Enter new nick")),
            Child,MBObj->STR_change_nick=StringObject,MUIA_Frame, MUIV_Frame_String,MUIA_String_Reject," ./",End,
            Child,MBObj->BT_change_nick_change=SimpleButton(GetCatalogStr(catalog,800,"Change")),
            Child,MBObj->BT_change_nick_cancel=SimpleButton(GetCatalogStr(catalog,800,"Cancel")),
        End,
    End;

    //dcc send initiate window

    LA_dcc_send_file = (Object*)Label(GetCatalogStr(catalog,61,"File to send"));

    MBObj->STR_PA_dcc_send_file = (Object*)String(my_settings.upload_path, 800);

    MBObj->PA_dcc_send_file = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_dcc_send_file = (Object*)PopaslObject,
        MUIA_HelpNode, "PA_dcc_send_file",
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GetCatalogStr(catalog,29,"Choose filename to send"),
        ASLFR_InitialDrawer,my_settings.upload_path,
        //ASLFR_InitialDrawer,"ram:",
        MUIA_Popstring_String, MBObj->STR_PA_dcc_send_file,
        MUIA_Popstring_Button, MBObj->PA_dcc_send_file,
    End;
if(AMIX) printf("21\n");
    GR_grp_19 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_19",
        MUIA_Group_Columns, 2,
        Child, LA_dcc_send_file,
        Child, MBObj->PA_dcc_send_file,
    End;

    LA_dcc_send_nick = (Object*)Label((char*)GetCatalogStr(catalog,240,"Reciever"));

    MBObj->STR_dcc_send_nick = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_dcc_send_nick",
        MUIA_String_Contents,"",
    End;
if(AMIX) printf("22\n");
    GR_grp_30= (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_1",
        MUIA_Group_Columns, 2,
        Child, LA_dcc_send_nick,
        Child, MBObj->STR_dcc_send_nick,
    End;

    MBObj->BT_dcc_send = (Object*)SimpleButton(GetCatalogStr(catalog,30,"Send File"));

    GROUP_ROOT_10 = (Object*)GroupObject,
        Child, GR_grp_19,
        Child, GR_grp_30,
        Child, MBObj->BT_dcc_send,
    End;
if(AMIX) printf("34\n");
    MBObj->WI_dcc_send = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,31,"DCC Send File"),
        MUIA_Window_ID, MAKE_ID('9', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_10,
    End;

if(AMIX) printf("35\n");
    MBObj->LV_graphical_smileys_preview = (Object*)NListObject,
        MUIA_Weight,100,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
        MUIA_NList_DisplayHook, &previewdisplay_TextHook,
    End;

    MBObj->LV_graphical_smileys_preview = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 100,
        MUIA_NListview_NList, MBObj->LV_graphical_smileys_preview,
    End;
if(AMIX) printf("36\n");
    MBObj->WI_graphical_smileys_preview = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,320,"Graphical Smileys Preview"),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', 'A', 'I', 'N'),
        WindowContents, MBObj->LV_graphical_smileys_preview,
    End;

    MBObj->GR_graphical_smileys_choose = (Object*)GroupObject,
        MUIA_Group_Columns, 10,
    End;
if(AMIX) printf("37\n");
    MBObj->WI_graphical_smileys_choose = (Object*)WindowObject,
        MUIA_Window_Title, " ",
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', 'A', 'B', 'N'),
        WindowContents, MBObj->GR_graphical_smileys_choose,
    End;


if(AMIX) printf("38\n");


//

    MBObj->LV_send_dcc = (Object*)NListObject,
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_ObjectID, MAKE_ID('1', 'D', 'C', 'C'),
        MUIA_NList_ConstructHook, &ConstructDCC_TextHook,
        MUIA_NList_DestructHook, &DestructDCC_TextHook,
        MUIA_NList_DisplayHook, &DisplayDCC_send_TextHook,
        MUIA_NList_Exports, MUIV_NList_Exports_ColWidth,
        MUIA_NList_Imports, MUIV_NList_Imports_ColWidth,
        //MUIA_NList_Format,"BAR,BAR,BAR,BAR,BAR,BAR,BAR,",
        MUIA_NList_Format,"WEIGHT=19 BAR,WEIGHT=15 BAR,WEIGHT=55 BAR,WEIGHT=10 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        //MUIA_NList_Format,"WEIGHT=-1 BAR,WEIGHT=15 BAR,WEIGHT=-1 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        MUIA_NList_Title, TRUE,
        #ifndef __AROS__
        //#ifndef __MORPHOS__
        MUIA_NList_ActiveObjectOnClick,TRUE,
        //#endif
        #endif
    End;
    
    MBObj->LV_send_dcc = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 80,
        MUIA_NListview_NList, MBObj->LV_send_dcc,
    End;

if(AMIX) printf("39\n");
    MBObj->BT_dcc_send_reoffer = (Object*)SimpleButton(GetCatalogStr(catalog,32,"Reoffer file"));
    MBObj->BT_dcc_send_cancel = (Object*)SimpleButton(GetCatalogStr(catalog,33,"Cancel Send"));
    MBObj->BT_dcc_send_remove = (Object*)SimpleButton(GetCatalogStr(catalog,34,"Remove"));
    MBObj->BT_dcc_send_new = (Object*)SimpleButton(GetCatalogStr(catalog,30,"Send File"));

    GR_send_dcc = (Object*)GroupObject,
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_dcc_send_new,
        Child, MBObj->BT_dcc_send_cancel,
        Child, MBObj->BT_dcc_send_remove,
    End;
if(AMIX) printf("40\n");
    MBObj->BT_dcc_resume = (Object*)SimpleButton(GetCatalogStr(catalog,35,"Resume"));
    MBObj->BT_dcc_overwrite = (Object*)SimpleButton(GetCatalogStr(catalog,36,"Overwrite"));
    MBObj->BT_dcc_abort = (Object*)SimpleButton(GetCatalogStr(catalog,37,"Abort"));
    MBObj->BT_dcc_rename = (Object*)SimpleButton(GetCatalogStr(catalog,369,"Rename"));

    GR_dcc_file_exists = (Object*)GroupObject,
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_dcc_resume,
        Child, MBObj->BT_dcc_overwrite,
        Child, MBObj->BT_dcc_rename,
        Child, MBObj->BT_dcc_abort,
    End;
if(AMIX) printf("41\n");
    MBObj->TX_oldname = (Object*)TextObject,End;
    MBObj->TX_oldsize = (Object*)TextObject,End;
    MBObj->TX_newsize = (Object*)TextObject,End;

    GR_dcc_grp_29 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_dcc_grp_29",
        Child, MBObj->TX_oldname,
        Child, MBObj->TX_oldsize,
        Child, MBObj->TX_newsize,
    End;

    GROUP_ROOT_8 = (Object*)GroupObject,
        Child, GR_dcc_grp_29,
        Child, GR_dcc_file_exists,
    End;
if(AMIX) printf("42\n");
    MBObj->WI_dcc_file_exists = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,38,"Incoming File already exists"),
        MUIA_Window_ID, MAKE_ID('6', 'W', 'I', 'N'),
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_NoMenus, FALSE,
        WindowContents, GROUP_ROOT_8,
        MUIA_Window_Activate,FALSE,
    End;

    MBObj->LA_upload_path = (Object*)Label2(GetCatalogStr(catalog, 371,"Upload Path"));

    MBObj->STR_dcc_PA_label_1 = (Object*)String("",200);
if(AMIX) printf("43\n");
    MBObj->PA_dcc_label_1 = (Object*)PopButton(MUII_PopDrawer);

    MBObj->PA_dcc_label_1 = (Object*)PopaslObject,
        ASLFR_DrawersOnly, TRUE,
        //MUIA_ShortHelp, GetCatalogStr(catalog,39,"Choose a path to save your downloads to"),
        MUIA_Popasl_Type, 0,
        MUIA_Popstring_String, MBObj->STR_dcc_PA_label_1,
        MUIA_Popstring_Button, MBObj->PA_dcc_label_1,
    End;

    GR_dcc_grp_282 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_dcc_grp_28",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->LA_upload_path,
        Child, MBObj->PA_dcc_label_1,
    End;
if(AMIX) printf("44\n");
    MBObj->LA_download_path = (Object*)Label2(GetCatalogStr(catalog, 305,"Download Path"));

    MBObj->STR_dcc_PA_label_0 = (Object*)String("",200);

    MBObj->PA_dcc_label_0 = (Object*)PopButton(MUII_PopDrawer);

    MBObj->PA_dcc_label_0 = (Object*)PopaslObject,
        ASLFR_DrawersOnly, TRUE,
        MUIA_ShortHelp, GetCatalogStr(catalog,39,"Choose a path to save your downloads to"),
        MUIA_Popasl_Type, 0,
        MUIA_Popstring_String, MBObj->STR_dcc_PA_label_0,
        MUIA_Popstring_Button, MBObj->PA_dcc_label_0,
    End;
if(AMIX) printf("45\n");
    GR_dcc_grp_28 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_dcc_grp_28",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->LA_download_path,
        Child, MBObj->PA_dcc_label_0,
    End;

    MBObj->BT_dcc_accept = (Object*)SimpleButton(GetCatalogStr(catalog,40,"Accept"));
    MBObj->BT_dcc_cancel = (Object*)SimpleButton(GetCatalogStr(catalog,41,"Cancel"));
    MBObj->BT_dcc_remove = (Object*)SimpleButton(GetCatalogStr(catalog,34,"Remove"));
if(AMIX) printf("46\n");
    GR_dcc = (Object*)GroupObject,
        MUIA_Group_Columns, 3,
        Child, MBObj->BT_dcc_accept,
        Child, MBObj->BT_dcc_cancel,
        Child, MBObj->BT_dcc_remove,
    End;

    MBObj->LV_dcc = (Object*)NListObject,
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_ObjectID, MAKE_ID('2', 'D', 'C', 'C'),
        MUIA_NList_ConstructHook, &ConstructDCC_TextHook,
        MUIA_NList_DestructHook, &DestructDCC_TextHook,
        MUIA_NList_DisplayHook, &DisplayDCC_recv_TextHook,
        //MUIA_NList_Format,"BAR,BAR,BAR,BAR,BAR,BAR,BAR,",
        MUIA_NList_Exports, MUIV_NList_Exports_ColWidth,
        MUIA_NList_Imports, MUIV_NList_Imports_ColWidth,
        //MUIA_NList_Format,"WEIGHT=-1 BAR,WEIGHT=15 BAR,WEIGHT=-1 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        MUIA_NList_Format,"WEIGHT=19 BAR,WEIGHT=15 BAR,WEIGHT=55 BAR,WEIGHT=10 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        MUIA_NList_Title, TRUE,
        #ifndef __AROS__
        MUIA_NList_ActiveObjectOnClick,TRUE,
        #endif
    End;
if(AMIX) printf("47\n");
    MBObj->LV_dcc = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 80,
        MUIA_NListview_NList, MBObj->LV_dcc,
    End;

    MBObj->LA_download_path2 = (Object*)Label2(GetCatalogStr(catalog, 305,"Download Path"));
    MBObj->TX_download_path = (Object*)TextObject,
        MUIA_Frame, MUIV_Frame_Text,
        MUIA_Weight, 100,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL,
    End;
if(AMIX) printf("48\n");
    APTR GR_download_path = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_download_path2,
        Child, MBObj->TX_download_path,
    End;

    GROUP_ROOT_7 = (Object*)GroupObject,
        //MUIA_FrameTitle, "Incoming File Transfers",
        MUIA_Group_Columns, 1,
        Child, GR_download_path,
        Child, MBObj->LV_dcc,
        Child, GR_dcc,
    End;
if(AMIX) printf("49\n");
    GROUP_ROOT_9 = (Object*)GroupObject,
        //MUIA_FrameTitle, "Outgoing File Transfers",
        Child, MBObj->LV_send_dcc,
        Child, GR_send_dcc,
    End;

    MBObj->WI_dcc = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,62,"Incoming File Transfers"),
        MUIA_Window_ID, MAKE_ID('5', 'W', 'I', 'N'),
        MUIA_Window_NoMenus, FALSE,
        WindowContents, GROUP_ROOT_7,
        MUIA_Window_Activate,FALSE,
    End;

    MBObj->WI_dcc2 = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,63,"Outgoing DCC File Transfers"),
        MUIA_Window_ID, MAKE_ID('8', 'W', 'I', 'N'),
        MUIA_Window_NoMenus, FALSE,
        WindowContents, GROUP_ROOT_9,
        MUIA_Window_Activate,FALSE,
    End;

    //below is the main settings window and objects
if(AMIX) printf("50\n");
    MBObj->RA_nick_completion_style_Label= (Object*)Label2(GetCatalogStr(catalog,42,"Nick Completion Style"));

    static const char *RA_nick_completion_style_Titles[] =
    {
       "AmIRC", "mIRC", NULL
    };

    MBObj->RA_nick_completion_style = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_nick_completion_style_Titles,
        MUIA_ShortHelp, GetCatalogStr(catalog,43,"AmIRC style puts a colon and a space\nafter users nicks if used at the beginning\nof a line, or just a space afterwards\nanywhere else.\nmIRC style: No colon\nor spaces are used, only the nick is\ndisplayed"),
    End;
if(AMIX) printf("51\n");
    GR_grp_32=(Object*)GroupObject,
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->RA_nick_completion_style_Label,
        Child, MBObj->RA_nick_completion_style,
        Child, HSpace(0),
    End;
if(AMIX) printf("52\n");
    static const char *RA_GroupTitleColor[] =
    {
       //"Overwrite", "Resume", "Abort", "Ask", NULL
       GetCatalogStr(catalog,36,"Overwrite"),
       GetCatalogStr(catalog,35,"Resume"),
       GetCatalogStr(catalog,37,"Abort"),
       GetCatalogStr(catalog,44,"Ask"),
       GetCatalogStr(catalog,369,"Rename"),
       NULL

    };
if(AMIX) printf("53\n");
    MBObj->RA_autoaccept = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_GroupTitleColor,
    End;

    MBObj->CH_autoaccept = (Object*)CheckMark(FALSE);
if(AMIX) printf("54\n");
    #ifdef __AROS__
    MBObj->LV_trusted_nicks = (Object*)NewObjectAROS(mcc2->mcc_Class,NULL,// NListObject,
    #else
    MBObj->LV_trusted_nicks = (Object*)NewObject(mcc2->mcc_Class,NULL,// NListObject,
        #endif
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_ListBackground, popimage_background,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
    End;
if(AMIX) printf("55\n");
    MBObj->CH_autojoin_channels_when_kicked = (Object*)CheckMark(FALSE);

    obj_autojoin_channels_when_kicked_label = (Object*)Label2(GetCatalogStr(catalog,45,"Auto-rejoin channels when kicked"));

    MBObj->CH_auto_open_query_tabs_when_msged = (Object*)CheckMark(FALSE);

    obj_auto_open_query_tabs_when_msged_label = (Object*)Label2(GetCatalogStr(catalog,46,"Auto-open private tabs when messaged"));

    grp_autojoin_channels_when_kicked = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        //Child, HSpace(0),
        Child, MBObj->CH_autojoin_channels_when_kicked,
        Child, obj_autojoin_channels_when_kicked_label,
        Child, HSpace(0),
    End;
if(AMIX) printf("56\n");
    grp_auto_open_query_tabs_when_msged = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        //Child, HSpace(0),
        Child, MBObj->CH_auto_open_query_tabs_when_msged,
        Child, obj_auto_open_query_tabs_when_msged_label,
        Child, HSpace(0),
    End;


    MBObj->STR_label_3 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_3",
        MUIA_String_Accept, "-0123456789",
        MUIA_String_MaxLen, 15,
    End;

    obj_aux3 = (Object*)Label2(GetCatalogStr(catalog,47,"Maximum number of lines to keep in buffer"));

    obj_aux2 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux3,
        Child, MBObj->STR_label_3,
    End;
    
if(AMIX) printf("57\n");
    REC_label_1 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->CH_label_2 = (Object*)CheckMark(FALSE);

    obj_aux5 = (Object*)Label2(GetCatalogStr(catalog,48,"Log channels"));


    MBObj->CH_label_3 = (Object*)CheckMark(FALSE);

    obj_aux7 = (Object*)Label2(GetCatalogStr(catalog,49,"Log private chats"));
if(AMIX) printf("58\n");
    obj_aux6 = (Object*)GroupObject,
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_3,
        Child, obj_aux7,
        Child, HSpace(0),
    End;

    GR_grp_23 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_23",
        //MUIA_FrameTitle, GetCatalogStr(catalog,20,"Logging"),
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        //Child, HSpace(0),
        Child, MBObj->CH_label_2,
        Child, obj_aux5,
        Child, HSpace(0),
        //Child, MBObj->CH_label_3,
        //Child, obj_aux7,
        //Child, HSpace(0),
    End;
if(AMIX) printf("59\n");
    REC_label_2 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->STR_label_1 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_1",
        MUIA_String_MaxLen, 800,
    End;
if(AMIX) printf("60\n");
    obj_aux9 = (Object*)Label2(GetCatalogStr(catalog,50,"Default Kick Message"));

    obj_aux8 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux9,
        Child, MBObj->STR_label_1,
    End;

    MBObj->STR_label_2 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_2",
        MUIA_String_MaxLen, 800,
    End;
if(AMIX) printf("61\n");
    obj_aux11 = (Object*)Label2(GetCatalogStr(catalog,51,"Default Ban Message"));

    obj_aux10 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux11,
        Child, MBObj->STR_label_2,
    End;

    MBObj->STR_label_4 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_4",
        MUIA_String_MaxLen, 400,
    End;

    obj_aux13 = (Object*)Label2(GetCatalogStr(catalog,52,"Quit Message"));
if(AMIX) printf("62\n");
    obj_aux12 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux13,
        Child, MBObj->STR_label_4,
    End;

    MBObj->STR_label_5 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_5",
        MUIA_String_MaxLen, 400,
    End;

    obj_aux15 = (Object*)Label2(GetCatalogStr(catalog,53,"Part Message"));

    obj_aux14 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux15,
        Child, MBObj->STR_label_5,
    End;
if(AMIX) printf("63\n");
    GR_grp_24 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_24",
        //MUIA_FrameTitle, "Messages",
        //Child, obj_aux2, //maximum lines to keep in buffer
        Child, obj_aux8,
        Child, obj_aux12,
        Child, obj_aux14,
    End;

    REC_label_3 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

if(AMIX) printf("64\n");
    MBObj->CH_label_4 = (Object*)CheckMark(FALSE);

    obj_aux17 = (Object*)Label2(GetCatalogStr(catalog,54,"Request Whois information"));
              
    MBObj->CH_label_5 = (Object*)CheckMark(FALSE);

    obj_aux19 = (Object*)Label2(GetCatalogStr(catalog,55,"Open private tab"));

    obj_aux18 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_5,
        Child, obj_aux19,
        Child, HSpace(0),
    End;
if(AMIX) printf("65\n");
    APTR obj_aux182 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_4,
        Child, obj_aux17,
        Child, HSpace(0),
    End;


              
    GR_grp_doubleclickactions = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_25",
        MUIA_Weight, 100,
        MUIA_FrameTitle, GetCatalogStr(catalog,56,"Nicklist doubleclick action(s)"),
        MUIA_Frame, MUIV_Frame_Group,
        MUIA_Group_Horiz, FALSE,
        Child, obj_aux18,
        Child, obj_aux182,
    End;


    REC_label_4 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->STR_label_0 = (Object*)StringObject,
        MUIA_Weight, 100,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp,GetCatalogStr(catalog,57,"Customise your timestamps! use %h for the hour, %m for the minute, and %s for the second. This is useful to see what time an event happened (such as a user joining, or speaking)"),
        MUIA_HelpNode, "STR_label_0",
        MUIA_String_MaxLen, 50,
    End;
if(AMIX) printf("66\n");
    obj_aux21 = (Object*)Label2(GetCatalogStr(catalog,58,"Timestamp Format"));

    MBObj->TX_highlight_label = (Object*)Label2(GetCatalogStr(catalog,59,"Words to Highlight"));

    MBObj->STR_highlight_label = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp,GetCatalogStr(catalog,258,"Enter words here for WookieChat to watch for. Lines of text will appear in the highlight colour if one of these words is found. Enter the words you want to highlight, separated by spaces"),
        MUIA_String_MaxLen, 800,
    End;

    
    GR_grp_26 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        //MUIA_FrameTitle, GetCatalogStr(catalog,64,"Highlight"),
        Child, MBObj->TX_highlight_label,
        Child, MBObj->STR_highlight_label,
    End;


    MBObj->CH_label_0 = (Object*)CheckMark(FALSE);
if(AMIX) printf("67\n");
    obj_aux23 = (Object*)Label2(GetCatalogStr(catalog,65,"Use timestamps"));

    obj_aux22 = (Object*)GroupObject,
        MUIA_FrameTitle, GetCatalogStr(catalog,66,"Timestamps"),
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_0,
        Child, obj_aux23,
        Child, HSpace(0),
    End;

    APTR obj_aux222 = (Object*)GroupObject,
        //MUIA_FrameTitle, GetCatalogStr(catalog,66,"Timestamps"),
        MUIA_Group_Horiz, TRUE,
        Child, obj_aux21,
        Child, MBObj->STR_label_0,
    End;


    REC_label_5 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;
if(AMIX) printf("68\n");
    MBObj->STR_PA_label_0 = (Object*)String("", 400);

    MBObj->PA_label_0 = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_label_0 = (Object*)PopaslObject,
        MUIA_HelpNode, "PA_label_0",
        MUIA_ShortHelp, GetCatalogStr(catalog,67,"This text editor is used when you edit your \"Perform on Connect\" scripts (from the Server Selection window)"),
        MUIA_Popstring_String, MBObj->STR_PA_label_0,
        MUIA_Popstring_Button, MBObj->PA_label_0,        
    End;

    REC_label_6 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->LA_texteditor = (Object*)Label2(GetCatalogStr(catalog,68,"Text Editor"));
if(AMIX) printf("69\n");
    GR_grp_28 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_28",
        MUIA_Group_Horiz, TRUE,
        //MUIA_FrameTitle, "Text Editor",
        Child, MBObj->LA_texteditor,
        Child, MBObj->PA_label_0,
    End;

    MBObj->PA_browser_label = (Object*)Label2(GetCatalogStr(catalog,69,"Web Browser"));

    MBObj->PA_browser = (Object*)PopButton(MUII_PopFile);

    MBObj->STR_browser = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GetCatalogStr(catalog,70,"Choose a web browser to use with WookieChat's \"URL Grabber\" Window. Double clicking an address there will load in this chosen browser"),
        MUIA_String_MaxLen, 1000,
    End;

    MBObj->PA_browser = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GetCatalogStr(catalog,71,"Choose browser to use"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_browser,
        MUIA_Popstring_Button, MBObj->PA_browser,
    End;
if(AMIX) printf("70\n");


             //
    GR_grp_31 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_31",
        MUIA_Group_Horiz, TRUE,
        //MUIA_FrameTitle, "Web Browser",
        Child, MBObj->PA_browser_label,
        Child, MBObj->PA_browser,
    End;

    MBObj->BT_label_0 = (Object*)SimpleButton(GetCatalogStr(catalog,72,"Save"));

    MBObj->BT_label_1 = (Object*)SimpleButton(GetCatalogStr(catalog,73,"Use"));

    MBObj->BT_label_2 = (Object*)SimpleButton(GetCatalogStr(catalog,74,"Cancel"));

    MBObj->BT_label_3 = (Object*)SimpleButton(GetCatalogStr(catalog,361,"Apply"));

    GR_grp_29 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_29",
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_label_0,
        Child, MBObj->BT_label_1,
        Child, MBObj->BT_label_3,
        Child, MBObj->BT_label_2,
    End;
if(AMIX) printf("71\n");

    obj_settings1_dcc = (Object*)Label2(GetCatalogStr(catalog,75,"Auto accept incoming DCC File Transfers"));
    obj_settings2_dcc = (Object*)Label2(GetCatalogStr(catalog,76,"Action to take if file exists"));

    obj_gethostid_dcc = (Object*)Label2(GetCatalogStr(catalog,77,"Use local address for DCC connections"));
    MBObj->CH_gethostid = (Object*)CheckMark(FALSE);

    MBObj->CH_ipaddr_dcc = (Object*)CheckMark(FALSE);
    obj_ipaddr_dcc = (Object*)Label2(GetCatalogStr(catalog,294,"Specify DCC address?"));
    MBObj->STR_ipaddr_dcc = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GetCatalogStr(catalog,295,"WookieChat attempts to obtain your address automatically for outgoing DCC connections. If this address is incorrect, you can specify it here"),
    End;
if(AMIX) printf("72\n");

    obj_port1_dcc = (Object*)Label2(GetCatalogStr(catalog,78,"DCC port range from "));
    obj_port2_dcc = (Object*)Label2(GetCatalogStr(catalog,79," to "));

    MBObj->STR_port1_dcc = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
        MUIA_ShortHelp, GetCatalogStr(catalog,80,"This is the start of the port range for incoming DCC connections. This is configurable so you can choose which ports to use and open up on any firewall you might have"),
    End;

    MBObj->STR_port2_dcc = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
        MUIA_ShortHelp, GetCatalogStr(catalog,81,"This is the end of the port range for incoming DCC connections. This is configurable so you can choose which ports to use and open up on any firewall you might have"),
    End;

    REC_label_7 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    grp_dcc_settings1 = (Object*)GroupObject,
        //MUIA_Group_Columns, 4,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->CH_autoaccept,
        Child, obj_settings1_dcc,
        Child, HSpace(0),
    End;

    grp_dcc_settings2 = (Object*)GroupObject,
        //MUIA_Group_Columns, 1,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, obj_settings2_dcc,
        Child, MBObj->RA_autoaccept,
        Child, HSpace(0),
    End;

    grp_dcc_settings3 = (Object*)GroupObject,
        //MUIA_Group_Columns, 4,
        //Child, HSpace(0),
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->CH_gethostid,
        Child, obj_gethostid_dcc,
        Child, HSpace(0),
    End;
if(AMIX) printf("74\n");
    grp_dcc_settings4 = (Object*)GroupObject,
        //MUIA_Group_Columns, 5,
        //Child, HSpace(0),
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_ipaddr_dcc,
        Child,obj_ipaddr_dcc,
        Child, MBObj->STR_ipaddr_dcc,
        //Child, HSpace(0),
    End;

    grp_dcc_settings5 = (Object*)GroupObject,
        //MUIA_Group_Columns, 6,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child,obj_port1_dcc,
        Child,MBObj->STR_port1_dcc,
        Child,obj_port2_dcc,
        Child,MBObj->STR_port2_dcc,
        //Child, HSpace(0),
    End;


    MBObj->LA_Maximum_Retries = (Object*)Label2(GetCatalogStr(catalog,261,"Maximum attempts to reconnect after non-requested disconnection"));

    MBObj->NB_Maximum_Retries = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;

    MBObj->LA_clone_detection = (Object*)Label2(GetCatalogStr(catalog,285,"Clone detection"));
    MBObj->CH_clone_detection = (Object*)CheckMark(FALSE);

    APTR GR_clone_detection = (Object*)GroupObject,
        MUIA_Weight, 100,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_clone_detection,
        Child, MBObj->LA_clone_detection,
        Child,HSpace(0),
    End;


if(AMIX) printf("75\n");
    MBObj->LA_Reconnect_Delay = (Object*)Label2(GetCatalogStr(catalog,262,"Delay (in seconds) in between reconnection attempts"));
    MBObj->NB_Reconnect_Delay = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    APTR GR_server1 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child,MBObj->LA_Maximum_Retries,    //maximum retrys (0 to turn off)
        Child,MBObj->NB_Maximum_Retries,    //maximum retrys (0 to turn off)
    End;

    APTR GR_server2 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child,MBObj->LA_Reconnect_Delay,   //reconnect delay (in seconds)
        Child,MBObj->NB_Reconnect_Delay,   //reconnect delay (in seconds)
    End;

    GROUP_ROOT_2_SERVER = (Object*)GroupObject,
        MUIA_Weight, 100,
        MUIA_Group_Columns, 1,
        Child, GR_server1,
        Child, GR_server2,
        Child, GR_clone_detection,
        Child, (Object*)GroupObject,
            MUIA_Weight, 100,
            MUIA_Group_Horiz, TRUE,
            Child, MBObj->CH_hide_joins_parts=(Object*)CheckMark(FALSE),
            Child, MBObj->LA_hide_joins_parts=(Object*)Label2(GetCatalogStr(catalog,383,"Hide Join and Part messages in Channels")),
            Child, HSpace(0),
        End,
        Child, (Object*)GroupObject,
            MUIA_Weight, 100,
            MUIA_Group_Horiz, TRUE,
            Child, MBObj->CH_hide_channel_mode_gadgets=(Object*)CheckMark(FALSE),
            Child, MBObj->LA_hide_channel_mode_gadgets=(Object*)Label2(GetCatalogStr(catalog,800,"Hide the channel mode buttons at the top of the display")),
            Child, HSpace(0),
        End,
    End;
if(AMIX) printf("76\n");
    //GUI SETTINGS objects
    MBObj->LA_no_server_tabs = (Object*)Label2(GetCatalogStr(catalog,292,"Disable server tabs"));
    MBObj->CH_no_server_tabs = (Object*)CheckMark(FALSE);

    static const char *CYCLE_tabs_entries[] =
    {
       GetCatalogStr(catalog,375,"Listview"),
       GetCatalogStr(catalog,376,"Buttons"),
       NULL,
    };

    //MBObj->LA_tabs = (Object*)Label2(GetCatalogStr(catalog,296,"Use button-style tabs"));
    //MBObj->CH_tabs = (Object*)CheckMark(TRUE);
    MBObj->LA_tabs = (Object*)Label2(GetCatalogStr(catalog,377,"Method of displaying Tabs for channels and private messages"));
    MBObj->CYCLE_tabs = (Object*)CycleObject,
       MUIA_Weight,0,
       MUIA_Cycle_Entries, CYCLE_tabs_entries,
    End;
    //MBObj->CH_tabs = (Object*)CheckMark(TRUE);



    MBObj->LA_listview_tabs_weight = (Object*)Label2(GetCatalogStr(catalog,306,"Listview tabs vertical weight"));
    MBObj->NB_listview_tabs_weight = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;

if(AMIX) printf("77\n");
    MBObj->LA_nicklist_horiz_weight = (Object*)Label2(GetCatalogStr(catalog,321,"Nicklist horizontal weight"));

    MBObj->NB_nicklist_horiz_weight = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    APTR GR_grp_34 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_29",
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        Child,MBObj->LA_listview_tabs_weight,
        Child,MBObj->NB_listview_tabs_weight,
        //Child,HSpace(0),
        //Child,MBObj->LA_nicklist_horiz_weight,
        //Child,MBObj->NB_nicklist_horiz_weight,
    End;

    APTR GR_grp_342 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_29",
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        //Child,MBObj->LA_listview_tabs_weight,
        //Child,MBObj->NB_listview_tabs_weight,
        Child,MBObj->LA_nicklist_horiz_weight,
        Child,MBObj->NB_nicklist_horiz_weight,
        //Child,HSpace(0),
    End;
if(AMIX) printf("78\n");
    GR_grp_33 = (Object*)GroupObject,
        MUIA_FrameTitle,GetCatalogStr(catalog,307,"Tab Preferences"),
        //MUIA_Frame,MUIV_Frame_Group,
        MUIA_HelpNode, "GR_grp_29",
        MUIA_Group_Horiz, TRUE,
        //MUIA_Group_Columns, 4,
        //MUIA_ShortHelp,GetCatalogStr(catalog,297,"Turn off this option to use Listview-style tabs (the listview is better having a large amount of tabs open, such as 20 or 30)"),
        //Child, HSpace(0),
        Child,MBObj->CYCLE_tabs,
        Child,MBObj->LA_tabs,
        //Child,MBObj->CH_no_server_tabs,
        //Child,MBObj->LA_no_server_tabs,
        Child, HSpace(0),
    End;
if(AMIX) printf("79\n");
    APTR GR_grp_332 = (Object*)GroupObject,
        //MUIA_FrameTitle,GetCatalogStr(catalog,307,"Tab Preferences"),
        //MUIA_Frame,MUIV_Frame_Group,
        MUIA_HelpNode, "GR_grp_29",
        MUIA_Group_Horiz, TRUE,
        Child,MBObj->CH_no_server_tabs,
        Child,MBObj->LA_no_server_tabs,
        Child, HSpace(0),
    End;

if(AMIX) printf("80\n");
    setmacro((Object*)MBObj->LA_tabs,MUIA_ShortHelp,GetCatalogStr(catalog,297,"Turn off this option to use Listview-style tabs (the listview is better having a large amount of tabs open, such as 20 or 30)"));
    setmacro((Object*)MBObj->LA_listview_tabs_weight,MUIA_ShortHelp,GetCatalogStr(catalog,308,"If you have disabled button style tabs, you can configure the size of the \"tabs listview\""));
    setmacro((Object*)MBObj->LA_no_server_tabs,MUIA_ShortHelp,GetCatalogStr(catalog,309,"You can turn off keeping a dedicated tab for server messages to conserve memory and valuable screen space"));
if(AMIX) printf("81\n");

    MBObj->LA_graphical_smileys = (Object*)Label2(GetCatalogStr(catalog,300,"Enable Graphical Smileys"));
    MBObj->CH_graphical_smileys = (Object*)CheckMark(TRUE);

    MBObj->LA_graphical_nicklist = (Object*)Label2(GetCatalogStr(catalog,310,"Enable Graphical Nicklist"));
    MBObj->CH_graphical_nicklist = (Object*)CheckMark(TRUE);


    MBObj->RA_graphical_smileys_background_colour_label= (Object*)Label2(GetCatalogStr(catalog,322,"Smileys theme"));
if(AMIX) printf("82\n");
    //char graphical_smiley_themes[10][100];

    if(find_themes())
    {
        for(count2=0; count2<count; count2++)
        {
            CYA_GroupTitleColor[count2]=new char[100];
            strcpy(CYA_GroupTitleColor[count2],graphical_smiley_themes[count2]);
        }
        CYA_GroupTitleColor[count2]=NULL;

        MBObj->graphical_smileys_set = CycleObject,
         MUIA_Cycle_Entries, CYA_GroupTitleColor,
        End;
    }
    else
    {
        CYA_GroupTitleColor[0]=new char[100];
        strcpy(CYA_GroupTitleColor[0],"");

        CYA_GroupTitleColor[1]=NULL;

        MBObj->graphical_smileys_set = CycleObject,
         MUIA_Cycle_Entries, CYA_GroupTitleColor,
        End;


    }
if(AMIX) printf("84\n");
    MBObj->BT_graphical_smileys_preview = (Object*)SimpleButton("?");
    setmacro((Object*)MBObj->BT_graphical_smileys_preview,MUIA_Weight,0);

    GR_graphical_smileys=(Object*)GroupObject,
        MUIA_ShortHelp, GetCatalogStr(catalog,302,"This option will display common ASCII \"smiley faces\" such as :) and :D in aa graphical way.\n(This feature is reliant on picture datatypes)"),
        MUIA_HelpNode, "GR_grp_31",
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_graphical_smileys,
        Child, MBObj->LA_graphical_smileys,
        Child, HSpace(0),
    End;

    APTR GR_graphical_nicklist=(Object*)GroupObject,
        MUIA_ShortHelp, GetCatalogStr(catalog,302,"This option will display common ASCII \"smiley faces\" such as :) and :D in aa graphical way.\n(This feature is reliant on picture datatypes)"),
        MUIA_HelpNode, "GR_grp_31",
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_graphical_nicklist,
        Child, MBObj->LA_graphical_nicklist,
        Child, HSpace(0),
    End;
if(AMIX) printf("85\n");
    MBObj->LA_column_width_timestamp = (Object*)Label2(GetCatalogStr(catalog,312,"Column width (in Pixels) for timestamps"));

    MBObj->NB_column_width_timestamp = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    MBObj->LA_column_width_nick = (Object*)Label2(GetCatalogStr(catalog,313,"Column width (in Pixels) for the middle/nicks column"));

    MBObj->NB_column_width_nick = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    MBObj->LA_column_spacing = (Object*)Label2(GetCatalogStr(catalog,314,"Amount of space surrounding columns divider bar"));

    //MBObj->NB_column_spacing = (Object*)NumericbuttonObject,
    MBObj->NB_column_spacing = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    MBObj->LA_remove_brackets = (Object*)Label2(GetCatalogStr(catalog,315,"Remove < and > from around nicks"));
    MBObj->CH_remove_brackets = (Object*)CheckMark(TRUE);
    
    APTR GR_column_width = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        MUIA_FrameTitle, GetCatalogStr(catalog,316,"Multicolumn display settings"),
        Child, MBObj->LA_column_width_timestamp,
        Child, MBObj->NB_column_width_timestamp,
        //Child,HSpace(0),
    End;

    APTR GR_column_width2 = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,2,
        //MUIA_FrameTitle, GetCatalogStr(catalog,316,"Multicolumn display settings"),
        //MUIA_Frame,MUIV_Frame_Group,
        Child, MBObj->LA_column_width_nick,
        Child, MBObj->NB_column_width_nick,
        //Child,HSpace(0),
    End;

    APTR GR_column_width3 = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,2,
        //MUIA_FrameTitle, GetCatalogStr(catalog,316,"Multicolumn display settings"),
        //MUIA_Frame,MUIV_Frame_Group,
        Child, MBObj->LA_column_spacing,
        Child, MBObj->NB_column_spacing,
        //Child,HSpace(0),
    End;

    APTR GR_column_width4 = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,2,
        //MUIA_FrameTitle, GetCatalogStr(catalog,316,"Multicolumn display settings"),
        //MUIA_Frame,MUIV_Frame_Group,
        Child, MBObj->CH_remove_brackets,
        Child, MBObj->LA_remove_brackets,
        Child, HSpace(0),
    End;

    APTR GR_smiley_sets = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,3,
        Child, MBObj->RA_graphical_smileys_background_colour_label,
        Child, MBObj->graphical_smileys_set,
        Child, MBObj->BT_graphical_smileys_preview,
        Child, HSpace(0),
    End;
if(AMIX) printf("86\n");
    APTR GROUP_ROOT_2_GUI = (Object*)GroupObject,
        Child, obj_aux22,//timestamps
        Child, obj_aux222,//timestamps
        Child, GR_grp_33, //listview tabs or button tabs
        Child, GR_grp_332, //listview tabs or button tabs
        Child, GR_grp_34, //nicklist horizontal weight, and listview tabs vertical weight
        Child, GR_grp_342, //nicklist horizontal weight, and listview tabs vertical weight
        Child, GR_graphical_smileys,
        Child, GR_graphical_nicklist,
        Child, GR_smiley_sets,
        Child, GR_column_width,
        Child, GR_column_width2,
        Child, GR_column_width3,
        Child, GR_column_width4,
        Child, (Object*)GroupObject,
            MUIA_Group_Horiz,TRUE,
            Child, MBObj->CH_user_modes = (Object*)CheckMark(FALSE),
            Child, MBObj->LA_user_modes = (Object*)Label2(GetCatalogStr(catalog,800,"Display usermodes beside a persons nick when they talk")),
            Child, HSpace(0),
        End,
        //Child, GR_column_width,
        //Child, GR_remove_brackets,
    End;
if(AMIX) printf("87\n");
    GROUP_ROOT_2_DCC = (Object*)GroupObject,
        Child, grp_dcc_settings1,
        Child, grp_dcc_settings2,
        Child, grp_dcc_settings3,
        Child, grp_dcc_settings4,
        Child, grp_dcc_settings5,
        Child, GR_dcc_grp_28,
        Child, GR_dcc_grp_282,
    End;
if(AMIX) printf("88\n");
    GROUP_ROOT_2 = (Object*)GroupObject,
        MUIA_Weight, 100,
        MUIA_HelpNode, "GR_grp_21",
        Child, grp_autojoin_channels_when_kicked,
        Child, grp_auto_open_query_tabs_when_msged,
        Child, GR_grp_32, //nick completion style group
        Child, GR_grp_24,
        Child, GR_grp_doubleclickactions,
        //Child, obj_aux18,
        Child, GR_grp_26,
        Child, GR_grp_28,
        Child, GR_grp_31,
        //Child, GR_grp_21,
    End;
if(AMIX) printf("89\n");
    // new message sample

    static const char *RA_sound_sample_newmsg_entries[] =
    {
       GetCatalogStr(catalog,82,"Never"),
       GetCatalogStr(catalog,367,"When private tab is not selected"),
       GetCatalogStr(catalog,83,"Always"),
       GetCatalogStr(catalog,84,"Only when Window is inactive"),
       NULL,
    };
if(AMIX) printf("90\n");
    MBObj->LA_sound_sample_newmsg = (Object*)Label2(GetCatalogStr(catalog,366,"Sample to play when a new PRIVMSG arrives"));

    MBObj->STR_sound_sample_newmsg = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, " ",
    End;

    MBObj->PA_sound_sample_newmsg_label = (Object*)Label2(GetCatalogStr(catalog,86,"Play sample"));

    MBObj->PA_sound_sample_newmsg = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_sound_sample_newmsg = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GetCatalogStr(catalog,87,"Choose filename"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_sample_newmsg,
        MUIA_Popstring_Button, MBObj->PA_sound_sample_newmsg,
    End;

    MBObj->RA_sound_sample_newmsg = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_sound_sample_newmsg_entries,
    End;
       
    //tabopen action entries

    static const char *RA_sound_sample_tabopen_entries[] =
    {
       GetCatalogStr(catalog,82,"Never"),
       GetCatalogStr(catalog,83,"Always"),
       GetCatalogStr(catalog,84,"Only when Window is inactive"),
       NULL,
    };
if(AMIX) printf("91\n");
    MBObj->LA_sound_sample_tabopen = (Object*)Label2(GetCatalogStr(catalog,85,"Sample to play when a tab opens"));

    MBObj->STR_sound_sample_tabopen = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, " ",
    End;

    MBObj->PA_sound_sample_tabopen_label = (Object*)Label2(GetCatalogStr(catalog,86,"Play sample"));

    MBObj->PA_sound_sample_tabopen = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_sound_sample_tabopen = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GetCatalogStr(catalog,87,"Choose filename"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_sample_tabopen,
        MUIA_Popstring_Button, MBObj->PA_sound_sample_tabopen,
    End;

    MBObj->RA_sound_sample_tabopen = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_sound_sample_tabopen_entries,
    End;
                                                  

    //highlight action objects
if(AMIX) printf("92\n");
    static const char *RA_sound_sample_highlight_entries[] =
    {
       //"Never", "Always", "Only when Window is inactive", NULL
       GetCatalogStr(catalog,82,"Never"),
       GetCatalogStr(catalog,83,"Always"),
       GetCatalogStr(catalog,84,"Only when Window is inactive"),
       GetCatalogStr(catalog,373,"Only when Tab is inactive"),
       NULL
    };

    MBObj->LA_sound_sample_highlight = (Object*)Label2(GetCatalogStr(catalog,88,"Sample to play on highlight"));

    MBObj->STR_sound_sample_highlight = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, " ",
    End;
    
    MBObj->PA_sound_sample_highlight_label = (Object*)Label2(GetCatalogStr(catalog,86,"Play sample"));

    MBObj->PA_sound_sample_highlight = (Object*)PopButton(MUII_PopFile);
if(AMIX) printf("93\n");
    MBObj->PA_sound_sample_highlight = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GetCatalogStr(catalog,87,"Choose filename"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_sample_highlight,
        MUIA_Popstring_Button, MBObj->PA_sound_sample_highlight,
    End;

    MBObj->RA_sound_sample_highlight = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_sound_sample_highlight_entries,
    End;
           
    MBObj->LA_samples_path = (Object*)Label2(GetCatalogStr(catalog,89,"Samples Path for CTCP SOUND"));
    
    MBObj->STR_samples_path = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GetCatalogStr(catalog,90,"When you recieve a CTCP SOUND request, WookieChat will\nlook in this path for sound samples"),
        MUIA_String_MaxLen, 400,
    End;

    MBObj->PA_samples_path = (Object*)PopButton(MUII_PopDrawer);

    MBObj->PA_samples_path = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GetCatalogStr(catalog,91,"Choose directory"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_samples_path,
        MUIA_Popstring_Button, MBObj->PA_samples_path,
    End;

if(AMIX) printf("94\n");
    //external sound player for CTCP SOUND
    MBObj->LA_sound_replayer = (Object*)Label2(GetCatalogStr(catalog,303,"Use External Replayer For CTCP SOUND"));
    MBObj->CH_sound_replayer = (Object*)CheckMark(FALSE);
    MBObj->STR_sound_replayer = (Object*)StringObject,MUIA_Frame, MUIV_Frame_String,MUIA_ShortHelp, " ",End;
    MBObj->PA_sound_replayer_label = (Object*)Label2(GetCatalogStr(catalog,304,"Sound player"));
    MBObj->PA_sound_replayer = (Object*)PopButton(MUII_PopFile);
    MBObj->PA_sound_replayer = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        //ASLFR_TitleText,GetCatalogStr(catalog,87,"Choose Sound Player"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_replayer,
        MUIA_Popstring_Button, MBObj->PA_sound_replayer,
    End;

if(AMIX) printf("95\n");
    APTR LA_space3 =(Object*)TextObject,
        MUIA_Weight, 1,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL, //"             ",
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;

    APTR LA_space4 =(Object*)TextObject,
        MUIA_Weight, 1,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL, //"             ",
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;

    APTR GR_sound_replayer=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 4,
        Child, LA_space4,
        Child, MBObj->LA_sound_replayer,
        Child, MBObj->CH_sound_replayer,
        Child, LA_space3,
    End;

    APTR GR_choose_sound_replayer=(Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Columns, 2,
        Child, MBObj->PA_sound_replayer_label,
        Child, MBObj->PA_sound_replayer,
    End;
if(AMIX) printf("96\n");
    APTR GR_ctcp_sound=(Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Columns, 1,
        Child, GR_sound_replayer,
        Child, GR_choose_sound_replayer,
    End;

    APTR GR_samples1=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Frame,MUIV_Frame_Group,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_sound_sample_tabopen,
        Child, MBObj->PA_sound_sample_tabopen,
        Child, MBObj->PA_sound_sample_tabopen_label,
        Child, MBObj->RA_sound_sample_tabopen,
    End;

    APTR GR_samples2=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Frame,MUIV_Frame_Group,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_sound_sample_highlight,
        Child, MBObj->PA_sound_sample_highlight,
        Child, MBObj->PA_sound_sample_highlight_label,
        Child, MBObj->RA_sound_sample_highlight,
    End;

    APTR GR_samples3=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Frame,MUIV_Frame_Group,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_sound_sample_newmsg,
        Child, MBObj->PA_sound_sample_newmsg,
        Child, MBObj->PA_sound_sample_newmsg_label,
        Child, MBObj->RA_sound_sample_newmsg,
    End;

    APTR GR_samples_path=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_samples_path,
        Child, MBObj->PA_samples_path,
    End;
if(AMIX) printf("97\n");
//HGroup, GroupFrame, Child, HSpace(0), Child, YourOptionGroup, Child, HSpace(0), End,

    APTR GR_samples_prefs=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 1,
        Child, GR_samples1,
        Child, GR_samples2,
        Child, GR_samples3,
        Child, GR_samples_path,
    End;

    GROUP_ROOT_2_ACTIONS = (Object*)GroupObject,
        MUIA_Group_Columns, 1,
        Child, GR_samples_prefs,
        Child, GR_ctcp_sound,
        //Child, GR_sound_replayer,
        //Child, GR_choose_sound_replayer,
    End;

    // log reloading gui objects

    MBObj->TX_how_many_lines_to_reload = (Object*)TextObject,
        MUIA_Text_Contents, GetCatalogStr(catalog,92,"# of lines to display from the end of logfiles when opening a tab\n(Put 0 to disable this feature)"),
    End;
if(AMIX) printf("98\n");
    APTR GR_how_many_lines_to_reload = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->TX_how_many_lines_to_reload,
        Child, HSpace(0),
    End;

    APTR obj_how_many_lines_to_reload_channel = (Object*)Label2(GetCatalogStr(catalog,93,"channel"));

    MBObj->STR_how_many_lines_to_reload_channel = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GetCatalogStr(catalog,94,"How many lines to reload from the end of log files when you join a channel"),
        MUIA_String_Accept, "0123456789",
        MUIA_String_MaxLen, 4,
    End;
if(AMIX) printf("99\n");
    APTR obj_how_many_lines_to_reload_private = (Object*)Label2(GetCatalogStr(catalog,95,"private"));

    MBObj->STR_how_many_lines_to_reload_private = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GetCatalogStr(catalog,96,"How many lines to reload from the end of log files when you open a private query tab to another user"),
        MUIA_String_Accept, "0123456789",
        MUIA_String_MaxLen, 4,
    End;

    APTR GR_grp_reload_log2 = (Object*)GroupObject,
        //MUIA_Group_Columns, 4,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, obj_how_many_lines_to_reload_channel,
        Child, MBObj->STR_how_many_lines_to_reload_channel,
        Child, obj_how_many_lines_to_reload_private,
        Child, MBObj->STR_how_many_lines_to_reload_private,
        //Child, HSpace(0),
    End;

    if(AMIX) printf("100\n");

    MBObj->CH_split_logs = (Object*)CheckMark(FALSE);
    MBObj->LA_split_logs = (Object*)Label(GetCatalogStr(catalog,289,"Split up logfiles into smaller chunks"));

    MBObj->LA_logsize = (Object*)Label(GetCatalogStr(catalog,290,"If the above is enabled, choose a log file size (in kbytes)"));
    MBObj->STR_logsize = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_servername",
    End;

    GR_logging_splitup = (Object*)GroupObject,
        MUIA_ShortHelp, GetCatalogStr(catalog,291,"When a tab is opened, the logfile will be examined. If it is over a certain size, then it will be renamed with the date appended to the end, and WookieChat will resume logging in a new empty logfile. This is implemented for performance reasons. It can take a few seconds to reload large logfiles that are a few MB in size."),
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_split_logs,
        Child, MBObj->LA_split_logs,
        Child, HSpace(0),
    End;

    GR_logging_splitup2 = (Object*)GroupObject,
        MUIA_ShortHelp, GetCatalogStr(catalog,291,"When a tab is opened, the logfile will be examined. If it is over a certain size, then it will be renamed with the date appended to the end, and WookieChat will resume logging in a new empty logfile. This is implemented for performance reasons, because it can sometimes take a few seconds to reload large logfiles that are a few MB in size."),
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_logsize,
        Child, MBObj->STR_logsize,
    End;
if(AMIX) printf("101\n");
    APTR GR_reload = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        MUIA_Frame, MUIV_Frame_Group,
        Child, GR_how_many_lines_to_reload,
        Child, GR_grp_reload_log2,
    End;


if(AMIX) printf("102\n");
    GROUP_ROOT_2_SECOND = (Object*)GroupObject,
        Child, obj_aux2, //maximum lines to keep in buffer.. feature currently removed due to crashing
        Child, GR_logging_splitup,
        Child, GR_logging_splitup2,
        Child, GR_grp_23,
        Child, obj_aux6,
        Child, GR_reload,
        //Child, GR_how_many_lines_to_reload,
        //Child, GR_grp_reload_log2,
    End;

    MBObj->LV_alias = (Object*) NListObject,
        MUIA_ShortHelp, GetCatalogStr(catalog,98,"Enter an alias name, then the command you want it to use\nE.g. With this line added to the alias list..\n/j /join\n..then the \"/j\" command will be replaced with \"/join\""),
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_DisplayHook, &Display_alias_TextHook,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input,TRUE,
        MUIA_NList_Title, TRUE,
        MUIA_NList_Format,"COL=0 WEIGHT=-1 BAR, COL=1 BAR",
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
    End;


    MBObj->LV_alias = (Object*) NListviewObject,
        MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always,
        MUIA_NListview_NList, MBObj->LV_alias,
    End;

    GR_alias1 = (Object*)GroupObject,
        Child, MBObj->LV_alias,
    End;

    MBObj->BT_add_alias = (Object*)SimpleButton(GetCatalogStr(catalog,99,"Add alias"));
    MBObj->BT_remove_alias = (Object*)SimpleButton(GetCatalogStr(catalog,100,"Remove alias"));

    GR_alias2 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, MBObj->BT_add_alias,
        Child, MBObj->BT_remove_alias,
    End;

    GROUP_ROOT_2_ALIASES = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        Child, GR_alias2,
        Child, GR_alias1,
        Child, (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 4,
            Child, Label(GetCatalogStr(catalog,280,"Alias")),
            Child, MBObj->STR_edit_alias_name=(Object*)StringObject,MUIA_Weight, 30, MUIA_Frame, MUIV_Frame_String,MUIA_String_Reject," ",MUIA_String_MaxLen, 50,End,
            Child, Label(GetCatalogStr(catalog,281,"Command")),
            Child, MBObj->STR_edit_alias_command=(Object*)StringObject,MUIA_Weight, 70, MUIA_Frame, MUIV_Frame_String,MUIA_String_MaxLen, 650,End,
        End,
    End;

    MBObj->LV_user_list_buttons = (Object*)NListObject,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input,TRUE,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_DisplayHook, &DisplayHook_LV_userlist_buttons,
        MUIA_NList_Title, TRUE,
        MUIA_NList_Format,"COL 0 BAR, COL 1",
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
        MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted,
    End;

    MBObj->GR_click_user_list_buttons = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        MUIA_Group_HorizSpacing, 1,
        MUIA_Group_VertSpacing, 1,
    End,

    MBObj->GR_user_list_buttons = (Object*)GroupObject,
        MUIA_Group_Columns, 1,
            Child, GroupObject,
                MUIA_Group_Columns,4,
                MUIA_Group_Horiz,TRUE,
                MUIA_Weight, 100,
                Child, MBObj->BT_user_list_buttons_add=SimpleButton(GetCatalogStr(catalog,378,"Add New")),
                Child, MBObj->BT_user_list_buttons_delete=SimpleButton(GetCatalogStr(catalog,379,"Delete")),
                Child, MBObj->BT_user_list_buttons_move_up=SimpleButton(GetCatalogStr(catalog,380,"Move up")),
                Child, MBObj->BT_user_list_buttons_move_down=SimpleButton(GetCatalogStr(catalog,381,"Move down")),
            End,
            Child, GroupObject,
                MUIA_Group_Columns,2,
                MUIA_Group_Horiz,TRUE,
                Child, MBObj->LA_user_list_buttons_help=Label(GetCatalogStr(catalog,382,"\033lCodes:\n%s = selected nick, %h = hostname of the selected nick,\n%c = current channel, %n = your own nick")),
                Child, HSpace(0),
            End,
            Child, MBObj->LV_user_list_buttons2 = (Object*)NListviewObject,
                MUIA_HelpNode, "LV_channel",
                MUIA_Weight, 100,
                MUIA_NListview_NList, MBObj->LV_user_list_buttons,
                MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always,
             End,
            Child, GroupObject,
                MUIA_Group_Columns,6,
                MUIA_Group_Horiz,TRUE,
                MUIA_Weight, 100,
                Child, Label("Name"),
                Child, MBObj->STR_user_list_buttons_name=(Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 30,
                    MUIA_Frame, MUIV_Frame_String,
                    //MUIA_ShortHelp, GetCatalogStr(catalog,17,"This is the hostname of the user you are currently talking to"),
                End,
                Child, Label("Command"),
                Child, MBObj->STR_user_list_buttons_command=(Object*)BetterStringObject,
                    MUIA_Weight, 70,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                    //MUIA_ShortHelp, GetCatalogStr(catalog,17,"This is the hostname of the user you are currently talking to"),
                End,
                Child, MBObj->LA_user_list_buttons_disable=Label(GetCatalogStr(catalog,384,"Hide all from the display")),
                Child, MBObj->CH_user_list_buttons_disable=CheckMark(FALSE),
            End,
        End;



    LV_events = (Object*) NListObject,
            MUIA_ShortHelp, GetCatalogStr(catalog,98,"Enter an alias name, then the command you want it to use\nE.g. With this line added to the alias list..\n/j /join\n..then the \"/j\" command will be replaced with \"/join\""),
            MUIA_Frame, MUIV_Frame_InputList,
            MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
            MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
            MUIA_NList_DisplayHook, &Display_events_TextHook,
            MUIA_NList_AutoCopyToClip, TRUE,
            MUIA_NList_Input,TRUE,
            MUIA_NList_Title, TRUE,
            MUIA_NList_Format,"COL=0 BAR, COL=1 WEIGHT=50 BAR, COL=2 BAR, COL=3",
            MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
    End;

    LV_events = (Object*) NListviewObject,
        MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always,
        MUIA_NListview_NList, LV_events,
    End;


    static const char *CYCLE_events_entries[] =
    {
       "Never", "Window is Active", "Window is Inactive", "Always", NULL
    };


    APTR GROUP_ROOT_2_EVENTS = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        /*Child, (Object*)GroupObject,
            MUIA_Group_Columns, 2,
            Child, (Object*)SimpleButton(GetCatalogStr(catalog,800,"Add")),
            Child, (Object*)SimpleButton(GetCatalogStr(catalog,800,"Remove")),
        End, */
        Child, GroupObject,
            MUIA_Group_Columns,2,
            MUIA_Group_Horiz,TRUE,
            Child, (Object*)Label(GetCatalogStr(catalog,800,"\033lCodes:\n%b = text being displayed in the window, %t = timestamp,\n%g = network name, %s = servername, %c = channel, %n = own nick, %m = nick")),
            Child, HSpace(0),
        End,
        Child, LV_events,
        Child, (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 3,
            Child, Label("Use when:"),
            Child, CYCLE_events=(Object*)CycleObject,
             MUIA_Cycle_Entries, CYCLE_events_entries,
                MUIA_ShortHelp, GetCatalogStr(catalog,800,"Choose when you want to execute this ARexx script"),
            End,
            Child, HSpace(0),
        End,
        Child, (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 4,
            Child, Label(GetCatalogStr(catalog,800,"ARexx Script")),
            Child, STR_events_script= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,

            Child, Label(GetCatalogStr(catalog,800,"Text for script")),
            Child, STR_events_args= (Object*)StringObject,MUIA_Weight, 70, MUIA_Frame, MUIV_Frame_String,MUIA_String_MaxLen, 800,End,
        End,
    End;

        /*
    APTR GROUP_ROOT_2_EVENTS = (Object*)GroupObject,
        MUIA_Group_Columns, 1,
        Child, GroupObject,
            MUIA_Group_Columns,2,
            MUIA_Group_Horiz,TRUE,
            Child, (Object*)Label(GetCatalogStr(catalog,800,"\033lCodes:\n%b = text being displayed in the window, %t = timestamp,\n%g = network name, %s = servername, %c = channel, %n = own nick, %m nick")),
            Child, HSpace(0),
        End,
        Child, GroupObject,
            //MUIA_Group_Columns,5,
            //MUIA_Group_Horiz,TRUE,
            MUIA_Group_Horiz,FALSE,
            MUIA_Group_Rows,10,
            MUIA_Weight, 100,
            //Child, Label("\033l\033b\033uEnabled"),
            //Child, Label("\033l\033b\033uUse when Window is Inactive"),
            Child, Label("\033l\033b\033uUse"),
            Child, Label("\033b\033uEvent"),
            Child, Label("\033l\033b\033uARexx script to execute"),
            Child, Label("\033l\033b\033uScript arguments"),
            Child, CH_events_highlight=CheckMark(TRUE),
            Child, CH_events_highlight2=CheckMark(TRUE),
            Child, (Object*)Label(GetCatalogStr(catalog,800,"Highlight")),
            Child, STR_events_highlight= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,
            Child, STR_events_highlight2=(Object*)BetterStringObject,
                    MUIA_Weight, 60,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
            End,
            Child, CH_events_connect=CheckMark(TRUE),
            Child, CH_events_connect2=CheckMark(TRUE),
            Child, (Object*)Label(GetCatalogStr(catalog,800,"Connection")),
            Child, STR_events_connect= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,
            Child, STR_events_connect2= (Object*)BetterStringObject,
                    MUIA_Weight, 60,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
            End,
            Child, CH_events_disconnect=CheckMark(TRUE),
            Child, CH_events_disconnect2=CheckMark(TRUE),
            Child, (Object*)Label(GetCatalogStr(catalog,800,"Disconnection")),
            Child, STR_events_disconnect= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,
            Child, STR_events_disconnect2=(Object*)BetterStringObject,
                    MUIA_Weight, 60,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
            End,
            Child, CH_events_join=CheckMark(TRUE),
            Child, CH_events_join2=CheckMark(TRUE),
            Child, Label(GetCatalogStr(catalog,800,"Joins")),
            Child, STR_events_join= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,
            Child, STR_events_join2= (Object*)BetterStringObject,
                    MUIA_Weight, 60,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
            End,
            Child, CH_events_part=CheckMark(TRUE),
            Child, CH_events_part2=CheckMark(TRUE),
            Child, Label(GetCatalogStr(catalog,800,"Parts")),
            Child, STR_events_part= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,
            Child, STR_events_part2= (Object*)BetterStringObject,
                    MUIA_Weight, 60,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
            End,
            Child, CH_events_quit=CheckMark(TRUE),
            Child, CH_events_quit2=CheckMark(TRUE),
            Child, Label(GetCatalogStr(catalog,800,"Quits")),
            Child, STR_events_quit= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,
            Child, STR_events_quit2= (Object*)BetterStringObject,
                    MUIA_Weight, 60,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
            End,
            Child, CH_events_dcc_recv_offered=CheckMark(TRUE),
            Child, CH_events_dcc_recv_offered2=CheckMark(TRUE),
            Child, Label(GetCatalogStr(catalog,800,"DCC RECV received")),
            Child, STR_events_dcc_recv_offered= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,
            Child, STR_events_dcc_recv_offered2= (Object*)BetterStringObject,
                    MUIA_Weight, 60,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
            End,
            Child, CH_events_dcc_recv_finished=CheckMark(TRUE),
            Child, CH_events_dcc_recv_finished2=CheckMark(TRUE),
            Child, Label(GetCatalogStr(catalog,800,"DCC RECV finished")),
            Child, STR_events_dcc_recv_finished= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,
            Child, STR_events_dcc_recv_finished2= (Object*)BetterStringObject,
                    MUIA_Weight, 60,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
            End,
            Child, CH_events_dcc_send_finished=CheckMark(TRUE),
            Child, CH_events_dcc_send_finished2=CheckMark(TRUE),
            Child, Label(GetCatalogStr(catalog,800,"DCC SEND finished")),
            Child, STR_events_dcc_send_finished= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GetCatalogStr(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,
            Child, STR_events_dcc_send_finished2= (Object*)BetterStringObject,
                    MUIA_Weight, 60,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
            End,
        End,
     End;
        */

    MBObj->SETTINGS_Register_Grp = (Object*)RegisterGroup(titles2),
        Child, GROUP_ROOT_2,
        Child, GROUP_ROOT_2_GUI,
        Child, GROUP_ROOT_2_SERVER,
        Child, GROUP_ROOT_2_SECOND,
        Child, GROUP_ROOT_2_DCC,
        Child, GROUP_ROOT_2_ACTIONS,
        Child, GROUP_ROOT_2_ALIASES,
        Child, MBObj->GR_user_list_buttons,
        Child, GROUP_ROOT_2_EVENTS,
    End;

if(AMIX) printf("105\n");
    MBObj->SETTINGS_ROOT = (Object*)GroupObject,
        Child, MBObj->SETTINGS_Register_Grp,
        Child, GR_grp_29,
    End;

    MBObj->WI_mainsettings = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,101,"Settings"),
        MUIA_Window_ID, MAKE_ID('4', 'W', 'I', 'N'),
        MUIA_Window_NoMenus, FALSE,
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_SizeGadget, TRUE,
        WindowContents, MBObj->SETTINGS_ROOT,
    End;
if(AMIX) printf("106\n");



    //below is the colour settings window objects
    LA_CSW_background = (Object*)Label(GetCatalogStr(catalog,335,"background"));
    MBObj->PP_CSW_background = (Object*)MUI_NewObject((char*)MUIC_Popimage,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_nicklistbackground = (Object*)Label(GetCatalogStr(catalog,336,"nicklistbackground"));
    MBObj->PP_CSW_nicklistbackground = (Object*)MUI_NewObject((char*)MUIC_Popimage,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_listviewtabs_background= (Object*)Label(GetCatalogStr(catalog,357,"tabs background"));
    MBObj->PP_CSW_listviewtabs_background = (Object*)MUI_NewObject((char*)MUIC_Popimage,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;


    LA_CSW_join = (Object*)Label(GetCatalogStr(catalog,337,"join"));
    MBObj->PP_CSW_join = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_part = (Object*)Label(GetCatalogStr(catalog,338,"part"));
    MBObj->PP_CSW_part = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_quit = (Object*)Label(GetCatalogStr(catalog,339,"quit"));
    MBObj->PP_CSW_quit = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_modes = (Object*)Label(GetCatalogStr(catalog,340,"modes"));
    MBObj->PP_CSW_modes = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_ctcp = (Object*)Label(GetCatalogStr(catalog,341,"ctcp"));
    MBObj->PP_CSW_ctcp = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_actions = (Object*)Label(GetCatalogStr(catalog,342,"actions"));
    MBObj->PP_CSW_actions = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_normal = (Object*)Label(GetCatalogStr(catalog,343,"normal"));
    MBObj->PP_CSW_normal = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_info = (Object*)Label(GetCatalogStr(catalog,344,"info"));
    MBObj->PP_CSW_info = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_owntext = (Object*)Label(GetCatalogStr(catalog,345,"owntext"));
    MBObj->PP_CSW_owntext = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_highlight = (Object*)Label(GetCatalogStr(catalog,346,"highlight"));
    MBObj->PP_CSW_highlight = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_invite = (Object*)Label(GetCatalogStr(catalog,347,"invite"));
    MBObj->PP_CSW_invite = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_kick = (Object*)Label(GetCatalogStr(catalog,348,"kick"));
    MBObj->PP_CSW_kick = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_nickchange = (Object*)Label(GetCatalogStr(catalog,349,"nickchange"));
    MBObj->PP_CSW_nickchange = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_topic = (Object*)Label(GetCatalogStr(catalog,350,"topic"));
    MBObj->PP_CSW_topic = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_wallops = (Object*)Label(GetCatalogStr(catalog,351,"wallops"));
    MBObj->PP_CSW_wallops = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_notices = (Object*)Label(GetCatalogStr(catalog,352,"notices"));
    MBObj->PP_CSW_notices = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_activity = (Object*)Label(GetCatalogStr(catalog,353,"activity"));
    MBObj->PP_CSW_activity = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_chatactivity = (Object*)Label(GetCatalogStr(catalog,354,"chatactivity"));
    MBObj->PP_CSW_chatactivity = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_highlightactivity = (Object*)Label(GetCatalogStr(catalog,355,"highlightactivity"));
    MBObj->PP_CSW_highlightactivity = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_nicklisttext = (Object*)Label(GetCatalogStr(catalog,356,"nicklisttext"));
    MBObj->PP_CSW_nicklisttext = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

    LA_CSW_listviewtabs_normaltext = (Object*)Label(GetCatalogStr(catalog,358,"tabs pen"));
    MBObj->PP_CSW_listviewtabs_normaltext = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GetCatalogStr(catalog,360,"Choose Colour"),End;

if(AMIX) printf("107\n");
    MBObj->BT_colour_settings_load = (Object*)SimpleButton(GetCatalogStr(catalog,323,"Load"));
    MBObj->BT_colour_settings_apply = (Object*)SimpleButton(GetCatalogStr(catalog,324,"Apply"));
    MBObj->BT_colour_settings_save = (Object*)SimpleButton(GetCatalogStr(catalog,325,"Save"));
    MBObj->BT_colour_settings_save_as = (Object*)SimpleButton(GetCatalogStr(catalog,326,"Save As..."));


    APTR colour_settings_actions = (Object*)GroupObject,//VirtgroupObject,
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_colour_settings_load,
        Child, MBObj->BT_colour_settings_apply,
        Child, MBObj->BT_colour_settings_save,
        Child, MBObj->BT_colour_settings_save_as,
    End;

    APTR GROUP_ROOT_11 = (Object*)GroupObject,//VirtgroupObject,
        MUIA_Group_Columns, 6,
        Child,    LA_CSW_background,
        Child,    MBObj->PP_CSW_background,
        Child,    LA_CSW_nicklistbackground,
        Child,    MBObj->PP_CSW_nicklistbackground,
        Child,    LA_CSW_listviewtabs_background,
        Child,    MBObj->PP_CSW_listviewtabs_background,
        Child,    LA_CSW_normal,
        Child,    MBObj->PP_CSW_normal,
        Child,    LA_CSW_nicklisttext,
        Child,    MBObj->PP_CSW_nicklisttext,
        Child,    LA_CSW_listviewtabs_normaltext,
        Child,    MBObj->PP_CSW_listviewtabs_normaltext,
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

    GROUP_ROOT_6 = (Object*)GroupObject,//VirtgroupObject,
        MUIA_Group_Columns, 1,
        Child, GROUP_ROOT_11,
        Child, colour_settings_actions,
    End;
if(AMIX) printf("108\n");
    MBObj->WI_colour_settings = (Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,102,"Colour Settings"),
        MUIA_Window_ID, MAKE_ID('3', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_6,
    End;
if(AMIX) printf("109\n");
    //below are menu items
    MN1_SelectServer = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,103,"Select server"),
        MUIA_Menuitem_Shortcut, "S",
    End;

    MN1_CloseTAB = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,104,"Close current tab"),
        MUIA_Menuitem_Shortcut, "Z",
    End;

    MNmenuBarLabel2 = (Object*)MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

    MN1_Hide = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,293,"Hide"),
        MUIA_Menuitem_Shortcut, "H",
    End;

    MN1_NewTAB = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,105,"New server tab"),
        MUIA_Menuitem_Shortcut, "T",
    End;

    MN1_NewGTAB = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,106,"Open global query tab"),
    End;

    MNmenuBarLabel0 = (Object*)MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

    MNmenuBarLabel1 = (Object*)MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);


    MN_about = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,26,"About"),
    End;

    MN_quit = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,107,"Quit"),
        MUIA_Menuitem_Shortcut, "Q",
    End;

    MN_ClearHistory = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,108,"Clear buffer"),
    End;

    MN_ClearAllHistory = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,334,"Clear all buffers"),
    End;

    MN_SaveHistory = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,109,"Save buffer as"),
    End;

    MN1_Main = (Object*)MenuObject,
        MUIA_Menu_Title, GetCatalogStr(catalog,110,"Main"),
        MUIA_Family_Child, MN1_SelectServer,
        MUIA_Family_Child, MN1_NewTAB,
        MUIA_Family_Child, MN1_NewGTAB,
        MUIA_Family_Child, MN1_CloseTAB,
        MUIA_Family_Child, MNmenuBarLabel0,
        MUIA_Family_Child, MN1_Hide,
        MUIA_Family_Child, MNmenuBarLabel2,
        MUIA_Family_Child, MN_ClearHistory,
        MUIA_Family_Child, MN_ClearAllHistory,
        MUIA_Family_Child, MN_SaveHistory,
        MUIA_Family_Child, MNmenuBarLabel1,
        MUIA_Family_Child, MN_about,
        MUIA_Family_Child, MN_quit,
    End;
if(AMIX) printf("110\n");
    MN_MainSettings  = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,111,"Main Settings..."),
        //MUIA_Menuitem_Shortcut, "S",
    End;

    MN_ColourSettings = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,102,"Colour Settings..."),
    End;

    MN_MUISettings = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,112,"MUI Settings..."),
        //MUIA_Menuitem_Shortcut, "S",
    End;

    MN_MultiColumnDisplay  = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,317,"Multi-column chat display"),
        MUIA_Menuitem_Checkit, TRUE,
        MUIA_Menuitem_Toggle, TRUE,
        //MUIA_Menuitem_Shortcut, "S",
    End;

    MN_Clipboard = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,318,"Column-marking Clipboard"),
        MUIA_Menuitem_Checkit, TRUE,
        MUIA_Menuitem_Toggle, TRUE,
    End;


    MN_SaveSettings = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,288,"Save settings"),
    End;


    MN_MuteSound = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,372,"Mute all sound"),
        MUIA_Menuitem_Checkit, TRUE,
        MUIA_Menuitem_Toggle, TRUE,
    End;

    MN_Edit = (Object*)MenuObject,
        MUIA_Menu_Title, GetCatalogStr(catalog,800,"Edit"),
        MUIA_Family_Child, MN_cut  = (Object*)MenuitemObject,MUIA_Menuitem_Title, GetCatalogStr(catalog,800,"Cut"),MUIA_Menuitem_Shortcut, "X", End,
        MUIA_Family_Child, MN_copy = (Object*)MenuitemObject,MUIA_Menuitem_Title, GetCatalogStr(catalog,800,"Copy"),MUIA_Menuitem_Shortcut, "C", End,
        MUIA_Family_Child, MN_paste = (Object*)MenuitemObject,MUIA_Menuitem_Title, GetCatalogStr(catalog,800,"Paste"),MUIA_Menuitem_Shortcut, "V", End,
    End;

    MN_Settings = (Object*)MenuObject,
        MUIA_Menu_Title, GetCatalogStr(catalog,101,"Settings"),
        MUIA_Family_Child, MN_MainSettings,
        MUIA_Family_Child, MN_ColourSettings,
        MUIA_Family_Child, MN_MUISettings,
        MUIA_Family_Child, MN_MultiColumnDisplay,
        MUIA_Family_Child, MN_Clipboard,
        MUIA_Family_Child, MN_MuteSound,
        MUIA_Family_Child, MN_SaveSettings,
    End;

    MN_windows_dcc = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,62,"Incoming DCC File Transfers..."),
        MUIA_Menuitem_Shortcut, "D",
    End;

    MN_windows_dcc2 = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,63,"Outgoing DCC File Transfers..."),
        MUIA_Menuitem_Shortcut, "R",
    End;

    MN_urlgrabber = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,114,"URL Grabber..."),
        MUIA_Menuitem_Shortcut, "U",
    End;

    MN_ignorelist = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GetCatalogStr(catalog,276,"Ignore List..."),
        MUIA_Menuitem_Shortcut, "I",
    End;

    MN_windows = (Object*)MenuObject,
        MUIA_Menu_Title, GetCatalogStr(catalog,214,"Windows"),
        MUIA_Family_Child, MN_windows_dcc,
        MUIA_Family_Child, MN_windows_dcc2,
        MUIA_Family_Child, MN_urlgrabber,
        MUIA_Family_Child, MN_ignorelist,
    End;

    MBObj->MN_ = (Object*)MenustripObject,
        MUIA_Family_Child, MN1_Main,
        MUIA_Family_Child, MN_Edit,
        MUIA_Family_Child, MN_Settings,
        MUIA_Family_Child, MN_windows,
        //MUIA_Family_Child, MN1_AREXX,
    End;

    //below are edit window stuff

    LA_servername = (Object*)Label(GetCatalogStr(catalog,115,"Servername"));

    MBObj->STR_servername = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_servername",
        MUIA_String_Reject," ",
    End;

    GR_grp_13 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_13",
        MUIA_Group_Horiz, TRUE,
        Child, LA_servername,
        Child, MBObj->STR_servername,
    End;

    LA_port = (Object*)Label(GetCatalogStr(catalog,116,"Port number"));
if(AMIX) printf("111\n");
    MBObj->STR_port = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
        MUIA_HelpNode, "STR_port",
    End;

    GR_grp_11 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_11",
        MUIA_Group_Horiz, TRUE,
        Child, LA_port,
        Child, MBObj->STR_port,
    End;

    MBObj->LA_autojoin = (Object*)Label(GetCatalogStr(catalog,362,"Automatically join these channels"));

    MBObj->STR_autojoin = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
        MUIA_String_MaxLen, 400,
    End;

    MBObj->LA_nick_registration_command = (Object*)Label(GetCatalogStr(catalog,363,"Nick registration password"));

    MBObj->STR_nick_registration_command = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GetCatalogStr(catalog,364,"This option will only work with a registered nick on a network with a Nickserv service"),
        MUIA_String_Secret, TRUE,
        MUIA_String_Reject," ",
    End;

    APTR GR_grp_autojoin = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_17",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 2,
        Child,MBObj->LA_autojoin,
        Child,MBObj->STR_autojoin,
    End;

    APTR GR_grp_nickregistration = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_17",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 2,
        Child,MBObj->LA_nick_registration_command,
        Child,MBObj->STR_nick_registration_command,
    End;

    LA_pass = (Object*)Label(GetCatalogStr(catalog,117,"Server password"));

    MBObj->STR_pass = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_pass",
        MUIA_String_Secret, TRUE,
        MUIA_String_Reject," ",
    End;

    GR_grp_17 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_17",
        MUIA_Group_Horiz, TRUE,
        Child, LA_pass,
        Child, MBObj->STR_pass,
    End;

    MBObj->BT_acceptserver = (Object*)SimpleButton(GetCatalogStr(catalog,365,"Accept Changes"));

    MBObj->BT_cancelserver = (Object*)SimpleButton(GetCatalogStr(catalog,74,"Cancel"));

    GR_grp_12 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_12",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_acceptserver,
        Child, MBObj->BT_cancelserver,
    End;

    MBObj->CH_autoconnect = (Object*)CheckMark(FALSE);
    MBObj->LA_autoconnect = (Object*)Label(GetCatalogStr(catalog,277,"Automatically connect here when WookieChat is started"));

    APTR LA_space7 =(Object*)TextObject,
        MUIA_Weight, 1,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL,
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;

    APTR LA_space8 =(Object*)TextObject,
        MUIA_Weight, 1,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL,
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;

    GR_autoconnect = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_autoconnect,
        Child, MBObj->LA_autoconnect,
        Child, LA_space7,
        Child, HSpace(0),
        Child, MBObj->CH_use_global = CheckMark(FALSE),
        Child, Label("Use Global user settings"),
        Child, HSpace(0),
    End;

    MBObj->LA_remote_charset=(Object*)Label(GetCatalogStr(catalog,327,"Server Charset"));

    MBObj->CYCLE_remote_charset = CycleObject,
       MUIA_Cycle_Entries, remote_charsets,
    End;

    APTR LA_space6 =(Object*)TextObject,
            MUIA_Weight, 100,
            MUIA_Text_PreParse, NULL,
            MUIA_Text_Contents, NULL,
            MUIA_InnerLeft, 0,
            MUIA_InnerRight, 0,
        End;

    MBObj->GR_charsets = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child,MBObj->LA_remote_charset,
        Child,MBObj->CYCLE_remote_charset,
        Child, LA_space6,
    End;
if(AMIX) printf("112\n");
    APTR GROUP_ROOT_4_COLUMN_1 = (Object*)GroupObject,
        MUIA_Weight, 45,
        Child, GR_grp_13,
        Child, GR_grp_11,
        Child, GR_grp_17,
    End;

    APTR GROUP_ROOT_4_COLUMN_2 = (Object*)GroupObject,
        MUIA_Weight, 55,
        Child, GR_grp_autojoin,
        Child, GR_grp_nickregistration,
        Child, MBObj->GR_charsets,
        //Child, GR_grp_12,
    End;
if(AMIX) printf("113\n");
    GROUP_ROOT_4 = (Object*)GroupObject,
        MUIA_Group_Horiz,FALSE,
        MUIA_Group_Columns,2,
        Child, GROUP_ROOT_4_COLUMN_1,
        Child, GROUP_ROOT_4_COLUMN_2,
    End;

if(AMIX) printf("114\n");
    LA_groupname = (Object*)Label(GetCatalogStr(catalog,119,"Group name"));

    MBObj->BT_perform = (Object*)SimpleButton(GetCatalogStr(catalog,120,"Perform on connect"));

    MBObj->STR_groupname = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_groupname",
    End;

    GR_grp_15 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_15",
        MUIA_Group_Horiz, TRUE,
        Child, LA_groupname,
        Child, MBObj->STR_groupname,
    End;

    MBObj->BT_acceptgroup =(Object*)SimpleButton(GetCatalogStr(catalog,40,"Accept"));

    MBObj->BT_cancelgroup =(Object*)SimpleButton(GetCatalogStr(catalog,74,"Cancel"));

    GR_grp_16 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_16",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_acceptgroup,
        Child, MBObj->BT_cancelgroup,
    End;

    GROUP_ROOT_5 =(Object*)GroupObject,
        Child, GR_grp_15,
        Child, GR_grp_16,
    End;
if(AMIX) printf("115\n");
    MBObj->WI_edit_group =(Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,121,"Edit Group"),
        MUIA_Window_ID, MAKE_ID('3', 'W', 'I', 'N'),
        MUIA_Window_CloseGadget, FALSE,
        WindowContents, GROUP_ROOT_5,
    End;

// above is edit window stuff

    LA_nicknames =(Object*)Label(GetCatalogStr(catalog,216,"Nick Names"));

    MBObj->STR_nickname1 =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_nickname1",
        MUIA_String_Reject,"\\/#.,?*@!",
    End;

    MBObj->STR_nickname2 =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_nickname2",
        MUIA_String_Reject,"\\/#.,?*@!",
    End;

    MBObj->STR_nickname3 =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_nickname3",
        MUIA_String_Reject,"\\/#.,?*@!",
    End;
if(AMIX) printf("116\n");
    GR_grp_0 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_0",
        MUIA_Group_Horiz, TRUE,
        Child, LA_nicknames,
        Child, MBObj->STR_nickname1,
        Child, MBObj->STR_nickname2,
        Child, MBObj->STR_nickname3,
    End;

    LA_realname =(Object*)Label(GetCatalogStr(catalog,122,"Real Name"));

    MBObj->STR_realname =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_realname",
    End;

    


    MBObj->LA_display_server_win=(Object*)Label(GetCatalogStr(catalog,278,"Display Server Window upon startup:"));
    MBObj->CH_display_server_win=(Object*)CheckMark(FALSE);

    DISPLAY_SERVER_WIN_GRP=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        //MUIA_Weight,400,
        Child, MBObj->LA_display_server_win,
        Child, MBObj->CH_display_server_win,
    End;

    APTR LA_ =(Object*)TextObject,
            MUIA_Weight, 1,
            MUIA_Text_PreParse, NULL,
            MUIA_Text_Contents, NULL,
            MUIA_InnerLeft, 0,
            MUIA_InnerRight, 0,
        End;
if(AMIX) printf("117\n");
    GR_grp_1 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_1",
        MUIA_Group_Horiz, TRUE,
        Child, DISPLAY_SERVER_WIN_GRP,
        Child, LA_,
    End;

        //Child, MBObj->LA_local_charset,
        //Child, MBObj->CYCLE_local_charset,

    APTR LA_space5 =(Object*)TextObject,
            MUIA_Weight, 100,
            MUIA_Text_PreParse, NULL,
            MUIA_Text_Contents, NULL,
            MUIA_InnerLeft, 0,
            MUIA_InnerRight, 0,
        End;

    GR_local_charset =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_1",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 3,
        Child, MBObj->LA_local_charset,
        Child, MBObj->CYCLE_local_charset,
        Child, LA_space5,
    End;
if(AMIX) printf("118\n");

    LA_username =(Object*)Label(GetCatalogStr(catalog,123,"User Name"));

    MBObj->STR_username =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_username",
    End;


    GR_grp_2 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_2",
        MUIA_Group_Horiz, TRUE,
        Child, LA_username,
        Child, MBObj->STR_username,
        Child, LA_realname,
        Child, MBObj->STR_realname,
        //Child, DISPLAY_SERVER_WIN_GRP,
    End;

    REC_label_0 =(Object*) RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    GR_grp_3 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_3",
        MUIA_Group_Horiz, TRUE,
        Child, REC_label_0,
    End;



    MBObj->NLT_Servers =(Object*) NListtreeObject,
        MUIA_NListtree_ConstructHook, MUIV_NListtree_ConstructHook_String,
        MUIA_NListtree_DestructHook, MUIV_NListtree_DestructHook_String,
        MUIA_NListtree_DisplayHook, &Display_servers_TextHook,
        MUIA_NListtree_Format,"COL=0 WEIGHT=45 BAR, COL=1 WEIGHT=10 BAR, COL=2 WEIGHT=15 BAR, COL=3 WEIGHT=15 BAR",
        MUIA_NListtree_Title, TRUE,
    End;

    if(Pro_Charsets_Enabled==FALSE)
        setmacro((Object*)MBObj->NLT_Servers,MUIA_NListtree_Format,"COL=0 WEIGHT=45 BAR, COL=1 WEIGHT=10 BAR, COL=2 WEIGHT=15 BAR");

    MBObj->NLV_Servers=(Object*)NListviewObject,
        MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Always,
        MUIA_NListview_NList, MBObj->NLT_Servers,
    End;
if(AMIX) printf("119\n");
    if(!MBObj->NLT_Servers) cleanexit((char*)GetCatalogStr(catalog,19,"Please download and install the NListtree and NList MUI Classes from aminet\n"));
if(AMIX) printf("120\n");
    GR_grp_8 =(Object*)GroupObject, //VirtgroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->NLV_Servers,
        MUIA_HelpNode, "GR_grp_8",
    End;

    REC_label_0CC =(Object*) RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    GR_grp_7 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_7",
        Child, REC_label_0CC,
    End;

    MBObj->BT_addserver =(Object*)SimpleButton(GetCatalogStr(catalog,124,"New Server"));

    MBObj->BT_addgroup =(Object*)SimpleButton(GetCatalogStr(catalog,125,"New Group"));

    MBObj->BT_delete_server =(Object*)SimpleButton(GetCatalogStr(catalog,127,"Delete Entry"));

    MBObj->BT_edit =(Object*)SimpleButton(GetCatalogStr(catalog,126,"Edit Entry"));

    GR_grp_9 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_9",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_addserver,
        Child, MBObj->BT_addgroup,
        Child, MBObj->BT_delete_server,
        //Child, MBObj->BT_edit,
        //Child, MBObj->BT_acceptserver,
    End;

    MBObj->BT_connect =(Object*)SimpleButton(GetCatalogStr(catalog,128,"Connect to server"));

    MBObj->BT_connect_tab =(Object*)SimpleButton(GetCatalogStr(catalog,129,"Connect in new tab"));

    GR_grp_6 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_6",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_connect,
        Child, MBObj->BT_connect_tab,
        Child, MBObj->BT_perform,
    End;

    APTR REC_separator =(Object*) RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;
if(AMIX) printf("123\n");
    
    GROUP_ROOT_3 =(Object*)GroupObject,
        MUIA_Group_HorizSpacing, 2,
        MUIA_Group_VertSpacing, 2,
        Child, GroupObject,
            MUIA_FrameTitle, "Global user settings",
            MUIA_Frame, MUIV_Frame_Group,
            Child, GR_grp_0,
            Child, GR_grp_2,
            Child, GR_grp_1,
            Child, GR_local_charset,
        End,
        //Child, GR_grp_3,
        Child, GR_grp_8,
        Child, GR_grp_7,
        Child, GR_grp_9,
        Child, GR_grp_6,
        //Child, REC_separator,
        Child, GroupObject,
            MUIA_FrameTitle, "Server settings",
            MUIA_Frame, MUIV_Frame_Group,
            Child, GR_autoconnect,
            Child, MBObj->GR_server_specific_user_prefs = (Object*)GroupObject,
                Child, GroupObject,
                    MUIA_Group_Horiz, TRUE,
                    Child, (Object*)Label(GetCatalogStr(catalog,216,"Nick Names")),
                    Child, MBObj->STR_server_nickname1 =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_HelpNode, "STR_nickname1",
                        MUIA_String_Reject,"\\/#.,?*@!",
                    End,
                    Child, MBObj->STR_server_nickname2 =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_HelpNode, "STR_nickname2",
                        MUIA_String_Reject,"\\/#.,?*@!",
                    End,

                    Child, MBObj->STR_server_nickname3 =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_HelpNode, "STR_nickname3",
                        MUIA_String_Reject,"\\/#.,?*@!",
                    End,
                End,
                Child, GroupObject,
                    MUIA_Group_Horiz, TRUE,
                    Child, (Object*)Label(GetCatalogStr(catalog,123,"User Name")),
                    Child, MBObj->STR_server_username =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_String_Reject," ",
                        MUIA_HelpNode, "STR_username",
                    End,
                    Child, (Object*)Label(GetCatalogStr(catalog,122,"Real Name")),
                    Child, MBObj->STR_server_realname =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_String_Reject," ",
                        MUIA_HelpNode, "STR_realname",
                    End,
                End,
            End,
            Child, (Object*) RectangleObject, MUIA_Rectangle_HBar, TRUE, MUIA_FixHeight, 8, End,
            Child, GROUP_ROOT_4,
        End,
    End;

if(AMIX) printf("124\n");
    MBObj->WI_label_2 =(Object*)WindowObject,
        MUIA_Window_Title, GetCatalogStr(catalog,130,"Select a server"),
        MUIA_Window_ID, MAKE_ID('1', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_3,
    End;
if(AMIX) printf("125\n");
    GR_top =(Object*)GroupObject,
        MUIA_Group_PageMode,TRUE,
        MUIA_Group_Horiz, TRUE,
        MUIA_HelpNode, "GR_top",
        MUIA_Group_HorizSpacing, 1,
        MUIA_Group_VertSpacing, 1,
        MUIA_Weight,100,
        //Child, MBObj->LV_tabs,
    End;

    if(string_default)
    {
        MBObj->STR_usertext =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_usertext",
        MUIA_String_MaxLen, BUFFERSIZE,
        End;
    }
    else
    {

        MBObj->STR_usertext =(Object*)BS_Object,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_BetterString_StayActive, TRUE,
        MUIA_HelpNode, "STR_usertext",
        MUIA_String_MaxLen, BUFFERSIZE,
        End;

    }

    GR_string_gadget =(Object*)GroupObject,
        MUIA_HelpNode, "GR_string_gadget",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_HorizSpacing,1,
        //Child, MBObj->TX_nickname = TextObject,MUIA_Text_SetMin, TRUE,MUIA_Text_SetMax, TRUE,/*MUIA_Frame,MUIV_Frame_Text,ButtonFrame,MUIA_Background, MUII_ButtonBack,*/MUIA_Text_Contents,"                     ",MUIA_Weight,0,End,
        Child, MBObj->TX_nickname = TextObject,MUIA_Text_SetMin, TRUE,MUIA_Text_SetMax, TRUE,ButtonFrame,MUIA_Background, MUII_ButtonBack,MUIA_Text_Contents,"                     ",MUIA_Weight,0,
        MUIA_InputMode, MUIV_InputMode_RelVerify,
        End,
        Child, MBObj->STR_usertext,
    End;

    GR_buttons =(Object*) VirtgroupObject,//GroupObject,
        MUIA_HelpNode, "GR_buttons",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_HorizSpacing, 1,
        MUIA_Group_VertSpacing, 1,
        MUIA_FixHeight, 200,
        //MUIA_Group_Rows,2,
    End;
if(AMIX) printf("126\n");

    LA_space =(Object*)TextObject,
        MUIA_Weight, 1,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL,
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;

    GR_bottom_group =(Object*)GroupObject,
        MUIA_HelpNode, "GR_bottom_group",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_VertSpacing, 1,
        MUIA_Group_HorizSpacing, 1,
        MUIA_Weight,100,
        Child, GR_buttons,
        Child, LA_space,
    End;

if(AMIX) printf("127\n");


    if(!my_settings.use_nlistobject_for_tabs)
    {
        if(AMIX) printf("127a\n");
        #ifdef __AROS__
        GROUP_ROOT_0 =(Object*)NewObjectAROS(mcc6->mcc_Class,NULL,  //(Object*)GroupObject,
        #else
        GROUP_ROOT_0 =(Object*)NewObject(mcc6->mcc_Class,NULL,  //(Object*)GroupObject,
            #endif
            MUIA_Group_HorizSpacing, 1,
            MUIA_Group_VertSpacing, 1,
            //MUIA_Group_VertSpacing, 2,
            Child, GR_top,
            Child, GR_string_gadget,
            Child, GR_bottom_group,
    End;
    }
    else
    {
        if(AMIX) printf("127b\n");
        #ifdef __AROS__
        GROUP_ROOT_0 =(Object*)NewObjectAROS(mcc6->mcc_Class,NULL, //(Object*)GroupObject,
        #else
        GROUP_ROOT_0 =(Object*)NewObject(mcc6->mcc_Class,NULL, //(Object*)GroupObject,
            #endif
            MUIA_Group_HorizSpacing, 1,
            MUIA_Group_VertSpacing, 1,
            //MUIA_Group_VertSpacing, 2,
            Child, GR_top,
            Child, GR_string_gadget,
        End;
    }
if(AMIX) printf("128\n");
           
    #ifdef __AROS__
    MBObj->WI_main =(Object*)NewObjectAROS(mcc4->mcc_Class,NULL, // (Object*)WindowObject,
    #else
    MBObj->WI_main =(Object*)NewObject(mcc4->mcc_Class,NULL, // (Object*)WindowObject,
        #endif
        MUIA_Window_Title, "Wookiechat",
        MUIA_Window_Menustrip, MBObj->MN_,
        MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_0,
    End;

      if(!MBObj->WI_main) printf("Error creating window object\n");

if(AMIX) printf("129\n");
    #ifdef __AROS__
    MBObj->LV_ban = (Object*)NewObjectAROS(mcc2->mcc_Class,NULL,// NListObject,
    #else
    MBObj->LV_ban = (Object*)NewObject(mcc2->mcc_Class,NULL,// NListObject,
    #endif
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_DisplayHook, &DisplayLI_ban_TextHook,
        MUIA_NList_Format, "COL=0 WEIGHT=-1 BAR, COL=1 WEIGHT =-1 BAR, COL=2 BAR",
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
        MUIA_NList_Input, TRUE,
        MUIA_NList_Title, TRUE,
    End;
if(AMIX) printf("130\n");
    MBObj->LV_ban = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 100,
        MUIA_NListview_NList, MBObj->LV_ban,
    End;

    MBObj->BT_unban = (Object*)SimpleButton(GetCatalogStr(catalog,131,"Unban"));
    MBObj->BT_refreshban = (Object*)SimpleButton(GetCatalogStr(catalog,132,"Refresh List"));

    GR_ban_subgroup = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_unban,
        Child, MBObj->BT_refreshban,
    End;

    GR_ban = (Object*)GroupObject,
        Child, MBObj->LV_ban,
        Child, GR_ban_subgroup,
    End;
if(AMIX) printf("131\n");
    MBObj->WI_ban = (Object*)WindowObject,
        MUIA_Window_Title, "Banlist",
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', '1', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, GR_ban,
    End;

#ifndef MUIA_Application_UsedClasses
#define MUIA_Application_UsedClasses 0x8042e9a7
#endif

    static STRPTR classlist[] = { (char*)"NList.mcc", (char*)"NListview.mcc", (char*)"NListtree.mcc", (char*)"BetterString.mcc", NULL };

//if(AMIX) printf("132\n");
    MBObj->App =(Object*) ApplicationObject,
        MUIA_Application_UsedClasses, classlist,
        MUIA_Application_Author, (char*)"James Carroll",
        MUIA_Application_Base, (char*)"WOOKIECHAT",
        MUIA_Application_Title, (char*)"WookieChat",
        MUIA_Application_Version, (char*)VERSION_MUI,
        MUIA_Application_Copyright, (char*)"James Carroll",
        MUIA_Application_Description, (char*)"IRC Client",
        MUIA_Application_DiskObject, dobj,
        SubWindow, MBObj->WI_main,
        SubWindow, MBObj->WI_label_2,
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
        SubWindow, MBObj->WI_ban,
        SubWindow, MBObj->WI_ignore,
        SubWindow, MBObj->WI_addignore,
        //SubWindow, MBObj->WI_edit_alias,
        SubWindow, MBObj->WI_graphical_smileys_preview,
        SubWindow, MBObj->WI_graphical_smileys_choose,
        SubWindow, MBObj->WI_change_nick,
    End;


    MBObj->smiley_choose_icon=NULL;
    //if(AMIX) printf("133\n");
    if (!MBObj->App)
    {
        //if(AMIX) printf("134.. THE END..\n");
        FreeVec(MBObj);
        printf("%s",GetCatalogStr(catalog,133,"unable to create app\n"));
        return(NULL);
    }

//if(AMIX) printf(" THE END.. \n");
    return(MBObj);
} 


