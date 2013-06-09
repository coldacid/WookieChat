/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#if defined(__AROS__)
#define CATCOMP_ARRAY 1
#elif defined(__MORPHOS__)
#define CATCOMP_BLOCK 1
#else
#error Provide locale specific definition for your system
#endif
#define NEW_CATCOMP_ARRAY_IDS
#include "locale.h"

#include <proto/locale.h>
#include <proto/exec.h>


static struct Catalog *locale_catalog = NULL;
static struct Locale  *locale_locale;

/*************************************************************************/

/* /// Locale_Open()
**
*/

/*************************************************************************/

BOOL Locale_Open( STRPTR catname, ULONG version, ULONG revision)
{

	if( ( LocaleBase ) ) {
		RemLibrary( (struct Library *) LocaleBase );

		if( ( locale_locale = OpenLocale( NULL ) ) ) {
			if( ( locale_catalog = OpenCatalogA( locale_locale, catname, TAG_DONE ) ) ) {

				if(    locale_catalog->cat_Version  ==  version &&
					   locale_catalog->cat_Revision == revision ) {
					return( TRUE );
				}
				CloseCatalog( locale_catalog );         /* so close catalog */
				locale_catalog = NULL;                  /* and use default (if present) */
			}
		}
    }
	return FALSE;
}
/* \\\ */
/* /// Locale_Close()
**
*/

/*************************************************************************/

void Locale_Close(void)
{
    if( LocaleBase) {
        if( locale_catalog) {
            CloseCatalog(locale_catalog);
            locale_catalog = NULL;
        }
        if( locale_locale) {
            CloseLocale(locale_locale);
            locale_locale = NULL;
        }
		RemLibrary( (struct Library *) LocaleBase ); /* flush our catalog */
    }
}
/* \\\ */
/* /// Locale_GetString()
**
*/

/*************************************************************************/

STRPTR Locale_GetString( long id )
{
#if defined(__AROS__)
    if (LocaleBase != NULL && locale_catalog != NULL)
    {
        return (STRPTR)GetCatalogStr(locale_catalog, id, CatCompArray[id].cca_Str);
    }
    else
    {
        return CatCompArray[id].cca_Str;
    }
#elif defined(__MORPHOS__)
LONG   *l;
UWORD  *w;
STRPTR  builtin;

	l = (LONG *) CatCompBlock;

    while (*l != id ) {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtin = (STRPTR)((ULONG)l + 6);

    if ( locale_catalog && LocaleBase ) {
		return( (STRPTR) GetCatalogStr( locale_catalog, id, builtin ) );
    }
	return( builtin );
#else
#error Provide locale specific definition for your system
#endif
}
/* \\\ */

