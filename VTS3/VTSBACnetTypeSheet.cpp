// VTSBACnetTypeSheet.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSBACnetTypeSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSBACnetTypeSheet

IMPLEMENT_DYNAMIC(VTSBACnetTypeSheet, CPropertySheet)

VTSBACnetTypeSheet::VTSBACnetTypeSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

VTSBACnetTypeSheet::VTSBACnetTypeSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

VTSBACnetTypeSheet::~VTSBACnetTypeSheet()
{
}


BEGIN_MESSAGE_MAP(VTSBACnetTypeSheet, CPropertySheet)
	//{{AFX_MSG_MAP(VTSBACnetTypeSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSBACnetTypeSheet message handlers
