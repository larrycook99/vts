// ScriptContentTree.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "ScriptContentTree.h"
#include "ScriptDocument.h"
// Added by Yajun Zhou, 2002-6-20
#include "ScriptFrame.h"

#include "ScriptEditIncludeDlg.h"

//////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptContentTree

IMPLEMENT_DYNCREATE(ScriptContentTree, CTreeView)

ScriptContentTree::ScriptContentTree()
   : m_pDoc(0)
   , m_pScriptContent(0)
{
   m_pframe = NULL;
}

ScriptContentTree::~ScriptContentTree()
{
   delete m_pScriptContent;
}

BEGIN_MESSAGE_MAP(ScriptContentTree, CTreeView)
   //{{AFX_MSG_MAP(ScriptContentTree)
   ON_WM_CREATE()
   ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
   ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
   ON_WM_LBUTTONDBLCLK()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptContentTree drawing

void ScriptContentTree::OnDraw(CDC* pDC)
{
   CDocument* pDoc = GetDocument();
   // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// ScriptContentTree diagnostics

#ifdef _DEBUG
void ScriptContentTree::AssertValid() const
{
   CTreeView::AssertValid();
}

void ScriptContentTree::Dump(CDumpContext& dc) const
{
   CTreeView::Dump(dc);
}
#endif //_DEBUG

//
// ScriptContentTree::PreCreateWindow
//
BOOL ScriptContentTree::PreCreateWindow(CREATESTRUCT& cs)
{
   if (!CTreeView::PreCreateWindow (cs))
      return FALSE;

   cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

   return TRUE;
}

//
// ScriptContentTree::OnCreate
//
int ScriptContentTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CTreeView::OnCreate(lpCreateStruct) == -1)
      return -1;

   // get a copy of the control
   m_pTreeCtrl = &GetTreeCtrl();

   // initialize the status image list
   m_ilStatus.Create( IDB_CONTENTSTATUS, 16, 1, RGB(255,0,255) );
   m_pTreeCtrl->SetImageList( &m_ilStatus, TVSIL_NORMAL );

   return 0;
}

//
// Bind the tree to the ScriptBase and its children
//
void ScriptContentTree::Bind( ScriptBasePtr sbp )
{
   // Make sure the document doesn't have a test selected
   m_pDoc->SetSelectedTest( NULL );
   m_pDoc->SetSelectedSection( NULL );

   if (m_pScriptContent)
   {
      // Disconnect the tree items from the old script nodes
      UnLoad( TVI_ROOT, m_pScriptContent );

      // Delete the old script nodes
      delete m_pScriptContent;
   }

   // Delete all items from the tree
   m_pTreeCtrl->DeleteAllItems();

   // Set to the new content
   m_pScriptContent = sbp;
   if (sbp)
   {
      // Add the content to the tree
      Load( TVI_ROOT, sbp );
   }
}

//
// Load the ScriptBase into the tree
//
void ScriptContentTree::Load( HTREEITEM parent, ScriptBasePtr sbp )
{
   HTREEITEM  itm;
   for (int i = 0; i < sbp->Length(); i++)
   {
      // Get the child
      ScriptBasePtr child = sbp->Child( i );

      // Insert the item
      itm = m_pTreeCtrl->InsertItem( child->baseLabel,
                                     child->baseImage + child->baseStatus,
                                     child->baseImage + child->baseStatus,
                                     parent );
      // Let the child know its handle
      child->baseItem = itm;

      // Tell the tree item to point back to the child
      m_pTreeCtrl->SetItemData( itm, (DWORD)child );

      // Recurse for the child's children
      Load( itm, child );
   }
}

//
// Disconnect the tree from ScriptBase items
//
void ScriptContentTree::UnLoad( HTREEITEM parent, ScriptBasePtr sbp )
{
   for (int i = 0; i < sbp->Length(); i++)
   {
      // Get the child
      ScriptBasePtr child = sbp->Child( i );

      // Tell the tree item NOT to point back to the child
      HTREEITEM itm = child->baseItem;
      m_pTreeCtrl->SetItemData( itm, NULL );

      // Tell the child to forget its tree item
      child->baseItem = NULL;

      // Recurse for the child's children
      UnLoad( itm, child );
   }
}

//
// ScriptContentTree::OnSelchanged
//
void ScriptContentTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
   // A long-standing bug caused this function to be called with invalid
   // ScriptBasePtr, causing exceptions and crashes.
   // The cause was that the tree's ItemData retained pointers to
   // deleted ScriptBase objects, or queued up Windows messages
   // for objects that would be deleted before the messages got
   // processed.  We have added safety checkes to ScriptBase,
   // and added UnLoad to this class to clean the tree before
   // deleting the ScripBase objects.

   HTREEITEM     hNewItem = m_pTreeCtrl->GetSelectedItem();
   ScriptBasePtr pElem = (hNewItem != NULL)
                         ? (ScriptBasePtr)m_pTreeCtrl->GetItemData( hNewItem )
                         : NULL;
   if ((pElem != NULL) && pElem->IsValid())
   {
      m_pDoc->SetSelectedTest( NULL );
      m_pDoc->SetSelectedSection( NULL );

      // Change status text for include filename if selected...
      if (m_pframe != NULL)
      {
         CString strStatusText;
         if (!pElem->m_strFile.IsEmpty())
            strStatusText.Format("INCLUDE: [%s]", pElem->m_strFile );

         m_pframe->SetMessageText(strStatusText);
      }

      // If a test is selected
      if (pElem->baseType == ScriptBase::scriptTest)
         m_pDoc->SetSelectedTest( (ScriptTestPtr)pElem );

      // If a section is selected
      if (pElem->baseType == ScriptBase::scriptSection)
         m_pDoc->SetSelectedSection( (ScriptSectionPtr)pElem );
   }

   *pResult = 0;
}

//
// ScriptContentTree::OnDblclk
//

void ScriptContentTree::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
   TRACE0( "Double click\n" );

#if 0
   // test flipping the status
   if (m_pSelectedElement->baseType == scriptTest)
      m_pDoc->SetTestStatus( (ScriptTestPtr)m_pSelectedElement
         , (m_pSelectedElement->baseStatus + 1) % 4
         );
#endif

   *pResult = 0;
}

//******************************************************************
// Author:     Yajun Zhou
// Date:    2002-7-12
// Purpose: Auto-scroll to the interrelated text in the edit pane
//          when double-clicking on a test item.  
//******************************************************************
void ScriptContentTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   UINT uFlags;
   HTREEITEM htItem = m_pTreeCtrl->HitTest(point, &uFlags);
   if ((htItem != NULL) && (TVHT_ONITEM & uFlags))
   {
      ScriptBasePtr  pElem = (ScriptBasePtr)m_pTreeCtrl->GetItemData( htItem );
      ScriptFrame* pCFrm = (ScriptFrame*)GetParentFrame();

      // madanner 6/03, add support for editing include files...
      if ( pElem->m_strFile.IsEmpty() )
      {
         pCFrm->SetActiveView(m_pEditView);
         m_pEditView->GotoLine(pElem->baseLineStart+1);
      }
      else
      {
         CScriptEditIncludeDlg   dlgInclude(pElem->m_strFile, NULL, NULL, pElem->baseLineStart+1, pCFrm);
         dlgInclude.DoModal();
      }
   }
   
   CTreeView::OnLButtonDblClk(nFlags, point);
}


//madanner 6/03, added to support different colors in tree for noded found in include file

BOOL ScriptContentTree::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
   if (message == WM_NOTIFY)
   {
      NMHDR *pnmh = (LPNMHDR) lParam; 

      // Is the code set to custom drawing?
      if (pnmh->code == NM_CUSTOMDRAW)
      {
         LPNMTVCUSTOMDRAW pTVCustomDraw = (LPNMTVCUSTOMDRAW) lParam;

         if (pTVCustomDraw)
         {
            switch (pTVCustomDraw->nmcd.dwDrawStage)
            {
               case CDDS_PREPAINT:             *pLResult = CDRF_NOTIFYITEMDRAW;
                                       return TRUE;

               case CDDS_ITEMPREPAINT:
                  {
                     //HTREEITEM hItem = HTREEITEM(pTVCustomDraw->nmcd.dwItemSpec);
                     ScriptBasePtr pbase = (ScriptBasePtr) pTVCustomDraw->nmcd.lItemlParam;

                     try {
                     if ( pbase != NULL  && !pbase->m_strFile.IsEmpty() )
                        pTVCustomDraw->clrText = RGB(0,0,255);
                     } catch (...) {}
                  }
            }
         }
      }
   }

   return CTreeView::OnChildNotify(message, wParam, lParam, pLResult);
}
