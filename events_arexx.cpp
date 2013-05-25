/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

char event_string[900];
char target_nick[60];

void create_arexx_event_string(char *arexx_event_to_use, char *arexx_arguments)
{

#define MAX_CODES_TO_CONVERT 7

    char buffer4[800];
    //strcpy(buffer4,arexx_event_to_use);
    strcpy(buffer4, arexx_arguments);
    char *search;
    char start_of_string[800];
    char end_of_string[800];
    char html_codes[MAX_CODES_TO_CONVERT + 1][8];

    strcpy(html_codes[0], "\%b");
    strcpy(html_codes[1], "\%t");
    strcpy(html_codes[2], "\%g");
    strcpy(html_codes[3], "\%s");
    strcpy(html_codes[4], "\%c");
    strcpy(html_codes[5], "\%n");
    strcpy(html_codes[6], "\%m");

    for (int codes_count = 0; codes_count < MAX_CODES_TO_CONVERT; codes_count++)
    {
        if (DEBUG)
            printf("codes_count:%d\n", codes_count);

        while ((search = strstr(buffer4, html_codes[codes_count])))
        {
            strcpy(end_of_string, search + 2);
            strncpy(start_of_string, buffer4, search - buffer4);
            start_of_string[search - buffer4] = '\0';
            if (codes_count == 0)
                sprintf(buffer4, "%s%s%s", start_of_string, buffer3, end_of_string);
            else if (codes_count == 1)
                sprintf(buffer4, "%s%s%s", start_of_string, timestamp, end_of_string);
            else if (codes_count == 2 && status_conductor)
                sprintf(buffer4, "%s%s%s", start_of_string, status_conductor->networkname, end_of_string);
            else if (codes_count == 3 && status_conductor)
                sprintf(buffer4, "%s%s%s", start_of_string, status_conductor->servername, end_of_string);
            else if (codes_count == 4 && status_conductor->conductor)
                sprintf(buffer4, "%s%s%s", start_of_string, status_conductor->conductor->name, end_of_string);
            else if (codes_count == 5)
                sprintf(buffer4, "%s%s%s", start_of_string, status_conductor->nick, end_of_string);
            else if (codes_count == 6)
                sprintf(buffer4, "%s%s%s", start_of_string, target_nick, end_of_string);
            else
                sprintf(buffer4, "%s%s", start_of_string, end_of_string);
            //printf("buffer4:%s\n",buffer4);
        }
    }

    //sprintf(work_buffer,"rx %s \"%s\"",my_settings.highlights.arexx_script,buffer3);
    sprintf(event_string, "rx %s \"%s\"", arexx_event_to_use, buffer4);
    if (DEBUG)
        printf("executing:%s\n", event_string);
}

BOOL is_window_active()
{
    char *active;
    getmacro((Object*) WookieChat->WI_main, MUIA_Window_Activate, &active);

    if (active)
        return TRUE;
    else
        return FALSE;

}
