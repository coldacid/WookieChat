/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** wookiechat.c
*/

/*************************************************************************/

#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "functions.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_network.h"
#include "version.h"
#include "intern.h"
#include "locale.h"

/*************************************************************************/

/* /// main()
**
*/

/*************************************************************************/

int main(int argc, char *argv[])
{
ULONG result = MSG_ERROR_NOERROR;

	SysBase = (APTR) (*((struct ExecBase **) 4 ) );

	WBMessage_Get();
	Locale_Open( (STRPTR)( APPLICATIONNAME "avoid catalog for now"), VERSION, REVISION );
	if( !Libraries_Open() ) {
		if( !( result = MUIClass_Open() ) ) { /* init our mui classes */
			Object *networkobj = (APTR) MUIGetVar( application, MA_APPLICATION_OBJECTNETWORK );
			IPTR waitsignals;
			LONG returnid;

			while( 1 ) {
				returnid = DoMethod( application, MUIM_Application_Input, &waitsignals );

				if( returnid == MUIV_Application_ReturnID_Quit ) {
					break;
				}

				if( !waitsignals ) {
					continue;
				}
				waitsignals |= SIGBREAKF_CTRL_C;

				DoMethod( networkobj, MM_NETWORK_WAITSELECT, &waitsignals );

				if( waitsignals & SIGBREAKF_CTRL_C ) {
					break;
				}
			}
		}
		MUIClass_Close(); /* close our mui classes */
	}
	if( result ) {
		ShowRequesterError( result, &result );
		result = 20;
	}
	Locale_Close();
	Libraries_Close();
	WBMessage_Reply(); /* release wbmessage */

	return( result );
}
/* \\\ */


