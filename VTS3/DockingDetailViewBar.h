// DockingDetailViewBar.h: interface for the CDockingDetailViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCKINGDETAILVIEWBAR_H__BFDFE529_25AE_4694_9AE0_F61EA28860D8__INCLUDED_)
#define AFX_DOCKINGDETAILVIEWBAR_H__BFDFE529_25AE_4694_9AE0_F61EA28860D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined AFX_DETAILVIEW_H
#include "DetailView.h"
#endif

#ifndef baseCMyBar
#define baseCMyBar CSizingControlBarCF
#endif

#include "DetailTreeCtrl.h"

class CDockingDetailViewBar : public baseCMyBar  
{
public:
	CDockingDetailViewBar(CCreateContext* pContext);
	virtual ~CDockingDetailViewBar();

	CDetailView* m_pDetailView;
	CCreateContext* m_pContext;
	CDetailTreeCtrl* m_pList;

protected:
	//{{AFX_MSG(CDockingDetailViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_DOCKINGDETAILVIEWBAR_H__BFDFE529_25AE_4694_9AE0_F61EA28860D8__INCLUDED_)
