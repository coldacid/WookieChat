#ifndef MUI4WRAPPER_H
#define MUI4WRAPPER_H 1

/*
** This file contains some magic. Basically it replaces mui nlist stuff
** bymui list stuff.
**
*/
/*************************************************************************/

#include <libraries/mui.h>

#if ENABLE_NLIST
 #include <mui/NList_mcc.h>
 #include <mui/NListview_mcc.h>
 #include <mui/NFloattext_mcc.h>
#else

/*************************************************************************/

/* floattext */

#undef  MUIC_NFloattext
#define MUIC_NFloattext MUIC_Floattext

#undef  NFloattextObject
#define NFloattextObject FloattextObject

#undef  MUIA_NFloattext_Text
#define MUIA_NFloattext_Text MUIA_Floattext_Text 

/* list view */

#undef  MUIC_NListview
#define MUIC_NListview MUIC_Listview

#undef  NListviewObject
#define NListviewObject ListviewObject

#undef  MUIA_NListview_NList
#define MUIA_NListview_NList MUIA_Listview_List

/* list */

#undef  MUIC_NList
#define MUIC_NList MUIC_List

#undef  NListObject
#define NListObject ListObject

#undef  MUIA_NList_Format
#define MUIA_NList_Format MUIA_List_Format

#undef  MUIA_NList_DragSortable
#define MUIA_NList_DragSortable MUIA_List_DragSortable

#undef  MUIA_NList_Active
#define MUIA_NList_Active MUIA_List_Active

#undef  MUIA_NList_TitleClick
#define MUIA_NList_TitleClick MUIA_List_TitleClick

#undef  MUIA_NList_DoubleClick
#define MUIA_NList_DoubleClick MUIA_List_DoubleClick

#undef  MUIA_NList_Quiet
#define MUIA_NList_Quiet MUIA_List_Quiet

#undef  MUIA_NList_Input
#define MUIA_NList_Input MUIA_List_Input

#undef  MUIM_NList_GetEntry
#define MUIM_NList_GetEntry MUIM_List_GetEntry

#undef  MUIV_NList_GetEntry_Active
#define MUIV_NList_GetEntry_Active MUIV_List_GetEntry_Active

#undef  MUIV_NList_Redraw_Active
#define MUIV_NList_Redraw_Active MUIV_List_Redraw_Active

#undef  MUIV_NList_Redraw_All
#define MUIV_NList_Redraw_All MUIV_List_Redraw_All

#undef  MUIA_NList_Input
#define MUIA_NList_Input MUIA_List_Input

#undef  MUIA_NList_Entries
#define MUIA_NList_Entries MUIA_List_Entries

#undef  MUIA_NList_Visible
#define MUIA_NList_Visible MUIA_List_Visible

#undef  MUIA_NList_First
#define MUIA_NList_First MUIA_List_First

#undef  MUIM_NList_Display
#define	MUIM_NList_Display MUIM_List_Display
struct  MUIP_NList_Display            { ULONG MethodID; APTR entry; STRPTR *strings; };

#undef  MUIM_NList_Compare
#define MUIM_NList_Compare MUIM_List_Compare
struct  MUIP_NList_Compare            { ULONG MethodID; APTR entry1; APTR entry2; LONG sort_type1; LONG sort_type2; };		/* $$$Sensei */

#undef  MUIM_NList_Construct
#define MUIM_NList_Construct MUIM_List_Construct
struct  MUIP_NList_Construct          { ULONG MethodID; APTR entry; APTR pool; };														/* $$$Sensei */

#undef  MUIM_NList_Destruct
#define MUIM_NList_Destruct MUIM_List_Destruct
struct  MUIP_NList_Destruct           { ULONG MethodID; APTR entry; APTR pool; };														/* $$$Sensei */

#undef  MUIM_NList_Sort
#define	MUIM_NList_Sort MUIM_List_Sort
struct  MUIP_NList_Sort            { ULONG MethodID; };

#undef  MUIM_NList_NextSelected
#define MUIM_NList_NextSelected MUIM_List_NextSelected

#undef  MUIV_NList_NextSelected_Start
#define MUIV_NList_NextSelected_Start MUIV_List_NextSelected_Start

#undef  MUIV_NList_Active_Bottom
#define MUIV_NList_Active_Bottom MUIV_List_Active_Bottom

#undef  MUIV_NList_Active_Top
#define MUIV_NList_Active_Top MUIV_List_Active_Top

#undef  MUIA_NList_First
#define MUIA_NList_First MUIA_List_First

#undef  MUIA_NList_AutoVisible
#define MUIA_NList_AutoVisible MUIA_List_AutoVisible

#undef  MUIA_NList_Title
#define MUIA_NList_Title MUIA_List_Title

#undef  MUIA_NList_InsertPosition
#define MUIA_NList_InsertPosition MUIA_List_InsertPosition

#undef  MUIA_NList_MultiSelect
#define MUIA_NList_MultiSelect MUIA_List_MultiSelect

#undef  MUIM_NList_TestPos
#define MUIM_NList_TestPos MUIM_List_TestPos

#undef  MUI_NList_TestPos_Result
#define MUI_NList_TestPos_Result MUI_List_TestPos_Result

#undef  MUIM_NList_Clear
#define MUIM_NList_Clear MUIM_List_Clear

#undef  MUIM_NList_InsertSingle
#define MUIM_NList_InsertSingle MUIM_List_InsertSingle

#undef  MUIV_NList_Insert_Bottom
#define MUIV_NList_Insert_Bottom MUIV_List_Insert_Bottom

#undef  MUIM_NList_Remove
#define MUIM_NList_Remove MUIM_List_Remove

#undef  MUIV_NList_Remove_Selected
#define MUIV_NList_Remove_Selected MUIV_List_Remove_Selected

#undef  MUIM_NList_Redraw
#define MUIM_NList_Redraw MUIM_List_Redraw

#undef  MUIM_NList_InsertSingleWrap
#define MUIM_NList_InsertSingleWrap MUIM_List_InsertSingle

#undef  WRAPCOL0
#define WRAPCOL0 0

#undef  ALIGN_LEFT
#define ALIGN_LEFT 0

#undef  MUIA_NListview_Horiz_ScrollBar
#define MUIA_NListview_Horiz_ScrollBar MUIA_Scrollgroup_AutoBars
#undef  MUIV_NListview_HSB_Auto
#define MUIV_NListview_HSB_Auto        TRUE

/*************************************************************************/

//#endif /* ENABLE_NLIST */

#if 0
#ifdef ENABLE_NLIST
#define NLISTVIEW_SCROLLBARS  MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Always, MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always,
#define NLIST_COLSORTABLE MUIA_NList_MinColSortable, 0,
#else
#define NLISTVIEW_SCROLLBARS
#define NLIST_COLSORTABLE
#endif
#endif

/*************************************************************************/

#endif /* ENABLE_NLIST */

#endif /* MUI4WRAPPER_H */

