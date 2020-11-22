// VTSListOfPropertyStatesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSListOfPropertyStatesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSListOfPropertyStatesDlg dialog


VTSListOfPropertyStatesDlg::VTSListOfPropertyStatesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSListOfPropertyStatesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSListOfPropertyStatesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VTSListOfPropertyStatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSListOfPropertyStatesDlg)
	DDX_Control(pDX, IDC_LIST_LISTOF, m_listOfItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSListOfPropertyStatesDlg, CDialog)
	//{{AFX_MSG_MAP(VTSListOfPropertyStatesDlg)
	ON_BN_CLICKED(IDC_LISTOF_ADD, OnListofAdd)
	ON_BN_CLICKED(IDC_LISTOF_DELETE, OnListofDelete)
	ON_BN_CLICKED(IDC_LISTOF_MODIFY, OnListofModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSListOfPropertyStatesDlg message handlers

void VTSListOfPropertyStatesDlg::OnListofAdd() 
{	
	
	VTSPropertyStatesDlg* dlg = new VTSPropertyStatesDlg(this);
	if(dlg->DoModal()==IDOK)
	{	
		m_ListOfpropstates.AddTail(dlg);
		CString str;
		str.Format("propertystates %d", m_ListOfpropstates.GetCount()-1);
		m_listOfItem.AddString(str);
	}
				
}
 
void VTSListOfPropertyStatesDlg::OnListofDelete() 
{	
	if(m_ListOfpropstates.GetCount() <= 0)
		return;
	int ndelete = m_listOfItem.GetCurSel();
	
	if(ndelete<0|| ndelete>= m_ListOfpropstates.GetCount())
	{
		throw "The specified index is beyond the scope of the sequenceof.";
	}
	POSITION pos = m_ListOfpropstates.FindIndex(ndelete);
	VTSPropertyStatesDlg* pDeleteElem=m_ListOfpropstates.GetAt(pos);
	
	if(pDeleteElem != NULL)
	{
		delete pDeleteElem;
		pDeleteElem = NULL;
	}	
	m_ListOfpropstates.RemoveAt(pos);		
	m_listOfItem.DeleteString(ndelete);
	
}
 
void VTSListOfPropertyStatesDlg::OnListofModify() 
{	
	int nmodify = m_listOfItem.GetCurSel();
	
	if(m_ListOfpropstates.GetCount() <= 0)
		return;
		if(nmodify<0)
		return;
		if(nmodify>= m_ListOfpropstates.GetCount())
			throw "The specified index is beyond the scope of the sequenceof.";
		POSITION pos = m_ListOfpropstates.FindIndex(nmodify);			
	VTSPropertyStatesDlg* dlg = m_ListOfpropstates.GetAt(pos);
	dlg->DoModal();		
}
BOOL VTSListOfPropertyStatesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_ListOfpropstates.GetCount() <= 0);
	else
		for(int i = 0; i < m_ListOfpropstates.GetCount(); i++)
		{	
			CString str;
			str.Format("propertystates %d", i);
			m_listOfItem.AddString(str);	
		}
		
	return TRUE;  
}
 
void VTSListOfPropertyStatesDlg::Decode(BACnetAPDUDecoder& dec)
{
	if (dec.pktLength != 0)	
	{
		while(dec.pktLength != 0)
		{	
			BACnetAPDUTag tag;
			dec.ExamineTag(tag);
			if(tag.tagClass == closingTagClass)
				break;
			VTSPropertyStatesDlg* dlg = new VTSPropertyStatesDlg(this);
			dlg->Decode(dec);
			m_ListOfpropstates.AddTail(dlg);	
		}	
	}
}
 
void VTSListOfPropertyStatesDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
		for(int i = 0; i < m_ListOfpropstates.GetCount(); i++)
		{	
			if(i<0|| i>= m_ListOfpropstates.GetCount())
				throw "The specified index is beyond the scope of the sequenceof.";
			POSITION pos = m_ListOfpropstates.FindIndex(i);			
			VTSPropertyStatesDlg* dlg = m_ListOfpropstates.GetAt(pos);
			dlg->Encode(enc);
		}
}
VTSListOfPropertyStatesDlg::~VTSListOfPropertyStatesDlg()
{
	for(int i = 0; i < m_ListOfpropstates.GetCount(); i++)
	{	
		if(i<0|| i>= m_ListOfpropstates.GetCount())
		{
			throw "The specified index is beyond the scope of the sequenceof.";
		}
		POSITION pos = m_ListOfpropstates.FindIndex(i);
		VTSPropertyStatesDlg* pDeleteElem=m_ListOfpropstates.GetAt(pos);
		
		if(pDeleteElem != NULL)
		{
			delete pDeleteElem;
			pDeleteElem = NULL;
		}	
		m_ListOfpropstates.RemoveAt(pos);			
	}
}
