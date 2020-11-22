// VTSResultFlasDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSResultFlasDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSResultFlasDlg dialog


#pragma warning( disable : 4355 )
VTSResultFlasDlg::VTSResultFlasDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSResultFlasDlg::IDD, pParent)
	,m_firstItem(this, IDC_FIRSTITEM, true)
	,m_lastItem(this, IDC_LASTITEM, true)
	,m_moreItem(this, IDC_MOREITEM, true)
{
	//{{AFX_DATA_INIT(VTSResultFlasDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSResultFlasDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSResultFlasDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSResultFlasDlg, CDialog)
	//{{AFX_MSG_MAP(VTSResultFlasDlg)
	ON_BN_CLICKED(IDC_FIRSTITEM, OnFirstitem)
	ON_BN_CLICKED(IDC_LASTITEM, OnLastitem)
	ON_BN_CLICKED(IDC_MOREITEM, OnMoreitem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSResultFlasDlg message handlers

void VTSResultFlasDlg::OnFirstitem() 
{
	m_firstItem.UpdateData();
}

void VTSResultFlasDlg::OnLastitem() 
{
   m_lastItem.UpdateData();
}

void VTSResultFlasDlg::OnMoreitem() 
{
  m_moreItem.UpdateData();
}

BOOL VTSResultFlasDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_firstItem.ObjToCtrl();
	m_lastItem.ObjToCtrl();
	m_moreItem.ObjToCtrl();
	return TRUE;  
}
void VTSResultFlasDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	BACnetBitString		bits(3);
	bits.SetBit( 0, m_firstItem.boolValue );
	bits.SetBit( 1, m_lastItem.boolValue );
	bits.SetBit( 2, m_moreItem.boolValue );
	bits.Encode( enc, context);
}
void VTSResultFlasDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength!=0)
	{
		BACnetBitString		bits( 3 );
		bits.Decode(dec);
		m_firstItem.boolValue = (bits.GetBit(0)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
		m_lastItem.boolValue= (bits.GetBit(1)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
		m_moreItem.boolValue= (bits.GetBit(2)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
	}
}

