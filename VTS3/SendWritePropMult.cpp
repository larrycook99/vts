// SendWritePropMult.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "propid.h"

#include "Send.h"
#include "SendWritePropMult.h"

#include "VTSObjectIdentifierDialog.h"

#include "WPRPList.h" //Xiao Shiyuan 2002-12-2

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern WPMRPMListList glWPMRPMListList[5]; //Xiao Shiyuan 2002-12-2
extern int glWPMRPMHistoryCount;
extern int glCurWPMRPMHistory;

void EncoderToHex( const BACnetAPDUEncoder &enc, CString &str );

BACnetAPDUEncoder CSendWritePropMult::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendWritePropMult dialog

IMPLEMENT_DYNCREATE( CSendWritePropMult, CPropertyPage )

#pragma warning( disable : 4355 )
CSendWritePropMult::CSendWritePropMult( void )
	: CSendPage( CSendWritePropMult::IDD )
	, m_PropListList( this )
{
	//{{AFX_DATA_INIT(CSendWritePropMult)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendWritePropMult::DoDataExchange(CDataExchange* pDX)
{
	WritePropListPtr	wplp = m_PropListList.wpllCurElem
	;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWritePropMult)
	DDX_Control(pDX, IDC_OBJLIST, m_ObjList);
	DDX_Control(pDX, IDC_PROPLIST, m_PropList);
	//}}AFX_DATA_MAP

	// if there is a selected object, allow the ObjID to update
	if (wplp)
		wplp->wplObjID.UpdateData( pDX->m_bSaveAndValidate );

	// if there is a selected property, allow the controls to update
	if (wplp && wplp->wplCurElem) {
		wplp->wplCurElem->wpePropCombo.UpdateData( pDX->m_bSaveAndValidate );
		wplp->wplCurElem->wpeArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
		wplp->wplCurElem->wpePriority.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(CSendWritePropMult, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWritePropMult)
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
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_EN_CHANGE(IDC_PRIORITYX, OnChangePriority)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWritePropMult::InitPage
//

void CSendWritePropMult::InitPage( void )
{
	TRACE0( "CSendWritePropMult::InitPage()\n" );
}

//
//	CSendWritePropMult::EncodePage
//

void CSendWritePropMult::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 16 );

	// encode the contents
	m_PropListList.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendWritePropMult::SavePage
//

void CSendWritePropMult::SavePage( void )
{
	TRACE0( "CSendWritePropMult::SavePage\n" );

	glWPMRPMListList[glCurWPMRPMHistory].RemoveAll(); //clear all data first
	
	for( int i = 0; i < m_PropListList.GetCount(); i++)
	{	
		WPMRPMListPtr wpmrplistPtr = new WPMRPMList();
        WritePropListPtr wplistPtr = m_PropListList.GetAt( m_PropListList.FindIndex(i) );

		wpmrplistPtr->m_ObjID = wplistPtr->wplObjID;        
		wpmrplistPtr->m_ObjIDStr = m_ObjList.GetItemText(i, 0);

		for(int j = 0; j < wplistPtr->GetCount(); j++)
		{
			WritePropElemPtr elemPtr = wplistPtr->GetAt(wplistPtr->FindIndex(j));
            WPMRPMElemPtr wpmrpmelemPtr = new WPMRPMElem();
			
			wpmrpmelemPtr->m_prop.m_enumValue = elemPtr->wpePropCombo.m_enumValue;
			wpmrpmelemPtr->m_array.uintValue = elemPtr->wpeArrayIndex.uintValue;
            wpmrpmelemPtr->m_value = elemPtr->wpeValue;	
			wpmrpmelemPtr->m_priority.uintValue = elemPtr->wpePriority.uintValue;

			wpmrplistPtr->AddTail(wpmrpmelemPtr);
		}

        glWPMRPMListList[glCurWPMRPMHistory].AddTail(wpmrplistPtr);
	}
}

//
//	CSendWritePropMult::RestorePage
//

void CSendWritePropMult::RestorePage( int index )
{	
	TRACE0( "CSendWritePropMult::RestorePage\n" );

    if(glWPMRPMHistoryCount < 1)
		return;
	
	if(index > glWPMRPMHistoryCount)
		return;

	index = glCurWPMRPMHistory - index - 1;
	if(index < 0)
		index = index + glMaxHistoryCount;

	//restore data. Xiao Shiyuan 2002-12-3	
	if(isShown)
	{
		ClearAll();
	}	  
	
	//restore data. Xiao Shiyuan 2002-12-3	
	for(int i = 0; i < glWPMRPMListList[index].GetCount(); i++)
	{	
		WritePropListPtr wplistPtr = new WritePropList(this);
		WPMRPMListPtr wpmrplistPtr = glWPMRPMListList[index].GetAt(glWPMRPMListList[index].FindIndex(i));

		BACnetObjectIdentifier *p = &(wplistPtr->wplObjID);		
		*p = wpmrplistPtr->m_ObjID;

		if(isShown)
			wplistPtr->wplObjID.ObjToCtrl();

		if(isShown)
			m_ObjList.InsertItem(i, wpmrplistPtr->m_ObjIDStr); 
		else
			m_strList.AddTail(wpmrplistPtr->m_ObjIDStr);

        POSITION elempos = wpmrplistPtr->GetHeadPosition();

		for(int j = 0; j < wpmrplistPtr->GetCount(); j++)
		{
			WritePropElemPtr elemPtr = new WritePropElem(this);
            WPMRPMElemPtr wpmrpmelemPtr = wpmrplistPtr->GetAt(wpmrplistPtr->FindIndex(j));
			
			elemPtr->wpePropCombo.m_enumValue = wpmrpmelemPtr->m_prop.m_enumValue;
			elemPtr->wpeArrayIndex.uintValue = wpmrpmelemPtr->m_array.uintValue;

			elemPtr->wpeArrayIndex.ctrlNull = FALSE; //value specified
            elemPtr->wpeValue = wpmrpmelemPtr->m_value; 
			
			if(isShown)
			{
				elemPtr->wpePropCombo.ObjToCtrl();
				elemPtr->wpeArrayIndex.ObjToCtrl();			
			}
			
			wplistPtr->AddTail( elemPtr);
		}

        m_PropListList.AddTail(wplistPtr);
	}
}


void CSendWritePropMult::ClearAll()
{
	m_ObjList.DeleteAllItems();
	m_PropList.DeleteAllItems();

	for (POSITION pos = m_PropListList.GetHeadPosition(); pos != NULL; )
	  delete m_PropListList.GetNext( pos );		

	m_PropListList.RemoveAll();
}
//
//	CSendWritePropMult::OnInitDialog
//

BOOL CSendWritePropMult::OnInitDialog() 
{
	TRACE0( "CSendWritePropMult::OnInitDialog()\n" );
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
	m_PropList.InsertColumn( 0, "Property", LVCFMT_LEFT, 96 );
	m_PropList.InsertColumn( 1, "Index", LVCFMT_RIGHT, 48 );
	m_PropList.InsertColumn( 2, "Value", LVCFMT_LEFT, 96 );
	m_PropList.InsertColumn( 3, "Priority", LVCFMT_RIGHT, 48 );

	// disable the controls, they'll be enabled when an object is selected
	GetDlgItem( IDC_OBJID )->EnableWindow( false );
	GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );
	GetDlgItem( IDC_ADDPROP )->EnableWindow( false );
	GetDlgItem( IDC_REMOVEPROP )->EnableWindow( false );
	GetDlgItem( IDC_PROPCOMBO )->EnableWindow( false );
	GetDlgItem( IDC_ARRAYINDEX )->EnableWindow( false );
	GetDlgItem( IDC_VALUE )->EnableWindow( false );
	GetDlgItem( IDC_PRIORITYX )->EnableWindow( false );
	
	//Xiao Shiyuan 2002-12-5
	for(i = 0; i < m_strList.GetCount(); i++)    
		m_ObjList.InsertItem(i, m_strList.GetAt(m_strList.FindIndex(i)));    

	
	return TRUE;
}

void CSendWritePropMult::OnAddObj() 
{
	m_PropListList.AddButtonClick();
}

void CSendWritePropMult::OnRemoveObj() 
{
	m_PropListList.RemoveButtonClick();
}

void CSendWritePropMult::OnObjUp() 
{
	m_PropListList.UpButtonClick();
}

void CSendWritePropMult::OnObjDown() 
{
	m_PropListList.DownButtonClick();
}

void CSendWritePropMult::OnChangeObjID() 
{
	m_PropListList.OnChangeObjID();
}

void CSendWritePropMult::OnObjectIDButton() 
{
	m_PropListList.OnObjectIDButton();
}

void CSendWritePropMult::OnItemchangingObjList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_PropListList.OnItemChanging( pNMHDR, pResult );
}

void CSendWritePropMult::OnAddProp() 
{
	if (m_PropListList.wpllCurElem)
		m_PropListList.wpllCurElem->AddButtonClick();
}

void CSendWritePropMult::OnRemoveProp() 
{
	if (m_PropListList.wpllCurElem)
		m_PropListList.wpllCurElem->RemoveButtonClick();
}

void CSendWritePropMult::OnPropUp() 
{
	if (m_PropListList.wpllCurElem)
		m_PropListList.wpllCurElem->UpButtonClick();
}

void CSendWritePropMult::OnPropDown() 
{
	if (m_PropListList.wpllCurElem)
		m_PropListList.wpllCurElem->DownButtonClick();
}

void CSendWritePropMult::OnItemchangingPropList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_PropListList.wpllCurElem)
		m_PropListList.wpllCurElem->OnItemChanging( pNMHDR, pResult );
}

void CSendWritePropMult::OnSelchangePropCombo()
{
	if (m_PropListList.wpllCurElem)
		m_PropListList.wpllCurElem->OnSelchangePropCombo();
}

void CSendWritePropMult::OnChangeArrayIndex()
{
	if (m_PropListList.wpllCurElem)
		m_PropListList.wpllCurElem->OnChangeArrayIndex();
}

void CSendWritePropMult::OnValue() 
{
	if (m_PropListList.wpllCurElem)
		m_PropListList.wpllCurElem->OnValue();
}

void CSendWritePropMult::OnChangePriority()
{
	if (m_PropListList.wpllCurElem)
		m_PropListList.wpllCurElem->OnChangePriority();
}




// madanner 9/04, added from EPICS view

void CSendWritePropMult::ForceValues(BACnetObjectIdentifier * pObjectID, int apPropID[], CPtrArray * apbacnetValues, int nSize )
{
	ClearAll();

	m_PropListList.ForceValues(pObjectID, apPropID, apbacnetValues, nSize);

	SavePage();
	UpdateEncoded();
}



//
//	WritePropElem::WritePropElem
//

WritePropElem::WritePropElem( CSendPagePtr wp )
	: wpePropCombo( wp, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, wpeArrayIndex( wp, IDC_ARRAYINDEX )
	, wpePriority( wp, IDC_PRIORITYX )
	, wpeValue(wp)			// for proper parent control
{
	// controls start out disabled
	wpePropCombo.ctrlEnabled = false;
	wpeArrayIndex.ctrlEnabled = false;
	wpePriority.ctrlEnabled = false;
}

//
//	WritePropElem::Bind
//

void WritePropElem::Bind( void )
{
	// set the control value to this element values
	wpePropCombo.ObjToCtrl();
	wpePropCombo.Enable();
	wpeArrayIndex.ObjToCtrl();
	wpeArrayIndex.Enable();

	wpePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( true );

	wpePriority.ObjToCtrl();
	wpePriority.Enable();
}

//
//	WritePropElem::Unbind
//

void WritePropElem::Unbind( void )
{
	// clear out the contents of the controls
	wpePropCombo.ctrlWindow->GetDlgItem( IDC_PROPCOMBO )->SetWindowText( "" );
	wpePropCombo.Disable();
	wpeArrayIndex.ctrlWindow->GetDlgItem( IDC_ARRAYINDEX )->SetWindowText( "" );
	wpeArrayIndex.Disable();

	wpePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( false );

	wpePriority.ctrlWindow->GetDlgItem( IDC_PRIORITYX )->SetWindowText( "" );
	wpePriority.Disable();
}

//
//	WritePropElem::Encode
//

void WritePropElem::Encode( BACnetAPDUEncoder& enc )
{
	// encode the property
	if (wpePropCombo.ctrlNull)
		throw "Property ID required";
	wpePropCombo.Encode( enc, 0 );

	// encode the (optional) array index
	if (!wpeArrayIndex.ctrlNull)
		wpeArrayIndex.Encode( enc, 1 );

	// do the value
	// Allow empty value: some properties are List-Of, with empty list allowed
//	if (wpeValue.m_anyList.Length() < 1)
//		throw "Value required";
	wpeValue.Encode( enc, 2 );

	// encode the (optional) priority
	if (!wpePriority.ctrlNull) {
		if ((wpePriority.uintValue < 1) || (wpePriority.uintValue > 16))
			throw "Priority out of range 1..16";
		wpePriority.Encode( enc, 3 );
	}
}

//
//	WritePropList::WritePropList
//

WritePropList::WritePropList( CSendWritePropMultPtr pp )
	: wplPagePtr( pp )
	, wplCurElem(0), wplCurElemIndx(0)
	, wplObjID( pp, IDC_OBJID )
{
	// give the object ID a default value
	wplObjID.ctrlEnabled = false;
	wplObjID.ctrlNull = false;
	wplObjID.objID = 0;

	wplAddInProgress = FALSE; //Xiao Shiyuan 2002-12-3
}

//
//	WritePropList::~WritePropList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

WritePropList::~WritePropList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	WritePropList::Bind
//

void WritePropList::Bind( void )
{
	int			i
	;
	CString		someText
	;

	// set the control value to this object id
	wplObjID.ObjToCtrl();
	wplObjID.Enable();
	wplPagePtr->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( true );

	// make sure the table is cleaned out
	wplPagePtr->m_PropList.DeleteAllItems();

	// fill out the table with the current list of elements
	i = 0;
	for (POSITION pos = GetHeadPosition(); pos != NULL; i++ ) {
		WritePropElemPtr	wpep = GetNext( pos )
		;

		wplPagePtr->m_PropList.InsertItem( i
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( wpep->wpePropCombo.m_enumValue )
			);
		if (wpep->wpeArrayIndex.ctrlNull)
			wplPagePtr->m_PropList.SetItemText( i, 1, "" );
		else {
			someText.Format( "%d", wpep->wpeArrayIndex.uintValue );
			wplPagePtr->m_PropList.SetItemText( i, 1, someText );
		}

		BACnetAPDUEncoder enc;
		wpep->wpeValue.Encode( enc );
		EncoderToHex( enc, someText );
		wplPagePtr->m_PropList.SetItemText( i, 2, someText );

		if (wpep->wpePriority.ctrlNull)
			wplPagePtr->m_PropList.SetItemText( i, 3, "" );
		else {
			someText.Format( "%d", wpep->wpePriority.uintValue );
			wplPagePtr->m_PropList.SetItemText( i, 3, someText );
		}
	}

	// enable the other controls
	wplPagePtr->GetDlgItem( IDC_ADDPROP )->EnableWindow( true );
	wplPagePtr->GetDlgItem( IDC_REMOVEPROP )->EnableWindow( true );
}

//
//	WritePropList::Unbind
//

void WritePropList::Unbind( void )
{
	// clear out the contents of the object id control
	wplObjID.ctrlWindow->GetDlgItem( IDC_OBJID )->SetWindowText( "" );
	wplObjID.Disable();
	wplPagePtr->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );

	// wipe out the contents of the table
	wplPagePtr->m_PropList.DeleteAllItems();

	// disable the other controls
	wplPagePtr->GetDlgItem( IDC_ADDPROP )->EnableWindow( false );
	wplPagePtr->GetDlgItem( IDC_REMOVEPROP )->EnableWindow( false );
}

//
//	WritePropList::AddButtonClick
//

void WritePropList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = wplPagePtr->m_PropList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = wplPagePtr->m_PropList.GetNextSelectedItem( selPos );
		wplPagePtr->m_PropList.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	wplAddInProgress = true;
	wplPagePtr->m_PropList.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	wplCurElem = new WritePropElem( wplPagePtr );
	wplCurElemIndx = listLen;

	wplCurElem->wpePropCombo.m_nObjType = wplObjID.GetObjectType();
	wplCurElem->wpePropCombo.LoadCombo();

	wplCurElem->wpePropCombo.SetEnumValue( PRESENT_VALUE );
	AddTail( wplCurElem );

	// bind the element to the controls
	wplCurElem->Bind();

	// update the encoding
	wplAddInProgress = false;

	// madanner, 8/26/02.  Sourceforge bug #472392
	OnSelchangePropCombo();				// Insert new list text for Present_Value
	wplPagePtr->m_PropList.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
//	wplPagePtr->UpdateEncoded();
}

//
//	WritePropList::RemoveButtonClick
//

void WritePropList::RemoveButtonClick( void )
{
	int					curRow = wplCurElemIndx
	;
	WritePropElemPtr	curElem = wplCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	wplPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	wplPagePtr->m_PropList.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	wplPagePtr->m_PropList.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	wplPagePtr->UpdateEncoded();
}

//
//	WritePropList::UpButtonClick
//

void WritePropList::UpButtonClick( void )
{
	int					curRow = wplCurElemIndx
	,					prevRow = curRow - 1
	;
	WritePropElemPtr	curElem = wplCurElem
	;

	// must have a selected row and a previous row
	if (curRow < 1)
		return;

	// move the row in the list
	wplPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );
	wplPagePtr->m_PropList.DeleteItem( curRow );
	wplPagePtr->m_PropList.InsertItem( prevRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION prevPos = FindIndex( prevRow );
	InsertBefore( prevPos, curElem );

	// current element moved up
	wplCurElemIndx -= 1;

	// select the row in its new position
	wplPagePtr->m_PropList.SetItemState( prevRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events (dont ask why we dont!)
	OnSelchangePropCombo();
	OnChangeArrayIndex();
	if (wplCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		wplCurElem->wpeValue.Encode( enc );
		EncoderToHex( enc, someText );
		wplPagePtr->m_PropList.SetItemText( wplCurElemIndx, 2, someText );
	}
	OnChangePriority();

	// update the encoding
	wplPagePtr->UpdateEncoded();
}

//
//	WritePropList::DownButtonClick
//

void WritePropList::DownButtonClick( void )
{
	int					curRow = wplCurElemIndx
	,					nextRow = curRow + 1
	;
	WritePropElemPtr	curElem = wplCurElem
	;

	// must have a selected row and a following row
	if ((curRow < 0) || (curRow >= GetCount()-1))
		return;

	// move the row in the list
	wplPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );
	wplPagePtr->m_PropList.DeleteItem( curRow );
	wplPagePtr->m_PropList.InsertItem( nextRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION nextPos = FindIndex( curRow );
	InsertAfter( nextPos, curElem );

	// current element moved down
	wplCurElemIndx += 1;

	// select the row in its new position
	wplPagePtr->m_PropList.SetItemState( nextRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events (dont ask why we dont!)
	OnSelchangePropCombo();
	OnChangeArrayIndex();
	if (wplCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		wplCurElem->wpeValue.Encode( enc );
		EncoderToHex( enc, someText );
		wplPagePtr->m_PropList.SetItemText( wplCurElemIndx, 2, someText );
	}
	OnChangePriority();

	// update the encoding
	wplPagePtr->UpdateEncoded();
}

//
//	WritePropList::OnSelchangePropCombo
//

void WritePropList::OnSelchangePropCombo( void )
{
	if (wplCurElem) {
		wplCurElem->wpePropCombo.UpdateData();
		wplPagePtr->UpdateEncoded();

		if ( wplCurElem->wpePropCombo.m_enumValue < 512 )
		{
			wplPagePtr->m_PropList.SetItemText( wplCurElemIndx, 0
				, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( wplCurElem->wpePropCombo.m_enumValue )
				);
		}
		else
		{
			CString c;
			c.Format("%d", wplCurElem->wpePropCombo.m_enumValue );
			wplPagePtr->m_PropList.SetItemText( wplCurElemIndx, 0, c );
		}
	}
}

//
//	WritePropList::OnChangeArrayIndex
//

void WritePropList::OnChangeArrayIndex( void )
{
	if (wplCurElem) {
		CString		someText
		;

		wplCurElem->wpeArrayIndex.UpdateData();
		wplPagePtr->UpdateEncoded();

		if (wplCurElem->wpeArrayIndex.ctrlNull)
			wplPagePtr->m_PropList.SetItemText( wplCurElemIndx, 1, "" );
		else {
			someText.Format( "%d", wplCurElem->wpeArrayIndex.uintValue );
			wplPagePtr->m_PropList.SetItemText( wplCurElemIndx, 1, someText );
		}
	}
}

//
//	WritePropList::OnChangeValue
//

void WritePropList::OnValue( void )
{
	if (wplCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		wplCurElem->wpeValue.DoModal();
		wplPagePtr->UpdateEncoded();

		wplCurElem->wpeValue.Encode( enc );
		EncoderToHex( enc, someText );
		wplPagePtr->m_PropList.SetItemText( wplCurElemIndx, 2, someText );
	}
}

//
//	WritePropList::OnChangePriority
//

void WritePropList::OnChangePriority( void )
{
	if (wplCurElem) {
		CString				someText
		;

		wplCurElem->wpePriority.UpdateData();
		wplPagePtr->UpdateEncoded();

		if (wplCurElem->wpePriority.ctrlNull)
			wplPagePtr->m_PropList.SetItemText( wplCurElemIndx, 3, "" );
		else {
			someText.Format( "%d", wplCurElem->wpePriority.uintValue );
			wplPagePtr->m_PropList.SetItemText( wplCurElemIndx, 3, someText );
		}
	}
}

//
//	WritePropList::OnItemChanging
//

void WritePropList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = wplCurElemIndx
	;
	WritePropElemPtr	curElem = wplCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (wplAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		// item becoming selected
		wplCurElemIndx = pNMListView->iItem;
		wplCurElem = GetAt( FindIndex( wplCurElemIndx ) );

		// bind the new current element
		wplCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == wplCurElemIndx) {
			// set nothing selected
			wplCurElem = 0;
			wplCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	WritePropList::Encode
//

void WritePropList::Encode( BACnetAPDUEncoder& enc )
{
	// encode the object ID
	if (wplObjID.ctrlNull)
		throw "Object ID required";
	wplObjID.Encode( enc, 0 );

	// encode each of the bound properties
	BACnetOpeningTag().Encode( enc, 1 );
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
	BACnetClosingTag().Encode( enc, 1 );
}

//
//	WritePropListList::WritePropListList
//

WritePropListList::WritePropListList( CSendWritePropMultPtr pp )
	: wpllPagePtr( pp )
	, wpllCurElem(0), wpllCurElemIndx(-1)
{
	wpllAddInProgress = FALSE; //Xiao Shiyuan 2002-12-3
}

//
//	WritePropListList::~WritePropListList
//
//	If there are any objects that have been added to the list they need to be 
//	deleted here.  The CList class will delete the stuff that it knows how to 
//	handle, but that doesn't include the WritePropList objects.
//

WritePropListList::~WritePropListList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	WritePropListList::AddButtonClick
//
//	This procedure is called when the user wants to add an additional object to
//	the list.
//

void WritePropListList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = wpllPagePtr->m_ObjList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = wpllPagePtr->m_ObjList.GetNextSelectedItem( selPos );
		wpllPagePtr->m_ObjList.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	wpllAddInProgress = true;
	wpllPagePtr->m_ObjList.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	wpllCurElem = new WritePropList( wpllPagePtr );
	wpllCurElemIndx = listLen;
	AddTail( wpllCurElem );

	// bind the element to the controls
	wpllCurElem->Bind();

	// update the encoding
	wpllAddInProgress = false;
	wpllPagePtr->m_ObjList.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
	wpllPagePtr->UpdateEncoded();
}


void WritePropListList::ForceValues(BACnetObjectIdentifier * pObjectID, int apPropID[], CPtrArray * apbacnetValues, int nSize )
{
	// create a new list item
	wpllAddInProgress = true;
	wpllPagePtr->m_ObjList.InsertItem( 0, "" );
	wpllCurElemIndx = 0;

	WritePropListPtr wplistPtr = new WritePropList(wpllPagePtr);
		
	wplistPtr->wplObjID.ctrlNull = FALSE;
	wplistPtr->wplObjID.objID = pObjectID->objID;

//      m_strList.AddTail(wpmrplistPtr->m_ObjIDStr);

	for ( int j = 0; j < nSize; j++)
	{
		WritePropElemPtr elemPtr = new WritePropElem(wpllPagePtr);
			
		elemPtr->wpePropCombo.m_enumValue = apPropID[j];
		elemPtr->wpePropCombo.ctrlNull = FALSE;
		elemPtr->wpeArrayIndex.uintValue = 0;
		elemPtr->wpeArrayIndex.ctrlNull = TRUE;
			
		elemPtr->wpeValue.m_anyList.KillAll();
		elemPtr->wpeValue.AddValue( (BACnetEncodeable *)(*apbacnetValues)[j] );
		elemPtr->wpePriority.ctrlNull = TRUE;

		wplistPtr->AddTail( elemPtr);
	}

	AddTail(wplistPtr);
	wplistPtr->Bind();

	// update the encoding
	wpllAddInProgress = false;
	wpllPagePtr->m_ObjList.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED);
}


//
//	WritePropListList::RemoveButtonClick
//

void WritePropListList::RemoveButtonClick( void )
{
	int					curRow = wpllCurElemIndx
	;
	WritePropListPtr	curElem = wpllCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	wpllPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	wpllPagePtr->m_ObjList.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	wpllPagePtr->m_ObjList.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	wpllPagePtr->UpdateEncoded();
}

//
//	WritePropListList::UpButtonClick
//

void WritePropListList::UpButtonClick( void )
{
	int				curRow = wpllCurElemIndx
	,				prevRow = curRow - 1
	;
	WritePropListPtr	curElem = wpllCurElem
	;

	// must have a selected row and a previous row
	if (curRow < 1)
		return;

	// move the row in the list
	wpllPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );
	wpllPagePtr->m_ObjList.DeleteItem( curRow );
	wpllPagePtr->m_ObjList.InsertItem( prevRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION prevPos = FindIndex( prevRow );
	InsertBefore( prevPos, curElem );

	// current element moved up
	wpllCurElemIndx -= 1;

	// select the row in its new position
	wpllPagePtr->m_ObjList.SetItemState( prevRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events
	OnChangeObjID();

	// update the encoding
	wpllPagePtr->UpdateEncoded();
}

//
//	WritePropListList::DownButtonClick
//

void WritePropListList::DownButtonClick( void )
{
	int					curRow = wpllCurElemIndx
	,					nextRow = curRow + 1
	;
	WritePropListPtr	curElem = wpllCurElem
	;

	// must have a selected row and a following row
	if ((curRow < 0) || (curRow >= GetCount()-1))
		return;

	// move the row in the list
	wpllPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );
	wpllPagePtr->m_ObjList.DeleteItem( curRow );
	wpllPagePtr->m_ObjList.InsertItem( nextRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION nextPos = FindIndex( curRow );
	InsertAfter( nextPos, curElem );

	// current element moved down
	wpllCurElemIndx += 1;

	// select the row in its new position
	wpllPagePtr->m_ObjList.SetItemState( nextRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events (dont ask why we dont!)
	OnChangeObjID();

	// update the encoding
	wpllPagePtr->UpdateEncoded();
}

//
//	WritePropListList::OnChangeObjID
//

void WritePropListList::OnChangeObjID( void )
{
	CString			rStr
	;

	// make sure we have a current object
	if (!wpllCurElem)
		return;

	// pass the message to the current selected object
	wpllCurElem->wplObjID.UpdateData();

	// get the control contents, update the list
	wpllPagePtr->GetDlgItem( IDC_OBJID )->GetWindowText( rStr );
	wpllPagePtr->m_ObjList.SetItemText( wpllCurElemIndx, 0, rStr );

	// update the encoding
	wpllPagePtr->UpdateEncoded();
}

//
//	WritePropListList::OnObjectIDButton
//

void WritePropListList::OnObjectIDButton( void )
{
	if (!wpllCurElem)
		return;

	VTSObjectIdentifierDialog	dlg(wpllPagePtr)			// for proper parent control
	;

	dlg.objID = wpllCurElem->wplObjID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		wpllCurElem->wplObjID.ctrlNull = false;
		wpllCurElem->wplObjID.objID = dlg.objID;
		wpllCurElem->wplObjID.ObjToCtrl();

		// copy the text from the control to the list
		CString text;
		wpllPagePtr->GetDlgItem( IDC_OBJID )->GetWindowText( text );
		wpllPagePtr->m_ObjList.SetItemText( wpllCurElemIndx, 0, text );

		wpllPagePtr->UpdateEncoded();
	}
}

//
//	WritePropListList::OnItemChanging
//

void WritePropListList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = wpllCurElemIndx
	;
	WritePropListPtr	curElem = wpllCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (wpllAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		// item becoming selected
		wpllCurElemIndx = pNMListView->iItem;
		wpllCurElem = GetAt( FindIndex( wpllCurElemIndx ) );

		// bind the new current element
		wpllCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == wpllCurElemIndx) {
			// set nothing selected
			wpllCurElem = 0;
			wpllCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	WritePropListList::Encode
//
//	Each WritePropList element understands how to encode itself, so this procedure 
//	just calls Encode() for each of the objects in its list.
//

void WritePropListList::Encode( BACnetAPDUEncoder& enc )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}

//Xiao Shiyuan 2002-12-2
void CSendWritePropMult::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	SavePage(); //save data first
	
	if(glWPMRPMHistoryCount < glMaxHistoryCount)
		glWPMRPMHistoryCount++;

	glCurWPMRPMHistory++;

	if(glCurWPMRPMHistory > glMaxHistoryCount - 1)
		glCurWPMRPMHistory = 0;		
}

void CSendWritePropMult::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	isShown = bShow;
	
	if(bShow)
	{
		pageParent->SetHistoryComboBox(glWPMRPMHistoryCount);
		pageParent->curPagePtr = this;
	}
}
