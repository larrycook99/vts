// VTSActionListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSActionListDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSActionListDlg dialog
 
VTSActionListDlg::VTSActionListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSActionListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSActionListDlg)
	//}}AFX_DATA_INIT
}

 
void VTSActionListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSActionListDlg)
	DDX_Control(pDX, IDC_LIST_LISTOF, m_listOfItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSActionListDlg, CDialog)
	//{{AFX_MSG_MAP(VTSActionListDlg)
	ON_BN_CLICKED(IDC_LISTOF_ADD, OnListOfAdd)
	ON_BN_CLICKED(IDC_LISTOF_DELETE, OnListOfDelete)
	ON_BN_CLICKED(IDC_LISTOF_MODIFY, OnListOfModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSActionListDlg message handlers
 
void VTSActionListDlg::OnListOfAdd() 
{	
	
	VTSActionCommandDlg* dlg = new VTSActionCommandDlg(this);
	if(dlg->DoModal()==IDOK)
	{	
		m_ListOfactioncommand.AddTail(dlg);
		CString str;
		str.Format("ActionCommand %d", m_ListOfactioncommand.GetCount()-1);
		m_listOfItem.AddString(str);
	}
				
}
 
void VTSActionListDlg::OnListOfDelete() 
{	
	if(m_ListOfactioncommand.GetCount() <= 0)
		return;
	int ndelete = m_listOfItem.GetCurSel();
	
	if(ndelete<0|| ndelete>= m_ListOfactioncommand.GetCount())
	{
		throw "The specified index is beyond the scope of the sequenceof.";
	}
	POSITION pos = m_ListOfactioncommand.FindIndex(ndelete);
	VTSActionCommandDlg* pDeleteElem=m_ListOfactioncommand.GetAt(pos);
	
	if(pDeleteElem != NULL)
	{
		delete pDeleteElem;
		pDeleteElem = NULL;
	}	
	m_ListOfactioncommand.RemoveAt(pos);		
	m_listOfItem.DeleteString(ndelete);
	
}
 
void VTSActionListDlg::OnListOfModify() 
{	
	int nmodify = m_listOfItem.GetCurSel();
	
	if(m_ListOfactioncommand.GetCount() <= 0)
		return;
		if(nmodify<0)
		return;
		if(nmodify>= m_ListOfactioncommand.GetCount())
			throw "The specified index is beyond the scope of the sequenceof.";
		POSITION pos = m_ListOfactioncommand.FindIndex(nmodify);			
	VTSActionCommandDlg* dlg = m_ListOfactioncommand.GetAt(pos);
	dlg->DoModal();		
}
 
void VTSActionListDlg::OnOK() 
{	
	CDialog::OnOK();
}
 
BOOL VTSActionListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_ListOfactioncommand.GetCount() <= 0);
	else
		for(int i = 0; i < m_ListOfactioncommand.GetCount(); i++)
		{	
			CString str;
			str.Format("ActionCommand %d", i);
			m_listOfItem.AddString(str);	
		}
		
	return TRUE;  
}
 
void VTSActionListDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)	
	{
		BACnetOpeningTag().Decode(dec);
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			VTSActionCommandDlg* dlg = new VTSActionCommandDlg(this);
			dlg->Decode(dec);
			m_ListOfactioncommand.AddTail(dlg);	
		}
		BACnetClosingTag().Decode(dec);		
	}
}
 
void VTSActionListDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
		BACnetOpeningTag().Encode(enc, 0);
		for(int i = 0; i < m_ListOfactioncommand.GetCount(); i++)
		{	
			if(i<0|| i>= m_ListOfactioncommand.GetCount())
				throw "The specified index is beyond the scope of the sequenceof.";
			POSITION pos = m_ListOfactioncommand.FindIndex(i);			
			VTSActionCommandDlg* dlg = m_ListOfactioncommand.GetAt(pos);
			dlg->Encode(enc);
		}
		BACnetClosingTag().Encode(enc, 0);	
}
VTSActionListDlg::~VTSActionListDlg()
{
	for(int i = 0; i < m_ListOfactioncommand.GetCount(); i++)
	{	
		if(i<0|| i>= m_ListOfactioncommand.GetCount())
		{
			throw "The specified index is beyond the scope of the sequenceof.";
		}
		POSITION pos = m_ListOfactioncommand.FindIndex(i);
		VTSActionCommandDlg* pDeleteElem=m_ListOfactioncommand.GetAt(pos);
		
		if(pDeleteElem != NULL)
		{
			delete pDeleteElem;
			pDeleteElem = NULL;
		}	
		m_ListOfactioncommand.RemoveAt(pos);			
	}
}
