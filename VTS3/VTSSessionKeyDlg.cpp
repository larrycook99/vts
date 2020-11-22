// VTSSessionKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSSessionKeyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSSessionKeyDlg dialog


#pragma warning( disable : 4355 )
VTSSessionKeyDlg::VTSSessionKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSSessionKeyDlg::IDD, pParent)
	, m_sessionKey( this, IDC_SESSIONKEY )
	, m_Network( this, IDC_NETWORK )
	, m_MACAddress( this, IDC_MACADDRESS )
{
	//{{AFX_DATA_INIT(VTSSessionKeyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSSessionKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSSessionKeyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_sessionKey.UpdateData( pDX->m_bSaveAndValidate );
	m_Network.UpdateData( pDX->m_bSaveAndValidate );
	m_MACAddress.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSSessionKeyDlg, CDialog)
	//{{AFX_MSG_MAP(VTSSessionKeyDlg)
	ON_EN_CHANGE(IDC_NETWORK, OnChangeNetwork)
	ON_EN_CHANGE(IDC_SESSIONKEY, OnChangeSessionkey)
	ON_EN_CHANGE(IDC_MACADDRESS, OnChangeMacaddress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSSessionKeyDlg message handlers

void VTSSessionKeyDlg::OnChangeNetwork() 
{
 m_Network.UpdateData();
}

void VTSSessionKeyDlg::OnChangeSessionkey() 
{
 m_sessionKey.UpdateData();
}

void VTSSessionKeyDlg::OnChangeMacaddress() 
{
 m_MACAddress.UpdateData();
}

BOOL VTSSessionKeyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  
}
void VTSSessionKeyDlg::Encode( BACnetAPDUEncoder& enc, int context)
{
	m_sessionKey.Encode(enc);
	m_Network.Encode(enc);
	m_MACAddress.Encode(enc);
}
void VTSSessionKeyDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)
	{
		m_sessionKey.Decode(dec);
		m_Network.Decode(dec);
		m_MACAddress.Decode(dec);
		m_sessionKey.ctrlNull = false;
		m_Network.ctrlNull = false;
		m_MACAddress.ctrlNull = false;
	}
}
