// VTSVendorPropIDDlg.cpp : implementation file
//
// This dialog has bee re-purposed to allow the entry of ANY propertyID as an integer.

#include "stdafx.h"
#include "vts.h"
#include "Propid.h"
#include "VTSVendorPropIDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////
//Added by Zhu Zhenhua 2003-7-22, Input Dlg for Vendor Property


/////////////////////////////////////////////////////////////////////////////
// VTSVendorPropIDDlg dialog


VTSVendorPropIDDlg::VTSVendorPropIDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSVendorPropIDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSVendorPropIDDlg)
	m_nEditPropID = 512;  // first vendor-proprietary propertyID
	//}}AFX_DATA_INIT
	m_PropID = -1;
}


void VTSVendorPropIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// Range is the entire legal range for propertyID
	//{{AFX_DATA_MAP(VTSVendorPropIDDlg)
	DDX_Text(pDX, IDC_VENDOR_PROPID, m_nEditPropID);
	DDV_MinMaxInt(pDX, m_nEditPropID, 0, 4194303);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_STATIC1, m_captionStatic);
}


BEGIN_MESSAGE_MAP(VTSVendorPropIDDlg, CDialog)
	//{{AFX_MSG_MAP(VTSVendorPropIDDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSVendorPropIDDlg message handlers

void VTSVendorPropIDDlg::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData(TRUE))
		return;
	m_PropID = m_nEditPropID;
	CDialog::OnOK();
}

BOOL VTSVendorPropIDDlg::OnInitDialog() 
{	
	if(m_PropID != -1)
	m_nEditPropID = m_PropID;
	CDialog::OnInitDialog();

	// Get the prompt string from the dialog.
	// Use it as a format string to insert the
	// current limit for defined BACnet propertyIDs
	CString fmt, str;
	m_captionStatic.GetWindowText( fmt );
	str.Format( fmt, MAX_PROP_ID-1, MAX_PROP_ID );
	m_captionStatic.SetWindowText( str );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
