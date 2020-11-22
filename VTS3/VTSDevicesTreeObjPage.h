#if !defined(AFX_VTSDEVICESTREEOBJPAGE_H__BFB0328D_B463_45D2_9C8F_0488E6C235CE__INCLUDED_)
#define AFX_VTSDEVICESTREEOBJPAGE_H__BFB0328D_B463_45D2_9C8F_0488E6C235CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDevicesTreeObjPage.h : header file
//

#include "VTSPropertyPage.h"

class VTSDevObject;

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeObjPage dialog

class VTSDevicesTreeObjPage : public VTSPropertyPage
{
   DECLARE_DYNCREATE(VTSDevicesTreeObjPage)

   VTSDevObject *       m_pdevobject;

// Construction
public:
   VTSDevicesTreeObjPage( VTSPageCallbackInterface * pparent );
   VTSDevicesTreeObjPage();   // not used
   ~VTSDevicesTreeObjPage();

// Dialog Data
   //{{AFX_DATA(VTSDevicesTreeObjPage)
   enum { IDD = IDD_DEVTREEOBJ };
   CComboBox   m_ObjTypeCombo;
   int         m_nObjType;
   int         m_nInstance;
   //}}AFX_DATA

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(VTSDevicesTreeObjPage)
   public:
   virtual BOOL OnSetActive();
   virtual BOOL OnKillActive();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(VTSDevicesTreeObjPage)
      // NOTE: the ClassWizard will add member functions here
   afx_msg void OnSelChangeObjCombo();
   afx_msg void OnChangeObjType();
   afx_msg void OnChangeInstance();
   //}}AFX_MSG

   void ObjToCtrl( VTSDevObject * pdevobject );
   void CtrlToObj( VTSDevObject * pdevobject );
   void ClearData(void);
   void EnableControls( bool fEnable );
   void ValidateTypeCode(void);

   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDEVICESTREEOBJPAGE_H__BFB0328D_B463_45D2_9C8F_0488E6C235CE__INCLUDED_)
