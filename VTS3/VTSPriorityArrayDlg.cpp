// VTSPriorityArrayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSPriorityArrayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSPriorityArrayDlg dialog


VTSPriorityArrayDlg::VTSPriorityArrayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSPriorityArrayDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSPriorityArrayDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VTSPriorityArrayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPriorityArrayDlg)
	DDX_Control(pDX, IDC_LIST_LISTOF, m_listOfItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSPriorityArrayDlg, CDialog)
	//{{AFX_MSG_MAP(VTSPriorityArrayDlg)
	ON_BN_CLICKED(IDC_LISTOF_ADD, OnListofAdd)
	ON_BN_CLICKED(IDC_LISTOF_DELETE, OnListofDelete)
	ON_BN_CLICKED(IDC_LISTOF_MODIFY, OnListofModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPriorityArrayDlg message handlers

void VTSPriorityArrayDlg::OnListofAdd() 
{	
	
	VTSPriorityValueDlg* dlg = new VTSPriorityValueDlg(this);
	if(dlg->DoModal()==IDOK)
	{	
		m_ListOfPriorityValue.AddTail(dlg);
		CString str;
		str.Format("PriorityValue %d", m_ListOfPriorityValue.GetCount()-1);
		m_listOfItem.AddString(str);
	}
				
}
 
void VTSPriorityArrayDlg::OnListofDelete() 
{	
	if(m_ListOfPriorityValue.GetCount() <= 0)
		return;
	int ndelete = m_listOfItem.GetCurSel();
	
	if(ndelete<0|| ndelete>= m_ListOfPriorityValue.GetCount())
	{
		throw "The specified index is beyond the scope of the sequenceof.";
	}
	POSITION pos = m_ListOfPriorityValue.FindIndex(ndelete);
	VTSPriorityValueDlg* pDeleteElem=m_ListOfPriorityValue.GetAt(pos);
	
	if(pDeleteElem != NULL)
	{
		delete pDeleteElem;
		pDeleteElem = NULL;
	}	
	m_ListOfPriorityValue.RemoveAt(pos);		
	m_listOfItem.DeleteString(ndelete);
	
}
 
void VTSPriorityArrayDlg::OnListofModify() 
{	
	int nmodify = m_listOfItem.GetCurSel();
	
	if(m_ListOfPriorityValue.GetCount() <= 0)
		return;
		if(nmodify<0)
		return;
		if(nmodify>= m_ListOfPriorityValue.GetCount())
			throw "The specified index is beyond the scope of the sequenceof.";
		POSITION pos = m_ListOfPriorityValue.FindIndex(nmodify);			
	VTSPriorityValueDlg* dlg = m_ListOfPriorityValue.GetAt(pos);
	dlg->DoModal();		
}

BOOL VTSPriorityArrayDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_ListOfPriorityValue.GetCount() <= 0);
	else
		for(int i = 0; i < m_ListOfPriorityValue.GetCount(); i++)
		{	
			CString str;
			str.Format("PriorityValue %d", i);
			m_listOfItem.AddString(str);	
		}
		
	return TRUE;  
}
 
void VTSPriorityArrayDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)	
	{
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			VTSPriorityValueDlg* dlg = new VTSPriorityValueDlg(this);
			dlg->Decode(dec);
			m_ListOfPriorityValue.AddTail(dlg);	
		}	
	}
}
 
void VTSPriorityArrayDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
		for(int i = 0; i < m_ListOfPriorityValue.GetCount(); i++)
		{	
			if(i>= m_ListOfPriorityValue.GetCount())
				throw "The specified index is beyond the scope of the sequenceof.";
			POSITION pos = m_ListOfPriorityValue.FindIndex(i);			
			VTSPriorityValueDlg* dlg = m_ListOfPriorityValue.GetAt(pos);
			dlg->Encode(enc);
		}
}
VTSPriorityArrayDlg::~VTSPriorityArrayDlg()
{
	for(int i = 0; i < m_ListOfPriorityValue.GetCount(); i++)
	{	
		if(i<0|| i>= m_ListOfPriorityValue.GetCount())
		{
			throw "The specified index is beyond the scope of the sequenceof.";
		}
		POSITION pos = m_ListOfPriorityValue.FindIndex(i);
		VTSPriorityValueDlg* pDeleteElem=m_ListOfPriorityValue.GetAt(pos);
		
		if(pDeleteElem != NULL)
		{
			delete pDeleteElem;
			pDeleteElem = NULL;
		}	
		m_ListOfPriorityValue.RemoveAt(pos);			
	}
}

