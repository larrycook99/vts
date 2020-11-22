#if !defined(AFX_VTSTIMESTAMP_H__A5F46EB1_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
#define AFX_VTSTIMESTAMP_H__A5F46EB1_797F_11D4_BED9_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSTimeStamp.h : header file
//

#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// VTSTimeStamp dialog

class VTSTimeStamp : public CDialog
{
// Construction
public:
	VTSTimeStamp(CWnd* pParent = NULL);   // standard constructor

	VTSTimeCtrl				m_Time;
	VTSUnsignedCtrl			m_SeqNum;
	VTSDateCtrl				m_DTDate;
	VTSTimeCtrl				m_DTTime;

	void Encode( BACnetAPDUEncoder& enc, int context );

	void SaveCtrl( BACnetAPDUEncoder& enc );		// save the contents
	void RestoreCtrl( BACnetAPDUDecoder& dec );		// restore the contents

// Dialog Data
	//{{AFX_DATA(VTSTimeStamp)
	enum { IDD = IDD_TIMESTAMP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSTimeStamp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSTimeStamp)
	afx_msg void OnChangeTime();
	afx_msg void OnChangeSeqNum();
	afx_msg void OnChangeDTDate();
	afx_msg void OnChangeDTTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSTIMESTAMP_H__A5F46EB1_797F_11D4_BED9_00A0C95A9812__INCLUDED_)
