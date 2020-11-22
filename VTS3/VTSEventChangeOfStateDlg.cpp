// VTSEventChangeOfStateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventChangeOfStateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSEventChangeOfStateDlg dialog


#pragma warning( disable : 4355 )
VTSEventChangeOfStateDlg::VTSEventChangeOfStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventChangeOfStateDlg::IDD, pParent)
	,m_timeDelay(this, IDC_TIMEDELAY)
{
	//{{AFX_DATA_INIT(VTSEventChangeOfStateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEventChangeOfStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventChangeOfStateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_timeDelay.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSEventChangeOfStateDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventChangeOfStateDlg)
	ON_EN_CHANGE(IDC_TIMEDELAY, OnChangeTimedelay)
	ON_BN_CLICKED(IDC_LISTOFPROPSTATES, OnListofpropstates)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventChangeOfStateDlg message handlers

void VTSEventChangeOfStateDlg::OnChangeTimedelay() 
{
	m_timeDelay.UpdateData();
}

BOOL VTSEventChangeOfStateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  
}

void VTSEventChangeOfStateDlg::OnListofpropstates() 
{
	m_listofpropstates.DoModal();
}
void VTSEventChangeOfStateDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	m_timeDelay.Encode(enc, 0);	
	BACnetOpeningTag().Encode(enc, 1);
	m_listofpropstates.Encode(enc);
	BACnetClosingTag().Encode(enc, 1);
}
void VTSEventChangeOfStateDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength != 0)
	{
	m_timeDelay.Decode(dec);
	m_timeDelay.ctrlNull = false;
	BACnetOpeningTag().Decode(dec);
	m_listofpropstates.Decode(dec);
	BACnetClosingTag().Decode(dec);
	}

}
