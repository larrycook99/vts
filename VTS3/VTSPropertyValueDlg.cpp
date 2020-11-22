// VTSPropertyValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSPropertyValueDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// VTSPropertyValueDlg dialog


#pragma warning( disable : 4355 )
VTSPropertyValueDlg::VTSPropertyValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSPropertyValueDlg::IDD, pParent)
	, m_propCombo( this, IDC_PROPCOMBO, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier, true, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_priority( this, IDC_PROPVALUE_PRIORITY )
{
	//{{AFX_DATA_INIT(VTSPropertyValueDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSPropertyValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPropertyValueDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_propCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	m_priority.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSPropertyValueDlg, CDialog)
	//{{AFX_MSG_MAP(VTSPropertyValueDlg)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_EN_CHANGE(IDC_PROPVALUE_PRIORITY, OnChangePropvaluePriority)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropcombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayindex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPropertyValueDlg message handlers

void VTSPropertyValueDlg::OnChangePropvaluePriority() 
{
  m_priority.UpdateData();
}

void VTSPropertyValueDlg::OnValue() 
{
  m_Value.DoModal();
}

void VTSPropertyValueDlg::OnSelchangePropcombo() 
{
	m_propCombo.CtrlToObj();
}
void VTSPropertyValueDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_propCombo.Encode(enc, 0);
	
	if(!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode(enc, 1);

	m_Value.Encode(enc, 2);
	
	if(!m_priority.ctrlNull)
		m_priority.Encode(enc, 3);	
}
void VTSPropertyValueDlg::Decode(BACnetAPDUDecoder& dec )
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
		if(dec.ExamineOption(3))
		{
			m_priority.Decode(dec);
			m_priority.ctrlNull = false;
		}
		else
			m_priority.ctrlNull = true;	
	}
}
BOOL VTSPropertyValueDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_propCombo.LoadCombo();	
	return TRUE;  
}

void VTSPropertyValueDlg::OnChangeArrayindex() 
{
	m_ArrayIndex.UpdateData();	
}

