// WinMSTP.h: interface for the WinMSTP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINMSTP_H__4A3DD8F8_80E0_48A0_B6F7_35BDB7CF2451__INCLUDED_)
#define AFX_WINMSTP_H__4A3DD8F8_80E0_48A0_B6F7_35BDB7CF2451__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BACnet.hpp"


// Klude for dialog implementation
class WinMSTPData
{
public:

	int 				m_nMaxMaster;
	int					m_nMaxInfoFrame;
	int 				m_nMAC;
	CString				m_strBaud;

public:
	WinMSTPData(LPCSTR lpszBaud, int nMAC, int nMaxMaster, int nMaxInfoFrame);
};


//===========================================================================
// Interface for different MSTP implementations

class WinMSTP;

class MSTPImplementation
{
public:
	WinMSTPData * m_pmstpData;			// use this guy for dialogs and such

protected:
	WinMSTP * m_pmstpPort;

public:
	MSTPImplementation( WinMSTP * pmstpPort, const char * config );
	virtual ~MSTPImplementation();

	virtual void Init() = 0;
	virtual void SendData( BACnetOctet *data, int len ) = 0;
	virtual void shutdown() {};
	virtual int ConfigureDlg(CWnd * pwndParent) = 0;
	virtual void LoadConfigString( CString * pstrConfig ) = 0;
};




// madanner 12/03
// This implementation uses the TCP/IP device NB-Link from American Automatrix
// to perform the actual MS/TP communcation.  The latency requirements for that 
// protocol are too small for a Windows implementation so the work is offloaded
// If changes to this idea are required then this class is all that needs to change.

class MSTPImp_NBLink : public MSTPImplementation
{
	friend UINT MSTP_NBLink_ThreadFunc( LPVOID pParam );

private:
	CWinThread			*m_Thread;

//	CSocket				*m_psocket;
	CAsyncSocket		*m_psocket;
	bool				m_Continue;

	int					m_nLocalPort;
	int					m_nListenPort;
	CString				m_strIPAddr;
	CString				m_strIPMask;
	CString				m_strIPGateway;
	bool				m_fInitOnStartup;


public:
	MSTPImp_NBLink( WinMSTP * pmstpPort, const char * config );

	virtual void Init();
	virtual void SendData( BACnetOctet *data, int len );
	virtual void shutdown();
	virtual int ConfigureDlg(CWnd * pwndParent);
	virtual void LoadConfigString( CString * pstrConfig );

	// Need to be public because they're called from a different thread
	void CloseConnection();
	bool SetupNBLinkDevice();

	// Used to format and disect parameters from 3rd party config dll
	void static FormatNBLinkString( CString * pstrOut, LPCSTR lpszIPRemote, LPCSTR lpszIP, LPCSTR lpszIPMask, LPCSTR lpszGateway, LPCSTR lpszBaud,
												int nListenPort, int nLocalPort, int nMAC, int nMaxMaster, int nMaxInfoFrame );
	void static GetLocalIPAddrString( CString * pstrOut );
	unsigned long static GetLocalIPAddr();
	bool CompareParameters( char * lpszParms, LPCSTR lpszIPThis );
};



class WinMSTP : public  BACnetPort  
{
private:
	MSTPImplementation * m_pmstpimp;

	void InitPort( const char * config );
	void shutdown();

public:
	int 				m_nMaxMaster;
	int					m_nMaxInfoFrame;
	int 				m_nMAC;
	CString				m_strBaud;


public:
	WinMSTP(const char * config);
	virtual ~WinMSTP(void);

	virtual void SetPortStatus( int nStatus, char * pszStatusMsg ){};

	void Indication( const BACnetNPDU &pdu );
	void SendData( BACnetOctet *data, int len );

	virtual void Msg( const char * msg ) {};
};




#endif // !defined(AFX_WINMSTP_H__4A3DD8F8_80E0_48A0_B6F7_35BDB7CF2451__INCLUDED_)
