/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

int create_new_status(int first)
{

    int a=1;

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
        strcpy(status_conductor->conductor->nlist_tab.entry,"         :");
        status_conductor->conductor->nlist_tab.colour=23;

        strcat(status_conductor->conductor->nlist_tab.entry,status_conductor->conductor->name);

        /*for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
        {
            for(status_conductor->conductor=status_conductor->root;
                status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
                {

                    if(status_conductor->conductor->removed==0) count++;
                }
    	} */


        status_conductor=status_current;
        status_conductor->conductor=status_conductor->current_query;

        give_each_tab_a_listview_number_for_switching_tabs();

        //DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_InsertSingle,&status_current->current_query->nlist_tab,count);
        //if(count>0) count--;
        //status_current->current_query->nlist_tab_number=count;

        if(DEBUG) printf("new status tab number is %d\n",count);

        //sprintf(status_conductor->conductor->nlist_tab_title.entry,"         :%s",status_conductor->servername);
        sprintf(status_conductor->conductor->nlist_tab_title.entry,"         :\033l\033E(no server joined)"); //,status_conductor->servername);
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

