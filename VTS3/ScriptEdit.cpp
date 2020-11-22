//******************************************************************
// 	Author:			Yajun Zhou
//	Date:			2002-4-22
//	Description:	Accept the message of mouse and keyboard of
//					the edit area; Get the current line index; 
//					Move the caret to the specified line; Highlight
//					the current line.
//	Modify Log:		1)	Modified by Yajun Zhou, 2002-4-27 
//						Add a ctrl to display the line number.
//					2)	Modified by Yajun Zhou, 2002-5-8
//						Add a new feature to highlight the current 
//						line;
//						Fix BUG_1 that couldn't display the correct
//						line number on the statusbar when the current
//						line is the first visible line;
//					3)	Modified by Yajun Zhou, 2002-5-10
//						Fix BUG_2 that display a wrong index in the
//						statusbar and in the LineNumCtrl when typing
//						ENTER;
//					4)	Modified by Yajun Zhou, 2002-5-10
//						Reduce flicker of LineNumCtrl when refresh.
//					5)	Modified by Yajun Zhou, 2002-5-22
//						Reduce flicker when refresh and fix some bugs
//						that highlight a wrong line.
//					6)	Modified by Yajun Zhou, 2002-6-20
//						Delete the method, OnKillfocus().  So that when
//						the EditView lost its focus the window on the 
//						statusbar display current line number.
//					7)	Modified by Yajun Zhou, 2002-6-24
//						Add a method, SetDefaultFont(), to change the 
//						default font. 
//					8)	Modified by Yajun Zhou, 2002-7-8
//						Fixed a bug that display wrong line numbers in 
//						the LineNumCtrl when mouse wheeling. 
//					9)	Modified by Yajun Zhou, 2002-7-12
//						Fixed a bug that sometimes when the current line 
//						is the last visible line, it doesn't be highlighted.
//					10)	Modified by Yajun Zhou, 2002-8-5
//						Modified the function, DisplayLnNum(), so that it 
//						could display numbers larger than 9999 correctly.
//					11)	Modified by Yajun Zhou, 2002-9-1
//						Fixed a bug that the LineNumCtrl's background is 
//						white and could not work when horizontally scrolling
//						under windows98.
//					12)	Modified by Yajun Zhou, 2003-5-19
//						Used memDC to reduce flicker in function OnPaint() and 
//						deleted some unused functions.
//					13)	Modified by Yajun Zhou, 2003-5-20
//						Added handlers of cut, paste, undo and contextmenu.
//******************************************************************
// ScriptEdit.cpp : implementation file
//
#include "stdafx.h"
#include "VTS.h"
#include "ScriptEdit.h"
#include "ScriptFrame.h"
#include "GoToLineDlg.h"
#include "StringTables.h"
#include "SelectTemplateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MARGIN_3			25
#define CHAR_HEIGHT			16
#define LIMITTEXT			0xFFFFFFFF

static const char s_registrySection[] = "ScriptEditor";
static const char s_registryKeyTemplate[] = "TemplateFile";
static const char s_registryKeyAutoObject[] = "AutoObject";
static const char s_registryKeyAutoProperty[] = "AutoProperty";
static const char s_registryKeyTabWidth[] = "TabWidth";


/////////////////////////////////////////////////////////////////////////////
// ScriptEdit

IMPLEMENT_DYNCREATE(ScriptEdit, CEditView)

// static 
bool				ScriptEdit::s_templateFileLoaded;
ScriptTemplateLibrary	ScriptEdit::s_templateLibrary;


ScriptEdit::ScriptEdit()
{
	m_pEdit	= &GetEditCtrl();

	m_nMargin		= MARGIN_3;
	m_nClientX		= 0;
	m_nClientY		= 0;
	m_ptCurPoint.x	= MARGIN_3;
	m_ptCurPoint.y	= 0;

	m_pframe = NULL; 
	
	// Default template document
	((VTSApp*)AfxGetApp())->GetRelativeToExe( m_templateFileName, "scripts\\templates.txt" );

	// I find these damned annoying, so changed the default to OFF.
	// The insert menu is more managable
	m_autoObjectType = 0;
	m_autoPropertyID = 0;

	m_tabWidth = 3;
}

ScriptEdit::~ScriptEdit()
{
}


BEGIN_MESSAGE_MAP(ScriptEdit, CEditView)
	//{{AFX_MSG_MAP(ScriptEdit)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_COMMAND(IDC_EDIT_GOTOLINE, OnEditGotoline)
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_HSCROLL, OnHscroll)
	ON_WM_VSCROLL()
	ON_CONTROL_REFLECT(EN_VSCROLL, OnVscroll)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditClear)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_REPEAT, OnEditRepeat)
	ON_COMMAND(ID_SET_TEMPLATE_FILE, OnSetTemplateFile)
	ON_COMMAND(ID_EDIT_AUTO_OBJECT, OnEditAutoObject)
	ON_UPDATE_COMMAND_UI(ID_EDIT_AUTO_OBJECT, OnUpdateEditAutoObject)
	ON_COMMAND(ID_EDIT_AUTO_PROPERTY, OnEditAutoProperty)
	ON_UPDATE_COMMAND_UI(ID_EDIT_AUTO_PROPERTY, OnUpdateEditAutoProperty)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE( ID_SCRIPT_TEMPLATE_0, ID_SCRIPT_TEMPLATE_LAST, OnTemplate )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptEdit drawing

void ScriptEdit::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// ScriptEdit diagnostics

#ifdef _DEBUG
void ScriptEdit::AssertValid() const
{
	CEditView::AssertValid();
}

void ScriptEdit::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ScriptEdit message handlers
int ScriptEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetDefaultFont();
	// TODO: Add your specialized creation code here
	DWORD dwStyle = ES_MULTILINE | ES_RIGHT
					| WS_CHILD | WS_VISIBLE | WS_TABSTOP ;

	if(!m_LineNumCtrl.Create(dwStyle,
		CRect(0, 0, 0, 0), &GetEditCtrl(), IDC_LINENUMBER))
	{
		TRACE0("Failed to create LineNumCtrl\n");
		return -1;      // fail to create
	}

	m_LineNumCtrl.SetDefaultFont();
	m_LineNumCtrl.SetReadOnly(true);
	m_LineNumCtrl.EnableWindow(false);


	m_pEdit->SetLimitText(LIMITTEXT);

	return 0;
}

void ScriptEdit::OnSize(UINT nType, int cx, int cy) 
{
	CEditView::OnSize(nType, cx, cy);

	m_nClientX = cx;
	m_nClientY = cy;

	if (::IsWindow(m_LineNumCtrl.m_hWnd))
		m_LineNumCtrl.MoveWindow(1, 1, m_nMargin, m_nClientY, TRUE);

	m_nVisibleLnCount = cy/CHAR_HEIGHT;
	
	UpdateEditArea();

//	TRACE("cx = %d, cy = %d\n", cx,cy);
}

HBRUSH ScriptEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if(nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetBkMode(TRANSPARENT); 
		return HBRUSH(GetStockObject(HOLLOW_BRUSH));
	}
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}

BOOL ScriptEdit::OnEraseBkgnd(CDC* pDC) 
{
 	return FALSE;
}

void ScriptEdit::OnInitialUpdate() 
{
	CEditView::OnInitialUpdate();
	
	m_pEdit->SetMargins(m_nMargin+1, 0);
	
	m_nFirstVisibleLn = 0;
	m_nCurrentLine = 0;
	m_nTempDigit = 3;
	m_nLineCount = m_pEdit->GetLineCount();

	FillAutoType();
	FillInsertMenu();

	m_tabWidth = AfxGetApp()->GetProfileInt( s_registrySection, s_registryKeyTabWidth, m_tabWidth );
}

void ScriptEdit::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rcClient;
    GetClientRect( &rcClient );
	
	CDC memDC;
    CBitmap bitmap;
    memDC.CreateCompatibleDC(&dc);
    bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	
	m_nFirstVisibleLn = m_pEdit->GetFirstVisibleLine();
	int nCurrentY;
	nCurrentY = (m_nCurrentLine - m_nFirstVisibleLn) * CHAR_HEIGHT;
//	TRACE("m_nCurrentLine = %d, m_nFirstVisibleLn = %d, nCurrentY = %d\n",m_nCurrentLine, m_nFirstVisibleLn, nCurrentY);
	
	memDC.FillSolidRect( rcClient, RGB(255,255,255) );
	
	if(nCurrentY+CHAR_HEIGHT-1 < m_nClientY)
	{
		//madanner 6/03, heap allocation not necessary
		CRect RLine(m_nMargin, nCurrentY, m_nClientX, nCurrentY + CHAR_HEIGHT);
		memDC.FillSolidRect( &RLine, RGB(255,255,0) );
//		CRect *pRLine = new CRect(m_nMargin, nCurrentY, m_nClientX, nCurrentY + CHAR_HEIGHT);
//		memDC.FillSolidRect( pRLine, RGB(255,255,0) );
//		delete pRLine;
	}
	
    CWnd::DefWindowProc(WM_PAINT,(WPARAM)memDC.m_hDC, 0 );

	DisplayLnNum();
	
    dc.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &memDC, 0,0, SRCCOPY);
    memDC.SelectObject(pOldBitmap);
}

void ScriptEdit::OnEditGotoline() 
{
	int nLine;
	CGoToLineDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		nLine = dlg.m_nLineIndex;
		GotoLine(nLine);
	}	
}

void ScriptEdit::OnSetfocus() 
{
	CString str;
	str.Format(" Ln: %d",m_nCurrentLine+1);

//madanner 5/03	CMainFrame* pFrm = (CMainFrame*) AfxGetMainWnd();
//	pFrm->SetLnPaneText(str);	
	if ( m_pframe != NULL )
		m_pframe->SetLnPaneText(str);
}

void ScriptEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_ptCurPoint = m_pEdit->GetCaretPos();

	CEditView::OnLButtonDown(nFlags, point);

	UpdateEditArea();
	GetCurLineIndex();
}

void ScriptEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CEditView::OnLButtonUp(nFlags, point);

	UpdateEditArea();
	GetCurLineIndex();
}

void ScriptEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == VK_UP || nChar == VK_DOWN )
		ScrollCurLnVisible(nChar);

	CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
	
	UpdateEditArea();
	GetCurLineIndex();
}

void ScriptEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// If nChar is TAB or Shift-TAB, and one or more entire lines are selected,
	// indent or undent the selection, and DON'T call CEditView::OnChar
	if (nChar == '\t')
	{
		CString tab( ' ', m_tabWidth );

		int startChar, endChar;
		m_pEdit->GetSel( startChar, endChar );
		int startLine = m_pEdit->LineFromChar( startChar );
		int	prevLine  = m_pEdit->LineFromChar( startChar - 1 );
		int endLine   = m_pEdit->LineFromChar( endChar );
		int endPrevLine = m_pEdit->LineFromChar( endChar - 1 );
		if ((startLine != endLine) &&							// at least one line selected
			((startChar == 0) || (startLine != prevLine)) &&	// begin at start of a line
			(endLine != endPrevLine))							// end at start of a line
		{
			bool shift = GetKeyState( VK_SHIFT ) < 0;
			CString newText, str;
			for (int ix = startLine; ix < endLine; ix++)
			{
				int len = m_pEdit->GetLine( ix, str.GetBuffer(1000), 1000 );
				str.ReleaseBuffer( len );
				if (shift)
				{
					if (str.Find( tab ) == 0)
					{
						str = str.Mid( m_tabWidth );
					}
					else if (str.Find( '\t' ) == 0)
					{
						str = str.Mid( 1 );
					}
				}
				else
				{
					newText += tab;
				}
				newText += str + "\r\n";
			}
			m_pEdit->ReplaceSel( newText, TRUE );
			m_pEdit->SetSel( startChar, startChar + newText.GetLength() );
		}
		else
		{
			// Just insert a tab's worth of spaces
			m_pEdit->ReplaceSel( tab, TRUE );
		}
		UpdateEditArea();
		GetCurLineIndex();
		return;
	}

	CEditView::OnChar(nChar, nRepCnt, nFlags);
	if((nChar >= 'A' && nChar <= 'Z') || (nChar >= 'a' && nChar <= 'z') || (nChar == '_' || nChar == '-'))
	{
		OnHelpInput(nChar, nRepCnt, nFlags);
//Added by Zhu Zhenhua, 2003-12-25, to help tester in inputing 
	}
		
	if(nChar == VK_RETURN)
	{
		UpdateEditArea();
		int curLine = GetCurLineIndex();

		// Auto-indent to the level of the previous line
		if (curLine > 0)
		{
			CString prevLine;
			int max = m_pEdit->GetLine( curLine-1, prevLine.GetBuffer(100), 100 );
			prevLine.ReleaseBuffer(max);
			if (max > 0)
			{
				CString white = prevLine.SpanIncluding( " \t" );
				if (!white.IsEmpty())
				{
					m_pEdit->ReplaceSel( white, TRUE );
				}
			}
		}
	}
}

void ScriptEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CEditView::OnVScroll(nSBCode, nPos, pScrollBar);

	UpdateEditArea();
}

void ScriptEdit::OnVscroll() 
{
	UpdateEditArea();
}

void ScriptEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CEditView::OnHScroll(nSBCode, nPos, pScrollBar);

	UpdateEditArea();
}

void ScriptEdit::OnHscroll() 
{
	UpdateEditArea();
}

void ScriptEdit::OnEditCut() 
{
	CEditView::OnEditCut();
	GetCurLineIndex();
	UpdateEditArea();	
}

void ScriptEdit::OnEditClear() 
{
	CEditView::OnEditClear();
	GetCurLineIndex();
	UpdateEditArea();	
}

void ScriptEdit::OnEditPaste() 
{
	CEditView::OnEditPaste();
	GetCurLineIndex();
	UpdateEditArea();	
}

void ScriptEdit::OnEditUndo() 
{
	CEditView::OnEditUndo();
	GetCurLineIndex();
	UpdateEditArea();	
}

// If Findtext were virtual, we could just override that.
// But it ain't...
void ScriptEdit::OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase)
{
	// Do the hard work
	CEditView::OnFindNext(lpszFind, bNext, bCase);

	// Use GotoLine to move the highlight and update the status line
	int start, end;
	GetEditCtrl().GetSel( start, end );
	GotoLine( GetEditCtrl().LineFromChar( start ) + 1 );

	// Re-select the search target, or find-again won't move
	GetEditCtrl().SetSel( start, end );
}

void ScriptEdit::OnEditRepeat() 
{
	CEditView::OnEditRepeat();

	// Use GotoLine to move the highlight and update the status line
	int start, end;
	GetEditCtrl().GetSel( start, end );
	GotoLine( GetEditCtrl().LineFromChar( start ) + 1 );

	// Re-select the search target, or find-again won't move
	GetEditCtrl().SetSel( start, end );
}

void ScriptEdit::OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, LPCTSTR lpszReplace )
{
	// Do the hard work
	CEditView::OnReplaceSel(lpszFind, bNext, bCase, lpszReplace );

	// Use GotoLine to move the highlight and update the status line
	int start, end;
	GetEditCtrl().GetSel( start, end );
	GotoLine( GetEditCtrl().LineFromChar( start ) + 1 );

	// Re-select the search target, or find-again won't move
	GetEditCtrl().SetSel( start, end );
}

void ScriptEdit::OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase )
{
	// Do the hard work
	CEditView::OnReplaceAll( lpszFind, lpszReplace, bCase );

	// Use GotoLine to move the highlight and update the status line
	int start, end;
	GetEditCtrl().GetSel( start, end );
	GotoLine( GetEditCtrl().LineFromChar( start ) + 1 );

	// Re-select the search target, or find-again won't move
	GetEditCtrl().SetSel( start, end );
}

void ScriptEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
//	CEditView::OnContextMenu(pWnd, point);
	CMenu menu;
	if (menu.LoadMenu( IDR_SCRIPT_CONTEXT ))
	{
		CMenu* pPopup = menu.GetSubMenu(0);
		s_templateLibrary.FillMenu( *pPopup, true );

		pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
								point.x, point.y,
								GetParentFrame() );
	}

	GetCurLineIndex();
	UpdateEditArea();		
}

//******************************************************************
//	Author:		Yajun Zhou
//	Date:		2002-4-22
//	Purpose:	Get the current line index
//	In:			void
//	Out:		void
//******************************************************************
int ScriptEdit::GetCurLineIndex()
{
	m_ptCurPoint = m_pEdit->GetCaretPos();
	
	int n = m_pEdit->CharFromPos(m_ptCurPoint);
	m_nCurrentLine = HIWORD(n);

	m_nFirstVisibleLn = m_pEdit->GetFirstVisibleLine();

	int nLineIndex = m_nCurrentLine + 1;

	//Start for BUG_1
	if(nLineIndex == m_nFirstVisibleLn)
	{
		nLineIndex++;
		m_nCurrentLine++;
	}
	//End for BUG_1

	if ( m_pframe != NULL )
	{
		CString str;
		str.Format(" Ln: %d",nLineIndex);
		m_pframe->SetLnPaneText(str);
	}

	return m_nCurrentLine;
}

//******************************************************************
//	Author:		Yajun Zhou
//	Date:		2002-4-22
//	Purpose:	Go to the specified line
//	In:			int nLineIndex: The specified line index
//	Out:		void
//******************************************************************
void ScriptEdit::GotoLine(int nLineIndex)
{
	if(nLineIndex > m_nLineCount)
			nLineIndex = m_nLineCount;

	int nCharIndex = m_pEdit->LineIndex(nLineIndex-1);

	//madanner 6/03, make goto line visible... wouldn't do this if goto was upward
	m_pEdit->LineScroll((nLineIndex-1) - m_pEdit->GetFirstVisibleLine());
//	m_pEdit->LineScroll(m_nLineCount-m_nCurrentLine);
	m_pEdit->SetSel(nCharIndex,nCharIndex,false);
	
	//madanner 6/03, Calling GetCurLineIndex() without setting the caret highlights the wrong
	//line... must set caret too
	m_pEdit->SetCaretPos(m_pEdit->PosFromChar(nCharIndex));

	UpdateEditArea();
	GetCurLineIndex();
}

//******************************************************************
//	Author:		Yajun Zhou
//	Date:		2002-4-27
//	Purpose:	Display line number in the CLineNumCtrl control.
//	In:			void
//	Out:		void
//******************************************************************
void ScriptEdit::DisplayLnNum()
{
	m_nLineCount = m_pEdit->GetLineCount();

	int nFirstVisibleLn = m_pEdit->GetFirstVisibleLine();
	int nLastVisibleLn = nFirstVisibleLn + m_nVisibleLnCount;

	m_LineNumCtrl.m_nLineCount = m_nLineCount;
	
	char buf[10];
	_itoa(nLastVisibleLn, buf, 10);
	CString strLineNum = buf;
	int nDigit = strLineNum.GetLength();
	
	if(nDigit > 3 && nDigit != m_nTempDigit)
	{
		m_nTempDigit = nDigit;
		m_nMargin = MARGIN_3 + (nDigit - 3) * 7;
		m_pEdit->SetMargins(m_nMargin+1, 0);
		if (::IsWindow(m_LineNumCtrl.m_hWnd))
			m_LineNumCtrl.MoveWindow(1, 1, m_nMargin, m_nClientY, TRUE);

	}

	if(nDigit <= 3 && nDigit != m_nTempDigit)
	{
		m_nTempDigit = nDigit;
		m_nMargin = MARGIN_3;
		m_pEdit->SetMargins(m_nMargin+1, 0);
		if (::IsWindow(m_LineNumCtrl.m_hWnd))
			m_LineNumCtrl.MoveWindow(1, 1, m_nMargin, m_nClientY, TRUE);

	}

	m_LineNumCtrl.Display(nFirstVisibleLn+1, nLastVisibleLn);
}

//******************************************************************
//	Author:		Yajun Zhou
//	Date:		2002-5-10
//	Purpose:	Refresh the edit area.
//	In:			void
//	Out:		void
//******************************************************************
void ScriptEdit::UpdateEditArea()
{
	//madanner 6/03, heap allocation not necessary
	CRect Rect(m_nMargin, 0, m_nClientX, m_nClientY);
	InvalidateRect(&Rect);

//	CRect* pRect = new CRect(m_nMargin, 0, m_nClientX, m_nClientY);
//	InvalidateRect(pRect);
//	delete pRect;
}

//******************************************************************
//	Author:		Yajun Zhou
//	Date:		2002-5-21
//	Purpose:	Judge whether the current line is visible or not.
//	In:			void
//	Out:		TRUE:	The current line is visible;
//				FALSE:	The current line is invisible;
//******************************************************************
BOOL ScriptEdit::IsCurLnVisible()
{
	if(m_nCurrentLine < m_nFirstVisibleLn ||
		m_nCurrentLine > (m_nFirstVisibleLn+m_nVisibleLnCount-1))
		return FALSE;
	else
		return TRUE;
}

//******************************************************************
//	Author:		Yajun Zhou
//	Date:		2002-5-22
//	Purpose:	When user type "Up" or "Down" and the current line is
//				invisible, it should scroll to be visible.
//	In:			void
//	Out:		void
//******************************************************************
void ScriptEdit::ScrollCurLnVisible(UINT nChar)
{
	if(m_nCurrentLine < m_nFirstVisibleLn)
	{
		if(nChar == VK_UP)
			m_pEdit->LineScroll(m_nCurrentLine-m_nFirstVisibleLn);
		if(nChar == VK_DOWN)
			m_pEdit->LineScroll(m_nCurrentLine-m_nFirstVisibleLn+1);
	}
	if(m_nCurrentLine > (m_nFirstVisibleLn+m_nVisibleLnCount-1))
	{
		if(nChar == VK_UP)
			m_pEdit->LineScroll(m_nCurrentLine-(m_nFirstVisibleLn+m_nVisibleLnCount));
		if(nChar == VK_DOWN)
			m_pEdit->LineScroll(m_nCurrentLine-(m_nFirstVisibleLn+m_nVisibleLnCount)+1);
	}
}

//******************************************************************
//	Author:		Yajun Zhou
//	Date:		2002-6-24
//	Purpose:	Set font.
//	In:			void
//	Out:		void
//******************************************************************
void ScriptEdit::SetDefaultFont()
{
	// instantiate a CFont pointer
	// TODO: this leaks at exit...
    CFont* pFont  = new CFont();
    	
   	LOGFONT lf ;
   	memset(&lf, 0, sizeof(LOGFONT));       
    
   	lf.lfHeight = 16;
   	lf.lfWeight = 0;
   	lf.lfEscapement = 0;
   	lf.lfOrientation = 0;
   	lf.lfWeight = FW_NORMAL;
   	lf.lfItalic = FALSE;
   	lf.lfUnderline = FALSE;
   	lf.lfStrikeOut = 0;
   	lf.lfCharSet = ANSI_CHARSET;
   	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
   	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
   	lf.lfQuality = DEFAULT_QUALITY;
   	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
   	strcpy(lf.lfFaceName, "Courier New");        
   		
    // give it a font using the LOGFONT structure
    pFont->CreateFontIndirect(&lf);
    
	// set the window's font, and force a repaint
    SetFont(pFont, TRUE); 
}

//******************************************************************
//	Author:		Yajun Zhou
//	Date:		2002-11-1
//	Purpose:	Set the text of the specified line.
//	In:			int nLineIndex: The specified line index
//				LPTSTR lpszString: The text of new line
//	Out:		void
//******************************************************************
void ScriptEdit::SetLine(int nLineIndex, LPTSTR lpszString)
{
	int nChar;

	ASSERT(nLineIndex <= m_pEdit->GetLineCount() && nLineIndex >= 0);

	if(nLineIndex < m_pEdit->GetLineCount())
	{
		nChar = m_pEdit->LineIndex(nLineIndex);
		m_pEdit->SetSel(nChar, nChar);
		m_pEdit->ReplaceSel(lpszString);
	}
	else if(nLineIndex == m_pEdit->GetLineCount())
	{
		CString strBuffer;
		m_pEdit->GetLine(nLineIndex-1, strBuffer.GetBuffer(0));
		nChar = m_pEdit->LineIndex(nLineIndex-1) + strBuffer.GetLength();
		
		CString strLineFeed = "\r\n";
		m_pEdit->SetSel(nChar, nChar);
		m_pEdit->ReplaceSel(strLineFeed.GetBuffer(3));

		nChar = m_pEdit->LineIndex(nLineIndex);
		m_pEdit->SetSel(nChar, nChar);
		m_pEdit->ReplaceSel(lpszString);
	}		
}

//Added by Zhu Zhenhua, 2003-12-25
// Add help string in stringList 
bool ScriptEdit::AddInputHelpString(CString sString)
{
	bool bRet;		
	int f;					// MAG 11AUG05 add this line, remove local declaration below since f is used out of that scope
	if( m_strList.IsEmpty() ) {
		// if list is empty add first string
		try {
			m_strList.AddHead( sString );
			bRet = true;
		} catch( CMemoryException *e ) {
			e->Delete();
			bRet = false;
		}
	} else {
		if( !m_strList.Find( sString ) ) {
			// insert into sorted list searching for a valid position
			int nCount = m_strList.GetCount();
			POSITION pos = m_strList.GetHeadPosition();
			for( f=0; f<nCount; f++ ) {
				// ascending order
				if( sString < m_strList.GetAt( pos ) ) {
					break;
				} else {
					m_strList.GetNext( pos );
				}
			}
			try {
				if( f == nCount )
					m_strList.AddTail( sString );
				else
					m_strList.InsertBefore( pos, sString );
				bRet = true;
			} catch( CMemoryException *e ) {
				e->Delete();
				bRet = false;
			}
		} else
			bRet = true;
	}

	return bRet;
}


//Added by Zhu Zhenhua, 2003-12-25, to help tester in inputing 
void ScriptEdit::OnHelpInput(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int x;					// MAG 11AUG05 add this line, remove local declaration below since x is used out of that scope
	POSITION pos;
	CString sBuffer,sWord, sBufferLine;
	char pBuffer[1024];
	int nLen, nBegin, nstar, nOver, nEnd,nLastSpace, nWordLen;
	int nCurlin = GetCurLineIndex(); 
	int ncount = m_pEdit->LineLength(nCurlin);
	nBegin=m_pEdit->LineIndex(nCurlin);
	nEnd = nBegin + m_pEdit->LineLength(nCurlin);
	int len = m_pEdit->GetLine(nCurlin, pBuffer, 1024);
	pBuffer[len] = 0;
	sBufferLine = pBuffer;
//	delete pBuffer;
	
	m_pEdit->GetSel(nstar,nOver);
	sBuffer = sBufferLine.Left((nstar - nBegin));
	nLen = sBuffer.GetLength();
	for( x=sBuffer.GetLength(); x>0; x-- )
		if( IsSpace( sBuffer.GetAt(x-1)))
			break;
	nLastSpace = x-1;
	if( nLastSpace >= 0 )
		sWord = sBuffer.Right( nLen - nLastSpace - 1 );
	else
		sWord = sBuffer;
	nWordLen = sWord.GetLength();
	if( !sWord.IsEmpty() ) 
	{
		for( int f=0; f<m_strList.GetCount(); f++ ) 
		{
			pos = m_strList.FindIndex( f );
			if( sWord == m_strList.GetAt( pos ).Left( nWordLen )) 
			{
				sWord = m_strList.GetAt( pos ).Right( m_strList.GetAt( pos ).GetLength() - nWordLen );
				m_pEdit->SetSel(nstar, nstar);
				m_pEdit->ReplaceSel(sWord.GetBuffer(0));
				m_pEdit->SetSel(nstar, (nstar+sWord.GetLength()) );
				break;
			}
		}
	}
}

void ScriptEdit::OnEditAutoObject() 
{
	m_autoObjectType = (m_autoObjectType == 0);
	AfxGetApp()->WriteProfileInt( s_registrySection, s_registryKeyAutoObject, m_autoObjectType );
	FillAutoType();
}

void ScriptEdit::OnUpdateEditAutoObject(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_autoObjectType );
}

void ScriptEdit::OnEditAutoProperty() 
{
	m_autoPropertyID = (m_autoPropertyID == 0);
	AfxGetApp()->WriteProfileInt( s_registrySection, s_registryKeyAutoProperty, m_autoPropertyID );
	FillAutoType();
}

void ScriptEdit::OnUpdateEditAutoProperty(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_autoPropertyID );
}

void ScriptEdit::OnSetTemplateFile() 
{
	// Don't change the current directory: template file shouldn't affect location of anything else
	CFileDialog dlg( TRUE, "*.txt", (LPCTSTR)m_templateFileName, 
					 OFN_NOCHANGEDIR, 
					 "Template Text Files (*.txt)|*.txt|All Files (*.*)|*.*||" );
	if (dlg.DoModal() == IDOK)
	{
		m_templateFileName = dlg.GetPathName();
		AfxGetApp()->WriteProfileString( s_registrySection, s_registryKeyTemplate, m_templateFileName );

		// Load the new file
		s_templateFileLoaded = false;
		FillInsertMenu();
	}
}

void ScriptEdit::FillAutoType()
{
	CString strprop;
	int i;
	m_autoPropertyID = AfxGetApp()->GetProfileInt( s_registrySection, s_registryKeyAutoProperty, m_autoPropertyID );
	if (m_autoPropertyID)
	{
		for(i = 0; i < NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_nStrings; i++)
		{	
			strprop = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_pStrings[i];
			AddInputHelpString(strprop);
		}
	}

	m_autoObjectType = AfxGetApp()->GetProfileInt( s_registrySection, s_registryKeyAutoObject, m_autoObjectType );
	if (m_autoObjectType)
	{
		for(i = 0; i < NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nStrings; i++)
		{
			strprop = NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_pStrings[i];
			AddInputHelpString(strprop);
		}
	}
}

void ScriptEdit::FillInsertMenu()
{
	m_templateFileName = AfxGetApp()->GetProfileString( s_registrySection, s_registryKeyTemplate, m_templateFileName );
	if (!s_templateFileLoaded)
	{
		s_templateLibrary.ReadFile( m_templateFileName );
		s_templateFileLoaded = true;
	}

	// Find the "Insert" menu and populate it
	// If an error is detected in an INCLUDE, this dialog will be invoked as a CScriptEditIncludeDlg
	// without a parent frame or menu.
	CFrameWnd *pFrame = GetParentFrame();
	if (pFrame)
	{
		CMenu *pMenu = pFrame->GetMenu();
		if (pMenu)
		{
			CString str;
			for (int ix = 0; ix < 20; ix++)
			{
				pMenu->GetMenuString( ix, str, MF_BYPOSITION );
				if (str == "&Insert")
				{
					s_templateLibrary.FillMenu( *pMenu->GetSubMenu( ix ), false );
					break;
				}
			}
		}
	}
}

void ScriptEdit::OnTemplate(UINT nID)
{
	UINT ix = nID - ID_SCRIPT_TEMPLATE_0;
	SelectTemplateDlg dlg( s_templateLibrary.Collection( ix ) );
	if (dlg.DoModal() == IDOK)
	{
		// Insert the text for the selected template
		int sel = dlg.GetTemplateIndex();
		if (sel >= 0)
		{
			// Auto-indent to the level of the current line
			CString white;
//			UpdateEditArea();
			int curLine = GetCurLineIndex();
			if (curLine >= 0)
			{
				CString lineText;
				int max = m_pEdit->GetLine( curLine, lineText.GetBuffer(100), 100 );
				lineText.ReleaseBuffer(max);
				if (max > 0)
				{
					white = lineText.SpanIncluding( " \t" );
				}
			}

			CString text;
			LPCTSTR pText = s_templateLibrary.Collection( ix ).TemplateBody( sel );
			while (*pText)
			{
				TCHAR ch = *pText++;
				text += ch;
				if ((ch == '\n') && !white.IsEmpty())
				{
					text += white;
				}
			}

			m_pEdit->ReplaceSel( text );
		}
	}
}

//=============================================================================
//=============================================================================
// Set of templates
ScriptTemplateCollection::ScriptTemplateCollection( const char *pTheTitle )
: m_title(pTheTitle)
{
}

//=============================================================================
ScriptTemplateCollection::~ScriptTemplateCollection()
{
}

void ScriptTemplateCollection::Add( ScriptTemplate &theTemplate )
{
	m_templates.Add( theTemplate );
};

//=============================================================================
//=============================================================================
// Group of template collections
ScriptTemplateLibrary::ScriptTemplateLibrary()
{
}

//=============================================================================
ScriptTemplateLibrary::~ScriptTemplateLibrary()
{
	RemoveAll();
}

//=============================================================================
// Load the templatefile
void ScriptTemplateLibrary::ReadFile( const char *pTheFileName )
{
	// Delete old contents
	RemoveAll();

	CString str;
	ScriptTemplateCollection *pCollection = NULL;
	ScriptTemplate templ;
	bool defining = false;

	// Create sections from the propertyID and object-type enumerations
	pCollection = new ScriptTemplateCollection( "Property Identifiers" );
	int ix;
	for (ix = 0; ix < NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_nStrings; ix++)
	{
		templ.m_name = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_pStrings[ix];
		templ.m_body = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_pStrings[ix];
		pCollection->Add( templ );
	}
	m_collections.Add( pCollection );

	pCollection = new ScriptTemplateCollection( "Object Types" );
	for (ix = 0; ix < NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_nStrings; ix++)
	{
		templ.m_name = NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_pStrings[ix];
		templ.m_body = NetworkSniffer::BAC_STRTAB_BACnetObjectType.m_pStrings[ix];
		pCollection->Add( templ );
	}
	m_collections.Add( pCollection );
	
	pCollection = NULL;

	// Read the file
	CStdioFile templateFile;
	if (templateFile.Open( pTheFileName, CFile::modeRead ))
	{
		CString line;
		int lineNumber = 0;
		while (templateFile.ReadString( line ))
		{
			lineNumber++;
			if (defining)
			{
				// Line of a definition
				if (line.Left(7).Compare( "ENDITEM" ) == 0)
				{
					defining = false;
					
					// Add the template 
					pCollection->Add( templ );
				}
				else
				{
					// Append to definition
					if (!templ.m_body.IsEmpty())
					{
						// non-initial line.
						templ.m_body += "\r\n";
					}
					templ.m_body += line;
				}
			}
			else
			{
				if (line.Left(5).Compare( "ITEM " ) == 0)
				{
					// Begin defining a template
					templ.m_name = line.Mid( 5 );
					templ.m_name.TrimLeft();
					if (templ.m_name.IsEmpty())
					{
						str.Format( "Template without a name on line %i of template file %s", 
									lineNumber, pTheFileName );
						AfxMessageBox( str, MB_OK | MB_ICONERROR );
						break;
					}
					
					if (pCollection == NULL)
					{
						str.Format( "Template %s defined outside a section on line %i of template file %s", 
									(LPCTSTR)templ.m_name, lineNumber, pTheFileName );
						AfxMessageBox( str, MB_OK | MB_ICONERROR );
						break;
					}
					
					templ.m_body.Empty();
					defining = true;
				}
				else if (line.Left(8).Compare( "SECTION " ) == 0)
				{
					// Add the previous collection to the library
					if (pCollection != NULL)
					{
						m_collections.Add( pCollection );
					}
					
					str = line.Mid( 8 );
					str.TrimLeft();
					if (str.IsEmpty())
					{
						str.Format( "Section without a name on line %i of template file %s", 
									lineNumber, pTheFileName );
						AfxMessageBox( str, MB_OK | MB_ICONERROR );
						break;
					}
					
					// Begin defining a collection of templates
					pCollection = new ScriptTemplateCollection( str );
				}
				else if ((line.GetLength() == 0) || (line[0] == ';'))
				{
					// Empty line or comment is ignored
				}
				else
				{
					str.Format( "Unknown command on line %i of template file %s: %s", 
								lineNumber, pTheFileName, (LPCTSTR)line );
					AfxMessageBox( str, MB_OK | MB_ICONERROR );
					break;
				}
			}
		}
		
		if (defining)
		{
			str.Format( "Unexpected end of file %s while defining %s", 
						pTheFileName, (LPCTSTR)templ.m_name );
			AfxMessageBox( str, MB_OK | MB_ICONERROR );
		}
		
		// Add any open collection to the library
		if (pCollection != NULL)
		{
			m_collections.Add( pCollection );
		}
		templateFile.Close();
	}
}

//=============================================================================
// Delete all templates
void ScriptTemplateLibrary::RemoveAll()
{
	for (int ix = 0; ix < m_collections.GetSize(); ix++)
	{
		ScriptTemplateCollection *pCollection = m_collections[ix];
		delete pCollection;
	}

	m_collections.RemoveAll();
}

//=============================================================================
// Fill the specified menu with our template sections
void ScriptTemplateLibrary::FillMenu( CMenu &theMenu, bool atEnd )
{
	int ix;

	// shortcuts for template sections
	static char shortcuts[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	// Remove any previous menu items
	for (ix = 0; ix < 20; ix++)
	{
		theMenu.DeleteMenu( ID_SCRIPT_TEMPLATE_0+ix, MF_BYCOMMAND );
	}
	
	// Build the new menu
	for (ix = 0; ix < m_collections.GetSize(); ix++)
	{
		ScriptTemplateCollection *pCollection = m_collections[ix];
		CString name;

		char sh[3];
		if (ix < 36)
		{
			// Shortcut 1-9, A-Z
			sh[0] = '&';
			sh[1] = shortcuts[ix];
			sh[2] = 0;
		}
		else
		{
			// No shortcut
			sh[0] = ' ';
			sh[1] = ' ';
			sh[2] = 0;
		}

		name.Format( "%s %s...", sh, pCollection->Title() );
		if (atEnd)
		{
			// Add at the end of the menu
			theMenu.AppendMenu( MF_STRING, ID_SCRIPT_TEMPLATE_0+ix, name );
		}
		else
		{
			// Add at the top of the menu
			theMenu.InsertMenu( ix, MF_BYPOSITION, ID_SCRIPT_TEMPLATE_0+ix, name );
		}
	}
}

//=============================================================================
LPCTSTR ScriptTemplateLibrary::SectionTitle( int theIndex )
{
	LPCTSTR pName = "unknown";
	if (theIndex < m_collections.GetSize())
	{
		pName = m_collections[theIndex]->Title();
	}

	return pName;
}

//=============================================================================
const ScriptTemplateCollection& ScriptTemplateLibrary::Collection( int theIndex )
{
	if (theIndex >= m_collections.GetSize())
	{
		theIndex = 0;
	}
	return *m_collections[theIndex];
}

