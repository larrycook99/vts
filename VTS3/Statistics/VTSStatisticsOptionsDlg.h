#if !defined(AFX_STAITISTICSOPTIONSDLG_H__6610DA97_F89C_4C23_AEA3_F6D9B0B5BF7B__INCLUDED_)
#define AFX_STAITISTICSOPTIONSDLG_H__6610DA97_F89C_4C23_AEA3_F6D9B0B5BF7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaitisticsOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VTSStatisticsOptionsDlg dialog

class VTSStatisticsOptionsDlg : public CDialog
{
// Construction
public:
	VTSStatisticsOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(VTSStatisticsOptionsDlg)
	enum { IDD = IDD_STATISTICS_OPTIONS };
	BOOL	m_bArcnetReceive;
	BOOL	m_bArcnetSend;
	BOOL	m_bEthernetReceive;
	BOOL	m_bIPRecive;
	BOOL	m_bEthernetSend;
	BOOL	m_bIPSend;
	BOOL	m_bMSTPReceive;
	BOOL	m_bMSTPSend;
	BOOL	m_bOverallReceive;
	BOOL	m_bOverallSend;
	BOOL	m_bPTPReceive;
	BOOL	m_bPTPSend;
	int		m_nSizeArray1;
	int		m_nSizeArray2;
	int		m_nSizeArray3;
	int		m_nSizeArray4;
	int		m_nSizeArray5;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSStatisticsOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSStatisticsOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelAll();
	afx_msg void OnClearAll();
	afx_msg void OnChangeRange2();
	afx_msg void OnChangeRange4();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STAITISTICSOPTIONSDLG_H__6610DA97_F89C_4C23_AEA3_F6D9B0B5BF7B__INCLUDED_)
