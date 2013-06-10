/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass.c
*/

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <libraries/mui.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowabout.h"
#include "muiclass_windowurlgrabber.h"

/*
** GUI Globals
*/

struct MUI_CustomClass *appclasses[ CLASSID_LAST ];

/*************************************************************************/

/* /// MUIClass_Open()
**
*/

/*************************************************************************/

ULONG MUIClass_Open( void )
{
ULONG result;

	if( !(result = MCC_WindowAbout_InitClass() ) ) {
		if( !(result = MCC_WindowURLGrabber_InitClass() ) ) {
		}
	}
	return( result );
}
/* \\\ */
/* /// MUIClass_Close()
**
*/

/*************************************************************************/

void MUIClass_Close( void )
{
	MCC_WindowAbout_DisposeClass();
	MCC_WindowURLGrabber_DisposeClass();
}
/* \\\ */


