// VTSDailyScheduleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDailyScheduleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSDailyScheduleDlg dialog


VTSDailyScheduleDlg::VTSDailyScheduleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSDailyScheduleDlg::IDD, pParent)
{
    m_showAsListofTimeValue = FALSE;
	//{{AFX_DATA_INIT(VTSDailyScheduleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VTSDailyScheduleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDailyScheduleDlg)
	DDX_Control(pDX, IDC_LIST_LISTOF, m_listOfItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDailyScheduleDlg, CDialog)
	//{{AFX_MSG_MAP(VTSDailyScheduleDlg)
	ON_BN_CLICKED(IDC_LISTOF_ADD, OnListofAdd)
	ON_BN_CLICKED(IDC_LISTOF_DELETE, OnListofDelete)
	ON_BN_CLICKED(IDC_LISTOF_MODIFY, OnListofModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDailyScheduleDlg message handlers

void VTSDailyScheduleDlg::OnListofAdd() 
{	
	
	VTSTimeValueDlg* dlg = new VTSTimeValueDlg(this);
	if(dlg->DoModal()==IDOK)
	{	
		m_ListOfTimeValue.AddTail(dlg);
		CString str;
		str.Format("TimeValue %d", m_ListOfTimeValue.GetCount()-1);
		m_listOfItem.AddString(str);
	}
				
}
 
void VTSDailyScheduleDlg::OnListofDelete() 
{	
	if(m_ListOfTimeValue.GetCount() <= 0)
		return;
	int ndelete = m_listOfItem.GetCurSel();
	
	if(ndelete<0|| ndelete>= m_ListOfTimeValue.GetCount())
	{
		throw "The specified index is beyond the scope of the sequenceof.";
	}
	POSITION pos = m_ListOfTimeValue.FindIndex(ndelete);
	VTSTimeValueDlg* pDeleteElem=m_ListOfTimeValue.GetAt(pos);
	
	if(pDeleteElem != NULL)
	{
		delete pDeleteElem;
		pDeleteElem = NULL;
	}	
	m_ListOfTimeValue.RemoveAt(pos);		
	m_listOfItem.DeleteString(ndelete);
	
}
 
void VTSDailyScheduleDlg::OnListofModify() 
{	
	int nmodify = m_listOfItem.GetCurSel();
	
	if(m_ListOfTimeValue.GetCount() <= 0)
		return;
		if(nmodify<0)
		return;
		if(nmodify>= m_ListOfTimeValue.GetCount())
			throw "The specified index is beyond the scope of the sequenceof.";
		POSITION pos = m_ListOfTimeValue.FindIndex(nmodify);			
	VTSTimeValueDlg* dlg = m_ListOfTimeValue.GetAt(pos);
	dlg->DoModal();		
}

BOOL VTSDailyScheduleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString texttile;
	if(m_showAsListofTimeValue)
	{
		texttile.Format("ListOfTimeValues");
		this->SetWindowText(texttile);
	}
	if(m_ListOfTimeValue.GetCount() <= 0);
	else
		for(int i = 0; i < m_ListOfTimeValue.GetCount(); i++)
		{	
			CString str;
			str.Format("TimeValue %d", i);
			m_listOfItem.AddString(str);	
		}
		
	return TRUE;  
}
 
void VTSDailyScheduleDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)	
	{
	if(!m_showAsListofTimeValue)
		BACnetOpeningTag().Decode(dec);
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			VTSTimeValueDlg* dlg = new VTSTimeValueDlg(this);
			dlg->Decode(dec);
			m_ListOfTimeValue.AddTail(dlg);	
		}
	if(!m_showAsListofTimeValue)
		BACnetClosingTag().Decode(dec);		
	}
}
 
void VTSDailyScheduleDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
	if(!m_showAsListofTimeValue)
		BACnetOpeningTag().Encode(enc, 0);
		for(int i = 0; i < m_ListOfTimeValue.GetCount(); i++)
		{	
			if(i<0|| i>= m_ListOfTimeValue.GetCount())
				throw "The specified index is beyond the scope of the sequenceof.";
			POSITION pos = m_ListOfTimeValue.FindIndex(i);			
			VTSTimeValueDlg* dlg = m_ListOfTimeValue.GetAt(pos);
			dlg->Encode(enc);
		}
	if(!m_showAsListofTimeValue)
		BACnetClosingTag().Encode(enc, 0);	
}
VTSDailyScheduleDlg::~VTSDailyScheduleDlg()
{
	for(int i = 0; i < m_ListOfTimeValue.GetCount(); i++)
	{	
		if(i<0|| i>= m_ListOfTimeValue.GetCount())
		{
			throw "The specified index is beyond the scope of the sequenceof.";
		}
		POSITION pos = m_ListOfTimeValue.FindIndex(i);
		VTSTimeValueDlg* pDeleteElem=m_ListOfTimeValue.GetAt(pos);
		
		if(pDeleteElem != NULL)
		{
			delete pDeleteElem;
			pDeleteElem = NULL;
		}	
		m_ListOfTimeValue.RemoveAt(pos);			
	}
}

