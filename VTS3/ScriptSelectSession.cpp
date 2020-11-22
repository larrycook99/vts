// ScriptSelectSession.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "ScriptSelectSession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptSelectSession dialog


ScriptSelectSession::ScriptSelectSession(CWnd* pParent /*=NULL*/)
	: CDialog(ScriptSelectSession::IDD, pParent)
{
	//{{AFX_DATA_INIT(ScriptSelectSession)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void ScriptSelectSession::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ScriptSelectSession)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_DOCLIST, m_DocList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ScriptSelectSession, CDialog)
	//{{AFX_MSG_MAP(ScriptSelectSession)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_DOCLIST, OnItemChangingDocList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptSelectSession message handlers

BOOL ScriptSelectSession::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// initialize the port list
	m_DocList.m_nFlags |= LVS_SINGLESEL;
	m_DocList.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 256 );

	// add items and subitems
	for (int i = 0; i < gDocList.Length(); i++) {
		VTSDocPtr	curDoc = gDocList.Child( i );

		// copy the descriptions into the list
		m_DocList.InsertItem( i, curDoc->GetTitle() );
		m_DocList.SetItemData( i, (DWORD)curDoc );
	}

	// nothing is selected
	m_SelectedDoc = 0;

	// disable the OK button until something is selected
	GetDlgItem( IDOK )->EnableWindow( false );

	// set the focus to the document list control
	GetDlgItem( IDC_DOCLIST )->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ScriptSelectSession::OnItemChangingDocList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// forget messages that dont change the state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// get the item data, which is a pointer to the document
	VTSDocPtr	dp = (VTSDocPtr)m_DocList.GetItemData( pNMListView->iItem );

	// if this is being selected, copy the pointer and enable OK
	if ((pNMListView->uNewState & LVIS_SELECTED) != 0) {
		m_SelectedDoc = dp;
		m_OK.EnableWindow( true );
	} else
	if (dp == m_SelectedDoc) {
		m_SelectedDoc = 0;
		m_OK.EnableWindow( false );
	}

	*pResult = 0;
}
