// VTSPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSPropertyPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSPropertyPage property page

IMPLEMENT_DYNCREATE(VTSPropertyPage, CPropertyPage)

VTSPropertyPage::VTSPropertyPage( int nIDD, VTSPageCallbackInterface * pparent )
				: CPropertyPage(nIDD)
{
	m_pparent = pparent;
}


VTSPropertyPage::VTSPropertyPage() : CPropertyPage()
{
	ASSERT(0);
	//{{AFX_DATA_INIT(VTSPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

VTSPropertyPage::~VTSPropertyPage()
{
}



BOOL VTSPropertyPage::OnKillActive() 
{
	return CPropertyPage::OnKillActive();
}


BOOL VTSPropertyPage::OnSetActive() 
{
	m_bFirstSetActive = FALSE;				// necessary for first time to call DDX
	return CPropertyPage::OnSetActive();
}



void VTSPropertyPage::NotifyOfDataChange(void)
{
	if ( m_pparent != NULL )
		m_pparent->DataChangeNotification();
}


void * VTSPropertyPage::RetrieveCurrentData(void)
{
	if ( m_pparent != NULL )
		return m_pparent->GetActiveData();

	return NULL;
}


BEGIN_MESSAGE_MAP(VTSPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(VTSPropertyPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPropertyPage message handlers
