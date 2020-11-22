#if !defined(AFX_SCRIPTPARMLIST_H__CBE193B5_B97C_11D4_BEF2_00A0C95A9812__INCLUDED_)
#define AFX_SCRIPTPARMLIST_H__CBE193B5_B97C_11D4_BEF2_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcview.h>

//
//	ScriptParm
//

class ScriptParm {
		friend class ScriptParmList;

	protected:
		bool		parmMark;								// marked for deletion

	public:
		CString		parmName;								// parameter name
		int			parmSymbol;								// hash code
		CString		parmValue;								// current value (editable)
		CString		parmScriptValue;						// value as defined in the script
		CString		parmDesc;								// description
		int			parmLine;								// where parm is defined in script
		bool		parmEnv;								// parm is from the environment

		ScriptParm( const char *name );
		~ScriptParm( void );
	};

typedef ScriptParm *ScriptParmPtr;

//
// ScriptParmList
//

class ScriptParmList : public CListView
{
protected:
	ScriptParmList();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(ScriptParmList)

// Attributes
public:
	int				m_iSelectedElem;
	CImageList		m_ilStatus;							// status images

	// madanner 5/03, We have to maintain a copy of the list control so we can safely
	// search the array of parameters from another thread (the script thread managed by
	// the task manager).  Don't delete memory from this list... all handled elsewhere.

	CTypedPtrArray<CPtrArray, ScriptParm *>	m_apParms;

	CListCtrl		*m_pListCtrl;						// handy pointer to list control

// Operations
public:
	// list operations
	void Add( CString &name, CString &valu, CString &desc, int line, bool env );	// add a child at the end

	void Mark( bool env );								// set up for cleanup
	void Release( void );								// delete those still marked

	int Length( void );									// number of children
	ScriptParmPtr operator [](int indx);				// get a child

	void Reset( void );									// change all values to script defined

	int LookupIndex( int code );						// find by code, return index in list
	ScriptParmPtr LookupParm( int code );				// find by code, return pointer to parm

	void UpdateItemVisual( int nIndex, ScriptParmPtr pp );	// visually change nth item in list
	void UpdateParameterVisual( ScriptParmPtr pp );			// visually change parm identified here in list

	void SetCurrentEnv( void );							// set this list as the current environment
	void ResetCurrentEnv( void );						// reset

	void LoadEnv( void );								// load environment parameters into this list
	void UnloadEnv( void );								// unload environment parameters

	void MatchEnv( void );								// match loaded parameters with current environment
	void MatchEnvParm( ScriptParmPtr envpp );			// match a parameter

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScriptParmList)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~ScriptParmList();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(ScriptParmList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

typedef ScriptParmList *ScriptParmListPtr;

//
//	ScriptParmListList
//

class ScriptParmListList : public CList<ScriptParmListPtr,ScriptParmListPtr> {
	public:
		ScriptParmListList( void );
		~ScriptParmListList( void );

		void Add( ScriptParmListPtr slp );				// add a list
		void Remove( ScriptParmListPtr slp );			// remove a list

		int Length( void );								// number of tokens
		ScriptParmListPtr operator []( int i );			// index operator
	};

typedef ScriptParmListList *ScriptParmListListPtr;

extern ScriptParmList		*gCurrentEnv;					// points to current environment
extern ScriptParmListList	gScriptParmLists;				// list of parameter lists

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTPARMLIST_H__CBE193B5_B97C_11D4_BEF2_00A0C95A9812__INCLUDED_)
