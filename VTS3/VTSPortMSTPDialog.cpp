// VTSPortMSTPDialog.cpp: implementation of the VTSPortMSTPDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSPortMSTPDialog.h"
#include "WinMSTP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(VTSPortMSTPDialog, VTSPropertyPage)

VTSPortMSTPDialog::VTSPortMSTPDialog( VTSPageCallbackInterface * pparent )
                      :VTSPropertyPage(VTSPortMSTPDialog::IDD, pparent)
{
	//{{AFX_DATA_INIT(VTSPortMSTPDialog)
	m_nMaxMaster = 127;
	m_nMaxInfoFrame = 10;
	m_nMAC = 80;
	m_strBaud = _T("9600");
	m_strImpName = _T("NB-Link");
	//}}AFX_DATA_INIT

	m_pstrConfigParms = NULL;
}

VTSPortMSTPDialog::VTSPortMSTPDialog(void) : VTSPropertyPage()
{
	ASSERT(0);
}


VTSPortMSTPDialog::~VTSPortMSTPDialog()
{
}


void VTSPortMSTPDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPortMSTPDialog)
	DDX_Text(pDX, IDC_MSTPMAXMASTER, m_nMaxMaster);
	DDV_MinMaxInt(pDX, m_nMaxMaster, 0, 127);
	DDX_Text(pDX, IDC_MSTPMAXINFO, m_nMaxInfoFrame);
	DDV_MinMaxInt(pDX, m_nMaxInfoFrame, 0, 255);
	DDX_Text(pDX, IDC_MSTPMAC, m_nMAC);
	DDV_MinMaxInt(pDX, m_nMAC, 0, 127);
	DDX_CBString(pDX, IDC_MSTPBAUD, m_strBaud);
	DDX_CBString(pDX, IDC_MSTPIMP, m_strImpName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSPortMSTPDialog, VTSPropertyPage)
	//{{AFX_MSG_MAP(VTSPortMSTPDialog)
	ON_EN_CHANGE(IDC_NBLINKSOCKET, OnChangeNblinksocket)
	ON_CBN_SELCHANGE(IDC_MSTPIMP, OnSelchangeMstpimp)
	ON_EN_CHANGE(IDC_MSTPMAC, OnChangeMstpmac)
	ON_CBN_SELCHANGE(IDC_MSTPBAUD, OnSelchangeMstpbaud)
	ON_EN_CHANGE(IDC_MSTPMAXINFO, OnChangeMstpmaxinfo)
	ON_EN_CHANGE(IDC_MSTPMAXMASTER, OnChangeMstpmaxmaster)
	ON_BN_CLICKED(ID_MSTPCONFIGIMP, OnMstpconfigimp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPortPTPDialog message handlers

BOOL VTSPortMSTPDialog::OnSetActive() 
{
	m_pstrConfigParms = (CString * ) RetrieveCurrentData();

	// Reset control data
	CComboBox * pcombo = (CComboBox *) GetDlgItem(IDC_MSTPBAUD);
	pcombo->ResetContent();
	pcombo->AddString("9600");
	pcombo->AddString("19200");
	pcombo->AddString("38400");
	pcombo->AddString("76800");

	pcombo = (CComboBox *) GetDlgItem(IDC_MSTPIMP);
	pcombo->ResetContent();
	pcombo->AddString("NB-Link");			// 0 element = NB-Link
//	pcombo->AddString("Other");				// 1 element = other... etc.


	ObjToCtrl();

	// calls DDX
	VTSPropertyPage::OnSetActive();
	NotifyOfDataChange();

	// if we return FALSE here then this page will not be activated.  It will move on to the next. 
	// That's good because it's extremely painful to disable all of the stuff.

	return m_pstrConfigParms != NULL;
}
	


BOOL VTSPortMSTPDialog::OnKillActive() 
{
	// calls DDX
	VTSPropertyPage::OnKillActive();
	CtrlToObj();

	return TRUE;
}



void VTSPortMSTPDialog::ObjToCtrl()
{
	if ( m_pstrConfigParms == NULL )
		return;

	char szBaud[100];
	char szImpName[100];
	memset(szBaud, 0, sizeof(szBaud));
	memset(szImpName, 0, sizeof(szImpName));

	// insert code here to push decoded object data into dialog variables

	//MSTP config format for string is:
	// MAC, MaxMaster, MaxInfoFrame, Baud String, Implementation Name, Implementation Parm String
	// Ex:  80, 127, 10, 9600, NB-Link, [blah, blah, blah]
	// Only read the first common parms... save the bracketed stuff for the implementation

	sscanf(*m_pstrConfigParms, "%d, %d, %d, %s %s", &m_nMAC, &m_nMaxMaster, &m_nMaxInfoFrame, szBaud, szImpName );
	szBaud[sizeof(szBaud)-1] = 0;
	szImpName[sizeof(szImpName)-1] = 0;
	m_strBaud = szBaud;
	m_strImpName = szImpName;

	int nPos = m_pstrConfigParms->Find('[');
	if ( nPos != -1 )
	{
		m_strImpParms = m_pstrConfigParms->Right(m_pstrConfigParms->GetLength()-nPos-1);
		if ( m_strImpParms[m_strImpParms.GetLength()-1] == ']' )
			m_strImpParms = m_strImpParms.Left(m_strImpParms.GetLength()-1);
	}

//	CDataExchange ddx(this, FALSE);
//	DoDataExchange(&ddx);
}


void VTSPortMSTPDialog::CtrlToObj()
{
	if ( m_pstrConfigParms == NULL )
		return;

	// insert code here to encode dialog data variables into string
//	CDataExchange ddx(this, TRUE);
//	DoDataExchange(&ddx);

	//MSTP config format for string is:
	// MAC, MaxMaster, MaxInfoFrame, Baud String, Implementation Name, Implementation Parm String
	// Ex:  80, 127, 10, 9600, NB-Link, [ blah, blah, blah ]

	m_pstrConfigParms->Format("%d, %d, %d, %s %s [%s]", m_nMAC, m_nMaxMaster, m_nMaxInfoFrame, m_strBaud, m_strImpName, m_strImpParms);
}


void VTSPortMSTPDialog::OnChangeNblinksocket() 
{
	UpdateData( true );
	CtrlToObj();
	NotifyOfDataChange();
}


void VTSPortMSTPDialog::OnSelchangeMstpimp() 
{
	UpdateData( true );
	CtrlToObj();
	NotifyOfDataChange();
}

void VTSPortMSTPDialog::OnChangeMstpmac() 
{
	UpdateData( true );
	CtrlToObj();
	NotifyOfDataChange();
}

void VTSPortMSTPDialog::OnSelchangeMstpbaud() 
{
	UpdateData( true );
	CtrlToObj();
	NotifyOfDataChange();
}


void VTSPortMSTPDialog::OnChangeMstpmaxinfo() 
{
	UpdateData( true );
	CtrlToObj();
	NotifyOfDataChange();
}

void VTSPortMSTPDialog::OnChangeMstpmaxmaster() 
{
	UpdateData( true );
	CtrlToObj();
	NotifyOfDataChange();
}


void VTSPortMSTPDialog::OnMstpconfigimp() 
{
	// Create temporary implementation object according to parameters...
	// Don't init !!
	// Tell it to config and suck out the config... like a crawdad!

	UpdateData( true );
	CtrlToObj();

	CComboBox * pcombo = (CComboBox *) GetDlgItem(IDC_MSTPIMP);
	MSTPImplementation * pimplementation = NULL;

	switch( pcombo->GetCurSel() )
	{
		case 0:		// NB-Link
					pimplementation = new MSTPImp_NBLink(NULL, m_strImpParms);
					break;
	}
	// 3/1/2006
	// Submitted by: Alan.Wood@nz.schneider-electric.com
	if(pimplementation == NULL)
		return;

	WinMSTPData * pdata = new WinMSTPData(m_strBaud, m_nMAC, m_nMaxMaster, m_nMaxInfoFrame);
	pimplementation->m_pmstpData = pdata;		// so dialog can also deal with MSTP data as well as implementation specific stuff

	if ( pimplementation != NULL &&  pimplementation->ConfigureDlg(this) == IDOK )
	{
		pimplementation->LoadConfigString(&m_strImpParms);
		CtrlToObj();
		NotifyOfDataChange();

	}

	// MSD: 7/29/04 - added to fix memory leak if the user selected the Cancel button
    if (pimplementation != NULL)
		delete pimplementation;

	delete pdata;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(VTSPortNullDialog, VTSPropertyPage)

VTSPortNullDialog::VTSPortNullDialog( VTSPageCallbackInterface * pparent )
                      :VTSPropertyPage(VTSPortNullDialog::IDD, pparent)
{
}


BEGIN_MESSAGE_MAP(VTSPortNullDialog, VTSPropertyPage)
	//{{AFX_MSG_MAP(VTSPortNullDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL VTSPortNullDialog::OnSetActive() 
{
	// need to call this to notify parent of selection change
	if ( RetrieveCurrentData() != NULL )
		NotifyOfDataChange();

	VTSPropertyPage::OnSetActive();
	return TRUE;
}

