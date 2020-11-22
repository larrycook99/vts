// SendReadProp.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendReadProp.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendReadProp::pageContents[glMaxHistoryCount];
int CSendReadProp::curHistoryIndex = 0;
int CSendReadProp::historyCount = 0;
/////////////////////////////////////////////////////////////////////////////
// CSendReadProp dialog

IMPLEMENT_DYNCREATE( CSendReadProp, CPropertyPage )

#pragma warning( disable : 4355 )
CSendReadProp::CSendReadProp( void )
	: CSendPage( CSendReadProp::IDD )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_PropCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
{
	//{{AFX_DATA_INIT(CSendReadProp)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendReadProp::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendReadProp::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReadProp)
	//}}AFX_DATA_MAP

	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_PropCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendReadProp, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReadProp)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReadProp::OnInitDialog
//

BOOL CSendReadProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_PropCombo.LoadCombo();
	
	return TRUE;
}

//
//	CSendReadProp::InitPage
//

void CSendReadProp::InitPage( void )
{
	// flush the data
	m_ObjectID.ctrlNull = true;
	m_PropCombo.ctrlNull = true;
	m_ArrayIndex.ctrlNull = true;
}

//
//	CSendReadProp::EncodePage
//

void CSendReadProp::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 12 );

	// encode the object ID
	if (m_ObjectID.ctrlNull)
		throw "Object ID required";
	m_ObjectID.Encode( enc, 0 );

	// encode the property
	if (m_PropCombo.ctrlNull)
		throw "Property ID required";
	m_PropCombo.Encode( enc, 1 );

	// encode the (optional) array index
	if (!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode( enc, 2 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendReadProp::SavePage
//

void CSendReadProp::SavePage( void )
{
	TRACE0( "CSendReadProp::SavePage\n" );

	pageContents[curHistoryIndex].Flush();

	m_ObjectID.SaveCtrl( pageContents[curHistoryIndex] );
	m_PropCombo.SaveCtrl( pageContents[curHistoryIndex] );
	m_ArrayIndex.SaveCtrl( pageContents[curHistoryIndex] );
}

//
//	CSendReadProp::RestorePage
//

void CSendReadProp::RestorePage( int index )
{
	TRACE0( "CSendReadProp::RestorePage\n" );

	if(historyCount < 1)
		return;
	
	if(index > historyCount)
		return;

	index = curHistoryIndex - index - 1;
	if(index < 0)
		index = index + glMaxHistoryCount;

	BACnetAPDUDecoder	dec( pageContents[index] );
	

	if (dec.pktLength == 0)
		return;

	m_ObjectID.RestoreCtrl( dec );
	m_PropCombo.RestoreCtrl( dec );
	m_ArrayIndex.RestoreCtrl( dec );

	if(isShown)
	{
		m_ObjectID.ObjToCtrl();
		m_PropCombo.ObjToCtrl();
		m_ArrayIndex.ObjToCtrl();		
	}	 

}

//
//	CSendReadProp::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendReadProp::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadProp::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)		// added parent wnd to bind to modeless Send dialog
	;

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendReadProp::OnSelchangePropCombo()
{
	m_PropCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadProp::OnChangeArrayIndex()
{
	m_ArrayIndex.UpdateData();
	SavePage();
	UpdateEncoded();
}

// madanner 9/04, added from EPICS view

void CSendReadProp::ForceValues(BACnetObjectIdentifier * pObjectID, BACnetEnumerated * pPropID )
{
	m_ObjectID.objID = pObjectID->objID;
	m_ObjectID.ctrlNull = FALSE;
	m_PropCombo.m_enumValue = pPropID->m_enumValue;
	m_PropCombo.ctrlNull = FALSE;
	m_ArrayIndex.ctrlNull = true;

	m_ObjectID.ObjToCtrl();
	m_PropCombo.ObjToCtrl();
	SavePage();
	UpdateEncoded();
}


void CSendReadProp::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	isShown = bShow;

	if(bShow)
	{
		pageParent->SetHistoryComboBox(historyCount);
		pageParent->curPagePtr = this;
	}
}

void CSendReadProp::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	if(historyCount < glMaxHistoryCount)
		historyCount++;

	curHistoryIndex++;
	
	if(curHistoryIndex > glMaxHistoryCount - 1)
		curHistoryIndex = 0;			
}

void CSendReadProp::OnDropdownPropcombo() 
{
//Added by Zhu Zhenhua , 2003-7-22 
//	to Load Standard property list for certain ObjectType
	m_PropCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_PropCombo.LoadCombo();
}
