/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/* NLIST FOR DCC WINDOW STUFF FOLLOWS */

#include "includes.h"

#include <proto/muimaster.h>

#include "intern.h"

LONG position;

/* Locals */
static char banmask[200];
static char list_found_nicks[5000][50]; // Nick completion function variables
static int nickcomp_count=0;
static int nickcomp_state=0;
static char *banmask_tokens[6];
static char string11[900];
static char *string2;
static char *string3;
static char *string1;
static char sendstuff[1500];

void setup_background_colours(void)
{
    //return;
    
    //if(GEIT) printf("setup_background_colours() 1\n");
    if(status_current)
    {
        if(status_current->current_query)
        {
            if(status_current->current_query->LV_channel != NULL)
            {
                //if(DEBUG) printf("channel: specs[2]:%s\n",pendisplay_specs[1]->buf);
                if(pendisplay_specs[1]->buf[0]=='2')
                {
                    //if(DEBUG) printf("channel: its an rgb colour\n");
                    setmacro((Object*)status_current->current_query->LV_nicklist,MUIA_NList_ListBackground, pendisplay_specs[1]);

                }
                else
                {
                    //if(DEBUG) printf("channel: its NOT an rgb colour\n");
                    setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_ListBackground, popimage_background);
                }

    //if(GEIT) printf("setup_background_colours() 1a\n");

                DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_Redraw,MUIV_NList_Redraw_All);
    //if(GEIT) printf("setup_background_colours() 1b\n");
            }
        }
    }

    //if(GEIT) printf("setup_background_colours() 2\n");

    if(WookieChat->LV_graphical_smileys_preview != NULL)
    {
        if(pendisplay_specs[1]->buf[0]=='2')
            setmacro((Object*)WookieChat->LV_graphical_smileys_preview ,MUIA_NList_ListBackground, pendisplay_specs[1]);
        else
            setmacro((Object*)WookieChat->LV_graphical_smileys_preview ,MUIA_NList_ListBackground, popimage_background);
    //if(GEIT) printf("setup_background_colours() 2a\n");
    }

    //if(GEIT) printf("setup_background_colours() 3\n");

    if(status_current)
    {
        if(status_current->current_query)
        {

            if(status_current->current_query->LV_nicklist != NULL)
            {
    //if(GEIT) printf("setup_background_colours() 3a\n");

                //if(DEBUG) printf("nicklist: specs[2]:%s\n",pendisplay_specs[2]->buf);
                if(pendisplay_specs[2]->buf[0]=='2')
                {
                    //if(DEBUG) printf("nicklist: its an rgb colour\n");
                    setmacro((Object*)status_current->current_query->LV_nicklist,MUIA_NList_ListBackground, pendisplay_specs[2]);

                }
                else
                {
                    //if(DEBUG) printf("nicklist: its NOT an rgb colour\n");
                    setmacro((Object*)status_current->current_query->LV_nicklist,MUIA_NList_ListBackground, popimage_nicklistbackground);
                }
    //if(GEIT) printf("setup_background_colours() 3b\n");

                setmacro((Object*)status_current->current_query->LV_nicklist,MUIA_NList_ListPen, pendisplay_specs[22]);
                DoMethod((Object*)status_current->current_query->LV_nicklist,MUIM_NList_Redraw,MUIV_NList_Redraw_All);
    //if(GEIT) printf("setup_background_colours() 3c\n");

            }
        }
    }


    //if(GEIT) printf("setup_background_colours() 4\n");

    if(WookieChat->LV_tabs != NULL)
    {

        //if(DEBUG) printf("tabs: specs[23]:%s\n",pendisplay_specs[23]->buf);
    //if(GEIT) printf("setup_background_colours() 4a\n");

        if(pendisplay_specs[23]->buf[0]=='2')
        {
            //if(DEBUG) printf("tabs: its an rgb colour\n");
            setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_ListBackground, pendisplay_specs[23]); //->buf);
        }
        else
        {

            //if(DEBUG) printf("tabs: its NOT an rgb colour\n");
            setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_ListBackground, popimage_tabsbackground);
        }

    //if(GEIT) printf("setup_background_colours() 4b\n");

        setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_ListPen, pendisplay_specs[24]);

    //if(GEIT) printf("setup_background_colours() 4c\n");

        DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_Redraw,MUIV_NList_Redraw_All);
    //if(GEIT) printf("setup_background_colours() 4dn");

    }


    // Below are the activity colours

    //if(GEIT) printf("setup_background_colours() 5\n");

    set_activity(pendisplay_specs[19]->buf, pendisplay_specs[20]->buf, pendisplay_specs[21]->buf);

    //if(GEIT) printf("setup_background_colours() 6\n");


}


#ifdef __amigaos4__
struct dcc_entry * ConstructDCC_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct dcc_entry *new_entry)) {

#elif __MORPHOS__
struct dcc_entry * ConstructDCC_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct dcc_entry *new_entry)) {
#elif __AROS__
struct dcc_entry * ConstructDCC_TextFunc(struct Hook *hook,Object *obj,struct dcc_entry *new_entry) {
    #elif __GNUC__
struct dcc_entry * ConstructDCC_TextFunc(void) {

  register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct dcc_entry *a1 __asm("a1");              struct dcc_entry *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

    struct dcc_entry *work_entry = (struct dcc_entry*) AllocVec(sizeof(struct dcc_entry),0);
    *work_entry=*new_entry;

    return work_entry;
}


#ifdef __MORPHOS__
struct Hook ConstructDCC_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)ConstructDCC_TextFunc, NULL };
#elif __AROS__
struct Hook ConstructDCC_TextHook = { { NULL,NULL }, (IPTR(*)())ConstructDCC_TextFunc, NULL,NULL };
#else
struct Hook ConstructDCC_TextHook = { { NULL,NULL }, (ULONG(*)())ConstructDCC_TextFunc, NULL,NULL };
#endif





  #ifdef __amigaos4__
static void DestructDCC_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct dcc_entry *new_entry)) {
    #elif __MORPHOS__
    static void DestructDCC_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct dcc_entry *new_entry)) {
#elif __AROS__
    static void DestructDCC_TextFunc(struct Hook *hook, Object *obj, struct dcc_entry *new_entry) {
  #elif __GNUC__
static void DestructDCC_TextFunc(void)
{

 register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct dcc_entry *a1 __asm("a1");             struct dcc_entry *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

  if (new_entry)
    FreeVec((struct dcc_entry*) new_entry);
}


#ifdef __MORPHOS__
struct Hook DestructDCC_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DestructDCC_TextFunc, NULL };
#elif __AROS__
struct Hook DestructDCC_TextHook = { { NULL,NULL },(IPTR(*)())DestructDCC_TextFunc, NULL,NULL };
#else
struct Hook DestructDCC_TextHook = { { NULL,NULL },(ULONG(*)())DestructDCC_TextFunc, NULL,NULL };
#endif

  #ifdef __amigaos4__
static void DisplayDCC_send_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct dcc_entry *a_entry)) {
#elif __MORPHOS__
static void DisplayDCC_send_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct dcc_entry *a_entry)) {
#elif __AROS__
static void DisplayDCC_send_TextFunc(struct Hook *hook, char **array, struct dcc_entry *a_entry) {

  #elif __GNUC__
static void DisplayDCC_send_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct dcc_entry *a1 __asm("a1");        struct dcc_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {

        array[0]=a_entry->status;
        array[1]=a_entry->nick;
        array[2]=a_entry->filename;
        array[3]=a_entry->cps;
        array[4]=a_entry->bytes_transferred;
        array[5]=a_entry->filesize;
        array[6]=a_entry->percentage;
        array[7]=a_entry->timeleft;

    }
    else
    {

        array[0]= (char*)GCS(150,"Status");
        array[1]= (char*)GCS(240,"Reciever");
        array[2]= (char*)GCS(215,"Filename");
        array[3]= (char*)"kB/s";
        array[4]= (char*)GCS(151,"Sent");
        array[5]= (char*)GCS(152,"File size");
        array[6]= (char*)"%";
        array[7]= (char*)GCS(153,"Time Left");

    }

}

#ifdef __MORPHOS__
struct Hook DisplayDCC_send_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DisplayDCC_send_TextFunc, NULL };
#elif __AROS__
struct Hook DisplayDCC_send_TextHook = { { NULL,NULL },(IPTR(*)())DisplayDCC_send_TextFunc, NULL,NULL };
#else
struct Hook DisplayDCC_send_TextHook = { { NULL,NULL },(ULONG(*)())DisplayDCC_send_TextFunc, NULL,NULL };
#endif

  #ifdef __amigaos4__
static void DisplayDCC_recv_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct dcc_entry *a_entry)) {
    #elif __MORPHOS__
    static void DisplayDCC_recv_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct dcc_entry *a_entry)) {
   #elif __AROS__
    static void DisplayDCC_recv_TextFunc(struct Hook *hook, char **array,struct dcc_entry *a_entry) {
  #elif __GNUC__
static void DisplayDCC_recv_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct dcc_entry *a1 __asm("a1");        struct dcc_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {

        array[0]=a_entry->status;
        array[1]=a_entry->nick;
        array[2]=a_entry->filename;
        array[3]=a_entry->cps;
        array[4]=a_entry->bytes_transferred;
        array[5]=a_entry->filesize;
        array[6]=a_entry->percentage;
        array[7]=a_entry->timeleft;


    }
    else
    {

        array[0]= (char*)GCS(150,"Status");
        array[1]= (char*)GCS(239,"Sender");
        array[2]= (char*)GCS(215,"Filename");
        array[3]= (char*)"kB/s";
        array[4]= (char*)GCS(154,"Recieved");
        array[5]= (char*)GCS(152,"File size");
        array[6]= (char*)"%";
        array[7]= (char*)GCS(153,"Time Left");


    }

}


#ifdef __MORPHOS__
struct Hook DisplayDCC_recv_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DisplayDCC_recv_TextFunc, NULL };
#elif __AROS__
struct Hook DisplayDCC_recv_TextHook = { { NULL,NULL },(IPTR(*)())DisplayDCC_recv_TextFunc, NULL,NULL };
#else
struct Hook DisplayDCC_recv_TextHook = { { NULL,NULL },(ULONG(*)())DisplayDCC_recv_TextFunc, NULL,NULL };
#endif

/* BETTER STRING STUFF FOLLOWS */


APTR my_object;


ULONG NList_HandleInput(struct IClass *cl, Object *obj, struct MUIP_HandleInput *msg)
{
    //printf("handle event\n");

    //struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);
    //setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);

    //printf("handle event subclass\n");

    /*if (msg->imsg)
    {
        switch (msg->imsg->Class)
        {

            case IDCMP_MOUSEBUTTONS:
                    //printf("mouse buttons\n");

                    switch(msg->imsg->Code)
                    {
                        case SELECTDOWN:
                            setmacro(status_current->current_query->LV_channel,MUIA_NList_Quiet,TRUE);
                            printf("select down\n");
                            break;

                        case SELECTUP:
                            setmacro(status_current->current_query->LV_channel,MUIA_NList_Quiet,FALSE);
                            printf("select up\n");


                            //DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_GetSelectInfo,res);
                            //if(res->start == -1 && res->end == -1 && !res->vnum)
                            //{
                          //        setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);
                          //      break;
                          //  }
                          //  else
                          //  {
                                //DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_CopyToClip, MUIV_NList_CopyToClip_Selected, 0, NULL, NULL);
                                //DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_Select,MUIV_NList_Select_All, MUIV_NList_Select_Off, NULL);
                          //        setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);
                                //return MUI_EventHandlerRC_Eat; //we want to eat the event

                          //  }
                          //  break;

                        default:
                            break;

                    }

                    break;



        }
    }    */

    if (!DoSuperMethodA(cl,obj,(Msg)msg))
        return(FALSE);

    return 0;
}

BOOL user_is_trying_to_select_text;
//struct channel_entry waiting_to_be_displayed_buffer[50];
int waiting_to_be_displayed_count;
Object *my_object1;

void selectup_code()
{
    #ifdef __amigaos4__

    if(user_is_trying_to_select_text==TRUE)
    {

        user_is_trying_to_select_text=FALSE;
        status_conductor=status_root;
        while(status_conductor)
        {
            status_conductor->conductor=status_conductor->root;

            while(status_conductor->conductor)
            {
                for(a=0; a<status_conductor->conductor->waiting_to_be_displayed_count; a++)
                {
                    strcpy(centry->entry,status_conductor->conductor->waiting_to_be_displayed_buffer[a].entry);
                    centry->colour=status_conductor->conductor->waiting_to_be_displayed_buffer[a].colour;
                    getmacro((Object*)status_conductor->conductor->LV_channel,MUIA_NList_Visible, &visible);

                    if(status_conductor->conductor==status_current->current_query)
                    {
                        getmacro((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Visible, &visible);

                        if(visible > 0)
                        {
                            getmacro((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Entries, &entries);
                            getmacro((Object*)status_conductor->current_query->LV_channel,MUIA_NList_First, &first);
                            DoMethod((Object*)status_conductor->current_query->LV_channel,MUIM_NList_InsertSingleWrap,centry,MUIV_NList_Insert_Bottom,WRAPCOL0,ALIGN_LEFT);

                            if ((first + visible) >= (entries - 1))
                            DoMethod((Object*)status_conductor->current_query->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

                        }
                        else
                        {
                            DoMethod((Object*)status_conductor->current_query->LV_channel,MUIM_NList_InsertSingleWrap,centry,MUIV_NList_Insert_Bottom,WRAPCOL0,ALIGN_LEFT);
                            DoMethod((Object*)status_conductor->current_query->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

                        }

                        status_conductor->current_query->entries_count++;

                        if(status_conductor->current_query->entries_count > my_settings.max_lines_for_buffers)
                        {
                            DoMethod((Object*)status_conductor->current_query->LV_channel,MUIM_NList_Remove,MUIV_NList_Remove_First);
                            status_conductor->current_query->entries_count--;
                        }
                    }
                    else
                    {

                        DoMethod((Object*)status_conductor->conductor->LV_channel,MUIM_NList_InsertSingleWrap,centry,MUIV_NList_Insert_Bottom,WRAPCOL0,ALIGN_LEFT);
                        DoMethod((Object*)status_conductor->conductor->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

                        status_conductor->conductor->entries_count++;

                        if(status_conductor->conductor->entries_count > my_settings.max_lines_for_buffers)
                        {
                            DoMethod((Object*)status_conductor->conductor->LV_channel,MUIM_NList_Remove,MUIV_NList_Remove_First);
                            status_conductor->conductor->entries_count--;
                        }

                    }

                }
                status_conductor->conductor=status_conductor->conductor->next;
            }
            status_conductor=status_conductor->next;

        }


    }

    #endif

}


ULONG NList_HandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
//    struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);
    //printf("custom nlist class handle event\n");


    getmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,&my_object1);
    if(status_current->current_query->name[0]=='#')
    {
        if(my_object1!=status_current->current_query->STR_topic &&
            my_object1!=status_current->current_query->STR_keyword &&
            my_object1!=status_current->current_query->STR_limit)
            setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);
    }
    else
    {
        if(my_object1!=status_current->current_query->STR_topic)
            setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);

    }


    #ifdef __amigaos4__

    if (msg->imsg)
    {
        switch (msg->imsg->Class)
        {

            case IDCMP_MOUSEBUTTONS:

                    switch(msg->imsg->Code)
                    {
                        //code for display pause while selecting text
                        case SELECTDOWN:
                                //printf("select down\n");
                            if(user_is_trying_to_select_text==FALSE)
                            {
                                //printf("select down\n");

                                user_is_trying_to_select_text=TRUE;
                                status_conductor=status_root;
                                while(status_conductor)
                                {
                                    status_conductor->conductor=status_conductor->root;

                                    while(status_conductor->conductor)
                                    {
                                        status_conductor->conductor->waiting_to_be_displayed_count=0;

                                        status_conductor->conductor=status_conductor->conductor->next;

                                    }
                                    status_conductor=status_conductor->next;
                                }
                            }
                            //return MUI_EventHandlerRC_Eat; //we want to eat the event

                        break;

                        case SELECTUP:
                                //printf("select up\n");
                            if(user_is_trying_to_select_text==TRUE)
                            {
                                selectup_code();
                                //printf("select up\n");
                            }
                            //return MUI_EventHandlerRC_Eat; //we want to eat the event


                            break;

                        default:
                            break;

                    }

                    break;



        }
    }

    #endif

    //experimental, 2008-11-18
    //if (!DoSuperMethodA(cl,obj,(Msg)msg))
    //    return(FALSE);

    //return MUI_EventHandlerRC_Eat; //we want to eat the event

    return 0;
}



ULONG NList_ContextMenu(struct IClass *cl, Object *obj, struct MUIP_ContextMenuChoice *msg)
{

    char work[200];
    int count=0;

    char action[1000];
    char people[2000];
    strcpy(action,"");
    strcpy(people,"");

    for(LONG id = MUIV_NList_NextSelected_Start;;)
    {
    struct list_entry *work_entry = NULL;
    DoMethod((Object*)obj,MUIM_NList_NextSelected,&id);
    DoMethod((Object*)obj,MUIM_NList_GetEntry,id, &work_entry);
    if(id == MUIV_NList_NextSelected_End) break;
    //printf("deselect\n");
    //the below causes the background to be grey when delsected instead of the background penspec I've configured
    //DoMethod((Object*)obj,MUIM_NList_Select,MUIV_NList_Select_Active,MUIV_NList_Select_Off, NULL);

    status_conductor=status_current;

    switch(muiUserData(msg->item))
    {


        case 65: // WHOIS
            status_conductor=status_current;
            SWhoIs(work_entry->name);
            break;

        case 66: // OPEN QUERY
            status_conductor=status_current;
            SQuery(work_entry->name);
        if(RECENTLY_CREATED_A_TAB==TRUE)
        {
            if(status_current)
            {
                if(status_current->current_query)
                {
                    setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_Active,status_current->current_query->nlist_tab_number);
                    sort_linked_list();
                 }
            }

            RECENTLY_CREATED_A_TAB=FALSE;
        }

        break;

        case 67: // GIVE OPS
            if(count==0) { strcat(action,"+"); count++; }
            strcat(action,"o");
            strcat(people,work_entry->name); strcat(people," ");

            //sprintf(work,"/raw MODE %s +o %s",status_current->current_query->name, work_entry->name);
            //process_outgoing(work,0);
            break;

        case 68: // TAKE OPS
            if(count==0) { strcat(action,"-"); count++; }
            strcat(action,"o");
            strcat(people,work_entry->name); strcat(people," ");
            //sprintf(work,"/raw MODE %s -o %s",status_current->current_query->name, work_entry->name);
            //process_outgoing(work,0);
            break;


        case 69: // GIVE HALFOPS
            if(count==0) { strcat(action,"+"); count++; }
            strcat(action,"h");
            strcat(people,work_entry->name); strcat(people," ");

            //sprintf(work,"/raw MODE %s +h %s",status_current->current_query->name, work_entry->name);
            //process_outgoing(work,0);
            break;

        case 70: // TAKE HALFOPS
            if(count==0) { strcat(action,"-"); count++; }
            strcat(action,"h");
            strcat(people,work_entry->name); strcat(people," ");

            //sprintf(work,"/raw MODE %s -h %s",status_current->current_query->name, work_entry->name);
            //process_outgoing(work,0);
            break;

        case 71: // GIVE VOICE
            if(count==0) { strcat(action,"+"); count++; }
            strcat(action,"v");
            strcat(people,work_entry->name); strcat(people," ");

            //sprintf(work,"/raw MODE %s +v %s",status_current->current_query->name, work_entry->name);
            //process_outgoing(work,0);
            break;

        case 72: // TAKE VOICE
            if(count==0) { strcat(action,"-"); count++; }
            strcat(action,"v");
            strcat(people,work_entry->name); strcat(people," ");

            //sprintf(work,"/raw MODE %s -v %s",status_current->current_query->name, work_entry->name);
            //process_outgoing(work,0);
            break;

        case 73: // KICK USER
            sprintf(work,"/kick %s",work_entry->name);
            process_outgoing(work,0);
            break;

        case 75: // CTCP PING
            SPing(work_entry->name);
            break;

        case 76: // CTCP VERSION
              sprintf(work,"/version %s",work_entry->name);
            process_outgoing(work,0);
            break;

        case 77: // CTCP TIME
              sprintf(work,"/time %s",work_entry->name);
            process_outgoing(work,0);
            break;

        case 78: // Channel Listview Copy to Clipboard
            DoMethod((Object*)obj,MUIM_NList_CopyToClip, MUIV_NList_CopyToClip_Selected, 0, NULL, NULL);
            DoMethod((Object*)obj,MUIM_NList_Select,MUIV_NList_Select_All, MUIV_NList_Select_Off, NULL);
            setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);
            break;

        case 79: //bring up dcc send window
            if(work_entry->name) setmacro((Object*)WookieChat->STR_dcc_send_nick,MUIA_String_Contents,work_entry->name);
                else setmacro((Object*)WookieChat->STR_dcc_send_nick,MUIA_String_Contents,"");
            setmacro((Object*)WookieChat->WI_dcc_send,MUIA_Window_Open, TRUE);
            break;

        case 81:
            if(work_entry->name) initiate_outgoing_dcc_chat(work_entry->name);
            break;

        case 80: //open private messages tab, all private messages will go here
            status_conductor=status_current;
            SQuery((char*)"-global");
        if(RECENTLY_CREATED_A_TAB==TRUE)
        {
            if(status_current)
            {
                if(status_current->current_query)
                {
                    setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_Active,status_current->current_query->nlist_tab_number);
                    sort_linked_list();
                 }
            }

            RECENTLY_CREATED_A_TAB=FALSE;
        }
            break;

        case 83: //opening ban window dialog
            status_conductor=status_current;
            status_conductor->conductor=current_query;

            for(int count=0; count<status_conductor->conductor->nicks; count++)
            {
                if(!stricmp(work_entry->name,status_conductor->conductor->nicklist[count].name))
                {
                    if(status_conductor->conductor->nicklist[count].hostname)
                    {
                        if(DEBUG) printf("set ban on user:%s with the hostname:%s\n",status_conductor->conductor->nicklist[count].name,status_conductor->conductor->nicklist[count].hostname);
                        strcpy(string11,status_conductor->conductor->nicklist[count].hostname);

                        banmask_tokens[0]=strtok(string11,"@");
                        banmask_tokens[1]=strtok(NULL,". ");
                        banmask_tokens[2]=strtok(NULL,". ");
                        banmask_tokens[3]=strtok(NULL,". ");
                        banmask_tokens[4]=strtok(NULL,". ");
                        banmask_tokens[5]=strtok(NULL," ");

                        if(banmask_tokens[5])
                            sprintf(banmask,"*!*%s@*%s.%s.%s",banmask_tokens[0],banmask_tokens[3],banmask_tokens[4],banmask_tokens[5]);
                        else if(banmask_tokens[4])
                            sprintf(banmask,"*!*%s@*%s.%s.%s",banmask_tokens[0],banmask_tokens[2],banmask_tokens[3],banmask_tokens[4]);
                        else if(banmask_tokens[3])
                            sprintf(banmask,"*!*%s@*%s.%s.%s",banmask_tokens[0],banmask_tokens[1],banmask_tokens[2],banmask_tokens[3]);
                        else if(banmask_tokens[2])
                            sprintf(banmask,"*!*%s@*%s.%s",banmask_tokens[0],banmask_tokens[1],banmask_tokens[2]);
                        else if(banmask_tokens[1])
                            sprintf(banmask,"*!*%s@*%s",banmask_tokens[0],banmask_tokens[1]);

                        if(DEBUG) printf("banning using the banmask:%s\n",banmask);

                        sprintf(sendstuff,"mode %s +b %s\r\n",status_conductor->conductor->name, banmask);
                        send_text(sendstuff);


                    }
                    break;
                }

            }

            break;

    }

    }

    if(count>=1)
    {
        sprintf(work,"/raw MODE %s %s %s",status_current->current_query->name, action, people);
        process_outgoing(work,0);
    }

    status_conductor=status_current;
    status_conductor->conductor=status_conductor->current_query;
    DisplayNicks();

    return 0;
}


ULONG NList_Setup(struct IClass *cl, Object *obj, struct Message *msg)
{

    //printf("setup\n");

    struct InstanceData *data = (struct InstanceData*)INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,(Msg)msg))
    {
        return(FALSE);
    }

    /* initialize and add window input event handler */
    //#ifdef __amigaos4__
    data->ehnode.ehn_Object = obj;
    data->ehnode.ehn_Class = cl;
    data->ehnode.ehn_Events = IDCMP_MOUSEBUTTONS;
    DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->ehnode);
    //#endif

    return(TRUE);
}

ULONG NList_Cleanup(struct IClass *cl,Object *obj, struct Message *msg)
{
    //printf("cleanup part1\n");
    struct InstanceData *data = (struct InstanceData*)INST_DATA(cl,obj);


    //#ifdef __amigaos4__
    DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->ehnode);
    //#endif

    if(!DoSuperMethodA(cl,obj,(Msg)msg)) return(FALSE);
    //printf("cleanup part2\n");


    return(TRUE);
}



#ifdef __amigaos4__
DISPATCHERPROTO(NList_Dispatcher)
{
#elif __MORPHOS__
DISPATCHER(NList_Dispatcher)
{
#elif __AROS__
//SAVEDS ULONG NList_Dispatcher( REG(a0, IClass *cl), REG(a2, Object * obj), REG(a1, Msg msg))
ULONG NList_Dispatcher(Class *cl, Object *obj, Msg msg)
{
#else
ULONG NList_Dispatcher(void)
{
  register struct IClass *a0 __asm("a0"); struct IClass *cl = a0;
  register Object *a2 __asm("a2");        Object *obj = a2;
  register Msg a1 __asm("a1");            Msg msg = a1;
#endif



    switch(msg->MethodID)
    {
        case MUIM_Setup:             return(NList_Setup(cl,obj,(struct Message*)msg));
        case MUIM_Cleanup:           return(NList_Cleanup(cl,obj, (struct Message *)msg));
        case MUIM_ContextMenuChoice: return(NList_ContextMenu(cl,obj,(struct MUIP_ContextMenuChoice*)msg));
        case MUIM_HandleEvent:       return(NList_HandleEvent(cl,obj,(struct MUIP_HandleEvent*)msg));
    }
    
    return(DoSuperMethodA(cl,obj,msg));
        
}

ULONG BetterString_HandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg);
ULONG BetterString_Setup(struct IClass *cl,Object *obj, struct Message *msg);
ULONG BetterString_Cleanup(struct IClass *cl,Object *obj, struct Message *msg);


#ifdef __amigaos4__
DISPATCHERPROTO(BetterString_Dispatcher)
{
#elif __MORPHOS__
DISPATCHER(BetterString_Dispatcher)
{
#elif __AROS__
//DISPATCHERPROTO(BetterString_Dispatcher)
ULONG BetterString_Dispatcher(Class *cl, Object *obj, Msg msg)
{    
#else
ULONG BetterString_Dispatcher(void)
{
  register struct IClass *a0 __asm("a0"); struct IClass *cl = a0;
  register Object *a2 __asm("a2");        Object *obj = a2;
  register Msg a1 __asm("a1");            Msg msg = a1;
#endif

    switch (msg->MethodID)
    {

        case MUIM_Setup: return(BetterString_Setup(cl,obj,(struct Message*)msg));
        case MUIM_Cleanup: return(BetterString_Cleanup(cl,obj, (struct Message *)msg));
        case MUIM_HandleEvent: return(BetterString_HandleEvent(cl,obj,(struct MUIP_HandleEvent*)msg));

    }

    return(DoSuperMethodA(cl,obj,msg));
}



ULONG BetterString_Setup(struct IClass *cl,Object *obj,struct Message *msg)
{
    struct InstanceData *data = (struct InstanceData*)INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,(Msg)msg))
    {
        return(FALSE);
    }


    /* initialize and add window input event handler */
    data->ehnode.ehn_Object = obj;
    data->ehnode.ehn_Class = cl;
    data->ehnode.ehn_Events = IDCMP_RAWKEY; /* don't use VANILLAKEY events,
    */
    /* else menu shortcuts are eaten
    */
    DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->ehnode);

    return(TRUE);
}

ULONG BetterString_Cleanup(struct IClass *cl,Object *obj, struct Message *msg)
{
    struct InstanceData *data = (struct InstanceData*)INST_DATA(cl,obj);

    /* remove user input handler */
    DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->ehnode);

    return(DoSuperMethodA(cl,obj,(Msg)msg));
}

BOOL ready_to_make_chat_gadget_active=FALSE;


//nick completion
ULONG BetterString_HandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{

    //if(DEBUG) printf("betterstring handle event\n");

    if (msg->imsg)
    {
        switch (msg->imsg->Class)
        {

            case IDCMP_RAWKEY:

            switch(msg->imsg->Qualifier)
            {
                case 0x8080: //ramiga key is being held down

                    switch (msg->imsg->Code)
                    {
                        /*case CURSORLEFT:
                            switch_between_tabs_with_keys2(LEFT);
                            return MUI_EventHandlerRC_Eat; //<- when we want to eat the event

                            break;

                        case CURSORRIGHT:
                            switch_between_tabs_with_keys2(RIGHT);
                            return MUI_EventHandlerRC_Eat; //<- when we want to eat the event

                            break;
                         */
                        default:
                            if(DEBUG) printf("lets switch with keys now\n");
                            int a=msg->imsg->Code;
                            if(a<=9)
                            {
                                if(DEBUG) printf("lets switch with keys now, to key %d\n",a);

                                switch_between_tabs_with_keys(a);
                                if(DEBUG) printf("exited switch_between_tabs_with_keys()\n");

                                if(current_query && my_settings.use_nlistobject_for_tabs == 0)
                                {
                                    DoMethod((Object*)current_query->BT_querybutton,MUIM_KillNotify,MUIA_Selected);
                                    setmacro(current_query->BT_querybutton,MUIA_Selected,TRUE);
                                    DoMethod((Object*)current_query->BT_querybutton,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,current_query->iv);

                                }
                                if(DEBUG) printf("selected the button\n");
                                //break;

                                return MUI_EventHandlerRC_Eat; //<- when we want to eat the event

                                if(DEBUG) printf("event eaten! but we shouldnt see this message!\n");

                            }
                            break;

                    }

                    break;

            }

            switch(msg->imsg->Code)
            {


                    int length;
                    int typed_length;
                    int begin_tab;
                    int a,b,c,d,found_nicks;

                case CURSORUP:
                    if(status_current->current_query->string_conductor)
                    {

                        if(status_current->current_query->string_conductor->previous)
                        {
                            char * string123 = NULL;
                            getmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,&string123);

                            //are we at the bottom and trying to go one previous? lets add new to the bottom first before going up
                            if(strcmp(string123,"") && status_current->current_query->string_conductor->next==NULL)
                            {
                                //printf("add new to the bottom before going up\n");
                                //strcpy(status_current->current_query->string_conductor->buffer_history,string123);
                                strncpy(status_current->current_query->string_conductor->buffer_history,string123,800-1);

                                struct history *work_history=status_current->current_query->string_conductor;
                                status_current->current_query->string_conductor->next = malloc(sizeof(struct history));
                                status_current->current_query->string_conductor=status_current->current_query->string_conductor->next;
                                status_current->current_query->string_conductor->previous=work_history;

                                status_current->current_query->string_conductor->next=NULL;
                                status_current->current_query->string_conductor->buffer_history[0]='\0';
                                status_current->current_query->string_conductor=status_current->current_query->string_conductor->previous;


                            }

                            //copy previous history entry in list to the string gadget
                            status_current->current_query->string_conductor=status_current->current_query->string_conductor->previous;

                            if(status_current->current_query->string_conductor->buffer_history[0] != '\0')
                            {
                                struct history *work_history=status_current->current_query->string_conductor;

                                setmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,work_history->buffer_history);
                            }
                        }

                    }
                    nickcomp_state=0;
                    return MUI_EventHandlerRC_Eat; //<- when we want to eat the event
                    break;

                case CURSORDOWN:
                    if(status_current->current_query->string_conductor)
                    {
                        char * string123 = NULL;
                        if(status_current->current_query->string_conductor->next)
                        {
                            status_current->current_query->string_conductor=status_current->current_query->string_conductor->next;
                        }

                        getmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,&string123);

                        if(status_current->current_query->string_conductor->buffer_history[0] != '\0')
                        {
                            setmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,status_current->current_query->string_conductor->buffer_history);
                        }
                        else
                        {
                            struct history *work_history=status_current->current_query->string_conductor->previous;

                            if(work_history)
                            {
                                if(strcmp(string123,"") && strcmp(string123,work_history->buffer_history))
                                {
                                    //printf("adding new text\n");
                                    //strcpy(status_current->current_query->string_conductor->buffer_history,string123);
                                    strncpy(status_current->current_query->string_conductor->buffer_history,string123,800-1);

                                    work_history=status_current->current_query->string_conductor;
                                    status_current->current_query->string_conductor->next = malloc(sizeof(struct history));
                                    status_current->current_query->string_conductor=status_current->current_query->string_conductor->next;
                                    status_current->current_query->string_conductor->previous=work_history;

                                    status_current->current_query->string_conductor->next=NULL;
                                    status_current->current_query->string_conductor->buffer_history[0]='\0';

                                    setmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,"");


                                }
                                else
                                    setmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,"");
                            }
                        }
                    }
                    nickcomp_state=0;
                    return MUI_EventHandlerRC_Eat; //<- when we want to eat the event
                    break;

                case 0xC2: // TAB has been pressed, now do nick completion stuff
                {
                    char tabwork4_string[100];
                    char tabwork5_string[100];
                    char tabwork3_string[900]; //text after cursor position
                    char tabwork2_string[900]; //text before cursor position
                    char tabwork_string[900]; //original string gadget contents
                    char * ptr;
                    /*tabwork_string //original string gadget contents
                    tabwork2_string  //text before cursor position
                    tabwork3_string  //text after cursor position
                    tabwork4_string  //nick text to complete
                    tabwork5_string  //completed nick text to insert
                     */

                    getmacro((Object*)WookieChat->STR_usertext,MUIA_String_BufferPos,&position);
                    getmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,&ptr);

                    length=strlen(ptr);
                    strncpy(tabwork_string, ptr, 900);
                    typed_length=0,a=position,b=0,c=0;
                    begin_tab=0;
                    found_nicks=0;

                    if(my_settings.nick_completion==AMIRC_STYLE)
                    { if(position>=2) {a=position-2; b++;} else { a=position-1; }  }
                    else
                    { a=position-1; }

                    while(tabwork_string[a] != ' ' && tabwork_string[a] != '\0' && a != 0)
                    { a=a-1; b++; }


                    if(a == 0) begin_tab=1;

                    typed_length=b; b=0; c=position;
                    
                    if(tabwork_string[a]== ' ') a++;

                    while(a!=c+1)
                    { tabwork4_string[b]=tabwork_string[a]; a++; b++; }

                    tabwork4_string[b]='\0';

                    //below is a test to prevent nick completion with empty substring searchesn
                    if(tabwork4_string[0]=='\0') break;

                    if(tabwork4_string[b-1]==' ') tabwork4_string[b-1]='\0';


                    c=position-typed_length; b=0; a=0;

                    if(position!=0)
                    {
                        do { tabwork2_string[b]=tabwork_string[a]; b++; a++; }while(a<c);
                    }

                    if(begin_tab) b=b-1; tabwork2_string[b]='\0';

                    a=position; b=0;


                    if(position!=length)
                    {
                        do{ tabwork3_string[b]=tabwork_string[a]; b++; a++; }while(a!=length);
                    }

                    tabwork3_string[b]='\0';


                    strcpy(tabwork5_string,"");


                    if(nickcomp_state)
                    {
                        nickcomp_count++;
                        if(!strcmp(list_found_nicks[nickcomp_count],""))
                        {
                            nickcomp_count=0;
                        }

                        strcpy(tabwork5_string,list_found_nicks[nickcomp_count]);

                        found_nicks=1;

                    }
                    else
                    {
                        struct query_window *work_query = NULL;

                        for(a=0; a!=11; a++) strcpy(list_found_nicks[a],""); //wipe the list of partially found nicks

                        d=0; found_nicks=0; a=0;
                        work_query=status_current->current_query;

                        if(status_current->current_query->name[0]!='#')
                        {

                            for(status_current->conductor=status_current->root;
                                status_current->conductor; status_current->conductor=status_current->conductor->next)
                                {
                                    if(status_current->conductor->name[0]=='#' || status_current->conductor->name[0]=='&')
                                    {

                                        for(d=0;d<status_current->conductor->nicks;d++)
                                        {
                                            if(strnicmp(tabwork4_string,status_current->conductor->nicklist[d].name,strlen(tabwork4_string)) == 0)
                                            {
                                                int count;
                                                for(count=0; count<=nickcomp_count; count++)
                                                {
                                                    //printf("comparing '%s' to '%s'\n",status_current->conductor->nicklist[d].name,list_found_nicks[count]);
                                                    if(!stricmp(status_current->conductor->nicklist[d].name,list_found_nicks[count])) { /*printf("found a duplicate nick! we will not add %s\n",tabwork4_string);*/ break; }

                                                }

                                                if(count==nickcomp_count+1)
                                                {

                                                    //printf("copying nicks to the nick completion struct\n");
                                                    strcpy(list_found_nicks[a],status_current->conductor->nicklist[d].name);
                                                    if(!found_nicks) strcpy(tabwork5_string,list_found_nicks[a]);
                                                    a++;

                                                    found_nicks=1;
                                                }
                                                //printf("count:%i nickcomp_count:%i\n",count, nickcomp_count+1);
                                            }
                                        }

                                    }

                                }

                        }
                        else
                        {

                            for(d=0;d<status_current->current_query->nicks;d++)
                            {
                                if(strnicmp(tabwork4_string,status_current->current_query->nicklist[d].name,strlen(tabwork4_string)) == 0)
                                {
                                    strcpy(list_found_nicks[a],status_current->current_query->nicklist[d].name);
                                    if(!found_nicks) strcpy(tabwork5_string,list_found_nicks[a]);
                                    a++;

                                    found_nicks=1;
                                }
                            }
                        }
                        status_current->conductor=work_query;

                        strcpy(list_found_nicks[a],""); a=0;

                        nickcomp_state=1;
                    }

                    if(found_nicks==0) break;


                    if(my_settings.nick_completion==AMIRC_STYLE)
                    {
                        if((position-typed_length)==1 || (position-typed_length)==-1 )
                        {
                            if((position-typed_length)==-1)
                            {
                                position+=2;
                            }

                            sprintf(tabwork_string,"%s%s: %s",tabwork2_string,tabwork5_string,tabwork3_string);
                            setmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,tabwork_string);
                            position=position-typed_length+strlen(tabwork5_string)+2;

                        }
                        else
                        {
                            sprintf(tabwork_string,"%s%s %s",tabwork2_string,tabwork5_string,tabwork3_string);
                            setmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,tabwork_string);
                            position=position-typed_length+strlen(tabwork5_string)+1;
                        }

                    }
                    else
                    {
                        sprintf(tabwork_string,"%s%s%s",tabwork2_string,tabwork5_string,tabwork3_string);
                        setmacro((Object*)WookieChat->STR_usertext,MUIA_String_Contents,tabwork_string);
                        position=position-typed_length+strlen(tabwork5_string);

                    }

                    setmacro((Object*)WookieChat->STR_usertext,MUIA_String_BufferPos,position);


                    return MUI_EventHandlerRC_Eat; //<- when we want to eat the event

                    break;
                }
                case 0xC0:
                case 0xC1:
                case 0xC4:

                    nickcomp_state=0;
                    //printf("space\n");
                    //break;


            }
            break;

        }
    }

    //return MUI_EventHandlerRC_Eat; //<- when we want to eat the event

    return 0;
}

// BELOW IS THE SUBCLASS FOR THE POPPEN OBJECTS

SAVEDS ULONG Group_Setup(struct IClass *cl,Object *obj,struct Message *msg)
{
    getmacro((Object*)WookieChat->PP_CSW_background,MUIA_Imagedisplay_Spec,&popimage_background);
    getmacro((Object*)WookieChat->PP_CSW_background,MUIA_Pendisplay_Spec,&pendisplay_specs[1]);
    strncpy(pendisplay_specs[1]->buf, (char *)popimage_background,32);
    if(pendisplay_specs[1]->buf[0]=='2')
        strncpy(pendisplay_specs[1]->buf,pendisplay_specs[1]->buf+2,32);

    getmacro((Object*)WookieChat->PP_CSW_nicklistbackground,MUIA_Imagedisplay_Spec,&popimage_nicklistbackground);
    getmacro((Object*)WookieChat->PP_CSW_nicklistbackground,MUIA_Pendisplay_Spec,&pendisplay_specs[2]);
    strncpy(pendisplay_specs[2]->buf,(char *)popimage_nicklistbackground,32);
    if(pendisplay_specs[2]->buf[0]=='2')
        strncpy(pendisplay_specs[2]->buf,pendisplay_specs[2]->buf+2,32);

    getmacro((Object*)WookieChat->PP_CSW_listviewtabs_background,MUIA_Imagedisplay_Spec,&popimage_tabsbackground);
    getmacro((Object*)WookieChat->PP_CSW_listviewtabs_background,MUIA_Pendisplay_Spec,&pendisplay_specs[23]);
    strncpy(pendisplay_specs[23]->buf,(char *)popimage_tabsbackground,32);
    if(pendisplay_specs[23]->buf[0]=='2')
        strncpy(pendisplay_specs[23]->buf,pendisplay_specs[23]->buf+2,32);


    custom_pen_colours[0]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[1],0);
    custom_pen_colours[1]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[2],0);
    custom_pen_colours[2]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[3],0);
    custom_pen_colours[3]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[4],0);
    custom_pen_colours[4]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[5],0);
    custom_pen_colours[5]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[6],0);
    custom_pen_colours[6]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[7],0);
    custom_pen_colours[7]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[8],0);
    custom_pen_colours[8]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[9],0);
    custom_pen_colours[9]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[10],0);
    custom_pen_colours[10]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[11],0);
    custom_pen_colours[11]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[12],0);
    custom_pen_colours[12]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[13],0);
    custom_pen_colours[13]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[14],0);
    custom_pen_colours[14]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[15],0);
    custom_pen_colours[15]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[16],0);
    custom_pen_colours[16]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[17],0);
    custom_pen_colours[17]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[18],0);
    custom_pen_colours[18]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[19],0);
    custom_pen_colours[19]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[20],0);
    custom_pen_colours[20]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[21],0);
    custom_pen_colours[21]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[22],0);
    custom_pen_colours[22]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[23],0);
    custom_pen_colours[23]= MUI_ObtainPen(muiRenderInfo(obj),pendisplay_specs[24],0);

    strncpy(pendisplay_specs[1]->buf,(char *)popimage_background,32);
    strncpy(pendisplay_specs[2]->buf,(char *)popimage_nicklistbackground,32);
    strncpy(pendisplay_specs[23]->buf,(char *)popimage_tabsbackground,32);

    setup_background_colours();


    if (!DoSuperMethodA(cl,obj,(Msg)msg))
        return(FALSE);

    return(TRUE);
}

SAVEDS ULONG Group_Cleanup(struct IClass *cl,Object *obj,struct Message *msg)
{
//    struct MyData *data = (MyData*)INST_DATA(cl,obj);

    //printf("cleanup..\n");

    //printf("Group_Cleanup..\n");

    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[0]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[1]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[2]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[3]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[4]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[5]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[6]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[7]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[8]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[9]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[10]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[11]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[12]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[13]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[14]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[15]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[16]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[17]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[18]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[19]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[20]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[21]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[22]);
    MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[23]);

    return(DoSuperMethodA(cl,obj,(Msg)msg));
}


#ifdef __amigaos4__
DISPATCHERPROTO(Group_Dispatcher)
{
#elif __MORPHOS__
DISPATCHER(Group_Dispatcher)
{
#elif __AROS__
ULONG Group_Dispatcher(Class *cl, Object *obj, Msg msg)
{
#else
ULONG Group_Dispatcher(void)
{
  register struct IClass *a0 __asm("a0"); struct IClass *cl = a0;
  register Object *a2 __asm("a2");        Object *obj = a2;
  register Msg a1 __asm("a1");            Msg msg = a1;
#endif

    switch (msg->MethodID)
    {
        //case OM_NEW        : return(PopPen_New      (cl,obj,(struct Message*)msg));
        //case OM_DISPOSE    : return(PopPen_Dispose  (cl,obj,(struct Message*)msg));
        //case OM_SET        : return(PopPen_Set      (cl,obj,(struct Message*)msg));
        //case OM_GET        : return(PopPen_Get      (cl,obj,(struct Message*)msg));
        //case MUIM_AskMinMax: return(PopPen_AskMinMax(cl,obj,(MUIP_AskMinMax*)msg));
        case MUIM_Setup    : return(Group_Setup    (cl,obj,(struct Message*)msg));
        case MUIM_Cleanup  : return(Group_Cleanup  (cl,obj,(struct Message*)msg));
        //case MUIM_Draw     : return(PopPen_Draw     (cl,obj,(MUIP_Draw*)msg));
    }

    //experimental 2008-11-18
    return(DoSuperMethodA(cl,obj,msg));
    //if (!DoSuperMethodA(cl,obj,(Msg)msg))
    //    return(FALSE);
}


//
// Window Subclassing stuff below
//

#ifndef __AROS__
#define MUIA_Window_RootObject              0x8042cba5 /* V4  isg Object *          */
#endif

SAVEDS ULONG Window_Setup(struct IClass *cl,Object *obj,struct Message *msg)
{
    struct InstanceData *data = (struct InstanceData*)INST_DATA(cl,obj);
    
    if (!DoSuperMethodA(cl,obj,(Msg)msg))
        return(FALSE);

    //printf("setup..\n");

    /* initialize and add window input event handler */
    data->ehnode.ehn_Object = obj;
    data->ehnode.ehn_Class = cl;
    data->ehnode.ehn_Events = IDCMP_MOUSEBUTTONS;

    DoMethod(obj,MUIM_Window_AddEventHandler,&data->ehnode);

    return(TRUE);
}

SAVEDS ULONG Window_Cleanup(struct IClass *cl,Object *obj,struct Message *msg)
{
      
    //printf("cleanup..\n");
    
    //MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[2]);

    //MUI_ReleasePen(muiRenderInfo(WookieChat->PP_CSW_join),custom_pen_colours[2]);

    struct InstanceData *data = (struct InstanceData*)INST_DATA(cl,obj);

    DoMethod(obj,MUIM_Window_RemEventHandler,&data->ehnode);

    return(DoSuperMethodA(cl,obj,(Msg)msg));
}

SAVEDS ULONG Window_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{

    //printf("drawing..\n");

    /*
    ** let our superclass draw itself first, area class would
    ** e.g. draw the frame and clear the whole region. What
    ** it does exactly depends on msg->flags.
    */

    DoSuperMethodA(cl,obj,(Msg)msg);

    /*
    ** if MADF_DRAWOBJECT isn't set, we shouldn't draw anything.
    ** MUI just wanted to update the frame or something like that.
    */


    //MUI_ReleasePen(muiRenderInfo(WookieChat->PP_CSW_join),custom_pen_colours[2]);

    //custom_pen_colours[2] = MUI_ObtainPen(muiRenderInfo((Object*)WookieChat->PP_CSW_join),(MUI_PenSpec*)&background_pen3,0);

    getmacro((Object*)status_current->current_query->STR_topic,MUIA_Selected,&string1);
    getmacro((Object*)status_current->current_query->STR_keyword,MUIA_Selected,&string2);
    getmacro((Object*)status_current->current_query->STR_limit,MUIA_Selected,&string3);

    //if(!string1 && !string2 && !string3) printf("lkjasdf\n");
    
    return(0);
}

#ifndef __AROS__
#define MUIM_Window_Cleanup 0x8042ab26 /* private */ /* V18 */
#define MUIM_Window_Setup   0x8042c34c /* private */ /* V18 */
#endif

//struct MUIP_Window_Cleanup { ULONG MethodID; }; /* private */
//struct MUIP_Window_Setup   { ULONG MethodID; }; /* private */

#ifdef __amigaos4__
DISPATCHERPROTO(Window_Dispatcher)
{
#elif __MORPHOS__
DISPATCHER(Window_Dispatcher)
{
#elif __AROS__
//DISPATCHERPROTO(Window_Dispatcher)
ULONG Window_Dispatcher(Class *cl, Object *obj, Msg msg)
{
#else
ULONG Window_Dispatcher(void)
{
  register struct IClass *a0 __asm("a0"); struct IClass *cl = a0;
  register Object *a2 __asm("a2");        Object *obj = a2;
  register Msg a1 __asm("a1");            Msg msg = a1;
#endif

    switch (msg->MethodID)
    {
        /*case OM_NEW        : return(penNew      (cl,obj,(struct Message*)msg));
        case OM_DISPOSE    : return(penDispose  (cl,obj,(struct Message*)msg));
        case OM_SET        : return(penSet      (cl,obj,(struct Message*)msg));
        case OM_GET        : return(penGet      (cl,obj,(struct Message*)msg));
        case MUIM_AskMinMax: return(penAskMinMax(cl,obj,(MUIP_AskMinMax*)msg));
        case MUIM_Setup    : return(penSetup    (cl,obj,(struct Message*)msg));
        case MUIM_Cleanup  : return(penCleanup  (cl,obj,(struct Message*)msg));
        case MUIM_Draw     : return(penDraw     (cl,obj,(MUIP_Draw*)msg));*/

        //case OM_NEW        : return(wNew      (cl,obj,(struct Message*)msg));
        case MUIM_Window_Setup    : return(Window_Setup    (cl,obj,(struct Message*)msg));
        case MUIM_Window_Cleanup  : return(Window_Cleanup  (cl,obj,(struct Message*)msg));
        //case MUIM_Draw            : return(wDraw     (cl,obj,(MUIP_Draw*)msg));
        //case MUIM_HandleEvent     : return(Window_HandleEvent(cl,obj,(struct MUIP_HandleEvent*)msg));

    }

    return(DoSuperMethodA(cl,obj,msg));
}
