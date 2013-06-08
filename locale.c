/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include <proto/locale.h>

#include "intern.h"

/* Locals */
static struct Catalog *catalog = NULL;

CONST_STRPTR GCS(ULONG stringNum, char * defaultString)
{
    return GetCatalogStr(catalog, stringNum, (loc_in)defaultString);
}

void locale_openbuiltincatalog()
{
    if (catalog) CloseCatalog(catalog);

    catalog = OpenCatalog(NULL, (loc_in)"WookieChat.catalog", OC_BuiltInLanguage, NULL, TAG_DONE);

    if (!catalog)
    {

        if (DEBUG)
            printf("unable to use default language\n");

        catalog = OpenCatalog(NULL, (loc_in)"WookieChat.catalog", OC_Language, "english", TAG_DONE);

        if (!catalog)
        {
            if (DEBUG)
                printf("unable to load english catalog, using built in strings\n");

        }

    }
}

void locale_opencatalog(char * language)
{
    if (catalog) CloseCatalog(catalog);

    printf("loading catalog for language %s\n", language);

    catalog = OpenCatalog(NULL, (loc_in)"WookieChat.catalog", OC_Language, language, TAG_DONE);

    if (!catalog)
        locale_openbuiltincatalog();
}



void locale_closecatalog()
{
    CloseCatalog(catalog);
}
