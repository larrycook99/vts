#if !defined(AFX_SCRIPTCONTENTTREE_H__00DACA52_BA0F_11D4_BEF4_00A0C95A9812__INCLUDED_)
#define AFX_SCRIPTCONTENTTREE_H__00DACA52_BA0F_11D4_BEF4_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptContentList.h : header file
//

#include <afxcview.h>

#include "ScriptBase.h"
//Added by Yajun Zhou, 2002-6-20
#include "ScriptEdit.h"
////////////////////////////////

class ScriptDocument;
typedef ScriptDocument *ScriptDocumentPtr;

//
//	ScriptContentTree
//

class ScriptContentTree : public CTreeView
{
protected:
	ScriptContentTree();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(ScriptContentTree)

// Attributes
public:
	CTreeCtrl		*m_pTreeCtrl;
	CImageList		m_ilStatus;
	//Added by Yajun Zhou, 2002-6-20
	ScriptEdit*		m_pEditView;
	////////////////////////////////
	CFrameWnd *		m_pframe;

// Operations
public:
	ScriptDocumentPtr	m_pDoc;
	ScriptBasePtr		m_pScriptContent;

	void Bind( ScriptBasePtr sbp );
	void Load( HTREEITEM parent, ScriptBasePtr sbp );
	void UnLoad( HTREEITEM parent, ScriptBasePtr sbp );
	void SetFrame( CFrameWnd * pframe ) { m_pframe = pframe; }  

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScriptContentTree)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~ScriptContentTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(ScriptContentTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTCONTENTTREE_H__00DACA52_BA0F_11D4_BEF4_00A0C95A9812__INCLUDED_)
