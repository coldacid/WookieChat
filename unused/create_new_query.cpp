/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

int create_new_query_window(char *name, int show_now, int query_type)
{

	// Make all buttons deselected
    if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, FALSE);

    if(DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_InitChange))
    {

    int running1=1;

    kill_my_notifys();

    if(status_conductor != status_current) show_now=0; //dont show channel on join if viewing different tab

    if(show_now) remove_tab_listview();

    if(show_now) current_query=status_conductor->conductor; //experimental

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
            switch_between_tabs(count+(count2*100)+1);

            DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);
            if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, TRUE);

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
        if(strlen(status_conductor->conductor->displayed_name)>11)
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

    //setmacro((Object*)WookieChat->TX_nickname,MUIA_Text_Contents,status_conductor->nick);
    update_your_nickname_text_label();

	if(show_now)
	{
        if(DEBUG) printf("show now for %s\n",status_conductor->conductor->name);

        if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
        {

            //if(ZUNE_SYSTEM==FALSE)
            {
                if(status_current)
                {
                    if(status_current->current_query)
                    {
                        //if(status_current->current_query->GR_conductor)
                          //  setmacro((Object*)status_current->current_query->GR_conductor, MUIA_ShowMe, FALSE);
                    }
                }
            } /*
            else
            {

                getmacro((Object*)GR_top,MUIA_Group_ChildList,&list);
                object_state = list->mlh_Head;
                while (( member_object = NextObject( (Object**)&object_state ) ))
                {
                        DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)member_object);
                }

            }   */

            status_conductor->current_query=status_conductor->conductor;
            status_current=status_conductor;
            current_query=status_conductor->conductor;
            status_conductor->conductor->activity=0;

            remove_tab_listview();
            add_tabs_to_nicklist_group();

            DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_current->current_query->GR_conductor);


            //if(ZUNE_SYSTEM==FALSE)
            {
                if(status_current)
                {
                    if(status_current->current_query)
                    {
                        //if(status_current->current_query->GR_conductor)
                          //  setmacro((Object*)GR_top,MUIA_Group_ActivePage,status_current->current_query->GR_conductor);

                            //setmacro((Object*)status_current->current_query->GR_conductor, MUIA_ShowMe, TRUE);
                    }
                }
            }

            DoMethod((Object*)GR_top,MUIM_Group_ExitChange);


        }

        DoMethod((Object*)status_conductor->conductor->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

        setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, TRUE);

    }
    else
    {
        //if(ZUNE_SYSTEM==FALSE)
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
                            //remove_tab_listview2();

                            DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->conductor->GR_conductor);
                            //setmacro((Object*)status_conductor->conductor->GR_conductor, MUIA_ShowMe, FALSE);

            //status_conductor->current_query=status_conductor->conductor;
            //status_current=status_conductor;
            //current_query=status_conductor->conductor;
            //status_conductor->conductor->activity=0;
                            //add_tabs_to_nicklist_group();

                        }
                    }
                }
                DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

            }
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
    strcat(status_conductor->conductor->nlist_tab.entry,status_conductor->conductor->name);


    //Count how many Entrys we have in our Listview-style Tabs object, and then we can add a new entry
    //to the end of it
    for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next) //,count++)
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; //status_conductor->conductor->next;
            status_conductor->conductor=status_conductor->conductor->next)
            {
                //if(DEBUG) printf("status: count:%d\n",count);

                if(status_conductor->conductor && status_conductor->conductor->removed==0)
                {
                    count++;
                    if(DEBUG) printf("conductor: count:%d\n",count);

                }
                if(status_conductor->conductor->next==NULL) break;
            }

        if(work_status == status_conductor) break;

    }

    count--;
    if(DEBUG) printf("nlist tab number for new tab is %d\n",count);

    if(DEBUG) printf("1a\n");

    if(status_conductor)
    {
        if(status_conductor->conductor)
        {

            /*getmacro((Object*)WookieChat->LV_tabs,MUIA_NList_Entries, &entries);
            if(DEBUG) printf("1aa, entries:%d count:%d\n",entries,count);
        	
            if(entries<=count) count=entries;
            */
            //if(DEBUG) printf("insert at:%d text:%s\n",count,status_conductor->conductor->nlist_tab);

            DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_InsertSingle,status_conductor->conductor->nlist_tab,count);

        }
    }

    if(DEBUG) printf("1b\n");

    //give each "nlist tab" entry a line number so we can change it easily
    for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor;
            status_conductor->conductor=status_conductor->conductor->next)
            {

                if(status_conductor->conductor && status_conductor->conductor->removed==0)
                {
                    status_conductor->conductor->nlist_tab_number=count;
                    count++;

                }
            }
    }


    if(DEBUG) printf("1c\n");


    if(DEBUG) printf("2\n");

    status_conductor=work_status;
    if(DEBUG) printf("3\n");

    //sort_linked_list();

    if(show_now)
	{

        setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_Active,status_current->current_query->nlist_tab_number);
            LONG num_page_groups;
            getmacro((Object*)GR_top,MUIA_Group_ActivePage,&num_page_groups);
            //if(num_page_groups >= status_current->current_query->nlist_tab_number)
            {
                setmacro((Object*)GR_top,MUIA_Group_ActivePage,status_current->current_query->nlist_tab_number);
            }

    }


    if(DEBUG) printf("4\n");
    //get to the last conductor that we just created
    for(status_conductor=work_status, status_conductor->conductor=status_conductor->root; status_conductor->conductor->next; status_conductor->conductor=status_conductor->conductor->next);

    if(DEBUG) printf("5\n");


	setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);

    if(DEBUG) printf("6\n");

    RECENTLY_CREATED_A_TAB=TRUE;

    DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);
    }

    if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, TRUE);

	return 0;

}
