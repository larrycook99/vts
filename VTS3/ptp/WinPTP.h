// WinPTP.h: interface for the WinPTP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINPTP_H__0BB57C93_26B8_4E78_8AF2_B4840ED415DF__INCLUDED_)
#define AFX_WINPTP_H__0BB57C93_26B8_4E78_8AF2_B4840ED415DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BACnet.hpp"
#include "ptp.h"

// PTP supported by Xiao Shiyuan 2002-4-22

class WinPTP : public  BACnetPort  
{
	friend UINT WinPTPThreadFunc( LPVOID pParam );

public:
	WinPTP(const char *config);
	~WinPTP(void);

	void Indication( const BACnetNPDU &pdu );
	void SendData( BACnetOctet *data, int len );
	void InitPort(const char *port);  

private:
	bool	m_Continue;
	CWinThread				*m_Thread;
};

typedef WinPTP *WinPTPPtr; 

#endif // !defined(AFX_WINPTP_H__0BB57C93_26B8_4E78_8AF2_B4840ED415DF__INCLUDED_)
