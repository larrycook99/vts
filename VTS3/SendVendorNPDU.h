#if !defined(AFX_SENDVENDORNPDU_H__060822B1_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_)
#define AFX_SENDVENDORNPDU_H__060822B1_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendVendorNPDU.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendVendorNPDU dialog

class CSendVendorNPDU : public CSendPage
{
   DECLARE_DYNCREATE(CSendVendorNPDU)

// Construction
public:
   CSendVendorNPDU( void );   // non-standard constructor

   VTSIntegerCtrl       m_MsgType;        // Message Type
   VTSIntegerCtrl       m_VendorID;       // Vender ID
   VTSOctetStringCtrl   m_Data;           // message data

   void InitPage( void );                 // give it a chance to init
   void EncodePage( CByteArray* contents );  // encode the page

   static BACnetAPDUEncoder   pageContents;

   void SavePage( void );                 // save contents
   void RestorePage( int index = 0 );     // restore contents to last saved values

// Dialog Data
   //{{AFX_DATA(CSendVendorNPDU)
   enum { IDD = IDD_SENDVENDORNPDU };
      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSendVendorNPDU)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CSendVendorNPDU)
   afx_msg void OnChangeMsgType();
   afx_msg void OnChangeVendorID();
   afx_msg void OnChangeData();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDVENDORNPDU_H__060822B1_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_)
