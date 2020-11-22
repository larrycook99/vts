#if !defined(AFX_SENDWHATISNETWORKNUMBER_H__060822B2_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_)
#define AFX_SENDWHATISNETWORKNUMBER_H__060822B2_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SendPage.h"
#include "VTSCtrl.h"

// CSendWhatIsNetworkNumber dialog
// Dialog to send a What-Is-Network-Number NPDU

class CSendWhatIsNetworkNumber : public CSendPage
{
   DECLARE_DYNAMIC(CSendWhatIsNetworkNumber)

public:
   CSendWhatIsNetworkNumber(void);   // non-standard constructor
   virtual ~CSendWhatIsNetworkNumber();

   // Overrides of base class methods
   virtual void InitPage( void );                     // give it a chance to init
   virtual void EncodePage( CByteArray* contents );   // encode the page into contents

   // Dialog Data
   enum { IDD = IDD_SENDWHATISNET };

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSendWhatIsNetworkNumber)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   // Generated message map functions
   //{{AFX_MSG(CSendWhatIsNetworkNumber)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SENDWHATISNETWORKNUMBER_H__060822B2_44C9_11D4_BEB1_00A0C95A9812__INCLUDED_)
