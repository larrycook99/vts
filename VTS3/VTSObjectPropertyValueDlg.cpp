// VTSObjectPropertyValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSObjectPropertyValueDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSObjectPropertyValueDlg dialog


#pragma warning( disable : 4355 )
VTSObjectPropertyValueDlg::VTSObjectPropertyValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSObjectPropertyValueDlg::IDD, pParent)
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_priority( this, IDC_PROPVALUE_PRIORITY )
{
	//{{AFX_DATA_INIT(VTSObjectPropertyValueDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSObjectPropertyValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSObjectPropertyValueDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	m_priority.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSObjectPropertyValueDlg, CDialog)
	//{{AFX_MSG_MAP(VTSObjectPropertyValueDlg)
	ON_EN_CHANGE(IDC_PROPVALUE_PRIORITY, OnChangePropvaluePriority)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSObjectPropertyValueDlg message handlers

void VTSObjectPropertyValueDlg::OnChangePropvaluePriority() 
{
  m_priority.UpdateData();
}

void VTSObjectPropertyValueDlg::OnValue() 
{
  m_Value.DoModal();
}

void VTSObjectPropertyValueDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSObjectPropertyValueDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSObjectPropertyValueDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSObjectPropertyValueDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}

void VTSObjectPropertyValueDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_ObjectID.Encode(enc, 0);
	m_propCombo.Encode(enc, 1);
	
	if(!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode(enc, 2);
	
	m_Value.Encode(enc, 3);
	
	if(!m_priority.ctrlNull)
		m_priority.Encode(enc, 4);	
}
void VTSObjectPropertyValueDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{		
		m_ObjectID.Decode(dec);
		m_ObjectID.ctrlNull = false;
		
		m_propCombo.Decode(dec);
		m_propCombo.ctrlNull = false;		
		if(dec.ExamineOption(2))
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
		if(dec.ExamineOption(4))
		{
			m_priority.Decode(dec);
			m_priority.ctrlNull = false;
		}
		else
			m_priority.ctrlNull = true;	
	}
}
BOOL VTSObjectPropertyValueDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();	
	return TRUE;  
}

void VTSObjectPropertyValueDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}

