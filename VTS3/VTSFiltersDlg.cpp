// VTSFiltersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSFiltersDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSFiltersDlg dialog


VTSFiltersDlg::VTSFiltersDlg( VTSFilters * pfilters, VTSPorts * pports, CWnd* pParent )
	: CDialog(VTSFiltersDlg::IDD, pParent)
{
	// duplicate the member initalization that the class 'wizard' adds
	m_FilterType = -1;
	m_Address = _T("");
	m_Network = _T("");

	m_filters.DeepCopy(pfilters);
	m_pfilters = pfilters;
	m_pports = pports;
	m_fWarnedAlready = false;
	m_iSelectedFilter = -1;
}


void VTSFiltersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSFiltersDlg)
	DDX_Control(pDX, IDC_FILTERLIST, m_FilterList);
	DDX_Radio(pDX, IDC_ACCEPT, m_FilterType);
	DDX_Control(pDX, IDC_PORTCOMBO, m_PortCombo);
	DDX_Control(pDX, IDC_ADDRCOMBO, m_AddrCombo);
	DDX_Control(pDX, IDC_ADDRTYPECOMBO, m_AddrTypeCombo);
	DDX_Text(pDX, IDC_ADDRESS, m_Address);
	DDX_Text(pDX, IDC_NETWORK, m_Network);
	DDX_Control(pDX, IDC_FNGROUPCOMBO, m_FnGroupCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSFiltersDlg, CDialog)
	//{{AFX_MSG_MAP(VTSFiltersDlg)
	ON_BN_CLICKED(ID_NEWFILTER, OnNewFilter)
	ON_BN_CLICKED(ID_DELETEFILTER, OnDeleteFilter)
	ON_BN_CLICKED(IDC_FILTERUP, OnUpButtonClick)
	ON_BN_CLICKED(IDC_FILTERDOWN, OnDownButtonClick)
	ON_BN_CLICKED(ID_IMPORTFILTERS, OnImportFilters)
	ON_BN_CLICKED(ID_EXPORTFILTERS, OnExportFilters)
	ON_NOTIFY(NM_DBLCLK, IDC_FILTERLIST, OnDblclkFilterList)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_FILTERLIST, OnItemchangingFilterList)
	ON_BN_CLICKED(IDC_ACCEPT, SaveChanges)
	ON_BN_CLICKED(IDC_REJECT, SaveChanges)
	ON_CBN_SELCHANGE(IDC_PORTCOMBO, SaveChanges)
	ON_CBN_SELCHANGE(IDC_ADDRCOMBO, SaveChanges)
	ON_CBN_SELCHANGE(IDC_ADDRTYPECOMBO, SaveChanges)
	ON_EN_CHANGE(IDC_NETWORK, SaveChanges)
	ON_EN_CHANGE(IDC_ADDRESS, SaveChanges)
	ON_CBN_SELCHANGE(IDC_FNGROUPCOMBO, SaveChanges)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSFiltersDlg message handlers

BOOL VTSFiltersDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// disable import and export for now
	GetDlgItem( ID_IMPORTFILTERS )->EnableWindow( false );
	GetDlgItem( ID_EXPORTFILTERS )->EnableWindow( false );

	InitPortList();					// get the ports from the database

	// initialize the address combo
	m_AddrCombo.AddString( "(unset)" );
	m_AddrCombo.AddString( "Source" );
	m_AddrCombo.AddString( "Destination" );
	m_AddrCombo.AddString( "Bidirectional" );
	m_AddrCombo.SetCurSel( 0 );

	// initialize the address type combo
	m_AddrTypeCombo.AddString( "(unset)" );
	m_AddrTypeCombo.AddString( "Local Station" );		// specific local station
	m_AddrTypeCombo.AddString( "Local Network" );		// any station on local network (no routing)
	m_AddrTypeCombo.AddString( "Local Broadcast" );		// local broadcast
	m_AddrTypeCombo.AddString( "Remote Station" );		// specific remote station
	m_AddrTypeCombo.AddString( "Remote Network" );		// remote station on [specific] remote network
	m_AddrTypeCombo.AddString( "Remote Broadcast" );	// remote broadcast on [specific] remote network
	m_AddrTypeCombo.AddString( "Global Broadcast" );	// global broadcast
	m_AddrTypeCombo.SetCurSel( 0 );

	// initialize the function group combo
	m_FnGroupCombo.AddString( "(unset)" );
	m_FnGroupCombo.AddString( "BVLL" );
	m_FnGroupCombo.AddString( "MSTP" );
	m_FnGroupCombo.AddString( "PTP" );
	m_FnGroupCombo.AddString( "Network" );
	m_FnGroupCombo.AddString( "Alarm and Event" );
	m_FnGroupCombo.AddString( "File Access" );
	m_FnGroupCombo.AddString( "Object Access" );
	m_FnGroupCombo.AddString( "Remote Device Management" );
	m_FnGroupCombo.AddString( "Virtual Terminal" );
	m_FnGroupCombo.AddString( "Errors" );
	m_FnGroupCombo.SetCurSel( 0 );

	InitFilterList();				// fill in the table from the database

	ResetSelection();				// nothing selected by default
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

//
//	VTSFiltersDlg::InitPortList
//

void VTSFiltersDlg::InitPortList()
{
	// Add the ports
	m_PortCombo.AddString( "(any port)" );

	// add items and subitems
	for (int i = 0; i < m_pports->GetSize(); i++)
		m_PortCombo.AddString( (*m_pports)[i]->GetName() );

	// set the first item as the default
	m_PortCombo.SetCurSel( 0 );
}

//
//	VTSFiltersDlg::InitFilterList
//

void VTSFiltersDlg::InitFilterList()
{
	int				len = m_filters.GetSize();

	// only allow one selected name at a time
	m_FilterList.m_nFlags |= LVS_SINGLESEL;

	// do not sort the list
	m_FilterList.m_nFlags &= ~LBS_SORT;

	// add columns.
	m_FilterList.InsertColumn (0, _T("Type"), LVCFMT_LEFT, 64 );
	m_FilterList.InsertColumn (1, _T("Port"), LVCFMT_LEFT, 64 );
	m_FilterList.InsertColumn (2, _T("Network"), LVCFMT_LEFT, 64 );
	m_FilterList.InsertColumn (3, _T("Address"), LVCFMT_LEFT, 128 );

	// add filters
	for (int i = 0; i < len; i++)
	{
		// make a placeholder for the item
		m_FilterList.InsertItem( i, _T("") );

		FilterToList( (VTSFilter *) m_filters[i], i );
	}
}

//
//	VTSFiltersDlg::ResetSelection
//

void VTSFiltersDlg::ResetSelection()
{
	// no name selected
	m_iSelectedFilter = -1;

	// clear out the contents of the local variables
	m_FilterType = -1;
	m_PortCombo.SetCurSel( 0 );
	m_AddrCombo.SetCurSel( 0 );
	m_AddrTypeCombo.SetCurSel( 0 );
	m_Address = _T("");
	m_Network = _T("");
	m_FnGroupCombo.SetCurSel( 0 );

	// make sure the control values reflect the empty settings
	UpdateData( false );

	// make sure all the controls are properly disabled
	SynchronizeControls();
}

//
//	VTSFiltersDlg::SynchronizeControls
//

void VTSFiltersDlg::SynchronizeControls()
{
	// so are the radio buttons for the address type
	GetDlgItem( IDC_ACCEPT )->EnableWindow( m_iSelectedFilter != -1 );
	GetDlgItem( IDC_REJECT )->EnableWindow( m_iSelectedFilter != -1 );
	GetDlgItem( IDC_PORTCOMBO )->EnableWindow( m_iSelectedFilter != -1 );

	// address, address type
	if (m_iSelectedFilter == -1) {
		GetDlgItem( IDC_ADDRCOMBO )->EnableWindow( false );
		GetDlgItem( IDC_ADDRTYPECOMBO )->EnableWindow( false );
		GetDlgItem( IDC_NETWORK )->EnableWindow( false );
		GetDlgItem( IDC_ADDRESS )->EnableWindow( false );
	} else {
		GetDlgItem( IDC_ADDRCOMBO )->EnableWindow( true );

		if (m_AddrCombo.GetCurSel() == 0) {
			GetDlgItem( IDC_ADDRTYPECOMBO )->EnableWindow( false );
			GetDlgItem( IDC_NETWORK )->EnableWindow( false );
			GetDlgItem( IDC_ADDRESS )->EnableWindow( false );
		} else {
			GetDlgItem( IDC_ADDRTYPECOMBO )->EnableWindow( true );

			switch (m_AddrTypeCombo.GetCurSel()) {
				case 0:		// unset
				case 2:		// local network
				case 3:		// local broadcast
				case 7:		// global broadcast
					GetDlgItem( IDC_NETWORK )->EnableWindow( false );
					GetDlgItem( IDC_ADDRESS )->EnableWindow( false );
					break;

				case 1:		// local station
					GetDlgItem( IDC_NETWORK )->EnableWindow( false );
					GetDlgItem( IDC_ADDRESS )->EnableWindow( true );
					break;

				case 5:		// remote network
				case 6:		// remote broadcast
					GetDlgItem( IDC_NETWORK )->EnableWindow( true );
					GetDlgItem( IDC_ADDRESS )->EnableWindow( false );
					break;

				case 4:		// remote station
					GetDlgItem( IDC_NETWORK )->EnableWindow( true );
					GetDlgItem( IDC_ADDRESS )->EnableWindow( true );
					break;
			}
		}
	}

	// function group
	GetDlgItem( IDC_FNGROUPCOMBO )->EnableWindow( m_iSelectedFilter != -1 );
}

//
//	VTSFiltersDlg::FilterToList
//

void VTSFiltersDlg::FilterToList( const VTSFilter * pfilter, int elem )
{
	CString		netStr, addrStr;

	ASSERT(pfilter != NULL);

	// update the display list
	m_FilterList.SetItemText( elem, 0, (pfilter->m_type == 0) ? "Accept" : "Reject" );

	if ( pfilter->m_pportLink == NULL )
		m_FilterList.SetItemText( elem, 1, _T("*") );
	else
		m_FilterList.SetItemText( elem, 1, _T(pfilter->m_pportLink->GetName()) );

	// check for some remote broadcast or remote station
	if ((pfilter->m_filteraddr.addrType == remoteBroadcastAddr) || (pfilter->m_filteraddr.addrType == remoteStationAddr))
		netStr.Format( "%d", pfilter->m_filteraddr.addrNet );
	else
		netStr = _T("");
	m_FilterList.SetItemText( elem, 2, netStr );

	// check for local or remote station
	if ((pfilter->m_filteraddr.addrType == localStationAddr) || (pfilter->m_filteraddr.addrType == remoteStationAddr)) {
		static const char	hex[] = "0123456789ABCDEF"
		;
		char	buff[kMaxAddressLen * 3], *s = buff
		;

		// clear the buffer
		buff[0] = 0;

		// encode the address
		for (int i = 0; i < pfilter->m_filteraddr.addrLen; i++) {
			if (i) *s++ = '-';
			*s++ = hex[ pfilter->m_filteraddr.addrAddr[i] >> 4 ];
			*s++ = hex[ pfilter->m_filteraddr.addrAddr[i] & 0x0F ];
		}
		*s = 0;

		addrStr = buff;
	} else
		addrStr = _T("");
	m_FilterList.SetItemText( elem, 3, addrStr );
}

//
//	VTSFiltersDlg::FilterToCtrl
//

void VTSFiltersDlg::FilterToCtrl( const VTSFilter * pfilter )
{
	// set the contents of the member vars from the record
	m_FilterType = pfilter->m_type;

	// port combo
	m_PortCombo.SelectString(0, pfilter->m_pportLink == NULL ? "(any port)" : pfilter->m_pportLink->GetName());

	// address combo
	m_AddrCombo.SetCurSel( pfilter->m_addr );

	// address type combo
	m_AddrTypeCombo.SetCurSel( pfilter->m_addrType );

	// network
	if ((pfilter->m_filteraddr.addrType == remoteBroadcastAddr) || (pfilter->m_filteraddr.addrType == remoteStationAddr))
		m_Network.Format( "%d", pfilter->m_filteraddr.addrNet );
	else
		m_Network = _T("");

	// address
	if ((pfilter->m_filteraddr.addrType == localStationAddr) || (pfilter->m_filteraddr.addrType == remoteStationAddr)) {
		static const char	hex[] = "0123456789ABCDEF"
		;
		char	buff[kMaxAddressLen * 3], *s = buff
		;

		// clear the buffer
		buff[0] = 0;

		// encode the address
		for (int i = 0; i < pfilter->m_filteraddr.addrLen; i++) {
			if (i) *s++ = '-';
			*s++ = hex[ pfilter->m_filteraddr.addrAddr[i] >> 4 ];
			*s++ = hex[ pfilter->m_filteraddr.addrAddr[i] & 0x0F ];
		}
		*s = 0;

		m_Address = buff;
	} else
		m_Address = _T("");

	// function group combo
	m_FnGroupCombo.SetCurSel( pfilter->m_fnGroup );

	// sync the controls
	SynchronizeControls();

	// sync the dialog controls with the member variables
	UpdateData( false );
}

//
//	VTSFiltersDlg::CtrlToFilter
//

void VTSFiltersDlg::CtrlToFilter( VTSFilter * pfilter )
{
	int			valu = 0
	;
	LPCSTR		s
	;

	// sync the member variables with the dialog controls
	UpdateData( true );

	// make sure the correct UI elements are enable/disabled
	SynchronizeControls();

	// copy the vars into the filter
	pfilter->m_type = m_FilterType;

	// must do more than accessing pointer link... we've got to setup the name as well
	// so if the ports are changed this NAME object can relink when ports dialog is used.

	if ( m_PortCombo.GetCurSel() == 0 )
	{
		pfilter->m_pportLink = NULL;
		pfilter->m_strPortNameTemp.Empty();
	}
	else
	{
		pfilter->m_pportLink = (*m_pports)[m_PortCombo.GetCurSel() - 1];
		pfilter->m_strPortNameTemp = pfilter->m_pportLink->GetName();
	}

	// address combo
	pfilter->m_addr = m_AddrCombo.GetCurSel();

	// address type combo
	if (pfilter->m_addr == 0) {
		pfilter->m_addrType = 0;
		pfilter->m_filteraddr.addrType = nullAddr;
	} else {
		pfilter->m_addrType = m_AddrTypeCombo.GetCurSel();
		switch (pfilter->m_addrType) {
			case 0:		// unset
				pfilter->m_filteraddr.addrType = nullAddr;
				break;

			case 1:		// local station
				pfilter->m_filteraddr.addrType = localStationAddr;
				break;

			case 2:		// local network
			case 3:		// local broadcast
				pfilter->m_filteraddr.addrType = localBroadcastAddr;
				break;

			case 4:		// remote station
				pfilter->m_filteraddr.addrType = remoteStationAddr;
				break;

			case 5:		// remote network
			case 6:		// remote broadcast
				pfilter->m_filteraddr.addrType = remoteBroadcastAddr;
				break;

			case 7:		// global broadcast
				pfilter->m_filteraddr.addrType = globalBroadcastAddr;
				break;
		}
	}

	// check for some remote broadcast or remote station
	if ((pfilter->m_filteraddr.addrType == remoteBroadcastAddr) || (pfilter->m_filteraddr.addrType == remoteStationAddr)) {
		s = m_Network;

		if ((s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X'))) {
			s += 2;
			for (valu = 0; IsXDigit(*s); s++)
				valu = (valu << 4) + (IsDigit(*s) ? (*s - '0') : (ToUpper(*s) - 'A' + 10));
		} else {
			for (valu = 0; IsDigit(*s); s++)
				valu = (valu *10) + (*s - '0');
		}

		pfilter->m_filteraddr.addrNet = valu;
	}

	// check for local or remote station
	if ((pfilter->m_filteraddr.addrType == localStationAddr) || (pfilter->m_filteraddr.addrType == remoteStationAddr)) {
		int			upperNibble, lowerNibble
		;
		char		c
		;

		s = m_Address;

		// remove contents
		pfilter->m_filteraddr.addrLen = 0;

		// translate the text into octets
		for (;;) {
			// look for a hex digit
			while ((c = ToUpper(*s++)) && !IsXDigit(c))
				;
			if (!c) break;
			upperNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

			// look for another hex digit
			while ((c = ToUpper(*s++)) && !IsXDigit(c))
				;
			if (!c) break;
			lowerNibble = (IsDigit(c) ? (c - '0') : (c - 'A' + 10));

			// add the byte
			if (pfilter->m_filteraddr.addrLen < kMaxAddressLen)
				pfilter->m_filteraddr.addrAddr[ pfilter->m_filteraddr.addrLen++ ] = (upperNibble << 4) + lowerNibble;
		}
	}

	// function group combo
	pfilter->m_fnGroup = m_FnGroupCombo.GetCurSel();
}

//
//	VTSFiltersDlg::OnOK
//

void VTSFiltersDlg::OnOK() 
{
	// First we have to validate the data...  so far, the only thing can could go wrong is
	// when we let them off of the address edit control having entered an invalid address
	// for specific types...  loop through all names and check 'em.

	for ( int n = 0; n < m_filters.GetSize(); n++ )
	{
		VTSFilter * pfilter = (VTSFilter *) m_filters[n];

		if ( (pfilter->m_filteraddr.addrType == localStationAddr  || pfilter->m_filteraddr.addrType == remoteStationAddr)
			 &&  pfilter->m_filteraddr.addrLen != 1 && pfilter->m_filteraddr.addrLen != 6 )
		{
			// select the item in the list...  then change focus to address edit control

			m_FilterList.SetItemState( n, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
			GetDlgItem( IDC_ADDRESS )->SetFocus();

			AfxMessageBox(IDS_FILTER_ADDRERROR, MB_ICONEXCLAMATION | MB_OK);
			return;  // don't allow OK
		}
	}

	// copy filters array...   then copy array.  Elements remain allocated
	m_pfilters->KillContents();

	// copy only the element references... changes ownership of element memory
	for ( int i = 0; i < m_filters.GetSize(); i++ )
	{
		m_pfilters->Add(m_filters[i]);
	}

	// empty list but don't remove elements...  this will avoid destructor
	// killing the memory... which we've already transferred ownership to document's member
	// names list

	m_filters.RemoveAll();

	CDialog::OnOK();
}

//
//	VTSFiltersDlg::OnCancel
//

void VTSFiltersDlg::OnCancel() 
{
	// remove allocated temp elements...  destructor will kill array
	m_filters.KillContents();

	CDialog::OnCancel();
}

//
//	VTSFiltersDlg::OnNewFilter
//

void VTSFiltersDlg::OnNewFilter() 
{
	int listLen = m_filters.GetSize();
	VTSFilter * pfilter = new VTSFilter();

	// deselect if something was selected
	POSITION	selPos = m_FilterList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int	 nItem = m_FilterList.GetNextSelectedItem( selPos );
		m_FilterList.SetItemState( nItem, 0, LVIS_SELECTED );
	}
	
	// create a new name
	m_filters.Add(pfilter);

	// make a placeholder for the item
	m_FilterList.InsertItem( listLen, _T("") );

	// copy the contents to the list
	FilterToList( pfilter, listLen );

	// make sure it is visible and selected, this also transfers the info to the ctrls
	m_FilterList.EnsureVisible( listLen, false );
	m_FilterList.SetItemState( listLen, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	// set the focus to the accept radio button
	GetDlgItem( IDC_ACCEPT )->SetFocus();
}

//
//	VTSFiltersDlg::OnDeleteFilter
//

void VTSFiltersDlg::OnDeleteFilter() 
{
	POSITION	selPos = m_FilterList.GetFirstSelectedItemPosition();

	// make sure something was selected
	if (selPos == NULL)
		return;

	// figure out which item it is
	int	 nItem = m_FilterList.GetNextSelectedItem( selPos );

	// now deselect it, clearing out the controls in the process
	m_FilterList.SetItemState( nItem, 0, LVIS_SELECTED | LVIS_FOCUSED );

	// delete it from the list
	m_FilterList.DeleteItem( nItem );

	// delete the name from the database
	m_filters.Remove( nItem );

	// now select the next item if there is one... select the previous item if not... until null
	if ( nItem >= m_FilterList.GetItemCount() )
		nItem = m_FilterList.GetItemCount() - 1;

	if ( nItem >= 0 )
		m_FilterList.SetItemState( nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
}

//
//	VTSFiltersDlg::OnUpButtonClick
//

void VTSFiltersDlg::OnUpButtonClick( void )
{
	POSITION	selPos = m_FilterList.GetFirstSelectedItemPosition()
	;

	// make sure something was selected
	if (selPos == NULL)
		return;

	// figure out which item it is
	int		curRow = m_FilterList.GetNextSelectedItem( selPos )
	,		prevRow = curRow - 1
	;

	// must have a selected row and a previous row
	if (curRow < 1)
		return;

	TRACE0( "(move filter up)\n" );
}

//
//	VTSFiltersDlg::OnDownButtonClick
//

void VTSFiltersDlg::OnDownButtonClick( void )
{
	POSITION	selPos = m_FilterList.GetFirstSelectedItemPosition()
	;

	// make sure something was selected
	if (selPos == NULL)
		return;

	// figure out which item it is
	int		curRow = m_FilterList.GetNextSelectedItem( selPos )
	,		nextRow = curRow + 1
	;

	// must have a selected row and a following row
	if ((curRow < 0) || (curRow >= m_filters.GetSize()-1))
		return;

	TRACE0( "(move filter down)\n" );
}

//
//	VTSFiltersDlg::OnImportFilters
//

void VTSFiltersDlg::OnImportFilters() 
{
}

//
//	VTSFiltersDlg::OnExportFilters
//

void VTSFiltersDlg::OnExportFilters() 
{
}

//
//	VTSFiltersDlg::OnItemchangingFilterList
//

void VTSFiltersDlg::OnItemchangingFilterList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW*	pNMListView = (NM_LISTVIEW*)pNMHDR;
	
#if 1
	TRACE3( "Item %d from %d to %d\n"
		, pNMListView->iItem
		, pNMListView->uOldState
		, pNMListView->uNewState
		);
#endif

	// forget messages that don't change anything
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	if ((pNMListView->uNewState & LVIS_SELECTED) != 0) {
		m_iSelectedFilter = pNMListView->iItem;

		// transfer the values to the controls
		FilterToCtrl( (VTSFilter *) m_filters[m_iSelectedFilter]);

	} else {
		if (pNMListView->iItem == m_iSelectedFilter)
			ResetSelection();
	}

	*pResult = 0;
}

//
//	VTSFiltersDlg::OnDblclkFilterList
//

void VTSFiltersDlg::OnDblclkFilterList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// set the focus to the accept radio button
	GetDlgItem( IDC_ACCEPT )->SetFocus();

	*pResult = 0;
}

//
//	VTSFiltersDlg::SaveChanges
//

void VTSFiltersDlg::SaveChanges()
{
	CtrlToFilter( (VTSFilter *) m_filters[m_iSelectedFilter] );
	FilterToList( (VTSFilter *) m_filters[m_iSelectedFilter], m_iSelectedFilter );
}
