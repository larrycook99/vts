// DetailTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "DetailTreeCtrl.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDetailTreeCtrl

CDetailTreeCtrl::CDetailTreeCtrl()
{
}

CDetailTreeCtrl::~CDetailTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CDetailTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CDetailTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetailTreeCtrl message handlers
void CDetailTreeCtrl::ContextChange( CFrameContext::Signal s )
{
	switch (s) {
		case CFrameContext::eNewCurrentPacket:
			m_FrameContext->SetCurrentDetail( -1 );
			if (m_FrameContext->m_CurrentPacket == -1)
			{
				m_FrameContext->SetDetailCount( 0 );
				SetRedraw(FALSE);
				DeleteAllItems();
				SetRedraw(TRUE);
			}
			else
			{
				m_FrameContext->SetDetailCount( m_FrameContext->m_PacketInfo.detailCount );
				ShowDetail();
			}
			break;
			
		case CFrameContext::eNewDetailCount:
			break;
			
		case CFrameContext::eNewCurrentDetail:
			break;
	}
}

// Structure to control nesting of elements
struct TreeNester
{
	HTREEITEM	m_hItem;
	int			m_offset;
	int			m_endOffset;
	bool		m_autoExpand;

	TreeNester();
	void Set( HTREEITEM theItem, BACnetPIDetail *pTheDetail, bool autoExpand );
};

TreeNester::TreeNester()
: m_hItem(0)
, m_offset(0)
, m_endOffset(0)
, m_autoExpand(false)
{
}

void TreeNester::Set( HTREEITEM theItem, BACnetPIDetail *pTheDetail, bool autoExpand )
{
	m_hItem = theItem;
	m_offset = pTheDetail->piOffset; 
	m_endOffset = pTheDetail->piOffset + pTheDetail->piLen;
	m_autoExpand = autoExpand;
}

#define MAX_NEST 30

void CDetailTreeCtrl::ShowDetail()
{
	SetRedraw(FALSE);
	DeleteAllItems();

	TVINSERTSTRUCT tvInsert;

	int sp = 0;
	TreeNester nesting[ MAX_NEST ];
	TreeNester last;
	
	HTREEITEM hItem;
	HTREEITEM hDLItem = NULL;
	HTREEITEM hNLItem = NULL;
	HTREEITEM hALItem = NULL;
	bool sawHeader = false;

	for (int i=0; i<m_FrameContext->m_PacketInfo.detailCount; i++)
	{
		if ((m_FrameContext->m_PacketInfo.detailLine == NULL) ||
			(m_FrameContext->m_PacketInfo.detailLine[i] == NULL))
		{
			throw( "NULL detail in CDetailTreeCtrl" );
		}
		
		CString temp(m_FrameContext->m_PacketInfo.detailLine[i]->piLine);
	
		tvInsert.hParent      = NULL;
		tvInsert.hInsertAfter = NULL;
		tvInsert.item.mask    = TVIF_TEXT;
		tvInsert.item.pszText = temp.GetBuffer(0);

		// TODO: there must be a better way to do this than
		// string compares.  At very least, make the strings into
		// symbols exported by their place of origin, so that change
		// to them don't break the view.
		// Possibly nodeType NT_ROOT, or some new value like NT_DATALINK?
		if((temp=="----- IP Frame Detail -----") ||
		   (temp=="----- Ethernet Frame Detail -----") ||
		   (temp=="----- ARCNET Frame Detail -----") ||
		   (temp=="----- BACnet MS/TP Frame Detail -----") ||
		   (temp=="----- BACnet PTP Frame Detail -----"))
		{
			// Datalink header
			hDLItem = InsertItem(&tvInsert);
			SetItemData(hDLItem,i);

			sp = 0;
			last.Set( hDLItem, m_FrameContext->m_PacketInfo.detailLine[i], true );
			nesting[0] = last;
			sawHeader = true;
			continue;
		}

		if (temp=="----- BACnet Virtual Link Layer Detail -----")
		{
			hItem = InsertItem(&tvInsert);
			SetItemData(hItem,i);

			sp = 0;
			last.Set( hItem, m_FrameContext->m_PacketInfo.detailLine[i], true );
			nesting[0] = last;
			sawHeader = true;
			continue;
		}
		
		if (temp=="----- BACnet Network Layer Detail -----")
		{
			hNLItem = InsertItem(&tvInsert);
			SetItemData(hNLItem,i);
			
			sp = 0;
			last.Set( hNLItem, m_FrameContext->m_PacketInfo.detailLine[i], true );
			nesting[0] = last;
			sawHeader = true;
			continue;
		}
				
		if (temp=="----- BACnet Application Layer Detail -----")
		{
			hALItem = InsertItem(&tvInsert);
			SetItemData(hALItem,i);

			sp = 0;
			last.Set( hALItem, m_FrameContext->m_PacketInfo.detailLine[i], true );
			nesting[0] = last;
			sawHeader = true;
			continue;
		}

		if (sawHeader)
		{
			// Everything under a header nests

			// If this is an opening tag, expand the children
			bool expandMe  = m_FrameContext->m_PacketInfo.detailLine[i]->piNodeType == NT_OPEN_TAG;
			int offset = m_FrameContext->m_PacketInfo.detailLine[i]->piOffset;
			int length = m_FrameContext->m_PacketInfo.detailLine[i]->piLen;
			int end    = offset + length;
			if (length == 0)
			{
				// Information-only line
				// Insert under last header
				tvInsert.hParent = nesting[sp].m_hItem;
				hItem = InsertItem(&tvInsert);
				SetItemData( hItem, i );

				// Ignore the "position" and zero length of the item:
				// mucks up subsequent nesting.
				//
				// Ideally, change generators not to SPECIFY length of zero:
				// pretty much everything IS tied to the PDU.
				//
				// Failing that, bring forward the position of the previous item.
				// Never expand this item - it can't be a header
				last.m_hItem = hItem;
				last.m_autoExpand = false;
			}
			else if (offset == last.m_offset)
			{
				if ((end < last.m_endOffset) ||
				    (nesting[sp].m_offset != offset) ||
				    (nesting[sp].m_endOffset != end))
				{
					// Sub-part of previous item, or first amplification line 
					// Insert as child of previous item
					if (sp < MAX_NEST-1)
					{
						sp += 1;
					}
					nesting[sp] = last;
					
					tvInsert.hParent = nesting[sp].m_hItem;
					hItem = InsertItem(&tvInsert);
					SetItemData( hItem, i );
					last.Set( hItem, m_FrameContext->m_PacketInfo.detailLine[i], expandMe );
				}
				else
				{
					// Same range as previous item
					// Insert under last header
					tvInsert.hParent = nesting[sp].m_hItem;
					hItem = InsertItem(&tvInsert);
					SetItemData( hItem, i );
					last.Set( hItem, m_FrameContext->m_PacketInfo.detailLine[i], expandMe );
				}
			}
			else
			{
				// New starting location.  Pop stack to find an enclosing parent.
				while ((offset >= nesting[sp].m_endOffset) && (sp > 0))
				{
					// TODO: as we unwind the stack, we could implement a variable
					// auto-expand: expand the first N levels.
					// Probably a different value of N for DL, BVLL, NL, AL
					// See repeat of this code after tne end of the line loop
					if (nesting[sp].m_autoExpand && (sp <= 5))
					{
						Expand( nesting[sp].m_hItem, TVE_EXPAND );
					}
					
					sp -= 1;
				}
				
				tvInsert.hParent = nesting[sp].m_hItem;
				hItem = InsertItem(&tvInsert);
				SetItemData( hItem, i );
				last.Set( hItem, m_FrameContext->m_PacketInfo.detailLine[i], expandMe );
			}
		}
		else
		{
			// Item without a header.  Just show lines
			hItem = InsertItem(&tvInsert);
			SetItemData( hItem, i );
		}
		
		if (m_FrameContext->m_PacketInfo.detailLine[i]->piNodeType == NT_ERROR)
		{
			// Error line: expand all parents, to be sure the line is visible
			for (int ix = sp; ix >= 0; ix--)
			{
				Expand( nesting[ix].m_hItem, TVE_EXPAND );
			}
		}
	}

	// If the stack isn't empty, do expansions
	while (sp > 0)
	{
		if (nesting[sp].m_autoExpand && (sp <= 5))
		{
			Expand( nesting[sp].m_hItem, TVE_EXPAND );
		}
					
		sp -= 1;
	}
	
	// TODO: might want a user option to auto-expand datalink
	if (hDLItem)
		Expand(hDLItem, TVE_EXPAND);

	// TODO: might want a user option to auto-expand network
//	if (hNLItem)
//		Expand(hNLItem, TVE_EXPAND);
	
	if (hALItem)
		Expand(hALItem,TVE_EXPAND);

	SetRedraw(TRUE);
}

void CDetailTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	HTREEITEM sel=GetSelectedItem();
	
	//notify hex view to show selected field		
	m_FrameContext->SetCurrentDetail(GetItemData(sel));

	*pResult = 0;
}

