// VTSLimitEnableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSLimitEnableDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSLimitEnableDlg dialog


#pragma warning( disable : 4355 )
VTSLimitEnableDlg::VTSLimitEnableDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSLimitEnableDlg::IDD, pParent)
	, m_lowLimit(this, IDC_LOWLIMIT, true)
	, m_highLimit(this, IDC_HIGHLIMIT, true)
{
	//{{AFX_DATA_INIT(VTSLimitEnableDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSLimitEnableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSLimitEnableDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSLimitEnableDlg, CDialog)
	//{{AFX_MSG_MAP(VTSLimitEnableDlg)
	ON_BN_CLICKED(IDC_LOWLIMIT, OnLowlimit)
	ON_BN_CLICKED(IDC_HIGHLIMIT, OnHighlimit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSLimitEnableDlg message handlers

void VTSLimitEnableDlg::OnLowlimit() 
{
 m_lowLimit.UpdateData();
}

void VTSLimitEnableDlg::OnHighlimit() 
{
 m_highLimit.UpdateData();
}

BOOL VTSLimitEnableDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_lowLimit.ObjToCtrl();
	m_highLimit.ObjToCtrl();
	return TRUE;  
}
void VTSLimitEnableDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	BACnetBitString		bits(2 );
	bits.SetBit( 0, m_lowLimit.boolValue );
	bits.SetBit( 1, m_highLimit.boolValue );
	bits.Encode( enc, context);
}
void VTSLimitEnableDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength!=0)
	{
		BACnetBitString		bits( 2 );
		bits.Decode(dec);
		m_lowLimit.boolValue = (bits.GetBit(0)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
		m_highLimit.boolValue= (bits.GetBit(1)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
	}
}
