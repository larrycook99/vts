// SendNull.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "SendNull.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendNull dialog

IMPLEMENT_DYNCREATE( CSendNull, CPropertyPage )

CSendNull::CSendNull( void )
	: CSendPage( CSendNull::IDD )
{
	//{{AFX_DATA_INIT(CSendNull)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSendNull::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendNull)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendNull, CPropertyPage)
	//{{AFX_MSG_MAP(CSendNull)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendNull message handlers
