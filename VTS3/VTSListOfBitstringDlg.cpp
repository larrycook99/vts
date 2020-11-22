// VTSListOfBitstringDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSListOfBitstringDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSListOfBitstringDlg dialog


#pragma warning( disable : 4355 )
VTSListOfBitstringDlg::VTSListOfBitstringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSListOfBitstringDlg::IDD, pParent)
	, m_BitstringList(this)
{
	//{{AFX_DATA_INIT(VTSListOfBitstringDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSListOfBitstringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSListOfBitstringDlg)
	DDX_Control(pDX, IDC_BITSTRINGLIST, m_BitstringListCtrl);
	//}}AFX_DATA_MAP
	if (m_BitstringList.tscurElem) {
		m_BitstringList.tscurElem->m_bstrvalue.UpdateData( pDX->m_bSaveAndValidate );
	}
}


BEGIN_MESSAGE_MAP(VTSListOfBitstringDlg, CDialog)
	//{{AFX_MSG_MAP(VTSListOfBitstringDlg)
	ON_BN_CLICKED(IDC_ADDSUMELEM, OnAddsumelem)
	ON_EN_CHANGE(IDC_BITSTRING, OnChangeBitstring)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_BITSTRINGLIST, OnItemchangingBitstringlist)
	ON_BN_CLICKED(IDC_REMOVESUMELEM, OnRemovesumelem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSListOfBitstringDlg message handlers


void VTSListOfBitstringDlg::OnItemchangingBitstringlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_BitstringList.OnItemChanging(pNMHDR, pResult);
}

void VTSListOfBitstringDlg::OnRemovesumelem() 
{
	m_BitstringList.RemoveButtonClick();
}

void VTSListOfBitstringDlg::OnAddsumelem() 
{
	m_BitstringList.AddButtonClick();
}

void VTSListOfBitstringDlg::OnChangeBitstring() 
{
	m_BitstringList.OnChangeBitstring();
}

BOOL VTSListOfBitstringDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int listLen = 0;
	// only allow one selection at a time, no sorting
	m_BitstringListCtrl.m_nFlags |= LVS_SINGLESEL;
	m_BitstringListCtrl.m_nFlags &= ~LBS_SORT;
	// set up the property list columns
	m_BitstringListCtrl.InsertColumn( 0, "data type", LVCFMT_LEFT, 126);
	m_BitstringListCtrl.InsertColumn( 1, "Value", LVCFMT_LEFT, 126);
	for (POSITION pos = m_BitstringList.GetHeadPosition(); pos != NULL;listLen++)
	{
		BitstringElemPtr item = m_BitstringList.GetNext( pos );
		m_BitstringListCtrl.InsertItem( listLen, "!" );
		m_BitstringListCtrl.SetItemText( listLen, 0, "bitstring" );
		if (item->m_bstrvalue.ctrlNull)
		m_BitstringListCtrl.SetItemText( listLen, 1, "" );
		else
		{
		//	CString str;
		//	str.Format("%d", item->m_bstrvalue.uintValue);
		m_BitstringListCtrl.SetItemText( listLen, 1, item->m_bstrvalue.ToString());
		}
	}
	return TRUE;  
}
void VTSListOfBitstringDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	for (POSITION pos = m_BitstringList.GetHeadPosition(); pos != NULL; )
		m_BitstringList.GetNext( pos )->Encode( enc );
}
void VTSListOfBitstringDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)	
	{
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			m_BitstringList.tscurElem = new BitstringElem(this);
			m_BitstringList.tscurElem->m_bstrvalue.Decode(dec);
			m_BitstringList.AddTail( m_BitstringList.tscurElem );
		}	
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
BitstringElem::BitstringElem( VTSListOfBitstringDlg* wp )
	: m_bstrvalue( wp, IDC_BITSTRING )
{	
	// controls start out disabled
	m_bstrvalue.ctrlEnabled = false;
	m_bstrvalue.ctrlNull = false;				
}

//
//	BitstringElem::Bind
//
void BitstringElem::Bind( void )
{
	// set the control value to this element value
	m_bstrvalue.ObjToCtrl();
	m_bstrvalue.Enable();

}

//
//	BitstringElem::Unbind
//

void BitstringElem::Unbind( void )
{
	// clear out the contents of the controls
	m_bstrvalue.ctrlWindow->GetDlgItem( IDC_BITSTRING )->SetWindowText( "" );
	m_bstrvalue.Disable();
}

//
//	BitstringElem::Encode
//

void BitstringElem::Encode( BACnetAPDUEncoder& enc, int context)
{	
	m_bstrvalue.Encode(enc);
}
void BitstringElem::Decode( BACnetAPDUDecoder& dec)
{
	m_bstrvalue.Decode(dec);
}

//
//	BitstringList::BitstringList
//
BitstringList::BitstringList( VTSListOfBitstringDlg* pp )
	: tsParentPtr( pp )
	, tscurElem(0), 
	  tscurElemIndx(0)
{
}

//
//	BitstringList::~BitstringList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

BitstringList::~BitstringList( void )
{	
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	BitstringList::AddButtonClick
//

void BitstringList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = 	tsParentPtr->m_BitstringListCtrl.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = 	tsParentPtr->m_BitstringListCtrl.GetNextSelectedItem( selPos );
		tsParentPtr->m_BitstringListCtrl.SetItemState( nItem, 0, LVIS_SELECTED );

	}

	// create a new list item
	tsaddInProgress = true;
	tsParentPtr->m_BitstringListCtrl.InsertItem( listLen, "!" );

	// create a new item, add to the end of the list
	tscurElem = new BitstringElem(tsParentPtr);
	tscurElemIndx = listLen;
	AddTail( tscurElem );

	// bind the element to the controls
	tscurElem->Bind();

	tsaddInProgress = false;
	tsParentPtr->m_BitstringListCtrl.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
}

//
//	BitstringList::RemoveButtonClick
//

void BitstringList::RemoveButtonClick( void )
{
	int				curRow = tscurElemIndx
	;
	BitstringElem*	curElem = tscurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	tsParentPtr->m_BitstringListCtrl.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	tsParentPtr->m_BitstringListCtrl.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// reselect a new row... just before the deleted one if any.
	tsParentPtr->m_BitstringListCtrl.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

}

void BitstringList::OnChangeBitstring()
{
	if (tscurElem) {
		tscurElem->m_bstrvalue.UpdateData();
		tsParentPtr->m_BitstringListCtrl.SetItemText( tscurElemIndx, 0, "bitstring" );
		if (tscurElem->m_bstrvalue.ctrlNull)
		tsParentPtr->m_BitstringListCtrl.SetItemText( tscurElemIndx, 1, "" );
		else
		{
//			CString str;
//			str.Format("%d", tscurElem->m_bstrvalue.ToString());
			tsParentPtr->m_BitstringListCtrl.SetItemText( tscurElemIndx, 1, tscurElem->m_bstrvalue.ToString());
		}
	}
}

void BitstringList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = tscurElemIndx
	;
	BitstringElem* 	    curElem = tscurElem
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
//	BitstringList::Encode
//
void BitstringList::Encode( BACnetAPDUEncoder& enc )
{
	// encode each of the bound elements
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}

