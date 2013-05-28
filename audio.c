/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include <proto/datatypes.h>

#include "intern.h"

#include "audio.h"

static BOOL muted_sound;
static Object * samples[MAX_SAMPLES];

void play_external_sound_replayer(char *string1)
{
    char string11[900];
    char file_name[800];

    if (muted_sound)
        return;

    if (my_settings.samples_path[strlen(my_settings.samples_path) - 1] == ':'
            || my_settings.samples_path[strlen(my_settings.samples_path) - 1] == '/')
        sprintf(string11, "%s%s", my_settings.samples_path, string1);
    else
        sprintf(string11, "%s/%s", my_settings.samples_path, string1);

    if (my_settings.use_external_sound_replayer)
    {

        sprintf(file_name, "run >nil: %s %s", my_settings.external_sound_replayer, string11);

#ifdef __amigaos4__
        SystemTags(file_name,TAG_DONE);
#else
        Execute(file_name, 0, 0);
#endif

    }
    else
    {
        Object * o = NULL;
        if (NULL != (o = NewDTObject(string11, DTA_GroupID, GID_SOUND, TAG_END)))
        {
            DoMethod(o, DTM_TRIGGER, NULL, STM_PLAY, NULL);
            DisposeDTObject(o);
        }
    }
}

void play_sample(int id)
{
    if (id >= MAX_SAMPLES || id < 0)
        return;

    if (!muted_sound && samples[id])
        DoMethod(samples[id], DTM_TRIGGER, NULL, STM_PLAY, NULL);
}

void samples_init()
{
    int i;
    for (i = 0; i < MAX_SAMPLES; i++) samples[i] = NULL;
}

void samples_dispose()
{
    int i;
    for (i = 0; i < MAX_SAMPLES; i++) if (samples[i]) { DisposeDTObject(samples[i]); samples[i] = NULL; }
}

void reload_sample(char * path, int id)
{
    if (id >= MAX_SAMPLES || id < 0)
        return;

    if (samples[id]) { DisposeDTObject(samples[id]); samples[id] = NULL; }

    samples[id] = NewDTObject(path,DTA_GroupID,GID_SOUND,TAG_END);
}

BOOL is_sound_muted()
{
    return muted_sound;
}

void set_sound_muted(BOOL muted)
{
    muted_sound = muted;
}
