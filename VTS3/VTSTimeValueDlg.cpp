// VTSTimeValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSTimeValueDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSTimeValueDlg dialog


#pragma warning( disable : 4355 )
VTSTimeValueDlg::VTSTimeValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSTimeValueDlg::IDD, pParent)
	, m_TimeCtrl( this, IDC_TIME )
{
	//{{AFX_DATA_INIT(VTSTimeValueDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSTimeValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSTimeValueDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	m_TimeCtrl.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSTimeValueDlg, CDialog)
	//{{AFX_MSG_MAP(VTSTimeValueDlg)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_EN_CHANGE(IDC_TIME, OnChangeTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSTimeValueDlg message handlers

void VTSTimeValueDlg::OnValue() 
{
  m_Value.DoModal();
}

void VTSTimeValueDlg::OnChangeTime() 
{
	m_TimeCtrl.UpdateData();
}

BOOL VTSTimeValueDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  
}

void VTSTimeValueDlg::Encode(BACnetAPDUEncoder& enc, int context)
{

	m_TimeCtrl.Encode(enc);
	m_Value.Encode(enc);
}
void VTSTimeValueDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	{
		m_TimeCtrl.Decode(dec);
		m_TimeCtrl.ctrlNull = false;
		BACnetAPDUTag tags;
		dec.ExamineTag(tags);
		if(tags.tagClass == applicationTagClass)
		{
			switch(tags.tagNumber)
			{
			case nullAppTag:
				BACnetNull().Decode(dec);
				break;
			case booleanAppTag:
				BACnetBoolean().Decode(dec);
				break;
			case unsignedIntAppTag:
				BACnetUnsigned().Decode(dec);
				break;
			case integerAppTag:
				BACnetInteger().Decode(dec);
				break;
			case realAppTag:
				BACnetReal().Decode(dec);
				break;
			case doubleAppTag:
				BACnetDouble().Decode(dec);
				break;
			case octetStringAppTag:
				BACnetOctetString().Decode(dec);
				break;
			case characterStringAppTag:
				BACnetCharacterString().Decode(dec);
				break;
			case bitStringAppTag:
				BACnetBitString().Decode(dec);
				break;
			case enumeratedAppTag:
				BACnetEnumerated().Decode(dec);
				break;
			case dateAppTag:
				BACnetDate().Decode(dec);
				break;
			case timeAppTag:
				BACnetTime().Decode(dec);
				break;
			case objectIdentifierAppTag:
				BACnetObjectIdentifier().Decode(dec);
				break;
			default:
				break;
			}		
		}
		else
		{
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
		}
	}
}
