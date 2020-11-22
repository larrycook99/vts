// ScriptLoadResults.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "ScriptLoadResults.h"

namespace PICS {

#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"

#include "propid.h"

}

extern PICS::PICSdb *gPICSdb;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptLoadResults dialog


ScriptLoadResults::ScriptLoadResults(CWnd* pParent /*=NULL*/)
	: CDialog(ScriptLoadResults::IDD, pParent)
{
	//{{AFX_DATA_INIT(ScriptLoadResults)
	m_VendorName = gPICSdb->VendorName;
	m_ProductName = gPICSdb->ProductName;
	m_ProductModelNumber = gPICSdb->ProductModelNumber;
	m_ProductDescription = gPICSdb->ProductDescription;
	//}}AFX_DATA_INIT
}


void ScriptLoadResults::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ScriptLoadResults)
	DDX_Text(pDX, IDC_VENDORNAME, m_VendorName);
	DDX_Text(pDX, IDC_PRODUCTNAME, m_ProductName);
	DDX_Text(pDX, IDC_PRODUCTMODELNUMBER, m_ProductModelNumber);
	DDX_Text(pDX, IDC_PRODUCTDESCRIPTION, m_ProductDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ScriptLoadResults, CDialog)
	//{{AFX_MSG_MAP(ScriptLoadResults)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptLoadResults message handlers
