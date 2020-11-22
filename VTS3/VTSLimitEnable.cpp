// VTSLimitEnable.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSLimitEnable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSLimitEnable dialog


VTSLimitEnable::VTSLimitEnable(CWnd* pParent /*=NULL*/)
	: CDialog(VTSLimitEnable::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSLimitEnable)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VTSLimitEnable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSLimitEnable)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSLimitEnable, CDialog)
	//{{AFX_MSG_MAP(VTSLimitEnable)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSLimitEnable message handlers
