// TreeListCtrl.cpp: implementation of the CTreeListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TreeListCtrl.h"
#include "VTS.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "VTSStatisticsCollector.h"
#include "VTSStatisticsDlg.h"
extern CString group[];
extern CString BACnet_msgs[];
extern int Service_Type_Index[];
extern VTSStatisticsCollector* gStatisticsCollector;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGroupIndex::CGroupIndex(int start,int end)
{
	m_nStartPos=start;
	m_nEndPos=end;
}

CGroupIndex::~CGroupIndex()
{

}
CTreeListCtrl::CTreeListCtrl(VTSStatisticsDlgPtr pParentDlg)
{
	m_pParentDlg=pParentDlg;
}

CTreeListCtrl::~CTreeListCtrl()
{
}

BEGIN_MESSAGE_MAP(CTreeListCtrl, CSuperGridCtrl)
	//{{AFX_MSG_MAP(CTreeListCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/***********************************************************************
Create the tree including general ,size distribution and watch items

***********************************************************************/

void CTreeListCtrl::InitializeGrid()
{
	int index=0,startPos=0,endPos=0;
	int i;  // MAG 31JAN06  i was used out of scope below

	for(i =0;i<10;i++)
	{
		startPos=index;
		for(int j =0;j<Service_Type_Index[i];j++)
		{
			index++;
		}
		endPos=index-1;
		m_mapGroupToIndex.SetAt(group[i],new CGroupIndex(startPos,endPos));
	}
	//create service to index map

	LPTSTR lpszCols[] = {_T("IP Send"),_T("IP Receive"),
						 _T("Ethernet Send"),_T("Ethernet Receive"),
						 _T("Arcnet Send"),_T("Arcnet Receive"),
						 _T("MSTP Send"),_T("MSTP Receive"),
						 _T("PTP Send"),_T("PTP Receive"),
						 _T("Total Send"),_T("Total Receive"),
						 0};

	LV_COLUMN   lvColumn;
	//initialize the columns

	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 250;
	lvColumn.pszText =_T("Service Type");
	InsertColumn(0,&lvColumn);
	
	int column=1;
	for(int x = 0; lpszCols[x]!=NULL; x++)
    {
		lvColumn.cx = 85;
		if(gStatisticsCollector->m_bColumnStatus[x]==TRUE)
		{
			lvColumn.pszText = lpszCols[x];
			InsertColumn(column++,&lvColumn);
		}
	}
	//insert columns
	
	CString sub0="",sub1="";
	CItemInfo* lp = new CItemInfo();
	lp->SetItemText(_T("General"));
	CTreeItem * pRoot = InsertRootItem(lp);
	GetGeneralStatistics();
	//insert first root item named "General" and get the statistics data
	
	index=0;
	for(i =0;i<10;i++)
	{
		lp = new CItemInfo();
		lp->SetItemText(group[i]);
		CTreeItem * pSecondRoot = InsertItem(pRoot,lp);
		GetGroupStatistics(group[i]);
		
		for(int j =0;j<Service_Type_Index[i];j++)
		{
			CItemInfo* pChild=new CItemInfo();
			pChild->SetItemText(BACnet_msgs[index]);
			InsertItem(pSecondRoot,pChild);
			GetServiceStatistics(BACnet_msgs[index++]);
		}
	}
	//create the general root and sub items

	lp = new CItemInfo();
	lp->SetItemText(_T("Size Distribution"));
	pRoot = InsertRootItem(lp);
	for(i=0;i<3;i++)
	{
		lp = new CItemInfo();
		CString sub="";
		if(i==0)
		{
			sub.Format(_T("%s%d"),"<=",gStatisticsCollector->m_nDistributionArray[1]);
			lp->SetItemText(sub);
			InsertItem(pRoot,lp);			
			
			GetSizeStatistics(gStatisticsCollector->m_nDistributionArray[0],gStatisticsCollector->m_nDistributionArray[1],sub);
		}
		else if(i==2)
		{
			sub.Format(_T("%s%d"),">=",gStatisticsCollector->m_nDistributionArray[4]);
			lp->SetItemText(sub);
			InsertItem(pRoot,lp);			
			
			GetSizeStatistics(gStatisticsCollector->m_nDistributionArray[4],gStatisticsCollector->m_nDistributionArray[5],sub);
			
		}
		else
		{			
			sub.Format(_T("%d%s%d"),gStatisticsCollector->m_nDistributionArray[2],"-",gStatisticsCollector->m_nDistributionArray[3]);
			lp->SetItemText(sub);
			InsertItem(pRoot,lp);
			
			GetSizeStatistics(gStatisticsCollector->m_nDistributionArray[2],gStatisticsCollector->m_nDistributionArray[3],sub);
		}
	}
	//create size distribution root and sub items
		
	lp = new CItemInfo();
	lp->SetItemText(_T("Watch"));
	pRoot=InsertRootItem(lp);
	POSITION pos = gStatisticsCollector->m_WatchGroupList.GetHeadPosition();
	for (i=0;i < gStatisticsCollector->m_WatchGroupList.GetCount();i++)
	{
	   CString groupName=gStatisticsCollector->m_WatchGroupList.GetNext(pos);
	   lp = new CItemInfo();
	   lp->SetItemText(groupName);
	   CTreeItem * pSecondRoot = InsertItem(pRoot,lp);
	   GetGroupStatistics(groupName);
	   CGroupIndex* index;
	   m_mapGroupToIndex.Lookup(groupName,(CGroupIndex*&)index);
	   for(int j=index->m_nStartPos;j<=index->m_nEndPos;j++)
	   {
			CString serviceName=BACnet_msgs[j];
			lp = new CItemInfo();
			lp->SetItemText(serviceName);
			InsertItem(pSecondRoot,lp);
			GetServiceStatistics(serviceName);
	   }
	}
	pos = gStatisticsCollector->m_WatchServiceList.GetHeadPosition();
	for (i=0;i<gStatisticsCollector->m_WatchServiceList.GetCount();i++)
	{
	   CString serviceName=gStatisticsCollector->m_WatchServiceList.GetNext(pos);
	   lp = new CItemInfo();
	   lp->SetItemText(serviceName);
	   InsertItem(pRoot,lp);
	   GetServiceStatistics(serviceName);
	}
	//create watch root and sub items

}

int CTreeListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSuperGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	//////////	///////////
	//remember this	
	///////////////////

	//associate imagelist with listviewctrl, 
	//this imagelist is only for the first column re: the tree column	
	if(m_image==NULL)
		if(!m_image.Create(IDB_PACKETTREE,16,1,RGB(0, 255, 255)))
			return -1;

	SetImageList(&m_image, LVSIL_SMALL);
	CImageList *pImageList = GetImageList(LVSIL_SMALL);
	if(pImageList)
		ImageList_GetIconSize(pImageList->m_hImageList, &m_cxImage, &m_cyImage);
	else
		return -1;

	return 0;
}

/************************************************************************
	called when left button down message is sent
	notify the statistics dialog to show current seleceted item's statistic

  ***********************************************************************/
BOOL CTreeListCtrl::OnItemLButtonDown(LVHITTESTINFO& ht)
{
	CItemInfo* pInfo;
	CString type;

	int empty[12]={0,0,0,0,0,0,0,0,0,0,0,0};

	if(ht.iItem!=-1)
	{
		CTreeItem*pSelItem = GetTreeItem(ht.iItem);
		if(IsRoot(pSelItem))//root
		{
			pInfo=GetData(pSelItem);
			type=pInfo->GetItemText();
			
			if(pSelItem==GetRootItem(0))//if general information
			{

				GetGeneralStatistics();
				m_pParentDlg->UpdateStatisticsView(type,m_nColumnData);
			}
			else
			{
				m_pParentDlg->UpdateStatisticsView(type,empty);
			}
		}
		else //child
		{	
			CTreeItem* pParent = GetParentItem(pSelItem);
			while(!IsRoot(pParent))
			{
				pParent=GetParentItem(pParent);
			}//get root item of source
			pInfo = GetData(pSelItem);
			type=pInfo->GetItemText();

			if(pParent==GetRootItem(1))//size-distribution child
			{
				if(type.Find("<=")!=-1)
				{
					GetSizeStatistics(gStatisticsCollector->m_nDistributionArray[0],gStatisticsCollector->m_nDistributionArray[1],type);
					m_pParentDlg->UpdateStatisticsView(type,m_nColumnData);
				}
				if(type.Find(">=")!=-1)
				{
					GetSizeStatistics(gStatisticsCollector->m_nDistributionArray[4],gStatisticsCollector->m_nDistributionArray[5],type);
					m_pParentDlg->UpdateStatisticsView(type,m_nColumnData);
				}
				if(type.Find("-")!=-1)
				{
					GetSizeStatistics(gStatisticsCollector->m_nDistributionArray[2],gStatisticsCollector->m_nDistributionArray[3],type);
					m_pParentDlg->UpdateStatisticsView(type,m_nColumnData);
				}
			}
			
			if(pParent==GetRootItem(0)||pParent==GetRootItem(2)) //if general or watch root's child
			{
				if(ItemHasChildren(pSelItem))//if group
				{
					GetGroupStatistics(type);
					m_pParentDlg->UpdateStatisticsView(type,m_nColumnData);
				}
				else // if one certain service
				{
					GetServiceStatistics(type);
					m_pParentDlg->UpdateStatisticsView(type,m_nColumnData);
				}
			}
			
		}
	}
	return 1;
}

void CTreeListCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	switch(pLVKeyDow->wVKey)
	{
		case VK_SPACE: 
			{
				if(GetExtendedStyle() & LVS_EX_CHECKBOXES)
				{	
					int nIndex = GetSelectedItem();
					if(nIndex !=-1)
					{
						CTreeItem* pItem = GetTreeItem(nIndex);
						if(pItem!=NULL)
						{
							CItemInfo *pInfo = GetData(pItem);
							pInfo->SetCheck(!pInfo->GetCheck());
						}
					}
				}
			}break;

		case VK_DELETE: 
			{
				
				int nItem = GetSelectedItem();
				CTreeItem* pItem=GetTreeItem(nItem);
				if(IsRoot(pItem))
					break;//can't delete root item

				CTreeItem* pParent = GetParentItem(pItem);
			
				
				while(!IsRoot(pParent))
				{
					pParent=GetParentItem(pParent);
				}//get root item of source

				if(pParent!=GetRootItem(2))
					break;
			
				//if child of watch , do delete
				if(nItem!=-1)
				{
					CTreeItem* pSelItem = GetTreeItem(nItem);
						
					CItemInfo* lp = GetData(pSelItem);
					CString strData = lp->GetItemText();

					if(pSelItem != NULL)
					{
						if(ItemHasChildren(pSelItem))
						{
							POSITION pos = gStatisticsCollector->m_WatchGroupList.Find(strData);
							gStatisticsCollector->m_WatchGroupList.RemoveAt(pos);
						}
						else
						{
							POSITION pos = gStatisticsCollector->m_WatchServiceList.Find(strData);
							gStatisticsCollector->m_WatchServiceList.RemoveAt(pos);
						}
						if(OnDeleteItem(pSelItem, nItem))
							DeleteItemEx(pSelItem, nItem);
					}	
				}
			}	break;


		case VK_MULTIPLY:
			{  
				int nIndex = GetSelectedItem();
				if(nIndex != -1)
				{
					CWaitCursor wait;
					SetRedraw(0);
					CTreeItem *pParent = GetTreeItem(nIndex);
					int nScroll=0;
					if(OnVKMultiply(pParent, nIndex))
					{	
						ExpandAll(pParent, nScroll);
					}		
					SetRedraw(1);
					RedrawItems(nIndex, nScroll);
					EnsureVisible(nScroll, TRUE);
				 }
			 }break;

		case VK_ADD:
			{
					int nIndex = GetSelectedItem();
					if(nIndex!=-1)
					{
						CWaitCursor wait;
						CTreeItem *pSelItem = GetTreeItem(nIndex);
						int nScrollIndex = 0;
						if(OnVKAdd(pSelItem, nIndex))
						{
							 nScrollIndex = Expand(pSelItem, nIndex);
						}
						InvalidateItemRect(nIndex);
						EnsureVisible(nScrollIndex, 1);
					}

			}break;


		case VK_SUBTRACT:
			{
				int nIndex = GetSelectedItem();
				if(nIndex!=-1)
				{
					CWaitCursor wait;
					CTreeItem *pSelItem = GetTreeItem(nIndex);
					if(OnVkSubTract(pSelItem, nIndex))
					{
						Collapse(pSelItem);
					}
					InvalidateItemRect(nIndex);
				}
			}break;
		default :break;
	}
	*pResult = 0;
}

void CTreeListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bIsDragging == TRUE)
    {
		KillTimer(1);
        CImageList::DragLeave(this);
        CImageList::EndDrag();
        ReleaseCapture();
        m_bIsDragging = FALSE;
		SetItemState(m_nDragTarget, 0, LVIS_DROPHILITED);
        if((m_nDragTarget != -1) && (m_nDragTarget != m_nDragItem) && (m_nDragItem!=-1))//no drop on me self
        {
		
			CTreeItem* pSource = GetTreeItem(m_nDragItem);
			CTreeItem* pTarget = GetTreeItem(m_nDragTarget);
			if(IsRoot(pSource))//can't drag root item
				return;

			CTreeItem* pSourceParent = GetParentItem(pSource);
			CTreeItem* pTargetParent = GetParentItem(pTarget);
			
			if(pSourceParent==pTarget) //can't drag child to parent
				return;

			while(!IsRoot(pSourceParent))
			{
				pSourceParent=GetParentItem(pSourceParent);
			}//get root item of source

			if(pSourceParent==GetRootItem(2)||pSourceParent==GetRootItem(1))
				return;//can't drag watch item and size items
			
			if(pTarget!=GetRootItem(2))
				return;//can't drag to none-watch root

			if(!IsChildOf(pSource, pTarget))//can't drag parent to child
			{
					
				CItemInfo* lp = GetData(pSource);
				CString strData = lp->GetItemText();
				if(ItemHasChildren(pSource))
				{
					gStatisticsCollector->m_WatchGroupList.AddTail(strData);
				}
				else
					gStatisticsCollector->m_WatchServiceList.AddTail(strData);

				CWaitCursor wait;
				SetRedraw(0);
				if(DoDragDrop(pTarget, pSource))
				{
					UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
					SetItemState(m_nDragTarget, uflag, uflag);
					m_nDragItem=-1;
					//delete source
				/*	int nIndex = NodeToIndex(pSource);			
					DeleteItem(nIndex);
					HideChildren(pSource, TRUE, nIndex);
					Delete(pSource);*/
					InternaleUpdateTree();
					SetRedraw(1);
					InvalidateRect(NULL);
					UpdateWindow();
				}else
					SetRedraw(1);
			}
	    }
    }
    else
		CListCtrl::OnLButtonUp(nFlags, point);
}

BOOL CTreeListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(GetFocus()==this)
		{
			switch( pMsg->wParam )
			{
				case VK_LEFT:
					{
						// Decrement the order number.
						m_CurSubItem--;
						if(m_CurSubItem < 0) 
							m_CurSubItem = 0;
						else{
							
							CHeaderCtrl* pHeader = GetHeaderCtrl();
							// Make the column visible.
							// We have to take into account that the header may be reordered.
							MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem));
							// Invalidate the item.
							int iItem = GetSelectedItem();
							if( iItem != -1 )
								InvalidateItemRect(iItem);
							
						}
					}
					return TRUE;

				case VK_RIGHT:
					{
						// Increment the order number.
						m_CurSubItem++;
						CHeaderCtrl* pHeader = GetHeaderCtrl();
						int nColumnCount = pHeader->GetItemCount();
						// Don't go beyond the last column.
						if( m_CurSubItem > nColumnCount -1 ) 
							m_CurSubItem = nColumnCount-1;
						else
						{
							MakeColumnVisible(Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem));
							 
							int iItem = GetSelectedItem();
							// Invalidate the item.
							if( iItem != -1 )
								InvalidateItemRect(iItem);
							
						}
					}
					return TRUE;

				case VK_RETURN://edit itemdata
					{//disabled by Hu Meng for not-editable
						/*
						BOOL bResult = OnVkReturn();
						if(!bResult)
						{
							int iItem = GetSelectedItem();
							CTreeItem* p= GetTreeItem(iItem);
							if(GetParentItem(p)!=GetRootItem(1))
								return 0;
							else
							{
								CSizeRangeInputDlg dlg;
								if(dlg.DoModal()==IDOK)
								{
								//	dlg.UpdateData(true);
									CItemInfo *lp = GetData(p);
									if(lp!=NULL)
									{
										lp->SetItemText("Dynamic");
										UpdateData(p, lp, TRUE); 
										//set bUpdateRow to TRUE here, this would update the listview row e.g write the content of CItemInfo
									}
								}
								//AfxMessageBox("enter");
							}

							
						}
							return 1;
							*/
					}
					break;
				default:
					break;
			}
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}

//helper function to copy CItemInfo used when drag/drop you must override this this function to suit your own CItemInfo class
CItemInfo* CTreeListCtrl::CopyData(CItemInfo* lpSrc)
{
	ASSERT(lpSrc!=NULL);
	CItemInfo* lpDest = new CItemInfo;
	//well okay I put all the copy thing in one function, located in CItemInfo class, 
	//so you should check out this function, remember to modify this function each time you add new data to CItemInfo class.
	lpDest->CopyObjects(lpSrc);
	return lpDest;
}

/*********************************************************

find group item and update the data
**********************************************************/
void CTreeListCtrl::GetGroupStatistics(CString group)
{
	//get group index range from the map
	int start,end,i;  // MAG 31JAN06 i was used out of scope below
	CGroupIndex* p;
	m_mapGroupToIndex.Lookup(group,(CGroupIndex*&)p);
	start=p->m_nStartPos;
	end=p->m_nEndPos;
	
	//empty buffer
	m_nColumnData[1] = 0;
	m_nColumnData[0] = 0;
	m_nColumnData[6] = 0;
	m_nColumnData[7] = 0;
	m_nColumnData[9] = 0;
	m_nColumnData[8] = 0;
	m_nColumnData[11] = 0;
	m_nColumnData[10] = 0;
	m_nColumnData[2] = 0;
	m_nColumnData[3] = 0;
	m_nColumnData[4] = 0;
	m_nColumnData[5] = 0;

	//get the statistics data
	for(i =start;i<=end;i++)
	{
		m_nColumnData[0]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::ipProtocol,CPacketCounter::sendPacket);
		m_nColumnData[1]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::ipProtocol,CPacketCounter::receivePacket);
		m_nColumnData[2]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::ethernetProtocol,CPacketCounter::sendPacket);
		m_nColumnData[3]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::ethernetProtocol,CPacketCounter::receivePacket);
		m_nColumnData[4]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::arcnetProtocol,CPacketCounter::sendPacket);
		m_nColumnData[5]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::arcnetProtocol,CPacketCounter::receivePacket);
		m_nColumnData[6]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::mstpProtocol,CPacketCounter::sendPacket);
		m_nColumnData[7]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::mstpProtocol,CPacketCounter::receivePacket);
		m_nColumnData[8]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::ptpProtocol,CPacketCounter::sendPacket);
		m_nColumnData[9]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::ptpProtocol,CPacketCounter::receivePacket);
		m_nColumnData[10]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::allProtocol,CPacketCounter::sendPacket);
		m_nColumnData[11]+=gStatisticsCollector->GetPacketCounter(BACnet_msgs[i],CPacketCounter::allProtocol,CPacketCounter::receivePacket);
	}
	
	//format the display string
	CString sub[12];

	for(i=0;i<12;i++)
	{
		sub[i]="";
		sub[i].Format(_T("%d"),m_nColumnData[i]);
	}
	
	//find the group item in general root
	CTreeItem *pGeneralRoot=GetRootItem(0);
	CTreeItem *pWatchRoot=GetRootItem(2);
	
	CTreeItem* pChild=SearchEx(pGeneralRoot,group);
	CItemInfo* lp=GetData(pChild);
	
	//update general data
	int column=0;
	for(i=0;i<12;i++)
	{
		if(gStatisticsCollector->m_bColumnStatus[i]==TRUE)
		lp->SetSubItemText(column++,sub[i]);
	}
	UpdateData(pChild, lp, TRUE); 
	

	//find the group item in watch root
	//update watch group data
	if(pWatchRoot!=NULL)
	{
		pChild=SearchEx(pWatchRoot,group);
		if(pChild!=NULL)
		{
			lp=GetData(pChild);
			column=0;
			for(i=0;i<12;i++)
			{
				if(gStatisticsCollector->m_bColumnStatus[i]==TRUE)
				lp->SetSubItemText(column++,sub[i]);
			
			}
			UpdateData(pChild, lp, TRUE); 
		}
	}
	
}

/*********************************************************

find service item and update the data
**********************************************************/
void CTreeListCtrl::GetServiceStatistics(CString service)
{
	m_nColumnData[0]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::ipProtocol,CPacketCounter::sendPacket);
	m_nColumnData[1]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::ipProtocol,CPacketCounter::receivePacket);
	m_nColumnData[2]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::ethernetProtocol,CPacketCounter::sendPacket);
	m_nColumnData[3]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::ethernetProtocol,CPacketCounter::receivePacket);
	m_nColumnData[4]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::arcnetProtocol,CPacketCounter::sendPacket);
	m_nColumnData[5]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::arcnetProtocol,CPacketCounter::receivePacket);
	m_nColumnData[6]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::mstpProtocol,CPacketCounter::sendPacket);
	m_nColumnData[7]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::mstpProtocol,CPacketCounter::receivePacket);
	m_nColumnData[8]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::ptpProtocol,CPacketCounter::sendPacket);
	m_nColumnData[9]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::ptpProtocol,CPacketCounter::receivePacket);
	m_nColumnData[10]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::allProtocol,CPacketCounter::sendPacket);
	m_nColumnData[11]=gStatisticsCollector->GetPacketCounter(service,CPacketCounter::allProtocol,CPacketCounter::receivePacket);

	CString sub[12];
	int i=0;
	for(i=0;i<12;i++)
	{
		sub[i]="";
		sub[i].Format(_T("%d"),m_nColumnData[i]);
	}
		
	CTreeItem *pGeneralRoot=GetRootItem(0);
	CTreeItem *pWatchRoot=GetRootItem(2);
	
	CTreeItem* pChild=SearchEx(pGeneralRoot,service);
	CItemInfo* lp=GetData(pChild);
	
	int column=0;
	for(i=0;i<12;i++)
	{
		if(gStatisticsCollector->m_bColumnStatus[i]==TRUE)
		lp->SetSubItemText(column++,sub[i]);
	}
	UpdateData(pChild, lp, TRUE); 
	//update general data
	
	if(pWatchRoot!=NULL)
	{
		pChild=SearchEx(pWatchRoot,service);
		if(pChild!=NULL)
		{
			lp=GetData(pChild);
			column=0;
			for(i=0;i<12;i++)
			{
				if(gStatisticsCollector->m_bColumnStatus[i]==TRUE)
				lp->SetSubItemText(column++,sub[i]);
			
			}
			UpdateData(pChild, lp, TRUE); 
		}
	}
	//update watch data
}

/*********************************************************

find general item and update the data
**********************************************************/

void CTreeListCtrl::GetGeneralStatistics()
{
	m_nColumnData[0]=gStatisticsCollector->GetTotalSend(CPacketCounter::ipProtocol);
	m_nColumnData[1]=gStatisticsCollector->GetTotalReceive(CPacketCounter::ipProtocol);
	m_nColumnData[2]=gStatisticsCollector->GetTotalSend(CPacketCounter::ethernetProtocol);
	m_nColumnData[3]=gStatisticsCollector->GetTotalReceive(CPacketCounter::ethernetProtocol);
	m_nColumnData[4]=gStatisticsCollector->GetTotalSend(CPacketCounter::arcnetProtocol);
	m_nColumnData[5]=gStatisticsCollector->GetTotalReceive(CPacketCounter::arcnetProtocol);
	m_nColumnData[6]=gStatisticsCollector->GetTotalSend(CPacketCounter::mstpProtocol);
	m_nColumnData[7]=gStatisticsCollector->GetTotalReceive(CPacketCounter::mstpProtocol);
	m_nColumnData[8]=gStatisticsCollector->GetTotalSend(CPacketCounter::ptpProtocol);
	m_nColumnData[9]=gStatisticsCollector->GetTotalReceive(CPacketCounter::ptpProtocol);
	m_nColumnData[10]=gStatisticsCollector->GetTotalSend(CPacketCounter::allProtocol);
	m_nColumnData[11]=gStatisticsCollector->GetTotalReceive(CPacketCounter::allProtocol);
	
	CString sub[12];
	int i=0;
	for(i=0;i<12;i++)
	{
		sub[i]="";
		sub[i].Format(_T("%d"),m_nColumnData[i]);
	}
	
	CTreeItem *pGeneralRoot=GetRootItem(0);
	if (pGeneralRoot!=NULL)
	{
		CItemInfo *lp = GetData(pGeneralRoot);
		if(lp!=NULL)
		{
			int column=0;
			for(i=0;i<12;i++)
			{
				if(gStatisticsCollector->m_bColumnStatus[i]==TRUE)
				lp->SetSubItemText(column++,sub[i]);
				//lp->SetSubItemText(1,sub[11]);
			}
			UpdateData(pGeneralRoot, lp, TRUE); 
		}
	}
}


/*********************************************************

update item data
**********************************************************/

void CTreeListCtrl::UpdateStatistics(CString service)
{
	int i;  // MAG 31JAN06 i was used out of scope below

	GetGeneralStatistics();
	//update general item info
	for(i=0;i<10;i++)
		GetGroupStatistics(group[i]);
	GetServiceStatistics(service);
	
	for(i=0;i<3;i++)
	{
		
		CString sub="";
		if(i==0)
		{
			sub.Format(_T("%s%d"),"<=",gStatisticsCollector->m_nDistributionArray[1]);
			GetSizeStatistics(gStatisticsCollector->m_nDistributionArray[0],gStatisticsCollector->m_nDistributionArray[1],sub);
		}
		else if(i==2)
		{
			sub.Format(_T("%s%d"),">=",gStatisticsCollector->m_nDistributionArray[4]);
			GetSizeStatistics(gStatisticsCollector->m_nDistributionArray[4],gStatisticsCollector->m_nDistributionArray[5],sub);
			
		}
		else
		{			
			sub.Format(_T("%d%s%d"),gStatisticsCollector->m_nDistributionArray[2],"-",gStatisticsCollector->m_nDistributionArray[3]);
			GetSizeStatistics(gStatisticsCollector->m_nDistributionArray[2],gStatisticsCollector->m_nDistributionArray[3],sub);
		}
	}
}


/*********************************************************

search certain item 
**********************************************************/
CTreeListCtrl::CTreeItem* CTreeListCtrl::SearchEx(CTreeItem *pStartPosition, CString strItem)
{
	CItemInfo* lp = GetData(pStartPosition);
	//if(lp->GetCheck()) another condition here maybe
	CString strData = lp->GetItemText();
	if(strData==strItem)
	{
		return pStartPosition;
	}

	const int nChildren = NumChildren(pStartPosition);
	if (nChildren > 0)
	{
		POSITION pos = GetHeadPosition(pStartPosition);
		while (pos)
		{
			CTreeItem *pChild = GetNextChild(pStartPosition, pos);
			CItemInfo* lp = GetData(pChild);
			CString strData = lp->GetItemText();
			if(strData==strItem)
			{
				return pChild;
			}
		}
	}

	POSITION pos = GetHeadPosition(pStartPosition);
	while (pos)
	{
		CTreeItem *pChild = GetNextChild(pStartPosition, pos);
		CItemInfo* lp = GetData(pChild);
		CString strData = lp->GetItemText();
		if(strData==strItem)
		{
			return pChild;
		}

		pChild = SearchEx(pChild, strItem);
		if(pChild!=NULL)
			return pChild;
	}
	return NULL;
}

/*********************************************************

find size distribution item and update the data
**********************************************************/

void CTreeListCtrl::GetSizeStatistics(int min, int max,CString name)
{

	m_nColumnData[0]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::ipProtocol,CPacketCounter::sendPacket);
	m_nColumnData[1]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::ipProtocol,CPacketCounter::receivePacket);
	m_nColumnData[2]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::ethernetProtocol,CPacketCounter::sendPacket);
	m_nColumnData[3]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::ethernetProtocol,CPacketCounter::receivePacket);
	m_nColumnData[4]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::arcnetProtocol,CPacketCounter::sendPacket);
	m_nColumnData[5]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::arcnetProtocol,CPacketCounter::receivePacket);
	m_nColumnData[6]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::mstpProtocol,CPacketCounter::sendPacket);
	m_nColumnData[7]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::mstpProtocol,CPacketCounter::receivePacket);
	m_nColumnData[8]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::ptpProtocol,CPacketCounter::sendPacket);
	m_nColumnData[9]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::ptpProtocol,CPacketCounter::receivePacket);
	m_nColumnData[10]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::allProtocol,CPacketCounter::sendPacket);
	m_nColumnData[11]=gStatisticsCollector->GetSizeCounter(min,max,CPacketCounter::allProtocol,CPacketCounter::receivePacket);
	
	CString sub[12];
	int i;
	for(i=0;i<12;i++)
	{
		sub[i]="";
		sub[i].Format(_T("%d"),m_nColumnData[i]);
	}
	
	CTreeItem *pSizeRoot=GetRootItem(1);
	
	CTreeItem* pChild=SearchEx(pSizeRoot,name);
	CItemInfo* lp=GetData(pChild);
	
	int column=0;
	for(i=0;i<12;i++)
	{
		if(gStatisticsCollector->m_bColumnStatus[i]==TRUE)
		lp->SetSubItemText(column++,sub[i]);
	}
	UpdateData(pChild, lp, TRUE); 

}
