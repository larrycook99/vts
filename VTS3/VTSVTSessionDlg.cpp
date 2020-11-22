// VTSVTSessionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSVTSessionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSVTSessionDlg dialog


#pragma warning( disable : 4355 )
VTSVTSessionDlg::VTSVTSessionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSVTSessionDlg::IDD, pParent)
	, m_localSessionID(this, IDC_LOCALSESSIONID)
	, m_remoteSessionID(this, IDC_REMOTESESSIONID)
	, m_Network( this, IDC_NETWORK )
	, m_MACAddress( this, IDC_MACADDRESS )
{
	//{{AFX_DATA_INIT(VTSVTSessionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void VTSVTSessionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSVTSessionDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_localSessionID.UpdateData( pDX->m_bSaveAndValidate );
	m_remoteSessionID.UpdateData( pDX->m_bSaveAndValidate );
	m_Network.UpdateData( pDX->m_bSaveAndValidate );
	m_MACAddress.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSVTSessionDlg, CDialog)
	//{{AFX_MSG_MAP(VTSVTSessionDlg)
	ON_EN_CHANGE(IDC_NETWORK, OnChangeNetwork)
	ON_EN_CHANGE(IDC_MACADDRESS, OnChangeMacaddress)
	ON_EN_CHANGE(IDC_LOCALSESSIONID, OnChangeLocalsessionid)
	ON_EN_CHANGE(IDC_REMOTESESSIONID, OnChangeRemotesessionid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSVTSessionDlg message handlers

void VTSVTSessionDlg::OnChangeNetwork() 
{
	m_Network.UpdateData();
}

void VTSVTSessionDlg::OnChangeMacaddress() 
{
	m_MACAddress.UpdateData();
}

void VTSVTSessionDlg::OnChangeLocalsessionid() 
{
	m_localSessionID.UpdateData();
}

void VTSVTSessionDlg::OnChangeRemotesessionid() 
{
   m_remoteSessionID.UpdateData();
}

BOOL VTSVTSessionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  
}

void VTSVTSessionDlg::Encode( BACnetAPDUEncoder& enc, int context)
{
	m_localSessionID.Encode(enc);
	m_remoteSessionID.Encode(enc);
	m_Network.Encode(enc);
	m_MACAddress.Encode(enc);
}
void VTSVTSessionDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)
	{
		m_localSessionID.Decode(dec);
		m_remoteSessionID.Decode(dec);
		m_Network.Decode(dec);
		m_MACAddress.Decode(dec);
		m_localSessionID.ctrlNull = false;
		m_remoteSessionID.ctrlNull = false;
		m_Network.ctrlNull = false;
		m_MACAddress.ctrlNull = false;
	}
}
