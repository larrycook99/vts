// SendReadPropMultACK.cpp : implementation file
//


#include "stdafx.h"
#include "VTS.h"
#include "propid.h"

#include "Send.h"
#include "SendReadPropMultACK.h"

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

BACnetAPDUEncoder CSendReadPropMultACK::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendReadPropMultACK dialog

IMPLEMENT_DYNCREATE( CSendReadPropMultACK, CPropertyPage )

#pragma warning( disable : 4355 )
CSendReadPropMultACK::CSendReadPropMultACK( void )
	: CSendPage( CSendReadPropMultACK::IDD )
	, m_PropListList( this )
{
	//{{AFX_DATA_INIT(CSendReadPropMultACK)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendReadPropMultACK::DoDataExchange(CDataExchange* pDX)
{
	ReadPropACKListPtr	rpalp = m_PropListList.rpallCurElem
	;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReadPropMultACK)
	DDX_Control(pDX, IDC_OBJLIST, m_ObjList);
	DDX_Control(pDX, IDC_PROPLIST, m_PropList);
	//}}AFX_DATA_MAP

	// if there is a selected object, allow the ObjID to update
	if (rpalp)
		rpalp->rpalObjID.UpdateData( pDX->m_bSaveAndValidate );

	// if there is a selected property, allow the controls to update
	if (rpalp && rpalp->rpalCurElem) {
		rpalp->rpalCurElem->rpaePropCombo.UpdateData( pDX->m_bSaveAndValidate );
		rpalp->rpalCurElem->rpaeArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
		rpalp->rpalCurElem->rpaeClassCombo.UpdateData( pDX->m_bSaveAndValidate );
		rpalp->rpalCurElem->rpaeCodeCombo.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(CSendReadPropMultACK, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReadPropMultACK)
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
	ON_CBN_SELCHANGE(IDC_ERRORCLASSCOMBO, OnSelchangeClassCombo)
	ON_CBN_SELCHANGE(IDC_ERRORCODECOMBO, OnSelchangeCodeCombo)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReadPropMultACK::InitPage
//

void CSendReadPropMultACK::InitPage( void )
{
	TRACE0( "CSendReadPropMultACK::InitPage()\n" );
}

//
//	CSendReadPropMultACK::EncodePage
//

void CSendReadPropMultACK::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 14 );

	// encode the contents
	m_PropListList.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendReadPropMultACK::SavePage
//

void CSendReadPropMultACK::SavePage( void )
{
	TRACE0( "CSendReadPropMultACK::SavePage\n" );

	glWPMRPMListList[glCurWPMRPMHistory].RemoveAll(); //clear all data first
	
	for( int i = 0; i < m_PropListList.GetCount(); i++)
	{	
		WPMRPMListPtr wpmrplistPtr = new WPMRPMList();
        ReadPropACKListPtr rplistPtr = m_PropListList.GetAt( m_PropListList.FindIndex(i) );

		wpmrplistPtr->m_ObjID = rplistPtr->rpalObjID;        
		wpmrplistPtr->m_ObjIDStr = m_ObjList.GetItemText(i, 0);

		for(int j = 0; j < rplistPtr->GetCount(); j++)
		{
			ReadPropACKElemPtr elemPtr = rplistPtr->GetAt(rplistPtr->FindIndex(j));
            WPMRPMElemPtr wpmrpmelemPtr = new WPMRPMElem();
			
			wpmrpmelemPtr->m_prop.m_enumValue = elemPtr->rpaePropCombo.m_enumValue;
			wpmrpmelemPtr->m_array.uintValue = elemPtr->rpaeArrayIndex.uintValue;
            wpmrpmelemPtr->m_class.m_enumValue = elemPtr->rpaeClassCombo.m_enumValue;
			wpmrpmelemPtr->m_code.m_enumValue = elemPtr->rpaeCodeCombo.m_enumValue;
			wpmrpmelemPtr->m_value = elemPtr->rpaeValue;
			wpmrplistPtr->AddTail(wpmrpmelemPtr);
		}

        glWPMRPMListList[glCurWPMRPMHistory].AddTail(wpmrplistPtr);
	}
}

//
//	CSendReadPropMultACK::RestorePage
//

void CSendReadPropMultACK::RestorePage( int index )
{	
	TRACE0( "CSendReadPropMultACK::RestorePage\n" );
	if(glWPMRPMHistoryCount < 1)
		return;
	
	if(index > glWPMRPMHistoryCount)
		return;

	index = glCurWPMRPMHistory - index - 1;
	if(index < 0)
		index = index + glMaxHistoryCount;

	if(isShown)
	{
		m_ObjList.DeleteAllItems();
		m_PropList.DeleteAllItems();

		for (POSITION pos = m_PropListList.GetHeadPosition(); pos != NULL; )
		  delete m_PropListList.GetNext( pos );		

		m_PropListList.RemoveAll();
	}

	//restore data. Xiao Shiyuan 2002-12-3	
	for(int i = 0; i < glWPMRPMListList[index].GetCount(); i++)
	{	
		ReadPropACKListPtr rplistPtr = new ReadPropACKList(this);
		WPMRPMListPtr wpmrplistPtr = glWPMRPMListList[index].GetAt(glWPMRPMListList[index].FindIndex(i));

		BACnetObjectIdentifier *p = &(rplistPtr->rpalObjID);		
		*p = wpmrplistPtr->m_ObjID;

		if(isShown)
		  rplistPtr->rpalObjID.ObjToCtrl();

		if(isShown)
		  m_ObjList.InsertItem(i, wpmrplistPtr->m_ObjIDStr); 
		else
          m_strList.AddTail(wpmrplistPtr->m_ObjIDStr);

		for(int j = 0; j < wpmrplistPtr->GetCount(); j++)
		{
			ReadPropACKElemPtr elemPtr = new ReadPropACKElem(this);
            WPMRPMElemPtr wpmrpmelemPtr = wpmrplistPtr->GetAt(wpmrplistPtr->FindIndex(j));
			
			elemPtr->rpaePropCombo.m_enumValue = wpmrpmelemPtr->m_prop.m_enumValue;
			elemPtr->rpaeArrayIndex.uintValue = wpmrpmelemPtr->m_array.uintValue;
			elemPtr->rpaeClassCombo.m_enumValue = wpmrpmelemPtr->m_class.m_enumValue;
			elemPtr->rpaeCodeCombo.m_enumValue = wpmrpmelemPtr->m_code.m_enumValue;
			elemPtr->rpaeValue = wpmrpmelemPtr->m_value;

			elemPtr->rpaeArrayIndex.ctrlNull = FALSE; //value specified

			if(isShown)
			{
				elemPtr->rpaePropCombo.ObjToCtrl();
				elemPtr->rpaeArrayIndex.ObjToCtrl();
			}

			rplistPtr->AddTail(elemPtr);
		}

		m_PropListList.AddTail(rplistPtr);
	}
}

//
//	CSendReadPropMultACK::OnInitDialog
//

BOOL CSendReadPropMultACK::OnInitDialog() 
{
	int i;					// MAG 11AUG05 add this line, remove local declaration below since i is used out of that scope
	CComboBox	*cbp;

	TRACE0( "CSendReadPropMultACK::OnInitDialog()\n" );

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
	m_PropList.InsertColumn( 3, "Class", LVCFMT_RIGHT, 48 );
	m_PropList.InsertColumn( 4, "Code", LVCFMT_RIGHT, 48 );

	// disable the controls, they'll be enabled when an object is selected
	GetDlgItem( IDC_OBJID )->EnableWindow( false );
	GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );
	GetDlgItem( IDC_ADDPROP )->EnableWindow( false );
	GetDlgItem( IDC_REMOVEPROP )->EnableWindow( false );
	GetDlgItem( IDC_PROPCOMBO )->EnableWindow( false );
	GetDlgItem( IDC_ARRAYINDEX )->EnableWindow( false );
	GetDlgItem( IDC_VALUE )->EnableWindow( false );
	GetDlgItem( IDC_ERRORCLASSCOMBO )->EnableWindow( false );
	GetDlgItem( IDC_ERRORCODECOMBO )->EnableWindow( false );

	// load the error class enumeration table
	cbp = (CComboBox *)GetDlgItem( IDC_ERRORCLASSCOMBO );
	NetworkSniffer::BAC_STRTAB_BACnetErrorClass.FillCombo( *cbp );

	// load the error code enumeration table
	cbp = (CComboBox *)GetDlgItem( IDC_ERRORCODECOMBO );
	NetworkSniffer::BAC_STRTAB_BACnetErrorCode.FillCombo( *cbp );

	//Xiao Shiyuan 2002-12-5
	for(i = 0; i < m_strList.GetCount(); i++)
		m_ObjList.InsertItem(i, m_strList.GetAt(m_strList.FindIndex(i))); 

	return TRUE;
}

void CSendReadPropMultACK::OnAddObj() 
{
	m_PropListList.AddButtonClick();
}

void CSendReadPropMultACK::OnRemoveObj() 
{
	m_PropListList.RemoveButtonClick();
}

void CSendReadPropMultACK::OnObjUp() 
{
	m_PropListList.UpButtonClick();
}

void CSendReadPropMultACK::OnObjDown() 
{
	m_PropListList.DownButtonClick();
}

void CSendReadPropMultACK::OnChangeObjID() 
{
	m_PropListList.OnChangeObjID();
}

void CSendReadPropMultACK::OnObjectIDButton() 
{
	m_PropListList.OnObjectIDButton();
}

void CSendReadPropMultACK::OnItemchangingObjList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_PropListList.OnItemChanging( pNMHDR, pResult );
}

void CSendReadPropMultACK::OnAddProp() 
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->AddButtonClick();
}

void CSendReadPropMultACK::OnRemoveProp() 
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->RemoveButtonClick();
}

void CSendReadPropMultACK::OnPropUp() 
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->UpButtonClick();
}

void CSendReadPropMultACK::OnPropDown() 
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->DownButtonClick();
}

void CSendReadPropMultACK::OnItemchangingPropList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->OnItemChanging( pNMHDR, pResult );
}

void CSendReadPropMultACK::OnSelchangePropCombo()
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->OnSelchangePropCombo();
}

void CSendReadPropMultACK::OnChangeArrayIndex()
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->OnChangeArrayIndex();
}

void CSendReadPropMultACK::OnValue() 
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->OnValue();
}

void CSendReadPropMultACK::OnSelchangeClassCombo()
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->OnSelchangeClassCombo();
}

void CSendReadPropMultACK::OnSelchangeCodeCombo()
{
	if (m_PropListList.rpallCurElem)
		m_PropListList.rpallCurElem->OnSelchangeCodeCombo();
}

//
//	ReadPropACKElem::ReadPropACKElem
//

ReadPropACKElem::ReadPropACKElem( CSendPagePtr wp )
	: rpaePropCombo( wp, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, rpaeArrayIndex( wp, IDC_ARRAYINDEX )
	, rpaeClassCombo( wp, IDC_ERRORCLASSCOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorClass, true )
	, rpaeCodeCombo( wp, IDC_ERRORCODECOMBO, NetworkSniffer::BAC_STRTAB_BACnetErrorCode, true )
	, rpaeValue(wp)
{
	// controls start out disabled
	rpaePropCombo.ctrlEnabled = false;
	rpaeArrayIndex.ctrlEnabled = false;
	rpaeClassCombo.ctrlEnabled = false;
	rpaeCodeCombo.ctrlEnabled = false;
}

//
//	ReadPropACKElem::Bind
//

void ReadPropACKElem::Bind( void )
{
	// set the control value to this element values
	rpaePropCombo.ObjToCtrl();
	rpaePropCombo.Enable();
	rpaeArrayIndex.ObjToCtrl();
	rpaeArrayIndex.Enable();

	rpaePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( true );

	rpaeClassCombo.ObjToCtrl();
	rpaeClassCombo.Enable();
	rpaeCodeCombo.ObjToCtrl();
	rpaeCodeCombo.Enable();
}

//
//	ReadPropACKElem::Unbind
//

void ReadPropACKElem::Unbind( void )
{
	// clear out the contents of the controls
	rpaePropCombo.ctrlWindow->GetDlgItem( IDC_PROPCOMBO )->SetWindowText( "" );
	rpaePropCombo.Disable();
	rpaeArrayIndex.ctrlWindow->GetDlgItem( IDC_ARRAYINDEX )->SetWindowText( "" );
	rpaeArrayIndex.Disable();

	rpaePropCombo.ctrlWindow->GetDlgItem( IDC_VALUE )->EnableWindow( false );

	rpaeClassCombo.ctrlWindow->GetDlgItem( IDC_ERRORCLASSCOMBO )->SetWindowText( "" );
	rpaeClassCombo.Disable();
	rpaeCodeCombo.ctrlWindow->GetDlgItem( IDC_ERRORCODECOMBO )->SetWindowText( "" );
	rpaeCodeCombo.Disable();
}

//
//	ReadPropACKElem::Encode
//

void ReadPropACKElem::Encode( BACnetAPDUEncoder& enc )
{
	// encode the property
	if (rpaePropCombo.ctrlNull)
		throw "Property ID required";
	rpaePropCombo.Encode( enc, 2 );

	// encode the (optional) array index
	if (!rpaeArrayIndex.ctrlNull)
		rpaeArrayIndex.Encode( enc, 3 );

	// do the value
	if (rpaeValue.m_anyList.Length() > 0) {
		if (!rpaeClassCombo.ctrlNull)
			throw "Omit error class with value";
		if (!rpaeCodeCombo.ctrlNull)
			throw "Omit error code with value";

		rpaeValue.Encode( enc, 4 );
	} else {
		if (rpaeClassCombo.ctrlNull && rpaeCodeCombo.ctrlNull)
		{
			// Allow empty value: some properties are List-Of, with empty list allowed
		}
		else
		{
			if (rpaeClassCombo.ctrlNull)
				throw "Error class required with error code";
			if (rpaeCodeCombo.ctrlNull)
				throw "Error code required with error class";

			BACnetOpeningTag().Encode( enc, 5 );
			rpaeClassCombo.Encode( enc );
			rpaeCodeCombo.Encode( enc );
			BACnetClosingTag().Encode( enc, 5 );
		}
	}
}

//
//	ReadPropACKList::ReadPropACKList
//

ReadPropACKList::ReadPropACKList( CSendReadPropMultACKPtr pp )
	: rpalPagePtr( pp )
	, rpalCurElem(0), rpalCurElemIndx(0)
	, rpalObjID( pp, IDC_OBJID )
{
	// give the object ID a default value
	rpalObjID.ctrlEnabled = false;
	rpalObjID.ctrlNull = false;
	rpalObjID.objID = 0;

	rpalAddInProgress = FALSE;
}

//
//	ReadPropACKList::~ReadPropACKList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

ReadPropACKList::~ReadPropACKList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	ReadPropACKList::Bind
//

void ReadPropACKList::Bind( void )
{
	int			i
	;
	CString		someText
	;

	// set the control value to this object id
	rpalObjID.ObjToCtrl();
	rpalObjID.Enable();
	rpalPagePtr->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( true );

	// clear out the current table contents
	rpalPagePtr->m_PropList.DeleteAllItems();

	// fill out the table with the current list of elements
	i = 0;
	for (POSITION pos = GetHeadPosition(); pos != NULL; i++ ) {
		ReadPropACKElemPtr	rpaep = GetNext( pos )
		;

		rpalPagePtr->m_PropList.InsertItem( i
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( rpaep->rpaePropCombo.m_enumValue )
			);
		if (rpaep->rpaeArrayIndex.ctrlNull)
			rpalPagePtr->m_PropList.SetItemText( i, 1, "" );
		else {
			someText.Format( "%d", rpaep->rpaeArrayIndex.uintValue );
			rpalPagePtr->m_PropList.SetItemText( i, 1, someText );
		}

		BACnetAPDUEncoder enc;
		rpaep->rpaeValue.Encode( enc );
		EncoderToHex( enc, someText );
		rpalPagePtr->m_PropList.SetItemText( i, 2, someText );

		// ###
	}

	// enable the other controls
	rpalPagePtr->GetDlgItem( IDC_ADDPROP )->EnableWindow( true );
	rpalPagePtr->GetDlgItem( IDC_REMOVEPROP )->EnableWindow( true );
	rpalPagePtr->GetDlgItem( IDC_PROPUP )->EnableWindow( true );
	rpalPagePtr->GetDlgItem( IDC_PROPDOWN )->EnableWindow( true );
}

//
//	ReadPropACKList::Unbind
//

void ReadPropACKList::Unbind( void )
{
	// clear out the contents of the object id control
	rpalObjID.ctrlWindow->GetDlgItem( IDC_OBJID )->SetWindowText( "" );
	rpalObjID.Disable();
	rpalPagePtr->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );

	// wipe out the contents of the table
	rpalPagePtr->m_PropList.DeleteAllItems();

	// disable the other controls
	rpalPagePtr->GetDlgItem( IDC_ADDPROP )->EnableWindow( false );
	rpalPagePtr->GetDlgItem( IDC_REMOVEPROP )->EnableWindow( false );
	rpalPagePtr->GetDlgItem( IDC_PROPUP )->EnableWindow( false );
	rpalPagePtr->GetDlgItem( IDC_PROPDOWN )->EnableWindow( false );
}

//
//	ReadPropACKList::AddButtonClick
//

void ReadPropACKList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = rpalPagePtr->m_PropList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = rpalPagePtr->m_PropList.GetNextSelectedItem( selPos );
		rpalPagePtr->m_PropList.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	rpalAddInProgress = true;
	rpalPagePtr->m_PropList.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	rpalCurElem = new ReadPropACKElem( rpalPagePtr );
	rpalCurElemIndx = listLen;

	rpalCurElem->rpaePropCombo.SetEnumValue( PRESENT_VALUE );

	AddTail( rpalCurElem );

	// bind the element to the controls
	rpalCurElem->Bind();

	// update the encoding
	rpalAddInProgress = false;

	// madanner, 8/26/02.  Sourceforge bug #472392
	OnSelchangePropCombo();				// Insert new list text for Present_Value
	rpalPagePtr->m_PropList.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
//	rpalPagePtr->UpdateEncoded();		Commented because prior call to selchange updates.
}

//
//	ReadPropACKList::RemoveButtonClick
//

void ReadPropACKList::RemoveButtonClick( void )
{
	int					curRow = rpalCurElemIndx
	;
	ReadPropACKElemPtr	curElem = rpalCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	rpalPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	rpalPagePtr->m_PropList.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	rpalPagePtr->m_PropList.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	rpalPagePtr->UpdateEncoded();
}

//
//	ReadPropACKList::UpButtonClick
//

void ReadPropACKList::UpButtonClick( void )
{
	int					curRow = rpalCurElemIndx
	,					prevRow = curRow - 1
	;
	ReadPropACKElemPtr	curElem = rpalCurElem
	;

	// must have a selected row and a previous row
	if (curRow < 1)
		return;

	// move the row in the list
	rpalPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );
	rpalPagePtr->m_PropList.DeleteItem( curRow );
	rpalPagePtr->m_PropList.InsertItem( prevRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION prevPos = FindIndex( prevRow );
	InsertBefore( prevPos, curElem );

	// current element moved up
	rpalCurElemIndx -= 1;

	// select the row in its new position
	rpalPagePtr->m_PropList.SetItemState( prevRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events (dont ask why we dont!)
	OnSelchangePropCombo();
	OnChangeArrayIndex();
	if (rpalCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		rpalCurElem->rpaeValue.Encode( enc );
		EncoderToHex( enc, someText );
		rpalPagePtr->m_PropList.SetItemText( rpalCurElemIndx, 2, someText );
	}
	OnSelchangeClassCombo();
	OnSelchangeCodeCombo();

	// update the encoding
	rpalPagePtr->UpdateEncoded();
}

//
//	ReadPropACKList::DownButtonClick
//

void ReadPropACKList::DownButtonClick( void )
{
	int					curRow = rpalCurElemIndx
	,					nextRow = curRow + 1
	;
	ReadPropACKElemPtr	curElem = rpalCurElem
	;

	// must have a selected row and a following row
	if ((curRow < 0) || (curRow >= GetCount()-1))
		return;

	// move the row in the list
	rpalPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );
	rpalPagePtr->m_PropList.DeleteItem( curRow );
	rpalPagePtr->m_PropList.InsertItem( nextRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION nextPos = FindIndex( curRow );
	InsertAfter( nextPos, curElem );

	// current element moved down
	rpalCurElemIndx += 1;

	// select the row in its new position
	rpalPagePtr->m_PropList.SetItemState( nextRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events (dont ask why we dont!)
	OnSelchangePropCombo();
	OnChangeArrayIndex();
	if (rpalCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		rpalCurElem->rpaeValue.Encode( enc );
		EncoderToHex( enc, someText );
		rpalPagePtr->m_PropList.SetItemText( rpalCurElemIndx, 2, someText );
	}
	OnSelchangeClassCombo();
	OnSelchangeCodeCombo();

	// update the encoding
	rpalPagePtr->UpdateEncoded();
}

//
//	ReadPropACKList::OnSelchangePropCombo
//

void ReadPropACKList::OnSelchangePropCombo( void )
{
	if (rpalCurElem) {
		rpalCurElem->rpaePropCombo.UpdateData();
		rpalPagePtr->UpdateEncoded();

		rpalPagePtr->m_PropList.SetItemText( rpalCurElemIndx, 0
			, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( rpalCurElem->rpaePropCombo.m_enumValue )
			);
	}
}

//
//	ReadPropACKList::OnChangeArrayIndex
//

void ReadPropACKList::OnChangeArrayIndex( void )
{
	if (rpalCurElem) {
		CString		someText
		;

		rpalCurElem->rpaeArrayIndex.UpdateData();
		rpalPagePtr->UpdateEncoded();

		if (rpalCurElem->rpaeArrayIndex.ctrlNull)
			rpalPagePtr->m_PropList.SetItemText( rpalCurElemIndx, 1, "" );
		else {
			someText.Format( "%d", rpalCurElem->rpaeArrayIndex.uintValue );
			rpalPagePtr->m_PropList.SetItemText( rpalCurElemIndx, 1, someText );
		}
	}
}

//
//	ReadPropACKList::OnChangeValue
//

void ReadPropACKList::OnValue( void )
{
	if (rpalCurElem) {
		CString				someText
		;
		BACnetAPDUEncoder	enc
		;

		rpalCurElem->rpaeValue.DoModal();
		rpalPagePtr->UpdateEncoded();

		rpalCurElem->rpaeValue.Encode( enc );
		EncoderToHex( enc, someText );
		rpalPagePtr->m_PropList.SetItemText( rpalCurElemIndx, 2, someText );
	}
}

//
//	ReadPropACKList::OnSelchangeClassCombo
//

void ReadPropACKList::OnSelchangeClassCombo( void )
{
	if (rpalCurElem) {
		rpalCurElem->rpaeClassCombo.UpdateData();
		rpalPagePtr->UpdateEncoded();

		rpalPagePtr->m_PropList.SetItemText( rpalCurElemIndx, 3
			, NetworkSniffer::BAC_STRTAB_BACnetErrorClass.EnumString( rpalCurElem->rpaeClassCombo.m_enumValue )
			);
	}
}

//
//	ReadPropACKList::OnSelchangeCodeCombo
//

void ReadPropACKList::OnSelchangeCodeCombo( void )
{
	if (rpalCurElem) {
		rpalCurElem->rpaeCodeCombo.UpdateData();
		rpalPagePtr->UpdateEncoded();

		rpalPagePtr->m_PropList.SetItemText( rpalCurElemIndx, 4
			, NetworkSniffer::BAC_STRTAB_BACnetErrorCode.EnumString( rpalCurElem->rpaeCodeCombo.m_enumValue )
			);
	}
}

//
//	ReadPropACKList::OnItemChanging
//

void ReadPropACKList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = rpalCurElemIndx
	;
	ReadPropACKElemPtr	curElem = rpalCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (rpalAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		// item becoming selected
		rpalCurElemIndx = pNMListView->iItem;
		rpalCurElem = GetAt( FindIndex( rpalCurElemIndx ) );

		// bind the new current element
		rpalCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == rpalCurElemIndx) {
			// set nothing selected
			rpalCurElem = 0;
			rpalCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	ReadPropACKList::Encode
//

void ReadPropACKList::Encode( BACnetAPDUEncoder& enc )
{
	// encode the object ID
	if (rpalObjID.ctrlNull)
		throw "Object ID required";
	rpalObjID.Encode( enc, 0 );

	// encode each of the bound properties
	BACnetOpeningTag().Encode( enc, 1 );
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
	BACnetClosingTag().Encode( enc, 1 );
}

//
//	ReadPropACKListList::ReadPropACKListList
//

ReadPropACKListList::ReadPropACKListList( CSendReadPropMultACKPtr pp )
	: rpallPagePtr( pp )
	, rpallCurElem(0), rpallCurElemIndx(-1)
{
	rpallAddInProgress = FALSE;
}

//
//	ReadPropACKListList::~ReadPropACKListList
//
//	If there are any objects that have been added to the list they need to be 
//	deleted here.  The CList class will delete the stuff that it knows how to 
//	handle, but that doesn't include the ReadPropACKList objects.
//

ReadPropACKListList::~ReadPropACKListList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	ReadPropACKListList::AddButtonClick
//
//	This procedure is called when the user wants to add an additional object to
//	the list.
//

void ReadPropACKListList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = rpallPagePtr->m_ObjList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = rpallPagePtr->m_ObjList.GetNextSelectedItem( selPos );
		rpallPagePtr->m_ObjList.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	rpallAddInProgress = true;
	rpallPagePtr->m_ObjList.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	rpallCurElem = new ReadPropACKList( rpallPagePtr );
	rpallCurElemIndx = listLen;
	AddTail( rpallCurElem );

	// bind the element to the controls
	rpallCurElem->Bind();

	// update the encoding
	rpallAddInProgress = false;

	rpallPagePtr->m_ObjList.SetItemState( listLen, LVIS_SELECTED, LVIS_SELECTED);
	rpallPagePtr->UpdateEncoded();
}

//
//	ReadPropACKListList::RemoveButtonClick
//

void ReadPropACKListList::RemoveButtonClick( void )
{
	int					curRow = rpallCurElemIndx
	;
	ReadPropACKListPtr	curElem = rpallCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	rpallPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	rpallPagePtr->m_ObjList.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	if ( pos != NULL )
	{
		delete GetAt( pos );
		RemoveAt( pos );
	}

	// madanner 9/4/02
	// reselect a new row... just before the deleted one if any.
	rpallPagePtr->m_ObjList.SetItemState( curRow-1 < 0 ? 0 : curRow-1, LVIS_SELECTED, LVIS_SELECTED );

	// update the encoding
	rpallPagePtr->UpdateEncoded();
}

//
//	ReadPropACKListList::UpButtonClick
//

void ReadPropACKListList::UpButtonClick( void )
{
	int					curRow = rpallCurElemIndx
	,					prevRow = curRow - 1
	;
	ReadPropACKListPtr	curElem = rpallCurElem
	;

	// must have a selected row and a previous row
	if (curRow < 1)
		return;

	// move the row in the list
	rpallPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );
	rpallPagePtr->m_ObjList.DeleteItem( curRow );
	rpallPagePtr->m_ObjList.InsertItem( prevRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION prevPos = FindIndex( prevRow );
	InsertBefore( prevPos, curElem );

	// current element moved up
	rpallCurElemIndx -= 1;

	// select the row in its new position
	rpallPagePtr->m_ObjList.SetItemState( prevRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events
	OnChangeObjID();

	// update the encoding
	rpallPagePtr->UpdateEncoded();
}

//
//	ReadPropACKListList::DownButtonClick
//

void ReadPropACKListList::DownButtonClick( void )
{
	int					curRow = rpallCurElemIndx
	,					nextRow = curRow + 1
	;
	ReadPropACKListPtr	curElem = rpallCurElem
	;

	// must have a selected row and a following row
	if ((curRow < 0) || (curRow >= GetCount()-1))
		return;

	// move the row in the list
	rpallPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );
	rpallPagePtr->m_ObjList.DeleteItem( curRow );
	rpallPagePtr->m_ObjList.InsertItem( nextRow, "?placeholder?" );

	// delete the element
	POSITION curPos = FindIndex( curRow );
	RemoveAt( curPos );
	POSITION nextPos = FindIndex( curRow );
	InsertAfter( nextPos, curElem );

	// current element moved down
	rpallCurElemIndx += 1;

	// select the row in its new position
	rpallPagePtr->m_ObjList.SetItemState( nextRow, LVIS_SELECTED, LVIS_SELECTED );

	// make believe we have update events (dont ask why we dont!)
	OnChangeObjID();

	// update the encoding
	rpallPagePtr->UpdateEncoded();
}

//
//	ReadPropACKListList::OnChangeObjID
//

void ReadPropACKListList::OnChangeObjID( void )
{
	CString			rStr
	;

	// make sure we have a current object
	if (!rpallCurElem)
		return;

	// pass the message to the current selected object
	rpallCurElem->rpalObjID.UpdateData();

	// get the control contents, update the list
	rpallPagePtr->GetDlgItem( IDC_OBJID )->GetWindowText( rStr );
	rpallPagePtr->m_ObjList.SetItemText( rpallCurElemIndx, 0, rStr );

	// update the encoding
	rpallPagePtr->UpdateEncoded();
}

//
//	ReadPropACKListList::OnObjectIDButton
//

void ReadPropACKListList::OnObjectIDButton( void )
{
	if (!rpallCurElem)
		return;

	VTSObjectIdentifierDialog	dlg(rpallPagePtr)			// for proper parent control
	;

	dlg.objID = rpallCurElem->rpalObjID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		rpallCurElem->rpalObjID.ctrlNull = false;
		rpallCurElem->rpalObjID.objID = dlg.objID;
		rpallCurElem->rpalObjID.ObjToCtrl();

		// copy the text from the control to the list
		CString text;
		rpallPagePtr->GetDlgItem( IDC_OBJID )->GetWindowText( text );
		rpallPagePtr->m_ObjList.SetItemText( rpallCurElemIndx, 0, text );

		rpallPagePtr->UpdateEncoded();
	}
}

//
//	ReadPropACKListList::OnItemChanging
//

void ReadPropACKListList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = rpallCurElemIndx
	;
	ReadPropACKListPtr	curElem = rpallCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (rpallAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		// item becoming selected
		rpallCurElemIndx = pNMListView->iItem;
		rpallCurElem = GetAt( FindIndex( rpallCurElemIndx ) );

		// bind the new current element
		rpallCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == rpallCurElemIndx) {
			// set nothing selected
			rpallCurElem = 0;
			rpallCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	ReadPropACKListList::Encode
//
//	Each ReadPropACKList element understands how to encode itself, so this procedure 
//	just calls Encode() for each of the objects in its list.
//

void ReadPropACKListList::Encode( BACnetAPDUEncoder& enc )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}

void CSendReadPropMultACK::OnDestroy() 
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

void CSendReadPropMultACK::OnShowWindow(BOOL bShow, UINT nStatus) 
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

void CSendReadPropMultACK::OnDropdownPropcombo() 
{
	//Added by Zhu Zhenhua , 2003-7-22 
	//	to Load Standard property list for certain ObjectType
	if(m_PropListList.rpallCurElem->rpalCurElem)
	{
		m_PropListList.rpallCurElem->rpalCurElem->rpaePropCombo.m_nObjType = m_PropListList.rpallCurElem->rpalObjID.GetObjectType();
		m_PropListList.rpallCurElem->rpalCurElem->rpaePropCombo.LoadCombo();
	}	
	
}
