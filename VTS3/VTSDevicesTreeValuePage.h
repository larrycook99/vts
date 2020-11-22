#if !defined(AFX_VTSDEVICESTREEVALUEPAGE_H__DBE0B95A_1E36_4827_BC7A_52B9431F6F0E__INCLUDED_)
#define AFX_VTSDEVICESTREEVALUEPAGE_H__DBE0B95A_1E36_4827_BC7A_52B9431F6F0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSDevicesTreeValuePage.h : header file
//

#include "VTSPropertyPage.h"
#include "VTSBACnetTypeGenericPage.h"
#include "VTSDevicesTreeObjPage.h"


class VTSDevValue;

/////////////////////////////////////////////////////////////////////////////
// VTSDevicesTreeValuePage dialog

class VTSDevicesTreeValuePage : public VTSPropertyPage, public VTSPageCallbackInterface
{
	DECLARE_DYNCREATE(VTSDevicesTreeValuePage)

private:
	VTSDevValue *		m_pdevvalue;

	CPropertySheet				m_sheetValueContainer;
	VTSBACnetTypeGenericPage	m_pageGeneric;
	VTSDevicesTreeObjPage		m_pageObjectID;

	VTSDevObject				m_devobject;			// for the support of Object ID page

// Construction
public:
	VTSDevicesTreeValuePage( VTSPageCallbackInterface * pparent );
	VTSDevicesTreeValuePage();
	~VTSDevicesTreeValuePage();

	// To support callback from child pages
	virtual void DataChangeNotification(void);
	virtual void * GetActiveData(void);

// Dialog Data
	//{{AFX_DATA(VTSDevicesTreeValuePage)
	enum { IDD = IDD_DEVTREEVALUE };
	CString	m_strValue;
	int		m_nDataType;
	int		m_nContext;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSDevicesTreeValuePage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSDevicesTreeValuePage)
	afx_msg void OnSelChangeTypeCombo();
	afx_msg void OnEditChangeContextTag();
	//}}AFX_MSG

	void ValueToCtrl( VTSDevValue * pdevvalue );
	void CtrlToValue( VTSDevValue * pdevvalue );
	void EnableControls( bool fEnable );
	void InitializeTypeSheet( void );
	void SwapTypePages(void);

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDEVICESTREEVALUEPAGE_H__DBE0B95A_1E36_4827_BC7A_52B9431F6F0E__INCLUDED_)
