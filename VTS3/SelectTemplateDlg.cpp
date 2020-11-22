// SelectTemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "ScriptEdit.h"
#include "SelectTemplateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//=============================================================================
//=============================================================================
SelectTemplateDlg::SelectTemplateDlg(const ScriptTemplateCollection &theCollection, CWnd* pParent)
: CDialog(SelectTemplateDlg::IDD, pParent)
, m_collection(theCollection)
, m_templateIndex(-1)
{
	//{{AFX_DATA_INIT(SelectTemplateDlg)
	//}}AFX_DATA_INIT
}

//=============================================================================
void SelectTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectTemplateDlg)
	DDX_Control(pDX, IDC_LIST1, m_templateList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SelectTemplateDlg, CDialog)
	//{{AFX_MSG_MAP(SelectTemplateDlg)
	ON_LBN_DBLCLK(IDC_LIST1, OnOK)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//=============================================================================
BOOL SelectTemplateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int ix = 0; ix < m_collection.Size(); ix++)
	{
		int jx = m_templateList.AddString( m_collection.TemplateName( ix ) );
		m_templateList.SetItemData( jx, (DWORD)ix );
	}
	
	return TRUE;  
}


//=============================================================================
void SelectTemplateDlg::OnOK() 
{
	int ix = m_templateList.GetCurSel();
	if (ix >= 0)
	{
		m_templateIndex = m_templateList.GetItemData( ix );
	}
	
	CDialog::OnOK();
}

void SelectTemplateDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	CWnd *pOK = GetDlgItem( IDOK );
	CWnd *pCancel = GetDlgItem( IDCANCEL );
	if (m_hWnd && m_templateList.m_hWnd && pOK && pCancel)
	{
		// This code would be much simpler if GetClientRect returned the 
		// client POSITION relative to its containing window as well as the size...

		// Get size of the dialog
		CRect rect;
		GetClientRect(&rect);

		// Get size and position of the list
		CRect rectList;
		m_templateList.GetClientRect( &rectList );

		// Get sizes of the ok and cancel buttons
		CRect rectOK, rectCancel;
		pOK->GetClientRect( &rectOK );
		pCancel->GetClientRect( &rectCancel );

#define DA_BORDER 8

		// Resize list: entire dialog, except for the buttons near the bottom
		m_templateList.MoveWindow( DA_BORDER, DA_BORDER, 
								   rect.Width() - 2*DA_BORDER, 
								   rect.Height() - rectOK.Height() - 3*DA_BORDER );

		// Move the ok and cancel buttons:
		// Just above the bottom, and centered in the dialog
		int okPos = (rect.Width() - rectOK.Width() - rectCancel.Width() - 2*DA_BORDER)/2;
		pOK->MoveWindow( okPos,
						 rect.Height() - rectOK.Height() - DA_BORDER, 
						 rectOK.Width(), rectOK.Height() );
		pCancel->MoveWindow( okPos + rectOK.Width() + 2*DA_BORDER,
							 rect.Height() - rectCancel.Height() - DA_BORDER, 
							 rectCancel.Width(), rectCancel.Height() );
	}
}
