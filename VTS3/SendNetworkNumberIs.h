#if !defined(AFX_SENDNETWORKNUMBERIS_H__060822B2_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_)
#define AFX_SENDNETWORKNUMBERIS_H__060822B2_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SendPage.h"
#include "VTSCtrl.h"

// CSendNetworkNumberIs dialog
// Dialog to send a Network-Number-Is NPDU

class CSendNetworkNumberIs : public CSendPage
{
   DECLARE_DYNAMIC(CSendNetworkNumberIs)

public:
   CSendNetworkNumberIs(void);   // non-standard constructor
   virtual ~CSendNetworkNumberIs();

   // Overrides of base class methods
   virtual void InitPage( void );                     // give it a chance to init
   virtual void EncodePage( CByteArray* contents );   // encode the page into contents
   virtual void SavePage( void );                     // save contents
   virtual void RestorePage( int index = 0 );         // restore contents to last saved values  

   static BACnetAPDUEncoder   pageContents;

   VTSIntegerCtrl       m_networkNumber;
   VTSEnumeratedCtrl    m_typeCombo;

   // Dialog Data
   enum { IDD = IDD_SENDNETNUMBERIS };

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSendNetworkNumberIs)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   // Generated message map functions
   //{{AFX_MSG(CSendNetworkNumberIs)
   afx_msg void OnChangeNetworkNumber();
   afx_msg void OnSelchangeTypeCombo();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SENDNETWORKNUMBERIS_H__060822B2_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_)
