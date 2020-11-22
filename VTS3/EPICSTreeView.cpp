// EPICSTreeView.cpp : implementation file
//

#include "stdafx.h"
#include <afxrich.h>
#include <process.h>

#include "stdafx.h"
#include "VTS.h"
#include "ChildFrm.h"


#include "DockingEPICSViewBar.h"
#include "EPICSTreeView.h"
#include "EPICSViewInfoPanel.h"
#include "EPICSViewNode.h"


///////////////////////////////
namespace PICS {
#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"
}

extern PICS::PICSdb * gPICSdb;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// 
// madanner 9/04 add setup for EPICS notification
// global hack avoids messing with bacfuncs too much
CEPICSTreeView * g_pEPICSTreeView = NULL;

// hack globals to build a list of obj/prop/values received from RP and RPM
void EPICS_AddRPValue(long obj_id, int prop_id, int prop_index, char * pbuffer, int nLen )
{
   // TODO: someday, nice to use the index to allow array properties
   if ( (g_pEPICSTreeView != NULL) && (prop_index < 0) )
   {
      g_pEPICSTreeView->AddRPValue(obj_id, prop_id, pbuffer, nLen );
   }
}

/////////////////////////////////////////////////////////////////////////////
// CEPICSTreeView

IMPLEMENT_DYNCREATE(CEPICSTreeView, CFormView)

CEPICSTreeView::CEPICSTreeView()
: CFormView(CEPICSTreeView::IDD)
, m_summary(false, true)
{
   m_pwndSplit = NULL;
   m_pnodeview = NULL;

   //{{AFX_DATA_INIT(CEPICSTreeView)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

CEPICSTreeView::~CEPICSTreeView()
{
}

void CEPICSTreeView::DoDataExchange(CDataExchange* pDX)
{
   CFormView::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CEPICSTreeView)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

void CEPICSTreeView::SetTreeImageList()
{
   // As of 30 May 2011, epicstree.bmp consists of
   // - 10 images for top-level items, of which 8 are currently used, followed by
   // - an image for each defined object type
   // If you use both of the spare top-level images, you must insert space, and
   // re-define this equate for the index of the first object type's image
   //
   // See test code in Refresh() to view all available images
   //
#define FIRST_OBJECT_IMAGE 10
#define PROPRIETARY_OBJECT_IMAGE 7
   m_imagelist.Create( IDB_EPICSTREE, 18, 2, RGB(255,0,255) );

   CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);
   ptree->SetImageList( &m_imagelist, TVSIL_NORMAL );
}

BEGIN_MESSAGE_MAP(CEPICSTreeView, CFormView)
   //{{AFX_MSG_MAP(CEPICSTreeView)
   ON_WM_SIZE()
   ON_BN_CLICKED(IDC_EPICSV_RESET, OnEpicsReset)
   ON_BN_CLICKED(IDC_EPICSV_LOAD, OnEpicsLoad)
   ON_NOTIFY(TVN_SELCHANGED, IDC_EPICSV_TREE, OnSelchangedEpicsTree)
   ON_BN_CLICKED(IDC_EPICSV_EDIT, OnEpicsEdit)
   ON_WM_DESTROY()
   ON_BN_CLICKED(IDC_EPICSV_GEN, OnEpicsReadSingleProps)
   ON_BN_CLICKED(IDC_EPICSV_GENALL, OnEpicsReadAllProps)
   ON_WM_CREATE()
   ON_CBN_DROPDOWN(IDC_EPICSV_DA, OnDropdownEpicsDA)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEPICSTreeView diagnostics

#ifdef _DEBUG
void CEPICSTreeView::AssertValid() const
{
   CFormView::AssertValid();
}

void CEPICSTreeView::Dump(CDumpContext& dc) const
{
   CFormView::Dump(dc);
}

VTSDoc* CEPICSTreeView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(VTSDoc)));
   return (VTSDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CEPICSTreeView message handlers

/////////////////////////////////////////////////////////////////////////////
// CEPICSView drawing

void CEPICSTreeView::OnSize(UINT nType, int cx, int cy) 
{
   // Calculate minimum sizes
   cx = cx < 120 ? 120 : cx;
   cy = cy < 130 ? 130 : cy;

   SetScrollSizes(MM_TEXT, CSize(cx,cy));

   // Resize the tree control
   ResizeControl(GetDlgItem(IDC_EPICSV_TREE), cx, cy);
   ResizeControl(GetDlgItem(IDC_EPICSV_FILENAME), cx, -1);

   CFormView::OnSize(nType, cx, cy);
}

int CEPICSTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CFormView::OnCreate(lpCreateStruct) == -1)
      return -1;

   SetContext( gNewFrameContext );
   return 0;
}

// Called when a new packet is received...
// See if we should pay attention to it.
void CEPICSTreeView::ContextChange( CFrameContext::Signal s )
{
   // If this notification isn't for a newly arrived packet, or we haven't loaded EPICS or
   // there are no packet... get out.

   if (s != CFrameContext::eNewPacketCount ||  gPICSdb == NULL  ||  m_FrameContext->m_PacketCount <= 0)
// if (s != CFrameContext::eNewCurrentPacket ||  gPICSdb == NULL  ||  m_FrameContext->m_PacketCount <= 0)
      return;
   
   // Get last added packet and get out if it's not a received packet
   VTSPacketPtr ppkt = m_FrameContext->m_pDoc->GetPacket(m_FrameContext->m_PacketCount-1);
// VTSPacketPtr ppkt = m_FrameContext->m_pDoc->GetPacket(m_FrameContext->m_CurrentPacket);
   if (ppkt->packetHdr.packetType != rxData)
      return;

   // Who's it from?
   const char *name = m_FrameContext->m_pDoc->AddrToName( ppkt->packetHdr.packetSource, ppkt->packetHdr.m_szPortName );
   if (name == NULL)
      return;

   // Is that our DA?
   CComboBox *pcomboDA = (CComboBox*)GetDlgItem(IDC_EPICSV_DA);
   CString strDA;
   pcomboDA->GetWindowText(strDA);
   if (strDA.Compare(name) != 0)
      return;

   // Clear out read arrays and prep for the sniffer to stuff the values
   PrepRPCallback(this);

   NetworkSniffer::SetLookupContext( ppkt->packetHdr.m_szPortName );
   m_summary.Interpret( (BACnetPIInfo::ProtocolType) ppkt->packetHdr.packetProtocolID, (char*) ppkt->packetData, ppkt->packetLen );

   PrepRPCallback(NULL);
}

void CEPICSTreeView::PrepRPCallback( CEPICSTreeView * pthis )
{
   g_pEPICSTreeView = pthis;        // setup global so sniffer populates these lists properly
// m_aRPCallback_ObjID.RemoveAll();
// m_aRPCallback_PropID.RemoveAll();

// for(int i = 0; i < m_aRPCallback_ValuePtr.GetSize(); i++ )
//    delete (BACnetEncodeable *) m_aRPCallback_ValuePtr[i];

// m_aRPCallback_ValuePtr.RemoveAll();
}

void CEPICSTreeView::ResizeControl(CWnd * pwnd, int cx, int cy)
{
   RECT rc;

   if ( pwnd != NULL )
   {
      pwnd->GetWindowRect(&rc);
      ScreenToClient(&rc);
      pwnd->SetWindowPos(NULL, 0, 0, cx - 3 - rc.left, cy == -1 ? rc.bottom - rc.top : cy - 3 - rc.top, SWP_NOMOVE | SWP_NOZORDER);
   }
}

void CEPICSTreeView::OnEpicsReset() 
{
   CString str;
   GetDlgItem(IDC_EPICSV_FILENAME)->GetWindowText(str);

   if ( str.GetLength() != 0 )
      EPICSLoad(str);
}

// Open a file dialog and get an EPICS file
bool CEPICSTreeView::GetEpicsFilename( CString &theFilePath ) 
{
   // Start with the current EPICS (if any)
   // Will override the default directory set below.
   CString lastEpics = gVTSPreferences.Setting_GetLastEPICS();

   // Don't change the current directory: EPICS file shouldn't affect location of anything else
   CFileDialog fd( TRUE, "tpi", lastEpics, 
                   OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, 
                   "EPICS (*.tpi)|*.tpi||" );

   // Default EPICS directory: there is a sample EPICS here
   CString dir;
   ((VTSApp*)AfxGetApp())->GetRelativeToExe( dir, "Docs" );
   fd.m_ofn.lpstrInitialDir = dir;
   
   bool retval = (fd.DoModal() == IDOK);
   if (retval)
   {
      theFilePath = fd.GetPathName();
   }

   return retval;
}

void CEPICSTreeView::OnEpicsLoad()
{
   CString filePath;
   if (GetEpicsFilename( filePath ))
   {
      EPICSLoad(filePath);
   }
}

void CEPICSTreeView::OnEpicsEdit()
{
   CString str;
   GetDlgItem(IDC_EPICSV_FILENAME)->GetWindowText(str);

   if (!str.IsEmpty())
   {
      const char *pEditor = gVTSPreferences.Setting_GetTextEditor();
      str.Insert(0, '"' );
      str += '"';
      _spawnlp(_P_NOWAIT, pEditor, pEditor, (LPCTSTR)str, NULL);
   }
}

void CEPICSTreeView::OnEpicsReadSingleProps()
{
   CChildFrame * p = (CChildFrame *) GetParentFrame();

   // Get selected Object node
   CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);
   HTREEITEM htreeitem = ptree->GetSelectedItem();
   CEPICSViewNodeObject * pnode = htreeitem != NULL ? (CEPICSViewNodeObject *) ptree->GetItemData(htreeitem) : NULL;

   if ( pnode != NULL && pnode->IsKindOf(RUNTIME_CLASS(CEPICSViewNodeObject)) )
      p->DoReadSingleProperties("ReadObjectProperties.vts", pnode->GetObjectID());
}

void CEPICSTreeView::OnEpicsReadAllProps()
{
   CChildFrame * p = (CChildFrame *) GetParentFrame();
   p->DoReadAllProperties();
}

void CEPICSTreeView::SetSplitter( CDockingEPICSViewBar * pbar, CSplitterWnd * pwndSplit, CCreateContext * pContext )
{
   m_pwndSplit = pwndSplit;
   m_pContext = pContext;
   m_pbar = pbar;
   CreateInfoView(RUNTIME_CLASS(CEPICSViewInfoPanel), FALSE);
   SetTreeImageList();
   Refresh();
}

CView * CEPICSTreeView::CreateInfoView( CRuntimeClass * pviewclass, BOOL fDeleteOldView )
{
   if ( m_pwndSplit == NULL )
      return NULL;

   int nRow = 1;
   int nCol = 0;

   if ( m_pwndSplit->GetColumnCount() == 2 )
   {
      nRow = 0;
      nCol = 1;
   }

   if ( fDeleteOldView )
      m_pwndSplit->DeleteView(nRow, nCol);

   if ( !m_pwndSplit->CreateView(nRow, nCol, pviewclass, CSize(0, 0), m_pContext) )
   {
      TRACE0("Failed to create edit pane on EPICS view\n");
      return NULL;
   }

   m_pnodeview = (CView *) m_pwndSplit->GetPane(nRow, nCol);
   m_pwndSplit->RecalcLayout();
   return m_pnodeview;
}

void CEPICSTreeView::DeleteDB()
{
   if ( gPICSdb )
   {
      PICS::MyDeletePICSObject( gPICSdb->Database );
      delete gPICSdb;
      gPICSdb = NULL;
//    m_pbar->m_nSyntaxE = 0;
//    m_pbar->m_nConE = 0;
   }
}

// Return TRUE if load was successful
BOOL CEPICSTreeView::EPICSLoad( LPCSTR lpszFileName )
{
   GetDlgItem(IDC_EPICSV_FILENAME)->SetWindowText( lpszFileName == NULL ? "" : lpszFileName );

   if ( lpszFileName == NULL || lstrlen(lpszFileName) == 0 )
   {
      Refresh();
      return TRUE;
   }

   DeleteDB();

   // make a new database
   gPICSdb = new PICS::PICSdb;

   m_pbar->m_nSyntaxE = 0;
   m_pbar->m_nConE = 0;

   // read in the EPICS
   // ReadTextPICS returns false if canceled by user

   if ( PICS::ReadTextPICS( (char *) (LPCSTR) lpszFileName, gPICSdb, &m_pbar->m_nSyntaxE, &m_pbar->m_nConE ) )
   {
      // Formerly saved the name only if read without error.
      // But then you can't use the Edit button to FIX the errors.
      gVTSPreferences.Setting_SetLastEPICS(lpszFileName);

      if ( m_pbar->m_nSyntaxE != 0 )
         DeleteDB();
   }
   else
   {
      // Cancelled by user... clear DB, reset last load, etc.
      gVTSPreferences.Setting_SetLastEPICS("");
      DeleteDB();
      GetDlgItem(IDC_EPICSV_FILENAME)->SetWindowText("");
   }

   Refresh();
   return gPICSdb == 0 ? FALSE : TRUE;
}

// Reload all of the controls and such for new EPICS database
void CEPICSTreeView::Refresh()
{
   CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);
   WipeOut(ptree, ptree->GetRootItem());
   ptree->DeleteAllItems();

   GetDlgItem(IDC_EPICSV_GEN)->EnableWindow(FALSE);
   GetDlgItem(IDC_EPICSV_GENALL)->EnableWindow(gPICSdb != NULL);

   // Build tree nodes...  
   HTREEITEM htreeitemLast = NULL;

   // Main node title... EPICS name
   LPCSTR lpsz = "No EPICS Data";
   if ( gPICSdb == NULL )
   {
      CString str;
      GetDlgItem(IDC_EPICSV_FILENAME)->GetWindowText(str);
      if ( str.GetLength() != 0 )
         lpsz = "EPICS Error";
   }
   else
   {
      lpsz = gPICSdb->ProductName;
      GetDlgItem(IDC_EPICSV_FILENAME)->SetWindowText(gVTSPreferences.Setting_GetLastEPICS());
   }

   HTREEITEM htreeitemRoot = ptree->InsertItem(TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE, lpsz, 0, 0, 0, 0, (LPARAM) new CEPICSViewNodeRoot(this, m_pbar->m_nSyntaxE, m_pbar->m_nConE), NULL, htreeitemLast );

   // Create BIBB node
   htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("BIBBs"), 1, 1, 0, 0, (LPARAM) new CEPICSViewNodeBIBB(this), htreeitemRoot, NULL );

   // Create Application Services
   htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Application Services"), 2, 2, 0, 0, (LPARAM) new CEPICSViewNodeAppService(this), htreeitemRoot, htreeitemLast );

   // Create Object Types
   htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Object List"), 3, 3, 0, 0, (LPARAM) new CEPICSViewNodeObjTypes(this), htreeitemRoot, htreeitemLast );
   m_htreeitemObjectParent = htreeitemLast;

   if ( gPICSdb != NULL )
      LoadObjectNodes(htreeitemLast);

   // Create Data Link Options
   htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Data Link Layer Options"), 4, 4, 0, 0, (LPARAM) new CEPICSViewNodeDataLink(this), htreeitemRoot, htreeitemLast );

   // Create Character Sets
   htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Character Sets Supported"), 5, 5, 0, 0, (LPARAM) new CEPICSViewNodeCharsets(this), htreeitemRoot, htreeitemLast );

   // Create Special Functionality
   htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Special Functionality"), 6, 6, 0, 0, (LPARAM) new CEPICSViewNodeSpecialFunctionality(this), htreeitemRoot, htreeitemLast );

#if 0
   // Test code: Show ALL icons in the list, to verify image/type matches when
   // new object types are added
   for (int ix=0; ix < 130; ix++)
   {
      CString str;
      if (ix >= FIRST_OBJECT_IMAGE)
      {
         str.Format( "%d) type=%d %s", 
                  ix, ix - FIRST_OBJECT_IMAGE, 
                  (LPCTSTR)NetworkSniffer::BAC_STRTAB_BACnetObjectType.EnumString( ix - FIRST_OBJECT_IMAGE ) );
      }
      else
      {
         str.Format( "%d ", ix );
      }

      if (ix >= m_imagelist.GetImageCount())
      {
         str += " no image";
      }
      htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, str, ix, ix, 0, 0, (LPARAM) new CEPICSViewNodeDataLink(this), htreeitemRoot, htreeitemLast );
   }
#endif

   // select the first item in the tree
   ptree->SelectItem(ptree->GetFirstVisibleItem());
}

// Find the object nodes in the tree and attempt to set the object/prop/value...
void CEPICSTreeView::AddRPValue( long obj_id, int prop_id, char * pbuffer, int nLen )
{
// g_aRPCallback_ObjID.Add((DWORD) obj_id);
// g_aRPCallback_PropID.Add( (UINT) prop_id);
// g_aRPCallback_ValuePtr.Add( BACnetEncodeable::Factory(datatype, &decoder) );

   CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);

   if ( !ptree->ItemHasChildren(m_htreeitemObjectParent) )
      return;

   // Could be any number of things that go wrong here... don't blow the sniffer decoding
   // just because we can't get our value stuffed into our list view

   try
   {
      for ( HTREEITEM htreeitemLast = ptree->GetChildItem(m_htreeitemObjectParent); htreeitemLast != NULL; htreeitemLast = ptree->GetNextSiblingItem(htreeitemLast) )
      {
         CEPICSViewNodeObject * pnode = (CEPICSViewNodeObject *) ptree->GetItemData(htreeitemLast);
         if ( pnode->IsKindOf(RUNTIME_CLASS(CEPICSViewNodeObject))  &&  pnode->GetObjectID() == (DWORD) obj_id )
         {
            unsigned short nParseType;
            int x = pnode->GetPropertyIndex((DWORD) prop_id, &nParseType);

            if ( x != -1 )
               pnode->SetDeviceValue(x, BACnetAnyValue(BACnetAnyValue::Factory(nParseType, BACnetAPDUDecoder((const BACnetOctet *) pbuffer, nLen), prop_id)).ToString());
            break;
         }
      }
   }
   catch(...){}
}

void CEPICSTreeView::LoadObjectNodes( HTREEITEM htreeitemParent )
{
   HTREEITEM htreeitemLast = NULL;
   PICS::generic_object * pObj = gPICSdb->Database;

   CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);

   while ( pObj != NULL )
   {
      if(pObj->object_type < 128)
      {
         // Standard object
         htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T(pObj->object_name), FIRST_OBJECT_IMAGE+(pObj->object_type), FIRST_OBJECT_IMAGE+(pObj->object_type), 0, 0, (LPARAM) new CEPICSViewNodeObject(this, pObj), htreeitemParent, htreeitemLast );
      }
      else
      {
         // Proprietary object
         htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T(pObj->object_name), PROPRIETARY_OBJECT_IMAGE, PROPRIETARY_OBJECT_IMAGE, 0, 0, (LPARAM) new CEPICSViewNodeObject(this, pObj), htreeitemParent, htreeitemLast );
      }

      pObj = (PICS::generic_object *) pObj->next;
   }
}

void CEPICSTreeView::LoadDACombo()
{
   CComboBox *pcombo = (CComboBox*)GetDlgItem(IDC_EPICSV_DA);

   // Get the current Destination setting so that we can restore it later
   CString strText;
   pcombo->GetWindowText(strText);

   // Fill the Destination combo
   pcombo->ResetContent();
   VTSDoc *pdoc = (VTSDoc*) ((VTSApp*) AfxGetApp())->GetWorkspace();
   VTSNames *pnames = (pdoc == NULL) ? NULL : pdoc->GetNames();

   for (int i = 0; pnames != NULL && i < pnames->GetSize(); i++)
   {
      pcombo->AddString((*pnames)[i]->GetName());
   }

   if (strText.IsEmpty())
   {
      // Default to the unit under test
      strText = "IUT";
   }

   // If SelectString fails, selection will remain at 0
   pcombo->SetCurSel(0);
   pcombo->SelectString(0, strText);
}

void CEPICSTreeView::WipeOut( CTreeCtrl *ptree, HTREEITEM htreeitem )
{
   while (htreeitem != NULL )
   {
      if ( ptree->GetItemData(htreeitem) != NULL )
         delete (CEPICSViewNode *) ptree->GetItemData(htreeitem);
      WipeOut(ptree, ptree->GetNextItem(htreeitem, TVGN_CHILD));
      htreeitem = ptree->GetNextItem(htreeitem, TVGN_NEXT);
   }
}

CView * CEPICSTreeView::GetInfoPanel()
{
   return m_pnodeview;
}

void CEPICSTreeView::OnInitialUpdate() 
{
   CFormView::OnInitialUpdate();

   GetDlgItem(IDC_EPICSV_GEN)->EnableWindow(FALSE);
   GetDlgItem(IDC_EPICSV_GENALL)->EnableWindow(FALSE);
   LoadDACombo();
}

void CEPICSTreeView::OnSelchangedEpicsTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

   CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);

   HTREEITEM htreeitem = pNMTreeView == NULL ? NULL : ptree->GetSelectedItem();
   CEPICSViewNode * pnode = htreeitem != NULL ? (CEPICSViewNode *) ptree->GetItemData(htreeitem) : NULL;

   GetDlgItem(IDC_EPICSV_GEN)->EnableWindow(pnode->IsKindOf(RUNTIME_CLASS(CEPICSViewNodeObject)));

   if ( pnode != NULL )
      pnode->LoadInfoPanel();

   *pResult = 0;
}

void CEPICSTreeView::OnDestroy() 
{
   CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);

   // Kill the frame context listner
   RemoveContext();

   WipeOut(ptree, ptree->GetRootItem());
   ptree->DeleteAllItems();

   CFormView::OnDestroy();
}

void CEPICSTreeView::OnDropdownEpicsDA() 
{
   LoadDACombo();
}
