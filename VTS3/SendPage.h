#if !defined(AFX_SENDPAGE_H__8707D034_2ADB_11D4_BE9D_00A0C95A9812__INCLUDED_)
#define AFX_SENDPAGE_H__8707D034_2ADB_11D4_BE9D_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendPage

class CSend;
typedef CSend *CSendPtr;

const int glMaxHistoryCount = 5;

class CSendPage : public CPropertyPage
{
public:
	CSendPtr	pageParent;				// pointer to parent CSend box  
	BOOL        isShown;                //if the page is shown now. Xiao Shiyuan 2002-12-5

	CSendPage( UINT id );
	virtual ~CSendPage( void );

	void UpdateEncoded( void );			// something changed, tell parent to update

	virtual void InitPage( void );						// give it a chance to init
	virtual void EncodePage( CByteArray* contents );	// encode the page into contents

	virtual void SavePage( void );						// save contents
	virtual void RestorePage( int index = 0 );			// restore contents to last saved values	
};

typedef CSendPage *CSendPagePtr, **CSendPageList;

//
//	CSendPageList
//
//	The CSend modeless dialog box keeps a list of pages that are installed.
//

typedef CSendPagePtr *CSendPageList;

//
//	CSendGroupItem
//
//	Each send group is composed of a series of pages that all share a base page.
//	Each item in the group has a name for the CSend::m_packetTree control and a pointer 
//	to the leftmost (and frontmost) page to display.
//

typedef CSendPage CSend::*CSendPageMPtr;	// ptr to page member of CSend
typedef CSendPageMPtr *CSendPageMList;		// pointer to a null terminated list of page pointers

struct CSendGroupItem {
	char*				itemName;			// item name in tree ctrl
	CSendPageMPtr		itemPage;			// page to display
	CSendPageMPtr		itemBase;			// specific base page
	};

typedef CSendGroupItem *CSendGroupItemPtr;

//
//	CSendGroup
//
//	The send group specifies a name for the items, a pointer to the list of items, 
//	an optional base page (which is displayed before each item in the group) and 
//	a followup page (displayed before the base page or item pages in a following 
//	group).
//

struct CSendGroup {
	char*				groupName;			// group name in tree ctrl
	CSendGroupItemPtr	groupItemList;		// ptr to null terminated list of items
	CSendPageMPtr		groupBasePage;		// base page for group (before each item)
	CSendPageMPtr		groupFollowupPage;	// page for following group
	};

typedef CSendGroup *CSendGroupPtr;

//
//	CSendGroupList
//
//	A send group list is a null terminated list of send group pointers.  Note that 
//	the base page in the group is displayed for subsequent groups, cascading to 
//	the base page for the subsequent group.
//

typedef CSendGroupPtr *CSendGroupList;

#endif // !defined(AFX_SENDPAGE_H__8707D034_2ADB_11D4_BE9D_00A0C95A9812__INCLUDED_)
