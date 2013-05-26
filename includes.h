/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/dos.h>
#include <proto/utility.h>

#include <libraries/mui.h>
#include <mui/NListview_mcc.h>

#ifdef __amigaos4__
#error "Missing socket includes for AmigaOS4"
#elif __MORPHOS__
#error "Missing socket includes for MorphOS"
#elif __AROS__
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bsdsocket/socketbasetags.h>
#include <proto/socket.h>
#else
#error "Missing socket includes for AmigaOS3"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
