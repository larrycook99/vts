// dudapi32.h : main header file for the DUDAPI32 DLL
//

#if !defined(AFX_DUDAPI32_H__6414E072_C235_11D1_B0AB_008029E9B447__INCLUDED_)
#define AFX_DUDAPI32_H__6414E072_C235_11D1_B0AB_008029E9B447__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDudapi32App
// See dudapi32.cpp for the implementation of this class
//

class CDudapi32App : public CWinApp
{
public:
	CDudapi32App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDudapi32App)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDudapi32App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUDAPI32_H__6414E072_C235_11D1_B0AB_008029E9B447__INCLUDED_)
