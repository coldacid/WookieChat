/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

int create_new_tab(char *name, int show_now, int query_type)
{

    check_if_at_bottom();

	// Make all buttons deselected

    if(DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_InitChange))
    {

    int running1=1;

    if(status_conductor != status_current) show_now=0; //dont show channel on join if viewing different tab

    if(show_now) kill_my_notifys();
    if(show_now) remove_tab_listview();
    if(show_now) current_query=status_conductor->conductor;

    work_status=status_conductor;

	status_conductor->conductor=status_conductor->root;

	while(running1)
	{
		if(show_now)
        {
            if(status_conductor->conductor->BT_querybutton)
            {
                if(status_conductor->conductor->BT_querybutton) setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);

            }

        }
		status_conductor->conductor=status_conductor->conductor->next;
		if(status_conductor->conductor==NULL) { running1=0; }
	}


    //are we opening a tab that is already open?
	for(count2=1,status_conductor=status_root; status_conductor && status_conductor!=work_status; status_conductor=status_conductor->next,count2++);
    for(count=1,status_conductor=work_status,status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next,count++)
    {
        if(!stricmp(status_conductor->conductor->name,name))
        {
            //holy crap, it exists! lets switch to it and forget about opening a new tab
            if(NEWDEBUG) printf("already exists.. switching\n");

            switch_between_tabs(count+(count2*100)+1);

            DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);

            return 0;
        }
    }

    status_conductor=work_status;
	status_conductor->conductor=status_conductor->root;

    while(status_conductor->conductor->next) { status_conductor->conductor=status_conductor->conductor->next; }

    status_conductor->conductor->next=init_conductor(query_type);

    query_window *work=status_conductor->conductor;

    status_conductor->conductor=status_conductor->conductor->next;
    status_conductor->conductor->previous=work;

    if(show_now) free_graphical_smilies(status_current->current_query);

    strcpy(status_conductor->conductor->name,name);
    strcpy(status_conductor->conductor->displayed_name,name);
    status_conductor->conductor->server_tab=0;

    if(name[0]!='#' && name[0] != '&')
    {
        strcpy(status_conductor->conductor->nicklist[0].name,name);
        status_conductor->conductor->nicks=1;
    }

    strcpy(status_conductor->conductor->your_current_mode,"");
	
    //removed, testing smaller tabs
    if(!SMALLTABS && my_settings.use_nlistobject_for_tabs==0)
    {
        if(strlen(status_conductor->conductor->displayed_name)>10)
        {
            status_conductor->conductor->displayed_name[10]='.';
            status_conductor->conductor->displayed_name[11]='.';
            status_conductor->conductor->displayed_name[12]='\0';
        }
    }

    status_conductor->conductor->next=NULL;

    //display the last few lines of logfile
    if(name[0]!='*') display_last_few_lines_of_logfile_conductor();

    // set the logfile to zero so we know its not open yet
    status_conductor->conductor->log_file=0;

    if(name[0]=='$')
    {
        dcc_chat_conductor->conductor=status_conductor->conductor;
    }

   //if its a channel lister, make the channels clickable
    if(name[0]=='*')
    {

        setmacro((Object*)status_conductor->conductor->LV_channel,MUIA_NList_AutoCopyToClip,TRUE);
        setmacro((Object*)status_conductor->conductor->LV_channel,MUIA_NList_Input,FALSE);
        setmacro((Object*)status_conductor->conductor->LV_channel,MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always);

    }

    // Add query button to the button taskbar

    if(DoMethod((Object*)GR_bottom_group,MUIM_Group_InitChange))
	{

        if(DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_InitChange))
    	{
            DoMethod((Object*)status_conductor->GR_server1_buttons, OM_ADDMEMBER, (Object*)status_conductor->conductor->BT_querybutton);

            setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);
            setmacro(status_conductor->conductor->BT_querybutton,MUIA_Text_PreParse, "\033c\0332");

    		DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_ExitChange);
        }

    	DoMethod((Object*)GR_bottom_group,MUIM_Group_ExitChange);

    }

    sprintf(string7,"\033I[2:00000000,00000000,00000000] %s",status_conductor->conductor->displayed_name);
    if(status_conductor->conductor->BT_querybutton)
    {
        setmacro((Object*)status_conductor->conductor->BT_querybutton,MUIA_Text_Contents,string7);
    }

    update_your_nickname_text_label();

	if(show_now)
	{
        if(DEBUG) printf("show now for %s\n",status_conductor->conductor->name);

        if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
        {

            if(status_current)
            {
                if(status_current->current_query)
                {
                    //if(status_current->current_query->GR_conductor)
                        //  setmacro((Object*)status_current->current_query->GR_conductor, MUIA_ShowMe, FALSE);
                }
            }

            status_conductor->current_query=status_conductor->conductor;
            status_current=status_conductor;
            current_query=status_conductor->conductor;
            status_conductor->conductor->activity=0;
            status_conductor->conductor->jump_to_bottom=TRUE;

            remove_tab_listview();
            add_tabs_to_nicklist_group();

            DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_current->current_query->GR_conductor);

            if(status_current)
            {
                if(status_current->current_query)
                {
                    //setmacro((Object*)status_current->current_query->GR_conductor, MUIA_ShowMe, TRUE);
                }
            }

            DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

        }

        DoMethod((Object*)status_conductor->conductor->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

        setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, TRUE);

    }
    else
    {
        if(DEBUG) printf("dont show now for %s\n",status_conductor->conductor->name);

        if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
        {
            if(status_conductor)
            {
                if(status_conductor->conductor)
                {
                    if(status_conductor->conductor->GR_conductor)
                    {
                        DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->conductor->GR_conductor);
                    }
                }
            }
            DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

            setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);
            status_conductor->conductor->jump_to_bottom=TRUE;

       }

    }
    status_conductor->iv++;
    status_conductor->conductor->iv=status_conductor->iv;

    DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,status_conductor->iv);

    //channel_entry nlist_tab;
    if(ZUNE_SYSTEM==TRUE)
        strcpy(status_conductor->conductor->nlist_tab.entry,"         ");
    else
        strcpy(status_conductor->conductor->nlist_tab.entry,"         :");

    status_conductor->conductor->nlist_tab.colour=23;
    strcat(status_conductor->conductor->nlist_tab.entry,status_conductor->conductor->displayed_name);

    give_each_tab_a_page_group_number();

    give_each_tab_a_listview_number_for_switching_tabs();

    if(status_conductor)
    {
        if(status_conductor->conductor)
            DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_InsertSingle,&status_conductor->conductor->nlist_tab,status_conductor->conductor->nlist_tab_number); //count);
    }

    status_conductor=work_status;

    if(show_now)
	{
        setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_Active,status_current->current_query->nlist_tab_number);
        setmacro((Object*)GR_top,MUIA_Group_ActivePage,status_current->current_query->page_number);
    }

    //get to the last conductor that we just created
    for(status_conductor=work_status, status_conductor->conductor=status_conductor->root; status_conductor->conductor->next; status_conductor->conductor=status_conductor->conductor->next);

	setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);

    RECENTLY_CREATED_A_TAB=TRUE;

    DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);
    }


	return 0;

}


//this will create a new Group to put our tabs in for that server, and also, create an initial Status/Root tab
int create_new_status(int first)
{

    int a=1;
    check_if_at_bottom();

    if(DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_InitChange))
    {


    //status_window *status_work=new status_window;
    //status_work=status_conductor;

    status_conductor=status_root;

    if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
	{

        /*if(ZUNE_SYSTEM==TRUE)
        {
            if(status_current)
            {
                if(status_current->current_query->GR_conductor)
                	DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)status_current->current_query->GR_conductor);
            }
        }
        else*/
        {
            if(status_current)
            {
                //if(status_current->current_query->GR_conductor) setmacro((Object*)status_current->current_query->GR_conductor, MUIA_ShowMe, FALSE);
                /*if(status_current->current_query->GR_conductor)
                {
                    setmacro((Object*)GR_top,MUIA_Group_ActivePage,status_current->current_query->GR_conductor);
                } */


            }


        }

        if(first)
        {
            status_root=new status_window;
            status_conductor=new status_window;

            if(my_settings.no_server_tabs)
            {
                status_conductor->root=init_conductor(1);
                status_conductor->root->server_tab=0;

            }
            else
            {
                status_conductor->root=init_conductor(2);
                status_conductor->root->server_tab=1;
             }

            //setmacro((Object*)WookieChat->TX_nickname,MUIA_Text_Contents,status_conductor->nick);

            status_conductor->root->previous=NULL; //test
            status_conductor->next=NULL;
            status_conductor->conductor=status_conductor->root;

            status_conductor->nick=new char[100];
            strcpy(status_conductor->nick,"(nick)");
            strcpy(status_conductor->conductor->your_current_mode,"");

            update_your_nickname_text_label();


            status_current=status_conductor;
            current_query=status_conductor->conductor;

            status_conductor->str=new char[BUFFERSIZE*2];
            status_conductor->buffer=new char[BUFFERSIZE*2];

            status_conductor->previous=NULL;


            status_root=status_conductor;
            status_root->next=NULL;

            //status_work=NULL;

        }
        else
        {

            remove_tab_listview();

            setmacro(current_query->BT_querybutton,MUIA_Selected, FALSE);

            a++;

            while(status_conductor->next)
            {
                status_conductor=status_conductor->next;
                a++;
            }


            work_status=status_conductor;

            status_conductor->next=new status_window;
            status_conductor=status_conductor->next;

            free_graphical_smilies(status_current->current_query);

            status_conductor->nick=new char[100];
            strcpy(status_conductor->nick,nick);


            if(my_settings.no_server_tabs)
            {
                status_conductor->root=init_conductor(1);
                status_conductor->root->server_tab=0;

            }
            else
            {
                status_conductor->root=init_conductor(2);
                status_conductor->root->server_tab=1;

            }

            strcpy(status_conductor->root->your_current_mode,"");

            //setmacro((Object*)WookieChat->TX_nickname,MUIA_Text_Contents,status_conductor->nick);

            status_conductor->next=NULL;

            status_conductor->previous=work_status;

            status_conductor->root->previous=NULL;
            status_conductor->root->next=NULL; //test
            status_conductor->conductor=status_conductor->root;

            status_current=status_conductor;
            current_query=status_conductor->conductor;

            update_your_nickname_text_label();
            strcpy(status_conductor->conductor->your_current_mode,"");

            status_conductor->str=new char[BUFFERSIZE*2];
            status_conductor->buffer=new char[BUFFERSIZE*2];


        }

        status_conductor->away=FALSE;

        status_conductor=status_current;
        status_conductor->current_query=status_conductor->conductor;

        setup_notifys();

        status_conductor->remote_charset=0;

        status_conductor->waiting_for_ping=0;

        strcpy(status_conductor->str,"");
        status_conductor->connection_active=0;
        status_conductor->pass[0]='\0';
        status_conductor->servername[0]='\0';
        status_conductor->groupname[0]='\0';
        status_conductor->networkname[0]='\0';
        status_conductor->retry_number=0;
        strcpy(status_conductor->last_incoming_line_unparsed,"");


        strcpy(status_conductor->nick_pass,"");
        strcpy(status_conductor->auto_joins,"");

        strcpy(status_conductor->last_notice_nick,"");
        strcpy(status_conductor->last_msg_nick,"");

        status_conductor->away=FALSE;
        //strcpy(status_conductor->conductor->your_current_mode,"");


        //status_conductor->your_current_mode[0]=='\0';
        //strcpy(status_conductor->nick_string,nick);
        //status_conductor->your_current_mode[0]==' ';
        //status_conductor->your_current_mode[1]=='\0';

        //setmacro((Object*)WookieChat->TX_nickname,MUIA_Text_Contents,status_conductor->nick_string);

        status_conductor->a_socket = -1;

        //status_conductor->a_socket = socket(AF_INET,SOCK_STREAM,0);
        //if(fdmax<status_conductor->a_socket) fdmax=status_conductor->a_socket;

        status_conductor->ident.a_socket=-1;

        status_conductor->filter_swear_words=0;

        status_conductor->max_modes=0;

        status_conductor->iv=(a*100)+2;
        status_conductor->root->iv=(a*100)+2;


        /*if(ZUNE_SYSTEM==TRUE)
        {
            getmacro((Object*)GR_top,MUIA_Group_ChildList,&list);
            object_state = list->mlh_Head;
            while (( member_object = NextObject( &object_state ) ))
            {
                    DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)member_object);
            }
            //if(status_current->current_query->GR_conductor)
            //   setmacro((Object*)status_current->current_query->GR_conductor, MUIA_ShowMe, FALSE);
        } */

        if(DoMethod((Object*)status_current->current_query->GR_conductor,MUIM_Group_InitChange))
        {
            if(DoMethod((Object*)status_current->current_query->GR_listviews,MUIM_Group_InitChange))
            {
                if(DoMethod((Object*)status_current->current_query->GR_nicklist_and_tabs,MUIM_Group_InitChange))
                {

                    getmacro((Object*)status_current->current_query->GR_nicklist_and_tabs,MUIA_Group_ChildList,&list);
                    object_state = list->mlh_Head;
                    while (( member_object = NextObject( (Object**)&object_state ) ))
                    {
                            DoMethod((Object*)status_current->current_query->GR_nicklist_and_tabs, OM_REMMEMBER, (Object*)member_object);
                    }
                    DoMethod((Object*)status_current->current_query->GR_nicklist_and_tabs, OM_ADDMEMBER, (Object*)status_current->current_query->LV_nicklist);


            		DoMethod((Object*)status_current->current_query->GR_nicklist_and_tabs,MUIM_Group_ExitChange);

                }

        		DoMethod((Object*)status_current->current_query->GR_listviews,MUIM_Group_ExitChange);

            }

    		DoMethod((Object*)status_current->current_query->GR_conductor,MUIM_Group_ExitChange);

        }



        /*if(ZUNE_SYSTEM==TRUE)
        {
        	DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->conductor->GR_conductor);
        }
        else*/
        {
        	DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->conductor->GR_conductor);
            //setmacro((Object*)status_conductor->conductor->GR_conductor, MUIA_ShowMe, TRUE);

        }

        DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

    }

	status_conductor->GR_server1_buttons = (Object*)GroupObject,
		MUIA_HelpNode, (char*)"GR_server1_buttons",
		MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
		MUIA_Group_HorizSpacing, 2,
	End;


    if(DoMethod((Object*)GR_bottom_group,MUIM_Group_InitChange))
	{

        if(DoMethod((Object*)GR_buttons,MUIM_Group_InitChange))
    	{

            if(DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_InitChange))
        	{

                DoMethod((Object*)GR_buttons, OM_ADDMEMBER,(Object*)status_conductor->GR_server1_buttons);

                DoMethod((Object*)status_conductor->GR_server1_buttons, OM_ADDMEMBER, (Object*)status_conductor->conductor->BT_querybutton);

                setmacro(status_conductor->conductor->BT_querybutton,MUIA_Text_Contents, "\033I[2:00000000,00000000,00000000] Status");
                strcpy(status_conductor->conductor->name,"Status");

                setmacro(status_conductor->conductor->BT_querybutton,MUIA_Selected, TRUE);
                status_conductor->conductor->activity=0;

            	DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_ExitChange);

            }

        	DoMethod((Object*)GR_buttons,MUIM_Group_ExitChange);
        }

        DoMethod((Object*)GR_bottom_group,MUIM_Group_ExitChange);
    }

    DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,status_conductor->iv);




    //if(status_work) status_conductor->conductor->previous=status_work->current_query;

    status_conductor->current_query=status_conductor->conductor;

        //channel_entry nlist_tab;
        if(ZUNE_SYSTEM==TRUE)
            strcpy(status_conductor->conductor->nlist_tab.entry,"         ");
        else
            strcpy(status_conductor->conductor->nlist_tab.entry,"         :");
        status_conductor->conductor->nlist_tab.colour=23;

        strcat(status_conductor->conductor->nlist_tab.entry,status_conductor->conductor->name);

        status_conductor=status_current;
        status_conductor->conductor=status_conductor->current_query;

        give_each_tab_a_listview_number_for_switching_tabs();

        if(DEBUG) printf("new status tab number is %d\n",count);

        if(ZUNE_SYSTEM==TRUE)
            sprintf(status_conductor->conductor->nlist_tab_title.entry,"         \033c\033E(no server joined)");
        else
            sprintf(status_conductor->conductor->nlist_tab_title.entry,"         :\033c\033E(no server joined)");
        status_conductor->conductor->nlist_tab_title.colour=23;

        DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_InsertSingle,&status_current->current_query->nlist_tab_title,status_current->current_query->nlist_tab_title_number);
        DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_InsertSingle,&status_current->current_query->nlist_tab,status_current->current_query->nlist_tab_number);

        setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_Active,status_current->current_query->nlist_tab_number);

        add_tabs_to_nicklist_group();

        give_each_tab_a_page_group_number();

        setmacro((Object*)GR_top,MUIA_Group_ActivePage,status_current->current_query->page_number);


    DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);
    }

    return 0;

}


//The groups we use for our Tabs are all added to a PageGroup. When we change tabs, we set the
//active page. We need to use this function because our tabs can be sorted when they're created, and then get into
//a different order internally..
void give_each_tab_a_page_group_number()
{
    BOOL is_our_reset_pointer_valid=TRUE;
    if(status_conductor)
    {
        status_work=status_conductor;
        work_query=status_conductor->conductor;
    }
    else is_our_reset_pointer_valid=FALSE;

    getmacro((Object*)GR_top,MUIA_Group_ChildList,&list);
    object_state = list->mlh_Head;
    int count=0;
    while (( member_object = NextObject( (Object**)&object_state ) ))
    {
        for(status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
        {
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
            {
                if(member_object == status_conductor->conductor->GR_conductor) { status_conductor->conductor->page_number=count; break;  }

            }
            if(status_conductor->conductor)
            {
                if(member_object == status_conductor->conductor->GR_conductor) { status_conductor->conductor->page_number=count; break;  }
            }
        }
        count++;
    }


        /*for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
        {
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next,count++)
            {
                printf("tab %d has the active page %d\n",count,status_conductor->conductor->page_number);
            }
        } */

    if(is_our_reset_pointer_valid==TRUE)
    {
        status_conductor=status_work;
        status_conductor->conductor=work_query;
    }
}


//in our LV_tabs object, the user can click on entrys to switch between tabs. Each Listview-style Tab Entry requires
//a "code", so when we click, it goes to the right tab. Lets sort out all the codes for all the existing tabs everytime
//a new tab is created or closed.
void give_each_tab_a_listview_number_for_switching_tabs()
{
    BOOL is_our_reset_pointer_valid=TRUE;
    if(status_conductor)
    {
        status_work=status_conductor;
        work_query=status_conductor->conductor;
    }
    else is_our_reset_pointer_valid=FALSE;

    for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
        {
            if(status_conductor->conductor == status_conductor->root)// && status_conductor->conductor->removed==0)
            {
                status_conductor->conductor->nlist_tab_title_number=count;
                count++;
                status_conductor->conductor->nlist_tab_number=count;
                count++;


            }
            else
            {
                status_conductor->conductor->nlist_tab_number=count;
                count++;

            }
           // printf("network:%d tab:%d\n",count,status_conductor->conductor->nlist_tab_number);
        }
    }

    if(is_our_reset_pointer_valid==TRUE)
    {
        status_conductor=status_work;
        status_conductor->conductor=work_query;
    }
}


query_window *query_work;
query_window *query_previous, *query_next;


void close_tab(void)
{


    status_conductor=status_current;

    status_conductor=status_current;
    status_conductor->conductor=status_conductor->current_query;

    count=0;
    iv--;

    //if we've only got one server group, and we've closed the only channel here, we
    //want to leave that tab open for when we join another channel
    //if(status_conductor->current_query->name[0]=='#')
    if(strcspn(status_conductor->current_query->name,status_conductor->chantypes)==0)
    {
        if(status_root->next==NULL && status_current->current_query == status_root->root && status_root->root->next == NULL)
        {
            strcpy(status_conductor->conductor->name," ");
            strcpy(status_conductor->conductor->displayed_name,"(none)");
            setmacro((Object*)status_conductor->conductor->BT_querybutton,MUIA_Text_Contents,status_conductor->conductor->displayed_name);

            strcpy(status_conductor->conductor->nlist_tab.entry,"         :");
            strcat(status_conductor->conductor->nlist_tab.entry,status_conductor->conductor->displayed_name);
            DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_ReplaceSingle,&status_conductor->conductor->nlist_tab,status_conductor->conductor->nlist_tab_number,0,0);

            sprintf(buffer3,"%s%s%s%s %s",timestamp,GetCatalogStr(catalog,217,"["),GetCatalogStr(catalog,165,"Part"),GetCatalogStr(catalog,218,"]"),status_conductor->nick);
        	add_text_to_current_list((char*)buffer3, 3, ACTIVITY);

        }
    }


    //Are we closing a DCC CHAT tab?
    if(status_conductor->current_query->name[0]=='$')
    {
        dcc_chat_conductor=dcc_chat_root->next;
        while(dcc_chat_conductor)
        {
            if(dcc_chat_conductor->conductor==status_conductor->current_query)
            {
                dcc_chat_conductor->removed=1; //yes its now removed
                shutdown((int)dcc_chat_conductor->dcc_socket,(int)2);
                dcc_chat_conductor->conductor=NULL;
                break;
            }
            dcc_chat_conductor=dcc_chat_conductor->next;

        }

    }

	strcpy(status_conductor->current_query->name," ");

    query_work=status_conductor->current_query->previous;
    status_work=status_conductor->previous;

    for(count=0,status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
    {
        if(status_conductor->conductor->BT_querybutton) count++;

    }

    if(status_root->next==NULL && status_current->current_query == status_root->root && status_root->root->next == NULL) return;

    if(DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_InitChange))
    {

        if(DoMethod((Object*)GR_bottom_group,MUIM_Group_InitChange))
    	{
            status_conductor=status_current;
            status_conductor->conductor=status_conductor->current_query;


            DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_Remove,status_conductor->current_query->nlist_tab_number);
            if(DEBUG) printf("closing %ld\n",status_conductor->current_query->nlist_tab_number);
            DoMethod((Object*)status_conductor->GR_server1_buttons, OM_REMMEMBER, (Object*)status_conductor->current_query->BT_querybutton);
            MUI_DisposeObject((Object*)status_conductor->current_query->BT_querybutton);
            status_conductor->current_query->BT_querybutton=NULL;
            status_conductor->current_query->removed=1;


            strcpy(status_conductor->current_query->name," ");
            strcpy(status_conductor->current_query->displayed_name,"(none)");

        }

        if(count==1)
        {
            if(DEBUG) printf("remove all buttons\n");

            if(my_settings.quit_msg) sprintf(buffer3,"QUIT :%s\r\n",my_settings.quit_msg);
                else sprintf(buffer3,"QUIT :Client Exiting\r\n");

            DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_Remove,status_conductor->root->nlist_tab_title_number);

            if(status_conductor->a_socket!=-1)
            {
                shutdown((int)status_conductor->a_socket,(int)2);
                FD_CLR(status_conductor->a_socket, &read_master);
            }
        }

    	DoMethod((Object*)GR_bottom_group,MUIM_Group_ExitChange);

    }


    // Add the new group and buttons now that the closed ones have been removed

    if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
    {

        if(DoMethod((Object*)status_conductor->conductor->GR_conductor,MUIM_Group_InitChange))
        {
            if(DoMethod((Object*)status_conductor->conductor->GR_listviews,MUIM_Group_InitChange))
            {

                getmacro((Object*)status_conductor->conductor->GR_listviews,MUIA_Group_ChildList,&list);
                object_state = list->mlh_Head;

            	while ((member_object = NextObject( (Object**)&object_state )) )
            	{
                    if(member_object == status_conductor->conductor->GR_nicklist_and_tabs)
                        DoMethod((Object*)status_conductor->conductor->GR_listviews, OM_REMMEMBER, (Object*)status_conductor->conductor->GR_nicklist_and_tabs);

                }

        		DoMethod((Object*)status_conductor->conductor->GR_listviews,MUIM_Group_ExitChange);

            }

    		DoMethod((Object*)status_conductor->conductor->GR_conductor,MUIM_Group_ExitChange);

        }

		DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

    }
    //does this belong here?

    getmacro((Object*)GR_top,MUIA_Group_ChildList,&list);
    object_state = list->mlh_Head;

	while ((member_object = NextObject( (Object**)&object_state )) )
	{
        if(member_object == status_conductor->conductor->GR_conductor)
            DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)status_conductor->conductor->GR_conductor);
    }

    // end experiment

    //is the logfile open?
    if(status_conductor->current_query->log_file)
    {
        Close(status_conductor->current_query->log_file);
        status_conductor->current_query->log_file=0;
    }

    //remove unsent messages from the paste delay array
    for(count=0; count<=status_conductor->conductor->queued_messages_total && count<MAX_QUEUED_MESSAGES-1; count++)
    {
        if(status_conductor->conductor->queued_messages[count])
        {
            delete status_conductor->conductor->queued_messages[count];
            status_conductor->conductor->queued_messages[count]=NULL;

        }

    }
    queued_messages_total=queued_messages_total-status_conductor->conductor->queued_messages_total;
    status_conductor->conductor->queued_messages_total=0;


    //free up nicklist struct memory!
    for(count=0; count<2500; count++)
    {
        if(status_conductor->current_query->nicklist[count].hostname)
        {
            delete [] status_conductor->current_query->nicklist[count].hostname;
            status_conductor->current_query->nicklist[count].hostname=NULL;
        }
    }

    //count how many tabs there are open
    for(count2=0, status_conductor->conductor=status_conductor->root;
        status_conductor->conductor;
        status_conductor->conductor=status_conductor->conductor->next)
        {
            if(status_conductor->conductor->removed==0) count2++;

        }

    status_conductor=status_current;
    status_conductor->conductor=status_current->current_query;

    if(status_conductor->current_query == status_conductor->root)
    {
            if(DEBUG) printf("closing root tab\n");

            //if(DEBUG) printf("freeing root tab string gadget struct\n");

            //attempting to clean up typed history
            for(status_conductor->current_query->string_conductor=status_conductor->current_query->string_root;
                status_conductor->current_query->string_conductor;
                status_conductor->current_query->string_conductor=work_history)
            {

                work_history=status_conductor->current_query->string_conductor->next;

                if(status_conductor->current_query->string_conductor->buffer_history[0]!='\0')
                {
                    delete status_conductor->current_query->string_conductor;
                }

            }

            if(status_conductor->conductor->next &&status_conductor->conductor->previous==NULL)
            {
                if(DEBUG) printf("closing root tab and moving root pointer one node forward\n");

                status_conductor->conductor=status_conductor->conductor->next;
                status_conductor->root=status_conductor->conductor;

                FreeVec(status_conductor->current_query);
                status_conductor->current_query=NULL;

                status_conductor->root->previous=NULL;
                status_conductor->current_query=status_conductor->root;

            }
            else
            {
                if(DEBUG) printf("closing root tab and removing group\n");

                if(DEBUG) printf("remove buttons group from display\n");

                if(DoMethod((Object*)GR_bottom_group,MUIM_Group_InitChange))
            	{

                    if(DoMethod((Object*)GR_buttons,MUIM_Group_InitChange))
                	{

                        if(DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_InitChange))
                    	{

                            DoMethod((Object*)GR_buttons, OM_REMMEMBER,(Object*)status_conductor->GR_server1_buttons);

                        	DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_ExitChange);

                        }

                    	DoMethod((Object*)GR_buttons,MUIM_Group_ExitChange);
                    }

                    DoMethod((Object*)GR_bottom_group,MUIM_Group_ExitChange);
                }

                if(DEBUG) printf("free up status_conductor node\n");

                FreeVec(status_conductor->current_query);
                status_conductor->current_query=NULL;

                delete [] status_conductor->str;
                delete [] status_conductor->buffer;

                if(status_conductor->a_socket != -1)
                {
                    if(FD_ISSET(status_conductor->a_socket, &write_master)) FD_CLR(status_conductor->a_socket, &write_master);
                    if(FD_ISSET(status_conductor->a_socket, &read_master)) FD_CLR(status_conductor->a_socket, &read_master);
                }
                if(status_current->conductor == status_root->root)
                {
                    if(DEBUG) printf("2closing root tab and moving root pointer one node forward\n");

                    status_conductor=status_conductor->next;
                    status_root=status_conductor;

                    delete status_current;
                    status_current=NULL;

                    status_conductor->previous=NULL;


                }
                else
                {
                    if(DEBUG) printf("2closing root tab and rejiggering root pointer\n");

                    if(status_conductor)
                    {
                        status_previous=status_conductor->previous;
                        status_next=status_conductor->next;
                        if(status_conductor) status_next=status_conductor->next;
                        if(status_previous) status_previous->next=status_next;
                        if(status_next) status_next->previous=status_previous;
                    }

                    delete status_current;
                    status_current=NULL;

                }

            }


    }
    else //we want to close a tab, remove its entry in the linked list, and pretend it never existed!
    {

        if(DEBUG) printf("closing regular tab\n");
        status_conductor->conductor=status_conductor->current_query;

        if(status_conductor->conductor)
        {
            query_previous=status_conductor->conductor->previous;
            query_next=status_conductor->conductor->next;
            if(query_previous) query_previous->next=query_next;
            if(query_next) query_next->previous=query_previous;
        }

        //attempting to clean up typed history
        for(status_conductor->current_query->string_conductor=status_conductor->current_query->string_root;
            status_conductor->current_query->string_conductor;
            status_conductor->current_query->string_conductor=work_history)
        {

            work_history=status_conductor->current_query->string_conductor->next;

            if(status_conductor->current_query->string_conductor->buffer_history[0]!='\0')
            {
                delete status_conductor->current_query->string_conductor;
            }

        }

        work_query=query_previous;

        FreeVec(status_conductor->current_query);
        status_conductor->current_query=NULL;

    }


    status_conductor=status_root;


    //our linked list has been changed, a node has been removed. so now, lets give new
    //"tab switching" id codes to every node.
    for(count=1,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next,count++)
    {
        for(status_conductor->conductor=status_conductor->root,status_conductor->iv=(count*100+1);
            status_conductor->conductor;
            status_conductor->conductor=status_conductor->conductor->next)
            {
                if(status_conductor->conductor)
                {

                    status_conductor->iv++;
                    status_conductor->conductor->iv=status_conductor->iv;

                    if(status_conductor->conductor->BT_querybutton)
                    {
                        DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_KillNotify,MUIA_Selected);
                        DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,status_conductor->iv);
                    }
                }
            }

    }

    give_each_tab_a_listview_number_for_switching_tabs();

    give_each_tab_a_page_group_number();

    /*
    //give every LISTVIEW TAB entry a number so we can change it
    for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
    {
            if(status_conductor)
            {
                for(status_conductor->conductor=status_conductor->root; status_conductor->conductor;
                    status_conductor->conductor=status_conductor->conductor->next)
                    {
                        if(status_conductor->conductor)
                        {
                            if(status_conductor->conductor->removed==0)
                            {
                                status_conductor->conductor->nlist_tab_number=count;
                                if(DEBUG) printf("closing irc tab, new entry number %d\n",count);
                                count++;
                            }
                        }
                }
            }
        } */

        //get to the last node in our linked list
        //for(status_conductor->conductor=status_conductor->root; status_conductor->conductor->next; status_conductor->conductor=status_conductor->conductor->next);

    status_conductor=status_root;
    status_conductor->conductor=status_conductor->root;
    if(status_current && query_work)
    {
        for(status_conductor=status_current; status_conductor; status_conductor=status_conductor->previous)
        {
            if(DEBUG) printf("search backwards for tab in current Status node\n");

            for(status_conductor->conductor=query_work; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->previous)
            {
                if(status_conductor->conductor)
                {
                    if(status_conductor->conductor->LV_channel)
                    {
                        activate_tab_button();

                        count=1000;
                        break;
                    }
                }
            }

            if(count==1000) break;

        }
    }
    else if(status_work)
    {
        for(status_conductor=status_work; status_conductor; status_conductor=status_conductor->previous)
        {
            if(DEBUG) printf("search backwards for tab in the previous Status node\n");
            //get to the last node in our linked list
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor->next; status_conductor->conductor=status_conductor->conductor->next);

            for(; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->previous)
            {
                if(status_conductor->conductor)
                {
                    if(status_conductor->conductor->LV_channel)
                    {
                        activate_tab_button();

                        count=1000;
                        break;
                    }
                }
            }

            if(count==1000) break;

        }
    }


    if(count!=1000)
    {
        count=0;
        if(DEBUG) printf("search FORWARDS for tab\n");

        for(status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
        {
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor;
                status_conductor->conductor=status_conductor->conductor->next)
            {
                if(status_conductor->conductor->removed==0)
                {
                    if(status_conductor->conductor->LV_channel)
                    {
                        //remove_tab_listview2();

                        activate_tab_button();

                        count=1000;
                        break;
                    }
                }
            }

            if(count==1000) break;

        }
    }

    setmacro((Object*)GR_top,MUIA_Group_ActivePage,status_current->current_query->page_number);


    DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);

    DoMethod((Object*)status_current->current_query->LV_nicklist,MUIM_NList_GetEntry,0, &work_list_entry);
    DoMethod((Object*)status_current->current_query->LV_nicklist,MUIM_NList_ReplaceSingle,work_list_entry,0,NOWRAP,ALIGN_LEFT);

    change_window_titlebar_text();

    if(status_current)
    {
        if(status_current->current_query)
        {
            if(status_current->current_query->LV_channel)
            {
                DoMethod((Object*)status_current->current_query->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);
            }
        }
    }






}

