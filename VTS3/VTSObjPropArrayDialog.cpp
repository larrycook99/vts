// VTSObjPropArrayDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSObjPropArrayDialog.h"
#include "VTSObjPropValueDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSObjPropArrayDialog dialog


VTSObjPropArrayDialog::VTSObjPropArrayDialog(VTSObjPropValueListPtr lp)
	: CDialog(VTSObjPropArrayDialog::IDD, NULL)
	, m_OPVL(lp), m_dArray(lp)
{
	//{{AFX_DATA_INIT(VTSObjPropArrayDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VTSObjPropArrayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSObjPropArrayDialog)
	DDX_Control(pDX, IDC_ELEMLIST, m_ElemList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSObjPropArrayDialog, CDialog)
	//{{AFX_MSG_MAP(VTSObjPropArrayDialog)
	ON_BN_CLICKED(IDC_ADDELEM, OnAddElem)
	ON_BN_CLICKED(IDC_REMOVEELEM, OnRemoveElem)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_ELEMLIST, OnItemchangingElemList)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSObjPropArrayDialog message handlers

BOOL VTSObjPropArrayDialog::OnInitDialog() 
{
	char	buff[8]
	;

	CDialog::OnInitDialog();
	
	// initialize the value list
	m_ElemList.m_nFlags |= LVS_SINGLESEL;
	m_ElemList.InsertColumn( 0, _T(""), LVCFMT_RIGHT, 36 );
	m_ElemList.InsertColumn( 1, _T("Value"), LVCFMT_LEFT, 128 );

	// fill out the control with stuff that may already be encoded
	for (int i = 0; i < m_dArray.Length(); i++) {
		sprintf( buff, "%d", i+1 );
		m_ElemList.InsertItem( i, buff );
		m_ElemList.SetItemText( i, 1, (LPCTSTR)m_dArray.GetText(i) );
	}

	// make sure nothing is selected
	m_iSelectedElem = -1;

	return TRUE;  // return TRUE unless you set the focus to a control
}

//
//	VTSObjPropArrayDialog::SetSelection
//

void VTSObjPropArrayDialog::SetSelection( int indx )
{
	// if something is selected and its not this one, reset
	if ((m_iSelectedElem >= 0) && (indx != m_iSelectedElem))
		ResetSelection();

	// set the selected component index
	m_iSelectedElem = indx;

	// sync the buttons
	GetDlgItem( IDC_REMOVEELEM )->EnableWindow( true );
	GetDlgItem( IDC_VALUE )->EnableWindow( true );
}

//
//	VTSObjPropArrayDialog::ResetSelection
//

void VTSObjPropArrayDialog::ResetSelection( void )
{
	// nothing selected, just return
	if (m_iSelectedElem < 0)
		return;

	// clear out the contents of the local vars
	m_iSelectedElem = -1;

	// sync the buttons
	GetDlgItem( IDC_REMOVEELEM )->EnableWindow( false );
	GetDlgItem( IDC_VALUE )->EnableWindow( false );
}

//
//	VTSObjPropArrayDialog::OnAddElem
//

void VTSObjPropArrayDialog::OnAddElem() 
{
	int		indx
	;
	char	buff[8]
	;

	// add an element to the array
	m_dArray.Add();

	// always the last element
	indx = m_dArray.Length() - 1;

	// add it to the list
	sprintf( buff, "%d", indx+1 );
	m_ElemList.InsertItem( indx, buff );
	m_ElemList.SetItemText( indx, 1, (LPCTSTR)m_dArray.GetText(indx) );

	// make sure the record is visible and selected
	m_ElemList.EnsureVisible( indx, false );
	m_ElemList.SetItemState( indx, LVIS_SELECTED, LVIS_SELECTED );

	// make it the selection
	SetSelection( indx );
}

//
//	VTSObjPropArrayDialog::OnRemoveElem
//

void VTSObjPropArrayDialog::OnRemoveElem() 
{
	// make sure an element is selected
	if (m_iSelectedElem < 0)
		return;

	// remove the selected element
	m_dArray.Remove( m_iSelectedElem + 1 );

	// remove it from the control
	m_ElemList.DeleteItem( m_iSelectedElem );

	// change the index number for the rest
	for (int i = m_iSelectedElem; i < m_dArray.Length(); i++) {
		char	buff[8]
		;

		sprintf( buff, "%d", i+1 );
		m_ElemList.SetItemText( i, 0, buff );
	}

	// reset the selection
	m_ElemList.SetItemState( m_iSelectedElem - 1 < 0 ? 0 : m_iSelectedElem - 1, LVIS_SELECTED, LVIS_SELECTED );
}

//
//	VTSObjPropArrayDialog::OnValue
//

void VTSObjPropArrayDialog::OnValue() 
{
//	VTSObjPropValueDialog	dlg( m_OPVL );
	VTSObjPropValueDialog	dlg( m_OPVL );

	// make sure an element is selected
	if (m_iSelectedElem < 0)
		return;

	// bind the value to the selected object and property
	dlg.m_dValue.Bind( m_dArray.arrayObjID, m_dArray.arrayPropID, m_iSelectedElem + 1 );

	// put up the dialog box
	dlg.DoModal();

	// build a new description
	m_dArray.NewDescription( m_iSelectedElem + 1 );

	// change the value column text
	m_ElemList.SetItemText( m_iSelectedElem, 1, (LPCTSTR)m_dArray.GetText(m_iSelectedElem) );
}

//
//	VTSObjPropArrayDialog::OnItemchangingElemList
//

void VTSObjPropArrayDialog::OnItemchangingElemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// forget messages that dont change the state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	if ((pNMListView->uNewState & LVIS_SELECTED) != 0)
		SetSelection( pNMListView->iItem );
	else
	if (pNMListView->iItem == m_iSelectedElem)
		ResetSelection();

	*pResult = 0;
}

