// VTSListOfTimeStampDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSListOfTimeStampDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSListOfTimeStampDlg dialog
#pragma warning( disable : 4355 )
IMPLEMENT_DYNCREATE( VTSListOfTimeStampDlg, CDialog )
VTSListOfTimeStampDlg::VTSListOfTimeStampDlg(CWnd* pParent)
	: CDialog(VTSListOfTimeStampDlg::IDD, pParent)
	,m_TimeStampList(this)
{
	//{{AFX_DATA_INIT(VTSListOfTimeStampDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void VTSListOfTimeStampDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSListOfTimeStampDlg)
	DDX_Control(pDX, IDC_TIMESTAMPLIST, m_TimeStampListCtrl);
	//}}AFX_DATA_MAP
	if (m_TimeStampList.tscurElem) {
		m_TimeStampList.tscurElem->m_Time.UpdateData( pDX->m_bSaveAndValidate );
		m_TimeStampList.tscurElem->m_SeqNum.UpdateData( pDX->m_bSaveAndValidate );
		m_TimeStampList.tscurElem->m_DTDate.UpdateData( pDX->m_bSaveAndValidate );
		m_TimeStampList.tscurElem->m_DTTime.UpdateData( pDX->m_bSaveAndValidate );
	}
}


BEGIN_MESSAGE_MAP(VTSListOfTimeStampDlg, CDialog)
	//{{AFX_MSG_MAP(VTSListOfTimeStampDlg)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_TIMESTAMPLIST, OnItemchangingTimestamplist)
	ON_EN_CHANGE(IDC_TIME, OnChangeTime)
	ON_EN_CHANGE(IDC_SEQNUM, OnChangeSeqnum)
	ON_EN_CHANGE(IDC_DTDATE, OnChangeDtdate)
	ON_EN_CHANGE(IDC_DTTIME, OnChangeDttime)
	ON_BN_CLICKED(IDC_TIMERADIO, OnTimeradio)
	ON_BN_CLICKED(IDC_DATETIMERADIO, OnDatetimeradio)
	ON_BN_CLICKED(IDC_UNSIGNEDRADIO, OnUnsignedradio)
	ON_BN_CLICKED(IDC_ADDSUMELEM, OnAddsumelem)
	ON_BN_CLICKED(IDC_REMOVESUMELEM, OnRemovesumelem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSListOfTimeStampDlg message handlers
void VTSListOfTimeStampDlg::OnAddsumelem() 
{
	m_TimeStampList.AddButtonClick();	
}
BOOL VTSListOfTimeStampDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int listLen = 0;
	// only allow one selection at a time, no sorting
	m_TimeStampListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_TimeStampListCtrl.m_nFlags &= ~LBS_SORT;

	// set up the property list columns
	m_TimeStampListCtrl.InsertColumn( 0, "timestamp type", LVCFMT_LEFT, 126);
	for (POSITION pos = m_TimeStampList.GetHeadPosition(); pos != NULL;listLen++)
	{	
		TimeStampElemPtr item = m_TimeStampList.GetNext( pos );
		switch(item->m_nChoice)
		{
		case 0:
			m_TimeStampListCtrl.InsertItem( listLen, "Time" );
			break;
		case 1:
			m_TimeStampListCtrl.InsertItem( listLen, "Unsigned" );
			break;
		case 2:
			m_TimeStampListCtrl.InsertItem( listLen, "DateTime" );
			break;
		default:
			break;
		}	
	}
	return TRUE;  
}
void VTSListOfTimeStampDlg::OnRemovesumelem() 
{
	m_TimeStampList.RemoveButtonClick();
}
void VTSListOfTimeStampDlg::OnItemchangingTimestamplist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_TimeStampList.OnItemChanging( pNMHDR, pResult );
}

void VTSListOfTimeStampDlg::OnChangeTime() 
{
	m_TimeStampList.OnChangeTime();	
}
void VTSListOfTimeStampDlg::OnChangeSeqnum() 
{
  m_TimeStampList.OnChangeSeqnum();	
}

void VTSListOfTimeStampDlg::OnChangeDtdate() 
{
  m_TimeStampList.OnChangeDtdate();
}

void VTSListOfTimeStampDlg::OnChangeDttime() 
{
  m_TimeStampList.OnChangeDttime();
}

void VTSListOfTimeStampDlg::OnTimeradio() 
{
  m_TimeStampList.OnTimeradio();
}

void VTSListOfTimeStampDlg::OnDatetimeradio() 
{
  m_TimeStampList.OnDatetimeradio();	
}

void VTSListOfTimeStampDlg::OnUnsignedradio() 
{
  m_TimeStampList.OnUnsignedradio();
}
void VTSListOfTimeStampDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	for (POSITION pos = m_TimeStampList.GetHeadPosition(); pos != NULL; )
		m_TimeStampList.GetNext( pos )->Encode( enc );
}
void VTSListOfTimeStampDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)	
	{
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			m_TimeStampList.tscurElem = new TimeStampElem(this);
			BACnetAPDUTag		tagTestType;
			dec.ExamineTag(tagTestType);
			int Choice=tagTestType.tagNumber;
			switch (Choice) 
			{
			case 0:		
				m_TimeStampList.tscurElem->m_Time.Decode(dec);
				break;
			case 1:
				m_TimeStampList.tscurElem->m_SeqNum.Decode(dec);
				break;
			case 2:
				BACnetOpeningTag().Decode(dec);
				m_TimeStampList.tscurElem->m_DTDate.Decode(dec);
				m_TimeStampList.tscurElem->m_DTTime.Decode(dec);	
				BACnetClosingTag().Decode(dec);
				break;
			default:
				throw "Unknow choice.";
				break;
			}
			m_TimeStampList.AddTail( m_TimeStampList.tscurElem );
		}	
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
TimeStampElem::TimeStampElem( VTSListOfTimeStampDlg* wp )
	: m_Time( wp, IDC_TIME )
	, m_SeqNum( wp, IDC_SEQNUM )
	, m_DTDate( wp, IDC_DTDATE )
	, m_DTTime( wp, IDC_DTTIME )
{	
	m_nChoice = 0;
	// controls start out disabled
	m_Time.ctrlEnabled = false;
	m_Time.ctrlNull = false;				
	m_SeqNum.ctrlEnabled = false;
	m_SeqNum.ctrlNull = false;
	m_DTDate.ctrlEnabled = false;
	m_DTDate.ctrlNull = false;	
	m_DTTime.ctrlEnabled = false;
	m_DTTime.ctrlNull = false;	
}

//
//	TimeStampElem::Bind
//

void TimeStampElem::Bind( void )
{
	// set the control value to this element value
	switch(m_nChoice) {
	case 0:
	m_Time.ObjToCtrl();
	m_Time.Enable();
	((CButton*)m_Time.ctrlWindow->GetDlgItem( IDC_TIMERADIO ))->SetCheck(1);
	m_SeqNum.Disable();
	m_DTDate.Disable();
	m_DTTime.Disable();
		break;
	case 1:
	m_SeqNum.ObjToCtrl();
	((CButton*)m_Time.ctrlWindow->GetDlgItem( IDC_UNSIGNEDRADIO ))->SetCheck(1);
	m_Time.Disable();
	m_SeqNum.Enable();
	m_DTDate.Disable();
	m_DTTime.Disable();
		break;
	case 2:
	m_Time.Disable();
	m_SeqNum.Disable();
	((CButton*)m_Time.ctrlWindow->GetDlgItem( IDC_DATETIMERADIO ))->SetCheck(1);
	m_DTDate.ObjToCtrl();
	m_DTDate.Enable();
	m_DTTime.ObjToCtrl();
	m_DTTime.Enable();
		break;
	default:
		break;
	}
}

//
//	TimeStampElem::Unbind
//

void TimeStampElem::Unbind( void )
{
	// clear out the contents of the controls
	m_Time.ctrlWindow->GetDlgItem( IDC_TIME )->SetWindowText( "" );
	m_Time.Disable();
	m_SeqNum.ctrlWindow->GetDlgItem( IDC_TIME )->SetWindowText( "" );
	m_SeqNum.Disable();
	m_DTDate.ctrlWindow->GetDlgItem( IDC_TIME )->SetWindowText( "" );
	m_DTDate.Disable();
	m_DTTime.ctrlWindow->GetDlgItem( IDC_TIME )->SetWindowText( "" );
	m_DTTime.Disable();
}

//
//	TimeStampElem::Encode
//

void TimeStampElem::Encode( BACnetAPDUEncoder& enc, int context)
{
	switch (m_nChoice) 
	{
	case 0:
		m_Time.Encode(enc, 0);
		break;
	case 1:
		m_SeqNum.Encode(enc, 1);
		break;
	case 2:
		BACnetOpeningTag().Encode(enc, 2);
		m_DTDate.Encode(enc);
		m_DTTime.Encode(enc);
		BACnetClosingTag().Encode(enc, 2);
		break;
	default:
		throw "Unknow choice.";
		break;
	}
}
void TimeStampElem::Decode( BACnetAPDUDecoder& dec)
{
	BACnetAPDUTag		tagTestType;
	dec.ExamineTag(tagTestType);

	// Tag has 0, 1, 2 for time, unsigned, datetime

	if (tagTestType.tagClass == openingTagClass)
	{
		BACnetOpeningTag().Decode(dec);
		m_DTDate.Decode(dec);
		m_DTTime.Decode(dec);
		BACnetClosingTag().Decode(dec);
	}
	else
    switch (tagTestType.tagNumber)
	{
		case 0:					m_Time.Decode(dec);	break;
		case 1:					m_SeqNum.Decode(dec); break;
		default:				TRACE0("INVALID type in encoded stream for TimeStamp");
								ASSERT(0);
	}
}

//
//	TimeStampList::TimeStampList
//
TimeStampList::TimeStampList( VTSListOfTimeStampDlg* pp )
	: tsParentPtr( pp )
	, tscurElem(0), 
	  tscurElemIndx(0)
{
}

//
//	TimeStampList::~TimeStampList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

TimeStampList::~TimeStampList( void )
{	
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	TimeStampList::AddButtonClick
//

void TimeStampList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = 	tsParentPtr->m_TimeStampListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = 	tsParentPtr->m_TimeStampListCtrl.GetNextSelectedItem( selPos );
			tsParentPtr->m_TimeStampListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	tsaddInProgress = true;
	tsParentPtr->m_TimeStampListCtrl.InsertItem( listLen, "!" );

	// create a new item, add to the end of the list
	tscurElem = new TimeStampElem(tsParentPtr);
	tscurElemIndx = listLen;
	AddTail( tscurElem );

	// bind the element to the controls
	tscurElem->Bind();

	tsaddInProgress = false;
	tsParentPtr->m_TimeStampListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
}

//
//	TimeStampList::RemoveButtonClick
//

void TimeStampList::RemoveButtonClick( void )
{
	int				curRow = tscurElemIndx
	;
	TimeStampElem*	curElem = tscurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	tsParentPtr->m_TimeStampListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	tsParentPtr->m_TimeStampListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	tsParentPtr->m_TimeStampListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );
}

void TimeStampList::OnChangeTime()
{
	if (tscurElem) {
		tscurElem->m_Time.UpdateData();
		if (tscurElem->m_Time.ctrlNull)
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "" );
		else
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "time" );
	}
}
void TimeStampList::OnChangeSeqnum()
{
	if (tscurElem) {
		tscurElem->m_SeqNum.UpdateData();
		if (tscurElem->m_SeqNum.ctrlNull)
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "" );
		else
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "unsigned" );
	}
}
void TimeStampList::OnChangeDtdate()
{
	if (tscurElem) {
		tscurElem->m_DTDate.UpdateData();
		if (tscurElem->m_SeqNum.ctrlNull)
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "" );
		else
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "DateTime" );
	}
}
void TimeStampList::OnChangeDttime()
{
	if (tscurElem) {
		tscurElem->m_DTTime.UpdateData();
		if (tscurElem->m_SeqNum.ctrlNull)
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "" );
		else
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "DateTime" );
	}
}
void TimeStampList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = tscurElemIndx
	;
	TimeStampElem* 	    curElem = tscurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	// madanner 9/5/02, proper masking check?
	if ((pNMListView->uOldState && LVIS_SELECTED) == (pNMListView->uNewState && LVIS_SELECTED))
		return;

	// skip messages during new item creation
	if (tsaddInProgress)
		return;

	if ((pNMListView->uNewState && LVIS_SELECTED) != 0) {		// madanner 9/5/02, proper mask ??
		// item becoming selected
		tscurElemIndx = pNMListView->iItem;
		tscurElem = GetAt( FindIndex( tscurElemIndx ) );

		// bind the new current element
		tscurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == tscurElemIndx) {
			// set nothing selected
			tscurElem = 0;
			tscurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	TimeStampList::Encode
//
void TimeStampList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound elements
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}
void TimeStampList::OnTimeradio()
{
	if (tscurElem) {
		tscurElem->m_nChoice = 0;
		if (tscurElem->m_Time.ctrlNull)
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 1, "" );
		else
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "time" );
	if (tscurElem->m_Time.ctrlNull)
		tscurElem->m_Time.ctrlNull = false;
		tscurElem->m_Time.ObjToCtrl();	
		tscurElem->m_Time.Enable();
		tscurElem->m_SeqNum.Disable();
		tscurElem->m_DTDate.Disable();
		tscurElem->m_DTTime.Disable();
		OnChangeTime();
	}
}
void TimeStampList::OnUnsignedradio()
{
	if (tscurElem) {
		if (tscurElem->m_SeqNum.ctrlNull)
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 1, "" );
		else
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "unsigned" );
	if (tscurElem->m_SeqNum.ctrlNull)
		tscurElem->m_SeqNum.ctrlNull = false;
		tscurElem->m_SeqNum.ObjToCtrl();
		tscurElem->m_nChoice = 1;
		tscurElem->m_Time.Disable();
		tscurElem->m_SeqNum.Enable();
		tscurElem->m_DTDate.Disable();
		tscurElem->m_DTTime.Disable();
		OnChangeSeqnum();
	}
}
void TimeStampList::OnDatetimeradio()
{
	if (tscurElem) {
		if (tscurElem->m_SeqNum.ctrlNull)
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 1, "" );
		else
			tsParentPtr->m_TimeStampListCtrl.SetItemText( tscurElemIndx, 0, "DateTime" );
	if (tscurElem->m_DTDate.ctrlNull)
		tscurElem->m_DTDate.ctrlNull = false;
		tscurElem->m_DTDate.ObjToCtrl();
	if (tscurElem->m_DTTime.ctrlNull)
		tscurElem->m_DTTime.ctrlNull = false;
		tscurElem->m_DTTime.ObjToCtrl();
		tscurElem->m_nChoice = 2;
		tscurElem->m_Time.Disable();
		tscurElem->m_SeqNum.Disable();
		tscurElem->m_DTDate.Enable();
		tscurElem->m_DTTime.Enable();
	}
	OnChangeDtdate();
	OnChangeDttime();
}

