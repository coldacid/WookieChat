/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#ifndef __AROS__
struct TagItem my_incoming_charset11_taglist[] = { {CSA_Source, (ULONG)buffer_text },
    {CSA_SourceCodeset, (ULONG)cs}, {CSA_DestCodeset, (ULONG)charsets[local_charset]}, {TAG_DONE, 0} };

struct TagItem my_incoming_charset22_taglist[] = { {CSA_Source, (ULONG)buffer_text },
    {CSA_SourceCodeset, (ULONG)charsets[0]}, {CSA_DestCodeset, (ULONG)charsets[local_charset]}, {TAG_DONE, 0} };

struct TagItem my_incoming_charset33_taglist[] = { {CSA_Source, (ULONG)buffer_text},{CSA_CodesetFamily, CSV_CodesetFamily_Latin},{CSA_FallbackToDefault, TRUE},{TAG_DONE,0} };
#endif

int display_last_few_lines_of_logfile_conductor()
{

    count_the_linefeeds=0;

        if(wookie_folder[strlen(wookie_folder)-1]==':' || wookie_folder[strlen(wookie_folder)-1]=='/')
            sprintf(file_name,"%slogs/%s_%s",wookie_folder,status_conductor->networkname,status_conductor->conductor->name);
        else
            sprintf(file_name,"%s/logs/%s_%s",wookie_folder,status_conductor->networkname,status_conductor->conductor->name);
        status_conductor->conductor->log_file = Open(file_name,MODE_OLDFILE);

    if(!status_conductor->conductor->log_file) return 0;

    a=0;

    #ifdef __amigaos4__
    ChangeFilePosition(status_conductor->conductor->log_file, -1, OFFSET_END);
    #else
    Seek(status_conductor->conductor->log_file, -1, OFFSET_END);
    #endif

    //Seek(status_conductor->conductor->log_file, -1, OFFSET_END);


    char_read_storage=FGetC(status_conductor->conductor->log_file);
    if(char_read_storage == -1L) return 0;

    if(status_conductor->conductor->name[0] == '#') how_many_lines = my_settings.how_many_lines_to_reload_channel;
        else how_many_lines = my_settings.how_many_lines_to_reload_private;

    while(count_the_linefeeds < how_many_lines+1)
    {
        if(char_read_storage == '\n') count_the_linefeeds++;
        #ifdef __amiagaos4__
        if(ChangeFilePosition(status_conductor->conductor->log_file, -2, OFFSET_CURRENT)) { count_the_linefeeds++; break; }
        #else
        if(Seek(status_conductor->conductor->log_file, -2, OFFSET_CURRENT) == -1) { count_the_linefeeds++; break; }
        #endif

        //if(Seek(status_conductor->conductor->log_file, -2, OFFSET_CURRENT) == -1) { count_the_linefeeds++; break; }

        char_read_storage=FGetC(status_conductor->conductor->log_file);
        if(char_read_storage == -1L) break;
    }

    int count=0;
    #ifdef __aamigaos4__
    if(count_the_linefeeds >= how_many_lines+1) ChangeFilePosition(status_conductor->conductor->log_file, 1, OFFSET_CURRENT);
    else { ChangeFilePosition(status_conductor->conductor->log_file, -1, OFFSET_CURRENT); count=1; }
    #else
    if(count_the_linefeeds >= how_many_lines+1) Seek(status_conductor->conductor->log_file, 1, OFFSET_CURRENT);
    else { Seek(status_conductor->conductor->log_file, -1, OFFSET_CURRENT); count=1; }
    #endif

    setmacro((Object*)status_conductor->conductor->LV_channel,MUIA_NList_Quiet, TRUE);

    for(;;)
    {

        char_read_storage=FGetC(status_conductor->conductor->log_file);

        if(char_read_storage=='\n')
        {
            count++;
            buffer_text[a]='\0'; a=-1;
            if(ZUNE_SYSTEM==TRUE)
                strcpy(centry->entry,"         ");
            else
                strcpy(centry->entry,"         :");
            centry->colour=9;
            if(centry->colour >= 0 && centry->colour <= 22) sprintf(new_array,"\033P[%ld]",custom_pen_colours[centry->colour]);
            else sprintf(new_array,"\033P[%ld]",custom_pen_colours[1]);
            strncpy(centry->entry,new_array,strlen(new_array));

            #ifndef __AROS__
            if((cs = CodesetsFindBestA(my_incoming_charset33_taglist)))
            {
                if(FindUTF8Chars(buffer_text))
                {
                    text3 = CodesetsUTF8ToStrA(my_incoming_charset22_taglist);
                }
                else
                {
                    text3 = CodesetsConvertStrA(my_incoming_charset11_taglist);
                }
            }
            else
            {
                struct TagItem my_incoming_charset4_taglist[] = { {CSA_Source, (ULONG)buffer_text },
                    {CSA_SourceCodeset, (ULONG)charsets[status_conductor->remote_charset]}, {CSA_DestCodeset, (ULONG)charsets[local_charset]}, {TAG_DONE, 0} };

                if(status_conductor->remote_charset==0)
                    text3 = CodesetsUTF8ToStrA(my_incoming_charset4_taglist);
                else
                    text3 = CodesetsConvertStrA(my_incoming_charset4_taglist);
            }
            if(text3)
            {
                strcat(centry->entry,text3);
                CodesetsFreeA(text3, NULL);
            }
            else
            {
                strcat(centry->entry,buffer_text);
            }

            #else
            strcat(centry->entry,buffer_text);
            #endif

            insert_graphical_smilies();



            getmacro((Object*)status_conductor->conductor->LV_channel,MUIA_NList_Visible, &visible);


            if(visible > 0)
            {
                getmacro((Object*)status_conductor->conductor->LV_channel,MUIA_NList_Entries, &entries);
                getmacro((Object*)status_conductor->conductor->LV_channel,MUIA_NList_First, &first);
            	DoMethod((Object*)status_conductor->conductor->LV_channel,MUIM_NList_InsertSingleWrap,centry,MUIV_NList_Insert_Bottom,WRAPCOL0,ALIGN_LEFT);

            }
            else
            {
            	DoMethod((Object*)status_conductor->conductor->LV_channel,MUIM_NList_InsertSingleWrap,centry,MUIV_NList_Insert_Bottom,WRAPCOL0,ALIGN_LEFT);

            }


        }
        else if(char_read_storage==-1L) break;
        else buffer_text[a]=char_read_storage;
        a++;
        if(count==count_the_linefeeds) break;


    }


    setmacro((Object*)status_conductor->conductor->LV_channel,MUIA_NList_Quiet, FALSE);

    status_conductor->conductor->jump_to_bottom=TRUE;

    Close(status_conductor->conductor->log_file);

    return 0;

}



