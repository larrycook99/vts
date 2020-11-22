// SendWriteProp.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendWriteProp.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendWriteProp::pageContents[glMaxHistoryCount];
int CSendWriteProp::historyCount = 0;           //Xiao Shiyuan 2002-12-5
int CSendWriteProp::curHistoryIndex = 0;        //Xiao Shiyuan 2002-12-5
/////////////////////////////////////////////////////////////////////////////
// CSendWriteProp dialog

IMPLEMENT_DYNCREATE( CSendWriteProp, CPropertyPage )

#pragma warning( disable : 4355 )
CSendWriteProp::CSendWriteProp( void )
	: CSendPage( CSendWriteProp::IDD )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_PropCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_Priority( this, IDC_PRIORITYX )
	, m_Value(this)			// for proper parent control
{
	//{{AFX_DATA_INIT(CSendWriteProp)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendWriteProp::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendWriteProp::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWriteProp)
	//}}AFX_DATA_MAP

	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_PropCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	m_Priority.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendWriteProp, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWriteProp)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_EN_CHANGE(IDC_PRIORITYX, OnChangePriority)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWriteProp::OnInitDialog
//

BOOL CSendWriteProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_PropCombo.LoadCombo();
	
	return TRUE;
}

//
//	CSendWriteProp::InitPage
//

void CSendWriteProp::InitPage( void )
{
	// flush the data
	m_ObjectID.ctrlNull = true;
	m_PropCombo.ctrlNull = true;
	m_ArrayIndex.ctrlNull = true;
}

//
//	CSendWriteProp::EncodePage
//

void CSendWriteProp::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 15 );

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

	// do the value
	// Allow empty value: some properties are List-Of, with empty list allowed
//	if (m_Value.m_anyList.Length() < 1)
//		throw "Value required";
	m_Value.Encode( enc, 3 );

	// encode the (optional) priority
	if (!m_Priority.ctrlNull) {
		if ((m_Priority.uintValue < 1) || (m_Priority.uintValue > 16))
			throw "Priority out of range 1..16";
		m_Priority.Encode( enc, 4 );
	}

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendWriteProp::SavePage
//

void CSendWriteProp::SavePage( void )
{
	TRACE0( "CSendWriteProp::SavePage\n" );

	pageContents[curHistoryIndex].Flush();

	m_ObjectID.SaveCtrl( pageContents[curHistoryIndex] );
	m_PropCombo.SaveCtrl( pageContents[curHistoryIndex] );
	m_ArrayIndex.SaveCtrl( pageContents[curHistoryIndex] );
	m_Value.SaveCtrl( pageContents[curHistoryIndex] );
	m_Priority.SaveCtrl( pageContents[curHistoryIndex] );
}

//
//	CSendWriteProp::RestorePage
//

void CSendWriteProp::RestorePage( int index )
{
	TRACE0( "CSendWriteProp::RestorePage\n" );

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
	m_Value.RestoreCtrl( dec );
	m_Priority.RestoreCtrl( dec );

	if(isShown)
	{
		m_ObjectID.ObjToCtrl();
		m_PropCombo.ObjToCtrl();
		m_ArrayIndex.ObjToCtrl();		
		m_Priority.ObjToCtrl();
	}
}

//
//	CSendWriteProp::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendWriteProp::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteProp::OnObjectIDButton() 
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

void CSendWriteProp::OnSelchangePropCombo()
{
	m_PropCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteProp::OnChangeArrayIndex()
{
	m_ArrayIndex.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteProp::OnValue() 
{
	m_Value.DoModal();
	SavePage();
	UpdateEncoded();
}

void CSendWriteProp::OnChangePriority()
{
	m_Priority.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteProp::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	if(historyCount < glMaxHistoryCount)
		historyCount++;

	curHistoryIndex++;	

	if(curHistoryIndex > glMaxHistoryCount - 1)
		curHistoryIndex = 0;			
}

void CSendWriteProp::OnShowWindow(BOOL bShow, UINT nStatus) 
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

void CSendWriteProp::OnDropdownPropcombo() 
{
//Added by Zhu Zhenhua , 2003-7-22 
//	to Load Standard property list for certain ObjectType	
	m_PropCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_PropCombo.LoadCombo();	
}


// madanner 9/04, added from EPICS view

void CSendWriteProp::ForceValues(BACnetObjectIdentifier * pObjectID, BACnetEnumerated * pPropID, BACnetEncodeable * pbacnetEncodeable )
{
	m_ObjectID.objID = pObjectID->objID;
	m_ObjectID.ctrlNull = FALSE;
	m_PropCombo.m_enumValue = pPropID->m_enumValue;
	m_PropCombo.ctrlNull = FALSE;
	m_ArrayIndex.ctrlNull = true;
	m_Priority.ctrlNull = true;

	m_ObjectID.ObjToCtrl();
	m_PropCombo.ObjToCtrl();

	m_Value.m_anyList.KillAll();
	if ( pbacnetEncodeable->IsKindOf(RUNTIME_CLASS(BACnetGenericArray)))
	{
		BACnetGenericArray *pbacnetarray = (BACnetGenericArray *)pbacnetEncodeable;
		for ( int i = 0; i < pbacnetarray->GetSize(); i++ )
		{
			BACnetAnyValue pbacnetAny;
			pbacnetAny.SetObject( (*pbacnetarray)[i].clone());	
			m_Value.AddValue(&pbacnetAny);
		}
	}
	else
	{
		m_Value.AddValue(pbacnetEncodeable);
	}

	SavePage();
	UpdateEncoded();
}
