// VTSObjPropDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSObjPropDialog.h"
#include "VTSObjectIdentifierDialog.h"
#include "VTSObjPropArrayDialog.h"
#include "VTSObjPropValueDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSObjPropDialog dialog


VTSObjPropDialog::VTSObjPropDialog( VTSObjPropValueListPtr opvlp )
	: CDialog(VTSObjPropDialog::IDD, NULL )
	, m_OPVL(opvlp)
	, m_dObjectList(opvlp), m_dPropList(opvlp)
	, m_iSelectedObj(-1), m_iSelectedProp(-1)
{
	//{{AFX_DATA_INIT(VTSObjPropDialog)
	m_ObjectID = _T("");
	m_PropID = _T("");
	//}}AFX_DATA_INIT
}


void VTSObjPropDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSObjPropDialog)
	DDX_Control(pDX, IDC_OBJLIST, m_ObjectList);
	DDX_Text(pDX, IDC_OBJID, m_ObjectID);
	DDX_Control(pDX, IDC_PROPLIST, m_PropList);
	DDX_Control(pDX, IDC_PROPCOMBO, m_PropCombo);
	DDX_Text(pDX, IDC_PROPID, m_PropID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSObjPropDialog, CDialog)
	//{{AFX_MSG_MAP(VTSObjPropDialog)
	ON_BN_CLICKED(IDC_ADDOBJ, OnAddObject)
	ON_BN_CLICKED(IDC_REMOVEOBJ, OnRemoveObject)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	ON_BN_CLICKED(IDC_ADDPROP, OnAddProperty)
	ON_BN_CLICKED(IDC_REMOVEPROP, OnRemoveProperty)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_BN_CLICKED(IDC_ARRAY, OnArray)
	ON_BN_CLICKED(IDC_VALUETOARRAY, OnValueToArray)
	ON_BN_CLICKED(IDC_ARRAYTOVALUE, OnArrayToValue)
	ON_EN_CHANGE(IDC_OBJID, OnChangeObjectID)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_OBJLIST, OnItemChangingObjectList)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_PROPLIST, OnItemChangingPropList)
	ON_EN_CHANGE(IDC_PROPID, OnChangePropID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSObjPropDialog message handlers

BOOL VTSObjPropDialog::OnInitDialog() 
{
	int			i, len
	;
	CString		buff
	;

	CDialog::OnInitDialog();
	
	// initialize the object list
	m_ObjectList.m_nFlags |= LVS_SINGLESEL;
	m_ObjectList.InsertColumn( 0, _T("Object ID"), LVCFMT_LEFT, 144 );

	// initialize the property list
	m_PropList.m_nFlags |= LVS_SINGLESEL;
	m_PropList.InsertColumn( 0, _T("Property"), LVCFMT_LEFT, 96 );
	m_PropList.InsertColumn( 1, _T("Value"), LVCFMT_LEFT, 192 );

	// load the drop-down
	NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.FillCombo( *cbp );

	// make sure at least eight lines are visible
	::SetDropDownSize( m_PropCombo, 8 );

	// load the object list
	len = m_dObjectList.Length();
	for (i = 0; i < len; i++ ) {
		// get the identifier
		unsigned int objID = m_dObjectList[i];

		// translate the ID into readable text
	    BACnetObjectIdentifier( objID ).Encode( buff.GetBuffer(32) );
		buff.ReleaseBuffer();

		// add it to the list
		m_ObjectList.InsertItem( i, buff );
	}

	// keep everything deselected for now
	ResetObjSelection();

	// return TRUE unless you set the focus to a control
	return TRUE;  
}


void VTSObjPropDialog::OnOK() 
{
	CDialog::OnOK(); // This will close the dialog and DoModal will return.
}


void VTSObjPropDialog::OnCancel() 
{
	CDialog::OnCancel(); // This will close the dialog and DoModal will return.
}


//
//	VTSObjPropDialog::SetObjSelection
//

void VTSObjPropDialog::SetObjSelection( int indx )
{
	int			i;
	CString		propDesc;

	// if we seem to be selecting something that is already selected, bail out
	if (m_iSelectedObj == indx)
		return;

	// if something is selected and its not this one, reset
	if ((m_iSelectedObj >= 0) && (indx != m_iSelectedObj))
		ResetObjSelection();

	// save the index
	m_iSelectedObj = indx;

	// get the object identifier
	unsigned int objID = m_dObjectList[indx];

	// translate the ID into readable text
	BACnetObjectIdentifier( objID ).Encode( m_ObjectID.GetBuffer(32) );
	m_ObjectID.ReleaseBuffer();

	// bind the property list to the new object
	m_dPropList.Bind( objID );

	// fill the table with the current list of elements
	for (i = 0; i < m_dPropList.Length(); i++ ) {
		VTSObjectPropDesc desc = m_dPropList[i];

		// translate the property ID into some kind of description
		if (desc.propID >= NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_nStrings)
			propDesc.Format( _T("%d"), desc.propID );
		else
			propDesc = NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_pStrings[ desc.propID ];
		m_PropList.InsertItem( i, propDesc );
		m_PropList.SetItemText( i, 1, desc.propValue );
	}

	// make sure the first property is visible and selected
	m_PropList.EnsureVisible( i, false );
	m_PropList.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );

	// select the first property
	SetPropSelection( 0 );

	// enable the buttons
	GetDlgItem( IDC_REMOVEOBJ )->EnableWindow( true );
	GetDlgItem( IDC_OBJID )->EnableWindow( true );
	GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( true );
	GetDlgItem( IDC_ADDPROP )->EnableWindow( true );

	// let the CDialog sync the controls with the local vars
	UpdateData( false );
}

//
//	VTSObjPropDialog::ResetObjSelection
//

void VTSObjPropDialog::ResetObjSelection( void )
{
	// nothing selected, just return
	if (m_iSelectedObj < 0)
		return;

	// clear out the contents of the local vars
	m_iSelectedObj = -1;

	m_ObjectID = _T("");

	// reset the property selection
	ResetPropSelection();

	// delete the current contents of the property list
	m_PropList.DeleteAllItems();

	// disable the buttons
	GetDlgItem( IDC_REMOVEOBJ )->EnableWindow( false );
	GetDlgItem( IDC_OBJID )->EnableWindow( false );
	GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );
	GetDlgItem( IDC_ADDPROP )->EnableWindow( false );

	// let the CDialog sync the controls with the local vars
	UpdateData( false );
}

//
//	VTSObjPropDialog::OnAddObject
//

void VTSObjPropDialog::OnAddObject() 
{
	int				i
	;
	unsigned int	objID
	;
	CString			buff
	;

	TRACE0( "----- Add Object\n" );

	// add an object to the list
	m_dObjectList.Add();

	// get its index, it will always be the last one
	i = m_dObjectList.Length() - 1;
	objID = m_dObjectList[i];

	// translate the ID into readable text
    BACnetObjectIdentifier( objID ).Encode( buff.GetBuffer(32) );
	buff.ReleaseBuffer();

	// add an item to the list
	m_ObjectList.InsertItem( i, buff );

	// make sure the record is visible and selected
	m_ObjectList.EnsureVisible( i, false );
	m_ObjectList.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );

	// select it, binding the property list
	SetObjSelection( i );
}

//
//	VTSObjPropDialog::OnRemoveObject
//

void VTSObjPropDialog::OnRemoveObject() 
{
	TRACE0( "----- Remove Object\n" );

	// make sure we have an object selected
	if (m_iSelectedObj < 0)
		return;

	// remove the selected element
	m_dObjectList.Remove( m_iSelectedObj );

	// remove it from the control
	m_ObjectList.DeleteItem( m_iSelectedObj );

	// reset the selection
	m_ObjectList.SetItemState( m_iSelectedObj - 1 < 0 ? 0 : m_iSelectedObj - 1, LVIS_SELECTED, LVIS_SELECTED );
}

//
//	VTSObjPropDialog::OnChangeObjectID
//

void VTSObjPropDialog::OnChangeObjectID() 
{
	unsigned int			curID, newID
	;
	BACnetObjectIdentifier	id
	;
	CString					buff
	;

	TRACE0( "----- Change Object ID\n" );

	// let the CDialog sync the local vars with the controls
	UpdateData( true );

	// make sure we have an object selected
	if (m_iSelectedObj < 0)
		return;

	// get the current value
	curID = newID = m_dObjectList[m_iSelectedObj];

	// interpret the contents from the user
	try {
		id.Decode( m_ObjectID );
		newID = id.objID;
	}
	catch (...) {
	}

	// don't bother with noops
	if (curID == newID)
		return;

	// dont change to one that exists
	if (m_dObjectList.Exists(newID))
		return;

	// pass along the renumber
	m_dObjectList.Move( curID, newID );

	// re-bind the property list, the display doesn't change
	m_dPropList.Bind( newID );

	// update the list version
	id.Encode( buff.GetBuffer(32) );
	buff.ReleaseBuffer();

	m_ObjectList.SetItemText( m_iSelectedObj, 0, buff );

}

//
//	VTSObjPropDialog::OnObjectIDButton
//

void VTSObjPropDialog::OnObjectIDButton() 
{
	unsigned int			curID, newID
	;
	BACnetObjectIdentifier	id
	;
	CString					buff
	;

	TRACE0( "----- Object ID Button\n" );

	// make sure we have an object selected
	if (m_iSelectedObj < 0)
		return;

	// get the current value
	curID = m_dObjectList[m_iSelectedObj];

	VTSObjectIdentifierDialog	dlg
	;

	dlg.objID = m_dObjectList[m_iSelectedObj];
	if (dlg.DoModal() && dlg.validObjID) {
		newID = dlg.objID;

		// don't bother with noops
		if (curID == newID)
			return;

		// dont change to one that exists
		if (m_dObjectList.Exists(newID))
			return;

		// pass along the renumber
		m_dObjectList.Move( curID, newID );

		// update the list version
		id.objID = newID;
		id.Encode( m_ObjectID.GetBuffer(32) );
		m_ObjectID.ReleaseBuffer();

		m_ObjectList.SetItemText( m_iSelectedObj, 0, m_ObjectID );

		// let the CDialog sync the controls with the local vars
		UpdateData( false );
	}
}

//
//	VTSObjPropDialog::SetPropSelection
//

void VTSObjPropDialog::SetPropSelection( int indx )
{
	VTSObjectPropDesc	desc
	;

	// if we seem to be selecting something that is already selected, bail out
	if (m_iSelectedProp == indx)
		return;

	// if something is selected and its not this one, reset
	if ((m_iSelectedProp >= 0) && (indx != m_iSelectedProp))
		ResetPropSelection();

	// save the index
	m_iSelectedProp = indx;

	// get a description of the property
	desc = m_dPropList[indx];

	// set the property
	GetDlgItem( IDC_PROPCOMBO )->EnableWindow( true );
	if (desc.propID < MAX_PROP_ID)
		m_PropCombo.SetCurSel( desc.propID );

	m_PropID.Format( _T("%d"), desc.propID );
	GetDlgItem( IDC_PROPID )->EnableWindow( true );
	GetDlgItem( IDC_PROPID )->SetWindowText( m_PropID );

	// value or array buttons
	GetDlgItem( IDC_VALUE )->EnableWindow( !desc.propIsArray );
	GetDlgItem( IDC_VALUETOARRAY )->EnableWindow( !desc.propIsArray );
	GetDlgItem( IDC_ARRAY )->EnableWindow( desc.propIsArray );
	GetDlgItem( IDC_ARRAYTOVALUE )->EnableWindow( desc.propIsArray );

	// enable the other controls
	GetDlgItem( IDC_REMOVEPROP )->EnableWindow( m_dPropList.Length() > 1 );
}

//
//	VTSObjPropDialog::ResetPropSelection
//

void VTSObjPropDialog::ResetPropSelection( void )
{
	// nothing selected, just return
	if (m_iSelectedProp < 0)
		return;

	// reset the selection
	m_iSelectedProp = -1;

	// clear the property list and disable
	m_PropCombo.SetCurSel( -1 );
	m_PropID = _T("");
	GetDlgItem( IDC_PROPID )->SetWindowText( m_PropID );

	// turn them off
	GetDlgItem( IDC_PROPCOMBO )->EnableWindow( false );
	GetDlgItem( IDC_PROPID )->EnableWindow( false );

	// disable the buttons
	GetDlgItem( IDC_REMOVEPROP )->EnableWindow( false );

	GetDlgItem( IDC_VALUE )->EnableWindow( false );
	GetDlgItem( IDC_VALUETOARRAY )->EnableWindow( false );
	GetDlgItem( IDC_ARRAY )->EnableWindow( false );
	GetDlgItem( IDC_ARRAYTOVALUE )->EnableWindow( false );
}

//
//	VTSObjPropDialog::OnAddProperty
//

void VTSObjPropDialog::OnAddProperty() 
{
	int					i
	;
	VTSObjectPropDesc	desc
	;

	TRACE0( "----- Add Property\n" );

	// add a property to the list
	m_dPropList.Add();

	// get its index, it will always be the last one
	i = m_dPropList.Length() - 1;
	desc = m_dPropList[i];

	// add an item to the list
	m_PropList.InsertItem( i
		, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_pStrings[ desc.propID ]
		);
	m_PropList.SetItemText( i, 1, desc.propValue );

	// make sure the record is visible and selected
	m_PropList.EnsureVisible( i, false );
	m_PropList.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );

	// select it, binding the property list
	SetPropSelection( i );
}

//
//	VTSObjPropDialog::OnRemoveProperty
//

void VTSObjPropDialog::OnRemoveProperty() 
{
	TRACE0( "----- Remove Property\n" );

	// make sure we have an object selected
	if (m_iSelectedProp < 0)
		return;

	// remove the selected element
	m_dPropList.Remove( m_iSelectedProp );

	// remove it from the control
	m_PropList.DeleteItem( m_iSelectedProp );

	// reset the selection
	m_PropList.SetItemState( m_iSelectedProp - 1 < 0 ? 0 : m_iSelectedProp - 1, LVIS_SELECTED, LVIS_SELECTED );
}

//
//	VTSObjPropDialog::OnValue
//

void VTSObjPropDialog::OnValue()
{
	unsigned int			objID = m_dObjectList[m_iSelectedObj]
	,						propID = m_dPropList[m_iSelectedProp].propID
	;
	VTSObjPropValueDialog	dlg( m_OPVL )
	;

	TRACE0( "----- Value\n" );

	// bind the value to the selected object and property
	dlg.m_dValue.Bind( objID, propID, -1 );

	// put up the dialog box
	dlg.DoModal();

	// build a new description
	m_dPropList.NewDescription( propID );

	// change the value column text
	m_PropList.SetItemText( m_iSelectedProp, 1, m_dPropList[m_iSelectedProp].propValue );
}

//
//	VTSObjPropDialog::OnArray
//

void VTSObjPropDialog::OnArray()
{
	unsigned int			objID = m_dObjectList[m_iSelectedObj]
	,						propID = m_dPropList[m_iSelectedProp].propID
	;
	VTSObjPropArrayDialog	dlg( m_OPVL )
	;

	TRACE0( "----- Array\n" );

	// bind the value to the selected object and property
	dlg.m_dArray.Bind( objID, propID );

	// put up the dialog box
	dlg.DoModal();
}

//
//	VTSObjPropDialog::OnValueToArray
//

void VTSObjPropDialog::OnValueToArray()
{
	int		propID
	;

	TRACE0( "----- Value To Array\n" );

	// make sure we have a property selected
	if (m_iSelectedProp < 0)
		return;

	propID = m_dPropList[m_iSelectedProp].propID;

	// change it
	m_dPropList.ValueToArray( propID );

	// build a new description
	m_dPropList.NewDescription( propID );

	// change the value column text
	m_PropList.SetItemText( m_iSelectedProp, 1, m_dPropList[m_iSelectedProp].propValue );

	// enable the buttons
	GetDlgItem( IDC_VALUE )->EnableWindow( false );
	GetDlgItem( IDC_VALUETOARRAY )->EnableWindow( false );
	GetDlgItem( IDC_ARRAY )->EnableWindow( true );
	GetDlgItem( IDC_ARRAYTOVALUE )->EnableWindow( true );
}

//
//	VTSObjPropDialog::OnArrayToValue
//

void VTSObjPropDialog::OnArrayToValue()
{
	int		propID
	;

	TRACE0( "----- Array To Value\n" );

	// make sure we have a property selected
	if (m_iSelectedProp < 0)
		return;

	propID = m_dPropList[m_iSelectedProp].propID;

	// change it
	m_dPropList.ArrayToValue( propID );

	// build a new description
	m_dPropList.NewDescription( propID );

	// change the value column text
	m_PropList.SetItemText( m_iSelectedProp, 1, m_dPropList[m_iSelectedProp].propValue );

	// enable the buttons
	GetDlgItem( IDC_VALUE )->EnableWindow( true );
	GetDlgItem( IDC_VALUETOARRAY )->EnableWindow( true );
	GetDlgItem( IDC_ARRAY )->EnableWindow( false );
	GetDlgItem( IDC_ARRAYTOVALUE )->EnableWindow( false );
}

//
//	VTSObjPropDialog::OnSelchangePropcombo
//

void VTSObjPropDialog::OnSelchangePropcombo()
{
	int		curProp, newProp
	;

	TRACE0( "----- Change Property ID\n" );

	// extract the current ID
	curProp = m_dPropList[m_iSelectedProp].propID;
	newProp = m_PropCombo.GetCurSel();

	// if this is the same value, skip it
	if (newProp == curProp)
		return;

	// pass along the renumber
	m_dPropList.Move( curProp, newProp );

	// change the list version
	m_PropList.SetItemText( m_iSelectedProp, 0, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_pStrings[newProp] );

	// change the decimal version
	m_PropID.Format( _T("%d"), newProp );
	GetDlgItem( IDC_PROPID )->SetWindowText( m_PropID );
}

//
//	VTSObjPropDialog::OnItemChangingObjectList
//

void VTSObjPropDialog::OnItemChangingObjectList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// forget messages that dont change the state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	if ((pNMListView->uNewState & LVIS_SELECTED) != 0)
		SetObjSelection( pNMListView->iItem );
	else
	if (pNMListView->iItem == m_iSelectedObj)
		ResetObjSelection();
	
	*pResult = 0;
}

//
//	VTSObjPropDialog::OnItemChangingPropList
//

void VTSObjPropDialog::OnItemChangingPropList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// forget messages that dont change the state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	if ((pNMListView->uNewState & LVIS_SELECTED) != 0)
		SetPropSelection( pNMListView->iItem );
	else
	if (pNMListView->iItem == m_iSelectedProp)
		ResetPropSelection();

	*pResult = 0;
}

//
//	VTSObjPropDialog::OnChangePropID
//

void VTSObjPropDialog::OnChangePropID() 
{
	int		curProp, newProp
	;

	// make sure we have an object selected
	if (m_iSelectedProp < 0)
		return;

	TRACE0( "----- Change Property ID\n" );

	// extract the current ID
	curProp = m_dPropList[m_iSelectedProp].propID;

	// get the new value
	GetDlgItem( IDC_PROPID )->GetWindowText( m_PropID );
	if (sscanf( m_PropID, "%d", &newProp ) < 1)
		return;

	// if this is the same value, skip it
	if (newProp == curProp)
		return;

	// pass along the renumber
	m_dPropList.Move( curProp, newProp );

	// change the list version
	if (newProp < NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_nStrings) {
		m_PropCombo.SetCurSel( newProp );
		m_PropList.SetItemText( m_iSelectedProp, 0, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.m_pStrings[newProp] );
	} else {
		m_PropCombo.SetCurSel( -1 );
		m_PropList.SetItemText( m_iSelectedProp, 0, m_PropID );
	}
}
