// VTSObjectPropertyReferenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSObjectPropertyReferenceDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSObjectPropertyReferenceDlg dialog


#pragma warning( disable : 4355 )
VTSObjectPropertyReferenceDlg::VTSObjectPropertyReferenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSObjectPropertyReferenceDlg::IDD, pParent)
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
{
	//{{AFX_DATA_INIT(VTSObjectPropertyReferenceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSObjectPropertyReferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSObjectPropertyReferenceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSObjectPropertyReferenceDlg, CDialog)
	//{{AFX_MSG_MAP(VTSObjectPropertyReferenceDlg)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSObjectPropertyReferenceDlg message handlers

void VTSObjectPropertyReferenceDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSObjectPropertyReferenceDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSObjectPropertyReferenceDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSObjectPropertyReferenceDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}

void VTSObjectPropertyReferenceDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_ObjectID.Encode(enc, 0);
	m_propCombo.Encode(enc, 1);
	
	if(!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode(enc, 2);
	
}
void VTSObjectPropertyReferenceDlg::Decode(BACnetAPDUDecoder& dec )
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
	
	}
}
BOOL VTSObjectPropertyReferenceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();
//	m_propCombo.ObjToCtrl();	
	return TRUE;  
}

void VTSObjectPropertyReferenceDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}
