#if !defined(AFX_VTSNOTIFICATIONPARAMETERS_H__C16F9182_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFICATIONPARAMETERS_H__C16F9182_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSNotificationParameters.h : header file
//

#include "BACnet.hpp"

#include "VTSNotifyPage.h"

#include "VTSNotifyBitstring.h"
#include "VTSNotifyState.h"
#include "VTSNotifyValue.h"
#include "VTSNotifyCmdFailure.h"
#include "VTSNotifyFloatLimit.h"
#include "VTSNotifyOutOfRange.h"
#include "VTSNotifyComplex.h"
#include "VTSNotifyBufferReady.h"		//Added by Zhu Zhenhua, 2004-5-17
#include "VTSNotifyExtended.h"
#include "VTSNotifyUnsignedRange.h"
#include "VTSNotifyChgLifeSafety.h"

/////////////////////////////////////////////////////////////////////////////
// VTSNotificationParameters

class VTSNotificationParameters : public CPropertySheet
{
	DECLARE_DYNAMIC(VTSNotificationParameters)

// Construction
public:
	VTSNotificationParameters(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	VTSNotificationParameters(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	bool				ctrlNull;				// no value specified
	int					m_EventType;			// also current page 0..7

	VTSNotifyBitstring	BitstringPage;
	VTSNotifyState		StatePage;
	VTSNotifyValue		ValuePage;
	VTSNotifyCmdFailure	CmdFailurePage;
	VTSNotifyFloatLimit	FloatLimitPage;
	VTSNotifyOutOfRange	OutOfRangePage;
	VTSNotifyComplex	ComplexPage;
	VTSNotifyBufferReady BufferReadyPage;      	//Added by Zhu Zhenhua, 2004-5-17
	VTSNotifyChgLifeSafety ChgLifeSafetyPage;
	VTSNotifyExtended	ExtendedPage;
	VTSNotifyUnsignedRange	UnsignedRangePage;

	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );	// encode

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSNotificationParameters)
	public:
	virtual BOOL OnInitDialog();
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VTSNotificationParameters();

	void AddPages( void );

	// Generated message map functions
protected:
	//{{AFX_MSG(VTSNotificationParameters)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSNOTIFICATIONPARAMETERS_H__C16F9182_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
