// GoToLineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "GoToLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGoToLineDlg dialog


CGoToLineDlg::CGoToLineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGoToLineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGoToLineDlg)
	m_nLineIndex = 1;
	//}}AFX_DATA_INIT
}


void CGoToLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGoToLineDlg)
	DDX_Text(pDX, IDC_EDIT_LINEINDEX, m_nLineIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGoToLineDlg, CDialog)
	//{{AFX_MSG_MAP(CGoToLineDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoToLineDlg message handlers

void CGoToLineDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);

	if(m_nLineIndex > 0)
		CDialog::OnOK();
	else
		AfxMessageBox("Please enter an integer no less than 1!",MB_OK);
}
