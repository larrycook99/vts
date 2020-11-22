// SendReadFileACK.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendReadFileACK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendReadFileACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendReadFileACK dialog

IMPLEMENT_DYNCREATE( CSendReadFileACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendReadFileACK::CSendReadFileACK( void )
	: CSendPage( CSendReadFileACK::IDD )
	, m_EOF( this, IDC_EOF, true )
	, m_StartPosition( this, IDC_STARTPOSITION )
	, m_Data( this, IDC_DATA )
	, m_StartRecord( this, IDC_STARTRECORD )
{
	//{{AFX_DATA_INIT(CSendReadFileACK)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void CSendReadFileACK::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReadFileACK)
	DDX_Control(pDX, IDC_RECORDLIST, m_RecordList);
	//}}AFX_DATA_MAP

	m_EOF.UpdateData( pDX->m_bSaveAndValidate );
	m_StartPosition.UpdateData( pDX->m_bSaveAndValidate );
	m_Data.UpdateData( pDX->m_bSaveAndValidate );
	m_StartRecord.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendReadFileACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReadFileACK)
	ON_BN_CLICKED(IDC_EOF, OnEOF)
	ON_EN_CHANGE(IDC_STARTPOSITION, OnChangeStartPosition)
	ON_EN_CHANGE(IDC_DATA, OnChangeData)
	ON_EN_CHANGE(IDC_STARTRECORD, OnChangeStartRecord)
	ON_BN_CLICKED(IDC_ADDRECORD, OnAddRecord)
	ON_BN_CLICKED(IDC_REMOVERECORD, OnRemoveRecord)
	ON_EN_CHANGE(IDC_RECORDDATA, OnChangeRecordData)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_RECORDLIST, OnItemchangingRecordList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReadFileACK::InitPage
//

void CSendReadFileACK::InitPage( void )
{
	TRACE0( "CSendReadFileACK::InitPage()\n" );
}

//
//	CSendReadFileACK::EncodePage
//

void CSendReadFileACK::EncodePage( CByteArray* contents )
{
	int					len = m_RecordCtrl.GetItemCount()
	;
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 6 );

	// encode the end-of-file
	m_EOF.Encode( enc );

	// check for access type
	if (!m_StartPosition.ctrlNull || !m_Data.ctrlNull) {
		if (m_StartPosition.ctrlNull)
			throw "Start position required";
		if (m_Data.ctrlNull)
			throw "Octet string required";
		if (!m_StartRecord.ctrlNull)
			throw "Omit start record for stream access";
		if (len != 0)
			throw "Omit record data for stream access";

		BACnetOpeningTag().Encode( enc, 0 );
		m_StartPosition.Encode( enc );
		m_Data.Encode( enc );
		BACnetClosingTag().Encode( enc, 0 );
	} else
	if (!m_StartRecord.ctrlNull || (len != 0)) {
		if (m_StartRecord.ctrlNull)
			throw "Start record required";
		if (!m_StartPosition.ctrlNull)
			throw "Omit start position for record access";
		if (!m_Data.ctrlNull)
			throw "Omit octet string for record access";

		BACnetOpeningTag().Encode( enc, 1 );
		m_StartRecord.Encode( enc );

		// encode the record count
		BACnetUnsigned( len ).Encode( enc );

		// validate and encode the record data
		for (int i = 0; i < len; i++) {
			BACnetOctetString	ostr
			;
			const char			*txt = m_RecordCtrl.GetItemText( i, 0 )
			;

			// make sure something was provided
			if (!txt || !*txt)
				throw "Invalid record data";

			// encode it
			int			upperNibble, lowerNibble
			;
			char		c
			;

			// translate the text into octets
			for (;;) {
				// look for a hex digit
				while ((c = ToUpper(*txt++)) && !IsXDigit(c))
					;
				if (!c) break;
				upperNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

				// look for another hex digit
				while ((c = ToUpper(*txt++)) && !IsXDigit(c))
					;
				if (!c) break;
				lowerNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

				// add the byte
				ostr.Append( (upperNibble << 4) + lowerNibble );
			}

			// encode the object
			ostr.Encode( enc );
		}
		BACnetClosingTag().Encode( enc, 1 );
	} else
		throw "Stream or record access required";

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendReadFileACK::SavePage
//

void CSendReadFileACK::SavePage( void )
{
	TRACE0( "CSendReadFileACK::SavePage\n" );

	pageContents.Flush();

	m_EOF.SaveCtrl( pageContents );
	m_StartPosition.SaveCtrl( pageContents );
	m_Data.SaveCtrl( pageContents );
	m_StartRecord.SaveCtrl( pageContents );
//	m_RecordCtrl.SaveCtrl( pageContents );
}

//
//	CSendReadFileACK::RestorePage
//

void CSendReadFileACK::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendReadFileACK::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_EOF.RestoreCtrl( dec );
	m_StartPosition.RestoreCtrl( dec );
	m_Data.RestoreCtrl( dec );
	m_StartRecord.RestoreCtrl( dec );
//	m_RecordCtrl.RestoreCtrl( dec );
}

//
//	CSendReadFileACK::OnInitDialog
//

BOOL CSendReadFileACK::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "Record Data", LVCFMT_RIGHT, 256, IDC_RECORDDATA }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendReadFileACK::OnInitDialog()\n" );

	CDialog::OnInitDialog();

	// init the usual fields
	m_EOF.ctrlNull = false;
	m_EOF.boolValue = BACnetBoolean::bFalse;
	m_StartPosition.ctrlNull = true;
	m_Data.ctrlNull = true;
	m_StartRecord.ctrlNull = true;
	
	// only allow one selection at a time, no sorting
	m_RecordList.m_nFlags |= LVS_SINGLESEL;
	m_RecordList.m_nFlags &= ~LBS_SORT;

	// initialize the list
	m_RecordCtrl.Init( this, &m_RecordList, colDesc );

	return TRUE;
}

//
//	CSendReadFileACK::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendReadFileACK::OnEOF()
{
	m_EOF.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadFileACK::OnChangeStartPosition()
{
	m_StartPosition.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadFileACK::OnChangeData()
{
	m_Data.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadFileACK::OnChangeStartRecord()
{
	m_StartRecord.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadFileACK::OnAddRecord() 
{
	m_RecordCtrl.AddButtonClick();

	// madanner 9/3/02, added default record data
	m_RecordCtrl.SetCtrlText(IDC_RECORDDATA, "(record data)");
}

void CSendReadFileACK::OnRemoveRecord() 
{
	m_RecordCtrl.RemoveButtonClick();
}

void CSendReadFileACK::OnChangeRecordData() 
{
	m_RecordCtrl.OnChangeItem( IDC_RECORDDATA );
}

void CSendReadFileACK::OnItemchangingRecordList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_RecordCtrl.OnItemChanging( pNMHDR, pResult );
}
