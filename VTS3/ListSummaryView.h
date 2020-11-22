#if !defined(AFX_LISTSUMMARYVIEW_H__B2A66D8C_50B2_4533_A960_50C0FECFD5F7__INCLUDED_)
#define AFX_LISTSUMMARYVIEW_H__B2A66D8C_50B2_4533_A960_50C0FECFD5F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListSummaryView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListSummaryView view
#include "FrameContext.h"
#include "ListSummaryCache.h"

#include <afxcview.h>

class CListSummaryView : public CListView,public CFrameContextListener
{
	friend int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, 
   LPARAM lParamSort);
	
protected:
	CListSummaryView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CListSummaryView)

// Attributes
public:
	CWnd* m_pTabRing;
// Operations
public:
	void ContextChange( CFrameContext::Signal s );

// No need for method anymore
//	CString* GetLineData(int lineNo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListSummaryView)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CListSummaryView();

private:
    BOOL m_bSortType; //modified: 2004/10/14 author:Xiao Shiyuan	purpose:Sorting flag, 
                    //TRUE: sort ascending FALSE: sort descending
	int  m_sortColumn; //column which is sorted
	BACnetPIInfo		m_summary;		// so it doesn't have to be allocated and deallocated
	CListSummaryCache	m_cache;

	CImageList          m_iconList;    // 2/17/2005 Shiyuan Xiao

	bool                m_bColumn[10]; //Added: 2004/11/23 author:Xiao Shiyuan	purpose: customize columns
	int                 m_columnWidth[10];
	void ReadReg();
	void SaveReg();

	CToolTipCtrl m_tooltip; //added: 2004/11/03 author:Xiao Shiyuan	
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);

	void SetSelectedLine(int currentLineNo);
	void AddLine(int lineNo);
	char * FillColumnData( int nColumn, char * pszFill, VTSPacket * ppkt );
	void CacheItem(int nItem, LVCachedItem * pcacheditem );

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CListSummaryView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnViewColor();
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowColumns(UINT nID);
	afx_msg void OnUpdateColumns(CCmdUI* pCmdUI);
	afx_msg void OnEditDeleteSel();
	afx_msg void OnEditAutoScroll();
	afx_msg void OnUpdateEditAutoScroll(CCmdUI* pCmdUI);
	afx_msg void OnEditReceivePkt();
	afx_msg void OnUpdateEditReceivePkt(CCmdUI* pCmdUI);
	afx_msg void OnEditSaveSentPkt();
	afx_msg void OnUpdateEditSaveSentPkt(CCmdUI* pCmdUI);
	afx_msg void OnEditSendPacket();
	afx_msg void OnUpdateEditSendPacket(CCmdUI* pCmdUI);
	afx_msg void OnDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEditDeleteSel(CCmdUI* pCmdUI);
	afx_msg void OnEditColumnValue();
	afx_msg void OnUpdateEditColumnValue(CCmdUI* pCmdUI);
	afx_msg void OnEditSendAllPacket();
	afx_msg void OnUpdateEditSendAllPacket(CCmdUI* pCmdUI);
	afx_msg void OnEditEditSendAllPkt();
	afx_msg void OnUpdateEditEditSendAllPkt(CCmdUI* pCmdUI);
	//}}AFX_MSG
	//Xiao Shiyuan 9/30/2004. Color Listview.  
	afx_msg void OnCustomdrawList(NMHDR*, LRESULT*);
	DECLARE_MESSAGE_MAP()
};

typedef struct tagListSortParam{
	CListSummaryView* pt;
	BOOL type;
    int  column;
}ListSortParam;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTSUMMARYVIEW_H__B2A66D8C_50B2_4533_A960_50C0FECFD5F7__INCLUDED_)
