// VTSTimeStamp.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "VTSTimeStamp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSTimeStamp dialog

#pragma warning( disable : 4355 )
VTSTimeStamp::VTSTimeStamp(CWnd* pParent /*=NULL*/)
	: CDialog(VTSTimeStamp::IDD, pParent)
	, m_Time( this, IDC_TIME )
	, m_SeqNum( this, IDC_SEQNUM )
	, m_DTDate( this, IDC_DTDATE )
	, m_DTTime( this, IDC_DTTIME )
{
	//{{AFX_DATA_INIT(VTSTimeStamp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void VTSTimeStamp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSTimeStamp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	m_Time.UpdateData( pDX->m_bSaveAndValidate );
	m_SeqNum.UpdateData( pDX->m_bSaveAndValidate );
	m_DTDate.UpdateData( pDX->m_bSaveAndValidate );
	m_DTTime.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(VTSTimeStamp, CDialog)
	//{{AFX_MSG_MAP(VTSTimeStamp)
	ON_EN_CHANGE(IDC_TIME, OnChangeTime)
	ON_EN_CHANGE(IDC_SEQNUM, OnChangeSeqNum)
	ON_EN_CHANGE(IDC_DTDATE, OnChangeDTDate)
	ON_EN_CHANGE(IDC_DTTIME, OnChangeDTTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSTimeStamp::Encode
//

void VTSTimeStamp::Encode( BACnetAPDUEncoder& enc, int context )
{
	// start out with a group opening tag
	BACnetOpeningTag().Encode( enc, context );

	// encode the time
	if (!m_Time.ctrlNull) {
		if (!m_SeqNum.ctrlNull)
			throw "Omit sequence number when providing time";
		if (!m_DTDate.ctrlNull || !m_DTTime.ctrlNull)
			throw "Omit date/time when providing time";

		m_Time.Encode( enc, 0 );
	} else

	// encode the sequence number
	if (!m_SeqNum.ctrlNull) {
		if (!m_DTDate.ctrlNull || !m_DTTime.ctrlNull)
			throw "Omit date/time when providing a sequence number";

		m_SeqNum.Encode( enc, 1 );
	} else

	// encode the date/time
	if (!m_DTDate.ctrlNull || !m_DTTime.ctrlNull) {
		if (m_DTDate.ctrlNull)
			throw "Date required when providing date/time";
		if (m_DTTime.ctrlNull)
			throw "Time required when providing date/time";

		// the BACnetDateTime needs a group
		BACnetOpeningTag().Encode( enc, 2 );
		m_DTDate.Encode( enc );
		m_DTTime.Encode( enc );
		BACnetClosingTag().Encode( enc, 2 );
	} else
		throw "Time, sequence number or date/time required";

	// close the group
	BACnetClosingTag().Encode( enc, context );
}

//
//	VTSTimeStamp::SaveCtrl
//

void VTSTimeStamp::SaveCtrl( BACnetAPDUEncoder& enc )
{
	TRACE0( "VTSTimeStamp::SaveCtrl\n" );

	m_Time.SaveCtrl( enc );
	m_SeqNum.SaveCtrl( enc );
	m_DTDate.SaveCtrl( enc );
	m_DTTime.SaveCtrl( enc );
}

//
//	VTSTimeStamp::RestoreCtrl
//

void VTSTimeStamp::RestoreCtrl( BACnetAPDUDecoder& dec )
{
	BACnetAPDUTag	tag
	;

	TRACE0( "VTSTimeStamp::RestoreCtrl\n" );

	m_Time.RestoreCtrl( dec );
	m_SeqNum.RestoreCtrl( dec );
	m_DTDate.RestoreCtrl( dec );
	m_DTTime.RestoreCtrl( dec );
}

/////////////////////////////////////////////////////////////////////////////
// VTSTimeStamp message handlers

void VTSTimeStamp::OnChangeTime() 
{
	m_Time.UpdateData();
}

void VTSTimeStamp::OnChangeSeqNum() 
{
	m_SeqNum.UpdateData();
}

void VTSTimeStamp::OnChangeDTDate() 
{
	m_DTDate.UpdateData();
}

void VTSTimeStamp::OnChangeDTTime() 
{
	m_DTTime.UpdateData();
}
