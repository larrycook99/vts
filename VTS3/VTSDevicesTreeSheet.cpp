// VTSDevicesTreeSheet.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDevicesTreeSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeSheet

IMPLEMENT_DYNAMIC(VTSDevicesTreeSheet, CPropertySheet)

VTSDevicesTreeSheet::VTSDevicesTreeSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

VTSDevicesTreeSheet::VTSDevicesTreeSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

VTSDevicesTreeSheet::~VTSDevicesTreeSheet()
{
}


BEGIN_MESSAGE_MAP(VTSDevicesTreeSheet, CPropertySheet)
	//{{AFX_MSG_MAP(VTSDevicesTreeSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeSheet message handlers
