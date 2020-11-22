// VTSPropertyReferenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSPropertyReferenceDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSPropertyReferenceDlg dialog


#pragma warning( disable : 4355 )
VTSPropertyReferenceDlg::VTSPropertyReferenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSPropertyReferenceDlg::IDD, pParent)
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
{
	//{{AFX_DATA_INIT(VTSPropertyReferenceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSPropertyReferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPropertyReferenceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSPropertyReferenceDlg, CDialog)
	//{{AFX_MSG_MAP(VTSPropertyReferenceDlg)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPropertyReferenceDlg message handlers

void VTSPropertyReferenceDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}
void VTSPropertyReferenceDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_propCombo.Encode(enc, 0);
	
	if(!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode(enc, 1);
	
}
void VTSPropertyReferenceDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{			
		m_propCombo.Decode(dec);
		m_propCombo.ctrlNull = false;		
		if(dec.ExamineOption(1))
		{
			m_ArrayIndex.Decode(dec);
			m_ArrayIndex.ctrlNull = false;
		}
		else
			m_ArrayIndex.ctrlNull = true;	
	
	}
}
BOOL VTSPropertyReferenceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();
//	m_propCombo.ObjToCtrl();	
	return TRUE;  
}
void VTSPropertyReferenceDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}

