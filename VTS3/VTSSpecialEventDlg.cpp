// VTSSpecialEventDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSSpecialEventDlg.h"
#include "VTSObjectIdentifierDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSSpecialEventDlg dialog


#pragma warning( disable : 4355 )
VTSSpecialEventDlg::VTSSpecialEventDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSSpecialEventDlg::IDD, pParent)
    , m_calendarRef(this,IDC_OBJECTID)
	, m_eventPriority(this,IDC_UNSIGNEDVALUE)
{	
	m_listofTimeValues.m_showAsListofTimeValue = TRUE;
	//{{AFX_DATA_INIT(VTSSpecialEventDlg)
	m_nChoice = -1;
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )


void VTSSpecialEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSSpecialEventDlg)
	DDX_Radio(pDX, IDC_RADIOCALENTRY, m_nChoice);
	//}}AFX_DATA_MAP
	m_calendarRef.UpdateData( pDX->m_bSaveAndValidate );
	m_eventPriority.UpdateData( pDX->m_bSaveAndValidate );
}


BEGIN_MESSAGE_MAP(VTSSpecialEventDlg, CDialog)
	//{{AFX_MSG_MAP(VTSSpecialEventDlg)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectid)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectidbtn)
	ON_BN_CLICKED(IDC_LISTOFTIMEVALUES, OnListoftimevalues)
	ON_EN_CHANGE(IDC_UNSIGNEDVALUE, OnChangeUnsignedvalue)
	ON_BN_CLICKED(IDC_RADIOCALENTRY, OnRadiocalentry)
	ON_BN_CLICKED(IDC_RADIOOBJID, OnRadioobjid)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSSpecialEventDlg message handlers

void VTSSpecialEventDlg::OnChangeObjectid() 
{
	m_calendarRef.UpdateData();
}

void VTSSpecialEventDlg::OnObjectidbtn() 
{
	VTSObjectIdentifierDialog	dlg(this);

	dlg.objID = m_calendarRef.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_calendarRef.ctrlNull = false;
		m_calendarRef.objID = dlg.objID;
		m_calendarRef.ObjToCtrl();
	}
}

void VTSSpecialEventDlg::OnListoftimevalues() 
{
	m_listofTimeValues.DoModal();
}

void VTSSpecialEventDlg::OnChangeUnsignedvalue() 
{
	m_eventPriority.UpdateData();
}

void VTSSpecialEventDlg::OnRadiocalentry() 
{	
	UpdateData();
	this->GetDlgItem(IDC_VALUE)->EnableWindow(true);
	this->GetDlgItem(IDC_OBJECTID)->EnableWindow(false);
	this->GetDlgItem(IDC_OBJECTIDBTN)->EnableWindow(false);
}

void VTSSpecialEventDlg::OnRadioobjid() 
{
	UpdateData();
	this->GetDlgItem(IDC_VALUE)->EnableWindow(false);
	this->GetDlgItem(IDC_OBJECTID)->EnableWindow(true);
	this->GetDlgItem(IDC_OBJECTIDBTN)->EnableWindow(true);	
}

void VTSSpecialEventDlg::OnValue() 
{
	m_calendarEntry.DoModal();
}

BOOL VTSSpecialEventDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	switch (m_nChoice) {
	case 0:
			OnRadiocalentry();
			break;
	case 1:
			OnRadioobjid();
			break;
	default:
			break;
	}			
	return TRUE;  
}

void VTSSpecialEventDlg::Encode(BACnetAPDUEncoder& enc, int context /* = kAppContext  */)
{
	switch(m_nChoice)
	{
	case 0:
		BACnetOpeningTag().Encode(enc, 0);
		m_calendarEntry.Encode(enc);
		BACnetClosingTag().Encode(enc, 0);
		break;
	case 1:
		m_calendarRef.Encode(enc, 1);
		break;
	default:
		break;
	}
	BACnetOpeningTag().Encode(enc, 2);
	m_listofTimeValues.Encode(enc);
	BACnetClosingTag().Encode(enc, 2);
	m_eventPriority.Encode(enc, 3);		
}
void VTSSpecialEventDlg::Decode(BACnetAPDUDecoder& dec )
{
  if(dec.pktLength != 0)
  {
	  BACnetAPDUTag		tagTestType;
	  dec.ExamineTag(tagTestType);
	  if(tagTestType.tagClass == openingTagClass)
		  m_nChoice = 0;	//calendarEntry
	  else
		  m_nChoice=tagTestType.tagNumber;
	  
	  switch(m_nChoice)
	  {
	  case 0:
		  BACnetOpeningTag().Decode(dec);
		  m_calendarEntry.Decode(dec);
		  BACnetClosingTag().Decode(dec);
		  break;
	  case 1:
		  m_calendarRef.Decode(dec);
		  m_calendarRef.ctrlNull = false;
		  break;
	  default:
		  break;
	  }		
	  BACnetOpeningTag().Decode(dec);
	  m_listofTimeValues.Decode(dec);
	  BACnetClosingTag().Decode(dec);
	  m_eventPriority.Decode(dec);
	  m_eventPriority.ctrlNull = false;
  }
}
