/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** system.h
*/

#ifndef SYSTEM_GENERAL_H
#define SYSTEM_GENERAL_H

/*************************************************************************/

#ifdef __AROS__

#include "system_aros.h"

#elif  __MORPHOS__

#include "system_morphos.h"

#elif __amigaos4__

#include "system_amigaos4.h"

#else

#include "system_classic.h"

#endif

/*************************************************************************/

#endif /* SYSTEM_GENERAL_H */

