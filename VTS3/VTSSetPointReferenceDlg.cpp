// VTSSetPointReferenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSSetPointReferenceDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSSetPointReferenceDlg dialog


#pragma warning( disable : 4355 )
VTSSetPointReferenceDlg::VTSSetPointReferenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSSetPointReferenceDlg::IDD, pParent)
	, m_ObjectID( this, IDC_OBJECTID )
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
{
	//{{AFX_DATA_INIT(VTSSetPointReferenceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSSetPointReferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSSetPointReferenceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSSetPointReferenceDlg, CDialog)
	//{{AFX_MSG_MAP(VTSSetPointReferenceDlg)
	ON_CBN_DROPDOWN(IDC_PROPCOMBO, OnDropdownPropcombo)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSSetPointReferenceDlg message handlers

void VTSSetPointReferenceDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();
	}	
}

void VTSSetPointReferenceDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}

void VTSSetPointReferenceDlg::OnDropdownPropcombo() 
{
	m_propCombo.m_nObjType = m_ObjectID.GetObjectType();
	m_propCombo.LoadCombo();
}

void VTSSetPointReferenceDlg::OnChangeObjectid() 
{
	m_ObjectID.UpdateData();
}

void VTSSetPointReferenceDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	if(!m_ObjectID.ctrlNull&&!m_propCombo.ctrlNull)
	{	
	BACnetOpeningTag().Encode(enc, 0);
		m_ObjectID.Encode(enc, 0);
		m_propCombo.Encode(enc, 1);	
		if(!m_ArrayIndex.ctrlNull)
			m_ArrayIndex.Encode(enc, 2);
	BACnetClosingTag().Encode(enc, 0);
	}
}
void VTSSetPointReferenceDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{
	BACnetOpeningTag().Decode(dec);		
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
	BACnetClosingTag().Decode(dec);	
	}
}
BOOL VTSSetPointReferenceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();
//	m_propCombo.ObjToCtrl();	
	return TRUE;  
}

void VTSSetPointReferenceDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}


