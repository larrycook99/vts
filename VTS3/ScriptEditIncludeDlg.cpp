// ScriptEditIncludeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "ScriptEditIncludeDlg.h"
#include "ScriptEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE(CIncludeDoc, CDocument)

BEGIN_MESSAGE_MAP(CIncludeDoc, CDocument)
	//{{AFX_MSG_MAP(CIncludeDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CIncludeDoc::Serialize(CArchive& ar)
{
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}


/////////////////////////////////////////////////////////////////////////////
// CScriptEditIncludeDlg dialog


CScriptEditIncludeDlg::CScriptEditIncludeDlg( LPCSTR lpszFileName, ScriptToken * ptok, LPCSTR lpszErrorMsg, int nLine /* = -1 */, CWnd * pParent /* = NULL */ )
	: CDialog(CScriptEditIncludeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptEditIncludeDlg)
	m_strError = _T("");
	//}}AFX_DATA_INIT

	m_strFileName = lpszFileName;
	m_pEdit = NULL;
	m_pdocinclude = NULL;
	m_ptok = ptok;
	m_strError = lpszErrorMsg;
	m_nLine = nLine;
}


CScriptEditIncludeDlg::~CScriptEditIncludeDlg()
{
	// These guys are already deleted...
//	if ( m_pEdit != NULL )
//		delete m_pEdit;

//	if ( m_pdocinclude != NULL )
//		delete m_pdocinclude;
}


void CScriptEditIncludeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptEditIncludeDlg)
	DDX_Text(pDX, IDC_INCLUDEERROR, m_strError);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptEditIncludeDlg, CDialog)
	//{{AFX_MSG_MAP(CScriptEditIncludeDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CScriptEditIncludeDlg message handlers

BOOL CScriptEditIncludeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Save original size of client
	GetClientRect(&m_rClient);

	// Save original position of error text
	GetDlgItem(IDC_INCLUDEERROR)->GetWindowRect(&m_rText);
	ScreenToClient(&m_rText);

	// Save original position of error text
	GetDlgItem(IDOK)->GetWindowRect(&m_rOK);
	ScreenToClient(&m_rOK);

	// Save original position of error text
	GetDlgItem(IDC_INCLUDEPLACE)->GetWindowRect(&m_rView);
	ScreenToClient(&m_rView);

	m_pdocinclude = new CIncludeDoc();
	CCreateContext	ccontext;
	memset(&ccontext, 0, sizeof(CCreateContext));
	ccontext.m_pCurrentDoc = m_pdocinclude;

	m_pEdit = new ScriptEdit();
	m_pEdit->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, m_rView, this, AFX_IDW_PANE_FIRST, &ccontext);
	m_pEdit->GetEditCtrl().ModifyStyle(0, ES_WANTRETURN);

	if ( !m_pdocinclude->OnOpenDocument(m_strFileName) )
		EndDialog(IDCANCEL);

	m_pEdit->OnInitialUpdate();

	// set the title
	CString strTitle;
	char	szFileNameOnly[200];
	GetWindowText(strTitle);
	::GetFileTitle(m_strFileName, szFileNameOnly, sizeof(szFileNameOnly));
	strTitle += " - ";
	strTitle += szFileNameOnly;
	SetWindowText(strTitle);

	if ( !m_strError.IsEmpty() )
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		((CStatic *) GetDlgItem(IDC_INCLUDEICON))->SetIcon(AfxGetApp()->LoadStandardIcon(IDI_EXCLAMATION));
	}

	if ( m_ptok != NULL )
		m_pEdit->GetEditCtrl().SetSel( (DWORD)m_ptok->tokenOffset, (DWORD)(m_ptok->tokenOffset + m_ptok->tokenLength), 0 );

	m_pEdit->SetFocus();
	if ( m_nLine != -1 )
		((ScriptEdit *) m_pEdit)->GotoLine(m_nLine);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CScriptEditIncludeDlg::OnOK() 
{
	m_pdocinclude->OnSaveDocument(m_strFileName);
	CDialog::OnOK();
}

void CScriptEditIncludeDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	GetDlgItem(IDC_INCLUDEERROR)->MoveWindow(m_rText.left, m_rText.top, cx - (m_rClient.right - m_rText.right) - m_rText.left, m_rText.bottom - m_rText.top);

	// Must repaint here...  for some reason repaint built into MoveWindow doesn't work
	GetDlgItem(IDC_INCLUDEERROR)->Invalidate();

	GetDlgItem(IDOK)->MoveWindow(cx - (m_rClient.right - m_rOK.left), m_rOK.top, m_rOK.right - m_rOK.left, m_rOK.bottom - m_rOK.top);
	m_pEdit->MoveWindow(m_rView.left, m_rView.top, cx - (m_rClient.right - m_rView.right), cy - m_rView.top);
}
