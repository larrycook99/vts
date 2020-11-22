// VTSInconsistentParsDlg.cpp: implementation of the VTSInconsistentParsDlg class.
// Lori Tribble - 10/17/2009
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSInconsistentParsDlg.h"
#include "VTSObjectIdentifierDialog.h"
#include "VTSDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSInconsistentParsDlg dialog


#pragma warning( disable : 4355 )
VTSInconsistentParsDlg::VTSInconsistentParsDlg(const VTSNames& names, const VTSPorts& ports, 
										 CWnd* pParent /*=NULL*/)
	: CDialog(VTSInconsistentParsDlg::IDD, pParent),
	  m_names(names),
	  m_ports(ports)
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
{
	//{{AFX_DATA_INIT(VTSInconsistentParsDlg)
	m_nFunction = -1;
	m_strPort = _T("");
	m_strDevice = _T("");
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSInconsistentParsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(VTSInconsistentParsDlg)
  DDX_Control(pDX, IDC_INVALIDTAG, m_funCtrl);
  DDX_Control(pDX, IDC_PORTCOMBO, m_portCtrl);
  DDX_Control(pDX, IDC_DEVICECOMBO, m_deviceCtrl);
  DDX_Radio(pDX, IDC_INVALIDTAG, m_nFunction);
  DDX_CBString(pDX, IDC_PORTCOMBO, m_strPort);
  DDX_CBString(pDX, IDC_DEVICECOMBO, m_strDevice);
  //}}AFX_DATA_MAP
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSInconsistentParsDlg, CDialog)
	//{{AFX_MSG_MAP(VTSInconsistentParsDlg)
	ON_CBN_SELENDOK(IDC_DEVICECOMBO, OnSelendokDevicecombo)
	ON_CBN_SELENDOK(IDC_PORTCOMBO, OnSelendokPortcombo)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSInconsistentParsDlg message handlers

BOOL VTSInconsistentParsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i;
	for ( i = 0; i < m_names.GetSize(); i++ )
	{
		m_deviceCtrl.AddString( ((VTSName *) m_names.GetAt(i))->m_strName );
	}
	m_deviceCtrl.SelectString(-1, "IUT");

	VTSPortPtr pPort;
	for ( i = 0; i < m_ports.GetSize(); i++ )
	{
		pPort = (VTSPort *) m_ports.GetAt(i);
		if (pPort->IsEnabled())
		{
			m_portCtrl.AddString( pPort->m_strName );
		}
	}
	m_portCtrl.SetCurSel(0);

	m_propCombo.LoadCombo();
	m_propCombo.ObjToCtrl();

	m_funCtrl.SetCheck(2);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void VTSInconsistentParsDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if (m_strPort.IsEmpty())
	{
		AfxMessageBox("VTS port must be specified!");
		m_portCtrl.SetFocus();
		return;
	}
	if (m_strDevice.IsEmpty())
	{
		AfxMessageBox("Name must be specified!");
		m_deviceCtrl.SetFocus();
		return;
	}
	
	CDialog::OnOK();
}

void VTSInconsistentParsDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSInconsistentParsDlg::OnSelendokDevicecombo() 
{
	// TODO: Add your control notification handler code here
	CString strDevice;
	m_deviceCtrl.GetWindowText(strDevice);
	CString strPort;
	m_portCtrl.GetWindowText(strPort);

	// consisteny check between Device and Port
	VTSPortPtr pPort;
	int index = const_cast<VTSNames&>(m_names).FindIndex(strDevice);
	ASSERT(index != -1);
	VTSName* pName = m_names.GetAt(index);
	if (pName->m_pportLink != NULL)
	{
		pPort = pName->m_pportLink;
		if (pPort->m_strName.CompareNoCase(strPort))
		{
			m_portCtrl.SetCurSel(-1);
		}
	}
}


void VTSInconsistentParsDlg::OnSelendokPortcombo() 
{
	// TODO: Add your control notification handler code here
	CString strPort;
	m_portCtrl.GetWindowText(strPort);

}

void VTSInconsistentParsDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSInconsistentParsDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSInconsistentParsDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}


