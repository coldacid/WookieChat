/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/* NLIST FOR DCC WINDOW STUFF FOLLOWS */



struct dcc_entry
{
    char status[50];
    char nick[90];
    char filename[200];
    char cps[20];
    char bytes_transferred[20];
    char filesize[20];
    char percentage[10];
    char timeleft[50];
    char port[20];
};

#ifdef __amigaos4__
struct dcc_entry * ConstructDCC_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, dcc_entry *new_entry)) {
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

const struct Hook ConstructDCC_TextHook = { { NULL,NULL }, (ULONG(*)())ConstructDCC_TextFunc, NULL,NULL };





  #ifdef __amigaos4__
static void DestructDCC_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, dcc_entry *new_entry)) {
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


const struct Hook DestructDCC_TextHook = { { NULL,NULL },(ULONG(*)())DestructDCC_TextFunc, NULL,NULL };



  #ifdef __amigaos4__
static void DisplayDCC_send_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct dcc_entry *a_entry)) {
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

        array[0]= "Status";
        array[1]= "Nick";
        array[2]= "Filename";
        array[3]= "KB/sec";
        array[4]= "Sent";
        array[5]= "File size";
        array[6]= "%";
        array[7]= "Time Left";

    }

}

const struct Hook DisplayDCC_send_TextHook = { { NULL,NULL },(ULONG(*)())DisplayDCC_send_TextFunc, NULL,NULL };



  #ifdef __amigaos4__
static void DisplayDCC_recv_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct dcc_entry *a_entry)) {
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

        array[0]= "Status";
        array[1]= "Nick";
        array[2]= "Filename";
        array[3]= "KB/sec";
        array[4]= "Recieved";
        array[5]= "File size";
        array[6]= "%";
        array[7]= "Time Left";

    }

}

const struct Hook DisplayDCC_recv_TextHook = { { NULL,NULL },(ULONG(*)())DisplayDCC_recv_TextFunc, NULL,NULL };



/* BETTER STRING STUFF FOLLOWS */

struct InstanceData
{

struct MUI_EventHandlerNode ehnode; /* input event handler*/

};

APTR my_object;


ULONG NList_HandleInput(struct IClass *cl, Object *obj, struct MUIP_HandleInput *msg)
{
    //printf("handle event\n");

    //struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);
	//set((Object*)James->WI_main,MUIA_Window_ActiveObject,(Object*)James->STR_usertext);

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
                            set(status_current->current_query->LV_channel,MUIA_NList_Quiet,TRUE);
                            printf("select down\n");
                            break;

                        case SELECTUP:
                            set(status_current->current_query->LV_channel,MUIA_NList_Quiet,FALSE);
                            printf("select up\n");


                            //DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_GetSelectInfo,res);
                            //if(res->start == -1 && res->end == -1 && !res->vnum)
                            //{
                          //  	  set((Object*)James->WI_main,MUIA_Window_ActiveObject,(Object*)James->STR_usertext);
                          //      break;
                          //  }
                          //  else
                          //  {
                                //DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_CopyToClip, MUIV_NList_CopyToClip_Selected, 0, NULL, NULL);
                                //DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_Select,MUIV_NList_Select_All, MUIV_NList_Select_Off, NULL);
                          //  	  set((Object*)James->WI_main,MUIA_Window_ActiveObject,(Object*)James->STR_usertext);
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
                    get((Object*)status_conductor->conductor->LV_channel,MUIA_NList_Visible, &visible);

                    if(status_conductor->conductor==status_current->current_query)
                    {
                        get((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Visible, &visible);

                        if(visible > 0)
                        {
                            get((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Entries, &entries);
                            get((Object*)status_conductor->current_query->LV_channel,MUIA_NList_First, &first);
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
    struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);
    //printf("handle event\n");

    get((Object*)James->WI_main,MUIA_Window_ActiveObject,&my_object1);
    if(status_current->current_query->name[0]=='#')
    {
        if(my_object1!=status_current->current_query->STR_topic &&
            my_object1!=status_current->current_query->STR_keyword &&
            my_object1!=status_current->current_query->STR_limit)
        	set((Object*)James->WI_main,MUIA_Window_ActiveObject,(Object*)James->STR_usertext);
    }
    else
    {
        if(my_object1!=status_current->current_query->STR_topic)
        	set((Object*)James->WI_main,MUIA_Window_ActiveObject,(Object*)James->STR_usertext);

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

                        break;

                        case SELECTUP:
                                //printf("select up\n");
                                selectup_code();
                            /*if(user_is_trying_to_select_text==TRUE)
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
                                            get((Object*)status_conductor->conductor->LV_channel,MUIA_NList_Visible, &visible);

                                            if(status_conductor->conductor==status_current->current_query)
                                            {
                                                get((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Visible, &visible);

                                                if(visible > 0)
                                                {
                                                    get((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Entries, &entries);
                                                    get((Object*)status_conductor->current_query->LV_channel,MUIA_NList_First, &first);
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


                            } */

                            //return MUI_EventHandlerRC_Eat; //we want to eat the event

                            break;

                        default:
                            break;

                    }

                    break;



        }
    }

    #endif

    //if (!DoSuperMethodA(cl,obj,(Msg)msg))
    //    return(FALSE);

    //return MUI_EventHandlerRC_Eat; //we want to eat the event

    return 0;
}



ULONG NList_ContextMenu(struct IClass *cl, Object *obj, struct MUIP_ContextMenuChoice *msg)
{

    //struct list_entry *work_entry = new list_entry;
    char work[200];

    for(LONG id = MUIV_NList_NextSelected_Start;;)
    {

    DoMethod((Object*)obj,MUIM_NList_NextSelected,&id);
    DoMethod((Object*)obj,MUIM_NList_GetEntry,id, &work_entry);
    if(id == MUIV_NList_NextSelected_End) break;

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
            break;

        case 67: // GIVE OPS
            sprintf(work,"/raw MODE %s +o %s",status_current->current_query->name, work_entry->name);
            process_outgoing(work,0);
            break;

        case 68: // TAKE OPS
              sprintf(work,"/raw MODE %s -o %s",status_current->current_query->name, work_entry->name);
            process_outgoing(work,0);
            break;


        case 69: // GIVE HALFOPS
              sprintf(work,"/raw MODE %s +h %s",status_current->current_query->name, work_entry->name);
            process_outgoing(work,0);
            break;

        case 70: // TAKE HALFOPS
              sprintf(work,"/raw MODE %s -h %s",status_current->current_query->name, work_entry->name);
            process_outgoing(work,0);
            break;

        case 71: // GIVE VOICE
              sprintf(work,"/raw MODE %s +v %s",status_current->current_query->name, work_entry->name);
            process_outgoing(work,0);
            break;

        case 72: // TAKE VOICE
              sprintf(work,"/raw MODE %s -v %s",status_current->current_query->name, work_entry->name);
            process_outgoing(work,0);
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
        	set((Object*)James->WI_main,MUIA_Window_ActiveObject,(Object*)James->STR_usertext);
            break;

        case 79: //bring up dcc send window
            if(work_entry->name) set((Object*)James->STR_dcc_send_nick,MUIA_String_Contents,work_entry->name);
                else set((Object*)James->STR_dcc_send_nick,MUIA_String_Contents,"");
            set((Object*)James->WI_dcc_send,MUIA_Window_Open, TRUE);
            break;

        case 81:
            if(work_entry->name) initiate_outgoing_dcc_chat(work_entry->name);
            break;

        case 80: //open private messages tab, all private messages will go here
            status_conductor=status_current;
            SQuery("!global");
            break;

        case 83: //opening ban window dialog
            //if(work_entry->hostname) printf("nick:%s hostname:%s\n", work_entry->name, work_entry->hostname);
            //else printf("nick:%s\n", work_entry->name);
            status_conductor=status_current;
            status_conductor->conductor=current_query;

            for(int count=0; count<status_conductor->conductor->nicks; count++)
            {
                //printf("count:%i\n", count);

                if(!stricmp(work_entry->name,status_conductor->conductor->nicklist[count].name))
                {
                    if(status_conductor->conductor->nicklist[count].hostname)
                    printf("set ban on user:%s with the hostname:%s\n",status_conductor->conductor->nicklist[count].name,status_conductor->conductor->nicklist[count].hostname);
                    break;
                }

            }

            break;

    }

    }

    return 0;
}


ULONG NList_Setup(struct IClass *cl, Object *obj, struct Message *msg)
{

    struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,(Msg)msg))
    {
        return(FALSE);
    }


    /* initialize and add window input event handler */
    #ifdef __amigaos4__
    data->ehnode.ehn_Object = obj;
    data->ehnode.ehn_Class = cl;
    data->ehnode.ehn_Events = IDCMP_MOUSEBUTTONS;

    DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->ehnode);
    #endif

    return(TRUE);
}

ULONG NList_Cleanup(struct IClass *cl,Object *obj, struct Message *msg)
{
    struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);

    if(!DoSuperMethodA(cl,obj,(Msg)msg)) return(FALSE);

    #ifdef __amigaos4__
    DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->ehnode);
    #endif

    return(TRUE);
}



extern "C"
{

#ifdef __amigaos4__
DISPATCHERPROTO(NList_Dispatcher)
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

        //case OM_NEW:                 return roster_new(cl, obj, (struct Message*)msg);
        case MUIM_ContextMenuChoice: return(NList_ContextMenu(cl,obj,(struct MUIP_ContextMenuChoice*)msg));
        case MUIM_HandleEvent:       return(NList_HandleEvent(cl,obj,(struct MUIP_HandleEvent*)msg));
        case MUIM_HandleInput:       return(NList_HandleInput(cl,obj,(struct MUIP_HandleInput*)msg));
        //case MUIM_CustomBackfill: return(cCustomBackfill(cl,obj, (struct MUIP_CustomBackfill*)msg));
    }

    return(DoSuperMethodA(cl,obj,msg));
}

}

ULONG BetterString_HandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg);
ULONG BetterString_Setup(struct IClass *cl,Object *obj, struct Message *msg);
ULONG BetterString_Cleanup(struct IClass *cl,Object *obj, struct Message *msg);


extern "C"
{

#ifdef __amigaos4__
DISPATCHERPROTO(BetterString_Dispatcher)
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

}



ULONG BetterString_Setup(struct IClass *cl,Object *obj,struct Message *msg)
{
    struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);

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
    struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);

    /* remove user input handler */
    DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->ehnode);

    return(DoSuperMethodA(cl,obj,(Msg)msg));
}

BOOL ready_to_make_chat_gadget_active=FALSE;


//nick completion
ULONG BetterString_HandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{

    //printf("betterstring handle event\n");

    if (msg->imsg)
    {
        switch (msg->imsg->Class)
        {

            case IDCMP_RAWKEY:

            //printf("Qualifier rawkey: %02lx\n",msg->imsg->Qualifier);

            switch(msg->imsg->Qualifier)
            {
                case 0x8080:
                    //printf("right amiga qualifier hit: %02lx\n",msg->imsg->Qualifier);

                    switch (msg->imsg->Code)
                    {
                        default:
                            int a=msg->imsg->Code;
                            if(a<=9)
                            {
                                switch_between_tabs_with_keys(a);
                                set(current_query->BT_querybutton,MUIA_Selected,TRUE);
                                //if(DEBUG) printf("finished switching tabs with keys\n");
                                return MUI_EventHandlerRC_Eat; //<- when we want to eat the event

                            }
                            break;

                    }

                    break;

            }

            switch(msg->imsg->Code)
            {

                    /*char *tabwork_string; //original string gadget contents
                    char *tabwork2_string; //text before cursor position
                    char *tabwork3_string; //text after cursor position
                    char *tabwork4_string; //nick text to complete
                    char *tabwork5_string;*/


                    ULONG position;
                    int length;
                    int typed_length;
                    int begin_tab;
                    int a,b,c,d,found_nicks;

                case CURSORUP:
                    if(status_current->current_query->string_conductor)
                    {

                        if(status_current->current_query->string_conductor->previous)
                        {
                            status_current->current_query->string_conductor=status_current->current_query->string_conductor->previous;
                            set((Object*)James->STR_usertext,MUIA_String_Contents,status_current->current_query->string_conductor->buffer_history);

                        }

                    }

                    nickcomp_state=0;
                    break;

                case CURSORDOWN:
                    if(status_current->current_query->string_conductor)
                    {

                        if(status_current->current_query->string_conductor->next)
                        {
                            status_current->current_query->string_conductor=status_current->current_query->string_conductor->next;
                            set((Object*)James->STR_usertext,MUIA_String_Contents,status_current->current_query->string_conductor->buffer_history);
                        }
                        else set((Object*)James->STR_usertext,MUIA_String_Contents,"");

                    }
                    nickcomp_state=0;
                    break;


                case 0xC2: // TAB has been pressed, now do nick completion stuff

                    /*tabwork_string //original string gadget contents
                    tabwork2_string  //text before cursor position
                    tabwork3_string  //text after cursor position
                    tabwork4_string  //nick text to complete
                    tabwork5_string  //completed nick text to insert
                     */

                    get((Object*)James->STR_usertext,MUIA_String_BufferPos,&position);
                    get((Object*)James->STR_usertext,MUIA_String_Contents,&tabwork_string);

                    length=strlen(tabwork_string);
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
                        for(a=0; a!=11; a++) strcpy(list_found_nicks[a],""); //wipe the list of partially found nicks

                        d=0; found_nicks=0; a=0;

                        for(d=0;d<status_current->current_query->nicks;d++)
                        {
                            if(strnicmp((p_in)tabwork4_string,(p_in)status_current->current_query->nicklist[d].name,strlen(tabwork4_string)) == 0)
                            {
                                strcpy(list_found_nicks[a],status_current->current_query->nicklist[d].name);
                                if(!found_nicks) strcpy(tabwork5_string,list_found_nicks[a]);
                                a++;

                                //printf("tabwork5_string:%s\n",tabwork5_string);

                                found_nicks=1;
                            }
                        }

                        strcpy(list_found_nicks[a],""); a=0;

                        nickcomp_state=1;
                    }

                    if(found_nicks==0) break;



                    //printf("position-typed_length:%i\n",(position-typed_length));

                    if(my_settings.nick_completion==AMIRC_STYLE)
                    {
                        if((position-typed_length)==1 || (position-typed_length)==-1 )
                        {
                            if((position-typed_length)==-1)
                            {
                                position+=2;
                                //for(a=0; a<strlen(tabwork5_string); a++) tabwork5_string[a]=tabwork5_string[a+1];
                            }

                            sprintf(tabwork_string,"%s%s: %s",tabwork2_string,tabwork5_string,tabwork3_string);
                            set((Object*)James->STR_usertext,MUIA_String_Contents,tabwork_string);
                            position=position-typed_length+strlen(tabwork5_string)+2;

                        }
                        else
                        {
                            sprintf(tabwork_string,"%s%s %s",tabwork2_string,tabwork5_string,tabwork3_string);
                            set((Object*)James->STR_usertext,MUIA_String_Contents,tabwork_string);
                            position=position-typed_length+strlen(tabwork5_string)+1;
                        }
                    }
                    else
                    {
                        sprintf(tabwork_string,"%s%s%s",tabwork2_string,tabwork5_string,tabwork3_string);
                        set((Object*)James->STR_usertext,MUIA_String_Contents,tabwork_string);
                        position=position-typed_length+strlen(tabwork5_string);

                    }

                    set((Object*)James->STR_usertext,MUIA_String_BufferPos,position);


                    //return MUI_EventHandlerRC_Eat; //<- when we want to eat the event

                    break;

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

struct MyData
{
	struct MUI_PenSpec penspec;
	LONG pen;
	BOOL penchange;
    int requestchange;
};



SAVEDS ULONG PopPen_New(struct IClass *cl,Object *obj,struct Message *msg)
{
	struct MyData *data;
	struct TagItem *tags,*tag;

    //printf("new..\n");

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg)))
		return(0);

	data = (MyData*)INST_DATA(cl,obj);

	/* parse initial taglist */


	for (tags=((struct opSet *)msg)->ops_AttrList;tag=NextTagItem(&tags);)
	{
		switch (tag->ti_Tag)
		{

            case MYATTR_PEN:
				if (tag->ti_Data)
                    printf("1\n");
					//data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
                    background_pen1 = ((struct MUI_PenSpec *)tag->ti_Data);
                break;
			case MYATTR_PEN2:
                    printf("2\n");
				if (tag->ti_Data)
                    background_pen2 = ((struct MUI_PenSpec *)tag->ti_Data);
                break;

			case MYATTR_PEN3:
                    printf("3\n");
				if (tag->ti_Data)
                    background_pen3 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN4:
				if (tag->ti_Data)
                    background_pen4 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN5:
				if (tag->ti_Data)
                    background_pen5 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN6:
				if (tag->ti_Data)
                    background_pen6 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN7:
				if (tag->ti_Data)
                    background_pen7 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN8:
				if (tag->ti_Data)
                    background_pen8 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN9:
				if (tag->ti_Data)
                    background_pen9 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN10:
				if (tag->ti_Data)
                    background_pen10 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN11:
				if (tag->ti_Data)
                    background_pen11 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN12:
				if (tag->ti_Data)
                    background_pen12 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN13:
				if (tag->ti_Data)
                    background_pen13 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN14:
				if (tag->ti_Data)
                    background_pen14 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN15:
				if (tag->ti_Data)
                    background_pen15 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN16:
				if (tag->ti_Data)
                    background_pen16 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;


            case MYATTR_PEN17:
				if (tag->ti_Data)
                    background_pen17 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN18:
				if (tag->ti_Data)
                    background_pen18 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN19:
				if (tag->ti_Data)
                    background_pen19 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN20:
				if (tag->ti_Data)
                    background_pen20 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

			case MYATTR_PEN21:
				if (tag->ti_Data)
                    background_pen21 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;

            case MYATTR_PEN22:
				if (tag->ti_Data)
                    background_pen22 = ((struct MUI_PenSpec *)tag->ti_Data);
				break;


		}

	}


	return((ULONG)obj);
}



SAVEDS ULONG PopPen_Dispose(struct IClass *cl,Object *obj,struct Message *msg)
{
	/* OM_NEW didnt allocates something, just do nothing here... */

    //printf("dispose..\n");
    return(DoSuperMethodA(cl,obj,(Msg)msg));
}


/*
** OM_SET method, we need to see if someone changed the penspec attribute.
*/

SAVEDS ULONG PopPen_Set(struct IClass *cl,Object *obj,struct Message *msg)
{
	struct MyData *data = (MyData*)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

    //printf("setting pen colours..\n");

    for (tags=((struct opSet *)msg)->ops_AttrList;tag=NextTagItem(&tags);)
	{
		switch (tag->ti_Tag)
		{

			case MYATTR_PEN:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
                    data->penchange = TRUE;
                    data->requestchange = 1;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN2:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 2;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN3:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 3;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN4:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 4;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN5:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 5;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN6:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 6;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN7:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 7;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN8:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 8;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN9:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 9;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN10:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 10;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN11:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 11;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN12:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 12;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN13:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 13;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN14:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 14;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN15:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 15;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN16:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 16;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN17:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 17;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN18:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 18;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN19:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 19;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN20:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 20;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN21:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 21;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;

			case MYATTR_PEN22:
				if (tag->ti_Data)
				{
					data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
					data->penchange = TRUE;
                    data->requestchange = 22;
                    MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely

				}
				break;


		}
	}

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}


/*
** OM_GET method, see if someone wants to read the color.
*/

static ULONG PopPen_Get(struct IClass *cl,Object *obj,struct Message *msg)
{
	struct MyData *data = (MyData*)INST_DATA(cl,obj);
	ULONG *store = ((struct opGet *)msg)->opg_Storage;

    //printf("get..\n");

	switch (((struct opGet *)msg)->opg_AttrID)
	{


		case MYATTR_PEN:
        case MYATTR_PEN2: 
		case MYATTR_PEN3: 
		case MYATTR_PEN4: 
		case MYATTR_PEN5: 
		case MYATTR_PEN6: 
		case MYATTR_PEN7: 
		case MYATTR_PEN8: 
		case MYATTR_PEN9: 
		case MYATTR_PEN10: 
		case MYATTR_PEN11: 
		case MYATTR_PEN12: 
		case MYATTR_PEN13: 
		case MYATTR_PEN14: 
		case MYATTR_PEN15: 
		case MYATTR_PEN16: 
		case MYATTR_PEN17: 
		case MYATTR_PEN18: 
		case MYATTR_PEN19: 
		case MYATTR_PEN20: 
		case MYATTR_PEN21: 
		case MYATTR_PEN22:
            *store = (ULONG)&data->penspec; return(TRUE);


    }


	return(DoSuperMethodA(cl,obj,(Msg)msg));
}


SAVEDS ULONG PopPen_Setup(struct IClass *cl,Object *obj,struct Message *msg)
{
	struct MyData *data = (MyData*)INST_DATA(cl,obj);

    //printf("setup..\n");

    //data->pen = MUI_ObtainPen(muiRenderInfo(obj),&data->penspec,0);

    //custom_pen_colours[2] = MUI_ObtainPen(muiRenderInfo(obj),(MUI_PenSpec*)&background_pen3,0);

	if (!DoSuperMethodA(cl,obj,(Msg)msg))
		return(FALSE);

    return(TRUE);
}


SAVEDS ULONG PopPen_Cleanup(struct IClass *cl,Object *obj,struct Message *msg)
{
	struct MyData *data = (MyData*)INST_DATA(cl,obj);

    //printf("cleanup..\n");

	//MUI_ReleasePen(muiRenderInfo(obj),data->pen);

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}


SAVEDS ULONG PopPen_AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	/*
	** let our superclass first fill in what it thinks about sizes.
	** this will e.g. add the size of frame and inner spacing.
	*/

	DoSuperMethodA(cl,obj,(Msg)msg);

	/*
	** now add the values specific to our object. note that we
	** indeed need to *add* these values, not just set them!
	*/

	msg->MinMaxInfo->MinWidth  += 80;
	msg->MinMaxInfo->DefWidth  += 120;
	msg->MinMaxInfo->MaxWidth  += 500;

	msg->MinMaxInfo->MinHeight += 7;
	msg->MinMaxInfo->DefHeight += 90;
	msg->MinMaxInfo->MaxHeight += 300;

	return(0);
}


/*
** Draw method is called whenever MUI feels we should render
** our object. This usually happens after layout is finished
** or when we need to refresh in a simplerefresh window.
** Note: You may only render within the rectangle
**       _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj).
*/

SAVEDS ULONG PopPen_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{

	//printf("drawing..\n");

    struct MyData *data = (MyData*)INST_DATA(cl,obj);
	int i;

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

	if (!(msg->flags & MADF_DRAWOBJECT))
		return(0);

	/*
	** test if someone changed our pen
	*/

	if (data->penchange)
	{
		data->penchange = FALSE;
		MUI_ReleasePen(muiRenderInfo(obj),data->pen);
		data->pen = MUI_ObtainPen(muiRenderInfo(obj),&data->penspec,0);

        data->requestchange = data->requestchange - 1;

        if(data->requestchange >= 0 && data->requestchange <= 21)
            custom_pen_colours[data->requestchange] = data->pen;

        MUI_Redraw(obj,MADF_DRAWOBJECT); // redraw ourselves completely
    }

	/*
	** ok, everything ready to render...
	** Note that we *must* use the MUIPEN() macro before actually
	** using pens from MUI_ObtainPen() in rendering calls.
	*/

	SetAPen(_rp(obj),MUIPEN(data->pen));


    RectFill(_rp(obj), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj));



	return(0);
}





/*
** Here comes the dispatcher for our custom class. We only need to
** care about MUIM_AskMinMax and MUIM_Draw in this simple case.
** Unknown/unused methods are passed to the superclass immediately.
*/

extern "C"
{

#ifdef __amigaos4__
DISPATCHERPROTO(PopPen_Dispatcher)
{

#else
ULONG PopPen_Dispatcher(void)
{
  register struct IClass *a0 __asm("a0"); struct IClass *cl = a0;
  register Object *a2 __asm("a2");        Object *obj = a2;
  register Msg a1 __asm("a1");            Msg msg = a1;
#endif

	switch (msg->MethodID)
	{
		case OM_NEW        : return(PopPen_New      (cl,obj,(struct Message*)msg));
		case OM_DISPOSE    : return(PopPen_Dispose  (cl,obj,(struct Message*)msg));
		case OM_SET        : return(PopPen_Set      (cl,obj,(struct Message*)msg));
		case OM_GET        : return(PopPen_Get      (cl,obj,(struct Message*)msg));
		case MUIM_AskMinMax: return(PopPen_AskMinMax(cl,obj,(MUIP_AskMinMax*)msg));
		case MUIM_Setup    : return(PopPen_Setup    (cl,obj,(struct Message*)msg));
		case MUIM_Cleanup  : return(PopPen_Cleanup  (cl,obj,(struct Message*)msg));
		case MUIM_Draw     : return(PopPen_Draw     (cl,obj,(MUIP_Draw*)msg));
	}

	return(DoSuperMethodA(cl,obj,msg));
}

}




//
// Window Subclassing stuff below
//

#define MUIA_Window_RootObject              0x8042cba5 /* V4  isg Object *          */

SAVEDS ULONG Window_Setup(struct IClass *cl,Object *obj,struct Message *msg)
{
	//struct MyData *data = (MyData*)INST_DATA(cl,obj);
    struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);
	
    if (!DoSuperMethodA(cl,obj,(Msg)msg))
		return(FALSE);

    //printf("setup..\n");

    /* initialize and add window input event handler */
    data->ehnode.ehn_Object = obj;
    data->ehnode.ehn_Class = cl;
    data->ehnode.ehn_Events = IDCMP_MOUSEBUTTONS;

    DoMethod(obj,MUIM_Window_AddEventHandler,&data->ehnode);



    set((Object*)James->WI_colour_settings,MUIA_Window_Open, TRUE);
    load_colours();


                memset(background,'\0',63); strcpy(background," "); strcat(background,background_pen2->buf);
                background[0]='2'; background[1]=':';
                //printf("background:%s\n",background);

                if(status_current->current_query->LV_nicklist != NULL)
                {
                    set((Object*)status_current->current_query->LV_nicklist,MUIA_NList_ListBackground, (LONG)background);
                }

                memset(background2,'\0',63); strcpy(background2," "); strcat(background2,background_pen1->buf);
                background2[0]='2'; background2[1]=':';
                //printf("background2:%s\n",background2);


                if(status_current->current_query->LV_channel != NULL)
                {
                    set((Object*)status_current->current_query->LV_channel,MUIA_NList_ListBackground, (LONG)background2);
                }


                if(status_current->current_query->LV_nicklist != NULL)
                {
                    set((Object*)status_current->current_query->LV_nicklist,MUIA_NList_ListPen, background_pen22);
                }

                /* Below are the activity colours */

                memset(activity,'\0',63); strcpy(activity," "); strcat(activity,background_pen19->buf);
                activity[0]='2'; activity[1]=':';
                memset(activity_chat,'\0',63); strcpy(activity_chat," "); strcat(activity_chat,background_pen20->buf);
                activity_chat[0]='2'; activity_chat[1]=':';
                memset(activity_highlight,'\0',63); strcpy(activity_highlight," "); strcat(activity_highlight,background_pen21->buf);
                activity_highlight[0]='2'; activity_highlight[1]=':';


    set((Object*)James->WI_colour_settings,MUIA_Window_Open, FALSE);


    DoMethod((Object*)current_query->LV_nicklist,MUIM_NList_Redraw,MUIV_NList_Redraw_All);
    DoMethod((Object*)current_query->LV_channel,MUIM_NList_Redraw,MUIV_NList_Redraw_All);



    return(TRUE);
}

SAVEDS ULONG Window_Cleanup(struct IClass *cl,Object *obj,struct Message *msg)
{
      
    //printf("cleanup..\n");
	
    //MUI_ReleasePen(muiRenderInfo(obj),custom_pen_colours[2]);

	//MUI_ReleasePen(muiRenderInfo(James->PP_CSW_join),custom_pen_colours[2]);

    struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);

    DoMethod(obj,MUIM_Window_RemEventHandler,&data->ehnode);

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}


ULONG Window_HandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
    //printf("window handle event\n");

	//DoSuperMethodA(cl,obj,(Msg)msg);

    //struct InstanceData *data = (InstanceData*)INST_DATA(cl,obj);

    if(status_current)
    {
        LONG string1, string2, string3;

        get((Object*)status_current->current_query->STR_topic,MUIA_Selected,&string1);
        get((Object*)status_current->current_query->STR_keyword,MUIA_Selected,&string2);
        get((Object*)status_current->current_query->STR_limit,MUIA_Selected,&string3);
        //printf("pos1:%i pos2:%i pos3:%i\n",string1,string2,string3);


        /*get((Object*)status_current->current_query->STR_topic,MUIA_Pressed,&string1);
        get((Object*)status_current->current_query->STR_keyword,MUIA_Pressed,&string2);
        get((Object*)status_current->current_query->STR_limit,MUIA_Pressed,&string3);
        */

        /*printf("wHandleEvent\n");
        if(ready_to_make_chat_gadget_active==TRUE)
    	{
            printf("make chat bar selected\n");
            set((Object*)James->WI_main,MUIA_Window_ActiveObject,(Object*)James->STR_usertext);
        }*/

        //if(string1 || string2 || string3) printf("dont select string gadget\n");
        //else printf("select string gadget\n");
    }

	//return(DoSuperMethodA(cl,obj,(Msg)msg));

    return 0;

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


	//MUI_ReleasePen(muiRenderInfo(James->PP_CSW_join),custom_pen_colours[2]);

    //custom_pen_colours[2] = MUI_ObtainPen(muiRenderInfo((Object*)James->PP_CSW_join),(MUI_PenSpec*)&background_pen3,0);

    get((Object*)status_current->current_query->STR_topic,MUIA_Selected,&string1);
    get((Object*)status_current->current_query->STR_keyword,MUIA_Selected,&string2);
    get((Object*)status_current->current_query->STR_limit,MUIA_Selected,&string3);

    if(!string1 && !string2 && !string3) printf("lkjasdf\n");
	
    return(0);
}


#define MUIM_Window_Cleanup 0x8042ab26 /* private */ /* V18 */
#define MUIM_Window_Setup   0x8042c34c /* private */ /* V18 */

//struct MUIP_Window_Cleanup { ULONG MethodID; }; /* private */
//struct MUIP_Window_Setup   { ULONG MethodID; }; /* private */


extern "C"
{

#ifdef __amigaos4__
DISPATCHERPROTO(Window_Dispatcher)
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
        case MUIM_HandleEvent     : return(Window_HandleEvent(cl,obj,(struct MUIP_HandleEvent*)msg));


	}

	return(DoSuperMethodA(cl,obj,msg));
}

}



