// VTSDevicesDlg.cpp : implementation file
//
// NOTE: This file is no longer part of VTS.
// It has been superceded by VTSDevicesTreeDevPage.

#include "stdafx.h"
#include "vts.h"
#include "VTSDevicesDlg.h"
#include "VTSObjPropertyDlg.h"
#include "VTSObjPropDialog.h"

#include "VTSValue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesDlg dialog


VTSDevicesDlg::VTSDevicesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSDevicesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSDevicesDlg)
	m_Segmentation = -1;
	//}}AFX_DATA_INIT
}

//MAD_DB VTSDevicesDlg::VTSDevicesDlg( VTSDeviceListPtr dlp )
VTSDevicesDlg::VTSDevicesDlg( VTSDevices * pdevices )
	: CDialog( VTSDevicesDlg::IDD, NULL )
//	, m_pDeviceList( dlp )
	, m_iSelectedDevice( -1 )
{
	// duplicate the member initalization that the class 'wizard' adds
	m_Name = _T("");

/*MAD_DB
	m_Instance = _T("");
	m_SegSize = _T("");
	m_WindowSize = _T("");
	m_NextInvokeID = _T("");
	m_MaxAPDUSize = _T("");
	m_APDUTimeout = _T("");
	m_APDUSegTimeout = _T("");
	m_APDURetries = _T("");
	m_VendorID = _T("");
*/
	m_nInstance = 0;
	m_nSegSize = 0;
	m_nWindowSize = 0;
	m_nNextInvokeID = 0;
	m_nMaxAPDUSize = 0;
	m_nAPDUTimeout = 0;
	m_nAPDUSegTimeout = 0;
	m_nAPDURetries = 0;
	m_nVendorID = 0;
	m_Segmentation = -1;
	m_nRouter = 0;

	m_pdevices = pdevices;
	m_devicesLocal.DeepCopy(m_pdevices);
}

void VTSDevicesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDevicesDlg)
	DDX_Control(pDX, IDC_DEVICELIST, m_DeviceList);
	DDX_Text(pDX, IDC_NAME, m_Name);
/*MAD_DB
	DDX_Text(pDX, IDC_INSTANCE, m_Instance);
	DDX_Text(pDX, IDC_SEGSIZE, m_SegSize);
	DDX_Text(pDX, IDC_WINDOWSIZE, m_WindowSize);
	DDX_Text(pDX, IDC_NEXTINVOKEID, m_NextInvokeID);
	DDX_Text(pDX, IDC_MAXAPDUSIZE, m_MaxAPDUSize);
	DDX_Text(pDX, IDC_APDUTIMEOUT, m_APDUTimeout);
	DDX_Text(pDX, IDC_APDUSEGTIMEOUT, m_APDUSegTimeout);
	DDX_Text(pDX, IDC_APDURETRIES, m_APDURetries);
	DDX_Text(pDX, IDC_VENDORID, m_VendorID);
*/
	DDX_Text(pDX, IDC_INSTANCE, m_nInstance);
	DDX_Text(pDX, IDC_SEGSIZE, m_nSegSize);
	DDX_Text(pDX, IDC_WINDOWSIZE, m_nWindowSize);
	DDX_Text(pDX, IDC_NEXTINVOKEID, m_nNextInvokeID);
	DDX_Text(pDX, IDC_MAXAPDUSIZE, m_nMaxAPDUSize);
	DDX_Text(pDX, IDC_APDUTIMEOUT, m_nAPDUTimeout);
	DDX_Text(pDX, IDC_APDUSEGTIMEOUT, m_nAPDUSegTimeout);
	DDX_Text(pDX, IDC_APDURETRIES, m_nAPDURetries);
	DDX_Text(pDX, IDC_VENDORID, m_nVendorID);
	DDX_Radio(pDX, IDC_SEGBOTH, m_Segmentation);
	DDX_Check(pDX, IDC_ROUTER, m_nRouter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDevicesDlg, CDialog)
	//{{AFX_MSG_MAP(VTSDevicesDlg)
	ON_BN_CLICKED(IDC_NEWDEVICE, OnNew)
	ON_BN_CLICKED(IDC_DELETEDEVICE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_DEVICELIST, OnItemchangingDevicelist)
	ON_NOTIFY(NM_DBLCLK, IDC_DEVICELIST, OnDblclkDevicelist)
	ON_BN_CLICKED(IDC_IAM, OnIAm)
	ON_EN_CHANGE(IDC_NAME, SaveChanges)
	ON_EN_CHANGE(IDC_INSTANCE, SaveChanges)
	ON_EN_CHANGE(IDC_SEGSIZE, SaveChanges)
	ON_EN_CHANGE(IDC_WINDOWSIZE, SaveChanges)
	ON_EN_CHANGE(IDC_NEXTINVOKEID, SaveChanges)
	ON_EN_CHANGE(IDC_MAXAPDUSIZE, SaveChanges)
	ON_EN_CHANGE(IDC_APDUTIMEOUT, SaveChanges)
	ON_EN_CHANGE(IDC_APDUSEGTIMEOUT, SaveChanges)
	ON_EN_CHANGE(IDC_APDURETRIES, SaveChanges)
	ON_EN_CHANGE(IDC_VENDORID, SaveChanges)
	ON_BN_CLICKED(IDC_SEGBOTH, SaveChanges)
	ON_BN_CLICKED(IDC_SEGTRANSMIT, SaveChanges)
	ON_BN_CLICKED(IDC_SEGRECEIVE, SaveChanges)
	ON_BN_CLICKED(IDC_SEGNONE, SaveChanges)
	ON_BN_CLICKED(IDC_OBJPROP, OnObjProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesDlg message handlers

BOOL VTSDevicesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	InitDeviceList();				// get device definitions from the database

	ResetSelection();				// nothing selected by default
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}



void VTSDevicesDlg::OnOK() 
{
	// copy names array...   then copy array.  Elements remain allocated
	m_pdevices->KillContents();

	// copy only the element references... changes ownership of element memory
	for ( int i = 0; i < m_devicesLocal.GetSize(); i++ )
	{
		VTSDevice * p = m_devicesLocal[i];
		m_pdevices->Add(m_devicesLocal[i]);
	}

	// empty list but don't remove elements...  this will avoid destructor
	// killing the memory... which we've already transferred ownership to document's member
	// names list

	m_devicesLocal.RemoveAll();

	CDialog::OnOK(); // This will close the dialog and DoModal will return.
}


void VTSDevicesDlg::OnCancel() 
{
	// remove allocated temp elements...  destructor will kill array
	m_devicesLocal.KillContents();

	CDialog::OnCancel(); // This will close the dialog and DoModal will return.
}



void VTSDevicesDlg::InitDeviceList()
{
//	VTSDevicePtr	curDevice;
//MAD_DB	int				len = m_pDeviceList->Length();

	// only allow one selected name at a time
	m_DeviceList.m_nFlags |= LVS_SINGLESEL;

	// do not sort the list
	m_DeviceList.m_nFlags &= ~LBS_SORT;

	// add columns.
	m_DeviceList.InsertColumn (0, _T("Name"), LVCFMT_LEFT, 198 );
	m_DeviceList.InsertColumn (1, _T("Instance"), LVCFMT_LEFT, 96 );

	// add items and subitems
//	for (int i = 0; i < m_pDeviceList->Length(); i++)
	for (int i = 0; i < m_devicesLocal.GetSize(); i++ )
	{
		// get a pointer to the ith port
//		curDevice = (*m_pDeviceList)[i];

		// make a placeholder for the item
		m_DeviceList.InsertItem( i, _T("") );

		// transfer the record contents to the list
//		DeviceToList( curDevice, i );
		DeviceToList( m_devicesLocal[i], i );
	}
}

//
//	VTSDevicesDlg::SetSelection
//

void VTSDevicesDlg::SetSelection( int indx )
{
//	VTSDevicePtr	curDevice;

	TRACE1( "SetSelection( %d )\n", indx );

	// if something is selected and its not this one, reset
	if ((m_iSelectedDevice >= 0) && (indx != m_iSelectedDevice))
		ResetSelection();

//	if ( indx >= m_pDeviceList->GetCount()  || indx < 0 )
	if ( indx >= m_devicesLocal.GetSize()  || indx < 0 )
		m_iSelectedDevice = -1;
	else
		m_iSelectedDevice = indx;

	if ( m_iSelectedDevice != -1 )
	{
		// get a pointer to the port
//		curDevice = (*m_pDeviceList)[indx];

		// set controls to content of record
//		DeviceToCtrl( curDevice );
		DeviceToCtrl( m_devicesLocal[indx] );

		// make sure the controls are properly enabled
		SynchronizeControls();

		// make sure the control values reflect the empty settings
		UpdateData( false );
	}
}

void VTSDevicesDlg::ResetSelection()
{
	// no name selected
	m_iSelectedDevice = -1;

	// clear out the contents of the local variables
	m_Name = _T("");
/*MAD_DB
	m_Instance = _T("");
	m_SegSize = _T("");
	m_WindowSize = _T("");
	m_NextInvokeID = _T("");
	m_MaxAPDUSize = _T("");
	m_APDUTimeout = _T("");
	m_APDUSegTimeout = _T("");
	m_APDURetries = _T("");
	m_VendorID = _T("");
*/
	m_nInstance = 0;
	m_nSegSize = 0;
	m_nWindowSize = 0;
	m_nNextInvokeID = 0;
	m_nMaxAPDUSize = 0;
	m_nAPDUTimeout = 0;
	m_nAPDUSegTimeout = 0;
	m_nAPDURetries = 0;
	m_nVendorID = 0;
	m_nRouter = 0;

	m_Segmentation = -1;

	// make sure the control values reflect the empty settings
	UpdateData( false );

	// make sure all the controls are properly disabled
	SynchronizeControls();
}

void VTSDevicesDlg::SynchronizeControls()
{
	// the name is enabled when there is something selected
	GetDlgItem( IDC_NAME )->EnableWindow( m_iSelectedDevice != -1 );

	GetDlgItem( IDC_INSTANCE )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_SEGSIZE )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_WINDOWSIZE )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_NEXTINVOKEID )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_MAXAPDUSIZE )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_APDUTIMEOUT )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_APDUSEGTIMEOUT )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_APDURETRIES )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_VENDORID )->EnableWindow( m_iSelectedDevice != -1 );

	GetDlgItem( IDC_SEGBOTH )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_SEGTRANSMIT )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_SEGRECEIVE )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_SEGNONE )->EnableWindow( m_iSelectedDevice != -1 );

	GetDlgItem( IDC_IAM )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_DELETEDEVICE )->EnableWindow( m_iSelectedDevice != -1 );
	GetDlgItem( IDC_OBJPROP )->EnableWindow( m_iSelectedDevice != -1 );
}

//
//	VTSDevicesDlg::DeviceToList
//

//void VTSDevicesDlg::DeviceToList( VTSDevicePtr dp, int elem )
void VTSDevicesDlg::DeviceToList( VTSDevice * pdevice, int elem )
{
	CString		devInstTxt;

	// update the display list
//	m_DeviceList.SetItemText( elem, 0, dp->devDesc.deviceName );
	m_DeviceList.SetItemText( elem, 0, pdevice->m_strName );

//	devInstTxt.Format( "%d", dp->devDesc.deviceInstance );
	devInstTxt.Format( "%d", pdevice->m_nInstance );
	m_DeviceList.SetItemText( elem, 1, devInstTxt );
}

//
//	VTSDevicesDlg::DeviceToCtrl
//

void VTSDevicesDlg::DeviceToCtrl( VTSDevice * pdevice )
{

/*MAD_DB
	// set the contents of the member vars from the record
	m_Name = pdevice->devDesc.deviceName;
	m_Instance.Format( "%d", pdevice->devDesc.deviceInstance );
	m_SegSize.Format( "%d", pdevice->devDesc.deviceSegmentSize );
	m_WindowSize.Format( "%d", pdevice->devDesc.deviceWindowSize );
	m_NextInvokeID.Format( "%d", pdevice->devDesc.deviceNextInvokeID );
	m_MaxAPDUSize.Format( "%d", pdevice->devDesc.deviceMaxAPDUSize );
	m_APDUTimeout.Format( "%d", pdevice->devDesc.deviceAPDUTimeout );
	m_APDUSegTimeout.Format( "%d", pdevice->devDesc.deviceAPDUSegmentTimeout );
	m_APDURetries.Format( "%d", pdevice->devDesc.deviceAPDURetries );
	m_VendorID.Format( "%d", pdevice->devDesc.deviceVendorID );
*/
/*
	m_nInstance = pdevice->devDesc.deviceInstance;
	m_nSegSize = pdevice->devDesc.deviceSegmentSize;
	m_nWindowSize = pdevice->devDesc.deviceWindowSize;
	m_nNextInvokeID = pdevice->devDesc.deviceNextInvokeID;
	m_nMaxAPDUSize = pdevice->devDesc.deviceMaxAPDUSize;
	m_nAPDUTimeout = pdevice->devDesc.deviceAPDUTimeout;
	m_nAPDUSegTimeout = pdevice->devDesc.deviceAPDUSegmentTimeout;
	m_nAPDURetries = pdevice->devDesc.deviceAPDURetries;
	m_nVendorID = pdevice->devDesc.deviceVendorID;

	m_Segmentation = pdevice->devDesc.deviceSegmentation;
*/

	m_Name = pdevice->m_strName;
	m_nInstance = pdevice->m_nInstance;
	m_nSegSize = pdevice->m_nSegmentSize;
	m_nWindowSize = pdevice->m_nWindowSize;
	m_nNextInvokeID = pdevice->m_nNextInvokeID;
	m_nMaxAPDUSize = pdevice->m_nMaxAPDUSize;
	m_nAPDUTimeout = pdevice->m_nAPDUTimeout;
	m_nAPDUSegTimeout = pdevice->m_nAPDUSegmentTimeout;
	m_nAPDURetries = pdevice->m_nAPDURetries;
	m_nVendorID = pdevice->m_nVendorID;
	m_Segmentation = pdevice->m_segmentation;
	m_nRouter = pdevice->m_fRouter ? 1 : 0;

	// sync the controls
	SynchronizeControls();

	// sync the dialog controls with the member variables
	UpdateData( false );
}

//
//	VTSDevicesDlg::CtrlToDevice
//

//void VTSDevicesDlg::CtrlToDevice( VTSDevicePtr dp, VTSDevice * pdevice )
void VTSDevicesDlg::CtrlToDevice( VTSDevice * pdevice )
{
	// sync the member variables with the dialog controls
	UpdateData( true );

	// make sure the correct UI elements are enable/disabled
	SynchronizeControls();

/*MAD_DB
	// copy the vars into the name
	strcpy( dp->devDesc.deviceName, m_Name );

	// do the rest
	sscanf( m_Instance, "%d", &dp->devDesc.deviceInstance );
	sscanf( m_SegSize, "%d", &dp->devDesc.deviceSegmentSize );
	sscanf( m_WindowSize, "%d", &dp->devDesc.deviceWindowSize );
	sscanf( m_NextInvokeID, "%d", &dp->devDesc.deviceNextInvokeID );
	sscanf( m_MaxAPDUSize, "%d", &dp->devDesc.deviceMaxAPDUSize );
	sscanf( m_APDUTimeout, "%d", &dp->devDesc.deviceAPDUTimeout );
	sscanf( m_APDUSegTimeout, "%d", &dp->devDesc.deviceAPDUSegmentTimeout );
	sscanf( m_APDURetries, "%d", &dp->devDesc.deviceAPDURetries );
	sscanf( m_VendorID, "%d", &dp->devDesc.deviceVendorID );

	dp->devDesc.deviceInstance = m_nInstance;
	dp->devDesc.deviceSegmentSize = m_nSegSize;
	dp->devDesc.deviceWindowSize = m_nWindowSize;
	dp->devDesc.deviceNextInvokeID = m_nNextInvokeID;
	dp->devDesc.deviceMaxAPDUSize = m_nMaxAPDUSize;
	dp->devDesc.deviceAPDUTimeout = m_nAPDUTimeout;
	dp->devDesc.deviceAPDUSegmentTimeout = m_nAPDUSegTimeout;
	dp->devDesc.deviceAPDURetries = m_nAPDURetries;
	dp->devDesc.deviceVendorID = m_nVendorID;
	dp->devDesc.deviceSegmentation = (BACnetSegmentation)m_Segmentation;
*/

	pdevice->m_strName = m_Name;
	pdevice->m_nInstance = m_nInstance;
	pdevice->m_fRouter = m_nRouter != 0;
	pdevice->m_nSegmentSize = m_nSegSize;
	pdevice->m_nWindowSize = m_nWindowSize;
	pdevice->m_nMaxAPDUSize = m_nMaxAPDUSize;
	pdevice->m_nNextInvokeID = m_nNextInvokeID;
	pdevice->m_nAPDUTimeout = m_nAPDUTimeout;
	pdevice->m_nAPDUSegmentTimeout = m_nAPDUSegTimeout;
	pdevice->m_nAPDURetries = m_nAPDURetries;
	pdevice->m_nVendorID = m_nVendorID;

	pdevice->m_segmentation = m_Segmentation;				// supports segments requests
}

//
//	VTSDevicesDlg::OnNew
//

void VTSDevicesDlg::OnNew() 
{
	int				indx;
//	VTSDevicePtr	curDevice;

	// deselect if something was selected
	POSITION	selPos = m_DeviceList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int	 nItem = m_DeviceList.GetNextSelectedItem( selPos );
		m_DeviceList.SetItemState( nItem, 0, LVIS_SELECTED );
	}
	
	// tell the list to add a new device
//	m_pDeviceList->Add();
	indx = m_devicesLocal.Add(new VTSDevice());

	// get its index, it will always be the last one
//	indx = m_pDeviceList->Length() - 1;

	// get a pointer to the new device
//	curDevice = (*m_pDeviceList)[indx];

	m_DeviceList.InsertItem( indx, _T("") );
//	m_DeviceList.InsertItem( indx, m_devicesLocal[indx]->m_strName );

	// copy the contents to the list
//	DeviceToList( curDevice, indx );
	DeviceToList( m_devicesLocal[indx], indx );

	// make sure the record is visible and selected
	m_DeviceList.EnsureVisible( indx, false );
	m_DeviceList.SetItemState( indx, LVIS_SELECTED, LVIS_SELECTED );
}


void VTSDevicesDlg::OnDelete()
{
	if ( m_iSelectedDevice != -1 )
	{
		int nIndex = m_iSelectedDevice;

		delete m_devicesLocal[nIndex];
		m_devicesLocal.RemoveAt(nIndex);

//		m_pDeviceList->Remove(nIndex);

		// now deselect it, clearing out the controls in the process
		m_DeviceList.SetItemState( nIndex, 0, LVIS_SELECTED | LVIS_FOCUSED );

		// delete it from the list
		m_DeviceList.DeleteItem( nIndex-- );

		if ( nIndex < 0  &&  m_DeviceList.GetItemCount() > 0 )
			nIndex = 0;

		if ( nIndex < m_DeviceList.GetItemCount()  && nIndex != -1 )
			m_DeviceList.SetItemState( nIndex, LVIS_SELECTED, LVIS_SELECTED );

		SetSelection(nIndex);		// handles no selection cases
	}
}


//
//	VTSDevicesDlg::OnItemchangingDevicelist
//

void VTSDevicesDlg::OnItemchangingDevicelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW*	pNMListView = (NM_LISTVIEW*)pNMHDR
	;
	
	// forget messages that dont change anything
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	if ((pNMListView->uNewState & LVIS_SELECTED) != 0)
		SetSelection( pNMListView->iItem );
	else
	if (pNMListView->iItem == m_iSelectedDevice)
		ResetSelection();

	*pResult = 0;
}

//
//	VTSDevicesDlg::OnDblclkDevicelist
//

void VTSDevicesDlg::OnDblclkDevicelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// set the focus to the name for editing, select all of the text
	GetDlgItem( IDC_NAME )->SetFocus();
	((CEdit*)GetDlgItem( IDC_NAME ))->SetSel( 0, -1 );

	*pResult = 0;
}

//
//	VTSDevicesDlg::SaveChanges
//

void VTSDevicesDlg::SaveChanges()
{
//	VTSDevicePtr	curDevice;

	// get a pointer to the device
//	curDevice = (*m_pDeviceList)[m_iSelectedDevice];

	// transfer the contents of the controls to the record
//	CtrlToDevice( curDevice, m_devicesLocal[m_iSelectedDevice] );
	CtrlToDevice( m_devicesLocal[m_iSelectedDevice] );

	// save the updated record
//	curDevice->WriteDesc();

	// bring the list up-to-date
//	DeviceToList( curDevice, m_iSelectedDevice );
	DeviceToList( m_devicesLocal[m_iSelectedDevice], m_iSelectedDevice );
}

//
//	VTSDevicesDlg::OnIAm
//

void VTSDevicesDlg::OnIAm() 
{
//	VTSDevicePtr	curDevice;

	// get a pointer to the device
//	curDevice = (*m_pDeviceList)[m_iSelectedDevice];

	// tell it to send an I-Am
//	curDevice->IAm();
	m_devicesLocal[m_iSelectedDevice]->IAm();
}

//
//	VTSDevicesDlg::OnObjProp
//
//	Access to the built-in objects, properties and values.
//

void VTSDevicesDlg::OnObjProp() 
{
//	VTSDevicePtr	curDevice;

	// get a pointer to the device
//	curDevice = (*m_pDeviceList)[m_iSelectedDevice];

	// set the list busy while the user is editing
//	curDevice->devObjPropValueList->SetBusy();

	// create a dialog for editing content
//MAD_DB	VTSObjPropDialog	opd( curDevice->devObjPropValueList );
	VTSObjPropertyDlg	opd( &m_devicesLocal[m_iSelectedDevice]->m_devobjects );

	// let it run
	opd.DoModal();

	// all done with the list
//	curDevice->devObjPropValueList->ResetBusy();
}
