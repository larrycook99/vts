//******************************************************************
// 	Author:			Yajun Zhou
//	Date:			2002-4-27
//******************************************************************
#if !defined(AFX_LINENUMCTRL_H__D81FEE59_CF5F_476E_AAA7_ADDD31F797F0__INCLUDED_)
#define AFX_LINENUMCTRL_H__D81FEE59_CF5F_476E_AAA7_ADDD31F797F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LineNumCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLineNumCtrl window

class CLineNumCtrl : public CRichEditCtrl
{
// Construction
public:
	CLineNumCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineNumCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_nLineCount;
	void Display(int nFirstVisibleLn, int nLastVisibleLn);
	void SetDefaultFont();
	virtual ~CLineNumCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLineNumCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINENUMCTRL_H__D81FEE59_CF5F_476E_AAA7_ADDD31F797F0__INCLUDED_)
