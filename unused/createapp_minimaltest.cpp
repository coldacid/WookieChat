/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

//I've removed nearly everything from CreateApp() and it still cant create the app..

struct ObjApp *CreateApp(void)
{

    if(AMIX) printf("1\n");

	struct ObjApp *MBObj;

	if (!(MBObj = (struct ObjApp *) AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
		return(NULL);


  	GROUP_ROOT_0 =(Object*)/*NewObject(mcc6->mcc_Class,NULL,  */(Object*)GroupObject,
            MUIA_Group_HorizSpacing, 0,
    		MUIA_Group_VertSpacing, 2,
            Child,HSpace(0),
            //Child, GR_top,
    		//Child, GR_string_gadget,
            //Child, GR_bottom_group,
	End;

    #ifdef __AROS__
    MBObj->WI_main =(Object*)NewObjectAROS(mcc4->mcc_Class,NULL, // (Object*)WindowObject,
    #else
	MBObj->WI_main =(Object*)/*NewObject(mcc4->mcc_Class,NULL, */ (Object*)WindowObject,
		#endif
        MUIA_Window_Title, "Wookiechat",
		MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_0,
	End;

#ifndef MUIA_Application_UsedClasses
#define MUIA_Application_UsedClasses 0x8042e9a7
#endif


if(AMIX) printf("132\n");
    MBObj->App =(Object*) ApplicationObject,
		MUIA_Application_Author, (char*)"James Carroll",
		MUIA_Application_Base, (char*)"WOOKIECHATtest",
		MUIA_Application_Title, (char*)"WookieChat",
		MUIA_Application_Version, (char*)VERSION_MUI,
		MUIA_Application_Copyright, (char*)"James Carroll",
		MUIA_Application_Description, (char*)"IRC Client",
        SubWindow, MBObj->WI_main,End;

    if(AMIX) printf("133\n");
	if (!MBObj->App)
	{
        if(AMIX) printf("134.. THE END..\n");
		FreeVec(MBObj);
        printf("%s",GetCatalogStr(catalog,133,"unable to create app\n"));
		return(NULL);
	}

if(AMIX) printf("135.. THE END.. \n");
	return(MBObj);
}


