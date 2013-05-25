/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

void check_column_size()
{
    if(NEWDEBUG) printf("checking columns sizes\n");

    if(my_settings.use_column_display == 1)
    {

        LONG col1,col2;

        col1=DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_ColWidth,1,MUIV_NList_ColWidth_Get);
        col2=DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_ColWidth,2,MUIV_NList_ColWidth_Get);

        //printf("width nicks-middle:%ld timestamp:%ld \n", col1,col2);
        if(col1 != -1) my_settings.nick_column_width=col1;
        if(col2 != -1) my_settings.timestamp_column_width=col2;

        if(col1 != -1 || col2 != -1)
        {
            //setmacro((Object*)WookieChat->NB_column_width_nick,MUIA_Numeric_Value,my_settings.nick_column_width);
            //setmacro((Object*)WookieChat->NB_column_width_timestamp,MUIA_Numeric_Value,my_settings.timestamp_column_width);
            sprintf(work_buffer,"%i",my_settings.timestamp_column_width);
            setmacro((Object*)WookieChat->NB_column_width_timestamp,MUIA_String_Contents,work_buffer);

            sprintf(work_buffer,"%i",my_settings.nick_column_width);
            setmacro((Object*)WookieChat->NB_column_width_nick,MUIA_String_Contents,work_buffer);

            sprintf(listview_format,"COL=2 PIXWIDTH=%i, COL=1 DELTA=%i PIXWIDTH=%i BAR, DELTA=%i COL=0",my_settings.timestamp_column_width,my_settings.delta,my_settings.nick_column_width,my_settings.delta);

        }
    }
}

void remove_tab_listview()
{

    if(!status_current) return;
    if(!status_current->current_query) return;

if(GEIT) printf("removing 1-5 \n");

    if(DoMethod((Object*)status_current->current_query->GR_listviews,MUIM_Group_InitChange))
    {

        if(DoMethod((Object*)status_current->current_query->GR_nicklist_and_tabs,MUIM_Group_InitChange))
        {

            getmacro((Object*)status_current->current_query->GR_nicklist_and_tabs,MUIA_Group_ChildList,&list);
            object_state = list->mlh_Head;

if(GEIT) printf("remove 2-5 \n");

        	while ((member_object = NextObject( (Object**)&object_state )) )
        	{

                if(member_object == status_current->current_query->GR_tabs)
                    DoMethod((Object*)status_current->current_query->GR_nicklist_and_tabs, OM_REMMEMBER, (Object*)status_current->current_query->GR_tabs);

            }

if(GEIT) printf("remove 3-5 \n");

    		DoMethod((Object*)status_current->current_query->GR_nicklist_and_tabs,MUIM_Group_ExitChange);


        }

if(GEIT) printf("remove 4-5 \n");

		DoMethod((Object*)status_current->current_query->GR_listviews,MUIM_Group_ExitChange);


    }

if(GEIT) printf("removing 5-5 \n");

}

BOOL is_it_there;

//trying to fix geit crashes
void add_tabs_to_nicklist_group()
{

    if(status_conductor)
    {

            if(!status_conductor->conductor) return;

            setmacro((Object*)status_conductor->conductor->GR_tabs, MUIA_Weight, my_settings.listview_tabs_weight);

            if(DoMethod((Object*)status_conductor->conductor->GR_listviews,MUIM_Group_InitChange))
            {

                if(DoMethod((Object*)status_conductor->conductor->GR_nicklist_and_tabs,MUIM_Group_InitChange))
                {

if(GEIT) printf("adding 1-6\n");


                    is_it_there=FALSE;

                    getmacro((Object*)status_conductor->conductor->GR_nicklist_and_tabs,MUIA_Group_ChildList,&list);
                    object_state = list->mlh_Head;

                	while ((member_object = NextObject( (Object**)&object_state )) )
                	{
                        if(member_object == status_conductor->conductor->GR_tabs)
                        {
                            is_it_there=TRUE;
                            if(GEIT) printf("yes its here\n");
                        }

                    }


                    if(my_settings.use_nlistobject_for_tabs)
                    {

                        if(is_it_there==FALSE)
                        {
                            DoMethod((Object*)status_conductor->conductor->GR_nicklist_and_tabs, OM_ADDMEMBER, (Object*)status_conductor->conductor->GR_tabs);

                            if(GEIT) printf("adding listview nlist object to the group\n");

                        }

                        setmacro((Object*)GR_bottom_group, MUIA_ShowMe, FALSE);

                        setmacro((Object*)status_conductor->conductor->GR_nicklist_and_tabs, MUIA_ShowMe, TRUE);
                        setmacro((Object*)status_conductor->conductor->GR_listviews_sub, MUIA_ShowMe, TRUE);
                        setmacro((Object*)WookieChat->LV_tabs, MUIA_ShowMe, TRUE);

                        if(!my_settings.hide_user_list_buttons)
                    		setmacro((Object*)WookieChat->GR_click_user_list_buttons, MUIA_ShowMe, TRUE);
                        else
                    		setmacro((Object*)WookieChat->GR_click_user_list_buttons, MUIA_ShowMe, FALSE);

                        setmacro((Object*)status_conductor->conductor->GR_tabs, MUIA_ShowMe, TRUE);

                    }
                    else
                    {

                        if(is_it_there==FALSE)
                        {
                            DoMethod((Object*)status_conductor->conductor->GR_nicklist_and_tabs, OM_ADDMEMBER, (Object*)status_conductor->conductor->GR_tabs);

                            if(GEIT) printf("adding listview nlist object to the group\n");

                        }

                        if(strcspn(status_conductor->conductor->name,status_conductor->chantypes)==0)
                            setmacro((Object*)status_conductor->conductor->GR_nicklist_and_tabs, MUIA_ShowMe, TRUE);
                        else
                            setmacro((Object*)status_conductor->conductor->GR_nicklist_and_tabs, MUIA_ShowMe, FALSE);

                        if(strcspn(status_conductor->conductor->name,status_conductor->chantypes)==0)
                            setmacro((Object*)status_conductor->conductor->GR_listviews_sub, MUIA_ShowMe, TRUE);
                        else
                            setmacro((Object*)status_conductor->conductor->GR_listviews_sub, MUIA_ShowMe, FALSE);

                        setmacro((Object*)WookieChat->LV_tabs, MUIA_ShowMe, FALSE);

                        if(!my_settings.hide_user_list_buttons)
                        {
                    		setmacro((Object*)WookieChat->GR_click_user_list_buttons, MUIA_ShowMe, TRUE);
                            setmacro((Object*)status_conductor->conductor->GR_tabs, MUIA_ShowMe, TRUE);
                        }
                        else
                        {
                    		setmacro((Object*)WookieChat->GR_click_user_list_buttons, MUIA_ShowMe, FALSE);
                            setmacro((Object*)status_conductor->conductor->GR_tabs, MUIA_ShowMe, FALSE);
                        }

                        setmacro((Object*)GR_bottom_group, MUIA_ShowMe, TRUE);
if(GEIT) printf("adding 3g\n");


                    }

                        if(my_settings.hide_channel_mode_buttons)
                    		setmacro((Object*)status_conductor->conductor->GR_mode_gadgets_sub_group, MUIA_ShowMe, FALSE);
                        else
                    		setmacro((Object*)status_conductor->conductor->GR_mode_gadgets_sub_group, MUIA_ShowMe, TRUE);

if(GEIT) printf("adding 4-6\n");


            		DoMethod((Object*)status_conductor->conductor->GR_nicklist_and_tabs,MUIM_Group_ExitChange);

                }
if(GEIT) printf("adding 5-6\n");

        		DoMethod((Object*)status_conductor->conductor->GR_listviews,MUIM_Group_ExitChange);

            }

    }

if(GEIT) printf("adding 6-6\n");


}

void kill_my_notifys(void)
{
    //printf("kill my notifys\n");

    if(status_current->current_query->BT_mode_N) DoMethod((Object*)status_current->current_query->BT_mode_N,MUIM_KillNotify,MUIA_Selected);
    if(status_current->current_query->BT_mode_T) DoMethod((Object*)status_current->current_query->BT_mode_T,MUIM_KillNotify,MUIA_Selected);
    if(status_current->current_query->BT_mode_S) DoMethod((Object*)status_current->current_query->BT_mode_S,MUIM_KillNotify,MUIA_Selected);
    if(status_current->current_query->BT_mode_I) DoMethod((Object*)status_current->current_query->BT_mode_I,MUIM_KillNotify,MUIA_Selected);
    if(status_current->current_query->BT_mode_P) DoMethod((Object*)status_current->current_query->BT_mode_P,MUIM_KillNotify,MUIA_Selected);
    if(status_current->current_query->BT_mode_M) DoMethod((Object*)status_current->current_query->BT_mode_M,MUIM_KillNotify,MUIA_Selected);
    if(status_current->current_query->BT_mode_K)DoMethod((Object*)status_current->current_query->BT_mode_K,MUIM_KillNotify,MUIA_Selected);
    if(status_current->current_query->STR_keyword) DoMethod((Object*)status_current->current_query->STR_keyword,MUIM_KillNotify,MUIA_String_Acknowledge);
    if(status_current->current_query->BT_mode_L) DoMethod((Object*)status_current->current_query->BT_mode_L,MUIM_KillNotify,MUIA_Selected);
    if(status_current->current_query->STR_limit) DoMethod((Object*)status_current->current_query->STR_limit,MUIM_KillNotify,MUIA_String_Acknowledge);

    check_column_size();

}


int update_TX_nicklist(void)
{

    //return 0;

    if(!status_conductor) return 0;
    if(!status_conductor->conductor) return 0;

        if(strcspn(status_conductor->conductor->name,status_conductor->chantypes)==0)
        {
            sprintf(status_conductor->conductor->nicklist_info,"\033c%d ops, %d %s", status_conductor->conductor->nicks_ops,status_conductor->conductor->nicks,GetCatalogStr(catalog,257,"users"));
            setmacro((Object*)status_conductor->conductor->TX_nicklist,MUIA_Text_Contents,status_conductor->conductor->nicklist_info);
            //setmacro((Object*)status_conductor->conductor->TX_nicklist,MUIA_ShowMe, TRUE);
        }
        else
        {
            sprintf(status_conductor->conductor->nicklist_info," "); //, status_conductor->conductor->nicks_ops,status_conductor->conductor->nicks,GetCatalogStr(catalog,257,"users"));
            setmacro((Object*)status_conductor->conductor->TX_nicklist,MUIA_Text_Contents,status_conductor->conductor->nicklist_info);
            //setmacro((Object*)status_conductor->conductor->TX_nicklist,MUIA_ShowMe, FALSE);
        }
    return 0;
}


int update_your_nickname_text_label(void)
{

    //return 0;

    //printf("updating nickname text label..\n");

    if(!status_conductor) return 0;
    if(!status_conductor->conductor) return 0;

    //char *my_string1;
    //getmacro((Object*)WookieChat->TX_nickname,MUIA_Text_Contents,&my_string1);
    //if(stricmp(my_string1,status_conductor->nick_string)) //if the displayed nick is different to whats in the server group, lets update it
    {
        if(DoMethod((Object*)GR_string_gadget,MUIM_Group_InitChange))
        {
            //setmacro((Object*)WookieChat->TX_nickname,MUIA_Text_Contents,NULL);

            if(status_conductor->conductor->your_current_mode[0]==' ' || status_conductor->conductor->your_current_mode[0]=='\0')
            {
                //printf("has NO mode, doing a straight copy\n");
                sprintf(status_conductor->nick_string," %s ",status_conductor->nick);
            }
            else
            {
                if(my_settings.graphical_nicklist)
                {
                    if(status_conductor->conductor->your_current_mode[0]=='@')
                        sprintf(status_conductor->nick_string," \033I[5:progdir:smilies/nicklist/ops] %s ",status_conductor->nick);
                    else if(status_conductor->conductor->your_current_mode[0]=='%')
                        sprintf(status_conductor->nick_string," \033I[5:progdir:smilies/nicklist/half_ops] %s ",status_conductor->nick);
                    else if(status_conductor->conductor->your_current_mode[0]=='+')
                        sprintf(status_conductor->nick_string," \033I[5:progdir:smilies/nicklist/voice] %s ",status_conductor->nick);
                    else
                        sprintf(status_conductor->nick_string," %c%s ",status_conductor->conductor->your_current_mode[0],status_conductor->nick);

                }
                else
                    sprintf(status_conductor->nick_string," %c%s ",status_conductor->conductor->your_current_mode[0],status_conductor->nick);

            }


            setmacro((Object*)WookieChat->TX_nickname,MUIA_Text_Contents,status_conductor->nick_string);

            DoMethod((Object*)GR_string_gadget,MUIM_Group_ExitChange);

        }
    }
    return 0;

}
void activate_tab_button()
{
    if(!status_conductor) { if(GEIT) printf("cant activate tab! (error #1)\n"); return; }
    if(!status_conductor->conductor) { if(GEIT) printf("cant activate tab! (error #2)\n"); return; }

    if(NEWDEBUG) printf("activating tab\n");

    status_conductor->conductor->number_of_lines_unread=0;
    update_your_nickname_text_label();

    if(status_current)
    {
        if(status_current->current_query)
        {
            status_current->current_query->activity=0;

        }
    }
    //if(ZUNE_SYSTEM==FALSE)
    {

        if(status_current)
        {
            if(status_current->current_query)
            {
                //if(status_current->current_query->GR_conductor) setmacro((Object*)status_current->current_query->GR_conductor, MUIA_ShowMe, FALSE);
                free_graphical_smilies(status_current->current_query);
            }
        }
    }

    if(status_conductor->conductor->BT_querybutton)
    {
        if(status_conductor->conductor->BT_querybutton) setmacro((Object*)status_conductor->conductor->BT_querybutton,MUIA_Text_PreParse,"\033c\0332");

        DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_KillNotify,MUIA_Selected);
        setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, TRUE);
        DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,status_conductor->conductor->iv);

        sprintf(string7,"\033I[2:00000000,00000000,00000000] %s",status_conductor->conductor->displayed_name);
        if(status_conductor->conductor->BT_querybutton) setmacro((Object*)status_conductor->conductor->BT_querybutton,MUIA_Text_Contents,string7);

        strcpy(status_conductor->conductor->nlist_tab.entry,"         :");
        status_conductor->conductor->nlist_tab.colour=23;
        strcat(status_conductor->conductor->nlist_tab.entry,status_conductor->conductor->displayed_name);
        DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_ReplaceSingle,&status_conductor->conductor->nlist_tab,status_conductor->conductor->nlist_tab_number,0,0);

    }


    if(status_conductor->conductor->GR_conductor)
    {
        status_conductor->current_query=status_conductor->conductor;
        status_current=status_conductor;
        current_query=status_conductor->current_query;

        setup_background_colours();

        set_channel_clipboard_hook();

        add_tabs_to_nicklist_group();

        setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_Active,status_conductor->conductor->nlist_tab_number);

        update_TX_nicklist();

        setmacro((Object*)GR_top,MUIA_Group_ActivePage,status_conductor->conductor->page_number);

        setmacro((Object*)status_conductor->conductor->GR_tabs, MUIA_Weight, my_settings.listview_tabs_weight);

        if(status_current->current_query->name[0]=='*')
        {
            setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_DisplayHook, &Display_channellist_TextHook);
            setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_Format,"COL=0 WEIGHT=-1 BAR, COL=1 WEIGHT=-1 BAR, COL=2");

        }
        else if(my_settings.use_column_display == 0)
        {
            setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_DisplayHook, &DisplayLI_channel_TextHook);
            setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_Format,"");
        }
        else if(my_settings.use_column_display)
        {
            setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_DisplayHook, &DisplayLI2_channel_TextHook);
            setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_Format,listview_format);

        }

        use_graphical_smilies(status_current->current_query);

    }


    /*if(status_current)
    {
        if(status_current->current_query)
        {
            if(status_current->current_query->LV_channel)
            {
                DoMethod((Object*)status_current->current_query->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);
            }
        }
    } */
}


void setup_notifys(void)
{

    //if(DEBUG) printf("setup notifys..\n");
    if(status_current->current_query->mode_N)
    setmacro((Object*)status_current->current_query->BT_mode_N,MUIA_Selected,TRUE);
    else
    setmacro((Object*)status_current->current_query->BT_mode_N,MUIA_Selected,FALSE);

    if(status_current->current_query->mode_T)
    setmacro((Object*)status_current->current_query->BT_mode_T,MUIA_Selected,TRUE);
    else
    setmacro((Object*)status_current->current_query->BT_mode_T,MUIA_Selected,FALSE);

    if(status_current->current_query->mode_S)
        setmacro((Object*)status_current->current_query->BT_mode_S,MUIA_Selected,TRUE);
    else
        setmacro((Object*)status_current->current_query->BT_mode_S,MUIA_Selected,FALSE);

    if(status_current->current_query->mode_I)
        setmacro((Object*)status_current->current_query->BT_mode_I,MUIA_Selected,TRUE);
    else
        setmacro((Object*)status_current->current_query->BT_mode_I,MUIA_Selected,FALSE);

    if(status_current->current_query->mode_P)
        setmacro((Object*)status_current->current_query->BT_mode_P,MUIA_Selected,TRUE);
    else
        setmacro((Object*)status_current->current_query->BT_mode_P,MUIA_Selected,FALSE);

    if(status_current->current_query->mode_M)
        setmacro((Object*)status_current->current_query->BT_mode_M,MUIA_Selected,TRUE);
    else
        setmacro((Object*)status_current->current_query->BT_mode_M,MUIA_Selected,FALSE);

    if(status_current->current_query->mode_K)
        setmacro((Object*)status_current->current_query->BT_mode_K,MUIA_Selected,TRUE);
    else
        setmacro((Object*)status_current->current_query->BT_mode_K,MUIA_Selected,FALSE);

    setmacro((Object*)status_current->current_query->STR_keyword,MUIA_String_Contents,status_current->current_query->keyword);

    if(status_current->current_query->mode_L)
        setmacro((Object*)status_current->current_query->BT_mode_L,MUIA_Selected,TRUE);
    else
        setmacro((Object*)status_current->current_query->BT_mode_L,MUIA_Selected,FALSE);


    setmacro((Object*)status_current->current_query->STR_limit,MUIA_String_Contents,status_current->current_query->limit);



    if(status_current->current_query->BT_mode_N) DoMethod((Object*)status_current->current_query->BT_mode_N,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,11);
    if(status_current->current_query->BT_mode_N) DoMethod((Object*)status_current->current_query->BT_mode_N,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,21);

    if(status_current->current_query->BT_mode_T) DoMethod((Object*)status_current->current_query->BT_mode_T,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,10);
    if(status_current->current_query->BT_mode_T) DoMethod((Object*)status_current->current_query->BT_mode_T,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,20);

    if(status_current->current_query->BT_mode_S) DoMethod((Object*)status_current->current_query->BT_mode_S,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,12);
    if(status_current->current_query->BT_mode_S) DoMethod((Object*)status_current->current_query->BT_mode_S,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,22);

    if(status_current->current_query->BT_mode_I) DoMethod((Object*)status_current->current_query->BT_mode_I,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,13);
    if(status_current->current_query->BT_mode_I) DoMethod((Object*)status_current->current_query->BT_mode_I,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,23);

    if(status_current->current_query->BT_mode_P) DoMethod((Object*)status_current->current_query->BT_mode_P,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,14);
    if(status_current->current_query->BT_mode_P) DoMethod((Object*)status_current->current_query->BT_mode_P,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,24);

    if(status_current->current_query->BT_mode_M) DoMethod((Object*)status_current->current_query->BT_mode_M,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,15);
    if(status_current->current_query->BT_mode_M) DoMethod((Object*)status_current->current_query->BT_mode_M,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,25);

    if(status_current->current_query->BT_mode_K) DoMethod((Object*)status_current->current_query->BT_mode_K,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);
    if(status_current->current_query->BT_mode_K) DoMethod((Object*)status_current->current_query->BT_mode_K,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,28);

    if(status_current->current_query->STR_keyword) DoMethod((Object*)status_current->current_query->STR_keyword,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);

    if(status_current->current_query->BT_mode_L) DoMethod((Object*)status_current->current_query->BT_mode_L,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);
    if(status_current->current_query->BT_mode_L) DoMethod((Object*)status_current->current_query->BT_mode_L,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,27);

    if(status_current->current_query->STR_limit) DoMethod((Object*)status_current->current_query->STR_limit,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);



}

//retrying to remove crash on geits machine
void remove_tab_listview2()
{


    if(!status_conductor) return;
    if(!status_conductor->conductor) return;
    if(status_conductor->conductor->removed==1) return;

if(GEIT) printf("remove 1-5 \n");

    getmacro((Object*)status_conductor->conductor->GR_nicklist_and_tabs,MUIA_Group_ChildList,&list);
    object_state = list->mlh_Head;

if(GEIT) printf("remove 2-5 \n");

	while ((member_object = NextObject( (Object**)&object_state )) )
	{

        if(member_object == status_conductor->conductor->GR_tabs) // && member_object != status_current->current_query->GR_tabs)
        {
                //this is the change that we might need to undo to fix the tab switch freeze on Geits machine
                if(DoMethod((Object*)status_conductor->conductor->GR_listviews,MUIM_Group_InitChange))
                {
                    if(DoMethod((Object*)status_conductor->conductor->GR_nicklist_and_tabs,MUIM_Group_InitChange))
                    {

if(GEIT) printf("remove 3-5 \n");
                            DoMethod((Object*)status_conductor->conductor->GR_nicklist_and_tabs, OM_REMMEMBER, (Object*)status_conductor->conductor->GR_tabs);

if(GEIT) printf("remove 3-5 \n");

                		DoMethod((Object*)status_conductor->conductor->GR_nicklist_and_tabs,MUIM_Group_ExitChange);

                    }

if(GEIT) printf("remove 4-5 \n");

            		DoMethod((Object*)status_conductor->conductor->GR_listviews,MUIM_Group_ExitChange);

                }


        }
    }

}


//trying to eliminate crash on Geits machine
void switch_between_tabs(int result)
{
    //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, FALSE);

    work_query=status_current->current_query;

    if(GEIT) printf("switching with button click..\n");

    if(DEBUG) printf("switching to tab: %i\n",result);

    //kill_my_notifys();

    work_status=status_conductor;

    status_conductor=status_root;
    status_conductor->conductor=status_conductor->root;

    int a=0, c=1;
    result=result-100;

    while(result > 100)
    {
        c++;
        result=result-100;
        if(status_conductor->next) status_conductor=status_conductor->next;

    }

    int b=1, d=1, running1=1;
    a=result-1;

    //experimental
    work_status=status_conductor;
    status_conductor=status_root;
    status_conductor->conductor=status_conductor->root;

    while(status_conductor)
    {

        b=1,running1=1;
        status_conductor->conductor=status_conductor->root;

        while(status_conductor->conductor)
        {
            if(b==a && c==d) break;
        	status_conductor->conductor=status_conductor->conductor->next;
            b++;

        }
        d++;

        status_conductor=status_conductor->next;
    }

    if(status_conductor)
    {
        if(status_conductor->conductor==status_current->current_query)
        {
            //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, TRUE);

            return;
        }
    }
    b=1, d=1, running1=1;
    a=result-1;
    //experimental

    kill_my_notifys();

    setmacro(status_current->current_query->BT_querybutton,MUIA_Text_PreParse, "\033c\0332");

    //status_window *status_work=new status_window;
    //work_status=status_conductor;

    status_conductor=status_root;
    status_conductor->conductor=status_conductor->root;

    if(status_conductor->conductor==NULL) return;

    if(DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_InitChange))
	{

        if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
    	{

            while(status_conductor)
            {

                b=1,running1=1;
                status_conductor->conductor=status_conductor->root;

                while(status_conductor->conductor && running1)
                {
                    if(status_conductor->conductor->removed==0) remove_tab_listview2();

                    if(b!=a || c!=d)
                    {
                        if(status_conductor->conductor->BT_querybutton)
                        {
                            /*if(ZUNE_SYSTEM==TRUE && my_settings.use_nlistobject_for_tabs == 0)
                                setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);
                            else if(ZUNE_SYSTEM==FALSE)*/
                                setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);
                        }
                    }
                	status_conductor->conductor=status_conductor->conductor->next;
                    b++;
            		if(status_conductor->conductor==NULL) running1=0;

                }
                d++;


                status_conductor=status_conductor->next;

            }

            status_conductor=work_status;
            //status_conductor=status_work;

            b=1, running1=1;

        	status_conductor->conductor=status_conductor->root;


            if(GEIT) printf("2-4 removing GUI objects\n");

            /*if(ZUNE_SYSTEM==TRUE)
            {
                getmacro((Object*)GR_top,MUIA_Group_ChildList,&list);
                object_state = list->mlh_Head;

            	while ((member_object = NextObject( (Object**)&object_state )) )
            	{
                        DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)member_object);

                        if(member_object == status_current->current_query->GR_conductor)
                            status_current->current_query->activity=0;
                }
            }*/
            //remove_tab_listview();

            if(GEIT) printf("3-4 sucessfully removed TAB listview\n");

    		while(running1)
    		{
                if(b == a)
    			{
                    activate_tab_button();
                }
    			b++;
    			if(running1 && status_conductor->conductor->next) status_conductor->conductor=status_conductor->conductor->next; else running1=0;

            }

        	DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

            if(GEIT) printf("4-4 sucessfully activated buttons/tabs\n");

            kill_my_notifys();


            setup_notifys();

            setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);

    	}

    	DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);

    }

        /*setmacro((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Quiet,TRUE);

        while(status_conductor->current_query->entries_count > my_settings.max_lines_for_buffers)
        {
            DoMethod((Object*)status_conductor->current_query->LV_channel,MUIM_NList_Remove,MUIV_NList_Remove_First);
            status_conductor->current_query->entries_count--;
        }

        setmacro((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Quiet,FALSE);
          */

    DoMethod((Object*)status_conductor->current_query->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

    status_conductor->conductor=status_conductor->current_query;

    change_window_titlebar_text();
    if(GEIT) printf("END\n");
    //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, TRUE);


}

//hold down ramiga + 1 to 9 to switch to a tab
//trying to fix geit crash
int switch_between_tabs_with_keys(int result)
{
    //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, FALSE);

    if(GEIT) printf("switching with keys..\n");
    work_query=status_current->current_query;

    //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, FALSE);

    if(DEBUG) printf("switching with keys to tab: %i\n",result);
    //kill_my_notifys();

    //int running1=1;
    int a=0,b=1,d=0,e=0;

    //setmacro(status_current->current_query->BT_querybutton,MUIA_Text_PreParse, "\033c\0332");

    status_conductor=status_root;

    // count how many status tabs there are, and what number to switch to in that tab

    while(status_conductor)
    {
        d++;
        e=0;
        status_conductor->conductor=status_conductor->root;

        while(status_conductor->conductor)
        {
            e++;
            a++;
            if(!status_conductor->conductor->BT_querybutton) result++;
            if(a==result) break;
            status_conductor->conductor=status_conductor->conductor->next;
        }

        if(a==result) break;
        status_conductor=status_conductor->next;

    }

    if(!status_conductor)
    {
        //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, TRUE);

        return 0;
    }
    a=result, b=0;
    //status_conductor=status_root;
    //status_conductor->conductor=status_conductor->root;

    if(status_conductor->conductor==status_current->current_query)
    {
        //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, TRUE);

        return 0;
    }
    kill_my_notifys();

    setmacro(status_current->current_query->BT_querybutton,MUIA_Text_PreParse, "\033c\0332");

    if(DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_InitChange))
	{

        if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
    	{


            for(status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
            {
                for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
                {
                    if(status_conductor->conductor->removed==0) remove_tab_listview2();
                    if(status_conductor->conductor->BT_querybutton) b++;
                    if(b!=a && status_conductor->conductor->BT_querybutton)
                    {
                        //DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_KillNotify,MUIA_Selected);
                        //setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);
                        //DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,status_conductor->iv);

                        /*if(ZUNE_SYSTEM==TRUE && my_settings.use_nlistobject_for_tabs == 0)
                            setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);
                        else if(ZUNE_SYSTEM==FALSE)*/
                            setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);

                    }
                }

            }

            /*if(ZUNE_SYSTEM==TRUE)
            {
                getmacro((Object*)GR_top,MUIA_Group_ChildList,&list);
                object_state = list->mlh_Head;

            	while ((member_object = NextObject( (Object**)&object_state )) )
            	{
                        DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)member_object);

                        if(member_object == status_current->current_query->GR_conductor)
                            status_current->current_query->activity=0;
                }
            }*/
            //remove_tab_listview();


            status_conductor=status_root;
            status_conductor->conductor=status_conductor->root;

            // get to the right status conductor that the selected tab is in

            status_conductor=status_root;
            for(int a=1;a!=d; a++) status_conductor=status_conductor->next;
            if(!status_conductor) { status_conductor=status_root; return 0; }
        	
            status_conductor->conductor=status_conductor->root;
            b=0;
            if(status_conductor)
            {
                while(status_conductor->conductor)
                {
                    b++;
                    if(b>=e) break;
                    status_conductor->conductor=status_conductor->conductor->next;
                }
            }

            activate_tab_button();

        	DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

        }

        kill_my_notifys();

        setup_notifys();

    	DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);
        //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, TRUE);


    }

	setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);
/*
        setmacro((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Quiet,TRUE);

        while(status_conductor->current_query->entries_count > my_settings.max_lines_for_buffers)
        {
            DoMethod((Object*)status_conductor->current_query->LV_channel,MUIM_NList_Remove,MUIV_NList_Remove_First);
            status_conductor->current_query->entries_count--;
        }

        setmacro((Object*)status_conductor->current_query->LV_channel,MUIA_NList_Quiet,FALSE);
  */
    DoMethod((Object*)status_conductor->current_query->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

    change_window_titlebar_text();

    /*if(ZUNE_SYSTEM==TRUE)
    {

        dont_open_colours=TRUE;
        setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, TRUE);
        dont_open_colours=FALSE;
        setmacro((Object*)WookieChat->WI_main,MUIA_Window_Activate, TRUE);

    } */

    //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, TRUE);

    return 0;
}

//trying to fix Geit crashes
int switch_between_tabs_with_nlisttabclick(LONG result, BOOL saving_prefs)
{
    //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, FALSE);
    work_query=status_current->current_query;

    //if(DEBUG) printf("switching to tab: %ld\n",result);
    if(NEWDEBUG) printf("switching to tab: %ld\n",result);


    for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
    {

        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
        {
            if(result==status_conductor->conductor->nlist_tab_number) { if(NEWDEBUG) printf("found %d\n",result); break; }

        }
        if(status_conductor)
        {
            if(status_conductor->conductor)
            {
                if(result==status_conductor->conductor->nlist_tab_number) { if(NEWDEBUG) printf("found %d\n",result); break; }
            }
        }

    }

    //find out if we're switching to the same channel
    for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
        {
            //if(count==result && status_conductor->conductor->BT_querybutton) break;
            if(result==status_conductor->conductor->nlist_tab_number) { if(NEWDEBUG) printf("found %d\n",result); break; }

            if(status_conductor->conductor->removed==0) count++;
        }

        if(status_conductor->conductor)
        {
            //if(count==result && status_conductor->conductor->BT_querybutton) break;
            if(result==status_conductor->conductor->nlist_tab_number) { if(NEWDEBUG) printf("found %d\n",result); break; }
        }

    }


    //work_query=status_conductor->conductor;

    //if(status_conductor->conductor==status_current->current_query) return 0;
    if(!status_conductor || !status_current)
    {
        return 0;
    }
    if(!status_conductor->conductor || !status_current->current_query)
    {
        return 0;
    }
    if(status_conductor->conductor==status_current->current_query && saving_prefs==FALSE)
    {
        return 0;
    }

    if(saving_prefs==TRUE)
    {

        DoMethod((Object*)status_current->current_query->LV_nicklist,MUIM_NList_GetEntry,0, &work_list_entry);

        if(work_list_entry)
        {
            strcpy(string8,work_list_entry->modes);
            strcpy(string9,work_list_entry->name);

            DoMethod((Object*)status_current->current_query->LV_nicklist,MUIM_NList_ReplaceSingle,work_list_entry,0,NOWRAP,ALIGN_LEFT);
        }
    }

    kill_my_notifys();

    if(DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_InitChange))
	{

        // remove the listview objects from the tabs!
        if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
    	{

            for(status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
            {
                for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
                {
                    if(status_conductor->conductor->removed==0) remove_tab_listview2();
                    if(status_conductor->conductor->BT_querybutton)
                    {
                            setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);

                    }
                }

            }


            // get to the right status conductor that the selected tab is in
            for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
            {
                for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
                {
                    //if(count==result && status_conductor->conductor->BT_querybutton) break;
                    if(result==status_conductor->conductor->nlist_tab_number && status_conductor->conductor->BT_querybutton)
                    {
                        printf("activating %d\n",result);
                        activate_tab_button();
                        kill_my_notifys();
                        setup_notifys();

                        break;
                    }

                }

                if(status_conductor->conductor)
                {
                    if(result==status_conductor->conductor->nlist_tab_number && status_conductor->conductor->BT_querybutton) break;
                    //if(count==result && status_conductor->conductor->BT_querybutton) break;
                }

            }


            b=0;

            //activate_tab_button();

        	DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

        }




    	DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);

    }



	setmacro((Object*)WookieChat->WI_main,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);


    DoMethod((Object*)status_conductor->current_query->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);


    change_window_titlebar_text();


    if(saving_prefs==TRUE)
    {
        if(work_list_entry)
        {

            work_list_entry2=new list_entry;

            strcpy(work_list_entry2->modes,string8);
            strcpy(work_list_entry2->name,string9);

            DoMethod((Object*)status_current->current_query->LV_nicklist,MUIM_NList_ReplaceSingle,work_list_entry2,0,NOWRAP,ALIGN_LEFT);

            delete work_list_entry2;
        }
    }


    return 0;
}

void change_display(LONG result)
{
    printf("change display\n");

    if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
    {

        /*for(status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
        {
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
            {
                if(status_conductor->conductor->removed==0) remove_tab_listview2();
                if(status_conductor->conductor->BT_querybutton)
                {
                    // setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);

                    if(ZUNE_SYSTEM==TRUE && my_settings.use_nlistobject_for_tabs == 0)
                        setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);
                    else if(ZUNE_SYSTEM==FALSE)
                        setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, FALSE);

                }
            }

        } */


        // get to the right status conductor that the selected tab is in
        /*for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
        {
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
            {
                if(count==result && status_conductor->conductor->BT_querybutton) break;
                if(status_conductor->conductor->removed==0) count++;
            }

            if(status_conductor->conductor)
            {
                if(count==result && status_conductor->conductor->BT_querybutton) break;
            }

        }


        b=0;
*/
        if(GEIT) printf("2-12 removing GUI objects\n");

        /*if(ZUNE_SYSTEM==TRUE)
        {
            getmacro((Object*)GR_top,MUIA_Group_ChildList,&list);
            object_state = list->mlh_Head;

        	while(( member_object = NextObject( (Object**)&object_state ) ))
        	{
                DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)member_object);

                if(status_current)
                {
                    if(status_current->current_query)
                    {
                        if(member_object == status_current->current_query->GR_conductor)
                            status_current->current_query->activity=0;
                    }
                }
        	}

        }*/
        if(GEIT) printf("3-12 sucessfully removed TAB listview\n");


        //if(ZUNE_SYSTEM==FALSE)
        {
            //if(DEBUG) printf("hiding tabs group in activate_tab();\n");

            if(status_current)
            {
                if(status_current->current_query)
                {
                    //if(status_current->current_query->GR_conductor) setmacro((Object*)status_current->current_query->GR_conductor, MUIA_ShowMe, FALSE);
                    //free_graphical_smilies(status_current->current_query);
                }
            }
        }
        if(GEIT) printf("activate 1\n");
        status_conductor=status_current;
        status_current->conductor=status_current->current_query;

        if(status_conductor->conductor->BT_querybutton)
        {
            if(status_conductor->conductor->BT_querybutton) setmacro((Object*)status_conductor->conductor->BT_querybutton,MUIA_Text_PreParse,"\033c\0332");

            //new change
            DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_KillNotify,MUIA_Selected);
            setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, TRUE);
            DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,status_conductor->conductor->iv);

            sprintf(string7,"\033I[2:00000000,00000000,00000000] %s",status_conductor->conductor->displayed_name);
            if(status_conductor->conductor->BT_querybutton) setmacro((Object*)status_conductor->conductor->BT_querybutton,MUIA_Text_Contents,string7);

            strcpy(status_conductor->conductor->nlist_tab.entry,"         :");
            status_conductor->conductor->nlist_tab.colour=23;
            strcat(status_conductor->conductor->nlist_tab.entry,status_conductor->conductor->displayed_name);
            DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_ReplaceSingle,&status_conductor->conductor->nlist_tab,status_conductor->conductor->nlist_tab_number,0,0);


        }

        if(GEIT) printf("activate 2\n");

        if(status_conductor->conductor->GR_conductor)
        {

            //free_graphical_smilies(status_current->current_query);

            setup_background_colours();

            set_channel_clipboard_hook();

            //add_tabs_to_nicklist_group();

            setmacro((Object*)WookieChat->LV_tabs,MUIA_NList_Active,status_conductor->conductor->nlist_tab_number);

            setmacro((Object*)GR_top,MUIA_Group_ActivePage,status_conductor->conductor->page_number);

            setmacro((Object*)status_conductor->conductor->GR_tabs, MUIA_Weight, my_settings.listview_tabs_weight);

            if(GEIT) printf("activating tab button 3\n");

            if(status_current->current_query->name[0]=='*')
            {
                setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_DisplayHook, &Display_channellist_TextHook);
                setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_Format,"COL=0 WEIGHT=-1 BAR, COL=1 WEIGHT=-1 BAR, COL=2");
            if(GEIT) printf("activating tab button 3a\n");

            }
            else if(my_settings.use_column_display == 0)
            {
                setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_DisplayHook, &DisplayLI_channel_TextHook);
                setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_Format,"");
            if(GEIT) printf("activating tab button 3b\n");
            }
            else if(my_settings.use_column_display)
            {
                setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_DisplayHook, &DisplayLI2_channel_TextHook);
                setmacro((Object*)status_current->current_query->LV_channel,MUIA_NList_Format,listview_format);
            if(GEIT) printf("activating tab button 3c\n");

            }

            if(DEBUG) printf("activating tab\n");
            //use_graphical_smilies(status_current->current_query);

            if(GEIT) printf("finished activating tab button\n");


        }


        if(GEIT) printf("4-12 trying to activate the TABs now\n");


    	DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

    }


}
