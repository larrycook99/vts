// EPICSViewInfoPanel.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "EPICSViewInfoPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewInfoPanel


IMPLEMENT_DYNCREATE(CEPICSViewInfoPanel, CRichEditView)

CEPICSViewInfoPanel::CEPICSViewInfoPanel()
{
}

CEPICSViewInfoPanel::~CEPICSViewInfoPanel()
{
}


BEGIN_MESSAGE_MAP(CEPICSViewInfoPanel, CRichEditView)
	//{{AFX_MSG_MAP(CEPICSViewInfoPanel)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CEPICSViewInfoPanel message handlers


void CEPICSViewInfoPanel::ResetPanel()
{
	SetWindowText("");

	// Set default character / para format
	CRichEditCtrl & e = GetRichEditCtrl();
	
	CHARFORMAT fmt;
	e.GetDefaultCharFormat(fmt);
	fmt.dwMask = CFM_FACE | CFM_SIZE;
	fmt.yHeight = 200;		// 10 pnt
	strcpy(fmt.szFaceName, "Arial");
	e.SetDefaultCharFormat(fmt);

	PARAFORMAT pfmt;
	e.GetParaFormat(pfmt);
	pfmt.dwMask = PFM_OFFSET | PFM_NUMBERING | PFM_ALIGNMENT | PFM_STARTINDENT;
	pfmt.dxOffset = 0;
	pfmt.wAlignment = PFA_LEFT;
	pfmt.dxStartIndent = 0;
	pfmt.wNumbering = 0;
	e.SetParaFormat(pfmt);

	e.SetReadOnly();
	e.EnableWindow();
}


void CEPICSViewInfoPanel::AddText(LPCSTR lpsz)
{
	GetRichEditCtrl().ReplaceSel(lpsz);
}


void CEPICSViewInfoPanel::AddLine(LPCSTR lpsz)
{
	AddText(lpsz);
	AddText("\n");
}



void CEPICSViewInfoPanel::SetCharStyle(int nPoints, COLORREF clr )
{
	CHARFORMAT fmt;
	GetRichEditCtrl().GetSelectionCharFormat(fmt);
	fmt.dwMask = CFM_SIZE | CFM_COLOR;
	fmt.yHeight = (LONG) nPoints * 20;
	if ( clr != 0xFFFFFFFF )
	{
		fmt.crTextColor = clr;
		fmt.dwEffects &= ~CFE_AUTOCOLOR;
	}
	else
	{
		fmt.dwEffects |= CFE_AUTOCOLOR;
		fmt.crTextColor = ::GetSysColor(COLOR_WINDOWTEXT);
	}

	GetRichEditCtrl().SetSelectionCharFormat(fmt);
}


void CEPICSViewInfoPanel::SetParagraphStyle(WORD wNumbering, LONG lIndent, LONG lOffset, WORD wAlignment)
{
	PARAFORMAT pfmt;
	GetRichEditCtrl().GetParaFormat(pfmt);
	pfmt.dwMask = PFM_NUMBERING | PFM_ALIGNMENT | PFM_STARTINDENT | PFM_OFFSET;
	pfmt.dxOffset = 0;
	pfmt.wAlignment = wAlignment;
	pfmt.dxStartIndent = lIndent;
	pfmt.dxOffset = lOffset;
	pfmt.wNumbering = wNumbering;
	GetRichEditCtrl().SetParaFormat(pfmt);
}

//long nStartChar, nEndChar;

//pmyRichEditCtrl->GetSel(nStartChar, nEndChar); 
//pmyRichEditCtrl->SetSel(nEndChar, -1);


/////////////////////////////////////////////////////////////////////////////
// CEPICSViewListPanel

IMPLEMENT_DYNCREATE(CEPICSViewListPanel, CListView)

CEPICSViewListPanel::CEPICSViewListPanel()
{
}

CEPICSViewListPanel::~CEPICSViewListPanel()
{
}


BEGIN_MESSAGE_MAP(CEPICSViewListPanel, CListView)
	//{{AFX_MSG_MAP(CEPICSViewListPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewListPanel drawing

void CEPICSViewListPanel::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewListPanel diagnostics

#ifdef _DEBUG
void CEPICSViewListPanel::AssertValid() const
{
	CListView::AssertValid();
}

void CEPICSViewListPanel::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewListPanel message handlers

int CEPICSViewListPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CListCtrl& m_ElemList=GetListCtrl();
	m_ElemList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	// column hiding not available yet...
	m_ElemList.m_nFlags |= LVS_REPORT;
	return 0;
}


void CEPICSViewListPanel::Reset(void)
{
	GetListCtrl().DeleteAllItems();

	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();

	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
		GetListCtrl().DeleteColumn(0);
}
