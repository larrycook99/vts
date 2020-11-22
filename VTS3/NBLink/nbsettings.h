#ifndef _NB_SETTINGS_H_
#define _NB_SETTINGS_H_

#define CNX_IMPORT extern "C" __declspec(dllexport)

//Protocol Definitions
#define NBRS4852WIRE		3
#define	NBRS4854WIRE		1
#define NBRS232C			0

//Baud Rate Definitions
#define	NBBR38400			0
#define	NBBR19200			1
#define NBBR9600			2
#define NBBR115200			8
#define	NBBR57600			9

//Flow Control
#define	NBNOFLOWCTL			0

//Bit definitions
#define NB7BIT				8
#define NB8BIT				12

//Parity definitions
#define	NBEVENPARITY		48
#define	NBODDPARITY			16
//For no parity neither of these bits are set, so we mask...
#define	NBNOPARITYMASK1		32
#define	NBNOPARITYMASK2		16

//Stop bit definiitions
#define	NB1STOPBIT			64
#define	NB2STOPBIT			196

//Error Codes
#define INVALID_SUBNET		3
#define BAD_CONF			-1

//************************************************************
//NBInit -- open a socket, request configuration
//************************************************************
//Opens the socket
//Returns:
//	generic error code, can be decoded with strerror call
//	WinSock Error	--meaning can be looked up
//************************************************************
int _stdcall NBInit(LPCSTR addr);

//************************************************************
//NBSetLinkConfiguration -- sets the link's settings
//************************************************************
//returns:
// WAIT_TIMEOUT		-- the set configuration send failed
// INVALID_SUBNET	-- incorrect SUBNET setting
// WinSock Error	-- meaning can be looked up
//************************************************************
int _stdcall NBSetLinkConfiguration(LPCTSTR pszString);

//************************************************************
//NBFactorySettings -- resets link to it's factory defaults
//************************************************************
//returns:
// WAIT_TIMEOUT		-- the set configuration send failed
// INVALID_SUBNET	-- incorrect SUBNET setting (should never happen here)
// WinSock Error	-- meaning can be looked up
//************************************************************
int _stdcall NBFactorySettings(LPCTSTR lpszAddress);

//************************************************************
//CQueryLinkConfiguration -- fills a string with link settings
//************************************************************
//returns:
//	WAIT_TIMEOUT		-- the set configuration send failed
//	0 for success
//	-1 for no setting available
//************************************************************
int _stdcall NBQueryLinkConfiguration(LPSTR lpszString, int nMaxSize);

//************************************************************
//NBUnload -- Shuts down the interface
//************************************************************
//Returns:
//  true if successful
//  false if failed
//************************************************************
int _stdcall NBUnload();

CString LookupError(int errCode);

#endif