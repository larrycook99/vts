// SendTest.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "SendTest.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendTest dialog

IMPLEMENT_DYNCREATE( CSendTest, CPropertyPage )

#pragma warning( disable : 4355 )
CSendTest::CSendTest( void )
	: CSendPage( CSendTest::IDD )
	, m_Boolean( this, IDC_BOOLEAN )
	, m_BooleanCheck( this, IDC_BOOLEANCHECK, true )
	, m_Enumerated( this, IDC_ENUMERATED, NetworkSniffer::BAC_STRTAB_BACnetEngineeringUnits )
	, m_EnumeratedCombo( this, IDC_ENUMERATEDCOMBO, NetworkSniffer::BAC_STRTAB_BACnetEngineeringUnits, true )
	, m_Unsigned( this, IDC_UNSIGNED )
	, m_Integer( this, IDC_INTEGER )
	, m_Real( this, IDC_REAL )
	, m_Double( this, IDC_DOUBLE )
	, m_CharStr( this, IDC_CHARSTR )
	, m_OctetStr( this, IDC_OCTETSTR )
	, m_BitStr( this, IDC_BITSTR )
	, m_Date( this, IDC_DATE )
	, m_Time( this, IDC_TIME )
	, m_ObjID( this, IDC_OBJID )
{
	//{{AFX_DATA_INIT(CSendTest)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendTest::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendTest::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendTest)
	//}}AFX_DATA_MAP

	m_Boolean.UpdateData( pDX->m_bSaveAndValidate );
	m_BooleanCheck.UpdateData( pDX->m_bSaveAndValidate );
	m_Enumerated.UpdateData( pDX->m_bSaveAndValidate );
	m_EnumeratedCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_Unsigned.UpdateData( pDX->m_bSaveAndValidate );
	m_Integer.UpdateData( pDX->m_bSaveAndValidate );
	m_Real.UpdateData( pDX->m_bSaveAndValidate );
	m_Double.UpdateData( pDX->m_bSaveAndValidate );
	m_CharStr.UpdateData( pDX->m_bSaveAndValidate );
	m_OctetStr.UpdateData( pDX->m_bSaveAndValidate );
	m_BitStr.UpdateData( pDX->m_bSaveAndValidate );
	m_Date.UpdateData( pDX->m_bSaveAndValidate );
	m_Time.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjID.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendTest, CPropertyPage)
	//{{AFX_MSG_MAP(CSendTest)
	ON_EN_CHANGE(IDC_BOOLEAN, OnChangeBoolean)
	ON_BN_CLICKED(IDC_BOOLEANCHECK, OnBooleanCheck)
	ON_EN_CHANGE(IDC_ENUMERATED, OnChangeEnumerated)
	ON_CBN_SELCHANGE(IDC_ENUMERATEDCOMBO, OnSelchangeEnumeratedCombo)
	ON_EN_CHANGE(IDC_UNSIGNED, OnChangeUnsigned)
	ON_EN_CHANGE(IDC_INTEGER, OnChangeInteger)
	ON_EN_CHANGE(IDC_REAL, OnChangeReal)
	ON_EN_CHANGE(IDC_DOUBLE, OnChangeDouble)
	ON_EN_CHANGE(IDC_CHARSTR, OnChangeCharStr)
	ON_EN_CHANGE(IDC_OCTETSTR, OnChangeOctetStr)
	ON_EN_CHANGE(IDC_BITSTR, OnChangeBitStr)
	ON_EN_CHANGE(IDC_DATE, OnChangeDate)
	ON_EN_CHANGE(IDC_TIME, OnChangeTime)
	ON_EN_CHANGE(IDC_OBJID, OnChangeObjID)
	ON_BN_CLICKED(IDC_ANY, OnAny)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendTest::InitPage
//

void CSendTest::InitPage( void )
{
	TRACE0( "CSendTest::InitPage()\n" );

	// flush the data
	m_Boolean.ctrlNull = true;
	m_BooleanCheck.ctrlNull = true;
	m_Enumerated.ctrlNull = true;
	m_EnumeratedCombo.ctrlNull = true;
	m_Unsigned.ctrlNull = true;
	m_Integer.ctrlNull = true;
	m_Real.ctrlNull = true;
	m_Double.ctrlNull = true;
	m_CharStr.ctrlNull = true;
	m_OctetStr.ctrlNull = true;
	m_BitStr.ctrlNull = true;
	m_Date.ctrlNull = true;
	m_Time.ctrlNull = true;
	m_ObjID.ctrlNull = true;
}

//
//	CSendTest::EncodePage
//

void CSendTest::EncodePage( CByteArray* contents )
{
	BACnetAPDUEncoder	enc
	;

	TRACE0( "CSendTest::EncodePage()\n" );

	if (!m_Boolean.ctrlNull)
		m_Boolean.Encode( enc );
	if (!m_BooleanCheck.ctrlNull)
		m_BooleanCheck.Encode( enc );
	if (!m_Enumerated.ctrlNull) {
		if (m_Enumerated.m_enumValue < 0)
			throw "Enumeration error";
		else
			m_Enumerated.Encode( enc );
	}
	if (!m_EnumeratedCombo.ctrlNull) {
		if (m_EnumeratedCombo.m_enumValue < 0)
			throw "Enumeration Combo error";
		else
			m_EnumeratedCombo.Encode( enc );
	}
	if (!m_Unsigned.ctrlNull)
		m_Unsigned.Encode( enc );
	if (!m_Integer.ctrlNull)
		m_Integer.Encode( enc );
	if (!m_Real.ctrlNull)
		m_Real.Encode( enc );
	if (!m_Double.ctrlNull)
		m_Double.Encode( enc );
	if (!m_CharStr.ctrlNull)
		m_CharStr.Encode( enc );
	if (!m_OctetStr.ctrlNull)
		m_OctetStr.Encode( enc );
	if (!m_BitStr.ctrlNull)
		m_BitStr.Encode( enc );
	if (!m_Date.ctrlNull)
		m_Date.Encode( enc );
	if (!m_Time.ctrlNull)
		m_Time.Encode( enc );
	if (!m_ObjID.ctrlNull)
		m_ObjID.Encode( enc );

	// do the any
	for (int j = 0; j < m_Any.m_anyList.Length(); j++) {
		BACnetAPDUEncoderPtr ep = &m_Any.m_anyList[j]->elemEncoder;
		enc.Append( ep->pktBuffer, ep->pktLength );
	}

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		contents->Add( enc.pktBuffer[i] );
}

//
//	CSendTest::OnInitDialog
//

BOOL CSendTest::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_EnumeratedCombo.LoadCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
//	CSendTest::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendTest::OnChangeBoolean()
{
	TRACE0( "CSendTest::OnChangeBoolean()\n" );
	m_Boolean.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnBooleanCheck()
{
	TRACE0( "CSendTest::OnBooleanCheck()\n" );
	m_BooleanCheck.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeEnumerated()
{
	TRACE0( "CSendTest::OnChangeEnumerated()\n" );
	m_Enumerated.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnSelchangeEnumeratedCombo() 
{
	TRACE0( "CSendTest::OnSelchangeEnumeratedCombo()\n" );
	m_EnumeratedCombo.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeUnsigned()
{
	TRACE0( "CSendTest::OnChangeUnsigned()\n" );
	m_Unsigned.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeInteger()
{
	TRACE0( "CSendTest::OnChangeInteger()\n" );
	m_Integer.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeReal()
{
	TRACE0( "CSendTest::OnChangeReal()\n" );
	m_Real.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeDouble()
{
	TRACE0( "CSendTest::OnChangeDouble()\n" );
	m_Double.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeCharStr()
{
	TRACE0( "CSendTest::OnChangeCharStr()\n" );
	m_CharStr.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeOctetStr()
{
	TRACE0( "CSendTest::OnChangeOctetStr()\n" );
	m_OctetStr.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeBitStr()
{
	TRACE0( "CSendTest::OnChangeBitStr()\n" );
	m_BitStr.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeDate()
{
	TRACE0( "CSendTest::OnChangeDate()\n" );
	m_Date.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeTime()
{
	TRACE0( "CSendTest::OnChangeTime()\n" );
	m_Time.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnChangeObjID()
{
	TRACE0( "CSendTest::OnChangeObjID()\n" );
	m_ObjID.UpdateData();
	UpdateEncoded();
}

void CSendTest::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg
	;

	dlg.objID = m_ObjID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjID.ctrlNull = false;
		m_ObjID.objID = dlg.objID;
		m_ObjID.ObjToCtrl();

		UpdateEncoded();
	}
}

void CSendTest::OnAny() 
{
	m_Any.DoModal();
	UpdateEncoded();
}
