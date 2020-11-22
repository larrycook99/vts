// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__BDE65084_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
#define AFX_MAINFRM_H__BDE65084_B82F_11D3_BE52_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HISTORY_LOGFILE_COUNT 5

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)

	CProgressCtrl	m_progress;

public:
	CMainFrame();

// Attributes
public:
	CComboBox   m_wndFileCombo;

// Operations
public:
	//Added: 2004/12/06 author:Xiao Shiyuan	purpose:save log file history	
	void SaveLogFileHistory(LPCTSTR filepath);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	//	Added by Yajun Zhou, 2002-4-22
// madanner, 5/03 moved to frame of edit
//	void SetLnPaneText(CString str);

	// Code for progress bar inside help text area of status bar
	CProgressCtrl * InitializeProgress( void );
	void ReleaseProgress( void );

	////////////////////////////////////
	//Added by Zhenhua Zhu, 2003-6-2
	CMDIChildWnd* GetChildFrame(CRuntimeClass *pClass, int nNum = 1);
	//Modified by Zhu Zhenhua, 2004-11-27,for #508589 request 
	void ShowOwnToolBar();
	bool GetToolBarStatus();
	//Added by Zhu Zhenhua, 2004-11-27, to control mainframe toobar's show up while in Childframe
	//because i think it's better than setting toolbar as public. 
	///////////////////////////////////

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CList<CString, CString>m_historyLogList;

	//modified: 2004/12/06 author:Xiao Shiyuan	purpose: read register table	
	void ReadReg();
	void SaveReg();

	void RestorePosition();

	//Added by Zhenhua Zhu, 2003-5-26
//////////////////////////////////

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowNew();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern int gSelectedPort;		// which port selected for Send menu
extern int gSelectedGroup;		// which packet group selected
extern int gSelectedItem;		// which item selected

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BDE65084_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
