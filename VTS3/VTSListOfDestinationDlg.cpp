// VTSListOfDestinationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSListOfDestinationDlg.h"
#include "VTSDestinationDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSListOfDestinationDlg dialog
 
VTSListOfDestinationDlg::VTSListOfDestinationDlg(int ItemType, CWnd* pParent /*=NULL*/)
	: CDialog(VTSListOfDestinationDlg::IDD, pParent)
	, m_nItemType(ItemType)
{
	//{{AFX_DATA_INIT(VTSListOfDestinationDlg)
	//}}AFX_DATA_INIT
}

 
void VTSListOfDestinationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSListOfDestinationDlg)
	DDX_Control(pDX, IDC_LIST_LISTOF, m_listOfItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSListOfDestinationDlg, CDialog)
	//{{AFX_MSG_MAP(VTSListOfDestinationDlg)
	ON_BN_CLICKED(IDC_LISTOF_ADD, OnListOfAdd)
	ON_BN_CLICKED(IDC_LISTOF_DELETE, OnListOfDelete)
	ON_BN_CLICKED(IDC_LISTOF_MODIFY, OnListOfModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSListOfDestinationDlg message handlers
 
void VTSListOfDestinationDlg::OnListOfAdd() 
{	
		switch(m_nItemType)
	{
		case 0: //Destination
			{	
				VTSDestinationDlg dlg(this);
				if(dlg.DoModal()==IDOK)
				{	
					m_ListOfDestination.AddElement(&dlg.m_Value);
					CString str;
					str.Format("Destination %d", m_ListOfDestination.GetItemCount()-1);
					m_listOfItem.AddString(str);
				}
				break;
			}
		default:
			break;
	}
}
 
void VTSListOfDestinationDlg::OnListOfDelete() 
{
		switch(m_nItemType)
	{
		case 0: //Destination
			{	
				if(m_ListOfDestination.GetItemCount() <= 0)
					break;
				int ndelete = m_listOfItem.GetCurSel();
				m_ListOfDestination.DeleteElement(ndelete);
				m_listOfItem.DeleteString(ndelete);
				break;
			}
		default:
			break;
	}	
}
 
void VTSListOfDestinationDlg::OnListOfModify() 
{	
	int nmodify = m_listOfItem.GetCurSel();
		switch(m_nItemType)
	{
		case 0: //Destination
			{	
				if(m_ListOfDestination.GetItemCount() <= 0)
					break;
				if(nmodify<0)
					return;
				if(nmodify>= m_ListOfDestination.GetItemCount())
					throw "The specified index is beyond the scope of the sequenceof.";
				BACnetDestination* pValue = m_ListOfDestination.GetElement(nmodify);		
				VTSDestinationDlg dlg(this);
				dlg.m_Value = *pValue;
					if(dlg.DoModal()==IDOK)
					{
						m_ListOfDestination.SetElement(nmodify,&dlg.m_Value);
					}
				break;
			}
		default:
			break;
	}
	
}
 
void VTSListOfDestinationDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
 
BOOL VTSListOfDestinationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		switch(m_nItemType)
	{
		case 0: //Destination
			{	
				if(m_ListOfDestination.GetItemCount() <= 0)
					break;
				for(int i = 0; i < m_ListOfDestination.GetItemCount(); i++)
				{	
					CString str;
					str.Format("Destination %d", i);
					m_listOfItem.AddString(str);	
				}
					break;
			}
		default:
			break;
	}
	
	return TRUE;  
}
 
void VTSListOfDestinationDlg::Decode(BACnetAPDUDecoder& dec)
{
		switch(m_nItemType)
	{
		case 0: //Destination
			{
				if (dec.pktLength != 0)
				m_ListOfDestination.Decode(dec);
				break;
			}
		default:
			break;
	}		
}
 
void VTSListOfDestinationDlg::Encode(BACnetAPDUEncoder& enc,int context)
{
		switch(m_nItemType)
	{
		case 0: //Destination
			{
				m_ListOfDestination.Encode(enc,context);
			}
		default:
			break;
	}		
}


