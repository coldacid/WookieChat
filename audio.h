/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/


#define SAMPLE_NEWMSG           0
#define SAMPLE_HIGHLIGHT        1
#define SAMPLE_TABOPEN          2
#define MAX_SAMPLES             3

void play_external_sound_replayer(char *string1);
void play_sample(int id);
void samples_init();
void samples_dispose();
void reload_sample(char * path, int id);

BOOL is_sound_muted();
void set_sound_muted(BOOL muted);
