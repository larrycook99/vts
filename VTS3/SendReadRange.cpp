// SendReadRange.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendReadRange.h"

#include "VTSObjectIdentifierDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace NetworkSniffer 
{
	extern char *BACnetPropertyIdentifier[];
}

BACnetAPDUEncoder SendReadRange::pageContents;

/////////////////////////////////////////////////////////////////////////////
// SendReadRange property page
//Modified by Zhu Zhenhua, 2004-5-22, remove timeRange choice, change the context
// of byTime, Add bySequenceNumber of context 6

// MAG 16AUG05 modify page to fix SourceForge bug #1155180
// -fixed day-of-week parsing
// -added comments that appear when Range:'By Time' is selected (and disappear when it's not)
// -fixed minor layout issues

IMPLEMENT_DYNCREATE(SendReadRange, CPropertyPage)

#pragma warning( disable : 4355 )
SendReadRange::SendReadRange(void)
 : CSendPage(SendReadRange::IDD)
	, m_ObjectID( this, IDC_OBJECTID )
	, m_PropCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
    , m_ReadRangeRefDate( this, IDC_STARTDATUM )
    , m_ReadRangeRefTime( this, IDC_TIMESTART )
    , m_ReadRangeCount( this, IDC_COUNT )
  	, m_ReadRangePosRef( this, IDC_REFINDEX )
	, m_RadioChoice(0)
{
	//{{AFX_DATA_INIT(SendReadRange)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void SendReadRange::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "SendReadRange::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SendReadRange)
	DDX_Radio(pDX, IDC_RADIONONE, m_RadioChoice);
	//}}AFX_DATA_MAP

	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_PropCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );

}

BEGIN_MESSAGE_MAP(SendReadRange, CPropertyPage)
	//{{AFX_MSG_MAP(SendReadRange)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_BN_CLICKED(IDC_RADIONONE, OnRadionone)
	ON_BN_CLICKED(IDC_RADIOPOSITION, OnRadioposition)
	ON_BN_CLICKED(IDC_RADIOTIME, OnRadiotime)
	ON_BN_CLICKED(IDC_RADIOSEQUENCE, OnRadioSequenceNum)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_EN_CHANGE(IDC_STARTDATUM, OnChangeStartdatum)
	ON_EN_SETFOCUS(IDC_STARTDATUM, OnSetfocusStartdatum)
	ON_EN_CHANGE(IDC_TIMESTART, OnChangeTimestart)
	ON_EN_CHANGE(IDC_REFINDEX, OnChangePosRef)
	ON_EN_CHANGE(IDC_COUNT, OnChangeCount)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SendReadRange message handlers

BOOL SendReadRange::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_PropCombo.LoadCombo();

    m_ReadRangeRefDate.Disable();
    m_ReadRangeRefTime.Disable();
	m_ReadRangeCount.Disable();
	m_ReadRangePosRef.Disable();

	
	UpdateData();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//
//	SendReadRange::InitPage
//

void SendReadRange::InitPage( void )
{
	// flush the data
	m_ObjectID.ctrlNull = true;
	m_PropCombo.ctrlNull = true;
	m_ArrayIndex.ctrlNull = true;

}

//
//	SendReadRange::EncodePage
//

void SendReadRange::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 0x1A );

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
	
		
	switch (m_RadioChoice)  // The current selection of the radio button
	{
	case 0:  // nothing selected
		break;

	case 1:
		BACnetOpeningTag().Encode( enc, 3 );

		m_ReadRangePosRef.Encode( enc);
        m_ReadRangeCount.Encode( enc );

		BACnetClosingTag().Encode( enc, 3 );
        break;

	case 2:
		BACnetOpeningTag().Encode( enc, 7 );
		m_ReadRangeRefDate.Encode( enc);
		m_ReadRangeRefTime.Encode( enc);
        m_ReadRangeCount.Encode( enc );
		BACnetClosingTag().Encode( enc, 7 );
		break;
//Modified by Zhu Zhenhua, 2004-5-22
	case 3:
		BACnetOpeningTag().Encode( enc, 6 );

		m_ReadRangePosRef.Encode( enc);
        m_ReadRangeCount.Encode( enc );

		BACnetClosingTag().Encode( enc, 6 );
		break;
	}

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendReadProp::SavePage
//

void SendReadRange::SavePage( void )
{
	TRACE0( "CSendReadProp::SavePage\n" );

	pageContents.Flush();

	m_ObjectID.SaveCtrl( pageContents );
	m_PropCombo.SaveCtrl( pageContents );
	m_ArrayIndex.SaveCtrl( pageContents );

	// MAG 16AUG05 put CalcDayOfWeek in this if block. DayOfWeek was being overwritten, even if specified
	if(m_ReadRangeRefDate.dayOfWeek > 8)
	{
		// 3/24/2005, Shiyuan Xiao 
		m_ReadRangeRefDate.CalcDayOfWeek();
	}
	

    m_ReadRangeRefDate.SaveCtrl( pageContents );
    m_ReadRangeRefTime.SaveCtrl( pageContents );
    m_ReadRangeCount.SaveCtrl( pageContents );
  	m_ReadRangePosRef.SaveCtrl( pageContents );
}

//
//	CSendReadProp::RestorePage
//

void SendReadRange::RestorePage( int index )
{
	BACnetAPDUDecoder	dec( pageContents )
	;
//	BACnetInteger		xType
	;
	TRACE0( "CSendReadProp::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ObjectID.RestoreCtrl( dec );
	m_PropCombo.RestoreCtrl( dec );
	m_ArrayIndex.RestoreCtrl( dec );
/*
	m_ReadRangeRefDate.SaveCtrl( dec );
	m_ReadRangeEndDate.SaveCtrl( dec );
    m_ReadRangeRefTime.SaveCtrl( dec );
	m_ReadRangeEndTime.SaveCtrl( dec );
    m_ReadRangeCount.SaveCtrl( dec );
  	m_ReadRangePosRef.SaveCtrl( dec );
*/
}

////////////////////////////////////////////////////////////////////////
// SendReadRange::OnChangeArrayindex
//
void SendReadRange::OnChangeArrayindex() 
{
	
	m_ArrayIndex.UpdateData();
	SavePage();
	UpdateEncoded();
	
}
////////////////////////////////////////////////////////////////////////
// SendReadRange::OnSelchangePropcombo
//
void SendReadRange::OnSelchangePropcombo() 
{
	m_PropCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}
////////////////////////////////////////////////////////////////////////
// SendReadRange::OnObjectidbtn
//
void SendReadRange::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this)			// for proper parent control
	;

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID)
	{
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

////////////////////////////////////////////////////////////////////////
// SendReadRange::OnChangeStartdatum
//
void SendReadRange::OnChangeStartdatum() 
{
    m_ReadRangeRefDate.UpdateData();
	SavePage();
	UpdateEncoded();
}
////////////////////////////////////////////////////////////////////////
// SendReadRange::OnRadionone
//
void SendReadRange::OnRadionone() 
{
	UpdateData();

    m_ReadRangeRefDate.Disable();
    m_ReadRangeRefTime.Disable();
	m_ReadRangePosRef.Disable();
	m_ReadRangeCount.Disable();
//	GetDlgItem(IDC_DOW1)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_DOW2)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_DOW3)->ShowWindow(SW_HIDE);
	SavePage();
	UpdateEncoded();
	
}
////////////////////////////////////////////////////////////////////////
// SendReadRange::OnRadioposition
//
void SendReadRange::OnRadioposition() 
{
	UpdateData();

	m_ReadRangeRefDate.Disable();
    m_ReadRangeRefTime.Disable();
	m_ReadRangePosRef.Enable();
	m_ReadRangeCount.Enable();
//	GetDlgItem(IDC_DOW1)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_DOW2)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_DOW3)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_DOW4)->ShowWindow(SW_HIDE);
	SavePage();
	UpdateEncoded();
	
}
////////////////////////////////////////////////////////////////////////
// SendReadRange::OnRadiotime
//
void SendReadRange::OnRadiotime() 
{
	UpdateData();

	m_ReadRangeRefDate.Enable();
    m_ReadRangeRefTime.Enable();
	m_ReadRangePosRef.Disable();
	m_ReadRangeCount.Enable();
	SetDlgItemText( IDC_BEGDATECAPTION, "Reference Date and Time");
//	GetDlgItem(IDC_DOW1)->ShowWindow(SW_SHOW);
//	GetDlgItem(IDC_DOW2)->ShowWindow(SW_SHOW);
//	GetDlgItem(IDC_DOW3)->ShowWindow(SW_SHOW);
//	GetDlgItem(IDC_DOW4)->ShowWindow(SW_SHOW);
	SavePage();
	UpdateEncoded();
	
}
//Added by Zhu Zhenhua, 2004-5-22
////////////////////////////////////////////////////////////////////////
// SendReadRange::OnRadioSequenceNum
//
void SendReadRange::OnRadioSequenceNum() 
{
	UpdateData();

	m_ReadRangeRefDate.Disable();
    m_ReadRangeRefTime.Disable();
	m_ReadRangePosRef.Enable();
	m_ReadRangeCount.Enable();
	SetDlgItemText( IDC_BEGDATECAPTION, "Reference Date and Time");
//	GetDlgItem(IDC_DOW1)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_DOW2)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_DOW3)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_DOW4)->ShowWindow(SW_HIDE);
	SavePage();
	UpdateEncoded();
}
////////////////////////////////////////////////////////////////////////
// SendReadRange::OnChangeObjectid
//

void SendReadRange::OnChangeObjectid() 
{
	
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
	
}

void SendReadRange::OnSetfocusStartdatum() 
{
	// TODO: Add your control notification handler code here
	
}
////////////////////////////////////////////////////////////////////////
//SendReadRange::OnChangeTimestart
//
void SendReadRange::OnChangeTimestart() 
{
    m_ReadRangeRefTime.UpdateData();
	SavePage();
	UpdateEncoded();
}
////////////////////////////////////////////////////////////////////////
// SendReadRange::OnChangePosRef
//
void SendReadRange::OnChangePosRef() 
{
    m_ReadRangePosRef.UpdateData();
	SavePage();
	UpdateEncoded();
	
}
////////////////////////////////////////////////////////////////////////
// SendReadRange::OnChangeCount
//
void SendReadRange::OnChangeCount() 
{
    m_ReadRangeCount.UpdateData();
	SavePage();
	UpdateEncoded();
	
}

void SendReadRange::OnDropdownPropcombo() 
{
//Added by Zhu Zhenhua , 2003-7-22 
//	to Load Standard property list for certain ObjectType	
	m_PropCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_PropCombo.LoadCombo();
}
