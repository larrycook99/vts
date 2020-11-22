//******************************************************************
// 	Author:			Yajun Zhou
//	Date:			2002-4-27
//	Description:	Display line's indexes in front of a 
//					edit box.
//******************************************************************
// LineNumCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "LineNumCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineNumCtrl

CLineNumCtrl::CLineNumCtrl()
{
	m_nLineCount = 1;
}

CLineNumCtrl::~CLineNumCtrl()
{
}


BEGIN_MESSAGE_MAP(CLineNumCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CLineNumCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineNumCtrl message handlers

void CLineNumCtrl::SetDefaultFont()
{
	CFont font;
	VERIFY(font.CreateFont(
		   16,                        // nHeight
		   0,                         // nWidth
		   0,                         // nEscapement
		   0,                         // nOrientation
		   FW_NORMAL,                 // nWeight
		   FALSE,                     // bItalic
		   FALSE,                     // bUnderline
		   0,                         // cStrikeOut
		   ANSI_CHARSET,              // nCharSet
		   OUT_DEFAULT_PRECIS,        // nOutPrecision
		   CLIP_DEFAULT_PRECIS,       // nClipPrecision
		   DEFAULT_QUALITY,           // nQuality
		   VARIABLE_PITCH | FF_ROMAN, // nPitchAndFamily
		  _T("Times New Roman")));    // lpszFacename
	
	SetFont(&font,TRUE);
	
	font.DeleteObject();

	SetBackgroundColor( FALSE, GetSysColor( COLOR_BTNFACE));
}

void CLineNumCtrl::Display(int nFirstVisibleLn, int nLastVisibleLn)
{
	CString strLnNum = "";
	CString str;
	
	for(int i = nFirstVisibleLn; 
		i <= min(m_nLineCount, nLastVisibleLn); 
		i++)
	{
		str.Format("%d",i);
		strLnNum +=str + "\n";
	}

	SetWindowText(strLnNum);
}
