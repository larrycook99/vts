// SendReadFile.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendReadFile.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BACnetAPDUEncoder CSendReadFile::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendReadFile dialog

IMPLEMENT_DYNCREATE( CSendReadFile, CPropertyPage )

#pragma warning( disable : 4355 )
CSendReadFile::CSendReadFile( void )
	: CSendPage( CSendReadFile::IDD )
	, m_FileID( this, IDC_FILEID )
	, m_StartPosition( this, IDC_STARTPOSITION )
	, m_OctetCount( this, IDC_OCTETCOUNT )
	, m_StartRecord( this, IDC_STARTRECORD )
	, m_RecordCount( this, IDC_RECORDCOUNT )
{
	//{{AFX_DATA_INIT(CSendReadFile)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendReadFile::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendReadFile::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReadFile)
	//}}AFX_DATA_MAP

	m_FileID.UpdateData( pDX->m_bSaveAndValidate );
	m_StartPosition.UpdateData( pDX->m_bSaveAndValidate );
	m_OctetCount.UpdateData( pDX->m_bSaveAndValidate );
	m_StartRecord.UpdateData( pDX->m_bSaveAndValidate );
	m_RecordCount.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendReadFile, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReadFile)
	ON_EN_CHANGE(IDC_FILEID, OnChangeFileID)
	ON_EN_CHANGE(IDC_STARTPOSITION, OnChangeStartPosition)
	ON_EN_CHANGE(IDC_OCTETCOUNT, OnChangeOctetCount)
	ON_EN_CHANGE(IDC_STARTRECORD, OnChangeStartRecord)
	ON_EN_CHANGE(IDC_RECORDCOUNT, OnChangeRecordCount)
	ON_BN_CLICKED(IDC_FILEIDBTN, OnFileIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReadFile::OnInitDialog
//

BOOL CSendReadFile::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

//
//	CSendReadFile::InitPage
//

void CSendReadFile::InitPage( void )
{
	// flush the data
	m_FileID.ctrlNull = true;
	m_StartPosition.ctrlNull = true;
	m_OctetCount.ctrlNull = true;
	m_StartRecord.ctrlNull = true;
	m_RecordCount.ctrlNull = true;
}

//
//	CSendReadFile::EncodePage
//

void CSendReadFile::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 6 );

	// encode the object ID
	if (m_FileID.ctrlNull)
		throw "File ID required";
	m_FileID.Encode( enc );

	// check for access type
	if (!m_StartPosition.ctrlNull || !m_OctetCount.ctrlNull) {
		if (m_StartPosition.ctrlNull)
			throw "Start position required";
		if (m_OctetCount.ctrlNull)
			throw "Octet count required";
		if (!m_StartRecord.ctrlNull)
			throw "Omit start record for stream access";
		if (!m_RecordCount.ctrlNull)
			throw "Omit record count for stream access";

		BACnetOpeningTag().Encode( enc, 0 );
		m_StartPosition.Encode( enc );
		m_OctetCount.Encode( enc );
		BACnetClosingTag().Encode( enc, 0 );
	} else
	if (!m_StartRecord.ctrlNull || !m_RecordCount.ctrlNull) {
		if (m_StartRecord.ctrlNull)
			throw "Start record required";
		if (m_RecordCount.ctrlNull)
			throw "Record count required";
		if (!m_StartPosition.ctrlNull)
			throw "Omit start position for record access";
		if (!m_OctetCount.ctrlNull)
			throw "Omit octet count for record access";

		BACnetOpeningTag().Encode( enc, 1 );
		m_StartRecord.Encode( enc );
		m_RecordCount.Encode( enc );
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
//	CSendReadFile::SavePage
//

void CSendReadFile::SavePage( void )
{
	TRACE0( "CSendReadFile::SavePage\n" );

	pageContents.Flush();

	m_FileID.SaveCtrl( pageContents );
	m_StartPosition.SaveCtrl( pageContents );
	m_OctetCount.SaveCtrl( pageContents );
	m_StartRecord.SaveCtrl( pageContents );
	m_RecordCount.SaveCtrl( pageContents );
}

//
//	CSendReadFile::RestorePage
//

void CSendReadFile::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendReadFile::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_FileID.RestoreCtrl( dec );
	m_StartPosition.RestoreCtrl( dec );
	m_OctetCount.RestoreCtrl( dec );
	m_StartRecord.RestoreCtrl( dec );
	m_RecordCount.RestoreCtrl( dec );
}

//
//	CSendReadFile::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendReadFile::OnChangeFileID()
{
	m_FileID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadFile::OnFileIDButton() 
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

void CSendReadFile::OnChangeStartPosition()
{
	m_StartPosition.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadFile::OnChangeOctetCount()
{
	m_OctetCount.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadFile::OnChangeStartRecord()
{
	m_StartRecord.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendReadFile::OnChangeRecordCount()
{
	m_RecordCount.UpdateData();
	SavePage();
	UpdateEncoded();
}
