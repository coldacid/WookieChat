/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/* ===================================================================
                           Start-Exit Code
   =================================================================== */
#include "includes.h"

#include <clib/alib_protos.h>
#include <proto/muimaster.h>
#include <proto/icon.h>
#include <proto/codesets.h>

#include "intern.h"
#include "objapp.h"
#include "audio.h"

#ifdef __AROS__
struct Library * AslBase = NULL;
struct Library * IconBase = NULL;
struct LocaleBase * LocaleBase = NULL;
struct GfxBase * GfxBase = NULL;
struct UtilityBase * UtilityBase = NULL;
struct Library * MUIMasterBase = NULL;
struct IntuitionBase * IntuitionBase = NULL;
struct Library * SocketBase = NULL;
struct Library * DataTypesBase = NULL;
struct Library * CodesetsBase = NULL;
#elif __amigaos4__
struct Library * AslBase = NULL;
struct Library * IconBase = NULL;
struct Library * LocaleBase = NULL;
struct Library * GfxBase = NULL;
struct Library * UtilityBase = NULL;
struct Library * MUIMasterBase = NULL;
struct Library * IntuitionBase = NULL;
struct Library * SocketBase = NULL;
struct Library * DataTypesBase = NULL;
struct Library * CodesetsBase = NULL;
#elif __MORPHOS__
struct Library * AslBase = NULL;
struct Library * IconBase = NULL;
struct Library * LocaleBase = NULL;
struct GfxBase * GfxBase = NULL;
struct Library * UtilityBase = NULL;
struct Library * MUIMasterBase = NULL;
struct IntuitionBase * IntuitionBase = NULL;
struct Library * SocketBase = NULL;
struct Library * DataTypesBase = NULL;
struct Library * CodesetsBase = NULL;
#else
struct Library * AslBase = NULL;
struct Library * IconBase = NULL;
struct Library * LocaleBase = NULL;
struct Library * GfxBase = NULL;
struct Library * UtilityBase = NULL;
struct Library * MUIMasterBase = NULL;
struct Library * IntuitionBase = NULL;
struct Library * SocketBase = NULL;
struct Library * DataTypesBase = NULL;
struct Library * CodesetsBase = NULL;
#endif

#ifdef __AROS__
typedef CONST_STRPTR nml;
#else
typedef STRPTR nml;
#endif

//the menu is left blank so we can use localised strings from a catalog.
//the code for inserting the localised text is in LoadAllLibs() in
//startexit.cpp
struct NewMenu MenuData1[] =
{
    { NM_TITLE, (nml)""   ,0,0,0  ,(APTR)0      },
    { NM_ITEM,  (nml)""   ,0,0,0  ,(APTR)65     },
    { NM_ITEM,  (nml)""   ,0,0,0  ,(APTR)66     },
    { NM_ITEM,  (nml)""   ,0,0,0  ,(APTR)80     },
    { NM_ITEM,  (nml)""   ,0,0,0  ,(APTR)0      },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)75     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)76     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)77     },
    { NM_ITEM,  (nml)""   ,0,0,0  ,(APTR)0      },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)79     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)81     },
    { NM_ITEM,  (nml)""   ,0,0,0  ,(APTR)0      },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)67     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)68     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)69     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)70     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)71     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)72     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)73     },
    { NM_SUB,   (nml)""   ,0,0,0  ,(APTR)83     },

    { NM_END,NULL,0,0,0,(APTR)0 },
};

struct DiskObject *dobj; //for the iconified icon

/* Locals */
static char *string1;
static char file_name[800];

void cleanexit(char*);


void LoadAllLibs(void)
{
    int a;

    centry = malloc(sizeof(struct channel_entry));

    nicklist_init();

    fdmax = -1;

    local_charset = 0;

    QUIET_DCC = FALSE;
    USE_AREXX = FALSE;
    set_sound_muted(FALSE);

    samples_init();

    for (a = 0; a < 25; a++)
        custom_pen_colours[a] = 0;
    for (a = 0; a < 25; a++)
        pendisplay_specs[a] = malloc(sizeof(struct MUI_PenSpec));

    //default values before tooltypes and shell switches are read
    my_settings.os3_about_window_gfx = 0;

#ifdef __amigaos4__
    DataTypesBase = (struct Library*)OpenLibrary((_ub_css)"datatypes.library",0);
    if(!DataTypesBase) cleanexit((char*)"cant open datatypes.library\n");

    AslBase = (struct Library*)OpenLibrary((_ub_cs)"asl.library",0);
    if(!AslBase) cleanexit((char*)"cant open asl.library\n");

    IconBase = (struct Library*)OpenLibrary((_ub_cs)"icon.library",0);
    if(!IconBase) cleanexit((char*)"cant open icon.library\n");

    LocaleBase = (struct Library*)OpenLibrary((_ub_cs)"locale.library",0);
    if(!LocaleBase) cleanexit((char*)"cant open locale.library\n");

    GfxBase = (struct Library*)OpenLibrary((_ub_cs)"graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    UtilityBase = (struct Library*)OpenLibrary((_ub_cs)"utility.library",0);
    if(!UtilityBase) cleanexit((char*)"cant open utility.library\n");

    SocketBase = OpenLibrary((_ub_cs)"bsdsocket.library",0L);
    if(!SocketBase) cleanexit((char*)"cant open bsdsocket.library\n");

    MUIMasterBase = OpenLibrary((_ub_cs)"muimaster.library", 0);
    if(!MUIMasterBase) cleanexit((char*)"cant open muimaster.library\n");

    IntuitionBase = OpenLibrary((_ub_cs)"intuition.library",33);
    if(!IntuitionBase) cleanexit((char*)"cant open intuition.library\n");

    RexxSysBase = OpenLibrary((_ub_cs)"rexxsyslib.library",33);
    if(!RexxSysBase) cleanexit((char*)"cant open rexxsyslib.library\n");

    CodesetsBase = OpenLibrary((_ub_cs)CODESETSNAME, CODESETSVER);
    if(!CodesetsBase) cleanexit((char*)"cant open codesets.library\nDownload and install this:\nhttp://aminet.net/util/libs/codesets-6.4.lha\n\n");

    IMUIMaster = (struct MUIMasterIFace*) GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
    ISocket = (struct SocketIFace*) GetInterface((struct Library *)SocketBase, "main", 1, NULL);
    IUtility = (struct UtilityIFace*)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
    IRexxSys = (struct RexxSysIFace*)GetInterface((struct Library *)RexxSysBase, "main", 1, NULL);
    ICodesets = (struct CodesetsIFace*)GetInterface((struct Library *)CodesetsBase, "main", 1, NULL);
    IIntuition = (struct IntuitionIFace*)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
    ILocale = (struct LocaleIFace*)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
    IDataTypes = (struct DataTypesIFace*)GetInterface((struct Library *)DataTypesBase, "main", 1, NULL);
    IIcon = (struct IconIFace*)GetInterface((struct Library *)IconBase, "main", 1, NULL);
    IAsl = (struct AslIFace*)GetInterface((struct Library *)AslBase, "main", 1, NULL);

#elif __MORPHOS__
    AslBase = (struct Library*)OpenLibrary((_ub_cs)"asl.library",0);
    if(!AslBase) cleanexit((char*)"cant open asl.library\n");

    DataTypesBase = (struct Library*)OpenLibrary((_ub_cs)"datatypes.library",0);
    if(!DataTypesBase) cleanexit((char*)"cant open datatypes.library\n");

    GfxBase = (struct GfxBase*)OpenLibrary("graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    IconBase = (struct Library*)OpenLibrary((_ub_cs)"icon.library",0);
    if(!IconBase) cleanexit((char*)"cant open icon.library\n");

    LocaleBase = (struct Library*)OpenLibrary((_ub_cs)"locale.library",0);
    if(!LocaleBase) cleanexit((char*)"cant open locale.library\n");

    GfxBase = (struct GfxBase*)OpenLibrary((_ub_cs)"graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    UtilityBase = (struct Library*)OpenLibrary((_ub_cs)"utility.library",0);
    if(!UtilityBase) cleanexit((char*)"cant open utility.library\n");

    SocketBase = OpenLibrary((_ub_cs)"bsdsocket.library",0L);
    if(!SocketBase) cleanexit((char*)"cant open bsdsocket.library\n");

    MUIMasterBase = OpenLibrary((_ub_cs)"muimaster.library", 0);
    if(!MUIMasterBase) cleanexit((char*)"cant open muimaster.library\n");

    IntuitionBase = (struct IntuitionBase*)OpenLibrary((_ub_cs)"intuition.library",33);
    if(!IntuitionBase) cleanexit((char*)"cant open intuition.library\n");

    CodesetsBase = OpenLibrary((_ub_cs)CODESETSNAME, CODESETSVER);
    if(!CodesetsBase) cleanexit((char*)"cant open codesets.library\nDownload and install this:\nhttp://aminet.net/util/libs/codesets-6.4.lha\n\n");
#elif __AROS__
    AslBase = (struct Library*) OpenLibrary((_ub_cs) "asl.library", 0);
    if (!AslBase)
        cleanexit((char*) "cant open asl.library\n");

    DataTypesBase = (struct Library*) OpenLibrary((_ub_cs) "datatypes.library", 0);
    if (!DataTypesBase)
        cleanexit((char*) "cant open datatypes.library\n");

    GfxBase = (struct GfxBase*) OpenLibrary((_ub_cs)"graphics.library", 0);
    if (!GfxBase)
        cleanexit((char*) "cant open graphics.library\n");

    IconBase = (struct Library*) OpenLibrary((_ub_cs) "icon.library", 0);
    if (!IconBase)
        cleanexit((char*) "cant open icon.library\n");

    LocaleBase = (struct LocaleBase*) OpenLibrary((_ub_cs) "locale.library", 0);
    if (!LocaleBase)
        cleanexit((char*) "cant open locale.library\n");

    GfxBase = (struct GfxBase*) OpenLibrary((_ub_cs) "graphics.library", 0);
    if (!GfxBase)
        cleanexit((char*) "cant open graphics.library\n");

    UtilityBase = (struct UtilityBase*) OpenLibrary((_ub_cs) "utility.library", 0);
    if (!UtilityBase)
        cleanexit((char*) "cant open utility.library\n");

    SocketBase = OpenLibrary((_ub_cs) "bsdsocket.library", 0L);
    if (!SocketBase)
        cleanexit((char*) "cant open bsdsocket.library\n");

    MUIMasterBase = OpenLibrary((_ub_cs) "muimaster.library", 0);
    if (!MUIMasterBase)
        cleanexit((char*) "cant open muimaster.library\n");

    IntuitionBase = (struct IntuitionBase*) OpenLibrary((_ub_cs) "intuition.library", 33);
    if (!IntuitionBase)
        cleanexit((char*) "cant open intuition.library\n");

    CodesetsBase = OpenLibrary((_ub_cs) CODESETSNAME, CODESETSVER);
    if (!CodesetsBase)
        cleanexit(
                (char*) "cant open codesets.library\nDownload and install this:\nhttp://aminet.net/util/libs/codesets-6.4.lha\n\n");

#else

    AslBase = (struct Library*)OpenLibrary((_ub_cs)"asl.library",0);
    if(!AslBase) cleanexit((char*)"cant open asl.library\n");

    DataTypesBase = (struct Library*)OpenLibrary((_ub_cs)"datatypes.library",0);
    if(!DataTypesBase) cleanexit((char*)"cant open datatypes.library\n");

    IconBase = (struct Library*)OpenLibrary((_ub_cs)"icon.library",0);
    if(!IconBase) cleanexit((char*)"cant open icon.library\n");

    LocaleBase = (struct Library*)OpenLibrary((_ub_cs)"locale.library",0);
    if(!LocaleBase) cleanexit((char*)"cant open locale.library\n");

    GfxBase = (struct Library*)OpenLibrary((_ub_cs)"graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    UtilityBase = (struct Library*)OpenLibrary((_ub_cs)"utility.library",0);
    if(!UtilityBase) cleanexit((char*)"cant open utility.library\n");

    SocketBase = OpenLibrary((_ub_cs)"bsdsocket.library",0L);
    if(!SocketBase) cleanexit((char*)"cant open bsdsocket.library\n");

    MUIMasterBase = OpenLibrary((_ub_cs)"muimaster.library", 0);
    if(!MUIMasterBase) cleanexit((char*)"cant open muimaster.library\n");

    IntuitionBase = OpenLibrary((_ub_cs)"intuition.library",33);
    if(!IntuitionBase) cleanexit((char*)"cant open intuition.library\n");

    CodesetsBase = OpenLibrary((_ub_cs)CODESETSNAME, CODESETSVER);
    if(!CodesetsBase) cleanexit((char*)"cant open codesets.library\nDownload and install this:\nhttp://aminet.net/util/libs/codesets-6.4.lha\n\n");

#endif

    catalog = OpenCatalog(NULL, (loc_in)"WookieChat.catalog", OC_BuiltInLanguage, NULL, TAG_DONE);

    if (!catalog)
    {

        if (DEBUG)
            printf("unable to use default language\n");

        catalog = OpenCatalog(NULL, (loc_in)"WookieChat.catalog", OC_Language, "english", TAG_DONE);

        if (!catalog)
        {
            if (DEBUG)
                printf("unable to load english catalog, using built in strings\n");

        }

    }

    //now that the catalog is open and locale library is open, lets give our context menu titles/items some labels

    MenuData1[0].nm_Label = (nml) GCS(catalog, 219, "Nicklist Options");
    MenuData1[1].nm_Label = (nml) GCS(catalog, 220, "Whois");
    MenuData1[2].nm_Label = (nml) GCS(catalog, 221, "Open Query");
    MenuData1[3].nm_Label = (nml) GCS(catalog, 222, "Open Global Query");
    MenuData1[4].nm_Label = (nml) GCS(catalog, 223, "CTCP");
    MenuData1[5].nm_Label = (nml) GCS(catalog, 224, "Ping");
    MenuData1[6].nm_Label = (nml) GCS(catalog, 225, "Version");
    MenuData1[7].nm_Label = (nml) GCS(catalog, 226, "Time");
    MenuData1[8].nm_Label = (nml) GCS(catalog, 227, "Direct Connection");
    MenuData1[9].nm_Label = (nml) GCS(catalog, 228, "Send file");
    MenuData1[10].nm_Label = (nml) GCS(catalog, 229, "Chat");
    MenuData1[11].nm_Label = (nml) GCS(catalog, 230, "Control");
    MenuData1[12].nm_Label = (nml) GCS(catalog, 231, "Op");
    MenuData1[13].nm_Label = (nml) GCS(catalog, 232, "DeOp");
    MenuData1[14].nm_Label = (nml) GCS(catalog, 233, "HalfOp");
    MenuData1[15].nm_Label = (nml) GCS(catalog, 234, "DeHalfOp");
    MenuData1[16].nm_Label = (nml) GCS(catalog, 235, "Voice");
    MenuData1[17].nm_Label = (nml) GCS(catalog, 236, "DeVoice");
    MenuData1[18].nm_Label = (nml) GCS(catalog, 237, "Kick");
    MenuData1[19].nm_Label = (nml) GCS(catalog, 238, "Ban");

    dobj = (struct DiskObject*) GetDiskObject((_ub_cs) "PROGDIR:WookieChat_OS4");
    if (!dobj)
        dobj = (struct DiskObject*) GetDiskObject((_ub_cs) "PROGDIR:WookieChat_OS3");
    if (!dobj)
        dobj = (struct DiskObject*) GetDiskObject((_ub_cs) "PROGDIR:WookieChat");

#ifdef __AROS__
    BPTR my_lock;

    if ((my_lock = Lock((_s_cs)"env:zune", ACCESS_READ)))
    {
        UnLock(my_lock);
        ZUNE_SYSTEM = TRUE;
    }
    else
        ZUNE_SYSTEM = FALSE;
#endif

    total_smileys = 0;

    //zero the socket filedescriptor sets
    FD_ZERO(&read_master);
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&write_master);

    pincoming_init();
}

void DisposeApp(struct ObjApp * MBObj)
{
    MUI_DisposeObject((Object*)MBObj->App);
    FreeVec(MBObj);
}


void cleanexit(char *str)
{
    if (GEIT)
        printf("closing\n1\n");

    TimerWait_Close();

    //if(local_charset) CodesetsFreeA(local_charset,blank_taglist);// my_utf8_taglist);
//printf("2\n");
    //printf("1\n");
    if (GEIT)
        printf("2\n");

    arexx_deinit_messageports();

    //printf("2\n");

    //printf("quit 7\n");

    if (GEIT)
        printf("7\n");

    if (WookieChat)
    {

        //Now Save all URL's to a text file
        sprintf(file_name, "progdir:urls.txt");
        urlgrabber_file = Open((_s_cs)file_name, MODE_NEWFILE);
        if (urlgrabber_file)
        {
            ULONG entries;
            getmacro((Object*) WookieChat->LV_urlgrabber, MUIA_NList_Entries, &entries);
            //printf("3\n");

            for (unsigned int aaa = 0; aaa < entries; aaa++)
            {
                DoMethod((Object*) WookieChat->LV_urlgrabber, MUIM_NList_GetEntry, aaa, &string1);

                if (string1)
                {
                    FPuts(urlgrabber_file, (l_in) string1);
                    FPutC(urlgrabber_file, '\n');

                    if (!strcmp(string1, urlgrabber_str))
                        break;
                }
            }
            Close(urlgrabber_file);

        }

        //printf("4\n");

        //printf("5\n");

        if (GEIT)
            printf("8\n");

//printf("3\n");

        status_conductor = status_root;
        while (status_conductor)
        {
            //printf("6\n");
            //printf("7\n");
            //printf("8\n");
            status_conductor = status_conductor->next;
            //printf("9\n");
        }

//printf("4\n");
        if (GEIT)
            printf("9\n");

    }

    //printf("10\n");

    if (WookieChat)
    {
        struct dcc *dcc_send_work;
        struct dcc *dcc_work;
        struct dcc_chat *dcc_chat_work;

        if (GEIT)
            printf("10\n");

        //free up dcc recv linked list
        for (dcc_conductor = dcc_root->next; dcc_conductor; dcc_conductor = dcc_work)
        {
            if (DEBUG)
                printf("cleaning up one dcc recv, file:%s\n", dcc_conductor->filename);
            dcc_work = dcc_conductor->next;
            if (dcc_conductor)
            {
                free(dcc_conductor);
            }
        }

        //printf("11\n");

        //free up dcc send linked list
        for (dcc_send_conductor = dcc_send_root->next; dcc_send_conductor; dcc_send_conductor = dcc_send_work)
        {

            dcc_send_work = dcc_send_conductor->next;
            if (dcc_send_conductor)
            {
                free(dcc_send_conductor);

            }
        }
        //printf("12\n");

        //free up dcc chat linked list
        for (dcc_chat_conductor = dcc_chat_root->next; dcc_chat_conductor; dcc_chat_conductor = dcc_chat_work)
        {

            dcc_chat_work = dcc_chat_conductor->next;
            if (dcc_chat_conductor)
            {
                free(dcc_chat_conductor);

            }
        }
        //printf("13\n");
        if (GEIT)
            printf("11\n");
    }

    if (WookieChat)
    {
        status_conductor = status_root;

        setmacro((Object*) WookieChat->WI_main, MUIA_Window_Open, FALSE);
        if (GEIT)
            printf("12\n");

        // //printf("2\n");
        //printf("14\n");

        //printf("5a\n");
        delete_smiley_objects();
        if (GEIT)
            printf("13\n");

        while (status_conductor)
        {
            status_conductor->conductor = status_conductor->root;
            // //printf("3\n");

            while (status_conductor->conductor)
            {
                int count;
                struct query_window *work_query = NULL;
                // printf("4\n");

                for (count = 0; count < 2500; count++)
                {
                    if (status_conductor->conductor->nicklist[count].hostname)
                        free(status_conductor->conductor->nicklist[count].hostname);
                }
                // printf("5\n");

                if (DoMethod((Object*) GROUP_ROOT_0, MUIM_Group_InitChange))
                {
                    if (status_conductor->conductor->GR_conductor
                            && (status_conductor->conductor != status_current->current_query))
                    {
                        APTR member_object = NULL;
                        struct MinList *list = NULL;

                        if (DoMethod((Object*) GR_top, MUIM_Group_InitChange))
                        {

                            if (DoMethod((Object*) status_conductor->conductor->GR_conductor, MUIM_Group_InitChange))
                            {
                                if (DoMethod((Object*) status_conductor->conductor->GR_listviews,
                                        MUIM_Group_InitChange))
                                {

                                    getmacro((Object*) status_conductor->conductor->GR_listviews, MUIA_Group_ChildList,
                                            &list);
                                    APTR object_state = list->mlh_Head;

                                    while ((member_object = NextObject((Object**) &object_state)))
                                    {
                                        if (member_object == status_conductor->conductor->GR_nicklist_and_tabs)
                                            DoMethod((Object*) status_conductor->conductor->GR_listviews, OM_REMMEMBER,
                                                    (Object*) status_conductor->conductor->GR_nicklist_and_tabs);

                                    }

                                    DoMethod((Object*) status_conductor->conductor->GR_listviews,
                                            MUIM_Group_ExitChange);

                                }

                                DoMethod((Object*) status_conductor->conductor->GR_conductor, MUIM_Group_ExitChange);

                            }

                            DoMethod((Object*) GR_top, MUIM_Group_ExitChange);

                        }
                        //does this belong here?

                        getmacro((Object*) GR_top, MUIA_Group_ChildList, &list);
                        APTR object_state = list->mlh_Head;

                        while ((member_object = NextObject((Object**) &object_state)))
                        {
                            if (member_object == status_conductor->conductor->GR_conductor)
                                DoMethod((Object*) GR_top, OM_REMMEMBER,
                                        (Object*) status_conductor->conductor->GR_conductor);
                        }

                    }

                    DoMethod((Object*) GROUP_ROOT_0, MUIM_Group_ExitChange);

                }

                if (status_conductor->conductor->GR_conductor
                        && (status_conductor->conductor != status_current->current_query))
                {
                    MUI_DisposeObject((Object*) status_conductor->conductor->GR_conductor);
                }

                // printf("6\n");

                work_query = status_conductor->conductor->next;
                if (status_conductor->conductor)
                {
                    struct history * work_history = NULL;

                    if (status_conductor->conductor->log_file)
                    {
                        Close(status_conductor->conductor->log_file);
                        status_conductor->conductor->log_file = 0;
                    }

                    //attempting to clean up typed history
                    for (status_conductor->conductor->string_conductor = status_conductor->conductor->string_root;
                            status_conductor->conductor->string_conductor;
                            status_conductor->conductor->string_conductor = work_history)
                    {

                        work_history = status_conductor->conductor->string_conductor->next;

                        if (status_conductor->conductor->string_conductor->buffer_history[0] != '\0')
                        {
                            free(status_conductor->conductor->string_conductor);
                        }

                    }

                    if (status_conductor->conductor != status_current->current_query)
                    {
                        FreeVec(status_conductor->conductor);
                        status_conductor->conductor = NULL;
                    }
                }

                status_conductor->conductor = work_query;

            }

            work_status = status_conductor->next;
            if (status_conductor && status_conductor != status_current)
            {
                free(status_conductor);
                status_conductor = NULL;
            }
            status_conductor = work_status;

        }
        if (GEIT)
            printf("14\n");

        //printf("5b\n");

        //printf("7\n");

        exit_delete_smiley_objects();
        if (GEIT)
            printf("15\n");
        DisposeApp(WookieChat);
        if (GEIT)
            printf("16\n");

        if (status_current)
        {
            if (status_current->current_query)
            {
                FreeVec(status_current->current_query);
                status_current->current_query = NULL;

                free(status_current);
                status_current = NULL;

            }
        }
        if (GEIT)
            printf("17\n");

        //printf("8\n");

    }
    //printf("15\n");

    //printf("6\n");

    samples_dispose();

    if (GEIT)
        printf("18\n");

    //printf("9\n");

    delete_custom_classes();

    //printf("16\n");

    if (GEIT)
        printf("19\n");
    //printf("8\n");

    //printf("17\n");

    //printf("9\n");

    if (GEIT)
        printf("20\n");

    //printf("10\n");

    if (str)
        printf("Error: %s\n", str);

#ifdef __amigaos4__
    DropInterface((struct Interface*)IUtility);
    DropInterface((struct Interface*)IMUIMaster);
    DropInterface((struct Interface*)ISocket);
    DropInterface((struct Interface*)IRexxSys);
    DropInterface((struct Interface*)ICodesets);
#endif

    if (dobj)
        FreeDiskObject(dobj);
    if (GEIT)
        printf("21\n");

    //printf("18\n");

    CloseCatalog(catalog);
    if (GEIT)
        printf("22\n");

    //printf("11\n");

#ifdef __AROS__
    if (UtilityBase)
        CloseLibrary((struct Library*) UtilityBase);
    if (SocketBase)
        CloseLibrary((struct Library*) SocketBase);
    if (IntuitionBase)
        CloseLibrary((struct Library*) IntuitionBase);
    if (LocaleBase)
        CloseLibrary((struct Library*) LocaleBase);
#else
    if(CodesetsBase) CloseLibrary(CodesetsBase);
    if(UtilityBase) CloseLibrary(UtilityBase);
    if(SocketBase) CloseLibrary(SocketBase);
    if(IntuitionBase) CloseLibrary((Library*)IntuitionBase);
    if(LocaleBase) CloseLibrary(LocaleBase);
#endif

#ifdef __MORPHOS__
    if(AslBase) CloseLibrary(AslBase);
    if(DataTypesBase) CloseLibrary(DataTypesBase);
    if(GfxBase) CloseLibrary((Library*)GfxBase);
#endif

    if (MUIMasterBase)
        CloseLibrary(MUIMasterBase);

    if (GEIT)
        printf("23\n");

    //printf("7\n");

    free(new_entry2.hostname);

    nicklist_deinit();


    exit(0);
}
