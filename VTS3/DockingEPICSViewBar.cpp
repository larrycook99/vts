// DockingEPICSViewBar.cpp: implementation of the CDockingEPICSViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "vts.h"
#include "DockingEPICSViewBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDockingEPICSViewBar::CDockingEPICSViewBar(CCreateContext* pContext)
{
	m_pContext = new CCreateContext();
	m_pContext->m_pCurrentDoc = pContext->m_pCurrentDoc;
	m_pContext->m_pCurrentFrame = pContext->m_pCurrentFrame;
	m_pContext->m_pLastView = pContext->m_pLastView;
	m_pContext->m_pNewDocTemplate = pContext->m_pNewDocTemplate;
	m_pContext->m_pNewViewClass = pContext->m_pNewViewClass;

	m_fPrevHorizDock = FALSE;
	m_pwndSplit = NULL;
	m_nSyntaxE = 0;
	m_nConE = 0;


//	CRuntimeClass* pFactory = RUNTIME_CLASS(CEPICSView);
//  m_pEPICSView = (CEPICSView *)(pFactory->CreateObject() );
}


CDockingEPICSViewBar::~CDockingEPICSViewBar()
{
	if ( m_pwndSplit != NULL )
		delete m_pwndSplit;
	if ( m_pContext != NULL )
		delete m_pContext;
}



BEGIN_MESSAGE_MAP(CDockingEPICSViewBar, baseCMyBar)
	//{{AFX_MSG_MAP(CDockingEPICSViewBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CDockingEPICSViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (baseCMyBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( CreateViewLayout(IsHorzDocked()) == -1 )
		return -1;

	return 0;
}


int CDockingEPICSViewBar::CreateViewLayout(BOOL fHorizDock)
{
	if ( m_pwndSplit != NULL )
		delete m_pwndSplit;

	m_pwndSplit = new CSplitterWnd();

	// create a splitter with 2 rows, 1 column
	if ( !m_pwndSplit->CreateStatic(this, fHorizDock ? 1 : 2, fHorizDock ? 2 : 1) )
	{
		TRACE0("Failed to CreateStaticSplitter for EPICS view\n");
		return -1;
	}

	if ( !m_pwndSplit->CreateView(0, 0, RUNTIME_CLASS(CEPICSTreeView), CSize(fHorizDock ? 350 : 200, fHorizDock ? 200 : 300), m_pContext) )
	{
		TRACE0("Failed to create Tree pane on EPICS view\n");
		return -1;
	}
	
	m_pEPICSView = (CEPICSTreeView *) m_pwndSplit->GetPane(0, 0);
	m_pEPICSView->SetSplitter(this, m_pwndSplit, m_pContext);
	OnInitialUpdate();

	return 0;
}


void CDockingEPICSViewBar::OnSize(UINT nType, int cx, int cy)
{
	baseCMyBar::OnSize(nType,cx,cy);
}


void CDockingEPICSViewBar::OnDockPositionChanged()
{
	BOOL fHoriz = IsHorzDocked();

	if ( fHoriz != m_fPrevHorizDock )
	{
		m_pwndSplit->DestroyWindow();
		CreateViewLayout(fHoriz);

		RECT rc;
		GetClientRect(&rc);
		OnSize(0, rc.right-rc.left, rc.bottom-rc.top);

		m_fPrevHorizDock = fHoriz;
	}

	baseCMyBar::OnDockPositionChanged();
}


// returns TRUE if there were no errors

BOOL CDockingEPICSViewBar::EPICSLoad( LPCSTR lpszFileName )
{
	return m_pEPICSView->EPICSLoad(lpszFileName);
}


void CDockingEPICSViewBar::OnDestroy() 
{
	baseCMyBar::OnDestroy();
	if ( m_pwndSplit != NULL )
		delete m_pwndSplit;
	m_pwndSplit = NULL;
}
