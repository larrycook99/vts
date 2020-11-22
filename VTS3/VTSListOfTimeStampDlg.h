#if !defined(AFX_VTSLISTOFTIMESTAMPDLG_H__E68A1C28_94A8_4502_BFF8_D85D758119A6__INCLUDED_)
#define AFX_VTSLISTOFTIMESTAMPDLG_H__E68A1C28_94A8_4502_BFF8_D85D758119A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSListOfTimeStampDlg.h : header file
//
#include "VTSCtrl.h"
class VTSListOfTimeStampDlg;
class TimeStampElem {
	public:
		TimeStampElem( VTSListOfTimeStampDlg* wp );	
		int						m_nChoice;		
		VTSTimeCtrl				m_Time;
		VTSUnsignedCtrl			m_SeqNum;
		VTSDateCtrl				m_DTDate;
		VTSTimeCtrl				m_DTTime;
		void Bind( void );						// this property is current
		void Unbind( void );					// detach from controls

		void Encode( BACnetAPDUEncoder& enc, int context = kAppContext);
		void Decode ( BACnetAPDUDecoder& dec);
	};

typedef TimeStampElem* TimeStampElemPtr;
class TimeStampList : public CList<TimeStampElemPtr,TimeStampElemPtr> {
	public:
		VTSListOfTimeStampDlg*			tsParentPtr;			// bound to specific page
		TimeStampElemPtr                 tscurElem;			// current element
		int								tscurElemIndx;		// index of current element
		bool							tsaddInProgress;	// adding an object

		TimeStampList(VTSListOfTimeStampDlg* pp );
		~TimeStampList( void );
		void AddButtonClick( void );				// add a new property
		void RemoveButtonClick( void );				// remove the selected property
		void OnChangeTime();
	    void OnChangeSeqnum();
	    void OnChangeDtdate();
	    void OnChangeDttime();
		void OnTimeradio();
        void OnDatetimeradio();
        void OnUnsignedradio();
		void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );	// selected summary element changing
		void Encode( BACnetAPDUEncoder& enc );
	};

typedef TimeStampList *TimeStampListPtr;

/////////////////////////////////////////////////////////////////////////////
// VTSListOfTimeStampDlg dialog
class VTSListOfTimeStampDlg : public CDialog
{
		DECLARE_DYNCREATE(VTSListOfTimeStampDlg)
// Construction
public:
	VTSListOfTimeStampDlg(CWnd* pParent = NULL);   // standard constructor
	void Decode(BACnetAPDUDecoder& dec);
	void Encode( BACnetAPDUEncoder& enc, int context = kAppContext );
	TimeStampList m_TimeStampList;
// Dialog Data
	//{{AFX_DATA(VTSListOfTimeStampDlg)
	enum { IDD = IDD_LISTOF_TIMESTAMP };
	CListCtrl	m_TimeStampListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSListOfTimeStampDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VTSListOfTimeStampDlg)
	afx_msg void OnItemchangingTimestamplist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeTime();
	afx_msg void OnChangeSeqnum();
	afx_msg void OnChangeDtdate();
	afx_msg void OnChangeDttime();
	afx_msg void OnTimeradio();
	afx_msg void OnDatetimeradio();
	afx_msg void OnUnsignedradio();
	virtual BOOL OnInitDialog();
	afx_msg void OnAddsumelem();
	afx_msg void OnRemovesumelem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLISTOFTIMESTAMPDLG_H__E68A1C28_94A8_4502_BFF8_D85D758119A6__INCLUDED_)
