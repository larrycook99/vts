// VTSEventParameterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSEventParameterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSEventParameterDlg dialog


VTSEventParameterDlg::VTSEventParameterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSEventParameterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSEventParameterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VTSEventParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSEventParameterDlg)
	DDX_Control(pDX, IDC_CHOICE, m_choiceCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSEventParameterDlg, CDialog)
	//{{AFX_MSG_MAP(VTSEventParameterDlg)
	ON_CBN_SELCHANGE(IDC_CHOICE, OnSelchangeChoice)
	ON_BN_CLICKED(IDC_DATA, OnData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSEventParameterDlg message handlers

void VTSEventParameterDlg::OnSelchangeChoice() 
{
	m_nSelItem = m_choiceCombo.GetCurSel();
}

void VTSEventParameterDlg::OnData() 
{
	switch(m_nSelItem) 
		{
		case 0:
			m_bitstring.DoModal();
			break;
		case 1:
			m_state.DoModal();
			break;
		case 2:
			m_value.DoModal();
			break;
		case 3:
			m_commandfailure.DoModal();
			break;
		case 4:
			m_floatlimit.DoModal();
			break;
		case 5:
			m_outofrange.DoModal();
			break;
		case 8:
			m_lifesafety.DoModal();
			break;
		case 10:
			m_bufferready.DoModal();
			break;
		default:
			break;
		}
}
void VTSEventParameterDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	switch(m_nSelItem) 
		{
		case 0:
			BACnetOpeningTag().Encode(enc, 0);
			m_bitstring.Encode(enc);
			BACnetClosingTag().Encode(enc, 0);
			break;
		case 1:
			BACnetOpeningTag().Encode(enc, 1);
			m_state.Encode(enc);
			BACnetClosingTag().Encode(enc, 1);
			break;
		case 2:
			BACnetOpeningTag().Encode(enc, 2);
			m_value.Encode(enc);
			BACnetClosingTag().Encode(enc, 2);
			break;
		case 3:
			BACnetOpeningTag().Encode(enc, 3);
			m_commandfailure.Encode(enc);
			BACnetClosingTag().Encode(enc, 3);
			break;
		case 4:
			BACnetOpeningTag().Encode(enc, 4);
			m_floatlimit.Encode(enc);
			BACnetClosingTag().Encode(enc, 4);
			break;
		case 5:
			BACnetOpeningTag().Encode(enc, 5);
			m_outofrange.Encode(enc);
			BACnetClosingTag().Encode(enc, 5);
			break;
		case 8:
			BACnetOpeningTag().Encode(enc, 8);
			m_lifesafety.Encode(enc);
			BACnetClosingTag().Encode(enc, 8);
			break;
		case 10:
			BACnetOpeningTag().Encode(enc, 10);
			m_bufferready.Encode(enc);
			BACnetClosingTag().Encode(enc, 10);
			break;
		default:
			break;
		}
}
void VTSEventParameterDlg::Decode(BACnetAPDUDecoder& dec )
{
	if(dec.pktLength != 0)
	{
		BACnetAPDUTag	tagTestType;
		dec.ExamineTag(tagTestType);
		m_nSelItem = tagTestType.tagNumber;
		switch(m_nSelItem) 
		{
		case 0: 
			BACnetOpeningTag().Decode(dec);
			m_bitstring.Decode(dec);
			BACnetClosingTag().Decode(dec);
			break;
		case 1:
			BACnetOpeningTag().Decode(dec);
			m_state.Decode(dec);
			BACnetClosingTag().Decode(dec);
			break;
		case 2:
			BACnetOpeningTag().Decode(dec);
			m_value.Decode(dec);
			BACnetClosingTag().Decode(dec);
			break;
		case 3:
			BACnetOpeningTag().Decode(dec);
			m_commandfailure.Decode(dec);
			BACnetClosingTag().Decode(dec);
			break;
		case 4:
			BACnetOpeningTag().Decode(dec);
			m_floatlimit.Decode(dec);
			BACnetClosingTag().Decode(dec);
			break;
		case 5:
			BACnetOpeningTag().Decode(dec);
			m_outofrange.Decode(dec);
			BACnetClosingTag().Decode(dec);
			break;
		case 8:
			BACnetOpeningTag().Decode(dec);
			m_lifesafety.Decode(dec);
			BACnetClosingTag().Decode(dec);
			break;
		case 10:
			BACnetOpeningTag().Decode(dec);
			m_bufferready.Decode(dec);
			BACnetClosingTag().Decode(dec);
			break;
		default:
			break;
		}
	}
}

BOOL VTSEventParameterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_nSelItem >= 0)	
		m_choiceCombo.SetCurSel(m_nSelItem);
	return TRUE; 
}

