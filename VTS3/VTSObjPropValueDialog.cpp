// VTSObjPropValueDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"

#include "VTSCtrl.h"
#include "VTSObjPropValueDialog.h"
#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSObjPropValueDialog dialog


VTSObjPropValueDialog::VTSObjPropValueDialog(VTSObjPropValueListPtr lp )
	: CDialog(VTSObjPropValueDialog::IDD, NULL)
	, m_dValue(lp)
{
	//{{AFX_DATA_INIT(VTSObjPropValueDialog)
	m_Value = _T("");
	m_Context = _T("");
	//}}AFX_DATA_INIT

//	m_pobjpropertyvalues = pobjpropertyvalues;
//	m_objpropertyvaluesLocal.DeepCopy(m_pobjpropertyvalues);
}


void VTSObjPropValueDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSObjPropValueDialog)
	DDX_Control(pDX, IDC_VALUEIDBTN, m_ValueIDButton);
	DDX_Control(pDX, IDC_COMPLIST, m_CompList);
	DDX_Control(pDX, IDC_TYPECOMBO, m_TypeCombo);
	DDX_Text(pDX, IDC_VALUE, m_Value);
	DDX_Text(pDX, IDC_CONTEXT, m_Context);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSObjPropValueDialog, CDialog)
	//{{AFX_MSG_MAP(VTSObjPropValueDialog)
	ON_BN_CLICKED(IDC_ADDCOMP, OnAddComp)
	ON_BN_CLICKED(IDC_REMOVECOMP, OnRemoveComp)
	ON_BN_CLICKED(IDC_VALUEIDBTN, OnValueIDButton)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_COMPLIST, OnItemChangingCompList)
	ON_EN_CHANGE(IDC_CONTEXT, OnChangeContext)
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnSelchangeTypeCombo)
	ON_EN_CHANGE(IDC_VALUE, OnChangeValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSObjPropValueDialog message handlers

BOOL VTSObjPropValueDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// initialize the value list
	m_CompList.m_nFlags |= LVS_SINGLESEL;
	m_CompList.InsertColumn( 0, _T("Type"), LVCFMT_LEFT, 96 );
	m_CompList.InsertColumn( 1, _T("Context"), LVCFMT_LEFT, 48 );
	m_CompList.InsertColumn( 2, _T("Data"), LVCFMT_LEFT, 96 );

	// fill out the control with stuff that may already be encoded
	for (int i = 0; i < m_dValue.Length(); i++) {
		m_CompList.InsertItem( i, (LPCTSTR)"(placeholder)" );
		CompToList( i );
	}

	// make sure nothing is selected
	ResetSelection();

	return TRUE;  // return TRUE unless you set the focus to a control
}






//
//	VTSObjPropValueDialog::SetSelection
//

void VTSObjPropValueDialog::SetSelection( int indx )
{
	VTSObjPropValueElemDesc		desc
	;

	// if something is selected and its not this one, reset
	if ((m_iSelectedComp >= 0) && (indx != m_iSelectedComp))
		ResetSelection();

	// set the selected component index
	m_iSelectedComp = indx;

	// read the contents
	m_dValue.ReadComponent( indx, desc );

	// set the type
	m_TypeCombo.SetCurSel( desc.descValue.valueType );

	// make sure the controls are properly enabled
	SynchronizeControls();

	try {
		// decode into m_Value based on the type
		DecodeValue();
	}
	catch (...) {
	}

	// set the context
	if (desc.descValue.valueContext >= 0)
		sprintf( m_Context.GetBuffer(16), "%d", desc.descValue.valueContext );
	else
		m_Context = _T("");
	m_Context.ReleaseBuffer();

	// let the CDialog sync the controls with the local vars
	UpdateData( false );
}

//
//	VTSObjPropValueDialog::ResetSelection
//

void VTSObjPropValueDialog::ResetSelection( void )
{
	// nothing selected, just return
	if (m_iSelectedComp < 0)
		return;

	// clear out the contents of the local vars
	m_iSelectedComp = -1;

	m_TypeCombo.SetCurSel( -1 );
	m_Context = _T("");
	m_Value = _T("");

	// disable the ID button
	m_ValueIDButton.EnableWindow( false );

	// let the CDialog sync the controls with the local vars
	UpdateData( false );

	// make sure the controls are properly disabled
	SynchronizeControls();
}

//
//	VTSObjPropValueDialog::SynchronizeControls
//

void VTSObjPropValueDialog::SynchronizeControls( void )
{
	// disable if nothing selected
	GetDlgItem( IDC_TYPECOMBO )->EnableWindow( m_iSelectedComp >= 0 );
	GetDlgItem( IDC_CONTEXT )->EnableWindow( m_iSelectedComp >= 0 );
	GetDlgItem( IDC_REMOVECOMP )->EnableWindow( (m_iSelectedComp >= 0) && (m_dValue.Length() > 1) );

	if (m_iSelectedComp < 0)
		GetDlgItem( IDC_VALUE )->EnableWindow( false );
	else {
		VTSObjPropValueElemDesc		desc
		;

		// read the contents to get the type
		m_dValue.ReadComponent( m_iSelectedComp, desc );

		int compType = desc.descValue.valueType;
		GetDlgItem( IDC_VALUE )->EnableWindow( (compType != 0) && (compType != 13) && (compType != 14) );
		m_ValueIDButton.EnableWindow( compType == 12 );
	}
}

//
//	VTSObjPropValueDialog::OnAddComp
//

void VTSObjPropValueDialog::OnAddComp() 
{
	int		i
	;

	// add a component to the value
	m_dValue.Add();

	// get its index, it will always be the last one
	i = m_dValue.Length() - 1;

	// add an item to the list and sync it
	m_CompList.InsertItem( i, _T("") );
	CompToList( i );

	// make sure the record is visible and selected
	m_CompList.EnsureVisible( i, false );
	m_CompList.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );

	// make sure it's selected
	SetSelection( i );
}

//
//	VTSObjPropValueDialog::OnRemoveComp
//

void VTSObjPropValueDialog::OnRemoveComp() 
{
	// if nothing selected, return
	if (m_iSelectedComp < 0)
		return;

	// remove the selected element
	m_dValue.Remove( m_iSelectedComp );

	// remove it from the control
	m_CompList.DeleteItem( m_iSelectedComp );

	// reset the selection
	m_CompList.SetItemState( m_iSelectedComp - 1 < 0 ? 0 : m_iSelectedComp - 1, LVIS_SELECTED, LVIS_SELECTED );
}

//
//	VTSObjPropValueDialog::OnValueIDButton
//

void VTSObjPropValueDialog::OnValueIDButton() 
{
	// make sure there is a selected element
	if (m_iSelectedComp < 0)
		return;

	// bind an object identifier to the value
	VTSObjectIdentifierCtrl m_ObjID( this, IDC_VALUE )
	;

	// translate the current contents
	m_ObjID.CtrlToObj();

	VTSObjectIdentifierDialog	dlg
	;

	dlg.objID = m_ObjID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		// value change successful
		m_ObjID.ctrlNull = false;
		m_ObjID.objID = dlg.objID;
		m_ObjID.ObjToCtrl();
	}
}

//
//	VTSObjPropValueDialog::OnItemChangingCompList
//

void VTSObjPropValueDialog::OnItemChangingCompList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// forget messages that dont change the state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	if ((pNMListView->uNewState & LVIS_SELECTED) != 0)
		SetSelection( pNMListView->iItem );
	else
	if (pNMListView->iItem == m_iSelectedComp)
		ResetSelection();

	*pResult = 0;
}

//
//	VTSObjPropValueDialog::OnChangeContext
//

void VTSObjPropValueDialog::OnChangeContext() 
{
	VTSObjPropValueElemDesc		desc
	;

	// skip changes when there is no selection
	if (m_iSelectedComp < 0)
		return;

	// let the CDialog sync the local vars with the controls
	UpdateData( true );

	// read the contents to get the type
	m_dValue.ReadComponent( m_iSelectedComp, desc );

	// parse the new context value
	LPCTSTR		s = m_Context.operator LPCTSTR()
	;

	// if no data available, set to no context
	if (!*s)
		desc.descValue.valueContext = -1;
	else {
		int		intValue
		;

		// check for a sign to ignore
		if (*s == '+')
			s += 1;

		// check for hex notation
		if ((s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X'))) {
			s += 2;
			for (intValue = 0; isxdigit(*s); s++)
				intValue = (intValue << 4) + (isdigit(*s) ? (*s - '0') : (toupper(*s) - 'A' + 10));
		} else {
			for (intValue = 0; isdigit(*s); s++)
				intValue = (intValue *10) + (*s - '0');
		}

		// set the context
		desc.descValue.valueContext = intValue;
	}

	// write it back to save it
	m_dValue.WriteComponent( m_iSelectedComp, desc );

	// update the encoding of the new m_Value in the current context
	EncodeValue();

	// sync the control
	CompToList( m_iSelectedComp );
}

//
//	VTSObjPropValueDialog::OnSelchangeTypeCombo
//

void VTSObjPropValueDialog::OnSelchangeTypeCombo() 
{
	int							elemType = m_TypeCombo.GetCurSel()
	;
	VTSObjPropValueElemDesc		desc
	;

	// skip changes when there is no selection
	if (m_iSelectedComp < 0)
		return;

	// read the contents to get the type
	m_dValue.ReadComponent( m_iSelectedComp, desc );

	// change the type
	desc.descValue.valueType = elemType;

	// write it back to save it
	m_dValue.WriteComponent( m_iSelectedComp, desc );

	// update the encoding of m_Value in the new context
	if ((elemType == 0) || (elemType == 13) || (elemType == 15))
		m_Value = _T("");
	else {
		EncodeValue();
	}

	// let the CDialog sync the controls with the local vars
	UpdateData( false );

	// synchronize the enable/disable state of the value and id button
	SynchronizeControls();

	// sync the control
	CompToList( m_iSelectedComp );
}

//
//	VTSObjPropValueDialog::OnChangeValue
//

void VTSObjPropValueDialog::OnChangeValue() 
{
	// skip changes when there is no selection
	if (m_iSelectedComp < 0)
		return;

	// let the CDialog sync the local vars with the controls
	UpdateData( true );

	// update the encoding of the new m_Value in the current context
	EncodeValue();

	// sync the control
	CompToList( m_iSelectedComp );
}

//
//	VTSObjPropValueDialog::CompToList
//

void VTSObjPropValueDialog::CompToList( int indx )
{
	char						buff[4]
	;
	VTSObjPropValueElemDesc		desc
	;
	CString						typeText, str
	;

	// read the component
	m_dValue.ReadComponent( indx, desc );

	// set the type column
	m_TypeCombo.GetLBText( desc.descValue.valueType, typeText );
	m_CompList.SetItemText( indx, 0, typeText );

	// set the context column
	if (desc.descValue.valueContext < 0)
		m_CompList.SetItemText( indx, 1, (LPCTSTR)"" );
	else {
		typeText.Format( "%d", desc.descValue.valueContext );
		m_CompList.SetItemText( indx, 1, typeText );
	}

	// hex encode the encoded element (if any)
	for (int i = 0; i < desc.descElement.elemLen; i++) {
		sprintf( buff, "%02X", desc.descElement.elemContent[i] );
		str.Insert( i * 2, buff );
	}

	// set the value column
	m_CompList.SetItemText( indx, 2, str );
}

//
//	VTSObjPropValueDialog::EncodeValue
//
//	Given the contents of the m_Context and m_Value strings, encode the 
//	value into the descriptor based on the current selected component.
//

void VTSObjPropValueDialog::EncodeValue( void )
{
	VTSObjPropValueElemDesc		desc
	;
	BACnetAPDUEncoder			compEncoder
	;

	// read the component
	m_dValue.ReadComponent( m_iSelectedComp, desc );

	// depending on the type, encode the value
	switch (desc.descValue.valueType) {
		case 0: {
					BACnetNull().Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 1: {
					VTSBooleanCtrl m_Boolean( this, IDC_VALUE )
					;

					m_Boolean.CtrlToObj();
					m_Boolean.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 2: {
					VTSUnsignedCtrl m_Unsigned( this, IDC_VALUE )
					;

					m_Unsigned.CtrlToObj();
					m_Unsigned.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 3: {
					VTSIntegerCtrl m_Integer( this, IDC_VALUE )
					;

					m_Integer.CtrlToObj();
					m_Integer.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 4: {
					VTSRealCtrl m_Real( this, IDC_VALUE )
					;

					m_Real.CtrlToObj();
					m_Real.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 5: {
					VTSDoubleCtrl m_Double( this, IDC_VALUE )
					;

					m_Double.CtrlToObj();
					m_Double.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 6: {
					VTSOctetStringCtrl m_OctetStr( this, IDC_VALUE )
					;

					m_OctetStr.CtrlToObj();
					m_OctetStr.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 7: {
					VTSCharacterStringCtrl m_CharStr( this, IDC_VALUE )
					;

					m_CharStr.CtrlToObj();
					m_CharStr.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 8: {
					VTSBitStringCtrl m_BitStr( this, IDC_VALUE )
					;

					m_BitStr.CtrlToObj();
					m_BitStr.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 9: {
					VTSEnumeratedCtrl m_Enumerated( this, IDC_VALUE, 0, 0 )
					;

					m_Enumerated.CtrlToObj();
					m_Enumerated.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 10: {
					VTSDateCtrl m_Date( this, IDC_VALUE )
					;

					m_Date.CtrlToObj();
					m_Date.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 11: {
					VTSTimeCtrl m_Time( this, IDC_VALUE )
					;

					m_Time.CtrlToObj();
					m_Time.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 12: {
					VTSObjectIdentifierCtrl m_ObjID( this, IDC_VALUE )
					;

					m_ObjID.CtrlToObj();
					m_ObjID.Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 13: {
					BACnetOpeningTag().Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
		case 14: {
					BACnetClosingTag().Encode( compEncoder, desc.descValue.valueContext );
					break;
				}
	}

	// save the encoding
	desc.descElement.elemLen = compEncoder.pktLength;
	memcpy( desc.descElement.elemContent, compEncoder.pktBuffer, compEncoder.pktLength );

	// write the component
	m_dValue.WriteComponent( m_iSelectedComp, desc );
}

//
//	VTSObjPropValueDialog::DecodeValue
//
//	Decode the contents of the current component into the m_Value control.
//

void VTSObjPropValueDialog::DecodeValue( void )
{
	VTSObjPropValueElemDesc		desc
	;

	// read the component
	m_dValue.ReadComponent( m_iSelectedComp, desc );

	// create and initialize a decoder
	BACnetAPDUDecoder			dec( desc.descElement.elemContent, desc.descElement.elemLen )
	;

	// depending on the type, decode the value
	switch (desc.descValue.valueType) {
		case 0: {
					GetDlgItem( IDC_VALUE )->SetWindowText( _T("") );
					break;
				}
		case 1: {
					VTSBooleanCtrl m_Boolean( this, IDC_VALUE )
					;

					m_Boolean.Decode( dec );
					m_Boolean.ctrlNull = false;
					m_Boolean.ObjToCtrl();
					break;
				}
		case 2: {
					VTSUnsignedCtrl m_Unsigned( this, IDC_VALUE )
					;

					m_Unsigned.Decode( dec );
					m_Unsigned.ctrlNull = false;
					m_Unsigned.ObjToCtrl();
					break;
				}
		case 3: {
					VTSIntegerCtrl m_Integer( this, IDC_VALUE )
					;

					m_Integer.Decode( dec );
					m_Integer.ctrlNull = false;
					m_Integer.ObjToCtrl();
					break;
				}
		case 4: {
					VTSRealCtrl m_Real( this, IDC_VALUE )
					;

					m_Real.Decode( dec );
					m_Real.ctrlNull = false;
					m_Real.ObjToCtrl();
					break;
				}
		case 5: {
					VTSDoubleCtrl m_Double( this, IDC_VALUE )
					;

					m_Double.Decode( dec );
					m_Double.ctrlNull = false;
					m_Double.ObjToCtrl();
					break;
				}
		case 6: {
					VTSOctetStringCtrl m_OctetStr( this, IDC_VALUE )
					;

					m_OctetStr.Decode( dec );
					m_OctetStr.ctrlNull = false;
					m_OctetStr.ObjToCtrl();
					break;
				}
		case 7: {
					VTSCharacterStringCtrl m_CharStr( this, IDC_VALUE )
					;

					m_CharStr.Decode( dec );
					m_CharStr.ctrlNull = false;
					m_CharStr.ObjToCtrl();
					break;
				}
		case 8: {
					VTSBitStringCtrl m_BitStr( this, IDC_VALUE )
					;

					m_BitStr.Decode( dec );
					m_BitStr.ctrlNull = false;
					m_BitStr.ObjToCtrl();
					break;
				}
		case 9: {
					VTSEnumeratedCtrl m_Enumerated( this, IDC_VALUE, 0, 0 )
					;

					m_Enumerated.Decode( dec );
					m_Enumerated.ctrlNull = false;
					m_Enumerated.ObjToCtrl();
					break;
				}
		case 10: {
					VTSDateCtrl m_Date( this, IDC_VALUE )
					;

					m_Date.Decode( dec );
					m_Date.ctrlNull = false;
					m_Date.ObjToCtrl();
					break;
				}
		case 11: {
					VTSTimeCtrl m_Time( this, IDC_VALUE )
					;

					m_Time.Decode( dec );
					m_Time.ctrlNull = false;
					m_Time.ObjToCtrl();
					break;
				}
		case 12: {
					VTSObjectIdentifierCtrl m_ObjID( this, IDC_VALUE )
					;

					m_ObjID.Decode( dec );
					m_ObjID.ctrlNull = false;
					m_ObjID.ObjToCtrl();
					break;
				}
		case 13: {
					GetDlgItem( IDC_VALUE )->SetWindowText( _T("") );
					break;
				}
		case 14: {
					GetDlgItem( IDC_VALUE )->SetWindowText( _T("") );
					break;
				}
	}

	// synchronize the local m_Value
	GetDlgItem( IDC_VALUE )->GetWindowText( m_Value );
}
