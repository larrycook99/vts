// VTSEventBitstringDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventBitstringDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSEventBitstringDlg dialog


#pragma warning( disable : 4355 )
VTSEventBitstringDlg::VTSEventBitstringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventBitstringDlg::IDD, pParent)
	,m_timeDelay(this, IDC_TIMEDELAY)
	,m_bitmask(this, IDC_BITMASK)
{
	//{{AFX_DATA_INIT(VTSEventBitstringDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEventBitstringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventBitstringDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_timeDelay.UpdateData( pDX->m_bSaveAndValidate );
	m_bitmask.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSEventBitstringDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventBitstringDlg)
	ON_EN_CHANGE(IDC_TIMEDELAY, OnChangeTimedelay)
	ON_EN_CHANGE(IDC_BITMASK, OnChangeBitmask)
	ON_BN_CLICKED(IDC_LISTOFBSTR, OnListofbstr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventBitstringDlg message handlers

void VTSEventBitstringDlg::OnChangeTimedelay() 
{
	m_timeDelay.UpdateData();
}

void VTSEventBitstringDlg::OnChangeBitmask() 
{
	m_bitmask.UpdateData();	
}

void VTSEventBitstringDlg::OnListofbstr() 
{
	m_listofbstr.DoModal();
}

BOOL VTSEventBitstringDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  
}

void VTSEventBitstringDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	m_timeDelay.Encode(enc, 0);
	m_bitmask.Encode(enc, 1);	
	BACnetOpeningTag().Encode(enc, 2);
	m_listofbstr.Encode(enc);
	BACnetClosingTag().Encode(enc, 2);
}
void VTSEventBitstringDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength != 0)
	{
	m_timeDelay.Decode(dec);
	m_timeDelay.ctrlNull = false;
	m_bitmask.Decode(dec);
	m_bitmask.ctrlNull = false;
	BACnetOpeningTag().Decode(dec);
	m_listofbstr.Decode(dec);
	BACnetClosingTag().Decode(dec);
	}

}
