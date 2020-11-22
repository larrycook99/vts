// SendReadBDT.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendReadBDT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendReadBDT property page

IMPLEMENT_DYNCREATE(CSendReadBDT, CPropertyPage)


CSendReadBDT::CSendReadBDT( void )
	: CSendPage( CSendReadBDT::IDD )
{
	//{{AFX_DATA_INIT(CSendReadBDT)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSendReadBDT::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReadBDT)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendReadBDT, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReadBDT)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReadBDT::EncodePage
//

void CSendReadBDT::EncodePage( CByteArray* contents )
{
	CByteArray	header
	;

	TRACE0( "CSendReadBDT::EncodePage()\n" );

	header.Add( 0x81 );
	header.Add( 0x02 );
	header.Add( 0x00 );
	header.Add( 0x04 );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

