/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

void MyTimerTask(void)
{

        //struct MsgPort *timertask_msgport=CreatePort("wookiechat_timertask_msgport",0);
        //struct XYMessage *timertask_msg;

        while(1)
        {
            Delay(100);
            Signal(MainTask,(1L<<SIGNAL_TIMER));
            
        }

        /*ReplyMsg((struct Message*)timertask_msg);

        while(GetMsg(timertask_msgport));

        RemPort(timertask_msgport);
        DeletePort(timertask_msgport);

        printf("timer task finished\n");
        */
}

