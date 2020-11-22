// SendWritePropMultError.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendWritePropMultError.h"

#include "VTSObjectIdentifierDialog.h"
#include "Send.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendWritePropMultError::pageContents[glMaxHistoryCount];
int CSendWritePropMultError::historyCount = 0;           //Xiao Shiyuan 2002-12-5
int CSendWritePropMultError::curHistoryIndex = 0;        //Xiao Shiyuan 2002-12-5

/////////////////////////////////////////////////////////////////////////////
// CSendWritePropMultError dialog

IMPLEMENT_DYNCREATE( CSendWritePropMultError, CPropertyPage )

#pragma warning( disable : 4355 )
CSendWritePropMultError::CSendWritePropMultError( void )
	: CSendPage( CSendWritePropMultError::IDD )
	, m_InvokeID( this, IDC_INVOKEID )
	, m_ServiceCombo( this, IDC_SERVICECOMBO, NetworkSniffer::BAC_STRTAB_BACnetConfirmedServiceChoice, true )
	, m_ErrorClassCombo( this, IDC_ERRORCLASSCOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorClass, true )
	, m_ErrorCodeCombo( this, IDC_ERRORCODECOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorCode, true )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_PropertyID( this, IDC_PROPERTYID, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, false )
	, m_Index( this, IDC_INDEX )
{
	//{{AFX_DATA_INIT(CSendWritePropMultError)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendWritePropMultError::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendWritePropMultError::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWritePropMultError)
	//}}AFX_DATA_MAP

	m_InvokeID.UpdateData( pDX->m_bSaveAndValidate );
	m_ServiceCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ErrorClassCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ErrorCodeCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_PropertyID.UpdateData( pDX->m_bSaveAndValidate );
	m_Index.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendWritePropMultError, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWritePropMultError)
	ON_EN_CHANGE(IDC_INVOKEID, OnChangeInvokeID)
	ON_CBN_SELCHANGE(IDC_SERVICECOMBO, OnSelchangeServiceCombo)
	ON_CBN_SELCHANGE(IDC_ERRORCLASSCOMBO, OnSelchangeErrorClassCombo)
	ON_CBN_SELCHANGE(IDC_ERRORCODECOMBO, OnSelchangeErrorCodeCombo)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_EN_CHANGE(IDC_PROPERTYID, OnChangePropertyID)
	ON_EN_CHANGE(IDC_INDEX, OnChangeIndex)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWritePropMultError::OnInitDialog
//

BOOL CSendWritePropMultError::OnInitDialog() 
{
	TRACE0( "CSendWritePropMultError::OnInitDialog()\n" );

	CPropertyPage::OnInitDialog();
	
	// load the enumeration tables
	m_ServiceCombo.LoadCombo();
	m_ErrorClassCombo.LoadCombo();
	m_ErrorCodeCombo.LoadCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendWritePropMultError::InitPage
//

void CSendWritePropMultError::InitPage( void )
{
	TRACE0( "CSendWritePropMultError::InitPage()\n" );

	// flush the data
	m_InvokeID.ctrlNull = true;

	m_ServiceCombo.ctrlNull = false;
	m_ServiceCombo.m_enumValue = 0;

	m_ErrorClassCombo.ctrlNull = false;
	m_ErrorClassCombo.m_enumValue = 0;

	m_ErrorCodeCombo.ctrlNull = false;
	m_ErrorCodeCombo.m_enumValue = 0;
}

//
//	CSendWritePropMultError::EncodePage
//

void CSendWritePropMultError::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendWritePropMultError::EncodePage()\n" );

	// encode the pdu type
	header.Add( 0x50 );

	// encode the invoke ID
	if (m_InvokeID.ctrlNull)
		throw "Original invoke ID required";
	if ((m_InvokeID.intValue < 0) || (m_InvokeID.intValue > 255))
		throw "Original invoke ID out of range 0..255";
	header.Add( m_InvokeID.intValue );

	// encode the service choice
	if (m_ServiceCombo.ctrlNull)
		throw "Service ACK choice required";
	if (m_ServiceCombo.m_enumValue != 16)
		throw "Service must be writePropertyMultiple";
	header.Add( m_ServiceCombo.m_enumValue );

	// opening tag
	BACnetOpeningTag().Encode( enc, 0 );

	// encode the error class
	if (m_ErrorClassCombo.ctrlNull)
		throw "Error class required";
	m_ErrorClassCombo.Encode( enc );

	// encode the error code
	if (m_ErrorCodeCombo.ctrlNull)
		throw "Error code required";
	m_ErrorCodeCombo.Encode( enc );

	// closing tag
	BACnetClosingTag().Encode( enc, 0 );

	// opening tag
	BACnetOpeningTag().Encode( enc, 1 );

	// encode the object identifier
	if (m_ObjectID.ctrlNull)
		throw "Object identifier required";
	m_ObjectID.Encode( enc, 0 );

	// encode the property identifier
	if (m_PropertyID.ctrlNull)
		throw "Property identifier required";
	m_PropertyID.Encode( enc, 1 );

	// encode the index (optional)
	if (!m_Index.ctrlNull)
		m_Index.Encode( enc, 2 );

	// closing tag
	BACnetClosingTag().Encode( enc, 1 );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendWritePropMultError::SavePage
//

void CSendWritePropMultError::SavePage( void )
{
	TRACE0( "CSendWritePropMultError::SavePage\n" );

	pageContents[curHistoryIndex].Flush();

	m_InvokeID.SaveCtrl( pageContents[curHistoryIndex] );
	m_ServiceCombo.SaveCtrl( pageContents[curHistoryIndex] );
	m_ErrorClassCombo.SaveCtrl( pageContents[curHistoryIndex] );
	m_ErrorCodeCombo.SaveCtrl( pageContents[curHistoryIndex] );
	m_ObjectID.SaveCtrl( pageContents[curHistoryIndex] );
	m_PropertyID.SaveCtrl( pageContents[curHistoryIndex] );
	m_Index.SaveCtrl( pageContents[curHistoryIndex] );
}

//
//	CSendWritePropMultError::RestorePage
//

void CSendWritePropMultError::RestorePage( int index )
{
	TRACE0( "CSendWritePropMultError::RestorePage\n" );

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

	m_InvokeID.RestoreCtrl( dec );
	m_ServiceCombo.RestoreCtrl( dec );
	m_ErrorClassCombo.RestoreCtrl( dec );
	m_ErrorCodeCombo.RestoreCtrl( dec );
	m_ObjectID.RestoreCtrl( dec );
	m_PropertyID.RestoreCtrl( dec );
	m_Index.RestoreCtrl( dec );

	if(isShown)
	{
		//update data to controls
		m_InvokeID.ObjToCtrl();
		m_ServiceCombo.ObjToCtrl();
		m_ErrorClassCombo.ObjToCtrl();
		m_ErrorCodeCombo.ObjToCtrl();
		m_ObjectID.ObjToCtrl();
		m_PropertyID.ObjToCtrl();
		m_Index.ObjToCtrl();
	}
}

//
//	CSendWritePropMultError::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendWritePropMultError::OnChangeInvokeID()
{
	m_InvokeID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWritePropMultError::OnSelchangeServiceCombo() 
{
	m_ServiceCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWritePropMultError::OnSelchangeErrorClassCombo() 
{
	m_ErrorClassCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWritePropMultError::OnSelchangeErrorCodeCombo() 
{
	m_ErrorCodeCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWritePropMultError::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWritePropMultError::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// for proper parent control
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

void CSendWritePropMultError::OnChangePropertyID()
{
	m_PropertyID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWritePropMultError::OnChangeIndex()
{
	m_Index.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWritePropMultError::OnShowWindow(BOOL bShow, UINT nStatus) 
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

void CSendWritePropMultError::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	if(historyCount < glMaxHistoryCount)
		historyCount++;

	curHistoryIndex++;
	
	if(curHistoryIndex > glMaxHistoryCount - 1)
		curHistoryIndex = 0;
}
