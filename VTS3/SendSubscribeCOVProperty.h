#pragma once

#include "SendPage.h"
#include "VTSCtrl.h"
#include "afxwin.h"


// CSendSubscribeCOVProperty dialog

class CSendSubscribeCOVProperty : public CSendPage
{
	DECLARE_DYNAMIC(CSendSubscribeCOVProperty)

public:
	CSendSubscribeCOVProperty();
	virtual ~CSendSubscribeCOVProperty();

// Dialog Data
	enum { IDD = IDD_SENDSUBSCRIBECOVPROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
  VTSUnsignedCtrl m_ProcessID;
  VTSObjectIdentifierCtrl m_ObjectID;
  VTSUnsignedCtrl m_ArrayIndex;
  VTSBooleanCtrl m_IssueConfirmed;
  VTSUnsignedCtrl m_Lifetime;
  VTSRealCtrl m_COVIncrement;
  VTSEnumeratedCtrl m_property;
	static BACnetAPDUEncoder	pageContents;
  void InitPage( void );
  void EncodePage( CByteArray* contents );
  void SavePage( void );
  void RestorePage( int index );
  afx_msg void OnBnClickedAddprop();
  afx_msg void OnEnChangeProcessid();
  afx_msg void OnBnClickedObjectidbtn();
  afx_msg void OnEnChangeIssueconfirmed();
  afx_msg void OnCbnDropdownPropcombo();
  afx_msg void OnEnChangeReal();
  afx_msg void OnEnUpdateObjectid();
  afx_msg void OnCbnSelchangePropcombo();
  afx_msg void OnEnChangeArrayindex();
  afx_msg void OnEnChangeLifetime();
};
