/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#define VERSION_STING "WookieChat 2.16"
#define VERSION_CTCP_SHARED VERSION_STING" - http://sourceforge.net/projects/wookiechat/"

#define VERSION_MUI "$VER: "VERSION_STING" (25.05.2013)"
#ifdef __amigaos4__
#define VERSION_CTCP VERSION_CTCP_SHARED" (AmigaOS 4.x binary)"
#elif __MORPHOS__
#define VERSION_CTCP VERSION_CTCP_SHARED" (MorphOS binary)"
#elif __AROS__
#define VERSION_CTCP VERSION_CTCP_SHARED" (AROS binary)"
#else
#define VERSION_CTCP VERSION_CTCP_SHARED" (AmigaOS 3.x binary)"
#endif
#define VERSION_ABOUT "\033c"VERSION_STING" - 2013-05-25\nhttp://sourceforge.net/projects/wookiechat/\n"

