// VTSNotificationParameters.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "VTSNotificationParameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSNotificationParameters

IMPLEMENT_DYNAMIC(VTSNotificationParameters, CPropertySheet)

//
//	VTSNotificationParameters::VTSNotificationParameters
//

VTSNotificationParameters::VTSNotificationParameters(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPages();
}

VTSNotificationParameters::VTSNotificationParameters(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPages();
}

//
//	VTSNotificationParameters::~VTSNotificationParameters
//

VTSNotificationParameters::~VTSNotificationParameters()
{
}

//
//	VTSNotificationParameters::AddPages
//

void VTSNotificationParameters::AddPages( void )
{
	// nothing has been set yet
	ctrlNull = true;
	m_EventType = 0;

	// add the pages
	AddPage( &BitstringPage );
	AddPage( &StatePage );
	AddPage( &ValuePage );
	AddPage( &CmdFailurePage );
	AddPage( &FloatLimitPage );
	AddPage( &OutOfRangePage );
	AddPage( &ComplexPage );
	AddPage( &BufferReadyPage );       	//Added by Zhu Zhenhua, 2004-5-17

	AddPage( &ChgLifeSafetyPage );       	//Added by LJT 4/3/2008
	AddPage( &ExtendedPage );       	//Added by LJT 4/3/2008
	AddPage( &UnsignedRangePage );       	//Added by LJT 4/3/2008

	// tell them who their parent is
	BitstringPage.pageParent = this;
	StatePage.pageParent = this;
	ValuePage.pageParent = this;
	CmdFailurePage.pageParent = this;
	FloatLimitPage.pageParent = this;
	OutOfRangePage.pageParent = this;
	ComplexPage.pageParent = this;
	BufferReadyPage.pageParent = this;

	ChgLifeSafetyPage.pageParent = this;
	ExtendedPage.pageParent = this;
	UnsignedRangePage.pageParent = this;
}

BEGIN_MESSAGE_MAP(VTSNotificationParameters, CPropertySheet)
	//{{AFX_MSG_MAP(VTSNotificationParameters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSNotificationParameters::Encode
//
//	To encode this structure, pass the encoding request on to the current page.
//

void VTSNotificationParameters::Encode( BACnetAPDUEncoder& enc, int context )
{
	// establish the context for the parameter
	BACnetOpeningTag().Encode( enc, context );

	// now let the current page encode itself (the context isn't used)
	switch (m_EventType) {
		case 0:		BitstringPage.Encode( enc, context );	break;
		case 1:		StatePage.Encode( enc, context );		break;
		case 2:		ValuePage.Encode( enc, context );		break;
		case 3:		CmdFailurePage.Encode( enc, context );	break;
		case 4:		FloatLimitPage.Encode( enc, context );	break;
		case 5:		OutOfRangePage.Encode( enc, context );	break;
		case 6:		ComplexPage.Encode( enc, context );		break;
		case 8:		ChgLifeSafetyPage.Encode( enc, context); break;
		case 9:		ExtendedPage.Encode( enc, context );	break;
		case 10:    BufferReadyPage.Encode(enc, context);	break; 	//Added by Zhu Zhenhua, 2004-5-17
		case 11:	UnsignedRangePage.Encode(enc, context);		break;
	}

	// finished with this context
	BACnetClosingTag().Encode( enc, context );
}

//
//	VTSNotificationParameters::OnInitDialog
//
//	This member function is called when the dialog box is first initialized before
//	display on the screen.  It will be called for each time it is about to be 
//	displayed, so the page to display should be the one that was displayed the last 
//	time the dialog box was closed.
//
//	When a property sheet is initialized, the first page is made the active page.
//	So thanks to the VTSNotifyBitstring::OnSetActive() function, the m_currentPage
//	gets set to zero.  So this function stores a copy of what the current page should 
//	be, then sets that page after the init happens.
//

BOOL VTSNotificationParameters::OnInitDialog() 
{
	int		curPage
	;
	BOOL	bResult
	;		
	// save the current page
	curPage = m_EventType;

	// pass the init dialog call down the chain, which sets page 0 active!
	bResult = CPropertySheet::OnInitDialog();
	
	// restore the current page
	m_EventType = curPage;

	// set the active page
	SetActivePage( m_EventType );

	// assume that at least something has been set
	ctrlNull = false;

	return bResult;
}
