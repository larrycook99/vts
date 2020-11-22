// VTSView.cpp : implementation of the CVTSView class
//

#include "stdafx.h"
#include "VTS.h"

#include "VTSDoc.h"
#include "VTSView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVTSView

IMPLEMENT_DYNCREATE(CVTSView, CView)

BEGIN_MESSAGE_MAP(CVTSView, CView)
	//{{AFX_MSG_MAP(CVTSView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVTSView construction/destruction

CVTSView::CVTSView()
{
	// TODO: add construction code here

}

CVTSView::~CVTSView()
{
}

BOOL CVTSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVTSView drawing

void CVTSView::OnDraw(CDC* pDC)
{
	CVTSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CVTSView printing

BOOL CVTSView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVTSView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVTSView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CVTSView diagnostics

#ifdef _DEBUG
void CVTSView::AssertValid() const
{
	CView::AssertValid();
}

void CVTSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVTSDoc* CVTSView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVTSDoc)));
	return (CVTSDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVTSView message handlers
