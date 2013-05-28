/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

//this function returns 1 (TRUE) if the nick or any configured highlight words are found
#include "includes.h"

#include "intern.h"
#include "audio.h"

BYTE sig;

int search_for_highlight_words(char *stringSearch, char *buffer3)
{

    if (!stringSearch)
        return 0;

    char string2[1024];
    strcpy(string2, stringSearch);

    //create_arexx_event_string(my_settings.events[HIGHLIGHT].arexx_script,my_settings.events[HIGHLIGHT].arexx_script2);

    if (strstr(string2, status_conductor->nick))
    {
        //if(my_settings.events[HIGHLIGHT].use_when > 0) Execute(event_string,0,0);

        getmacro((Object*) WookieChat->WI_main, MUIA_Window_Activate, &string1);

        if ((!string1 && my_settings.play_sound_when_highlight == 2) || my_settings.play_sound_when_highlight == 1
                || (my_settings.play_sound_when_highlight == 3
                        && status_conductor->conductor != status_current->current_query)
                || (my_settings.play_sound_when_highlight == 3 && !string1))
        {
            play_sample(SAMPLE_HIGHLIGHT);
        }

        return 1;
    }

    for (unsigned int a = 0; a != strlen(string2) + 1; a++)
        string2[a] = ToLower(string2[a]);

    strcpy(string7, my_settings.highlight);

    string1 = strtok(string7, " ");
    while (string1)
    {
        if (strstr(string2, string1))
        {
            //if(my_settings.events[HIGHLIGHT].use_when > 0) Execute(event_string,0,0);

            getmacro((Object*) WookieChat->WI_main, MUIA_Window_Activate, &string1);

            if ((!string1 && my_settings.play_sound_when_highlight == 2) || my_settings.play_sound_when_highlight == 1
                    || (my_settings.play_sound_when_highlight == 3
                            && status_conductor->conductor != status_current->current_query)
                    || (my_settings.play_sound_when_highlight == 3 && !string1))
            {
                play_sample(SAMPLE_HIGHLIGHT);
            }

            return 1;

        }
        string1 = strtok(NULL, " ");

    }

    return 0;

}

