/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include <clib/alib_protos.h>

#include "intern.h"

/* Locals */
static BOOL Timer1_WaitCheck;
static BOOL Timer2_WaitCheck;
static BOOL Timer4_WaitCheck;
static BOOL Timer5_WaitCheck;
static struct MsgPort *TimerMP;


BOOL TimerWait_Close(void);

BOOL TimerWait_Open(void)
{

    Timer1_WaitCheck = FALSE;
    Timer2_WaitCheck = FALSE;
    //Timer3_WaitCheck = FALSE;
    Timer4_WaitCheck = FALSE;
    Timer5_WaitCheck = FALSE;

    Timer1_WaitCheck = FALSE;
    if ((TimerMP = CreatePort(0, 0)))
    {
        Timer2_WaitCheck = FALSE;

        if ((Timer2MP = CreatePort(0, 0)))
        {
            Timer4_WaitCheck = FALSE;

            if ((Timer4MP = CreatePort(0, 0)))
            {
                Timer5_WaitCheck = FALSE;

                if ((Timer5MP = CreatePort(0, 0)))
                {
                    if ((Timer5IO = (struct timerequest *) CreateIORequest(Timer5MP, sizeof(struct timerequest))))
                    {
                        if (!(error = OpenDevice((_ub_cs) TIMERNAME, UNIT_WAITUNTIL, (struct IORequest *) Timer5IO, 0L)))
                        {
                            Timer5_WaitCheck = TRUE;

                            Timer5IO->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;

                            if ((Timer4IO = (struct timerequest *) CreateIORequest(Timer4MP, sizeof(struct timerequest))))
                            {

                                if (!(error = OpenDevice((_ub_cs) TIMERNAME, UNIT_WAITUNTIL,
                                        (struct IORequest *) Timer4IO, 0L)))
                                {
                                    Timer4_WaitCheck = TRUE;

                                    Timer4IO->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
                                    Timer4IO->tr_node.io_Command = TR_ADDREQUEST;

                                    if ((Timer2IO = (struct timerequest *) CreateIORequest(Timer2MP,
                                            sizeof(struct timerequest))) != NULL)
                                    {
                                        if (!(error = OpenDevice((_ub_cs) TIMERNAME, UNIT_WAITUNTIL,
                                                (struct IORequest *) Timer2IO, 0L)))
                                        {
                                            Timer2_WaitCheck = TRUE;
                                            timer_signal = 1L << Timer2MP->mp_SigBit;

                                            Timer2IO->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
                                            Timer2IO->tr_node.io_Command = TR_ADDREQUEST;

                                            if ((TimerIO = (struct timerequest *) CreateIORequest(TimerMP,
                                                    sizeof(struct timerequest))) != NULL)
                                            {
                                                if (!(error = OpenDevice((_ub_cs) TIMERNAME, UNIT_VBLANK,
                                                        (struct IORequest *) TimerIO, 0L)))
                                                {
                                                    Timer1_WaitCheck = TRUE;
                                                    TimerIO->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;

                                                    TimerIO->tr_node.io_Command = TR_GETSYSTIME;
                                                }
                                            }

                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }

    }

    if (Timer2_WaitCheck)
    {
        struct timeval systime;
        get_sys_time(&systime); // Get current system time
#ifdef __amigaos4__
        Timer2IO->tr_time.tv_sec = systime.tv_sec+2;
        Timer2IO->tr_time.tv_usec = systime.tv_usec;
#else
        Timer2IO->tr_time.tv_secs = systime.tv_secs + 2;
        Timer2IO->tr_time.tv_micro = systime.tv_micro;
#endif
        SendIO((struct IORequest *) Timer2IO); // Get the results
    }

    if (Timer4_WaitCheck)
    {
        struct timeval systime;
        get_sys_time(&systime); // Get current system time
#ifdef __amigaos4__
        Amiga2Date(systime.tv_sec, clockdata);
#else
        Amiga2Date(systime.tv_secs, clockdata);
#endif
        timestamp_2_string();
        clockdata2->sec = 0;
        clockdata2->min = 59;
        clockdata2->hour = 23;
        clockdata2->mday = clockdata->mday;
        clockdata2->month = clockdata->month;
        clockdata2->year = clockdata->year;
        clockdata2->wday = clockdata->wday;
#ifdef __amigaos4__
        Timer4IO->tr_time.tv_sec = Date2Amiga(clockdata2)+60;
        Timer4IO->tr_time.tv_usec = 100;
#else
        Timer4IO->tr_time.tv_secs = Date2Amiga(clockdata2) + 60;
        Timer4IO->tr_time.tv_micro = 100;
#endif
        SendIO((struct IORequest *) Timer4IO); // Get the results
    }

    if (!Timer1_WaitCheck)
    { /*TimerWait_Close();*/
        return FALSE;
    }
    else
        return TRUE;

}

BOOL TimerWait_Close(void)
{
    //return TRUE;

    if (Timer2_WaitCheck)
    {
        if (Timer2IO)
        {
            if (!CheckIO((struct IORequest *) Timer2IO))
            {
                AbortIO((struct IORequest *) Timer2IO);
                WaitIO((struct IORequest *) Timer2IO);
            }
        }

        if (Timer2IO)
        {
            CloseDevice((struct IORequest *) Timer2IO);
            DeleteIORequest((struct IORequest*) Timer2IO);
        }
        if (Timer2MP)
            DeletePort(Timer2MP);

        if (Timer1_WaitCheck)
        {
            if (TimerIO)
            {
                if (!CheckIO((struct IORequest *) TimerIO))
                {
                    AbortIO((struct IORequest *) TimerIO);
                    WaitIO((struct IORequest *) TimerIO);
                }
            }

            if (TimerIO)
            {
                CloseDevice((struct IORequest *) TimerIO);
                DeleteIORequest((struct IORequest*) TimerIO);
            }
            //while(GetMsg(TimerMP));
            if (TimerMP)
                DeletePort(TimerMP);

            if (Timer4_WaitCheck)
            {
                if (!CheckIO((struct IORequest *) Timer4IO))
                {
                    AbortIO((struct IORequest *) Timer4IO);
                    WaitIO((struct IORequest *) Timer4IO);
                }

                CloseDevice((struct IORequest *) Timer4IO);
                DeleteIORequest((struct IORequest*) Timer4IO);

                //while(GetMsg(Timer4MP));
                if (Timer4MP)
                    DeletePort(Timer4MP);

                if (Timer5_WaitCheck)
                {
                    if (!CheckIO((struct IORequest *) Timer5IO))
                    {
                        AbortIO((struct IORequest *) Timer5IO);
                        WaitIO((struct IORequest *) Timer5IO);
                    }

                    CloseDevice((struct IORequest *) Timer5IO);
                    DeleteIORequest((struct IORequest*) Timer5IO);

                    //while(GetMsg(Timer5MP));
                    if (Timer5MP)
                        DeletePort(Timer5MP);

                }

            }
        }
    }
    return TRUE;
}

