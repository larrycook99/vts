#if !defined(AFX_VTSDEVICESTREESHEET_H__DAFD256F_0EF6_4F50_BEBE_686289CAB08D__INCLUDED_)
#define AFX_VTSDEVICESTREESHEET_H__DAFD256F_0EF6_4F50_BEBE_686289CAB08D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDevicesTreeSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeSheet

class VTSDevicesTreeSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(VTSDevicesTreeSheet)

// Construction
public:
	VTSDevicesTreeSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	VTSDevicesTreeSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDevicesTreeSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VTSDevicesTreeSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(VTSDevicesTreeSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDEVICESTREESHEET_H__DAFD256F_0EF6_4F50_BEBE_686289CAB08D__INCLUDED_)
