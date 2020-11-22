// EPICSViewPropPanel.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "send.h"
#include "BACnet.hpp"
#include "ChildFrm.h"

#include "VTSDoc.h"

#include "EPICSViewNode.h"
#include "EPICSViewPropPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewPropPanel

IMPLEMENT_DYNCREATE(CEPICSViewPropPanel, CListView)

CEPICSViewPropPanel::CEPICSViewPropPanel()
{
	m_pSend = NULL;
	m_fSendReuse = true;
}

CEPICSViewPropPanel::~CEPICSViewPropPanel()
{
}


BEGIN_MESSAGE_MAP(CEPICSViewPropPanel, CListView)
	//{{AFX_MSG_MAP(CEPICSViewPropPanel)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EPICS_PROP_RP, OnReadProperty)
	ON_COMMAND(ID_EPICS_PROP_WP, OnWriteProperty)
	ON_COMMAND(ID_EPICS_PROP_RPM, OnReadPropertyMultiple)
	ON_COMMAND(ID_EPICS_PROP_WPM, OnWritePropertyMultiple)
	ON_COMMAND(ID_EPICS_PROP_GEN, OnReadAllProperty)
	ON_COMMAND(ID_EPICS_PROP_CLEAR, OnDeviceValueClear)
	ON_COMMAND(ID_EPICS_PROP_REUSE, OnCSendReuse)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewPropPanel drawing

void CEPICSViewPropPanel::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewPropPanel diagnostics

#ifdef _DEBUG
void CEPICSViewPropPanel::AssertValid() const
{
	CListView::AssertValid();
}

void CEPICSViewPropPanel::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEPICSViewPropPanel message handlers

int CEPICSViewPropPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= LVS_REPORT | LVS_SHOWSELALWAYS;
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CListCtrl& listCtrl = GetListCtrl();
	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	listCtrl.InsertColumn( 0, _T("Property Name"), LVCFMT_LEFT, 150 );
	listCtrl.InsertColumn( 1, _T("Writeable"), LVCFMT_LEFT, 60 );
	listCtrl.InsertColumn( 2, _T("EPICS Value"), LVCFMT_LEFT, 125 );
	listCtrl.InsertColumn( 3, _T("Device Value"), LVCFMT_LEFT, 125 );
	//Shiyuan Xiao 7/28/2005
	listCtrl.InsertColumn( 4, _T("Restriction"), LVCFMT_LEFT, 125 );	
	// column hiding not available yet...
	listCtrl.m_nFlags |= LVS_REPORT;
	return 0;
}


void CEPICSViewPropPanel::Reset( CEPICSViewNodeObject * pObjectNode, LPCSTR lpszColumnText )
{
	GetListCtrl().DeleteAllItems();
	m_pObjectNode = pObjectNode;

//	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();

	// Delete all of the columns.
//	for (int i=0;i < nColumnCount;i++)
//		GetListCtrl().DeleteColumn(0);

	if ( lpszColumnText != NULL )
		SetColumnText(0, lpszColumnText);
}


void CEPICSViewPropPanel::SetColumnText( int nCol, LPCSTR lpszText )
{
	HDITEM hdi;
	TCHAR  lpBuffer[256];

	hdi.mask = HDI_TEXT;
	hdi.pszText = lpBuffer;
	hdi.cchTextMax = sizeof(lpBuffer);

	strcpy(lpBuffer, lpszText);
	GetListCtrl().GetHeaderCtrl()->SetItem(nCol, &hdi);
}


void CEPICSViewPropPanel::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;

	int nCount = GetListCtrl().GetSelectedCount();

	menu.CreatePopupMenu();
	menu.AppendMenu( (nCount == 1 ? MF_ENABLED : MF_GRAYED) | MF_UNCHECKED | MF_STRING, ID_EPICS_PROP_RP, _T("Read Property"));
	menu.AppendMenu( (nCount == 1 ? MF_ENABLED : MF_GRAYED) | MF_UNCHECKED | MF_STRING, ID_EPICS_PROP_WP, _T("Write Property"));
	menu.AppendMenu( (nCount >= 1 ? MF_ENABLED : MF_GRAYED) | MF_UNCHECKED | MF_STRING, ID_EPICS_PROP_RPM, _T("Read Property Multiple"));
	menu.AppendMenu( (nCount >= 1 ? MF_ENABLED : MF_GRAYED) | MF_UNCHECKED | MF_STRING, ID_EPICS_PROP_WPM, _T("Write Property Multiple"));
	menu.AppendMenu(MF_ENABLED | MF_UNCHECKED | MF_SEPARATOR);
	menu.AppendMenu( (GetListCtrl().GetItemCount() > 0 ? MF_ENABLED : MF_GRAYED) | MF_UNCHECKED | MF_STRING, ID_EPICS_PROP_GEN, _T("Read Object Properties"));
	menu.AppendMenu( (GetListCtrl().GetItemCount() > 0 && m_pObjectNode->HasDeviceValues() ? MF_ENABLED : MF_GRAYED) | MF_UNCHECKED | MF_STRING, ID_EPICS_PROP_CLEAR, _T("Clear Device Values"));
	menu.AppendMenu(MF_ENABLED | MF_UNCHECKED | MF_SEPARATOR);
	menu.AppendMenu( (m_fSendReuse ? MF_CHECKED : MF_UNCHECKED) | MF_ENABLED | MF_STRING, ID_EPICS_PROP_REUSE, _T("Reuse Send Window"));

	menu.TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
}



void CEPICSViewPropPanel::OnReadProperty() 
{
	int nSelectedIndex;
	CSend * psend = InvokeSendWindow("ReadProperty", &nSelectedIndex);

	if ( psend != NULL )
	{
		BACnetObjectIdentifier bacnetID( (unsigned int) m_pObjectNode->GetObjectID());
		BACnetEnumerated bacnetPropID((int) m_pObjectNode->GetPropertyID(nSelectedIndex));
		psend->ReadPropPage.ForceValues(&bacnetID, &bacnetPropID);
	}
}


void CEPICSViewPropPanel::OnWriteProperty() 
{
	int nSelectedIndex;
	CSend * psend = InvokeSendWindow("WriteProperty", &nSelectedIndex);

	if ( psend != NULL )
	{
		BACnetObjectIdentifier bacnetID( (unsigned int) m_pObjectNode->GetObjectID());

		BACnetAnyValue bacnetAny;
		BACnetEnumerated bacnetPropID((int) m_pObjectNode->GetPropertyIDAndValue(nSelectedIndex, &bacnetAny));

		psend->WritePropPage.ForceValues(&bacnetID, &bacnetPropID, bacnetAny.GetObject());
	}
}


void CEPICSViewPropPanel::OnReadPropertyMultiple() 
{
	int nSelectedIndex;
	CSend * psend = InvokeSendWindow("ReadPropertyMultiple", &nSelectedIndex);

	if ( psend != NULL )
	{
		BACnetObjectIdentifier bacnetID( (unsigned int) m_pObjectNode->GetObjectID());

		// Cheat a little bit here.. create an array of ints and pretend they're BACnetEnumerated...
		int * anPropIDs = new int[GetListCtrl().GetSelectedCount()];
		for (int i = 0; i < (int) GetListCtrl().GetSelectedCount(); i++, nSelectedIndex = GetNextSelectedIndex() )
			anPropIDs[i] = (int) m_pObjectNode->GetPropertyID(nSelectedIndex);

		psend->ReadPropMultPage.ForceValues(&bacnetID, anPropIDs, GetListCtrl().GetSelectedCount() );
		delete anPropIDs;
	}
}


void CEPICSViewPropPanel::OnWritePropertyMultiple() 
{
	int nSelectedIndex;
	CSend * psend = InvokeSendWindow("WritePropertyMultiple", &nSelectedIndex);

	if ( psend != NULL )
	{
		int nCount = GetListCtrl().GetSelectedCount();
		BACnetObjectIdentifier bacnetID( (unsigned int) m_pObjectNode->GetObjectID());

		// Cheat a little bit here.. create an array of ints and pretend they're BACnetEnumerated...
		int * anPropIDs = new int[nCount];
		CPtrArray apbacnetValues;
		apbacnetValues.SetSize(nCount);
		BACnetAnyValue bacnetAny;

		for (int i = 0; i < nCount; i++, nSelectedIndex = GetNextSelectedIndex() )
		{
			anPropIDs[i] = (int) m_pObjectNode->GetPropertyIDAndValue(nSelectedIndex, &bacnetAny);
			apbacnetValues[i] = bacnetAny.GetObject();

			// Have to set to null to avoid ->SetObject destroying previous BACnetEncodeable value
			// We're storing these in arrays so we'll delete them ourselves
			bacnetAny.pbacnetTypedValue = NULL;
		}

		psend->WritePropMultPage.ForceValues(&bacnetID, anPropIDs, &apbacnetValues, nCount );

		for (int j = 0; j < nCount; j++ )
			delete (BACnetEncodeable *) apbacnetValues[j];

		delete anPropIDs;
	}
}


void CEPICSViewPropPanel::OnReadAllProperty() 
{
	CChildFrame * p = (CChildFrame *) GetParentFrame();
	p->DoReadSingleProperties("ReadObjectProperties.vts", m_pObjectNode->GetObjectID());
}


void CEPICSViewPropPanel::OnDeviceValueClear() 
{
	m_pObjectNode->ClearDeviceValues();
}


void CEPICSViewPropPanel::OnCSendReuse() 
{
	m_fSendReuse = !m_fSendReuse;
}


int CEPICSViewPropPanel::GetFirstSelectedIndex()
{
	m_curpos = GetListCtrl().GetFirstSelectedItemPosition();
	return  m_curpos != NULL ? GetListCtrl().GetNextSelectedItem(m_curpos) : -1;
}


int CEPICSViewPropPanel::GetNextSelectedIndex()
{
	return GetListCtrl().GetNextSelectedItem(m_curpos);
}



void CEPICSViewPropPanel::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if ( GetListCtrl().GetSelectedCount() > 1 )
		OnReadPropertyMultiple();
	else if ( GetListCtrl().GetSelectedCount() == 1 )
		OnReadProperty();

	*pResult = 0;
}




CSend * CEPICSViewPropPanel::InvokeSendWindow( LPCSTR lpszAction, int * pSelectedIndex )
{
	*pSelectedIndex = GetFirstSelectedIndex();
	if ( *pSelectedIndex < 0 )
		return NULL;

	if ( !IsPortSelected() )
		return NULL;

	int nSelectedGroup, nSelectedItem;

	CSend::FindActionIndicies(lpszAction, &nSelectedGroup, &nSelectedItem);

	if ( nSelectedGroup == -1 || nSelectedItem == -1 )
	{
		TRACE0("Unable to locate proper Object Access group");
		ASSERT(0);
	}

	// Attempt to reuse the send window
	// This doesn't work because the CSend window needs massaging to invoke the proper tab pages...

//	if ( m_pSend == NULL || !AfxIsMemoryBlock(m_pSend, sizeof(CSend)) ||  !::IsWindow(m_pSend->m_hWnd)  ||  !m_fSendReuse )
	if ( m_pSend == NULL || !::IsWindow(m_hwndSend)  ||  !m_fSendReuse )
	{
		VTSDoc * pdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
		m_pSend = pdoc->DoSendWindow( nSelectedGroup, nSelectedItem );
		m_hwndSend = m_pSend->m_hWnd;
	}
	else
	{
		m_pSend->BringWindowToTop();
		m_pSend->ChangePacketTree(nSelectedGroup, nSelectedItem);
	}

	return m_pSend;
}


// This global is set in the menus...  It's needed for the send window stuff
extern int gSelectedPort;


BOOL CEPICSViewPropPanel::IsPortSelected()
{
	VTSDoc * pdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
	VTSPorts * pports = pdoc == NULL ? NULL : pdoc->GetPorts();

	// Make sure the globally selected port is golden

	if ( pports == NULL || pports->GetSize() == 0 )
	{
		AfxMessageBox("At least one VTS port must be defined to issue this Object Access request.", MB_OK );
		return FALSE;
	}

	if ( gSelectedPort < 0 )
		gSelectedPort = 0;
	if ( gSelectedPort >= pports->GetSize() )
		gSelectedPort = pports->GetSize()-1;

	// Now check to see if the selected port is enabled...
	if ( !(*pports)[gSelectedPort]->IsEnabled() )
	{
		CString strError;
		strError.Format("Port '%s:%s' is selected but disabled.  You must enable the port to issue this Object Access request.", (*pports)[gSelectedPort]->GetName(), (*pports)[gSelectedPort]->GetTypeDesc() );
		AfxMessageBox(strError, MB_OK);
		return FALSE;
	}

	return TRUE;
}


