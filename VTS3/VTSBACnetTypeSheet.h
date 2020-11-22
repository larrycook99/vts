#if !defined(AFX_VTSBACNETTYPESHEET_H__5647A25B_126C_4D82_9D8E_C0BBCCF887CF__INCLUDED_)
#define AFX_VTSBACNETTYPESHEET_H__5647A25B_126C_4D82_9D8E_C0BBCCF887CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSBACnetTypeSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VTSBACnetTypeSheet

class VTSBACnetTypeSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(VTSBACnetTypeSheet)

// Construction
public:
	VTSBACnetTypeSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	VTSBACnetTypeSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSBACnetTypeSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VTSBACnetTypeSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(VTSBACnetTypeSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSBACNETTYPESHEET_H__5647A25B_126C_4D82_9D8E_C0BBCCF887CF__INCLUDED_)
