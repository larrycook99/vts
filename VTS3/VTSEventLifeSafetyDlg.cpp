// VTSEventLifeSafetyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventLifeSafetyDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSEventLifeSafetyDlg dialog


#pragma warning( disable : 4355 )
VTSEventLifeSafetyDlg::VTSEventLifeSafetyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventLifeSafetyDlg::IDD, pParent)
	, m_timeDelay(this, IDC_TIMEDELAY)
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
{
	//{{AFX_DATA_INIT(VTSEventLifeSafetyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSEventLifeSafetyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventLifeSafetyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_timeDelay.UpdateData( pDX->m_bSaveAndValidate );
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSEventLifeSafetyDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventLifeSafetyDlg)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceid)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceidbtn)
	ON_BN_CLICKED(IDC_LISTOFAV, OnListofav)
	ON_BN_CLICKED(IDC_LISTOFLSFAV, OnListoflsfav)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_EN_CHANGE(IDC_TIMEDELAY, OnChangeTimedelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventLifeSafetyDlg message handlers

void VTSEventLifeSafetyDlg::OnListofav() 
{
	m_listofav.DoModal();
}

void VTSEventLifeSafetyDlg::OnListoflsfav() 
{
	m_listoflfsftav.DoModal();
}
void VTSEventLifeSafetyDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSEventLifeSafetyDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSEventLifeSafetyDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSEventLifeSafetyDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}

void VTSEventLifeSafetyDlg::OnDeviceidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}	
}

void VTSEventLifeSafetyDlg::OnChangeDeviceid() 
{
	m_DeviceID.UpdateData();
}

void VTSEventLifeSafetyDlg::Encode(BACnetAPDUEncoder& enc, int context)
{	
	m_timeDelay.Encode(enc, 0);	
	BACnetOpeningTag().Encode(enc, 1);
	m_listoflfsftav.Encode(enc);
	BACnetClosingTag().Encode(enc, 1);
	BACnetOpeningTag().Encode(enc, 2);
	m_listofav.Encode(enc);
	BACnetClosingTag().Encode(enc, 2);
	BACnetOpeningTag().Encode(enc, 3);
	m_ObjectID.Encode(enc, 0);
	m_propCombo.Encode(enc, 1);
	
	if(!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode(enc, 2);
	if(!m_DeviceID.ctrlNull)
		m_DeviceID.Encode(enc, 3);
	BACnetClosingTag().Encode(enc, 3);	
	
}
void VTSEventLifeSafetyDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{	
		m_timeDelay.Decode(dec);
		m_timeDelay.ctrlNull = false;
		BACnetOpeningTag().Decode(dec);
		m_listoflfsftav.Decode(dec);
		BACnetClosingTag().Decode(dec);	
		BACnetOpeningTag().Decode(dec);
		m_listofav.Decode(dec);
		BACnetClosingTag().Decode(dec);	
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
BOOL VTSEventLifeSafetyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();
	m_propCombo.ObjToCtrl();
	return TRUE;  
}

void VTSEventLifeSafetyDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}
void VTSEventLifeSafetyDlg::OnChangeTimedelay() 
{
	m_timeDelay.UpdateData();
}
