/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <libraries/gadtools.h>
#include <mui/NListview_mcc.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "intern.h"
#include "version_info.h"

int disable_getline_hook(void);

char string_to_send2[800];

struct NewMenu AREXX_Menu[MAX_AREXX_SCRIPTS];

APTR MN1_AREXX;
char maintask_basename[100];

struct AREXX_Menu AREXX_Menu_Items[MAX_AREXX_SCRIPTS];

/* Locals */
static struct MsgPort *arexx_process_port;
static struct MsgPort *arexx_quit_port;

BOOL SafePutToPort(struct XYMessage *message, STRPTR portname)
{
    struct MsgPort *port;
    Forbid();

    port = FindPort((c_in)portname);
    if (port) PutMsg(port, (struct Message*)message);
    Permit();
    return(port ? TRUE : FALSE);
}

int add_scripts_to_menu()
{

    int count=0;

    char work_buffer2[500];
    #ifdef __amigaos4__
    uint32 EAData[1024*10];
    #else
    int EAData[1024*10];
    #endif
    struct ExAllData *ead;
    BOOL more;

    struct ExAllControl *eac = (struct ExAllControl*) AllocDosObject(DOS_EXALLCONTROL,NULL);
    if (!eac)
    {
        return 0;
    }
    eac->eac_LastKey = 0;

    BPTR my_lock=Lock("progdir:arexx_scripts",ACCESS_READ);


    if(MN1_AREXX) DoMethod((Object*)WookieChat->MN_, OM_REMMEMBER, (Object*)MN1_AREXX);

    MN1_AREXX = (Object*)MenuObject,
        MUIA_Menu_Title, (char*)GetCatalogStr(catalog,331,"ARexx"),
    End;

    if(!my_lock)
    {
        if(USE_AREXX==FALSE) return 0;

        printf("Unable to obtain a lock on arexx_scripts/ directory\n");
        return 0;
    }



    strcpy(AREXX_Menu_Items[count].MenuItem_String,GetCatalogStr(catalog,330,"<Rescan scripts directory>"));
    strcpy(AREXX_Menu_Items[count].MenuItem_FullFilename,"");
    AREXX_Menu_Items[count].return_id=AREXX_MENU_VALUES+count;

    AREXX_Menu_Items[count].MN_MenuItem = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, (char*)AREXX_Menu_Items[count].MenuItem_String,
    End;

    DoMethod((Object *)AREXX_Menu_Items[count].MN_MenuItem, MUIM_Notify, MUIA_Menuitem_Trigger,MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,AREXX_Menu_Items[count].return_id);

    DoMethod((Object*)MN1_AREXX, OM_ADDMEMBER, (Object*)AREXX_Menu_Items[count].MN_MenuItem);
    

    count++;

    do
    {
       more = ExAll(my_lock, (struct ExAllData*)EAData, sizeof(EAData), ED_TYPE, (struct ExAllControl*) eac);
       if ((!more) && (IoErr() != ERROR_NO_MORE_ENTRIES))
       {
            //printf("failed abnormally with some other error\n");
            break;

       }
       if (eac->eac_Entries == 0)
       {
            //printf("failed normally with no entries\n");
            break;
       }
       for(ead = (struct ExAllData *)EAData; ead; ead = ead->ed_Next)
       {
            if(EAD_IS_FILE(ead))
              {
                //if(IDOS->DevNameFromLock(my_lock, work_buffer, 800, DN_FULLPATH))
                {
                    if(wookie_folder[strlen(wookie_folder)-1]==':' || wookie_folder[strlen(wookie_folder)-1]=='/')
                        sprintf(work_buffer,"%sarexx_scripts/",wookie_folder);
                    else
                        sprintf(work_buffer,"%s/arexx_scripts/",wookie_folder);

                    if(work_buffer[strlen(work_buffer)-1]==':')
                        sprintf(work_buffer2,"%s%s",work_buffer,ead->ed_Name);
                    else
                        sprintf(work_buffer2,"%s%s",work_buffer,ead->ed_Name);

                    strcpy(AREXX_Menu_Items[count].MenuItem_String,(char*)ead->ed_Name);
                    strcpy(AREXX_Menu_Items[count].MenuItem_FullFilename,work_buffer2);
                    AREXX_Menu_Items[count].return_id=AREXX_MENU_VALUES+count;

                    AREXX_Menu_Items[count].MN_MenuItem = (Object*)MenuitemObject,
                        MUIA_Menuitem_Title, (char*)AREXX_Menu_Items[count].MenuItem_String,
                    End;

                    DoMethod((Object *)AREXX_Menu_Items[count].MN_MenuItem, MUIM_Notify, MUIA_Menuitem_Trigger,MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,AREXX_Menu_Items[count].return_id);

                    DoMethod((Object*)MN1_AREXX, OM_ADDMEMBER, (Object*)AREXX_Menu_Items[count].MN_MenuItem);


                }
                if(count>MAX_AREXX_SCRIPTS-1) break;

                count++;


              }

        }

    } while(more);


    DoMethod((Object*)WookieChat->MN_, OM_ADDMEMBER, (Object*)MN1_AREXX);


    FreeDosObject(DOS_EXALLCONTROL,eac);
    if(my_lock) UnLock(my_lock);
    return 0;





}

int get_right_network(LONG **parameters)
{

    if(parameters[0])
    {
        if(!stricmp((char*)parameters[0],"CURRENT"))
        {
            status_conductor=status_current;
            status_conductor->conductor=status_conductor->current_query;
            return 1;
        }

        status_conductor=status_root;

        for(count=0; count<atoi((char*)parameters[0]) && status_conductor; count++)
            status_conductor=status_conductor->next;

        if(!status_conductor) { status_conductor=status_current;
        status_conductor->conductor=status_conductor->current_query;
        return 0; }
        //printf("network number:%s, stopped search at:%d\n",(char*)parameters[0],count);

    }
    else { status_conductor=status_current; status_conductor->conductor=status_conductor->current_query; }

    return 1;

}

// SAYALL

#ifdef __amigaos4__
int selhookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int selhookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int selhookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int selhookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif


    if(parameters[0])
    {

        sprintf(string_to_send2,"/sayall %s",(char*)parameters[0]);

        struct XYMessage *my_message2=(struct XYMessage*)AllocMem(sizeof(struct XYMessage),MEMF_PUBLIC|MEMF_CLEAR);

        my_message2->xy_Msg.mn_Node.ln_Type=NT_MESSAGE;
        my_message2->xy_Msg.mn_Length=sizeof(struct XYMessage);
        my_message2->xy_Msg.mn_ReplyPort = arexx_process_port;
        my_message2->xy_Getline = 0;
        my_message2->xy_QuitArexx = 0;
        my_message2->xy_Sendtext = 1;

          
        if(SafePutToPort(my_message2, maintask_basename))
        {
            WaitPort(arexx_process_port);
            GetMsg(arexx_process_port);
        }

    }

    status_conductor=status_current;
    status_conductor->conductor=status_conductor->root;

   //status_conductor->conductor=status_conductor->current_query;

    return 0;

}

#ifdef __MORPHOS__
struct Hook selhook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)selhookfunc, NULL};
#elif __AROS__
struct Hook selhook = { { NULL,NULL }, (IPTR(*)())selhookfunc, NULL,NULL };
#else
struct Hook selhook = { { NULL,NULL }, (ULONG(*)())selhookfunc, NULL,NULL };
#endif




// SAY

#ifdef __amigaos4__
int sel2hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel2hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel2hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel2hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(parameters[2])
    {
        if(parameters[1])
        {

            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
            {
                if(!stricmp(status_conductor->conductor->name,(char*)parameters[1])) break;
            }
            if(!status_conductor->conductor) status_conductor->conductor=status_conductor->current_query;
        }

        sprintf(string_to_send2,"/say %s",(char*)parameters[2]);

        struct XYMessage *my_message2=(struct XYMessage*)AllocMem(sizeof(struct XYMessage),MEMF_PUBLIC|MEMF_CLEAR);

        my_message2->xy_Msg.mn_Node.ln_Type=NT_MESSAGE;
        my_message2->xy_Msg.mn_Length=sizeof(struct XYMessage);
        my_message2->xy_Msg.mn_ReplyPort = arexx_process_port;
        my_message2->xy_Getline = 0;
        my_message2->xy_QuitArexx = 0;
        my_message2->xy_Sendtext = 1;

        //if(SafePutToPort(my_message2, "WookieChatMainTask"))
        if(SafePutToPort(my_message2, maintask_basename))
        {
            WaitPort(arexx_process_port);
            GetMsg(arexx_process_port);
        }

    }
    if(DEBUG) printf("finished doing say command\n");

    status_conductor=work_status;

    //status_conductor->conductor=status_conductor->current_query;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel2hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel2hookfunc, NULL};
#elif __AROS__
struct Hook sel2hook = { { NULL,NULL }, (IPTR(*)())sel2hookfunc, NULL,NULL };
#else
struct Hook sel2hook = { { NULL,NULL }, (ULONG(*)())sel2hookfunc, NULL,NULL };
#endif


// GETMYNICK

#ifdef __amigaos4__
int sel3hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel3hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel3hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel3hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(status_conductor)
        setmacro((Object*)app, MUIA_Application_RexxString, status_conductor->nick);

    status_conductor=work_status;

    return 0;


}

#ifdef __MORPHOS__
struct Hook sel3hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel3hookfunc, NULL};
#elif __AROS__
struct Hook sel3hook = { { NULL,NULL }, (IPTR(*)())sel3hookfunc, NULL,NULL };
#else
struct Hook sel3hook = { { NULL,NULL }, (ULONG(*)())sel3hookfunc, NULL,NULL };
#endif


// ECHO

#ifdef __amigaos4__
int sel4hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel4hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel4hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel4hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif


    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(parameters[2])
    {

        if(parameters[1]) sprintf(string_to_send2,"/echo %s %s",(char*)parameters[1],(char*)parameters[2]);
        else sprintf(string_to_send2,"/echo 9 %s",(char*)parameters[2]);

        struct XYMessage *my_message2=(struct XYMessage*)AllocMem(sizeof(struct XYMessage),MEMF_PUBLIC|MEMF_CLEAR);

        my_message2->xy_Msg.mn_Node.ln_Type=NT_MESSAGE;
        my_message2->xy_Msg.mn_Length=sizeof(struct XYMessage);
        my_message2->xy_Msg.mn_ReplyPort = arexx_process_port;
        my_message2->xy_Getline = 0;
        my_message2->xy_QuitArexx = 0;
        my_message2->xy_Sendtext = 1;

        //printf("ECHO start:\n%s\n",string_to_send2);
                    //printf("send 1 - lets safeputtoport\n");

        //if(SafePutToPort(my_message2, "WookieChatMainTask"))
        if(SafePutToPort(my_message2, maintask_basename))
        {
                    //printf("send 2 - waiting\n");

            WaitPort(arexx_process_port);

                    //printf("send 3 - got it\n");

            GetMsg(arexx_process_port);

                    //printf("send 4 - we've replied\n");
        }

        //printf("ECHO finish\n");

    }

    status_conductor=work_status;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel4hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel4hookfunc, NULL};
#elif __AROS__
struct Hook sel4hook = { { NULL,NULL }, (IPTR(*)())sel4hookfunc, NULL,NULL };
#else
struct Hook sel4hook = { { NULL,NULL }, (ULONG(*)())sel4hookfunc, NULL,NULL };
#endif



//RAW

#ifdef __amigaos4__
int sel5hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel5hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel5hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel5hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(parameters[1])
    {
        sprintf(string_to_send2,"/raw %s",(char*)parameters[1]);

        struct XYMessage *my_message2=(struct XYMessage*)AllocMem(sizeof(struct XYMessage),MEMF_PUBLIC|MEMF_CLEAR);

        my_message2->xy_Msg.mn_Node.ln_Type=NT_MESSAGE;
        my_message2->xy_Msg.mn_Length=sizeof(struct XYMessage);
        my_message2->xy_Msg.mn_ReplyPort = arexx_process_port;
        my_message2->xy_Getline = 0;
        my_message2->xy_QuitArexx = 0;
        my_message2->xy_Sendtext = 1;

        //if(SafePutToPort(my_message2, "WookieChatMainTask"))
        if(SafePutToPort(my_message2, maintask_basename))
        {
            WaitPort(arexx_process_port);
            GetMsg(arexx_process_port);
        }

       if(DEBUG)  printf("raw string, sent!\n");


    }

    status_conductor=work_status;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel5hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel5hookfunc, NULL};
#elif __AROS__
struct Hook sel5hook = { { NULL,NULL }, (IPTR(*)())sel5hookfunc, NULL,NULL };
#else
struct Hook sel5hook = { { NULL,NULL }, (ULONG(*)())sel5hookfunc, NULL,NULL };
#endif




// ISCONNECTED

#ifdef __amigaos4__
int sel6hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel6hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel6hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel6hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(status_conductor)
    {
        status_conductor=work_status;

        if(status_conductor->connection_active)
        {
            status_conductor=work_status;
            return 0;
        }
        else
        {
            status_conductor=work_status;
            return 5;
        }
    }

    status_conductor=work_status;

    return 5;
}

#ifdef __MORPHOS__
struct Hook sel6hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel6hookfunc, NULL};
#elif __AROS__
struct Hook sel6hook = { { NULL,NULL }, (IPTR(*)())sel6hookfunc, NULL,NULL };
#else
struct Hook sel6hook = { { NULL,NULL }, (ULONG(*)())sel6hookfunc, NULL,NULL };
#endif



// GETSERVERNAME

#ifdef __amigaos4__
int sel7hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel7hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel7hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel7hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(status_conductor)
        setmacro((Object*)app, MUIA_Application_RexxString, status_conductor->servername);

    status_conductor=work_status;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel7hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel7hookfunc, NULL};
#elif __AROS__
struct Hook sel7hook = { { NULL,NULL }, (IPTR(*)())sel7hookfunc, NULL,NULL };
#else
struct Hook sel7hook = { { NULL,NULL }, (ULONG(*)())sel7hookfunc, NULL,NULL };
#endif


//CHANNELS

#ifdef __amigaos4__
int sel8hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel8hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel8hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel8hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    char channels_joined[6000];
    strcpy(channels_joined,"");

    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
    {
        if(status_conductor->conductor->name[0]=='#')
        {
            strcat(channels_joined,status_conductor->conductor->name);
            strcat(channels_joined," ");
        }
    }

    channels_joined[strlen(channels_joined)-1]='\0';

    setmacro((Object*)app, MUIA_Application_RexxString, channels_joined);

    status_conductor=work_status;

    //status_conductor->conductor=status_conductor->root;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel8hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel8hookfunc, NULL};
#elif __AROS__
struct Hook sel8hook = { { NULL,NULL }, (IPTR(*)())sel8hookfunc, NULL,NULL };
#else
struct Hook sel8hook = { { NULL,NULL }, (ULONG(*)())sel8hookfunc, NULL,NULL };
#endif

// GETCHANNEL

#ifdef __amigaos4__
int sel9hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel9hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel9hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel9hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    if(status_current)
    {
        if(status_current->current_query)
            setmacro((Object*)app, MUIA_Application_RexxString, status_current->current_query->name);
    }

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel9hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel9hookfunc, NULL};
#elif __AROS__
struct Hook sel9hook = { { NULL,NULL }, (IPTR(*)())sel9hookfunc, NULL,NULL };
#else
struct Hook sel9hook = { { NULL,NULL }, (ULONG(*)())sel9hookfunc, NULL,NULL };
#endif


// GETSELECTEDUSER

#ifdef __amigaos4__
int sel10hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel10hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel10hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel10hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(parameters[1]) //channel
    {
        if(status_conductor)
        {
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
            {
                if(!stricmp(status_conductor->conductor->name,(char*)parameters[1]))
                {
                    DoMethod((Object*)status_conductor->conductor->LV_nicklist,MUIM_NList_GetEntry,MUIV_NList_GetEntry_Active, &work_entry);
                    if(work_entry) setmacro((Object*)app, MUIA_Application_RexxString, work_entry->name);
                    break;
                }
            }
        }
    }
    else if(status_current)
    {
        if(status_current->current_query)
        {
            DoMethod((Object*)status_current->current_query->LV_nicklist,MUIM_NList_GetEntry,MUIV_NList_GetEntry_Active, &work_entry);
            if(work_entry) setmacro((Object*)app, MUIA_Application_RexxString, work_entry->name);

        }
        else setmacro((Object*)app, MUIA_Application_RexxString, "");
    }
    else setmacro((Object*)app, MUIA_Application_RexxString, "");

    status_conductor=work_status;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel10hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel10hookfunc, NULL};
#elif __AROS__
struct Hook sel10hook = { { NULL,NULL }, (IPTR(*)())sel10hookfunc, NULL,NULL };
#else
struct Hook sel10hook = { { NULL,NULL }, (ULONG(*)())sel10hookfunc, NULL,NULL };
#endif




// GETUSER

#ifdef __amigaos4__
int sel11hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel11hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel11hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel11hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    //printf("GETUSER: param0:%s\n",(char*)parameters[0]);
    //printf("GETUSER: param1:%s\n",(char*)parameters[1]);
    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(parameters[2])
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
        {
            if(!stricmp(status_conductor->conductor->name,(char*)parameters[2]))
            {
                break;
            }
        }
    }
    else
    {
        status_conductor->conductor=status_conductor->current_query;
    }

    if(status_conductor)
    {
        if(status_conductor->conductor)
        {
            DoMethod((Object*)status_conductor->conductor->LV_nicklist,MUIM_NList_GetEntry,atol((char*)parameters[1]), &work_entry);
            if(work_entry) setmacro((Object*)app, MUIA_Application_RexxString, work_entry->name);
        }
        else setmacro((Object*)app, MUIA_Application_RexxString, "");
    }
    else setmacro((Object*)app, MUIA_Application_RexxString, "");

    status_conductor=work_status;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel11hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel11hookfunc, NULL};
#elif __AROS__
struct Hook sel11hook = { { NULL,NULL }, (IPTR(*)())sel11hookfunc, NULL,NULL };
#else
struct Hook sel11hook = { { NULL,NULL }, (ULONG(*)())sel11hookfunc, NULL,NULL };
#endif





// GETUSERS

#ifdef __amigaos4__
int sel12hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel12hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel12hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel12hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    //printf("GETUSERS: param0:%s\n",(char*)parameters[0]);

    //get_right_network(parameters);
    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(parameters[1])
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
        {
            if(!stricmp(status_conductor->conductor->name,(char*)parameters[1]))
            {
                break;
            }
        }
    }
    else
    {
        status_conductor->conductor=status_conductor->current_query;
    }

    if(status_conductor)
    {
        if(status_conductor->conductor)
        {
            char found_nicks[30*status_conductor->conductor->nicks];
            strcpy(found_nicks,"");
            for(count=0; count<status_conductor->conductor->nicks; count++)
            {
                if(status_conductor->conductor->nicklist[count].modes[0]!=' ')
                    strcat(found_nicks,status_conductor->conductor->nicklist[count].modes);

                strcat(found_nicks,status_conductor->conductor->nicklist[count].name);
                strcat(found_nicks," ");
            }

            found_nicks[strlen(found_nicks)-1]='\0';

            setmacro((Object*)app, MUIA_Application_RexxString, found_nicks);
            //printf("GETUSERS user list:%s\n",(char*)found_nicks);

        }
        else setmacro((Object*)app, MUIA_Application_RexxString, "");
    }
    else setmacro((Object*)app, MUIA_Application_RexxString, "");

    status_conductor=work_status;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel12hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel12hookfunc, NULL};
#elif __AROS__
struct Hook sel12hook = { { NULL,NULL }, (IPTR(*)())sel12hookfunc, NULL,NULL };
#else
struct Hook sel12hook = { { NULL,NULL }, (ULONG(*)())sel12hookfunc, NULL,NULL };
#endif





// GETUSERSTATE

#ifdef __amigaos4__
int sel13hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel13hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel13hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel13hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    //printf("GETUSERSTATE: param0:%s\n",(char*)parameters[0]); //user
    //printf("GETUSERSTATE: param1:%s\n",(char*)parameters[1]); //channel
    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(parameters[2])
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
        {
            if(!stricmp(status_conductor->conductor->name,(char*)parameters[2]))
            {
                break;
            }
        }
    }
    else
    {
        status_conductor->conductor=status_conductor->current_query;
    }

    if(status_conductor)
    {
        if(status_conductor->conductor)
        {
            setmacro((Object*)app, MUIA_Application_RexxString, status_conductor->conductor->nicklist[atoi((char*)parameters[1])].modes);
            status_conductor=work_status;
            return 0;

        }
        else setmacro((Object*)app, MUIA_Application_RexxString, "");
    }
    else setmacro((Object*)app, MUIA_Application_RexxString, "");

    status_conductor=work_status;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel13hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel13hookfunc, NULL};
#elif __AROS__
struct Hook sel13hook = { { NULL,NULL }, (IPTR(*)())sel13hookfunc, NULL,NULL };
#else
struct Hook sel13hook = { { NULL,NULL }, (ULONG(*)())sel13hookfunc, NULL,NULL };
#endif






// GETLASTLINE

#ifdef __amigaos4__
int sel15hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel15hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel15hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel15hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif
    work_status=status_conductor;

   if(!get_right_network(parameters)) return 1;

    setmacro((Object*)app, MUIA_Application_RexxString, status_conductor->last_incoming_line_unparsed);

    status_conductor=work_status;

    return 0;


}

#ifdef __MORPHOS__
struct Hook sel15hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel15hookfunc, NULL};
#elif __AROS__
struct Hook sel15hook = { { NULL,NULL }, (IPTR(*)())sel15hookfunc, NULL,NULL };
#else
struct Hook sel15hook = { { NULL,NULL }, (ULONG(*)())sel15hookfunc, NULL,NULL };
#endif



// GETAWAYSTATE

#ifdef __amigaos4__
int sel16hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel16hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel16hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel16hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif
    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(status_conductor)
    {
        if(status_conductor->away)
        {
            status_conductor=work_status;

            return 5;
        }
        else
        {
            status_conductor=work_status;

            return 0;
        }
    }
    else
    {
        status_conductor=work_status;

        return 0;
    }
}

#ifdef __MORPHOS__
struct Hook sel16hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel16hookfunc, NULL};
#elif __AROS__
struct Hook sel16hook = { { NULL,NULL }, (IPTR(*)())sel16hookfunc, NULL,NULL };
#else
struct Hook sel16hook = { { NULL,NULL }, (ULONG(*)())sel16hookfunc, NULL,NULL };
#endif



// GETVERSION

#ifdef __amigaos4__
int sel17hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel17hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel17hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel17hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif


    setmacro((Object*)app, MUIA_Application_RexxString, VERSION_CTCP);

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel17hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel17hookfunc, NULL};
#elif __AROS__
struct Hook sel17hook = { { NULL,NULL }, (IPTR(*)())sel17hookfunc, NULL,NULL };
#else
struct Hook sel17hook = { { NULL,NULL }, (ULONG(*)())sel17hookfunc, NULL,NULL };
#endif


// GETCTCPSOUNDDIR

#ifdef __amigaos4__
int sel18hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel18hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel18hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel18hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif


    setmacro((Object*)app, MUIA_Application_RexxString, my_settings.samples_path);

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel18hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel18hookfunc, NULL};
#elif __AROS__
struct Hook sel18hook = { { NULL,NULL }, (IPTR(*)())sel18hookfunc, NULL,NULL };
#else
struct Hook sel18hook = { { NULL,NULL }, (ULONG(*)())sel18hookfunc, NULL,NULL };
#endif



// GETDCCRECEIVEDIR

#ifdef __amigaos4__
int sel19hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel19hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel19hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel19hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif


    setmacro((Object*)app, MUIA_Application_RexxString, my_settings.download_path);

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel19hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel19hookfunc, NULL};
#elif __AROS__
struct Hook sel19hook = { { NULL,NULL }, (IPTR(*)())sel19hookfunc, NULL,NULL };
#else
struct Hook sel19hook = { { NULL,NULL }, (ULONG(*)())sel19hookfunc, NULL,NULL };
#endif




// PLAYSOUND

#ifdef __amigaos4__
int sel20hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel20hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel20hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel20hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif
    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(parameters[1]) sprintf(buffer2,"/sound %s",(char*)parameters[1]);
    process_outgoing(buffer2,0);

    status_conductor=work_status;

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel20hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel20hookfunc, NULL};
#elif __AROS__
struct Hook sel20hook = { { NULL,NULL }, (IPTR(*)())sel20hookfunc, NULL,NULL };
#else
struct Hook sel20hook = { { NULL,NULL }, (ULONG(*)())sel20hookfunc, NULL,NULL };
#endif



/*
 NAME
    SetRexxVar - Sets the value of a variable of a running ARexx program

   SYNOPSIS
    error = SetRexxVar(message,varname,value,length)
    D0                 A0      A1      D0    D1

    LONG SetRexxVar(struct RexxMsg *,char *,char *,ULONG);

   FUNCTION
    This function will attempt to the the value of the symbol
    varname in the ARexx script that sent the message.

    While this function is new in the V37 amiga.lib, it is safe to
    call it in all versions of the operating system.  It is also
    PURE code, thus usable in resident/pure executables.

   NOTE
    This is a stub in amiga.lib.  It is only available via amiga.lib.
    The stub has two labels.  One, _SetRexxVar, takes the arguments
    from the stack.  The other, SetRexxVar, takes the arguments in
    registers.

    This routine does a CheckRexxMsg() on the message.

   EXAMPLE

    char    *value;

    if (!SetRexxVar(rxmsg,"TheVar","25 Dollars",10))
    {
        // The value of TheVar will now be "25 Dollars"
    }

   INPUTS
    message        A message gotten from an ARexx script
    varname        The name of the variable to set
    value        A string that will be the new value of the variable
    length        The length of the value string


   RESULTS
    error        0 for success, otherwise an error code.
            (Other codes may exists, these are documented)
            3  == Insufficient Storage
            9  == String too long
            10 == invalid message

   SEE ALSO
    SetRexxVar(), CheckRexxMsg()

*/
BOOL wanna_quit;
BOOL getline_wait;
char *line_string;
char network_result[5];
char line_buffer[800];
LONG getline_signal;
struct RexxMsg *my_rexxmsg;
char nick_hostname[200];

// GETLINE


#ifdef __amigaos4__
int sel21hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel21hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel21hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel21hookfunc(void) {
  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    getline_wait=TRUE;

    //printf("1 getline incoming message\n");

    if(wanna_quit==TRUE)
    {
        //disable_getline_hook();
        incoming_message=(struct XYMessage*)GetMsg(arexx_process_port);
        return 1;
    }
    if(wanna_quit==FALSE)
    {
        //printf("1 getline waiting at msgport\n");
        incoming_message=(struct XYMessage*)WaitPort(arexx_process_port);

    }

    {
        //incoming_message=(struct XYMessage*)WaitPort(arexx_process_port);

        if(incoming_message)
        {
            //printf("3 processing incoming message!\n");
            //printf("getline? %d\n",incoming_message->xy_Getline);
            //printf("getline? %d, quit? %d\n",incoming_message->xy_Getline, incoming_message->xy_QuitArexx);

            //edit below
            incoming_message=(struct XYMessage*)GetMsg(arexx_process_port);
            if(incoming_message)
            //while((incoming_message=(struct XYMessage*)GetMsg(arexx_process_port)))
            {
                if(incoming_message->xy_Getline==1)
                {
                    //while(GetMsg(incoming_message));
                    //printf("2 getline message recieved\n");

                    //set the string while the display is paused
                    getmacro((Object*)app,MUIA_Application_RexxMsg,&my_rexxmsg);
                    
                    #ifdef __amigaos4__
                    if(status_conductor)
                    {
                        if(status_conductor->last_incoming_line_unparsed)
                        {
                            strcpy(line_buffer,status_conductor->last_incoming_line_unparsed);

                            work_status=status_conductor;
                            for(count=0,status_conductor=status_root; status_conductor && status_conductor!=work_status; count++,status_conductor=status_conductor->next);
                            sprintf(network_result,"%d",count);
                            IRexxSys->SetRexxVarFromMsg((char*)"line.network",network_result,(struct RexxMsg*)my_rexxmsg);
                            status_conductor=work_status;

                            if(stricmp(status_conductor->last_incoming_line_unparsed,""))
                            {
                                IRexxSys->SetRexxVarFromMsg((char*)"line",status_conductor->last_incoming_line_unparsed,(struct RexxMsg*)my_rexxmsg);


                                line_string=strtok(line_buffer," ");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                
                                if(line_string)
                                {
                                    //copy the nick from the prefix variable
                                    IRexxSys->SetRexxVarFromMsg((char*)"line.prefix",line_string,(struct RexxMsg*)my_rexxmsg);
                                }


                                if(strchr((char*)line_string,'!'))
                                {
                                    //copy the nick from the prefix variable
                                    if(line_string[0]==':') count=1; else count=0;
                                    for(count2=0;count<=(LONG)strlen((char*)line_string) && line_string[count]!='!' && line_string[count]!='\0'; count2++,count++)
                                    {
                                        nick_hostname[count2]=line_string[count];
                                    }
                                    nick_hostname[count]='\0';
                                    IRexxSys->SetRexxVarFromMsg((char*)"line.nick",nick_hostname,(struct RexxMsg*)my_rexxmsg);

                                    //copy the hostname from the prefix variable
                                    for(count++,count2=0;count<(LONG)strlen((char*)line_string) && line_string[count]!='\0'; count2++,count++)
                                    {
                                        nick_hostname[count2]=line_string[count];
                                    }
                                    nick_hostname[count]='\0';
                                    IRexxSys->SetRexxVarFromMsg((char*)"line.hostname",nick_hostname,(struct RexxMsg*)my_rexxmsg);

                                }
                                line_string=strtok(NULL," ");
                                if(line_string) IRexxSys->SetRexxVarFromMsg((char*)"line.command",line_string,(struct RexxMsg*)my_rexxmsg);

                                line_string=strtok(NULL," ");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) IRexxSys->SetRexxVarFromMsg((char*)"line.dest",line_string,(struct RexxMsg*)my_rexxmsg);

                                line_string=strtok(NULL,"");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) IRexxSys->SetRexxVarFromMsg((char*)"line.rest",line_string,(struct RexxMsg*)my_rexxmsg);
                            }
                        }
                    }
                    #elif __morphos__
                    if(status_conductor)
                    {
                        if(status_conductor->last_incoming_line_unparsed)
                        {
                            strcpy(line_buffer,status_conductor->last_incoming_line_unparsed);

                            work_status=status_conductor;
                            for(count=0,status_conductor=status_root; status_conductor && status_conductor!=work_status; count++,status_conductor=status_conductor->next);
                            sprintf(network_result,"%d",count);
                            SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.NETWORK",(c_in)network_result,strlen(network_result));
                            status_conductor=work_status;

                            if(stricmp(status_conductor->last_incoming_line_unparsed,""))
                            {
                                SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE",(c_in)status_conductor->last_incoming_line_unparsed,strlen(status_conductor->last_incoming_line_unparsed));

                                line_string=strtok(line_buffer," ");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.PREFIX",(c_in)line_string,strlen(line_string));

                                if(strchr((char*)line_string,'!'))
                                {
                                    //copy the nick from the prefix variable
                                    if(line_string[0]==':') count=1; else count=0;
                                    for(count2=0;count<=(LONG)strlen((char*)line_string) && line_string[count]!='!' && line_string[count]!='\0'; count2++,count++)
                                    {
                                        nick_hostname[count2]=line_string[count];
                                    }
                                    nick_hostname[count]='\0';
                                    SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.NICK",(c_in)nick_hostname,strlen(nick_hostname));

                                    //copy the hostname from the prefix variable
                                    for(count++,count2=0;count<(LONG)strlen((char*)line_string) && line_string[count]!='\0'; count2++,count++)
                                    {
                                        nick_hostname[count2]=line_string[count];
                                    }
                                    nick_hostname[count]='\0';
                                    SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.HOSTNAME",(c_in)nick_hostname,strlen(nick_hostname));

                                }

                                line_string=strtok(NULL," ");
                                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.COMMAND",(c_in)line_string,strlen(line_string));

                                line_string=strtok(NULL," ");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.DEST",(c_in)line_string,strlen(line_string));

                                line_string=strtok(NULL,"");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.REST",(c_in)line_string,strlen(line_string));
                            }
                        }
                    }
                    #elif __AROS__
                    if(status_conductor)
                    {
                        if(status_conductor->last_incoming_line_unparsed)
                        {
                            strcpy(line_buffer,status_conductor->last_incoming_line_unparsed);

                            work_status=status_conductor;
                            for(count=0,status_conductor=status_root; status_conductor && status_conductor!=work_status; count++,status_conductor=status_conductor->next);
                            sprintf(network_result,"%d",count);
                            SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.network",(c_in)network_result,strlen(network_result));
                            status_conductor=work_status;

                            if(stricmp(status_conductor->last_incoming_line_unparsed,""))
                            {
                                SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line",(c_in)status_conductor->last_incoming_line_unparsed,strlen(status_conductor->last_incoming_line_unparsed));

                                line_string=strtok(line_buffer," ");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.prefix",(c_in)line_string,strlen(line_string));

                                if(strchr((char*)line_string,'!'))
                                {
                                    //copy the nick from the prefix variable
                                    if(line_string[0]==':') count=1; else count=0;
                                    for(count2=0;count<=(LONG)strlen((char*)line_string) && line_string[count]!='!' && line_string[count]!='\0'; count2++,count++)
                                    {
                                        nick_hostname[count2]=line_string[count];
                                    }
                                    nick_hostname[count]='\0';
                                    SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.nick",(c_in)nick_hostname,strlen(nick_hostname));

                                    //copy the hostname from the prefix variable
                                    for(count++,count2=0;count<(LONG)strlen((char*)line_string) && line_string[count]!='\0'; count2++,count++)
                                    {
                                        nick_hostname[count2]=line_string[count];
                                    }
                                    nick_hostname[count]='\0';
                                    SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.hostname",(c_in)nick_hostname,strlen(nick_hostname));

                                }

                                line_string=strtok(NULL," ");
                                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.command",(c_in)line_string,strlen(line_string));

                                line_string=strtok(NULL," ");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.dest",(c_in)line_string,strlen(line_string));

                                line_string=strtok(NULL,"");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.rest",(c_in)line_string,strlen(line_string));
                            }
                        }
                    }

                    #else
                    if(status_conductor)
                    {
                        if(status_conductor->last_incoming_line_unparsed)
                        {
                            strcpy(line_buffer,status_conductor->last_incoming_line_unparsed);

                            work_status=status_conductor;
                            for(count=0,status_conductor=status_root; status_conductor && status_conductor!=work_status; count++,status_conductor=status_conductor->next);
                            sprintf(network_result,"%d",count);
                            SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line.network",(c_in)network_result,strlen(network_result));
                            status_conductor=work_status;

                            if(stricmp(status_conductor->last_incoming_line_unparsed,""))
                            {
                                SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line",(c_in)status_conductor->last_incoming_line_unparsed,strlen(status_conductor->last_incoming_line_unparsed));

                                line_string=strtok(line_buffer," ");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line.prefix",(c_in)line_string,strlen(line_string));

                                if(strchr((char*)line_string,'!'))
                                {
                                    //copy the nick from the prefix variable
                                    if(line_string[0]==':') count=1; else count=0;
                                    for(count2=0;count<=(LONG)strlen((char*)line_string) && line_string[count]!='!' && line_string[count]!='\0'; count2++,count++)
                                    {
                                        nick_hostname[count2]=line_string[count];
                                    }
                                    nick_hostname[count]='\0';
                                    SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line.nick",(c_in)nick_hostname,strlen(nick_hostname));

                                    //copy the hostname from the prefix variable
                                    for(count++,count2=0;count<(LONG)strlen((char*)line_string) && line_string[count]!='\0'; count2++,count++)
                                    {
                                        nick_hostname[count2]=line_string[count];
                                    }
                                    nick_hostname[count]='\0';
                                    SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line.hostname",(c_in)nick_hostname,strlen(nick_hostname));

                                }

                                line_string=strtok(NULL," ");
                                if(line_string) SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line.command",(c_in)line_string,strlen(line_string));

                                line_string=strtok(NULL," ");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line.dest",(c_in)line_string,strlen(line_string));

                                line_string=strtok(NULL,"");
                                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                                if(line_string) SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line.rest",(c_in)line_string,strlen(line_string));
                            }
                        }
                    }
                    #endif

                }
                getline_wait=FALSE;

                ReplyMsg((struct Message*)incoming_message);
                //printf("5 replied to getline message\n");
            }
        }
    }

    //printf("completed wait\n");

    return 0;

}

#ifdef __MORPHOS__
struct Hook sel21hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel21hookfunc, NULL};
#elif __AROS__
struct Hook sel21hook = { { NULL,NULL }, (IPTR(*)())sel21hookfunc, NULL,NULL };
#else
struct Hook sel21hook = { { NULL,NULL }, (ULONG(*)())sel21hookfunc, NULL,NULL };
#endif



// PARSELINE

#ifdef __amigaos4__
int sel23hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel23hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel23hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel23hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    work_status=status_conductor;

    if(!get_right_network(parameters)) return 1;

    if(parameters[1])
    {
       if(DEBUG)  printf("parseline 1\n");
        getmacro((Object*)app,MUIA_Application_RexxMsg,&my_rexxmsg);

        #ifdef __amigaos4__
        if(status_conductor)
        {
       if(DEBUG)  printf("parseline 2\n");

            strcpy(line_buffer,(char*)parameters[1]);

            for(count=0,status_conductor=status_root; status_conductor && status_conductor!=work_status; count++,status_conductor=status_conductor->next);
            sprintf(network_result,"%d",count);
            IRexxSys->SetRexxVarFromMsg((char*)"line.network",network_result,(struct RexxMsg*)my_rexxmsg);

            //if(stricmp(status_conductor->last_incoming_line_unparsed,""))
            if(stricmp(line_buffer,""))
            {
       if(DEBUG)  printf("parseline 3\n");

                //IRexxSys->SetRexxVarFromMsg("line",status_conductor->last_incoming_line_unparsed,(struct RexxMsg*)my_rexxmsg);
                IRexxSys->SetRexxVarFromMsg((char*)"line",line_buffer,(struct RexxMsg*)my_rexxmsg);

       if(DEBUG)  printf("parseline line_string:%s\n",line_buffer);

                line_string=strtok(line_buffer," ");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) IRexxSys->SetRexxVarFromMsg((char*)"line.prefix",line_string,(struct RexxMsg*)my_rexxmsg);

                if(strchr((char*)line_string,'!'))
                {
                    //copy the nick from the prefix variable
                    if(line_string[0]==':') count=1; else count=0;
                    for(count2=0;count<=(LONG)strlen((char*)line_string) && line_string[count]!='!' && line_string[count]!='\0'; count2++,count++)
                    {
                        nick_hostname[count2]=line_string[count];
                    }
                    nick_hostname[count]='\0';
                    IRexxSys->SetRexxVarFromMsg((char*)"line.nick",nick_hostname,(struct RexxMsg*)my_rexxmsg);

                    //copy the hostname from the prefix variable
                    for(count++,count2=0;count<(LONG)strlen((char*)line_string) && line_string[count]!='\0'; count2++,count++)
                    {
                        nick_hostname[count2]=line_string[count];
                    }
                    nick_hostname[count]='\0';
                    IRexxSys->SetRexxVarFromMsg((char*)"line.hostname",nick_hostname,(struct RexxMsg*)my_rexxmsg);

                }
                line_string=strtok(NULL," ");
                if(line_string) IRexxSys->SetRexxVarFromMsg((char*)"line.command",line_string,(struct RexxMsg*)my_rexxmsg);

                line_string=strtok(NULL," ");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) IRexxSys->SetRexxVarFromMsg((char*)"line.dest",line_string,(struct RexxMsg*)my_rexxmsg);

                line_string=strtok(NULL,"");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) IRexxSys->SetRexxVarFromMsg((char*)"line.rest",line_string,(struct RexxMsg*)my_rexxmsg);


        }
        }
        #elif __morphos__
        if(status_conductor)
        {
            //strcpy(line_buffer,status_conductor->last_incoming_line_unparsed);
            strcpy(line_buffer,(char*)parameters[1]);

            for(count=0,status_conductor=status_root; status_conductor && status_conductor!=work_status; count++,status_conductor=status_conductor->next);
            sprintf(network_result,"%d",count);
            SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.NETWORK",(c_in)network_result,strlen(network_result));

            //if(stricmp(status_conductor->last_incoming_line_unparsed,""))
            if(stricmp(line_buffer,""))
            {
                //SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line",(c_in)status_conductor->last_incoming_line_unparsed,strlen(status_conductor->last_incoming_line_unparsed));
                SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE",(c_in)line_buffer,strlen(line_buffer));

                line_string=strtok(line_buffer," ");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.PREFIX",(c_in)line_string,strlen(line_string));

                if(strchr((char*)line_string,'!'))
                {
                    //copy the nick from the prefix variable
                    if(line_string[0]==':') count=1; else count=0;
                    for(count2=0;count<=(LONG)strlen((char*)line_string) && line_string[count]!='!' && line_string[count]!='\0'; count2++,count++)
                    {
                        nick_hostname[count2]=line_string[count];
                    }
                    nick_hostname[count]='\0';
                    SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.NICK",(c_in)nick_hostname,strlen(nick_hostname));

                    //copy the hostname from the prefix variable
                    for(count++,count2=0;count<(LONG)strlen((char*)line_string) && line_string[count]!='\0'; count2++,count++)
                    {
                        nick_hostname[count2]=line_string[count];
                    }
                    nick_hostname[count]='\0';
                    SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.HOSTNAME",(c_in)nick_hostname,strlen(nick_hostname));

                }

                line_string=strtok(NULL," ");
                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.COMMAND",(c_in)line_string,strlen(line_string));

                line_string=strtok(NULL," ");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.DEST",(c_in)line_string,strlen(line_string));

                line_string=strtok(NULL,"");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"LINE.REST",(c_in)line_string,strlen(line_string));
            }
        }
        #elif __AROS__
        if(status_conductor)
        {
            //strcpy(line_buffer,status_conductor->last_incoming_line_unparsed);
            strcpy(line_buffer,(char*)parameters[1]);

            for(count=0,status_conductor=status_root; status_conductor && status_conductor!=work_status; count++,status_conductor=status_conductor->next);
            sprintf(network_result,"%d",count);
            SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.network",(c_in)network_result,strlen(network_result));

            //if(stricmp(status_conductor->last_incoming_line_unparsed,""))
            if(stricmp(line_buffer,""))
            {
                //SetRexxVar((struct Message*)my_rexxmsg,(c_in)"line",(c_in)status_conductor->last_incoming_line_unparsed,strlen(status_conductor->last_incoming_line_unparsed));
                SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line",(c_in)line_buffer,strlen(line_buffer));

                line_string=strtok(line_buffer," ");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.prefix",(c_in)line_string,strlen(line_string));

                if(strchr((char*)line_string,'!'))
                {
                    //copy the nick from the prefix variable
                    if(line_string[0]==':') count=1; else count=0;
                    for(count2=0;count<=(LONG)strlen((char*)line_string) && line_string[count]!='!' && line_string[count]!='\0'; count2++,count++)
                    {
                        nick_hostname[count2]=line_string[count];
                    }
                    nick_hostname[count]='\0';
                    SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.nick",(c_in)nick_hostname,strlen(nick_hostname));

                    //copy the hostname from the prefix variable
                    for(count++,count2=0;count<(LONG)strlen((char*)line_string) && line_string[count]!='\0'; count2++,count++)
                    {
                        nick_hostname[count2]=line_string[count];
                    }
                    nick_hostname[count]='\0';
                    SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.hostname",(c_in)nick_hostname,strlen(nick_hostname));

                }

                line_string=strtok(NULL," ");
                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.command",(c_in)line_string,strlen(line_string));

                line_string=strtok(NULL," ");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.dest",(c_in)line_string,strlen(line_string));

                line_string=strtok(NULL,"");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"line.rest",(c_in)line_string,strlen(line_string));
            }
        }
        #else
        if(status_conductor)
        {
            //strcpy(line_buffer,status_conductor->last_incoming_line_unparsed);
            strcpy(line_buffer,(char*)parameters[1]);

            for(count=0,status_conductor=status_root; status_conductor && status_conductor!=work_status; count++,status_conductor=status_conductor->next);
            sprintf(network_result,"%d",count);
            SetRexxVar((struct Message*)my_rexxmsg,(a_in)"line.network",(a_in)network_result,strlen(network_result));

            //if(stricmp(status_conductor->last_incoming_line_unparsed,""))
            if(stricmp(line_buffer,""))
            {
                //SetRexxVar((struct Message*)my_rexxmsg,(a_in)"line",(a_in)status_conductor->last_incoming_line_unparsed,strlen(status_conductor->last_incoming_line_unparsed));
                SetRexxVar((struct Message*)my_rexxmsg,(a_in)"line",(a_in)line_buffer,strlen(line_buffer));

                line_string=strtok(line_buffer," ");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) SetRexxVar((struct Message*)my_rexxmsg,(a_in)"line.prefix",(a_in)line_string,strlen(line_string));

                if(strchr((char*)line_string,'!'))
                {
                    //copy the nick from the prefix variable
                    if(line_string[0]==':') count=1; else count=0;
                    for(count2=0;count<=(LONG)strlen((char*)line_string) && line_string[count]!='!' && line_string[count]!='\0'; count2++,count++)
                    {
                        nick_hostname[count2]=line_string[count];
                    }
                    nick_hostname[count]='\0';
                    SetRexxVar((struct Message*)my_rexxmsg,(a_in)"line.nick",(a_in)nick_hostname,strlen(nick_hostname));

                    //copy the hostname from the prefix variable
                    for(count++,count2=0;count<(LONG)strlen((char*)line_string) && line_string[count]!='\0'; count2++,count++)
                    {
                        nick_hostname[count2]=line_string[count];
                    }
                    nick_hostname[count]='\0';
                    SetRexxVar((struct Message*)my_rexxmsg,(a_in)"line.hostname",(a_in)nick_hostname,strlen(nick_hostname));

                }

                line_string=strtok(NULL," ");
                if(line_string) SetRexxVar((struct Message*)my_rexxmsg,(a_in)"line.command",(a_in)line_string,strlen(line_string));

                line_string=strtok(NULL," ");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) SetRexxVar((struct Message*)my_rexxmsg,(a_in)"line.dest",(a_in)line_string,strlen(line_string));

                line_string=strtok(NULL,"");
                if(line_string) { if(line_string[0]==':') { for(ULONG count=0; count<=strlen(line_string); count++) line_string[count]=line_string[count+1]; } }
                if(line_string) SetRexxVar((struct Message*)my_rexxmsg,(a_in)"line.rest",(a_in)line_string,strlen(line_string));
            }
        }
        #endif

    }

    status_conductor=work_status;

    return 0;


}

#ifdef __MORPHOS__
struct Hook sel23hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel23hookfunc, NULL};
#elif __AROS__
struct Hook sel23hook = { { NULL,NULL }, (IPTR(*)())sel23hookfunc, NULL,NULL };
#else
struct Hook sel23hook = { { NULL,NULL }, (ULONG(*)())sel23hookfunc, NULL,NULL };
#endif



// GETLINECOUNT

#ifdef __amigaos4__
int sel14hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel14hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel14hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel14hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif


    getmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_Entries, &entries);

    sprintf(buffer2,"%lu",entries);

    setmacro((Object*)app, MUIA_Application_RexxString, buffer2);

    return 0;


}

#ifdef __MORPHOS__
struct Hook sel14hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel14hookfunc, NULL};
#elif __AROS__
struct Hook sel14hook = { { NULL,NULL }, (IPTR(*)())sel14hookfunc, NULL,NULL };
#else
struct Hook sel14hook = { { NULL,NULL }, (ULONG(*)())sel14hookfunc, NULL,NULL };
#endif



// COMMAND

#ifdef __amigaos4__
int sel22hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel22hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel22hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel22hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    work_status=status_conductor;
    if(!get_right_network(parameters)) return 1;

    if(parameters[1])
    {
        struct XYMessage *my_message2=(struct XYMessage*)AllocMem(sizeof(struct XYMessage),MEMF_PUBLIC|MEMF_CLEAR);

        my_message2->xy_Msg.mn_Node.ln_Type=NT_MESSAGE;
        my_message2->xy_Msg.mn_Length=sizeof(struct XYMessage);
        my_message2->xy_Msg.mn_ReplyPort = arexx_process_port;
        my_message2->xy_Getline = 0;
        my_message2->xy_QuitArexx = 0;
        my_message2->xy_Sendtext = 1;

        sprintf(string_to_send2,"%s",(char*)parameters[1]);

       if(DEBUG)  printf("doing a COMMAND\n");
        QUIET_DCC=TRUE;
        //if(SafePutToPort(my_message2, "WookieChatMainTask"))
        if(SafePutToPort(my_message2, maintask_basename))
        {
            WaitPort(arexx_process_port);
            GetMsg(arexx_process_port);
        }
        QUIET_DCC=FALSE;

    }

    status_conductor=work_status;

    return 0;


}

#ifdef __MORPHOS__
struct Hook sel22hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel22hookfunc, NULL};
#elif __AROS__
struct Hook sel22hook = { { NULL,NULL }, (IPTR(*)())sel22hookfunc, NULL,NULL };
#else
struct Hook sel22hook = { { NULL,NULL }, (ULONG(*)())sel22hookfunc, NULL,NULL };
#endif



// NETWORKS
// ;this command counts the amount of networks you're on

#ifdef __amigaos4__
int sel24hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel24hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel24hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel24hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif


    work_status=status_conductor;

    for(count=0,status_conductor=status_root; status_conductor;)
    {    status_conductor=status_conductor->next; if(status_conductor) count++; }

    sprintf(network_result,"%d",count);
    setmacro((Object*)app, MUIA_Application_RexxString, network_result);

    status_conductor=work_status;

    return 0;





}

#ifdef __MORPHOS__
struct Hook sel24hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel24hookfunc, NULL};
#elif __AROS__
struct Hook sel24hook = { { NULL,NULL }, (IPTR(*)())sel24hookfunc, NULL,NULL };
#else
struct Hook sel24hook = { { NULL,NULL }, (ULONG(*)())sel24hookfunc, NULL,NULL };
#endif


// GETNETWORK
// returns information for the current network you're on

#ifdef __amigaos4__
int sel25hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel25hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel25hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel25hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif

    getmacro((Object*)app,MUIA_Application_RexxMsg,&my_rexxmsg);

    work_status=status_conductor;

    status_conductor=status_root;

    for(count=0; status_conductor; count++)
    {
        if(status_conductor==status_current) break;
        status_conductor=status_conductor->next;

    }
    if(status_conductor)
    {
        if(!status_conductor->connection_active)
        {
            status_conductor=work_status;
            return 0;
        }

        sprintf(network_result,"%d",count);

        #ifdef __amigaos4__
        if(strcmp(status_conductor->groupname,"")) IRexxSys->SetRexxVarFromMsg((char*)"network.groupname",status_conductor->groupname,(struct RexxMsg*)my_rexxmsg);
        if(strcmp(status_conductor->servername,"")) IRexxSys->SetRexxVarFromMsg((char*)"network.servername",status_conductor->servername,(struct RexxMsg*)my_rexxmsg);
        IRexxSys->SetRexxVarFromMsg((char*)"network.number",network_result,(struct RexxMsg*)my_rexxmsg);
        #elif __morphos__
        if(strcmp(status_conductor->groupname,"")) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"NETWORK.GROUPNAME",(c_in)status_conductor->groupname,strlen(status_conductor->servername));
        if(strcmp(status_conductor->servername,"")) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"NETWORK.SERVERNAME",(c_in)status_conductor->servername,strlen(status_conductor->servername));
        SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"NETWORK.NUMBER",(c_in)network_result,strlen(network_result));
        #elif __AROS__
        if(strcmp(status_conductor->groupname,"")) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"network.groupname",(c_in)status_conductor->groupname,strlen(status_conductor->servername));
        if(strcmp(status_conductor->servername,"")) SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"network.servername",(c_in)status_conductor->servername,strlen(status_conductor->servername));
        SetRexxVar((struct RexxMsg*)my_rexxmsg,(c_in)"network.number",(c_in)network_result,strlen(network_result));
        #else
        if(strcmp(status_conductor->groupname,"")) SetRexxVar((struct Message*)my_rexxmsg,(a_in)"network.groupname",(a_in)status_conductor->groupname,strlen(status_conductor->servername));
        if(strcmp(status_conductor->servername,"")) SetRexxVar((struct Message*)my_rexxmsg,(a_in)"network.servername",(a_in)status_conductor->servername,strlen(status_conductor->servername));
        SetRexxVar((struct Message*)my_rexxmsg,(a_in)"network.number",(a_in)network_result,strlen(network_result));
        #endif

    }

    status_conductor=work_status;

    return 0;


}

#ifdef __MORPHOS__
struct Hook sel25hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel25hookfunc, NULL};
#elif __AROS__
struct Hook sel25hook = { { NULL,NULL }, (IPTR(*)())sel25hookfunc, NULL,NULL };
#else
struct Hook sel25hook = { { NULL,NULL }, (ULONG(*)())sel25hookfunc, NULL,NULL };
#endif



// ACTIVATE THE WINDOW

#ifdef __amigaos4__
int sel26hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __MORPHOS__
int sel26hookfunc(REG(a0, struct Hook *hook),REG(a2, APTR app),REG(a1, LONG **parameters)) {
#elif __AROS__
int sel26hookfunc(struct Hook *hook, APTR app,LONG **parameters) {
#elif __GNUC__
int sel26hookfunc(void) {

  register APTR a2 __asm("a2");                          APTR app = a2;
  register LONG **a1 __asm("a1");                        LONG **parameters = a1;
  register struct Hook *a0 __asm("a0");                  struct Hook *hook = a0;
#endif


        strcpy(string_to_send2,"/activate");

        struct XYMessage *my_message2=(struct XYMessage*)AllocMem(sizeof(struct XYMessage),MEMF_PUBLIC|MEMF_CLEAR);

        my_message2->xy_Msg.mn_Node.ln_Type=NT_MESSAGE;
        my_message2->xy_Msg.mn_Length=sizeof(struct XYMessage);
        my_message2->xy_Msg.mn_ReplyPort = arexx_process_port;
        my_message2->xy_Getline = 0;
        my_message2->xy_QuitArexx = 0;
        my_message2->xy_Sendtext = 1;


        if(SafePutToPort(my_message2, maintask_basename))
        {
            WaitPort(arexx_process_port);
            GetMsg(arexx_process_port);
        }


    return 0;


}

#ifdef __MORPHOS__
struct Hook sel26hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)sel26hookfunc, NULL};
#elif __AROS__
struct Hook sel26hook = { { NULL,NULL }, (IPTR(*)())sel26hookfunc, NULL,NULL };
#else
struct Hook sel26hook = { { NULL,NULL }, (ULONG(*)())sel26hookfunc, NULL,NULL };
#endif


static struct MUI_Command commands[] =
{
    { (char*)"SAY",              (char*)"NETWORK/K,CHANNEL/K,TEXT/F/A" , 3        , &sel2hook  },
    { (char*)"SAYALL",           (char*)"TEXT/F/A"           , 1        , &selhook   },
    { (char*)"GETMYNICK",        (char*)"NETWORK/K"          , 1        , &sel3hook  },
    { (char*)"ISCONNECTED",      (char*)"NETWORK/K"          , 1        , &sel6hook  },
    { (char*)"GETSERVERNAME",    (char*)"NETWORK/K"          , 1        , &sel7hook  },
    // "USERHOST"
    { (char*)"ECHO",             (char*)"NETWORK/K,COLOUR/K,TEXT/F/A", 3, &sel4hook  },
    { (char*)"RAW",              (char*)"NETWORK/K,TEXT/F"   , 2        , &sel5hook  },
    // "GETWINNUM"
    { (char*)"CHANNELS",         (char*)"NETWORK/K"          , 1        , &sel8hook  },
    { (char*)"GETCHANNEL",       NULL                 , 0        , &sel9hook  },
    // "GETDCC"
    { (char*)"GETSELECTEDUSER",  (char*)"NETWORK/K,CHANNEL/K", 2        , &sel10hook },
    { (char*)"GETUSER",          (char*)"NETWORK/K,USER/A,CHANNEL/K", 3 , &sel11hook },
    { (char*)"GETUSERS",         (char*)"NETWORK/K,CHANNEL/K", 1        , &sel12hook },
    { (char*)"GETUSERSTATE",     (char*)"NETWORK/K,USER/A,CHANNEL/K", 3 , &sel13hook },
    { (char*)"GETLINE",          NULL                 , 0        , &sel21hook },
    { (char*)"GETLINECOUNT",     NULL                 , 0        , &sel14hook },
    { (char*)"GETLASTLINE",      (char*)"NETWORK/K"          , 1        , &sel15hook },
    { (char*)"GETAWAYSTATE",     (char*)"NETWORK/K"          , 1        , &sel16hook },
    { (char*)"GETVERSION",       NULL                 , 0        , &sel17hook },
    { (char*)"GETCTCPSOUNDDIR",  NULL                 , 0        , &sel18hook },
    { (char*)"GETDCCRECIEVEDIR", NULL                 , 0        , &sel19hook },
    { (char*)"GETDCCRECEIVEDIR", NULL                 , 0        , &sel19hook },
    // "MATCHUSERHOST"
    { (char*)"PLAYSOUND",       (char*)"NETWORK/K,FILENAME/F/A"   , 2        , &sel20hook },
    { (char*)"COMMAND",         (char*)"NETWORK/K,TEXT/F/A"   , 2        , &sel22hook },
    { (char*)"PARSELINE",       (char*)"NETWORK/K,TEXT/F/A"   , 2        , &sel23hook },
    { (char*)"GETNETWORK",      NULL                  , 0        , &sel25hook },
//    { (char*)"ACTIVATE",        NULL                  , 0        , &sel26hook },
    { (char*)"NETWORKS",        NULL                  , 0        , &sel24hook },
    #ifdef __AROS__
    { 0,       0              , 0    , 0     }
    #else
    { (LONG)NULL,       (LONG)NULL               , (LONG)NULL     , (LONG)NULL     }
    #endif
};


static struct MUI_Command commands2[] =
{
    #ifdef __AROS__
    { 0, 0, 0, 0}
    #else
    { (LONG)NULL,       (LONG)NULL               , (LONG)NULL     , (LONG)NULL     }
    #endif
};


APTR AREXX_App;
char basename[100];
BOOL AREXX_started;
char arexxquit_portname[100];

int disable_getline_hook(void)
{
    if(DEBUG)  printf("disable hooks\n");
    setmacro((Object*)AREXX_App,MUIA_Application_Commands, &commands2);
    return 0;
}

int AREXX_Task(void)
{

    AREXX_started=TRUE;

    //APTR AREXX_App = AllocVec(sizeof(Object*), MEMF_PUBLIC|MEMF_CLEAR);
    //printf("arexx 1\n");

    AREXX_App =(Object*) ApplicationObject,
        //MUIA_Application_UsedClasses, classlist,
        MUIA_Application_Author, "James Carroll",
        MUIA_Application_Base, "WOOKIECHAT_AREXX",
        MUIA_Application_Title, "WookieChat AREXX",
        MUIA_Application_Version, VERSION_MUI,
        MUIA_Application_Copyright, "James Carroll",
        MUIA_Application_Description, "IRC Client",
        //MUIA_Application_DiskObject, dobj,
        MUIA_Application_UseRexx, TRUE,
        MUIA_Application_Commands, &commands,
    End;

    //printf("arexx 2\n");

    getmacro((Object*)AREXX_App,MUIA_Application_Base,&string1);
    strcpy(basename,string1);
    sprintf(work_buffer,"\n\033cAREXX port: %s",string1);
    setmacro((Object*)WookieChat->TX_about3, MUIA_Text_Contents,work_buffer);
    strcat(basename,"_msgport");

    BOOL running = TRUE;

    if(DEBUG) printf("arexx task started\n");
    if(DEBUG) printf("finding port %s\n",basename);

    Forbid();
    #ifdef __morphos__
    if(!FindPort((c_in)basename))
    #elif __AROS__
    if(!FindPort((c_in)basename))
    #else
    if(!FindPort((i_in)basename))
    #endif
    {
        arexx_process_port = CreatePort(basename,0);

        sprintf(arexxquit_portname,"%s_quit",maintask_basename);

        arexx_quit_port = CreatePort(arexxquit_portname,0);
        if(arexx_process_port && arexx_quit_port)
        {
            LONG quit_signal = 1L << arexx_quit_port->mp_SigBit;
            LONG port_signal = 1L << arexx_process_port->mp_SigBit;
            arexx_process_port->mp_Flags=PA_SIGNAL;

            while(running)
            {

            ULONG sigs = 0;

             while(DoMethod((Object*)AREXX_App,MUIM_Application_NewInput,&sigs) != (ULONG)MUIV_Application_ReturnID_Quit)
             {
                  if(sigs)
                  {
                       sigs = Wait(sigs | SIGBREAKF_CTRL_C /*| port_signal*/ | quit_signal);

                    if(sigs & quit_signal)
                    {
                       if(DEBUG)  printf("arexx quit port hit!\n");

                        incoming_message=(struct XYMessage*)GetMsg(arexx_quit_port);
                        if(incoming_message)
                        {
                            if(incoming_message->xy_QuitArexx==1)
                            {
                                //printf("quitting arexx task\n");
                                running=FALSE;
                                ReplyMsg((struct Message*)incoming_message);
                                break;
                            }

                        }

                    }

                    if(sigs & port_signal)
                    {
                       if(DEBUG)  printf("arexx port hit!\n");

                    }

                    if(sigs & SIGBREAKF_CTRL_C) { running=FALSE; printf("arexx break\n"); break; }


                    //Delay(10);

                  }
                 }
                running=FALSE;

            }


            //printf("arexx task ending 1..\n");


            while((incoming_message=(struct XYMessage*)GetMsg(arexx_process_port))) ReplyMsg((struct Message*)incoming_message);
            //printf("arexx task ending 2..\n");

            DeletePort(arexx_process_port);
            DeletePort(arexx_quit_port);

        }
        else printf("cant create AREXX process message port\n");
    }
    else printf("cant create AREXX process message port, already exists\n");
    Permit();

    //printf("arexx task ending 3..\n");

    //printf("arexx task ended\n");

    if(AREXX_App) MUI_DisposeObject((Object*)AREXX_App);

    AREXX_started=FALSE;


    return 0;


}


