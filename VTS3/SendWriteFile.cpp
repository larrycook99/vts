// SendWriteFile.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendWriteFile.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendWriteFile::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendWriteFile dialog

IMPLEMENT_DYNCREATE( CSendWriteFile, CPropertyPage )

#pragma warning( disable : 4355 )
CSendWriteFile::CSendWriteFile( void )
	: CSendPage( CSendWriteFile::IDD )
	, m_FileID( this, IDC_FILEID )
	, m_StartPosition( this, IDC_STARTPOSITION )
	, m_Data( this, IDC_DATA )
	, m_StartRecord( this, IDC_STARTRECORD )
{
	//{{AFX_DATA_INIT(CSendWriteFile)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void CSendWriteFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWriteFile)
	DDX_Control(pDX, IDC_RECORDLIST, m_RecordList);
	//}}AFX_DATA_MAP

	m_FileID.UpdateData( pDX->m_bSaveAndValidate );
	m_StartPosition.UpdateData( pDX->m_bSaveAndValidate );
	m_Data.UpdateData( pDX->m_bSaveAndValidate );
	m_StartRecord.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendWriteFile, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWriteFile)
	ON_EN_CHANGE(IDC_FILEID, OnChangeFileID)
	ON_EN_CHANGE(IDC_STARTPOSITION, OnChangeStartPosition)
	ON_EN_CHANGE(IDC_DATA, OnChangeData)
	ON_EN_CHANGE(IDC_STARTRECORD, OnChangeStartRecord)
	ON_BN_CLICKED(IDC_ADDRECORD, OnAddRecord)
	ON_BN_CLICKED(IDC_REMOVERECORD, OnRemoveRecord)
	ON_EN_CHANGE(IDC_RECORDDATA, OnChangeRecordData)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_RECORDLIST, OnItemchangingRecordList)
	ON_BN_CLICKED(IDC_FILEIDBTN, OnFileIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWriteFile::InitPage
//

void CSendWriteFile::InitPage( void )
{
	TRACE0( "CSendWriteFile::InitPage()\n" );
}

//
//	CSendWriteFile::EncodePage
//

void CSendWriteFile::EncodePage( CByteArray* contents )
{
	int					len = m_RecordCtrl.GetItemCount()
	;
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 7 );

	// encode the object ID
	if (m_FileID.ctrlNull)
		throw "File ID required";
	m_FileID.Encode( enc );

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
//	CSendWriteFile::SavePage
//

void CSendWriteFile::SavePage( void )
{
	TRACE0( "CSendWriteFile::SavePage\n" );

	pageContents.Flush();

	m_FileID.SaveCtrl( pageContents );
	m_StartPosition.SaveCtrl( pageContents );
	m_Data.SaveCtrl( pageContents );
	m_StartRecord.SaveCtrl( pageContents );
//	m_RecordCtrl.SaveCtrl( pageContents );
}

//
//	CSendWriteFile::RestorePage
//

void CSendWriteFile::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendWriteFile::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_FileID.RestoreCtrl( dec );
	m_StartPosition.RestoreCtrl( dec );
	m_Data.RestoreCtrl( dec );
	m_StartRecord.RestoreCtrl( dec );
//	m_RecordCtrl.RestoreCtrl( dec );
}

//
//	CSendWriteFile::OnInitDialog
//

BOOL CSendWriteFile::OnInitDialog() 
{
	static VTSListColDesc colDesc[] =
		{ { "Record Data", LVCFMT_RIGHT, 256, IDC_RECORDDATA }
		, { 0, 0, 0, 0 }
		};

	TRACE0( "CSendWriteFile::OnInitDialog()\n" );

	CDialog::OnInitDialog();

	// init the usual fields
	m_FileID.ctrlNull = true;
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
//	CSendWriteFile::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendWriteFile::OnChangeFileID()
{
	m_FileID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteFile::OnFileIDButton() 
{
	VTSObjectIdentifierDialog	dlg(this)			// for proper parent control
	;

	dlg.objID = m_FileID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_FileID.ctrlNull = false;
		m_FileID.objID = dlg.objID;
		m_FileID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendWriteFile::OnChangeStartPosition()
{
	m_StartPosition.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteFile::OnChangeData()
{
	m_Data.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteFile::OnChangeStartRecord()
{
	m_StartRecord.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteFile::OnAddRecord() 
{
	m_RecordCtrl.AddButtonClick();

	// madanner 9/3/02, added default record data
	m_RecordCtrl.SetCtrlText(IDC_RECORDDATA, "(record data)");
}

void CSendWriteFile::OnRemoveRecord() 
{
	m_RecordCtrl.RemoveButtonClick();
}

void CSendWriteFile::OnChangeRecordData() 
{
	m_RecordCtrl.OnChangeItem( IDC_RECORDDATA );
}

void CSendWriteFile::OnItemchangingRecordList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_RecordCtrl.OnItemChanging( pNMHDR, pResult );
}
