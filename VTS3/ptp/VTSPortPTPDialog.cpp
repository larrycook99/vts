// VTSPortPTPDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "VTSPortPTPDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// VTSPortPTPDialog dialog


IMPLEMENT_DYNCREATE(VTSPortPTPDialog, VTSPropertyPage)

VTSPortPTPDialog::VTSPortPTPDialog( VTSPageCallbackInterface * pparent )
                      :VTSPropertyPage(VTSPortPTPDialog::IDD, pparent)
{
	//{{AFX_DATA_INIT(VTSPortPTPDialog)
	m_strCOM = _T("");
	m_strBaud = _T("");
	//}}AFX_DATA_INIT

	m_pstrConfigParms = NULL;
}

VTSPortPTPDialog::VTSPortPTPDialog(void) : VTSPropertyPage()
{
	ASSERT(0);
}


VTSPortPTPDialog::~VTSPortPTPDialog()
{
}


void VTSPortPTPDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPortPTPDialog)
	DDX_CBString(pDX, IDC_PORTCOMBO, m_strCOM);
	DDX_CBString(pDX, IDC_BAUDRATE, m_strBaud);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSPortPTPDialog, VTSPropertyPage)
	//{{AFX_MSG_MAP(VTSPortPTPDialog)
	ON_CBN_SELCHANGE(IDC_PORTCOMBO, OnDataChange)
	ON_CBN_SELCHANGE(IDC_BAUDRATE, OnDataChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPortPTPDialog message handlers

BOOL VTSPortPTPDialog::OnSetActive() 
{
	// Call the first time to get the data... this will cause the parent to 
	// update the list... 

	m_pstrConfigParms = (CString * ) RetrieveCurrentData();

	ObjToCtrl();

	// Called DDX
	VTSPropertyPage::OnSetActive();

	// Call it again to tell the parent to refresh it's data..
	NotifyOfDataChange();

	// if we return FALSE here then this page will not be activated.  It will move on to the next. 
	// That's good because it's extremely painful to disable all of the stuff.

	return m_pstrConfigParms != NULL;
}
	


BOOL VTSPortPTPDialog::OnKillActive() 
{
	// calls DDX
	VTSPropertyPage::OnKillActive();
	CtrlToObj();

	return TRUE;
}



void VTSPortPTPDialog::ObjToCtrl()
{
	if ( m_pstrConfigParms == NULL )
		return;

	// Check for validity of existing data.. assume all bad if first part no good
	if ( m_pstrConfigParms->Find("COM") != 0  ||  m_pstrConfigParms->Find(':') != 4 )
		*m_pstrConfigParms = "COM1:9600,N,8,1";

	m_strCOM = m_pstrConfigParms->Left(4);
	int n = m_pstrConfigParms->Find(',');

	if ( n != -1 )
		m_strBaud = m_pstrConfigParms->Mid(5, n-5);
	else
		m_strBaud = "9600";

/* madanner 5/03.  Is this necessary?  Baud was not being saved...
	DCB dcb;
	if ( !BuildCommDCB(m_strCOM, &dcb) )
		return;
	 
    m_strBaud.Format("%d", dcb.BaudRate);
*/
}


//	When the user accepts the configuration information and clicks OK, 
//	this function is called which transfers the new configuration into 
//	the config data provided by the caller, VTSPort::Configuration.

void VTSPortPTPDialog::CtrlToObj()
{
	if ( m_pstrConfigParms == NULL )
		return;

	*m_pstrConfigParms = m_strCOM + ":" + m_strBaud + ",N,8,1";
}


void VTSPortPTPDialog::OnDataChange() 
{
	UpdateData( true );
	CtrlToObj();
	NotifyOfDataChange();
}
