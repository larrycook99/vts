// VTS.h : main header file for the VTS application
//

#if !defined(AFX_VTS_H__BDE65080_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
#define AFX_VTS_H__BDE65080_B82F_11D3_BE52_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define MAX_SERVS_SUPP      50      // Must be at least as large as the number of elements in BACnetServicesSupported
#define DEVICE_LOOPBACK     1       // set to zero to disable "looped" messages

#define MAX_FAIL_TIMES         9    // Size of BACnetFailTimes array per 135.1-2013 4.5.9
#define MAX_BIBBS            200    // Number of BIBBS (plus growing room)
#define MAX_DATALINK_OPTIONS  30    // Size of array of supported datalinks from EPICS (plus growing room)

// Just the name.  Each usage must use GetTempPath for the path
#define FILE_CHECK_EPICS_CONS "EPICSConsChk.txt"

// TODO: These fundamental equates COULD go into its a file of various BACnet enumerations.
// Or they could all live HERE

// To avoid collisions with C++ NULL, BACnetPropertyIdentifier OBJECT_IDENTIFIER
// and suchlike, we have prepended PRIM_ to these
#define PRIM_NULL               0
#define PRIM_BOOLEAN            1
#define PRIM_UNSIGNED           2
#define PRIM_SIGNED             3
#define PRIM_REAL               4
#define PRIM_DOUBLE             5
#define PRIM_OCTET_STRING       6
#define PRIM_CHARACTER_STRING   7
#define PRIM_BIT_STRING         8
#define PRIM_ENUMERATED         9
#define PRIM_DATE              10
#define PRIM_TIME              11
#define PRIM_OBJECT_IDENTIFIER 12

enum  BACnetObjectType
{
   OBJ_ANALOG_INPUT            = 0,
   OBJ_ANALOG_OUTPUT           = 1,
   OBJ_ANALOG_VALUE            = 2,
   OBJ_BINARY_INPUT            = 3,
   OBJ_BINARY_OUTPUT           = 4,
   OBJ_BINARY_VALUE            = 5,
   OBJ_CALENDAR                = 6,
   OBJ_COMMAND                 = 7,
   OBJ_DEVICE                  = 8,
   OBJ_EVENT_ENROLLMENT        = 9,
   OBJ_FILE                    = 10,
   OBJ_GROUP                   = 11,
   OBJ_LOOP                    = 12,
   OBJ_MULTI_STATE_INPUT       = 13,
   OBJ_MULTI_STATE_OUTPUT      = 14,
   OBJ_NOTIFICATIONCLASS       = 15,
   OBJ_PROGRAM                 = 16,
   OBJ_SCHEDULE                = 17,
   OBJ_AVERAGING               = 18,
   OBJ_MULTI_STATE_VALUE       = 19,
   OBJ_TREND_LOG               = 20,
   OBJ_LIFE_SAFETY_POINT       = 21,
   OBJ_LIFE_SAFETY_ZONE        = 22,
   OBJ_ACCUMULATOR             = 23,
   OBJ_PULSE_CONVERTER         = 24,
   OBJ_EVENT_LOG               = 25,
   OBJ_GLOBAL_GROUP            = 26,
   OBJ_TREND_LOG_MULTIPLE      = 27,
   OBJ_LOAD_CONTROL            = 28,
   OBJ_STRUCTURED_VIEW         = 29,
   OBJ_ACCESS_DOOR             = 30,
   OBJ_UNUSED_OBJ_31           = 31, // Was lighting-output during early review
   OBJ_ACCESS_CREDENTIAL       = 32, // Addendum 2008-j
   OBJ_ACCESS_POINT            = 33,
   OBJ_ACCESS_RIGHTS           = 34,
   OBJ_ACCESS_USER             = 35,
   OBJ_ACCESS_ZONE             = 36,
   OBJ_CREDENTIAL_DATA_INPUT   = 37, // Last in protocol revision 9
   OBJ_NETWORK_SECURITY        = 38, // 38 Addendum 2008-g
   OBJ_BITSTRING_VALUE         = 39, // protocol revision 10
   OBJ_CHARACTERSTRING_VALUE   = 40,
   OBJ_DATE_PATTERN_VALUE      = 41,
   OBJ_DATE_VALUE              = 42,
   OBJ_DATETIME_PATTERN_VALUE  = 43,
   OBJ_DATETIME_VALUE          = 44,
   OBJ_INTEGER_VALUE           = 45,
   OBJ_LARGE_ANALOG_VALUE      = 46,
   OBJ_OCTETSTRING_VALUE       = 47,
   OBJ_POSITIVE_INTEGER_VALUE  = 48,
   OBJ_TIME_PATTERN_VALUE      = 49,
   OBJ_TIME_VALUE              = 50, // 50 Last in 2008-w, protocol revision 10
   OBJ_NOTIFICATION_FORWARDER  = 51, // protocol revision 13
   OBJ_ALERT_ENROLLMENT        = 52,
   OBJ_CHANNEL                 = 53, // protocol revision 14
   OBJ_LIGHTING_OUTPUT         = 54, // last in 135-2012 protocol revision 14

   // TODO: see comments at the end of StringTables.cpp BACnetObjectType regarding
   // actions to be taken when an object type is added

   // After the last object, determine size of the above
   MAX_DEFINED_OBJ                   // This is one more than the highest defined object type
};

//
//	VTSApp
//

class VTSApp : public CWinApp {

	private:
		CDocTemplate      *m_pdoctempConfig;
		CRecentFileList   *m_pRecentWorkspaceList;
		CString           m_version;

	public:
		VTSApp(void);
		virtual ~VTSApp(void);

		// Get the full path of theDirectory relative to the executable
		void GetRelativeToExe( CString &thePath, LPCTSTR theDirectory ) const;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(VTSApp)
		public:
		virtual BOOL InitInstance();
		virtual int ExitInstance();
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		//}}AFX_VIRTUAL

		void LoadWorkspaceMRU(UINT nMaxMRU);
		void AddToRecentWorkspaceList(LPCTSTR lpszPathName);
		CRecentFileList* GetRecentFileList();

		CDocument * GetWorkspace(void);
		void CheckWinPcapVersion( void );

		const CString& GetVersion() const { return m_version; }
		void ShowUpdateInfo();

	// Implementation
		//{{AFX_MSG(VTSApp)
		afx_msg void OnAppAbout();
		afx_msg void OnFileWksNew();
		afx_msg void OnFileWksSwitch();
		afx_msg void OnUpdateRecentWorkspaceMenu(CCmdUI* pCmdUI);
		afx_msg BOOL OnOpenRecentWorkspace(UINT nID);
		afx_msg void OnCheckForUpdates();

		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

typedef VTSApp *VTSAppPtr;

//
//	Application specific windows messages
//

#define WM_VTS_RCOUNT		(WM_APP+1)			// new record count
#define WM_VTS_PORTSTATUS	(WM_APP+2)			// new port status
#define WM_VTS_EXECMSG		(WM_APP+3)			// new executor message
#define WM_VTS_MAXPACKETS	(WM_APP+4)			// reached max packets
#define WM_VTS_GOT_VERSION	(WM_APP+5)			// got lastest version info from the web


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTS_H__BDE65080_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
