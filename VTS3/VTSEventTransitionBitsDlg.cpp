// VTSEventTransitionBitsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventTransitionBitsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSEventTransitionBitsDlg dialog

#pragma warning( disable : 4355 )
VTSEventTransitionBitsDlg::VTSEventTransitionBitsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventTransitionBitsDlg::IDD, pParent)
	, m_offNormal(this, IDC_OFFNORMAL, true)
	, m_fault(this, IDC_FAULT, true)
	, m_normal(this, IDC_NORMAL, true)
{
	//{{AFX_DATA_INIT(VTSEventTransitionBitsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEventTransitionBitsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventTransitionBitsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSEventTransitionBitsDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventTransitionBitsDlg)
	ON_BN_CLICKED(IDC_FAULT, OnFault)
	ON_BN_CLICKED(IDC_NORMAL, OnNormal)
	ON_BN_CLICKED(IDC_OFFNORMAL, OnOffnormal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventTransitionBitsDlg message handlers

void VTSEventTransitionBitsDlg::OnFault() 
{
 m_fault.UpdateData();
}

void VTSEventTransitionBitsDlg::OnNormal() 
{
 m_normal.UpdateData();
}

void VTSEventTransitionBitsDlg::OnOffnormal() 
{
 m_offNormal.UpdateData();
}

BOOL VTSEventTransitionBitsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_offNormal.ObjToCtrl();
	m_fault.ObjToCtrl();
	m_normal.ObjToCtrl();	
	return TRUE;  
}
void VTSEventTransitionBitsDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	BACnetBitString		bits( 3 );
	bits.SetBit( 0, m_offNormal.boolValue );
	bits.SetBit( 1, m_fault.boolValue );
	bits.SetBit( 2, m_normal.boolValue );
	bits.Encode( enc, context);
}
void VTSEventTransitionBitsDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength!=0)
	{
		BACnetBitString		bits( 3 );
		bits.Decode(dec);
		m_offNormal.boolValue = (bits.GetBit(0)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
		m_fault.boolValue= (bits.GetBit(1)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
		m_normal.boolValue =  (bits.GetBit(2)!=0)?VTSBooleanCtrl::bTrue:	VTSBooleanCtrl::bFalse;
	}
}
