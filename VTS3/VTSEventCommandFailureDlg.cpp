// VTSEventCommandFailureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventCommandFailureDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSEventCommandFailureDlg dialog


#pragma warning( disable : 4355 )
VTSEventCommandFailureDlg::VTSEventCommandFailureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventCommandFailureDlg::IDD, pParent)
	, m_timeDelay(this, IDC_TIMEDELAY)
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
{
	//{{AFX_DATA_INIT(VTSEventCommandFailureDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEventCommandFailureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventCommandFailureDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_timeDelay.UpdateData( pDX->m_bSaveAndValidate );
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSEventCommandFailureDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventCommandFailureDlg)
	ON_EN_CHANGE(IDC_TIMEDELAY, OnChangeTimedelay)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceidbtn)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceid)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventCommandFailureDlg message handlers

void VTSEventCommandFailureDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSEventCommandFailureDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSEventCommandFailureDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSEventCommandFailureDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}

void VTSEventCommandFailureDlg::OnDeviceidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}	
}

void VTSEventCommandFailureDlg::OnChangeDeviceid() 
{
	m_DeviceID.UpdateData();
}

void VTSEventCommandFailureDlg::Encode(BACnetAPDUEncoder& enc, int context)
{	
	m_timeDelay.Encode(enc, 0);	
	BACnetOpeningTag().Encode(enc, 1);
	m_ObjectID.Encode(enc, 0);
	m_propCombo.Encode(enc, 1);
	
	if(!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode(enc, 2);
	if(!m_DeviceID.ctrlNull)
		m_DeviceID.Encode(enc, 3);
	BACnetClosingTag().Encode(enc, 1);	
	
}
void VTSEventCommandFailureDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{	
		m_timeDelay.Decode(dec);
		m_timeDelay.ctrlNull = false;
		BACnetOpeningTag().Decode(dec);
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
		BACnetClosingTag().Decode(dec);		
	}
}
BOOL VTSEventCommandFailureDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();
	m_propCombo.ObjToCtrl();
	return TRUE;  
}

void VTSEventCommandFailureDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}
void VTSEventCommandFailureDlg::OnChangeTimedelay() 
{
	m_timeDelay.UpdateData();
}
