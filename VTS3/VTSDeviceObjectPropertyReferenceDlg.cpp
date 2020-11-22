// VTSDeviceObjectPropertyReferenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDeviceObjectPropertyReferenceDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSDeviceObjectPropertyReferenceDlg dialog


#pragma warning( disable : 4355 )
VTSDeviceObjectPropertyReferenceDlg::VTSDeviceObjectPropertyReferenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSDeviceObjectPropertyReferenceDlg::IDD, pParent)
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
{
	//{{AFX_DATA_INIT(VTSDeviceObjectPropertyReferenceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSDeviceObjectPropertyReferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDeviceObjectPropertyReferenceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSDeviceObjectPropertyReferenceDlg, CDialog)
	//{{AFX_MSG_MAP(VTSDeviceObjectPropertyReferenceDlg)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceidbtn)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceid)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDeviceObjectPropertyReferenceDlg message handlers

void VTSDeviceObjectPropertyReferenceDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSDeviceObjectPropertyReferenceDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSDeviceObjectPropertyReferenceDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSDeviceObjectPropertyReferenceDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}

void VTSDeviceObjectPropertyReferenceDlg::OnDeviceidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}	
}

void VTSDeviceObjectPropertyReferenceDlg::OnChangeDeviceid() 
{
	m_DeviceID.UpdateData();
}

void VTSDeviceObjectPropertyReferenceDlg::Encode(BACnetAPDUEncoder& enc, int context)
{		
	m_ObjectID.Encode(enc, 0);
	m_propCombo.Encode(enc, 1);
	
	if(!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode(enc, 2);
	if(!m_DeviceID.ctrlNull)
		m_DeviceID.Encode(enc, 3);	
}
void VTSDeviceObjectPropertyReferenceDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{			
		m_ObjectID.Decode(dec);
		m_ObjectID.ctrlNull = false;
		
		m_propCombo.Decode(dec);
		
		if(dec.ExamineOption(2))
		{
			m_ArrayIndex.Decode(dec);
			m_ArrayIndex.ctrlNull = false;
		}
		else
			m_ArrayIndex.ctrlNull = true;	
		if(dec.ExamineOption(3))
		{
			m_DeviceID.Decode(dec);
			m_DeviceID.ctrlNull = false;
		}
		else
			m_DeviceID.ctrlNull = true;
	}
}
BOOL VTSDeviceObjectPropertyReferenceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();
	m_propCombo.ObjToCtrl();
	return TRUE;  
}

void VTSDeviceObjectPropertyReferenceDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}
