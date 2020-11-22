// VTSEventBufferReadyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventBufferReadyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSEventBufferReadyDlg dialog

#pragma warning( disable : 4355 )
VTSEventBufferReadyDlg::VTSEventBufferReadyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventBufferReadyDlg::IDD, pParent)
	, m_ntfthreshold(this, IDC_NTFTHRESH)
	, m_prevntfcount(this, IDC_PREVIOUSNOTFC)
{
	//{{AFX_DATA_INIT(VTSEventBufferReadyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEventBufferReadyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventBufferReadyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_ntfthreshold.UpdateData( pDX->m_bSaveAndValidate );
	m_prevntfcount.UpdateData( pDX->m_bSaveAndValidate );	
}


BEGIN_MESSAGE_MAP(VTSEventBufferReadyDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventBufferReadyDlg)
	ON_EN_CHANGE(IDC_NTFTHRESH, OnChangeNtfthresh)
	ON_EN_CHANGE(IDC_PREVIOUSNOTFC, OnChangePreviousnotf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventBufferReadyDlg message handlers

void VTSEventBufferReadyDlg::OnChangeNtfthresh() 
{
	m_ntfthreshold.UpdateData();	
}

void VTSEventBufferReadyDlg::OnChangePreviousnotf() 
{
	m_prevntfcount.UpdateData();	
}
void VTSEventBufferReadyDlg::Encode(BACnetAPDUEncoder& enc, int context)
{	
	m_ntfthreshold.Encode(enc, 0);	
	m_prevntfcount.Encode(enc, 1);
	
}
void VTSEventBufferReadyDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{	
		m_ntfthreshold.Decode(dec);
		m_ntfthreshold.ctrlNull = false;
		m_prevntfcount.Decode(dec);
		m_prevntfcount.ctrlNull = false;		
	}
}
