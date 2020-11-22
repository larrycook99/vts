// VTSListOfDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSListOfDlg.h"
#include "VTSDestinationDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSListOfDlg dialog
 
VTSListOfDlg::VTSListOfDlg(int ItemType, CWnd* pParent /*=NULL*/)
	: CDialog(VTSListOfDlg::IDD, pParent)
	, m_nItemType(ItemType)
{
	//{{AFX_DATA_INIT(VTSListOfDlg)
	//}}AFX_DATA_INIT
}

 
void VTSListOfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSListOfDlg)
	DDX_Control(pDX, IDC_LIST_LISTOF, m_listOfItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSListOfDlg, CDialog)
	//{{AFX_MSG_MAP(VTSListOfDlg)
	ON_BN_CLICKED(IDC_LISTOF_ADD, OnListOfAdd)
	ON_BN_CLICKED(IDC_LISTOF_DELETE, OnListOfDelete)
	ON_BN_CLICKED(IDC_LISTOF_MODIFY, OnListOfModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSListOfDlg message handlers
 
void VTSListOfDlg::OnListOfAdd() 
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
 
void VTSListOfDlg::OnListOfDelete() 
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
 
void VTSListOfDlg::OnListOfModify() 
{	
	int nmodify = m_listOfItem.GetCurSel();
		switch(m_nItemType)
	{
		case 0: //Destination
			{	
				if(m_ListOfDestination.GetItemCount() <= 0)
					break;
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
 
void VTSListOfDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
 
BOOL VTSListOfDlg::OnInitDialog() 
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
 
void VTSListOfDlg::Decode(BACnetAPDUDecoder& dec)
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
 
void VTSListOfDlg::Encode(BACnetAPDUEncoder& enc,int context)
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


