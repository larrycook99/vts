// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__BDE65086_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
#define AFX_CHILDFRM_H__BDE65086_B82F_11D3_BE52_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FrameContext.h"
/////////////////////////////////
#include "DockingDetailViewBar.h"
#include "DockingHexViewBar.h"
#include "DockingEPICSViewBar.h"
/////////////////////////////////
#include "VdbPrint.h"
/////////////////////////////////
// forward definitions
class CDetailView;
class CHexView;
class CListSummaryView;
class CReadAllPropSettingsDlg;


class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)

// Attributes
public:
	CSplitterWnd	m_wndSplit1;
	CSplitterWnd	m_wndSplit2;        // embedded in the first
	CFrameContext*	m_frameContext;		// frame context

	CListSummaryView*	m_pSummaryView;
	CDetailTreeCtrl*  m_pDetailView;
	CHexView*		m_pHexView;
//	CEPICSView *	m_pEPICSView;
	CDockingDetailViewBar* m_pwndDetailViewBar;
	CDockingHexViewBar* m_pwndHexViewBar;
	CDockingEPICSViewBar * m_pwndEPICSViewBar;


	/////////////////////////////////
	//Added by Zhenhua Zhu, 2003-6-2
	CVdbPrint m_printer;
	BOOL	  m_bPrintSetup;
	BOOL	  m_bInPrinting;

	/////////////////////////////////
protected:
		CToolBar * m_wndGlobalBar;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	CChildFrame();
	virtual ~CChildFrame();

	void SaveBarStates(void);
	void EPICSLoad( LPCSTR lpszFileName );
	void DoReadAllProperties();
	void DoReadSingleProperties( LPCSTR lpszFileName, unsigned long ulObjectID );


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


private:
	BOOL CreateScriptFile( CString * pstrFileName, CReadAllPropSettingsDlg * pdlg, unsigned long ulObjectID );

// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnCancelMode();
	afx_msg void OnUpdateFileWksNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileWksSwitch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileWksSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRefresh(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPorts(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditNames(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDevices(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditLogfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCaptureFilters(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDisplayFilters(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFirstFrame(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLastFrame(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewNextFrame(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPrevFrame(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSend(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewDetail(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewHex(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewEPICS(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEPICSLoad(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEPICSReadallproperty(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnEditRefresh();
	afx_msg void OnEditPorts();
	afx_msg void OnEditNames();
	afx_msg void OnEditDevices();
	afx_msg void OnEditLogfile();
	afx_msg void OnEditCaptureFilters();
	afx_msg void OnEditDisplayFilters();
	afx_msg void OnEditPreferences();
	afx_msg void OnViewFirstFrame();
	afx_msg void OnViewPrevFrame();
	afx_msg void OnViewNextFrame();
	afx_msg void OnViewLastFrame();
	afx_msg void OnSendNewPacket();
	afx_msg void OnSendSelectPort(UINT uID);
	afx_msg void OnSendSelectPacket(UINT uID);
	//add by Hu Meng responding to new menu items
	afx_msg void OnViewDetail();
	afx_msg void OnViewHex();
	afx_msg void OnViewEPICS();
	afx_msg void OnEPICSLoad();
	afx_msg void OnEPICSLoadAuto();
	//end 6.24
	// Added by Yajun Zhou, 2002-7-24
	afx_msg void OnFileExport();
	afx_msg void OnDestroy();
	//Added by Zhenhua Zhu, 2003-6-2
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintSetup();
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnReadAllProperty();
	//Added by Zhu Zhenhua, 2004-11-27,for #508589 request 
	afx_msg void OnGlobalScriptSteppass();
	afx_msg void OnUpdateGlobalScriptSteppass(CCmdUI* pCmdUI);
	afx_msg void OnGlobalScriptStepfail();
	afx_msg void OnUpdateGlobalScriptStepfail(CCmdUI* pCmdUI);
	afx_msg void OnGlobalScriptStep();
	afx_msg void OnUpdateGlobalScriptStep(CCmdUI* pCmdUI);
	afx_msg void OnGlobalScriptKill();
	afx_msg void OnUpdateGlobalScriptKill(CCmdUI* pCmdUI);
	afx_msg void OnGlobalScriptHalt();
	afx_msg void OnUpdateGlobalScriptHalt(CCmdUI* pCmdUI);
	afx_msg void OnGlobalDisablePort();
	afx_msg void OnUpdateGlobalDisablePort(CCmdUI* pCmdUI);
	afx_msg void OnGlobalScriptChecksyntax();
	afx_msg void OnUpdateGlobalScriptChecksyntax(CCmdUI* pCmdUI);
	afx_msg void OnGlobalScriptReset();
	afx_msg void OnUpdateGlobalScriptReset(CCmdUI* pCmdUI);
	afx_msg void OnGlobalScriptRun();
	afx_msg void OnUpdateGlobalScriptRun(CCmdUI* pCmdUI);
	afx_msg void OnViewMaintoolbar();
	afx_msg void OnViewGlobalToolbar();
	afx_msg void OnUpdateViewMaintoolbar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewGlobalToolbar(CCmdUI* pCmdUI);
	//}}AFX_MSG
	void OnBackupRestore();
	void OnUpdateBackupRestore(CCmdUI* pCmdUI);
	void OnInconsistentPars();
	void OnUpdateInconsistentPars(CCmdUI* pCmdUI);
	void OnDiscoverDevices();
	void OnGenerateEPICS();
	void OnUpdateDiscovery(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__BDE65086_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
