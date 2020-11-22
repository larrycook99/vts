// DockingEPICSViewBar.h: interface for the CDockingEPICSViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCKINGEPICSVIEWBAR_H__17B301D3_5476_443F_B4D4_49A961ADA8B0__INCLUDED_)
#define AFX_DOCKINGEPICSVIEWBAR_H__17B301D3_5476_443F_B4D4_49A961ADA8B0__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EPICSTreeView.h"

#ifndef baseCMyBar
#define baseCMyBar CSizingControlBarCF
#endif

class CDockingEPICSViewBar : public baseCMyBar
{
private:
	CCreateContext* m_pContext;
	CSplitterWnd		 * m_pwndSplit;
	BOOL			m_fPrevHorizDock;
	CEPICSTreeView * m_pEPICSView;

	int CreateViewLayout(BOOL fHorizDock);

public:

	CDockingEPICSViewBar(CCreateContext* pContext);
	virtual ~CDockingEPICSViewBar();
	virtual void OnDockPositionChanged();

	CWnd*		m_pTabRing;
	int			m_nSyntaxE;
	int         m_nConE;


	BOOL EPICSLoad( LPCSTR lpszFileName );			// returns TRUE if there were no errors


protected:
	//{{AFX_MSG(CDockingEPICSViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_DOCKINGEPICSVIEWBAR_H__17B301D3_5476_443F_B4D4_49A961ADA8B0__INCLUDED_)


