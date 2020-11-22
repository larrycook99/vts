// VTSListOfReadAccessSpecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "propid.h"
#include "VTSListOfReadAccessSpecDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSListOfReadAccessSpecDlg dialog

IMPLEMENT_DYNCREATE( VTSListOfReadAccessSpecDlg, CDialog )

#pragma warning( disable : 4355 )
VTSListOfReadAccessSpecDlg::VTSListOfReadAccessSpecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSListOfReadAccessSpecDlg::IDD, pParent)
	, m_PropListList( this )
{
	//{{AFX_DATA_INIT(VTSListOfReadAccessSpecDlg)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void VTSListOfReadAccessSpecDlg::DoDataExchange(CDataExchange* pDX)
{
	ReadAccessSpecPtr	rplp = m_PropListList.rpllCurElem
	;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSListOfReadAccessSpecDlg)
	DDX_Control(pDX, IDC_OBJLIST, m_ObjList);
	DDX_Control(pDX, IDC_PROPLIST, m_PropList);
	//}}AFX_DATA_MAP

	// if there is a selected object, allow the ObjID to update
	if (rplp)
		rplp->rplObjID.UpdateData( pDX->m_bSaveAndValidate );

	// if there is a selected property, allow the controls to update
	if (rplp && rplp->rplCurElem) {
		rplp->rplCurElem->rpePropCombo.UpdateData( pDX->m_bSaveAndValidate );
		rplp->rplCurElem->rpeArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(VTSListOfReadAccessSpecDlg, CDialog)
	//{{AFX_MSG_MAP(VTSListOfReadAccessSpecDlg)
	ON_BN_CLICKED(IDC_ADDOBJ, OnAddObj)
	ON_BN_CLICKED(IDC_REMOVEOBJ, OnRemoveObj)
	ON_BN_CLICKED(IDC_OBJUP, OnObjUp)
	ON_BN_CLICKED(IDC_OBJDOWN, OnObjDown)
	ON_EN_CHANGE(IDC_OBJID, OnChangeObjID)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_OBJLIST, OnItemchangingObjList)
	ON_BN_CLICKED(IDC_ADDPROP, OnAddProp)
	ON_BN_CLICKED(IDC_REMOVEPROP, OnRemoveProp)
	ON_BN_CLICKED(IDC_PROPUP, OnPropUp)
	ON_BN_CLICKED(IDC_PROPDOWN, OnPropDown)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_PROPLIST, OnItemchangingPropList)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void VTSListOfReadAccessSpecDlg::Encode( BACnetAPDUEncoder& enc, int context)
{
	m_PropListList.Encode( enc );
}
void VTSListOfReadAccessSpecDlg::Decode( BACnetAPDUDecoder& dec )
{
	if(dec.pktLength != 0)
	{
		m_PropListList.Decode( dec );
	}
}
//
//	VTSListOfReadAccessSpecDlg::OnInitDialog
//

BOOL VTSListOfReadAccessSpecDlg::OnInitDialog() 
{
	TRACE0( "VTSListOfReadAccessSpecDlg::OnInitDialog()\n" );
	int i;					// MAG 11AUG05 add this line, remove local declaration below since i is used out of that scope

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_ObjList.m_nFlags |= LVS_SINGLESEL;
	m_ObjList.m_nFlags &= ~LBS_SORT;

	// initialize the object list
	m_ObjList.InsertColumn( 0, "Object ID", LVCFMT_LEFT, 128 );

	// only allow one selection at a time, no sorting
	m_PropList.m_nFlags |= LVS_SINGLESEL;
	m_PropList.m_nFlags &= ~LBS_SORT;

	// set up the property list columns
	m_PropList.InsertColumn( 0, "Property", LVCFMT_LEFT, 175 );
	m_PropList.InsertColumn( 1, "Index", LVCFMT_RIGHT, 48 );

	// disable the controls, they'll be enabled when an object is selected
	GetDlgItem( IDC_OBJID )->EnableWindow( false );
	GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );
	GetDlgItem( IDC_ADDPROP )->EnableWindow( false );
	GetDlgItem( IDC_REMOVEPROP )->EnableWindow( false );
	GetDlgItem( IDC_PROPCOMBO )->EnableWindow( false );
	GetDlgItem( IDC_ARRAYINDEX )->EnableWindow( false );

	for(int m = 0; m < m_PropListList.GetCount(); m++)
	{	
		ReadAccessSpecPtr wpmrplistPtr = m_PropListList.GetAt(m_PropListList.FindIndex(m));		
		CString str;
		wpmrplistPtr->rplObjID.Encode( str, BACnetEncodeable::FMT_EPICS );
		m_strList.AddTail(str);
		wpmrplistPtr->Bind();
		for(int n = 0; n < wpmrplistPtr->GetCount(); n++)
		{
            PropRefElemPtr wpmrpmelemPtr = wpmrplistPtr->GetAt(wpmrplistPtr->FindIndex(n));
			wpmrpmelemPtr->rpePropCombo.ObjToCtrl();
			wpmrpmelemPtr->rpeArrayIndex.ObjToCtrl();
		}
	}
	for(i = 0; i < m_strList.GetCount(); i++)
		m_ObjList.InsertItem(i, m_strList.GetAt(m_strList.FindIndex(i)));

	return TRUE;
}

void VTSListOfReadAccessSpecDlg::OnAddObj() 
{
	m_PropListList.AddButtonClick();
}

void VTSListOfReadAccessSpecDlg::OnRemoveObj() 
{
	m_PropListList.RemoveButtonClick();
}

void VTSListOfReadAccessSpecDlg::OnObjUp() 
{
	m_PropListList.UpButtonClick();
}

void VTSListOfReadAccessSpecDlg::OnObjDown() 
{
	m_PropListList.DownButtonClick();
}

void VTSListOfReadAccessSpecDlg::OnChangeObjID() 
{
	m_PropListList.OnChangeObjID();
}

void VTSListOfReadAccessSpecDlg::OnObjectIDButton() 
{
	m_PropListList.OnObjectIDButton();
}

void VTSListOfReadAccessSpecDlg::OnItemchangingObjList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_PropListList.OnItemChanging( pNMHDR, pResult );
}

void VTSListOfReadAccessSpecDlg::OnAddProp() 
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->AddButtonClick();
}

void VTSListOfReadAccessSpecDlg::OnRemoveProp() 
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->RemoveButtonClick();
}

void VTSListOfReadAccessSpecDlg::OnPropUp() 
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->UpButtonClick();
}

void VTSListOfReadAccessSpecDlg::OnPropDown() 
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->DownButtonClick();
}

void VTSListOfReadAccessSpecDlg::OnItemchangingPropList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->OnItemChanging( pNMHDR, pResult );
}

void VTSListOfReadAccessSpecDlg::OnSelchangePropCombo()
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->OnSelchangePropCombo();
}

void VTSListOfReadAccessSpecDlg::OnChangeArrayIndex()
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->OnChangeArrayIndex();
}
void VTSListOfReadAccessSpecDlg::OnDropdownPropcombo() 
{
	if(m_PropListList.rpllCurElem->rplCurElem)
	{
		m_PropListList.rpllCurElem->rplCurElem->rpePropCombo.m_nObjType = m_PropListList.rpllCurElem->rplObjID.GetObjectType();
		m_PropListList.rpllCurElem->rplCurElem->rpePropCombo.LoadCombo();
	}
}
//
//	PropRefElem::PropRefElem
//

PropRefElem::PropRefElem( VTSListOfReadAccessSpecDlgPtr wp )
	: rpePropCombo( wp, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, rpeArrayIndex( wp, IDC_ARRAYINDEX )
{
	// controls start out disabled
	rpePropCombo.ctrlEnabled = false;
	rpeArrayIndex.ctrlEnabled = false;
}

//
//	PropRefElem::Bind
//

void PropRefElem::Bind( void )
{
	// set the control value to this element values
	rpePropCombo.ObjToCtrl();
	rpePropCombo.Enable();
	rpeArrayIndex.ObjToCtrl();
	rpeArrayIndex.Enable();
}

//
//	PropRefElem::Unbind
//

void PropRefElem::Unbind( void )
{
	// clear out the contents of the controls
	rpePropCombo.ctrlWindow->GetDlgItem( IDC_PROPCOMBO )->SetWindowText( "" );
	rpePropCombo.Disable();
	rpeArrayIndex.ctrlWindow->GetDlgItem( IDC_ARRAYINDEX )->SetWindowText( "" );
	rpeArrayIndex.Disable();
}

//
//	PropRefElem::Encode
//

void PropRefElem::Encode( BACnetAPDUEncoder& enc )
{
	// encode the property
	if (rpePropCombo.ctrlNull)
		throw "Property ID required";
	rpePropCombo.Encode( enc, 0 );

	// encode the (optional) array index
	if (!rpeArrayIndex.ctrlNull)
		rpeArrayIndex.Encode( enc, 1 );
}
void PropRefElem::Decode( BACnetAPDUDecoder& dec )
{
	rpePropCombo.Decode(dec);
	rpePropCombo.ctrlNull = false;

	if(dec.ExamineOption(1))
	{
		rpeArrayIndex.Decode(dec);
		rpeArrayIndex.ctrlNull = false;
	}
}
//
//	ReadAccessSpec::ReadAccessSpec
//

ReadAccessSpec::ReadAccessSpec( VTSListOfReadAccessSpecDlgPtr pp )
	: rplPagePtr( pp )
	, rplCurElem(0), rplCurElemIndx(0)
	, rplObjID( pp, IDC_OBJID )
{
	// give the object ID a default value
	rplObjID.ctrlEnabled = false;
	rplObjID.ctrlNull = false;
	rplObjID.objID = 0;

	//Xiao shiyuan 2002-12-3
	rplAddInProgress = FALSE; 
}

//
//	ReadAccessSpec::~ReadAccessSpec
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

ReadAccessSpec::~ReadAccessSpec( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	ReadAccessSpec::Bind
//

void ReadAccessSpec::Bind( void )
{
	int			i
	;
	CString		someText
	;

	// set the control value to this object id
	rplObjID.ObjToCtrl();
	rplObjID.Enable();
	rplPagePtr->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( true );

	// clear out the current table contents
	rplPagePtr->m_PropList.DeleteAllItems();

	// fill out the table with the current list of elements
	i = 0;
	for (POSITION pos = GetHeadPosition(); pos != NULL; i++ ) {
		PropRefElemPtr	rpep = GetNext( pos )
		;

		rplPagePtr->m_PropList.InsertItem( i
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( rpep->rpePropCombo.m_enumValue )
			);
		if (rpep->rpeArrayIndex.ctrlNull)
			rplPagePtr->m_PropList.SetItemText( i, 1, "" );
		else {
			someText.Format( "%d", rpep->rpeArrayIndex.uintValue );
			rplPagePtr->m_PropList.SetItemText( i, 1, someText );
		}
	}

	// enable the other controls
	rplPagePtr->GetDlgItem( IDC_ADDPROP )->EnableWindow( true );
	rplPagePtr->GetDlgItem( IDC_REMOVEPROP )->EnableWindow( true );
	rplPagePtr->GetDlgItem( IDC_PROPUP )->EnableWindow( true );
	rplPagePtr->GetDlgItem( IDC_PROPDOWN )->EnableWindow( true );
}

//
//	ReadAccessSpec::Unbind
//

void ReadAccessSpec::Unbind( void )
{
	// clear out the contents of the object id control
	rplObjID.ctrlWindow->GetDlgItem( IDC_OBJID )->SetWindowText( "" );
	rplObjID.Disable();
	rplPagePtr->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );

	// wipe out the contents of the table
	rplPagePtr->m_PropList.DeleteAllItems();

	// disable the other controls
	rplPagePtr->GetDlgItem( IDC_ADDPROP )->EnableWindow( false );
	rplPagePtr->GetDlgItem( IDC_REMOVEPROP )->EnableWindow( false );
	rplPagePtr->GetDlgItem( IDC_PROPUP )->EnableWindow( false );
	rplPagePtr->GetDlgItem( IDC_PROPDOWN )->EnableWindow( false );
}

//
//	ReadAccessSpec::AddButtonClick
//

void ReadAccessSpec::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = rplPagePtr->m_PropList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = rplPagePtr->m_PropList.GetNextSelectedItem( selPos );
		rplPagePtr->m_PropList.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	rplAddInProgress = true;
	rplPagePtr->m_PropList.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	rplCurElem = new PropRefElem( rplPagePtr );
	rplCurElemIndx = listLen;

	rplCurElem->rpePropCombo.SetEnumValue( PRESENT_VALUE );

	AddTail( rplCurElem );

	// bind the element to the controls
	rplCurElem->Bind();

	// update the encoding
	rplAddInProgress = false;

	// madanner, 8/26/02.  Sourceforge bug #472392
	OnSelchangePropCombo();				// Insert new list text for Present_Value
	rplPagePtr->m_PropList.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
//			Commented because prior call to selchange updates.
}

//
//	ReadAccessSpec::RemoveButtonClick
//

void ReadAccessSpec::RemoveButtonClick( void )
{
	int				curRow = rplCurElemIndx
	;
	PropRefElemPtr	curElem = rplCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	rplPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	rplPagePtr->m_PropList.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	rplPagePtr->m_PropList.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	
}

//
//	ReadAccessSpec::UpButtonClick
//

void ReadAccessSpec::UpButtonClick( void )
{
	int				curRow = rplCurElemIndx
	,				prevRow = curRow - 1
	;
	PropRefElemPtr	curElem = rplCurElem
	;

	// must have a selected row and a previous row
	if (curRow < 1)
		return;

	// move the row in the list
	rplPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );
	rplPagePtr->m_PropList.DeleteItem( curRow );
	rplPagePtr->m_PropList.InsertItem( prevRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION prevPos = FindIndex( prevRow );
	InsertBefore( prevPos, curElem );

	// current element moved up
	rplCurElemIndx -= 1;

	// select the row in its new position
	rplPagePtr->m_PropList.SetItemState( prevRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events (dont ask why we dont!)
	OnSelchangePropCombo();
	OnChangeArrayIndex();

	// update the encoding
	
}

//
//	ReadAccessSpec::DownButtonClick
//

void ReadAccessSpec::DownButtonClick( void )
{
	int				curRow = rplCurElemIndx
	,				nextRow = curRow + 1
	;
	PropRefElemPtr	curElem = rplCurElem
	;

	// must have a selected row and a following row
	if ((curRow < 0) || (curRow >= GetCount()-1))
		return;

	// move the row in the list
	rplPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );
	rplPagePtr->m_PropList.DeleteItem( curRow );
	rplPagePtr->m_PropList.InsertItem( nextRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION nextPos = FindIndex( curRow );
	InsertAfter( nextPos, curElem );

	// current element moved down
	rplCurElemIndx += 1;

	// select the row in its new position
	rplPagePtr->m_PropList.SetItemState( nextRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events (dont ask why we dont!)
	OnSelchangePropCombo();
	OnChangeArrayIndex();

	// update the encoding
	
}

//
//	ReadAccessSpec::OnSelchangePropCombo
//

void ReadAccessSpec::OnSelchangePropCombo( void )
{
	TRACE1( "ReadAccessSpec::OnSelchangePropCombo - %s\n", (rplCurElem ? "y" : "n") );

	if (rplCurElem) {
		rplCurElem->rpePropCombo.UpdateData();
		

		rplPagePtr->m_PropList.SetItemText( rplCurElemIndx, 0
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( rplCurElem->rpePropCombo.m_enumValue )
			);
	}
}

//
//	ReadAccessSpec::OnChangeArrayIndex
//

void ReadAccessSpec::OnChangeArrayIndex( void )
{
	TRACE0( "ReadAccessSpec::OnChangeArrayIndex\n" );

	if (rplCurElem) {
		CString		someText
		;

		rplCurElem->rpeArrayIndex.UpdateData();
		

		if (rplCurElem->rpeArrayIndex.ctrlNull)
			rplPagePtr->m_PropList.SetItemText( rplCurElemIndx, 1, "" );
		else {
			someText.Format( "%d", rplCurElem->rpeArrayIndex.uintValue );
			rplPagePtr->m_PropList.SetItemText( rplCurElemIndx, 1, someText );
		}
	}
}

//
//	ReadAccessSpec::OnItemChanging
//

void ReadAccessSpec::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = rplCurElemIndx
	;
	PropRefElemPtr	curElem = rplCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (rplAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		TRACE1( "ReadAccessSpec::OnItemChanging - %d selected\n", pNMListView->iItem );

		// item becoming selected
		rplCurElemIndx = pNMListView->iItem;
		rplCurElem = GetAt( FindIndex( rplCurElemIndx ) );

		// bind the new current element
		rplCurElem->Bind();
	} else {
		TRACE0( "ReadAccessSpec::OnItemChanging - nothing selected\n" );

		// item no longer selected
		if (pNMListView->iItem == rplCurElemIndx) {
			// set nothing selected
			rplCurElem = 0;
			rplCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	ReadAccessSpec::Encode
//

void ReadAccessSpec::Encode( BACnetAPDUEncoder& enc )
{
	// encode the object ID
	if (rplObjID.ctrlNull)
		throw "Object ID required";
	rplObjID.Encode( enc, 0 );

	// encode each of the bound properties
	BACnetOpeningTag().Encode( enc, 1 );
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
	BACnetClosingTag().Encode( enc, 1 );
}
void ReadAccessSpec::Decode( BACnetAPDUDecoder& dec )
{
	rplObjID.Decode(dec);
	rplObjID.ctrlNull = false;
	if(dec.ExamineOption(1))
	{   
		BACnetOpeningTag().Decode(dec);
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			rplCurElem = new PropRefElem( rplPagePtr );
			rplCurElem->Decode(dec);
			AddTail(rplCurElem);	
		}
		BACnetClosingTag().Decode(dec);	
	}
}
//
//	ReadAccessSpecList::ReadAccessSpecList
//

ReadAccessSpecList::ReadAccessSpecList( VTSListOfReadAccessSpecDlgPtr pp )
	: rpllPagePtr( pp )
	, rpllCurElem(0), rpllCurElemIndx(-1)
{
	rpllAddInProgress = FALSE; //Xiaoshiyuan 2002-12-3
}

//
//	ReadAccessSpecList::~ReadAccessSpecList
//
//	If there are any objects that have been added to the list they need to be 
//	deleted here.  The CList class will delete the stuff that it knows how to 
//	handle, but that doesn't include the ReadAccessSpec objects.
//

ReadAccessSpecList::~ReadAccessSpecList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )	
        delete GetNext( pos );
}

//
//	ReadAccessSpecList::AddButtonClick
//
//	This procedure is called when the user wants to add an additional object to
//	the list.
//

void ReadAccessSpecList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = rpllPagePtr->m_ObjList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = rpllPagePtr->m_ObjList.GetNextSelectedItem( selPos );
		rpllPagePtr->m_ObjList.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	rpllAddInProgress = true;
	rpllPagePtr->m_ObjList.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	rpllCurElem = new ReadAccessSpec( rpllPagePtr );
	rpllCurElemIndx = listLen;
	AddTail( rpllCurElem );

	// bind the element to the controls
	rpllCurElem->Bind();

	// update the encoding
	rpllAddInProgress = false;

	rpllPagePtr->m_ObjList.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
	 
}

//
//	ReadAccessSpecList::RemoveButtonClick
//

void ReadAccessSpecList::RemoveButtonClick( void )
{
	int				curRow = rpllCurElemIndx
	;
	ReadAccessSpecPtr	curElem = rpllCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	rpllPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	rpllPagePtr->m_ObjList.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	rpllPagePtr->m_ObjList.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	 
}

//
//	ReadAccessSpecList::UpButtonClick
//

void ReadAccessSpecList::UpButtonClick( void )
{
	int				curRow = rpllCurElemIndx
	,				prevRow = curRow - 1
	;
	ReadAccessSpecPtr	curElem = rpllCurElem
	;

	// must have a selected row and a previous row
	if (curRow < 1)
		return;

	// move the row in the list
	rpllPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );
	rpllPagePtr->m_ObjList.DeleteItem( curRow );
	rpllPagePtr->m_ObjList.InsertItem( prevRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION prevPos = FindIndex( prevRow );
	InsertBefore( prevPos, curElem );

	// current element moved up
	rpllCurElemIndx -= 1;

	// select the row in its new position
	rpllPagePtr->m_ObjList.SetItemState( prevRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events
	OnChangeObjID();

	// update the encoding
	 
}

//
//	ReadAccessSpecList::DownButtonClick
//

void ReadAccessSpecList::DownButtonClick( void )
{
	int				curRow = rpllCurElemIndx
	,				nextRow = curRow + 1
	;
	ReadAccessSpecPtr	curElem = rpllCurElem
	;

	// must have a selected row and a following row
	if ((curRow < 0) || (curRow >= GetCount()-1))
		return;

	// move the row in the list
	rpllPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );
	rpllPagePtr->m_ObjList.DeleteItem( curRow );
	rpllPagePtr->m_ObjList.InsertItem( nextRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION nextPos = FindIndex( curRow );
	InsertAfter( nextPos, curElem );

	// current element moved down
	rpllCurElemIndx += 1;

	// select the row in its new position
	rpllPagePtr->m_ObjList.SetItemState( nextRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events (dont ask why we dont!)
	OnChangeObjID();

	// update the encoding
	 
}

//
//	ReadAccessSpecList::OnChangeObjID
//

void ReadAccessSpecList::OnChangeObjID( void )
{
	CString			rStr
	;

	// make sure we have a current object
	if (!rpllCurElem)
		return;

	// pass the message to the current selected object
	rpllCurElem->rplObjID.UpdateData();

	// get the control contents, update the list
	rpllPagePtr->GetDlgItem( IDC_OBJID )->GetWindowText( rStr );
	rpllPagePtr->m_ObjList.SetItemText( rpllCurElemIndx, 0, rStr );

	// update the encoding
	 
}

//
//	ReadAccessSpecList::OnObjectIDButton
//

void ReadAccessSpecList::OnObjectIDButton( void )
{
	if (!rpllCurElem)
		return;

	VTSObjectIdentifierDialog	dlg(rpllPagePtr)			// for proper parent control
	;

	dlg.objID = rpllCurElem->rplObjID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		rpllCurElem->rplObjID.ctrlNull = false;
		rpllCurElem->rplObjID.objID = dlg.objID;
		rpllCurElem->rplObjID.ObjToCtrl();

		// copy the text from the control to the list
		CString text;
		rpllPagePtr->GetDlgItem( IDC_OBJID )->GetWindowText( text );
		rpllPagePtr->m_ObjList.SetItemText( rpllCurElemIndx, 0, text );

		 
	}
}

//
//	ReadAccessSpecList::OnItemChanging
//

void ReadAccessSpecList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = rpllCurElemIndx
	;
	ReadAccessSpecPtr	curElem = rpllCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (rpllAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		// item becoming selected
		rpllCurElemIndx = pNMListView->iItem;
		rpllCurElem = GetAt( FindIndex( rpllCurElemIndx ) );

		// bind the new current element
		rpllCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == rpllCurElemIndx) {
			// set nothing selected
			rpllCurElem = 0;
			rpllCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	ReadAccessSpecList::Encode
//
//	Each ReadAccessSpec element understands how to encode itself, so this procedure 
//	just calls Encode() for each of the objects in its list.
//

void ReadAccessSpecList::Encode( BACnetAPDUEncoder& enc )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}
void ReadAccessSpecList::Decode( BACnetAPDUDecoder& dec )
{
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			rpllCurElem = new ReadAccessSpec( rpllPagePtr );
			rpllCurElem->Decode(dec);
			AddTail(rpllCurElem);	
		}
}

