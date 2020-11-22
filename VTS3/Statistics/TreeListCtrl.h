// TreeListCtrl.h: interface for the CTreeListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TreeListCtrl_H__AEF543F1_8668_4A63_823C_4207B29C3BA3__INCLUDED_)
#define AFX_TreeListCtrl_H__AEF543F1_8668_4A63_823C_4207B29C3BA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "supergridctrl.h"

class VTSStatisticsDlg;
typedef VTSStatisticsDlg *VTSStatisticsDlgPtr;

class CGroupIndex: public CObject
{
public:
	CGroupIndex(int start,int end);

// Attributes
public:
	int m_nStartPos;
	int m_nEndPos;

	
// Operations
public:
// Implementation
public:
	virtual ~CGroupIndex();
};


typedef CTypedPtrMap<CMapStringToOb,CString,CGroupIndex*> CMapStringToIndex;

class CTreeListCtrl : public CSuperGridCtrl  
{
public:
	void GetSizeStatistics(int min,int max,CString name);
	void GetGeneralStatistics();
	void GetServiceStatistics(CString service);
	void GetGroupStatistics(CString group);
	void InitializeGrid();
	CTreeListCtrl(VTSStatisticsDlgPtr pParentDlg);
	virtual ~CTreeListCtrl();
	BOOL OnItemLButtonDown(LVHITTESTINFO& ht);
	CItemInfo* CopyData(CItemInfo* lpSrc);
	BOOL PreTranslateMessage(MSG* pMsg) ;
	void UpdateStatistics(CString service);
	CTreeListCtrl::CTreeItem* SearchEx(CTreeItem *pStartPosition, CString strItem);

	VTSStatisticsDlg* m_pParentDlg;

	int m_nColumnData[12];

protected:
	CImageList m_image;//Must provide an imagelist
	//{{AFX_MSG(CTreeListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	CMapStringToIndex m_mapGroupToIndex;
};

#endif // !defined(AFX_TreeListCtrl_H__AEF543F1_8668_4A63_823C_4207B29C3BA3__INCLUDED_)
