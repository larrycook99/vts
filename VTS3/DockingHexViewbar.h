// DockingHexViewbar.h: interface for the CDockingHexViewbar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCKINGHEXVIEWBAR_H__5470E55D_940E_46C4_92AB_532BD9020E51__INCLUDED_)
#define AFX_DOCKINGHEXVIEWBAR_H__5470E55D_940E_46C4_92AB_532BD9020E51__INCLUDED_

#if !defined AFX_HEXVIEW_H
#include "HexView.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef baseCMyBar
#define baseCMyBar CSizingControlBarCF
#endif

class CDockingHexViewBar : public baseCMyBar  
{
public:
	CDockingHexViewBar(CCreateContext* pContext);
	virtual ~CDockingHexViewBar();
	CHexView* m_pHexView;
	CCreateContext* m_pContext;

protected:
	//{{AFX_MSG(CDockingHexViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_DOCKINGHEXVIEWBAR_H__5470E55D_940E_46C4_92AB_532BD9020E51__INCLUDED_)
