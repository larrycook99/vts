// VTSDeviceObjectReferenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDeviceObjectReferenceDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDeviceObjectReferenceDlg dialog


#pragma warning( disable : 4355 )
VTSDeviceObjectReferenceDlg::VTSDeviceObjectReferenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSDeviceObjectReferenceDlg::IDD, pParent)
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
{
	//{{AFX_DATA_INIT(VTSDeviceObjectReferenceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSDeviceObjectReferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDeviceObjectReferenceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSDeviceObjectReferenceDlg, CDialog)
	//{{AFX_MSG_MAP(VTSDeviceObjectReferenceDlg)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceidbtn)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDeviceObjectReferenceDlg message handlers

void VTSDeviceObjectReferenceDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();	
}

void VTSDeviceObjectReferenceDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}
}

void VTSDeviceObjectReferenceDlg::OnDeviceidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}
}

void VTSDeviceObjectReferenceDlg::OnChangeDeviceid() 
{
	m_DeviceID.UpdateData();
}

BOOL VTSDeviceObjectReferenceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	
	return TRUE;  
}
void VTSDeviceObjectReferenceDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength!=0)
	{
		if(dec.ExamineOption(0))
		{
			m_DeviceID.Decode(dec);
			m_DeviceID.ctrlNull = false;
		}
		else
			m_DeviceID.ctrlNull = true;
		
		m_ObjectID.Decode(dec);
		m_ObjectID.ctrlNull = false;
	}
}

void VTSDeviceObjectReferenceDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	if(!m_DeviceID.ctrlNull)
		m_DeviceID.Encode(enc, 0);
	
	m_ObjectID.Encode(enc, 1);	
}
