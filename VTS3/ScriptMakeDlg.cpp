// ScriptMakeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "ScriptMakeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptMakeDlg dialog


CScriptMakeDlg::CScriptMakeDlg(bool fMakeDlg, LPCSTR lpszText, CWnd* pParent /*=NULL*/)
			   : CDialog(fMakeDlg ? CScriptMakeDlg::IDDMAKE : CScriptMakeDlg::IDDCHECK, pParent)
{
	//{{AFX_DATA_INIT(CScriptMakeDlg)
	m_strText = _T("");
	//}}AFX_DATA_INIT

	if ( lpszText != NULL )
		m_strText = lpszText;

	m_nDestroyCode = 0;
	m_fModal = false;
}


void CScriptMakeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptMakeDlg)
	DDX_Text(pDX, IDC_SCRIPTMAKETEXT1, m_strText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptMakeDlg, CDialog)
	//{{AFX_MSG_MAP(CScriptMakeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CScriptMakeDlg::DoModeless()
{
	Create(IDD_SCRIPTMAKE);
	ShowWindow(SW_SHOW);
}


void CScriptMakeDlg::SetText(LPCSTR lpszText)
{
	if ( lpszText != NULL )
	{
		m_strText = lpszText;
		SetDlgItemText(IDC_SCRIPTMAKETEXT1, m_strText);
	}
}


bool CScriptMakeDlg::IsUp()
{
	return m_nDestroyCode == 0;
}


bool CScriptMakeDlg::IsSuccess()
{
	return m_nDestroyCode == IDOK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptMakeDlg message handlers


BOOL CScriptMakeDlg::OnInitDialog() 
{
	CRect	rectWnd, rectDesktop, rectText, rectTextMin;
	
	CDialog::OnInitDialog();
	
	CWnd::GetDesktopWindow()->GetWindowRect(&rectDesktop);
//	AfxGetMainWnd()->GetWindowRect(&rectDesktop);
	GetWindowRect(&rectWnd);
	((CWnd *) GetDlgItem(IDC_SCRIPTMAKETEXT1))->GetClientRect(&rectText);
	rectTextMin = rectText;

	CDC * pdc = GetDC();
	pdc->SelectObject(GetFont());
	pdc->DrawText( m_strText, &rectText, DT_CALCRECT );

	// see to it that the width and height of the text rect is not more than 75% of the screen area...
	if ( rectText.Width() > (int) (rectDesktop.Width() * 75 / 100) )
	{
		// reset new right boundary and recalculate with word wrap on...
		rectText.right = rectText.left + (int) (rectDesktop.Width() * 75 / 100);
		pdc->DrawText( m_strText, &rectText, DT_CALCRECT | DT_WORDBREAK );
	}

	// see to it that the final height of the text isn't arbitrarily greater than 75% of the screen height

	if ( rectText.Height() > (int) (rectDesktop.Height() * 75 / 100) )
		rectText.bottom = rectText.top + (int) (rectDesktop.Height() * 75 / 100);

	if ( rectText.Width() < rectTextMin.Width() )
		rectText.right = rectTextMin.right;

	if ( rectText.Height() < rectTextMin.Height() )
		rectText.bottom = rectTextMin.bottom;

	ReleaseDC(pdc);

	// rectText now contains the proper width and height of the text area... now figure the amount of change for 
	// easy positioning and resizing...

	int nXGrow  = rectText.Width() - rectTextMin.Width();
	int nYGrow  = rectText.Height() - rectTextMin.Height();

	// Resize the text window within the dialog... add some fudge factor because the actual drawing of the text
	// is slightly larger than our calculations... which leads to word wrap...

	((CWnd *) GetDlgItem(IDC_SCRIPTMAKETEXT1))->SetWindowPos( NULL, rectText.left, rectText.top, rectText.Width() + 5, rectText.Height() + 5, SWP_NOMOVE );
	rectWnd.right += nXGrow;
	rectWnd.bottom += nYGrow;

	// now move both of the OK/Cancel buttons...
	SetWindowPos( NULL, (rectDesktop.Width() - rectWnd.Width()) / 2,  (rectDesktop.Height() - rectWnd.Height()) / 2,
					rectWnd.Width(), rectWnd.Height(), 0 );

	// Get new position of window...
	((CWnd *) GetDlgItem(IDOK))->GetWindowRect(&rectText);			// coords of OK button, in screen coords
	ScreenToClient(&rectText);
	((CWnd *) GetDlgItem(IDOK))->SetWindowPos( NULL, rectText.left + nXGrow / 2, rectText.top + nYGrow, 0, 0, SWP_NOSIZE );

	((CWnd *) GetDlgItem(IDCANCEL))->GetWindowRect(&rectText);			// coords of OK button, in screen coords
	ScreenToClient(&rectText);
	((CWnd *) GetDlgItem(IDCANCEL))->SetWindowPos( NULL, rectText.left + nXGrow / 2, rectText.top + nYGrow, 0, 0, SWP_NOSIZE );

	// Now set window Title...
	CString strTitle, strAppTitle;
	GetWindowText(strTitle);
	strAppTitle = AfxGetApp()->m_pszAppName;
	strTitle = strAppTitle + " : " + strTitle;
	SetWindowText(strTitle);
	((CWnd *) GetDlgItem(IDOK))->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScriptMakeDlg::OnCancel() 
{
	m_nDestroyCode = IDCANCEL;

	if ( m_fModal )
		EndDialog(m_nDestroyCode);
	else
		DestroyWindow();
}

void CScriptMakeDlg::OnOK() 
{
	m_nDestroyCode = IDOK;

	if ( m_fModal )
		EndDialog(m_nDestroyCode);
	else
		DestroyWindow();
}

int CScriptMakeDlg::DoModal() 
{
	m_fModal = true;
	return CDialog::DoModal();
}
