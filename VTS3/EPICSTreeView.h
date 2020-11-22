#if !defined(AFX_EPICSTREEVIEW_H__BCB4A3DF_B51B_4104_ABD8_649B27D690C6__INCLUDED_)
#define AFX_EPICSTREEVIEW_H__BCB4A3DF_B51B_4104_ABD8_649B27D690C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EPICSTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEPICSTreeView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "resource.h"
#include "VTSDoc.h"

// Added by Liangping Xu,2002-11
//madanner 6/03, moved out of ScriptFrame
//#include "CheckEPICSCons.h"
//#include "ScriptLoadResults.h"

#include "VTSPreferences.h"
#include "VTSDoc.h"
#include "FrameContext.h"

extern VTSPreferences gVTSPreferences;


class CDockingEPICSViewBar;

class CEPICSTreeView : public CFormView, public CFrameContextListener
{
private:
	CSplitterWnd * m_pwndSplit;
	CCreateContext* m_pContext;
	CView * m_pnodeview;
	CImageList		m_imagelist;
	CDockingEPICSViewBar * m_pbar;

	BACnetPIInfo		m_summary;		// so it doesn't have to be allocated and deallocated
	HTREEITEM			m_htreeitemObjectParent;

	// Stuff for RP callback values assemblies
//	CPtrArray m_aRPCallback_ValuePtr;
//	CUIntArray m_aRPCallback_PropID;
//	CDWordArray m_aRPCallback_ObjID;

public:
	void AddRPValue( long obj_id, int prop_id, char * pbuffer, int nLen );

	static bool GetEpicsFilename( CString &theFilePath );

protected:
	CEPICSTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEPICSTreeView)

	virtual void ContextChange( CFrameContext::Signal s );

	void ResizeControl(CWnd * pwnd, int cx, int cy);
	CView * CreateInfoView( CRuntimeClass * pviewclass, BOOL fDeleteOldView );
	void Refresh();
	void DeleteDB();
	void CreateRichTextView();
	void LoadObjectNodes( HTREEITEM htreeitemParent );
	void WipeOut( CTreeCtrl * ptree, HTREEITEM htreeitem );
	void SetTreeImageList();
	void PrepRPCallback( CEPICSTreeView * pthis );
	void LoadDACombo(void);


	// Node selection methods
	void DisplayRootNode();



// Form Data
public:
	//{{AFX_DATA(CEPICSTreeView)
	enum { IDD = IDD_EPICSVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	VTSDoc*		GetDocument();
	void SetSplitter( CDockingEPICSViewBar * pbar, CSplitterWnd * pwndSplit, CCreateContext * pContext );
	BOOL EPICSLoad( LPCSTR lpszFileName );
	CView * GetInfoPanel();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEPICSTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEPICSTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CEPICSTreeView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEpicsReset();
	afx_msg void OnEpicsLoad();
	afx_msg void OnSelchangedEpicsTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEpicsEdit();
	afx_msg void OnDestroy();
	afx_msg void OnEpicsReadSingleProps();
	afx_msg void OnEpicsReadAllProps();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropdownEpicsDA();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#ifndef _DEBUG  // debug version in EPICSView.cpp
inline VTSDoc* CEPICSTreeView::GetDocument()
   { return (VTSDoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EPICSTREEVIEW_H__BCB4A3DF_B51B_4104_ABD8_649B27D690C6__INCLUDED_)
