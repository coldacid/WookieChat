/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include <proto/muimaster.h>

#include "intern.h"
#include "objapp.h"

int total_smileys, preview_total_smileys;

struct smilies_struct smilies[MAXIMUM_SMILEYS];
struct smilies_struct preview_smilies[MAXIMUM_SMILEYS];

/* Locals */
static char string7[900];
static char *string1;

int find_themes()
{
    count = 0;

    char work_buffer2[500];
#ifdef __amigaos4__
    uint32 EAData[1024*10];
#else
    int EAData[1024 * 10];
#endif

    struct ExAllData *ead;
    BOOL more;

    struct ExAllControl *eac = (struct ExAllControl*) AllocDosObject(DOS_EXALLCONTROL, NULL);
    if (!eac)
    {
        return 0;
    }
    eac->eac_LastKey = 0;

    BPTR my_lock = Lock((_s_cs)"progdir:smilies", ACCESS_READ);

    if (!my_lock)
    {
        printf("Unable to obtain a lock on smilies/ directory\n");
        return 0;
    }

    do
    {
        more = ExAll(my_lock, (struct ExAllData*) EAData, sizeof(EAData), ED_TYPE, (struct ExAllControl*) eac);
        if ((!more) && (IoErr() != ERROR_NO_MORE_ENTRIES))
        {
            //printf("failed abnormally with some other error\n");
            break;

        }
        if (eac->eac_Entries == 0)
        {
            //printf("failed normally with no entries\n");
            //return 0;
            break;
        }
        for (ead = (struct ExAllData *) EAData; ead; ead = ead->ed_Next)
        {
            if (EAD_IS_FILE(ead))
            {
                if (strstr((char*) ead->ed_Name, ".theme"))
                {
                    if (wookie_folder[strlen(wookie_folder) - 1] == ':'
                            || wookie_folder[strlen(wookie_folder) - 1] == '/')
                        sprintf(work_buffer, "%s", wookie_folder);
                    else
                        sprintf(work_buffer, "%s/", wookie_folder);

                    if (work_buffer[strlen(work_buffer) - 1] == ':')
                        sprintf(work_buffer2, "%s%s", work_buffer, (char*) ead->ed_Name);
                    else
                        sprintf(work_buffer2, "%s/%s", work_buffer, (char*) ead->ed_Name);

                    strcpy(graphical_smiley_themes[count], (char*) ead->ed_Name);

                    count++;

                }
                if (count >= 10)
                    break;

                //strcpy(graphical_smiley_themes[count],"");

            }

        }

    } while (more);

    FreeDosObject(DOS_EXALLCONTROL, eac);
    if (my_lock)
        UnLock(my_lock);
    return 1;

}

int free_graphical_smilies(struct query_window *new_tab)
{
    //return 0;

    if (DEBUG)
        printf("freeing all graphical smilies() so they can be used in a new tab..\n");
    //if(new_tab==status_conductor->conductor)
    /*if(new_tab == status_conductor->conductor)
     {
     printf("not doing free_graphical_smilies()\n");
     work_query=status_conductor->conductor;
     return 0;
     } */

    if (my_settings.graphical_smilies == TRUE)
    {

        for (count = 1; count <= total_smileys; count++)
        {
            if (smilies[count].icon)
            {

                DoMethod((Object*) new_tab->LV_channel, MUIM_NList_UseImage, NULL, count, 0L);
                //smilies[count].icon=NULL;
            }
        }

    }

    if (my_settings.graphical_nicklist == TRUE)
    {

        if (graphical_nicklist[0].icon)
            DoMethod((Object*) new_tab->LV_nicklist, MUIM_NList_UseImage, NULL, 0, 0L);

        if (graphical_nicklist[1].icon)
            DoMethod((Object*) new_tab->LV_nicklist, MUIM_NList_UseImage, NULL, 1, 0L);

        if (graphical_nicklist[2].icon)
            DoMethod((Object*) new_tab->LV_nicklist, MUIM_NList_UseImage, NULL, 2, 0L);

    }

    if (my_settings.graphical_nicklist)
        setmacro(
                (Object*) new_tab->LV_nicklist,
                MUIA_ShortHelp,
                GCS(
                        catalog,
                        319,
                        "Graphical Usermodes Key:\nGreen = Channel Operators or @\nBlue = HalfOps or \%\nYellow = Voiced Users or +"));
    else
        setmacro((Object*) new_tab->LV_nicklist, MUIA_ShortHelp, "The list of users on this channel");

    return 0;

}

int use_graphical_smilies(struct query_window *new_tab)
{

    if (DEBUG)
        printf("use_graphical_smilies()\n");

    if (my_settings.graphical_smilies == TRUE)
    {

        for (count = 1; count <= total_smileys; count++)
        {
            if (smilies[count].icon)
            {
                //printf("using graphical smiley %d\n",count);
                DoMethod((Object*) new_tab->LV_channel, MUIM_NList_UseImage, NULL, count, 0L);
                DoMethod((Object*) new_tab->LV_channel, MUIM_NList_UseImage, smilies[count].icon, count, 0L);
            }
        }

    }

    if (my_settings.graphical_nicklist == TRUE)
    {

        DoMethod((Object*) new_tab->LV_nicklist, MUIM_NList_UseImage, NULL, 0, 0L);
        DoMethod((Object*) new_tab->LV_nicklist, MUIM_NList_UseImage, NULL, 1, 0L);
        DoMethod((Object*) new_tab->LV_nicklist, MUIM_NList_UseImage, NULL, 2, 0L);

        if (graphical_nicklist[0].icon)
            DoMethod((Object*) new_tab->LV_nicklist, MUIM_NList_UseImage, graphical_nicklist[0].icon, 0, 0L);

        if (graphical_nicklist[1].icon)
            DoMethod((Object*) new_tab->LV_nicklist, MUIM_NList_UseImage, graphical_nicklist[1].icon, 1, 0L);

        if (graphical_nicklist[2].icon)
            DoMethod((Object*) new_tab->LV_nicklist, MUIM_NList_UseImage, graphical_nicklist[2].icon, 2, 0L);

    }

    if (my_settings.graphical_nicklist)
        setmacro(
                (Object*) new_tab->LV_nicklist,
                MUIA_ShortHelp,
                GCS(
                        catalog,
                        319,
                        "Graphical Usermodes Key:\nGreen = Channel Operators or @\nBlue = HalfOps or \%\nYellow = Voiced Users or +"));
    else
        setmacro((Object*) new_tab->LV_nicklist, MUIA_ShortHelp, "The list of users on this channel");

    return 0;

}

void exit_delete_smiley_objects()
{

    if (DEBUG)
        printf("exit_delete_smiley_objects()\n");

    getmacro((Object*) WookieChat->GR_graphical_smileys_choose, MUIA_Group_ChildList, &list);
    object_state = list->mlh_Head;
    while ((member_object = NextObject((Object**) &object_state)))
    {
        if (member_object)
        {
            DoMethod((Object*) WookieChat->GR_graphical_smileys_choose, OM_REMMEMBER, (Object*) member_object);
            MUI_DisposeObject((Object*) member_object);
            member_object = NULL;
        }
    }

    if (WookieChat->smiley_choose_icon)
    {
        DoMethod((Object*) GR_string_gadget, OM_REMMEMBER, (Object*) WookieChat->smiley_choose_icon);
        MUI_DisposeObject((Object*) WookieChat->smiley_choose_icon);
    }

    for (count = 1; count <= total_smileys; count++)
    {

        if (smilies[count].icon)
            MUI_DisposeObject((Object*) smilies[count].icon);

    }

    for (count = 1; count <= preview_total_smileys; count++)
    {

        if (preview_smilies[count].icon)
            MUI_DisposeObject((Object*) preview_smilies[count].icon);

    }

    for (count = 0; count <= 2; count++)
    {
        if (graphical_nicklist[count].icon)
        {
            //if(DEBUG) printf("exit_delete_smiley_objects() 2\n");

            MUI_DisposeObject((Object*) graphical_nicklist[count].icon);
            graphical_nicklist[count].icon = NULL;
        }
    }
}

void delete_smiley_objects()
{

    if (status_conductor)
    {
        work_status = status_conductor;

        if (status_conductor->conductor)
            work_query = status_conductor->conductor;

    }

    for (status_conductor = status_root; status_conductor; status_conductor = status_conductor->next)
    {
        for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                status_conductor->conductor = status_conductor->conductor->next)
        {
            for (count = 1; count <= total_smileys; count++)
            {

                if (status_conductor->conductor->LV_channel)
                    DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_UseImage, NULL, count, 0L);

            }
            if (status_conductor->conductor->LV_nicklist)
            {
                if (DEBUG)
                    printf("delete_smiley_objects()\n");

                DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_UseImage, NULL, 0, 0L);
                DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_UseImage, NULL, 1, 0L);
                DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_UseImage, NULL, 2, 0L);

            }
        }

    }

    //status_conductor=work_status;
    //if(status_conductor) status_conductor->conductor=work_query;

    for (count = 1; count <= total_smileys; count++)
    {

        if (smilies[count].icon)
        {
            MUI_DisposeObject((Object*) smilies[count].icon);
            smilies[count].icon = NULL;
        }
    }
    for (count = 0; count <= 2; count++)
    {
        if (graphical_nicklist[count].icon)
        {
            if (DEBUG)
                printf("delete_smiley_objects() 2\n");

            MUI_DisposeObject((Object*) graphical_nicklist[count].icon);
            graphical_nicklist[count].icon = NULL;
        }
    }
    status_conductor = status_root;
    if (status_conductor)
        status_conductor->conductor = status_conductor->root;
}

void delete_preview_smiley_objects()
{

    for (count = 1; count <= preview_total_smileys; count++)
    {

        DoMethod((Object*) WookieChat->LV_graphical_smileys_preview, MUIM_NList_UseImage, NULL, count, 0L);
        if (preview_smilies[count].icon)
            MUI_DisposeObject((Object*) preview_smilies[count].icon);

    }

}

int load_graphical_smilies(void)
{

    if (DEBUG)
        printf("opening graphical smilies\n");

    if (my_settings.graphical_smilies == TRUE)
    {
        sprintf(work_buffer, "progdir:smilies/%s", graphical_smiley_themes[my_settings.graphical_smilies_background]);
        if (DEBUG)
            printf("opening \"%s\"\n", work_buffer);
        newbptr_file = Open((_s_cs)work_buffer, MODE_OLDFILE);

        if (newbptr_file)
        {
            getmacro((Object*) WookieChat->GR_graphical_smileys_choose, MUIA_Group_ChildList, &list);
            object_state = list->mlh_Head;
            while ((member_object = NextObject((Object**) &object_state)))
            {
                if (member_object)
                {
                    DoMethod((Object*) WookieChat->GR_graphical_smileys_choose, OM_REMMEMBER, (Object*) member_object);
                    MUI_DisposeObject((Object*) member_object);
                }
            }

            delete_smiley_objects();

            total_smileys = 0;
            BOOL at_start = TRUE;

            while (FGets(newbptr_file, (STRPTR) work_buffer, 900))
            {
                if (work_buffer[0] != '#' && at_start == TRUE)
                    break;

                at_start = FALSE;

                if (work_buffer[0] != '#')
                {
                    total_smileys++;
                    //printf("line %s",work_buffer);
                    string1 = strtok(work_buffer, " ");
                    if (string1)
                    {
                        sprintf(smilies[total_smileys].filename, "%s", string1);
                        if (DEBUG)
                            printf("Opening #%d, \"%s\"\n", total_smileys, smilies[total_smileys].filename);

                        //attempt to free up images when quit

#ifdef __amigaos4__
                        smilies[total_smileys].icon=(Object*)NBitmapFile(smilies[total_smileys].filename);
                        smilies[total_smileys].choose_icon=(Object*)NBitmapFile(smilies[total_smileys].filename);
                        /*#elif __MORPHOS__
                         smilies[total_smileys].icon=(Object*)NBitmapFile(smilies[total_smileys].filename);
                         smilies[total_smileys].choose_icon=(Object*)NBitmapFile(smilies[total_smileys].filename);
                         */
#else
                        smilies[total_smileys].icon=(Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, smilies[total_smileys].filename, End;
                                smilies[total_smileys].choose_icon=(Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, smilies[total_smileys].filename, End;
#endif

                                        if (!smilies[total_smileys].choose_icon)
                                        {
                                            //if(DEBUG) printf("cant open a graphical smiley! turning off graphical smileys now..\n");
                                            //my_settings.graphical_smilies=FALSE;
                                            //Close(newbptr_file);

                                            //return 0;

                                        }

                                        setmacro((Object*) smilies[total_smileys].choose_icon, MUIA_InputMode,
                                                MUIV_InputMode_RelVerify);
                                        DoMethod((Object*) smilies[total_smileys].choose_icon, MUIM_Notify,
                                                MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
                                                MUIM_Application_ReturnID, GRAPHICAL_SMILEY_VALUES + total_smileys);

                                        if (total_smileys == 1)
                                        {

                                            if (DoMethod((Object*) GR_string_gadget, MUIM_Group_InitChange))
                                            {

                                                getmacro((Object*) GR_string_gadget, MUIA_Group_ChildList, &list);
                                                object_state = list->mlh_Head;
                                                while ((member_object = NextObject((Object**) &object_state)))
                                                {
                                                    if (member_object == WookieChat->smiley_choose_icon)
                                                    {

                                                        if (member_object)
                                                            DoMethod((Object*) member_object, MUIM_KillNotify,
                                                                    MUIA_Selected);

                                                        DoMethod((Object*) GR_string_gadget, OM_REMMEMBER,
                                                                (Object*) member_object);
                                                    }
                                                }

                                                //attempt to free up images when quit
                                                if (WookieChat->smiley_choose_icon)
                                                    MUI_DisposeObject((Object*) WookieChat->smiley_choose_icon);

#ifdef __amigaos4__
                                                WookieChat->smiley_choose_icon=(Object*)NBitmapFile(smilies[total_smileys].filename);
                                                /*#elif  __MORPHOS__
                                                 WookieChat->smiley_choose_icon=(Object*)NBitmapFile(smilies[total_smileys].filename);
                                                 */
#else
                                                WookieChat->smiley_choose_icon=(Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, smilies[total_smileys].filename, End;
#endif

                                                        if (WookieChat->smiley_choose_icon)
                                                        {
                                                            setmacro((Object*) WookieChat->smiley_choose_icon,
                                                                    MUIA_InputMode, MUIV_InputMode_RelVerify);

                                                            DoMethod((Object*) GR_string_gadget, OM_ADDMEMBER,
                                                                    (Object*) WookieChat->smiley_choose_icon);

                                                            DoMethod((Object*) WookieChat->smiley_choose_icon,
                                                                    MUIM_Notify, MUIA_Selected, FALSE,
                                                                    (Object*) WookieChat->App, 2,
                                                                    MUIM_Application_ReturnID, 95);
                                                        }
                                                        DoMethod((Object*) GR_string_gadget, MUIM_Group_ExitChange);
                                                    }

                                                    if (DEBUG)
                                                        printf("setup our CHOOSER icon..\n");

                                                }

                                                //if(DEBUG) printf("adding smiley %d to the chooser popup window..\n",total_smileys);

                                                if (smilies[total_smileys].choose_icon)
                                                    DoMethod((Object*) WookieChat->GR_graphical_smileys_choose,
                                                            OM_ADDMEMBER, (Object*) smilies[total_smileys].choose_icon);

                                            }
                                            smilies[total_smileys].ascii_total = 0;
                                            string1 = strtok(NULL, "\n ");
                                            while (string1)
                                            {
                                                sprintf(
                                                        smilies[total_smileys].ascii[smilies[total_smileys].ascii_total],
                                                        "%s ", string1);
                                                if (smilies[total_smileys].ascii_total == 0)
                                                    sprintf(smilies[total_smileys].remove_ascii, "%s", string1);
                                                smilies[total_smileys].ascii_total++;
                                                string1 = strtok(NULL, "\n ");
                                                if (smilies[total_smileys].ascii_total >= MAXIMUM_SMILEY_ASCII)
                                                { /*smilies[total_smileys].ascii_total--;*/
                                                    break;
                                                }

                                            }
                                            if (total_smileys >= MAXIMUM_SMILEYS)
                                                break;
                                        }
                                    }

                                    Close(newbptr_file);

                                }

                            }

                            if (my_settings.graphical_nicklist == TRUE)
                            {

#ifdef __amigaos4__
                                graphical_nicklist[0].icon=(Object*)NBitmapFile("progdir:smilies/nicklist/ops");
                                graphical_nicklist[1].icon=(Object*)NBitmapFile("progdir:smilies/nicklist/half_ops");
                                graphical_nicklist[2].icon=(Object*)NBitmapFile("progdir:smilies/nicklist/voice");
#else
graphical_nicklist                            [0].icon=(Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:smilies/nicklist/ops", End;
                                    graphical_nicklist[1].icon=(Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:smilies/nicklist/half_ops", End;
                                            graphical_nicklist[2].icon=(Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:smilies/nicklist/voice", End;
#endif

                                                }

    return 0;

}

char * remove_smiley(char *work_buffer, char *ascii_smiley, int smiley_gfx_number)
{

    if (!work_buffer)
        return NULL;

    char *graphical_string1;
    char display_hook_breakup2[800];

    sprintf(string7, "\033o[%d]", smiley_gfx_number);
    graphical_string1 = strstr((char*) work_buffer, string7);
    while (graphical_string1)
    {
        strcpy(display_hook_breakup2, graphical_string1 + 5);
        strcpy(graphical_string1, "");
        strcat(graphical_string1, ascii_smiley);
        strcat(work_buffer, display_hook_breakup2);
        graphical_string1 = strstr((char*) work_buffer, string7);

    }

    return work_buffer;
}

char * convert_graphical_smilies_2_text(char *work_buffer)
{

    if (my_settings.graphical_smilies)
    {

        for (count4 = 1; count4 <= total_smileys && count4 <= MAXIMUM_SMILEYS; count4++)
        {

            remove_smiley(work_buffer, smilies[count4].remove_ascii, count4);

        }

    }

    return work_buffer;

}

void insert_smiley(char *ascii_smiley, int smiley_gfx_number)
{

    count = strlen(centry->entry);
    count2 = strlen(ascii_smiley);

    //printf("start of insert smiley\n");

    for (int a = count - 1, b = count2 - 2, c = 0; a > 0 && c < count2 - 1; a--, b--, c++)
    {
        if (centry->entry[a] != ascii_smiley[b])
            break;

        if (centry->entry[a] == ascii_smiley[b] && c == count2 - 2)
        {
            centry->entry[count - count2 + 1] = '\0';
            sprintf(work_buffer, "\033o[%d] ", smiley_gfx_number);
            //sprintf(work_buffer,"\033o[1] ");
            strcat(centry->entry, work_buffer);
            //if(DEBUG) printf("Putting a smiley in the display. Hard coded now to smiley Number #1!\n");

        }

    }

    graphical_string1 = strstr((char*) centry->entry, ascii_smiley);
    while (graphical_string1)
    {
        strcpy(display_hook_breakup2, graphical_string1 + strlen(ascii_smiley));
        sprintf(graphical_string1, "\033o[%d] ", smiley_gfx_number);
        //sprintf(graphical_string1,"\033o[1] ");
        strcat(centry->entry, display_hook_breakup2);
        graphical_string1 = strstr((char*) centry->entry, ascii_smiley);
        //if(DEBUG) printf("Putting a smiley in the display. Hard coded now to smiley Number #1!\n");
    }
}

void insert_graphical_smilies(void)
{

    if (my_settings.graphical_smilies)
    {

        for (count4 = 1; count4 <= total_smileys && count4 <= MAXIMUM_SMILEYS; count4++)
        {
            for (count3 = 0; count3 < smilies[count4].ascii_total && count3 <= MAXIMUM_SMILEY_ASCII; count3++)
            {
                insert_smiley(smilies[count4].ascii[count3], count4);

            }
        }

    }

}
