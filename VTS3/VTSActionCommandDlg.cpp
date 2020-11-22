// VTSActionCommandDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSActionCommandDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSActionCommandDlg dialog


#pragma warning( disable : 4355 )
VTSActionCommandDlg::VTSActionCommandDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSActionCommandDlg::IDD, pParent)
	, m_DeviceID( this, IDC_DEVICEID )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_Priority(this, IDC_PRIO)
	, m_PostDelay(this, IDC_POSTDELAY)
	, quitOnFail(this, IDC_QUITONFAIL, true)
	, writeSuccess(this, IDC_WRITESUCCESS, true)
{
	//{{AFX_DATA_INIT(VTSActionCommandDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSActionCommandDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSActionCommandDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_DeviceID.UpdateData( pDX->m_bSaveAndValidate );
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	m_Priority.UpdateData( pDX->m_bSaveAndValidate );
	m_PostDelay.UpdateData( pDX->m_bSaveAndValidate );
	quitOnFail.UpdateData( pDX->m_bSaveAndValidate );
	writeSuccess.UpdateData( pDX->m_bSaveAndValidate );
}
BEGIN_MESSAGE_MAP(VTSActionCommandDlg, CDialog)
	//{{AFX_MSG_MAP(VTSActionCommandDlg)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_DEVICEIDBTN, OnDeviceidbtn)
	ON_EN_CHANGE(IDC_DEVICEID, OnChangeDeviceid)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_BN_CLICKED(IDC_WRITESUCCESS, OnWritesuccess)
	ON_BN_CLICKED(IDC_QUITONFAIL, OnQuitonfail)
	ON_EN_CHANGE(IDC_PRIO, OnChangePrio)
	ON_EN_CHANGE(IDC_POSTDELAY, OnChangePostdelay)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSActionCommandDlg message handlers

void VTSActionCommandDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSActionCommandDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSActionCommandDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSActionCommandDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}

void VTSActionCommandDlg::OnDeviceidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_DeviceID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_DeviceID.ctrlNull = false;
		m_DeviceID.objID = dlg.objID;
		m_DeviceID.ObjToCtrl();
	}	
}

void VTSActionCommandDlg::OnChangeDeviceid() 
{
	m_DeviceID.UpdateData();
}

void VTSActionCommandDlg::OnValue() 
{
	m_propValue.DoModal();
}

void VTSActionCommandDlg::OnWritesuccess() 
{

	writeSuccess.UpdateData();
}

void VTSActionCommandDlg::OnQuitonfail() 
{
	quitOnFail.UpdateData();
}

void VTSActionCommandDlg::OnChangePrio() 
{
	m_Priority.UpdateData();
}

void VTSActionCommandDlg::OnChangePostdelay() 
{
	m_PostDelay.UpdateData();
	
}
void VTSActionCommandDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	
	if(!m_DeviceID.ctrlNull)
		m_DeviceID.Encode(enc, 0);
	
	m_ObjectID.Encode(enc, 1);
	m_propCombo.Encode(enc, 2);
	
	if(!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode(enc, 3);

	m_propValue.Encode(enc, 4);	
	
	if(!m_Priority.ctrlNull)
		m_Priority.Encode(enc, 5);
	if(!m_PostDelay.ctrlNull)
		m_PostDelay.Encode(enc, 6);
	
	quitOnFail.Encode(enc, 7);
	writeSuccess.Encode(enc, 8);
}
void VTSActionCommandDlg::Decode(BACnetAPDUDecoder& dec )
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
		
		m_propCombo.Decode(dec);
		
		if(dec.ExamineOption(3))
		{
			m_ArrayIndex.Decode(dec);
			m_ArrayIndex.ctrlNull = false;
		}
		else
			m_ArrayIndex.ctrlNull = true;	
		BACnetOpeningTag().Decode(dec);
		BACnetAPDUTag tag;
		int tagmiss = 1; 
			while(tagmiss != 0)
			{
				BACnetOctet	tag;			
				if (dec.pktLength < 1)
					break;				
				tag = (dec.pktLength--,*dec.pktBuffer++);
				
				if ((tag & 0x0F) == 0x0E)
					tagmiss++;
				if ((tag & 0x0F) == 0x0F)
					tagmiss--;
			}
//		BACnetClosingTag().Decode(dec);	
		if(dec.ExamineOption(5))
		{
			m_Priority.Decode(dec);
			m_Priority.ctrlNull = false;
		}
		else
			m_Priority.ctrlNull = true;
		
		if(dec.ExamineOption(6))
		{
			m_PostDelay.Decode(dec);
			m_PostDelay.ctrlNull = false;
		}
		else
			m_ArrayIndex.ctrlNull = true;
		
		quitOnFail.Decode(dec);
		writeSuccess.Decode(dec);
	}
}
BOOL VTSActionCommandDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();
	m_propCombo.ObjToCtrl();
	quitOnFail.ObjToCtrl();
	writeSuccess.ObjToCtrl();
	
	return TRUE;  
}

void VTSActionCommandDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}
