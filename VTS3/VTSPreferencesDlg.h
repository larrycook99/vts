#if !defined(AFX_VTSPREFERENCESDLG_H__9E8924BA_AF56_4134_9676_05CD965B0E02__INCLUDED_)
#define AFX_VTSPREFERENCESDLG_H__9E8924BA_AF56_4134_9676_05CD965B0E02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSPreferencesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VTSPreferencesDlg dialog

class VTSPreferencesDlg : public CDialog
{
// Construction
public:
	VTSPreferencesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(VTSPreferencesDlg)
	enum { IDD = IDD_PREFERENCES };
	int		m_nAutoscrollTimeout;
	int		m_nPacketCount;
	int		m_nRelative;
	BOOL	m_fVerify;
	BOOL	m_fLoadEPICS;
	BOOL	m_bAutoScroll;
	BOOL	m_bRecvPkt;
	BOOL	m_bSaveSentPkt;
	UINT	m_resendInterval;
	CString	m_textEditor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSPreferencesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSPreferencesDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPREFERENCESDLG_H__9E8924BA_AF56_4134_9676_05CD965B0E02__INCLUDED_)
