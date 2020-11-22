#if !defined(AFX_VTSPROPERTYPAGE_H__11D873B1_4AC0_4804_97AA_62C4AE8E7488__INCLUDED_)
#define AFX_VTSPROPERTYPAGE_H__11D873B1_4AC0_4804_97AA_62C4AE8E7488__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class VTSPageCallbackInterface
{
	public:
		virtual void DataChangeNotification(void) = 0;
		virtual void * GetActiveData(void) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// VTSPropertyPage dialog

class VTSPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(VTSPropertyPage)

private:
	VTSPageCallbackInterface *	m_pparent;

// Construction
public:
	VTSPropertyPage( int nIDD, VTSPageCallbackInterface * pparent );
	VTSPropertyPage();
	~VTSPropertyPage();

// Dialog Data
	//{{AFX_DATA(VTSPropertyPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(VTSPropertyPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(VTSPropertyPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG

	void NotifyOfDataChange(void);
	void * RetrieveCurrentData(void);

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPROPERTYPAGE_H__11D873B1_4AC0_4804_97AA_62C4AE8E7488__INCLUDED_)
