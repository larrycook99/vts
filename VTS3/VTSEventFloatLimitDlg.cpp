// VTSEventFloatLimitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventFloatLimitDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSEventFloatLimitDlg dialog


#pragma warning( disable : 4355 )
VTSEventFloatLimitDlg::VTSEventFloatLimitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventFloatLimitDlg::IDD, pParent)
	, m_timeDelay(this, IDC_TIMEDELAY)
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_LowLimit(this, IDC_LOWLIMIT)
	, m_HighLimit(this, IDC_HIGHLIMIT)
	, m_DeadBand(this, IDC_DEADBAND)
{
	//{{AFX_DATA_INIT(VTSEventFloatLimitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEventFloatLimitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventFloatLimitDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_timeDelay.UpdateData( pDX->m_bSaveAndValidate );
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	m_LowLimit.UpdateData( pDX->m_bSaveAndValidate );
	m_HighLimit.UpdateData( pDX->m_bSaveAndValidate );
	m_DeadBand.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSEventFloatLimitDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventFloatLimitDlg)
	ON_EN_CHANGE(IDC_HIGHLIMIT, OnChangeHighlimit)
	ON_EN_CHANGE(IDC_LOWLIMIT, OnChangeLowlimit)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceidbtn)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceid)
	ON_EN_CHANGE(IDC_DEADBAND, OnChangeDeadband)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_EN_CHANGE(IDC_TIMEDELAY, OnChangeTimedelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventFloatLimitDlg message handlers

void VTSEventFloatLimitDlg::OnChangeHighlimit() 
{
	m_HighLimit.UpdateData();
}

void VTSEventFloatLimitDlg::OnChangeLowlimit() 
{
	m_LowLimit.UpdateData();	
}
void VTSEventFloatLimitDlg::OnChangeDeadband() 
{
	m_DeadBand.UpdateData();	
}
void VTSEventFloatLimitDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSEventFloatLimitDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSEventFloatLimitDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSEventFloatLimitDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}

void VTSEventFloatLimitDlg::OnDeviceidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}	
}

void VTSEventFloatLimitDlg::OnChangeDeviceid() 
{
	m_DeviceID.UpdateData();
}

void VTSEventFloatLimitDlg::Encode(BACnetAPDUEncoder& enc, int context)
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
	m_LowLimit.Encode(enc, 2);
	m_HighLimit.Encode(enc, 3);
	m_DeadBand.Encode(enc, 4);
	
}
void VTSEventFloatLimitDlg::Decode(BACnetAPDUDecoder& dec )
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
		m_LowLimit.Decode(dec);
		m_LowLimit.ctrlNull = false;
		m_HighLimit.Decode(dec);
		m_HighLimit.ctrlNull = false;
		m_DeadBand.Decode(dec);	
		m_DeadBand.ctrlNull = false;		
	}
}
BOOL VTSEventFloatLimitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();
	m_propCombo.ObjToCtrl();
	return TRUE;  
}

void VTSEventFloatLimitDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}
void VTSEventFloatLimitDlg::OnChangeTimedelay() 
{
	m_timeDelay.UpdateData();
}



