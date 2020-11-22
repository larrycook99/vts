// SendReadFDT.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendReadFDT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendReadFDT dialog

IMPLEMENT_DYNCREATE(CSendReadFDT, CPropertyPage)


CSendReadFDT::CSendReadFDT( void )
	: CSendPage( CSendReadFDT::IDD )
{
	//{{AFX_DATA_INIT(CSendReadFDT)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSendReadFDT::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReadFDT)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendReadFDT, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReadFDT)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReadFDT::EncodePage
//

void CSendReadFDT::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendReadFDT::EncodePage()\n" );

	// encode the type and function
	header.Add( 0x81 );
	header.Add( 0x06 );

	// encode the length
	header.Add( 0x00 );
	header.Add( 0x04 );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

