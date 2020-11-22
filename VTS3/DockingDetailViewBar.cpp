// DockingDetailViewBar.cpp: implementation of the CDockingDetailViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DockingDetailViewBar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDockingDetailViewBar::CDockingDetailViewBar(CCreateContext* pContext)
: m_pList(NULL)
{
   m_pContext = pContext;
   CRuntimeClass* pFactory = RUNTIME_CLASS(CDetailView);
   m_pDetailView = (CDetailView *)(pFactory->CreateObject() );
}

CDockingDetailViewBar::~CDockingDetailViewBar()
{
   delete m_pList;
   delete m_pDetailView;
}


BEGIN_MESSAGE_MAP(CDockingDetailViewBar, baseCMyBar)
	//{{AFX_MSG_MAP(CDockingDetailViewBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CDockingDetailViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (baseCMyBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pList=new CDetailTreeCtrl();
	m_pList->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP 
		|TVS_HASLINES |TVS_HASBUTTONS |TVS_LINESATROOT , CRect(0,0,0,0),this,0x1001);

	CRect rc;
	ScreenToClient(rc);
	if(m_pList->GetSafeHwnd())
		m_pList->MoveWindow(rc);
	
	m_pList->SetContext(gNewFrameContext);
	return 0;
}

