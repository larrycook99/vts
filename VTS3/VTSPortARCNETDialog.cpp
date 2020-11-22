// VTSPortARCNETDialog.cpp: implementation of the VTSPortARCNETDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSPortARCNETDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(VTSPortARCNETDialog, VTSPropertyPage)

VTSPortARCNETDialog::VTSPortARCNETDialog( VTSPageCallbackInterface * pparent )
                      :VTSPropertyPage(VTSPortARCNETDialog::IDD, pparent)
{
	//{{AFX_DATA_INIT(VTSPortARCNETDialog)
	//}}AFX_DATA_INIT

	m_pstrConfigParms = NULL;
}

VTSPortARCNETDialog::VTSPortARCNETDialog(void) : VTSPropertyPage()
{
	ASSERT(0);
}


VTSPortARCNETDialog::~VTSPortARCNETDialog()
{
}


void VTSPortARCNETDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPortARCNETDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSPortARCNETDialog, VTSPropertyPage)
	//{{AFX_MSG_MAP(VTSPortARCNETDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPortPTPDialog message handlers

BOOL VTSPortARCNETDialog::OnSetActive() 
{
	m_pstrConfigParms = (CString * ) RetrieveCurrentData();

	ObjToCtrl();

	// calls DDX
	VTSPropertyPage::OnSetActive();

	// if we return FALSE here then this page will not be activated.  It will move on to the next. 
	// That's good because it's extremely painful to disable all of the stuff.

	return FALSE; // change to this when implemented... m_pstrConfigParms != NULL;
}
	


BOOL VTSPortARCNETDialog::OnKillActive() 
{
	// calls DDX
	VTSPropertyPage::OnKillActive();
	CtrlToObj();

	return TRUE;
}



void VTSPortARCNETDialog::ObjToCtrl()
{
	if ( m_pstrConfigParms == NULL )
		return;

	// insert code here to push decoded object data into dialog variables
}


void VTSPortARCNETDialog::CtrlToObj()
{
	if ( m_pstrConfigParms == NULL )
		return;

	// insert code here to encode dialog data variables into string
}
