// SendPage.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "SendPage.h"
#include "Send.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CSendPage

//
//	CSendPage::CSendPage
//

CSendPage::CSendPage( UINT id )
	: CPropertyPage( id )
	, pageParent(0)
{
	isShown = FALSE; //Xiao Shiyuan 2002-12-5
}

//
//	CSendPage::~CSendPage
//

CSendPage::~CSendPage( void )
{
}

//
//	CSendPage::UpdateEncoded
//

void CSendPage::UpdateEncoded( void )
{
	if (pageParent)
		pageParent->UpdateEncoded();
}

//
//	CSendPage::InitPage
//
//	This function should be overridden to initialize its member variables 
//	when it gets installed.  There may be information in the port that will 
//	be useful.
//

void CSendPage::InitPage( void )
{
}

//
//	CSendPage::EncodePage
//
//	This function should be overridden to encode the contents of the page 
//	into the byte array.  If the page is at the top of the protocol stack 
//	it will be empty, othersize it will contain the results of the upper 
//	layers.
//
//	Most pages add some prefix information to what was generated at the 
//	higher levels, some modify the contents, like the special character 
//	processing of PTP.  Someday there will be a security BVLL, so the contents 
//	could be encrypted.
//

void CSendPage::EncodePage( CByteArray* contents )
{
}

//
//	CSendPage::SavePage
//
//	This function is called to make a backup of the contents of a page so 
//	the next time the page is displayed the contents can be restored.
//

void CSendPage::SavePage( void )
{
}

//
//	CSendPage::RestorePage
//
//	This function is called at the end of InitPage to restore the page contents 
//	to what they were when the page was last saved.
//

void CSendPage::RestorePage( int )
{
}
