// VTSListOfUnsignedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSListOfUnsignedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSListOfUnsignedDlg dialog
#pragma warning( disable : 4355 )
IMPLEMENT_DYNCREATE( VTSListOfUnsignedDlg, CDialog )
VTSListOfUnsignedDlg::VTSListOfUnsignedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSListOfUnsignedDlg::IDD, pParent)
	, m_UnsignedList(this)
{
	//{{AFX_DATA_INIT(VTSListOfUnsignedDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VTSListOfUnsignedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSListOfUnsignedDlg)
	DDX_Control(pDX, IDC_UNSIGNEDLIST, m_UnsignedListCtrl);
	//}}AFX_DATA_MAP
	if (m_UnsignedList.tscurElem) {
		m_UnsignedList.tscurElem->m_uintvalue.UpdateData( pDX->m_bSaveAndValidate );
	}
}


BEGIN_MESSAGE_MAP(VTSListOfUnsignedDlg, CDialog)
	//{{AFX_MSG_MAP(VTSListOfUnsignedDlg)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_UNSIGNEDLIST, OnItemchangingUnsignedlist)
	ON_BN_CLICKED(IDC_REMOVESUMELEM, OnRemovesumelem)
	ON_BN_CLICKED(IDC_ADDSUMELEM, OnAddsumelem)
	ON_EN_CHANGE(IDC_UNSIGNED, OnChangePriority)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSListOfUnsignedDlg message handlers

void VTSListOfUnsignedDlg::OnItemchangingUnsignedlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_UnsignedList.OnItemChanging(pNMHDR, pResult);
}

void VTSListOfUnsignedDlg::OnRemovesumelem() 
{
	m_UnsignedList.RemoveButtonClick();
}

void VTSListOfUnsignedDlg::OnAddsumelem() 
{
	m_UnsignedList.AddButtonClick();
}

void VTSListOfUnsignedDlg::OnChangePriority() 
{
	m_UnsignedList.OnChangePriority();
}

BOOL VTSListOfUnsignedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int listLen = 0;
	// only allow one selection at a time, no sorting
	m_UnsignedListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_UnsignedListCtrl.m_nFlags &= ~LBS_SORT;
	// set up the property list columns
	m_UnsignedListCtrl.InsertColumn( 0, "data type", LVCFMT_LEFT, 126);
	m_UnsignedListCtrl.InsertColumn( 1, "Value", LVCFMT_LEFT, 126);
	for (POSITION pos = m_UnsignedList.GetHeadPosition(); pos != NULL;listLen++)
	{
		UnsignedElemPtr item = m_UnsignedList.GetNext( pos );
		m_UnsignedListCtrl.InsertItem( listLen, "!" );
		m_UnsignedListCtrl.SetItemText( listLen, 0, "Unsigned" );
		if (item->m_uintvalue.ctrlNull)
		m_UnsignedListCtrl.SetItemText( listLen, 1, "" );
		else
		{
			CString str;
			str.Format("%d", item->m_uintvalue.uintValue);
		m_UnsignedListCtrl.SetItemText( listLen, 1, str.GetBuffer(0) );
		}
	}
	return TRUE;  
}
void VTSListOfUnsignedDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	for (POSITION pos = m_UnsignedList.GetHeadPosition(); pos != NULL; )
		m_UnsignedList.GetNext( pos )->Encode( enc );
}
void VTSListOfUnsignedDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)	
	{
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			m_UnsignedList.tscurElem = new UnsignedElem(this);
			m_UnsignedList.tscurElem->m_uintvalue.Decode(dec);
			m_UnsignedList.AddTail( m_UnsignedList.tscurElem );
		}	
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
UnsignedElem::UnsignedElem( VTSListOfUnsignedDlg* wp )
	: m_uintvalue( wp, IDC_UNSIGNED )
{	
	// controls start out disabled
	m_uintvalue.ctrlEnabled = false;
	m_uintvalue.ctrlNull = false;				
}

//
//	UnsignedElem::Bind
//
void UnsignedElem::Bind( void )
{
	// set the control value to this element value
	m_uintvalue.ObjToCtrl();
	m_uintvalue.Enable();

}

//
//	UnsignedElem::Unbind
//

void UnsignedElem::Unbind( void )
{
	// clear out the contents of the controls
	m_uintvalue.ctrlWindow->GetDlgItem( IDC_UNSIGNED )->SetWindowText( "" );
	m_uintvalue.Disable();
}

//
//	UnsignedElem::Encode
//

void UnsignedElem::Encode( BACnetAPDUEncoder& enc, int context)
{	
	m_uintvalue.Encode(enc);
}
void UnsignedElem::Decode( BACnetAPDUDecoder& dec)
{
	m_uintvalue.Decode(dec);
}

//
//	UnsignedList::UnsignedList
//
UnsignedList::UnsignedList( VTSListOfUnsignedDlg* pp )
	: tsParentPtr( pp )
	, tscurElem(0), 
	  tscurElemIndx(0)
{
}

//
//	UnsignedList::~UnsignedList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

UnsignedList::~UnsignedList( void )
{	
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	UnsignedList::AddButtonClick
//

void UnsignedList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = 	tsParentPtr->m_UnsignedListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = 	tsParentPtr->m_UnsignedListCtrl.GetNextSelectedItem( selPos );
		tsParentPtr->m_UnsignedListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );

	}

	// create a new list item
	tsaddInProgress = true;
	tsParentPtr->m_UnsignedListCtrl.InsertItem( listLen, "!" );

	// create a new item, add to the end of the list
	tscurElem = new UnsignedElem(tsParentPtr);
	tscurElemIndx = listLen;
	AddTail( tscurElem );

	// bind the element to the controls
	tscurElem->Bind();

	tsaddInProgress = false;
	tsParentPtr->m_UnsignedListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
}

//
//	UnsignedList::RemoveButtonClick
//

void UnsignedList::RemoveButtonClick( void )
{
	int				curRow = tscurElemIndx
	;
	UnsignedElem*	curElem = tscurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	tsParentPtr->m_UnsignedListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	tsParentPtr->m_UnsignedListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// reselect a new row... just before the deleted one if any.
	tsParentPtr->m_UnsignedListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

}

void UnsignedList::OnChangePriority()
{
	if (tscurElem) {
		tscurElem->m_uintvalue.UpdateData();
		tsParentPtr->m_UnsignedListCtrl.SetItemText( tscurElemIndx, 0, "Unsigned" );
		if (tscurElem->m_uintvalue.ctrlNull)
		tsParentPtr->m_UnsignedListCtrl.SetItemText( tscurElemIndx, 1, "" );
		else
		{
			CString str;
			str.Format("%d", tscurElem->m_uintvalue.uintValue);
			tsParentPtr->m_UnsignedListCtrl.SetItemText( tscurElemIndx, 1, str.GetBuffer(0) );
		}
	}
}

void UnsignedList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = tscurElemIndx
	;
	UnsignedElem* 	    curElem = tscurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
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
//	UnsignedList::Encode
//
void UnsignedList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound elements
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}

