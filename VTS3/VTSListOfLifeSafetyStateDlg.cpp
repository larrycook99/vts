// VTSListOfLifeSafetyStateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSListOfLifeSafetyStateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// VTSListOfLifeSafetyStateDlg dialog

#pragma warning( disable : 4355 )
VTSListOfLifeSafetyStateDlg::VTSListOfLifeSafetyStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSListOfLifeSafetyStateDlg::IDD, pParent)
	, m_EnumList(this)
{
	//{{AFX_DATA_INIT(VTSListOfLifeSafetyStateDlg)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void VTSListOfLifeSafetyStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSListOfLifeSafetyStateDlg)
	DDX_Control(pDX, IDC_ENUMLIST, m_EnumListCtrl);
	//}}AFX_DATA_MAP
	if (m_EnumList.tscurElem) 
		m_EnumList.tscurElem->m_enumvalue.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSListOfLifeSafetyStateDlg, CDialog)
	//{{AFX_MSG_MAP(VTSListOfLifeSafetyStateDlg)
	ON_CBN_SELCHANGE(IDC_ENUMVALUE, OnSelchangeEnumvalue)
	ON_BN_CLICKED(IDC_REMOVESUMELEM, OnRemovesumelem)
	ON_BN_CLICKED(IDC_ADDSUMELEM, OnAddsumelem)
	ON_NOTIFY(HDN_ITEMCHANGING, IDC_ENUMLIST, OnItemchangingEnumlist)
	ON_CBN_DROPDOWN(IDC_ENUMVALUE, OnDropdownEnumvalue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSListOfLifeSafetyStateDlg message handlers


void VTSListOfLifeSafetyStateDlg::OnItemchangingEnumlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_EnumList.OnItemChanging(pNMHDR, pResult);
}

void VTSListOfLifeSafetyStateDlg::OnRemovesumelem() 
{
	m_EnumList.RemoveButtonClick();
}

void VTSListOfLifeSafetyStateDlg::OnAddsumelem() 
{
	m_EnumList.AddButtonClick();
}

void VTSListOfLifeSafetyStateDlg::OnSelchangeEnumvalue() 
{
	m_EnumList.OnSelchangeEnumvalue();
}

BOOL VTSListOfLifeSafetyStateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int listLen = 0;
	// only allow one selection at a time, no sorting
	m_EnumListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_EnumListCtrl.m_nFlags &= ~LBS_SORT;
	// set up the property list columns
	m_EnumListCtrl.InsertColumn( 0, "data type", LVCFMT_LEFT, 126);
	m_EnumListCtrl.InsertColumn( 1, "Value", LVCFMT_LEFT, 126);
	for (POSITION pos = m_EnumList.GetHeadPosition(); pos != NULL;listLen++)
	{
		EnumElemPtr item = m_EnumList.GetNext( pos );
		m_EnumListCtrl.InsertItem( listLen, "!" );
		m_EnumListCtrl.SetItemText( listLen, 0, "EnumValue" );
		if (item->m_enumvalue.ctrlNull)
		m_EnumListCtrl.SetItemText( listLen, 1, "" );
		else
		{
			CString str;
			str.Format("%d", item->m_enumvalue.m_enumValue);
		m_EnumListCtrl.SetItemText( listLen, 1, str.GetBuffer(0) );
		}
	}
	return TRUE;  
}
void VTSListOfLifeSafetyStateDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	for (POSITION pos = m_EnumList.GetHeadPosition(); pos != NULL; )
		m_EnumList.GetNext( pos )->Encode( enc );
}
void VTSListOfLifeSafetyStateDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)	
	{
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			m_EnumList.tscurElem = new EnumElem(this);
			m_EnumList.tscurElem->m_enumvalue.Decode(dec);
			m_EnumList.AddTail( m_EnumList.tscurElem );
		}	
	}
}
void VTSListOfLifeSafetyStateDlg::OnDropdownEnumvalue() 
{
	m_EnumList.OnDropdownEnumvalue();
}
////////////////////////////////////////////////////////////////////////////////////////////
EnumElem::EnumElem( VTSListOfLifeSafetyStateDlg* wp )
	:m_enumvalue( wp, IDC_ENUMVALUE, NetworkSniffer::BAC_STRTAB_BACnetLifeSafetyState, true )
{	
	// controls start out disabled
	m_enumvalue.ctrlEnabled = false;
	m_enumvalue.ctrlNull = false;				
}

//
//	EnumElem::Bind
//
void EnumElem::Bind( void )
{
	// set the control value to this element value
	m_enumvalue.ObjToCtrl();
	m_enumvalue.Enable();

}

//
//	EnumElem::Unbind
//

void EnumElem::Unbind( void )
{
	// clear out the contents of the controls
	m_enumvalue.ctrlWindow->GetDlgItem( IDC_ENUMVALUE )->SetWindowText( "" );
	m_enumvalue.Disable();
}

//
//	EnumElem::Encode
//

void EnumElem::Encode( BACnetAPDUEncoder& enc, int context)
{	
	m_enumvalue.Encode(enc);
}
void EnumElem::Decode( BACnetAPDUDecoder& dec)
{
	m_enumvalue.Decode(dec);
}

//
//	EnumList::EnumList
//
EnumList::EnumList( VTSListOfLifeSafetyStateDlg* pp )
	: tsParentPtr( pp )
	, tscurElem(0), 
	  tscurElemIndx(0)
{
}

//
//	EnumList::~EnumList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

EnumList::~EnumList( void )
{	
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	EnumList::AddButtonClick
//

void EnumList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = 	tsParentPtr->m_EnumListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = 	tsParentPtr->m_EnumListCtrl.GetNextSelectedItem( selPos );
		tsParentPtr->m_EnumListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );

	}

	// create a new list item
	tsaddInProgress = true;
	tsParentPtr->m_EnumListCtrl.InsertItem( listLen, "!" );

	// create a new item, add to the end of the list
	tscurElem = new EnumElem(tsParentPtr);
	tscurElemIndx = listLen;
	AddTail( tscurElem );

	// bind the element to the controls
	tscurElem->Bind();

	tsaddInProgress = false;
	tsParentPtr->m_EnumListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
}

//
//	EnumList::RemoveButtonClick
//

void EnumList::RemoveButtonClick( void )
{
	int				curRow = tscurElemIndx
	;
	EnumElem*	curElem = tscurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	tsParentPtr->m_EnumListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	tsParentPtr->m_EnumListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// reselect a new row... just before the deleted one if any.
	tsParentPtr->m_EnumListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

}

void EnumList::OnSelchangeEnumvalue()
{
	if (tscurElem) {
//		tscurElem->m_enumvalue.UpdateData();
		tscurElem->m_enumvalue.ctrlNull = false;
		tscurElem->m_enumvalue.CtrlToObj();
		tsParentPtr->m_EnumListCtrl.SetItemText( tscurElemIndx, 0, "enumvalue" );
		if (tscurElem->m_enumvalue.ctrlNull)
		tsParentPtr->m_EnumListCtrl.SetItemText( tscurElemIndx, 1, "" );
		else
		{
			CString str;	
			str.Format("%d", tscurElem->m_enumvalue.m_enumValue);
			tsParentPtr->m_EnumListCtrl.SetItemText( tscurElemIndx, 1, str.GetBuffer(0) );
		}
	}
}
void EnumList::OnDropdownEnumvalue()
{
	if (tscurElem) 
		tscurElem->m_enumvalue.LoadCombo();
}
void EnumList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = tscurElemIndx
	;
	EnumElem* 	    curElem = tscurElem
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
//	EnumList::Encode
//
void EnumList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound elements
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}



