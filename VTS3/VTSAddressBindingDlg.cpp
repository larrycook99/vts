// VTSAddressBindingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSAddressBindingDlg.h"
#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSAddressBindingDlg dialog

#pragma warning( disable : 4355 )
VTSAddressBindingDlg::VTSAddressBindingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSAddressBindingDlg::IDD, pParent)
	, m_ObjectIDCtrl(this,IDC_ADDRESSBINDING_OBJECTID)
	, m_NetworkNumberCtrl(this,IDC_ADDRESSBINDING_NETWORKNUMBER)
	, m_MACAddressCtrl(this,IDC_ADDRESSBINDING_MACADDRESS)
	, m_pValue(NULL)
{
	//{{AFX_DATA_INIT(VTSAddressBindingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void VTSAddressBindingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSAddressBindingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSAddressBindingDlg, CDialog)
	//{{AFX_MSG_MAP(VTSAddressBindingDlg)
	ON_BN_CLICKED(IDC_ADDRESSBINDING_OIDBUTTON, OnOid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSAddressBindingDlg message handlers
void VTSAddressBindingDlg::OnOK() 
{
	// TODO: Add extra validation here

	if(m_pValue != NULL)
		delete m_pValue;
	
	m_ObjectIDCtrl.CtrlToObj();
	m_NetworkNumberCtrl.CtrlToObj();
	m_MACAddressCtrl.CtrlToObj();

	m_pValue = new BACnetAddressBinding(m_ObjectIDCtrl.objID, (unsigned short)m_NetworkNumberCtrl.uintValue, 
										m_MACAddressCtrl.strBuff, m_MACAddressCtrl.strLen);

	CDialog::OnOK();
}

BOOL VTSAddressBindingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_pValue == NULL)
		m_pValue = new BACnetAddressBinding();
	
	m_NetworkNumberCtrl.uintValue = m_pValue->m_bacnetAddr.m_bacnetAddress.addrNet;
	m_NetworkNumberCtrl.ctrlNull=false;
	m_NetworkNumberCtrl.ObjToCtrl();

	m_MACAddressCtrl.Insert(m_pValue->m_bacnetAddr.m_bacnetAddress.addrAddr, m_pValue->m_bacnetAddr.m_bacnetAddress.addrLen, 0);
	m_MACAddressCtrl.ctrlNull=false;
	m_MACAddressCtrl.ObjToCtrl();

	BACnetObjectIdentifier* p3=&m_ObjectIDCtrl;
	*p3 = m_pValue->m_bacnetObjectID;
	m_ObjectIDCtrl.ctrlNull=false;
	m_ObjectIDCtrl.ObjToCtrl();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void VTSAddressBindingDlg::OnOid() 
{
	// TODO: Add your control notification handler code here
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectIDCtrl.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectIDCtrl.ctrlNull = false;
		m_ObjectIDCtrl.objID = dlg.objID;
		m_ObjectIDCtrl.ObjToCtrl();
	}
}

void VTSAddressBindingDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (m_pValue != NULL)
		delete m_pValue;
	if (dec.pktLength != 0)
		m_pValue = new BACnetAddressBinding(dec);
}

void VTSAddressBindingDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
	if(m_pValue != NULL)
		m_pValue->Encode(enc, context);
}

