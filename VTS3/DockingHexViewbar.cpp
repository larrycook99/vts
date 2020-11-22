// DockingHexViewbar.cpp: implementation of the CDockingHexViewbar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DockingHexViewbar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDockingHexViewBar::CDockingHexViewBar(CCreateContext* pContext)
{
	m_pContext = pContext;

	CRuntimeClass* pFactory = RUNTIME_CLASS(CHexView);
    m_pHexView = (CHexView *)(pFactory->CreateObject() );
}

CDockingHexViewBar::~CDockingHexViewBar()
{

}

BEGIN_MESSAGE_MAP(CDockingHexViewBar, baseCMyBar)
	//{{AFX_MSG_MAP(CDockingHexViewBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CDockingHexViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (baseCMyBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect r;
	r=GetBorders();
	// TODO: Add your specialized creation code here
	if (m_pHexView )
   {
       m_pHexView->CreateView(this,m_pContext,r);
   }

	return 0;
}

void CDockingHexViewBar::OnSize(UINT nType, int cx, int cy)
{
	baseCMyBar::OnSize(nType,cx,cy);
	
}
