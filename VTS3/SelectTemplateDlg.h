#if !defined(AFX_SELECTTEMPLATEDLG_H__5F7DF683_1D51_449C_8B18_FB2AF6218163__INCLUDED_)
#define AFX_SELECTTEMPLATEDLG_H__5F7DF683_1D51_449C_8B18_FB2AF6218163__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectTemplateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectTemplateDlg dialog
class ScriptTemplateCollection;

class SelectTemplateDlg : public CDialog
{
// Construction
public:
	SelectTemplateDlg(const ScriptTemplateCollection &theCollection, CWnd* pParent = NULL);

	int GetTemplateIndex() const { return m_templateIndex; }

// Dialog Data
	//{{AFX_DATA(SelectTemplateDlg)
	enum { IDD = IDD_SELECT_TEMPLATE };
	CListBox	m_templateList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelectTemplateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	const ScriptTemplateCollection &m_collection;
	int m_templateIndex;
	
	// Generated message map functions
	//{{AFX_MSG(SelectTemplateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTTEMPLATEDLG_H__5F7DF683_1D51_449C_8B18_FB2AF6218163__INCLUDED_)
