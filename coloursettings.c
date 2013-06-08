/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include "intern.h"

struct MUI_PenSpec *pendisplay_specs[25];
STRPTR popimage_background;
STRPTR popimage_nicklistbackground;
STRPTR popimage_tabsbackground;

/* Locals */
static char background2[64];
static char file_name[800];

void set_colours(void)
{
    setmacro((Object*) WookieChat->PP_CSW_background, MUIA_Imagedisplay_Spec, popimage_background);
    strncpy(pendisplay_specs[1]->buf, (char *)popimage_background, 32);
    setmacro((Object*) WookieChat->PP_CSW_background, MUIA_Pendisplay_Spec, pendisplay_specs[1]);

    setmacro((Object*) WookieChat->PP_CSW_nicklistbackground, MUIA_Imagedisplay_Spec, popimage_nicklistbackground);
    strncpy(pendisplay_specs[2]->buf, (char *)popimage_nicklistbackground, 32);
    setmacro((Object*) WookieChat->PP_CSW_nicklistbackground, MUIA_Pendisplay_Spec, pendisplay_specs[2]);

    setmacro((Object*) WookieChat->PP_CSW_listviewtabs_background, MUIA_Imagedisplay_Spec, popimage_tabsbackground);
    strncpy(pendisplay_specs[23]->buf, (char *)popimage_tabsbackground, 32);
    setmacro((Object*) WookieChat->PP_CSW_listviewtabs_background, MUIA_Pendisplay_Spec, pendisplay_specs[23]);

    strncpy(pendisplay_specs[1]->buf, (char *)popimage_background, 32);
    strncpy(pendisplay_specs[2]->buf, (char *)popimage_nicklistbackground, 32);
    strncpy(pendisplay_specs[23]->buf, (char *)popimage_tabsbackground, 32);

    setup_background_colours();

    setmacro((Object*) WookieChat->PP_CSW_join, MUIA_Pendisplay_Spec, pendisplay_specs[3]);
    setmacro((Object*) WookieChat->PP_CSW_part, MUIA_Pendisplay_Spec, pendisplay_specs[4]);
    setmacro((Object*) WookieChat->PP_CSW_quit, MUIA_Pendisplay_Spec, pendisplay_specs[5]);
    setmacro((Object*) WookieChat->PP_CSW_modes, MUIA_Pendisplay_Spec, pendisplay_specs[6]);
    setmacro((Object*) WookieChat->PP_CSW_ctcp, MUIA_Pendisplay_Spec, pendisplay_specs[7]);
    setmacro((Object*) WookieChat->PP_CSW_actions, MUIA_Pendisplay_Spec, pendisplay_specs[8]);
    setmacro((Object*) WookieChat->PP_CSW_normal, MUIA_Pendisplay_Spec, pendisplay_specs[9]);
    setmacro((Object*) WookieChat->PP_CSW_info, MUIA_Pendisplay_Spec, pendisplay_specs[10]);
    setmacro((Object*) WookieChat->PP_CSW_owntext, MUIA_Pendisplay_Spec, pendisplay_specs[11]);
    setmacro((Object*) WookieChat->PP_CSW_highlight, MUIA_Pendisplay_Spec, pendisplay_specs[12]);
    setmacro((Object*) WookieChat->PP_CSW_notices, MUIA_Pendisplay_Spec, pendisplay_specs[13]);
    setmacro((Object*) WookieChat->PP_CSW_invite, MUIA_Pendisplay_Spec, pendisplay_specs[14]);
    setmacro((Object*) WookieChat->PP_CSW_kick, MUIA_Pendisplay_Spec, pendisplay_specs[15]);
    setmacro((Object*) WookieChat->PP_CSW_nickchange, MUIA_Pendisplay_Spec, pendisplay_specs[16]);
    setmacro((Object*) WookieChat->PP_CSW_topic, MUIA_Pendisplay_Spec, pendisplay_specs[17]);
    setmacro((Object*) WookieChat->PP_CSW_wallops, MUIA_Pendisplay_Spec, pendisplay_specs[18]);
    setmacro((Object*) WookieChat->PP_CSW_activity, MUIA_Pendisplay_Spec, pendisplay_specs[19]);
    setmacro((Object*) WookieChat->PP_CSW_chatactivity, MUIA_Pendisplay_Spec, pendisplay_specs[20]);
    setmacro((Object*) WookieChat->PP_CSW_highlightactivity, MUIA_Pendisplay_Spec, pendisplay_specs[21]);
    setmacro((Object*) WookieChat->PP_CSW_nicklisttext, MUIA_Pendisplay_Spec, pendisplay_specs[22]);
    setmacro((Object*) WookieChat->PP_CSW_listviewtabs_normaltext, MUIA_Pendisplay_Spec, pendisplay_specs[24]);

}

void get_colours(void)
{

    //printf("get_colours()\n");

    getmacro((Object*) WookieChat->PP_CSW_background, MUIA_Imagedisplay_Spec, &popimage_background);
    getmacro((Object*) WookieChat->PP_CSW_nicklistbackground, MUIA_Imagedisplay_Spec, &popimage_nicklistbackground);
    getmacro((Object*) WookieChat->PP_CSW_listviewtabs_background, MUIA_Imagedisplay_Spec, &popimage_tabsbackground);

    getmacro((Object*) WookieChat->PP_CSW_background, MUIA_Pendisplay_Spec, &pendisplay_specs[1]);
    getmacro((Object*) WookieChat->PP_CSW_nicklistbackground, MUIA_Pendisplay_Spec, &pendisplay_specs[2]);
    getmacro((Object*) WookieChat->PP_CSW_listviewtabs_background, MUIA_Pendisplay_Spec, &pendisplay_specs[23]);

    getmacro((Object*) WookieChat->PP_CSW_join, MUIA_Pendisplay_Spec, &pendisplay_specs[3]);
    getmacro((Object*) WookieChat->PP_CSW_part, MUIA_Pendisplay_Spec, &pendisplay_specs[4]);
    getmacro((Object*) WookieChat->PP_CSW_quit, MUIA_Pendisplay_Spec, &pendisplay_specs[5]);
    getmacro((Object*) WookieChat->PP_CSW_modes, MUIA_Pendisplay_Spec, &pendisplay_specs[6]);
    getmacro((Object*) WookieChat->PP_CSW_ctcp, MUIA_Pendisplay_Spec, &pendisplay_specs[7]);
    getmacro((Object*) WookieChat->PP_CSW_actions, MUIA_Pendisplay_Spec, &pendisplay_specs[8]);
    getmacro((Object*) WookieChat->PP_CSW_normal, MUIA_Pendisplay_Spec, &pendisplay_specs[9]);
    getmacro((Object*) WookieChat->PP_CSW_info, MUIA_Pendisplay_Spec, &pendisplay_specs[10]);
    getmacro((Object*) WookieChat->PP_CSW_owntext, MUIA_Pendisplay_Spec, &pendisplay_specs[11]);
    getmacro((Object*) WookieChat->PP_CSW_highlight, MUIA_Pendisplay_Spec, &pendisplay_specs[12]);
    getmacro((Object*) WookieChat->PP_CSW_notices, MUIA_Pendisplay_Spec, &pendisplay_specs[13]);
    getmacro((Object*) WookieChat->PP_CSW_invite, MUIA_Pendisplay_Spec, &pendisplay_specs[14]);
    getmacro((Object*) WookieChat->PP_CSW_kick, MUIA_Pendisplay_Spec, &pendisplay_specs[15]);
    getmacro((Object*) WookieChat->PP_CSW_nickchange, MUIA_Pendisplay_Spec, &pendisplay_specs[16]);
    getmacro((Object*) WookieChat->PP_CSW_topic, MUIA_Pendisplay_Spec, &pendisplay_specs[17]);
    getmacro((Object*) WookieChat->PP_CSW_wallops, MUIA_Pendisplay_Spec, &pendisplay_specs[18]);
    getmacro((Object*) WookieChat->PP_CSW_activity, MUIA_Pendisplay_Spec, &pendisplay_specs[19]);
    getmacro((Object*) WookieChat->PP_CSW_chatactivity, MUIA_Pendisplay_Spec, &pendisplay_specs[20]);
    getmacro((Object*) WookieChat->PP_CSW_highlightactivity, MUIA_Pendisplay_Spec, &pendisplay_specs[21]);
    getmacro((Object*) WookieChat->PP_CSW_nicklisttext, MUIA_Pendisplay_Spec, &pendisplay_specs[22]);
    getmacro((Object*) WookieChat->PP_CSW_listviewtabs_normaltext, MUIA_Pendisplay_Spec, &pendisplay_specs[24]);

}
char colours_theme[800];

char colour_settings_title[200];

char work2[1024];
char work3[1024];
char work4[1024];

void load_colours(char *load_this_theme)
{
    if (!load_this_theme)
        sprintf(my_settings.default_colours_theme, "progdir:Colours/default.txt");
    else if (!stricmp(load_this_theme, ""))
        sprintf(my_settings.default_colours_theme, "progdir:Colours/default.txt");
    else
        strcpy(my_settings.default_colours_theme, load_this_theme);

    BPTR open_file = Open((_s_cs)my_settings.default_colours_theme, MODE_OLDFILE);
    if (!open_file)
    {
        strcpy(work2, "2:00000000,00000000,00000000");
        if (work2[0] == 'r')
        {
            memset(background2, '\0', 63);
            strcpy(background2, " ");
            strcat(background2, work2);
            background2[0] = '2';
            background2[1] = ':';
            strcpy(work2, background2);
        }
        popimage_background = (STRPTR)work2;
        strcpy(pendisplay_specs[1]->buf, (char *)popimage_background);

        strcpy(work3, "2:284E284E,3D983D98,91919191");
        if (work3[0] == 'r')
        {
            memset(background2, '\0', 63);
            strcpy(background2, " ");
            strcat(background2, work3);
            background2[0] = '2';
            background2[1] = ':';
            strcpy(work3, background2);
        }
        popimage_nicklistbackground = (STRPTR)work3;
        strcpy(pendisplay_specs[2]->buf, (char *)popimage_nicklistbackground);

        strcpy(pendisplay_specs[3]->buf, "r521D521D,FFFFFFFF,2D9C2D9C");

        strcpy(pendisplay_specs[4]->buf, "r209C209C,A1AFA1AF,1A271A27");

        strcpy(pendisplay_specs[5]->buf, "rE7D8E7D8,25A425A4,FFFFFFFF");

        strcpy(pendisplay_specs[6]->buf, "rFFFFFFFF,8FC08FC0,12D112D1");

        strcpy(pendisplay_specs[7]->buf, "rFFFFFFFF,0D620D62,00000000");

        strcpy(pendisplay_specs[8]->buf, "rFFFF0000,FFFF0000,FFFF0000");

        strcpy(pendisplay_specs[9]->buf, "rFFFDFFFD,FFFBFFFB,FFFFFFFF");

        strcpy(pendisplay_specs[10]->buf, "rFFFFFFFF,AEACAEAC,D159D159");

        strcpy(pendisplay_specs[11]->buf, "r7C3B7C3B,FFFFFFFF,F8CEF8CE");

        strcpy(pendisplay_specs[12]->buf, "rFFFFFFFF,FBCCFBCC,25A225A2");

        strcpy(pendisplay_specs[13]->buf, "rFFFFFFFF,1A591A59,25292529");

        strcpy(pendisplay_specs[14]->buf, "rFFFFFFFF,12D012D0,168E168E");

        strcpy(pendisplay_specs[15]->buf, "rFFFFFFFF,24CE24CE,12D212D2");

        strcpy(pendisplay_specs[16]->buf, "rFFFFFFFF,8F098F09,1E1D1E1D");

        strcpy(pendisplay_specs[17]->buf, "rFFFFFFFF,96409640,34B234B2");

        strcpy(pendisplay_specs[18]->buf, "r61E061E0,FFFFFFFF,8CDC8CDC");

        strcpy(pendisplay_specs[19]->buf, "r43C043C0,508F508F,FFFFFFFF");

        strcpy(pendisplay_specs[20]->buf, "rFFFFFFFF,07850785,0B360B36");

        strcpy(pendisplay_specs[21]->buf, "r54F454F4,EBCAEBCA,299B299B");

        strcpy(pendisplay_specs[22]->buf, "rFFFDFFFD,FFFCFFFC,FFFFFFFF");

        strcpy(work4, "2:DE50DE50,DE4ADE4A,DE47DE47");
        if (work4[0] == 'r')
        {
            memset(background2, '\0', 63);
            strcpy(background2, " ");
            strcat(background2, work4);
            background2[0] = '2';
            background2[1] = ':';
            strcpy(work4, background2);
        }
        popimage_tabsbackground = (STRPTR)work4;

        strcpy(pendisplay_specs[23]->buf, (char *)popimage_tabsbackground);

        strcpy(pendisplay_specs[24]->buf, "2:00000000,00000000,00000000");

        set_colours();

        return;
    }

    //strcpy(my_settings.default_colour_theme,colours_theme);

	sprintf(colour_settings_title, "%s %s", (char *) LGS(MSG_COLOUR_SETTINGS_WINDOW_TITLE),
            my_settings.default_colours_theme);
    setmacro((Object*) WookieChat->WI_colour_settings, MUIA_Window_Title, colour_settings_title);

    char *len2;
    char *work = malloc(sizeof(char) * 1024);

    len2 = (char*) FGets(open_file, (STRPTR) work2, 1024);
    work2[strlen(work2) - 1] = '\0';
    if (!len2)
        strcpy(work2, "2:00000000,00000000,00000000");
    if (work2[0] == 'r')
    {
        memset(background2, '\0', 63);
        strcpy(background2, " ");
        strcat(background2, work2);
        background2[0] = '2';
        background2[1] = ':';
        strcpy(work2, background2);
    }
    popimage_background = (STRPTR)work2;
    strcpy(pendisplay_specs[1]->buf, (char *)popimage_background);

    len2 = (char*) FGets(open_file, (STRPTR) work3, 1024);
    work3[strlen(work3) - 1] = '\0';
    if (!len2)
        strcpy(work3, "2:284E284E,3D983D98,91919191");
    if (work3[0] == 'r')
    {
        memset(background2, '\0', 63);
        strcpy(background2, " ");
        strcat(background2, work3);
        background2[0] = '2';
        background2[1] = ':';
        strcpy(work3, background2);
    }
    popimage_nicklistbackground = (STRPTR)work3;
    strcpy(pendisplay_specs[2]->buf, (char *)popimage_nicklistbackground);

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[3]->buf, work);
    else
        strcpy(pendisplay_specs[3]->buf, "r521D521D,FFFFFFFF,2D9C2D9C");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[4]->buf, work);
    else
        strcpy(pendisplay_specs[4]->buf, "r209C209C,A1AFA1AF,1A271A27");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[5]->buf, work);
    else
        strcpy(pendisplay_specs[5]->buf, "rE7D8E7D8,25A425A4,FFFFFFFF");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[6]->buf, work);
    else
        strcpy(pendisplay_specs[6]->buf, "rFFFFFFFF,8FC08FC0,12D112D1");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[7]->buf, work);
    else
        strcpy(pendisplay_specs[7]->buf, "rFFFFFFFF,0D620D62,00000000");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[8]->buf, work);
    else
        strcpy(pendisplay_specs[8]->buf, "rFFFF0000,FFFF0000,FFFF0000");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[9]->buf, work);
    else
        strcpy(pendisplay_specs[9]->buf, "rFFFDFFFD,FFFBFFFB,FFFFFFFF");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[10]->buf, work);
    else
        strcpy(pendisplay_specs[10]->buf, "rFFFFFFFF,AEACAEAC,D159D159");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[11]->buf, work);
    else
        strcpy(pendisplay_specs[11]->buf, "r7C3B7C3B,FFFFFFFF,F8CEF8CE");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[12]->buf, work);
    else
        strcpy(pendisplay_specs[12]->buf, "rFFFFFFFF,FBCCFBCC,25A225A2");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[13]->buf, work);
    else
        strcpy(pendisplay_specs[13]->buf, "rFFFFFFFF,1A591A59,25292529");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[14]->buf, work);
    else
        strcpy(pendisplay_specs[14]->buf, "rFFFFFFFF,12D012D0,168E168E");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[15]->buf, work);
    else
        strcpy(pendisplay_specs[15]->buf, "rFFFFFFFF,24CE24CE,12D212D2");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[16]->buf, work);
    else
        strcpy(pendisplay_specs[16]->buf, "rFFFFFFFF,8F098F09,1E1D1E1D");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[17]->buf, work);
    else
        strcpy(pendisplay_specs[17]->buf, "rFFFFFFFF,96409640,34B234B2");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[18]->buf, work);
    else
        strcpy(pendisplay_specs[18]->buf, "r61E061E0,FFFFFFFF,8CDC8CDC");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[19]->buf, work);
    else
        strcpy(pendisplay_specs[19]->buf, "r43C043C0,508F508F,FFFFFFFF");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[20]->buf, work);
    else
        strcpy(pendisplay_specs[20]->buf, "rFFFFFFFF,07850785,0B360B36");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[21]->buf, work);
    else
        strcpy(pendisplay_specs[21]->buf, "r54F454F4,EBCAEBCA,299B299B");

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[22]->buf, work);
    else
        strcpy(pendisplay_specs[22]->buf, "rFFFDFFFD,FFFCFFFC,FFFFFFFF");

    len2 = (char*) FGets(open_file, (STRPTR) work4, 1024);
    work4[strlen(work4) - 1] = '\0';
    if (!len2)
        strcpy(work4, "2:DE50DE50,DE4ADE4A,DE47DE47");
    if (work4[0] == 'r')
    {
        memset(background2, '\0', 63);
        strcpy(background2, " ");
        strcat(background2, work4);
        background2[0] = '2';
        background2[1] = ':';
        strcpy(work4, background2);
    }
    popimage_tabsbackground = (STRPTR)work4;
    strcpy(pendisplay_specs[23]->buf, (char *)popimage_tabsbackground);

    len2 = (char*) FGets(open_file, (STRPTR) work, 32);
    work[strlen(work) - 1] = '\0';
    if (len2)
        strcpy(pendisplay_specs[24]->buf, work);
    else
        strcpy(pendisplay_specs[24]->buf, "2:00000000,00000000,00000000");

    if (open_file)
        Close(open_file);

    set_colours();

}

void save_colours(void)
{

    //strcpy(my_settings.default_colours_theme,colours_theme);
    get_colours();

    //BPTR save_file = Open("progdir:Colours/default.txt",MODE_NEWFILE);
    BPTR save_file = Open((_s_cs)my_settings.default_colours_theme, MODE_NEWFILE);
    if (!save_file)
    {
        if (wookie_folder[strlen(wookie_folder) - 1] == ':')
            sprintf(file_name, "%sColours", wookie_folder);
        else
            sprintf(file_name, "%s/Colours", wookie_folder);

        BPTR created_dir_lock = CreateDir((_s_cs)file_name);
        UnLock(created_dir_lock);

        //save_file = Open("progdir:Colours/default.txt",MODE_NEWFILE);
        save_file = Open((_s_cs)my_settings.default_colours_theme, MODE_NEWFILE);
        //printf("saving %s\n",my_settings.default_colours_theme);

        //if(!save_file) printf("%s\n\n",GetCatalogStr(catalog,197,"Unable to save your colour settings. Make sure you have a drawer called 'Colours' inside WookieChat's drawer, for it to save your colour preferences to."));
        if (!save_file)
            return;
    }

	sprintf(colour_settings_title, "%s %s", (char *) LGS( MSG_COLOUR_SETTINGS_WINDOW_TITLE ),
            my_settings.default_colours_theme);
    setmacro((Object*) WookieChat->WI_colour_settings, MUIA_Window_Title, colour_settings_title);

    FPuts(save_file, (l_in) popimage_background);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) popimage_nicklistbackground);
    FPuts(save_file, (l_in) "\n");

    //printf("popimage_background:%s\n", popimage_background);
    /*
     FPuts(save_file,(l_in)pendisplay_specs[1]->buf); FPuts(save_file,(l_in)"\n");

     FPuts(save_file,(l_in)pendisplay_specs[2]->buf); FPuts(save_file,(l_in)"\n");
     */

    FPuts(save_file, (l_in) pendisplay_specs[3]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[4]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[5]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[6]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[7]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[8]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[9]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[10]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[11]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[12]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[13]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[14]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[15]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[16]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[17]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[18]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[19]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[20]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[21]->buf);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[22]->buf);
    FPuts(save_file, (l_in) "\n");

    //FPuts(save_file,(l_in)pendisplay_specs[23]->buf); FPuts(save_file,(l_in)"\n");
    FPuts(save_file, (l_in) popimage_tabsbackground);
    FPuts(save_file, (l_in) "\n");

    FPuts(save_file, (l_in) pendisplay_specs[24]->buf);
    FPuts(save_file, (l_in) "\n");

    Close(save_file);

}

