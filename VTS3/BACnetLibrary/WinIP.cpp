
#include "stdafx.h"

#include "BACnetIP.hpp"
#include "WinIP.hpp"

#include "Iphlpapi.h"   // for adapter enumeration

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UINT WinIPThreadFunc( LPVOID pParam );
void CheckLastError( const char *pTheFormat );
const char* GetSystemErrorMsg( CString &theResult, const char *pTheFormat, int nSocketError );

//=============================================================================
// Class to enumerate available IP adapters
class WinIP_EnumerateAdapters
{
public:
   IP_ADAPTER_INFO *m_pInfo;

   WinIP_EnumerateAdapters()
   : m_pInfo( NULL )
   {
   }

   ~WinIP_EnumerateAdapters()
   {
      if (m_pInfo)
         free(m_pInfo);
   }

   // "Sanitize" the adapter name to avoid conflicts with port specification string.
   // - turn commas into dashes
   static void Sanitize( CString &theString )
   {
      theString.Replace( ',', '-' );
   }

   // Get a list of available interfaces
   bool Enumerate()
   {
      if (m_pInfo == NULL)
      {
         // Allow several attempts if initial buffer is too small
         ULONG ret;
         ULONG size = 16384;
         for (int ix = 0; ix < 3; ix++)
         {
            m_pInfo = (IP_ADAPTER_INFO*)malloc(size);
            ret = GetAdaptersInfo( m_pInfo, &size );
            if (ret != ERROR_BUFFER_OVERFLOW)
            {
               break;
            }
            free( m_pInfo );
            m_pInfo = NULL;
         }

         if (ret != ERROR_SUCCESS)
         {
            CString str;
            GetSystemErrorMsg( str, "Adapter enumeration failed: %s", ret );
            AfxMessageBox( str );
            if (m_pInfo) {
               free(m_pInfo);
               m_pInfo = NULL;
            }
         }
      }

      return (m_pInfo != NULL);
   }

   // Fill a ComboBox with the available interfaces
   void FillCombo( CComboBox &theCombo )
   {
      theCombo.ResetContent();
      if (Enumerate())
      {
         IP_ADAPTER_INFO *pCur = m_pInfo;
         while (pCur)
         {
            // We find that
            // Name = {45355057-0882-430A-A542-99EA72F36353}
            // Description = Intel(R) 82567LM Gigabit Network Connection - Packet Scheduler Miniport
            // FriendlyName = Local Area Connection
            // So "Description" is the best to show a human
            if (strcmp(pCur->IpAddressList.IpAddress.String, "0.0.0.0") != 0)
            {
               // IP address is useful in the combo, but undesirable in the saved
               // configuration string, since DHCP may cause it to change from one
               // run to the next.  So the dialog that uses this combo must call
               // WinIP::GetInterfaceName to retrieve the saveable interface
               // name without IP address.
               //
               // We haven't encountered it, but SOMEWHERE there is probably an interface
               // description that has a comma in it, which would muck up the lame
               // scheme used by VTSPort to store interface specs.  So call Sanitize.
               // To esure matching voodo, GetAddresses must also Sanitize the description.
               CString desc( pCur->Description );
               Sanitize( desc );
               CString str;
               str.Format( "%s (%s)", (LPCTSTR)desc, pCur->IpAddressList.IpAddress.String );
               theCombo.AddString( str );
            }
            pCur = pCur->Next;
         }
      }
   }

   // Get the addresses best matching the specified adapter
   bool GetAddresses( const char *pTheAdapter,
                      in_addr     &theNodeAddress,
                      in_addr     &theBroadcastAddress )
   {
      bool retval = false;
      if (Enumerate())
      {
         IP_ADAPTER_INFO *pCur = m_pInfo;
         IP_ADAPTER_INFO *pDefault = NULL;
         while (pCur)
         {
            if (strcmp(pCur->IpAddressList.IpAddress.String, "0.0.0.0") != 0)
            {
               // Adapter has an address
               if (pDefault == NULL)
               {
                  // Remember this as the default adapter
                  pDefault = pCur;
               }

               // Compare the sanitized description
               CString desc( pCur->Description );
               Sanitize( desc );
               if (desc.Compare( pTheAdapter ) == 0)
               {
                  // Found
                  break;
               }
            }

            pCur = pCur->Next;
         }

         if (pCur == NULL)
         {
            CString str;
            if (pDefault == NULL)
            {
               str.Format( "No IP adapters are available" );
               AfxMessageBox( str );
            }
            else if (*pTheAdapter != 0)
            {
               str.Format( "Selected IP adapter \"%s\" is not available.\n"
                           "Using adapter \"%s\"", pTheAdapter, pDefault->Description );
               pCur = pDefault;
               AfxMessageBox( str );
            }
            else
            {
               // No adapter selected, as from an old-style configuration.
               // Just quietly use the default
               pCur = pDefault;
            }
         }

         if (pCur != NULL)
         {
            theNodeAddress.S_un.S_addr = inet_addr( pCur->IpAddressList.IpAddress.String );
            theBroadcastAddress.S_un.S_addr =
               theNodeAddress.S_un.S_addr | ~inet_addr( pCur->IpAddressList.IpMask.String );

            retval = true;
         }
      }

      return retval;
   }
};

//=============================================================================
// Fill a combo with the list of available IP interfaces
// static 
void WinIP::FillInterfaceCombo( CComboBox &theCombo )
{
   WinIP_EnumerateAdapters adapters;
   adapters.FillCombo( theCombo );
}

//=============================================================================
// Get the name of the currently selected IP interface from the combo
// static
void WinIP::GetInterfaceName( CString &theResult, CComboBox &theCombo )
{
   // Can't call GetWindowText, as we may be called by notification
   // BEFORE the combo's visible text changes.
   int sel = theCombo.GetCurSel();
   if (sel < 0)
   {
      sel = 0;
   }

   theCombo.GetLBText( sel, theResult );

   // Remove the trailing IP address, which is in parenthesis
   sel = theResult.ReverseFind( '(' );
   if (sel > 0)
   {
      theResult = theResult.Left( sel - 1 );
   }
}

//=============================================================================
//
// WinIP::WinIP
//
WinIP::WinIP( const char *pInterface, u_short udpPort )
: m_udpPort(0)
, m_Continue(false)
, m_socket(INVALID_SOCKET)
, m_Thread(NULL)
{
   InitPort( pInterface, udpPort );
}

//
// WinIP::WinIP
//
WinIP::WinIP( const char *pPortAndInterface )
: m_udpPort(0)
, m_Continue(false)
, m_socket(INVALID_SOCKET)
, m_Thread(NULL)
{
   // For historical reasons, pPortAndInterface has
   //   {UDP port number};{Interface Name},{B/IP function integer}{,possibly additional parameters for BBMD etc.}
   char *pOut;
   unsigned int udpPort = strtoul( pPortAndInterface, &pOut, 0 );  // handles decimal, or hex with 0x prefix
   pPortAndInterface = pOut;
   if (*pPortAndInterface == ';')
      pPortAndInterface += 1;

   // WinIP_EnumerateAdapters ensures that there are no commas in adapter descriptions
   // to confuse the parse.
   int len = strcspn( pPortAndInterface, "," );
   CString iface( pPortAndInterface, len );

   // Empty string selects the first available interface.
   InitPort( iface, udpPort );
}

//=============================================================================
//
// WinIP::InitPort
//
void WinIP::InitPort( const char *pInterface, u_short udpPort )
{
   // Until we are happy
   portStatus = -1;
   m_Continue = false;

   // Initialize WinSock
   WSADATA wsaData;
   WORD versionRequested = MAKEWORD( 2, 0 );
   int err = ::WSAStartup( versionRequested, &wsaData );
   if (err != 0)
   {
      CString errMsg;
      GetSystemErrorMsg( errMsg, "WSAStartup() failed: %s", err );
      AfxMessageBox( errMsg );
      return;
   }

   // initialize the socket descriptor
   m_socket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
   if (m_socket == INVALID_SOCKET)
   {
      CheckLastError( "socket() failed: %s" );
      return;
   }

   // http://blogs.msdn.com/zhengpei/Default.aspx?p=2 has info on
   // Windows multi-home operation.  Says
   // - Windows implements "weak end model": an address refers to a host,
   //   not an interface.
   // - bind to IPADDR_ANY will "pick" an address.
   //   - outgoing packets will be routed based on IP address
   //   - incoming packets that match ANY interface will be accepped
   // Thus, we prefer to bind to the address of a particular interface

   WinIP_EnumerateAdapters adapters;
   in_addr  nodeAddress, broadcastAddress;
   adapters.GetAddresses( pInterface, nodeAddress, broadcastAddress );
   m_udpPort = htons( udpPort );

   // We saw cases where SOMETHING hung onto our UDP port when we closed, especially
   // when debugging, and prevented us re-opening the port.  Debugger?  Anti-virus?
   // Firewall?  Who knows. So we added SO_REUSEADDR to prevent this by binding anyway.
   //
   // Will this also allow us to open when some OTHER program is using the port?
   // http://stackoverflow.com/questions/14388706/socket-options-so-reuseaddr-and-so-reuseport-how-do-they-differ-do-they-mean-t/14388707#14388707
   // says "A socket with SO_REUSEADDR can always bind to exactly the same source address
   // and port as an already bound socket, even if the other socket did not have this
   // option set when it was bound. This behavior is somewhat dangerous because it allows
   // an application "to steal" the connected port of another application.
   // Microsoft now recommends using SO_EXCLUSIVEADDRUSE to guard against SO_REUSEADDR."
   //
   // In our case we WANT reuse.  But it may not work if there is another application
   // running that uses SO_EXCLUSIVEADDRUSE.
   // https://msdn.microsoft.com/en-us/library/windows/desktop/ms740621%28v=vs.85%29.aspx
   // has a a chart showing what should and should not work.  But experiments show that
   // on my PC (at least) show that SO_REUSEADDR here won't bind if another app has the
   // port open, even if they use SO_REUSEADDR.  Could this be due to antivirus or
   // firewall on my corporate PC?  Feel free to do some experiments and let me know.
   BOOL bVal = TRUE;
   err = ::setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR,
                       (const char*)&bVal, sizeof(bVal) );
   if (err == SOCKET_ERROR) {
      CheckLastError( "setsockopt(SO_REUSEADDR) failed: %s" );
      // This one is non-fatal
   }

   // "When enabled, datagram sockets receive packets sent to a broadcast
   // address and they are allowed to send packets to a broadcast address."
   err = ::setsockopt( m_socket, SOL_SOCKET, SO_BROADCAST,
                       (const char*)&bVal, sizeof(bVal) );
   if (err == SOCKET_ERROR) {
      CheckLastError( "setsockopt(SO_BROADCAST) failed: %s" );
      // This one is non-fatal, although we probably won't be happy
   }

   // Bind to the adapter
   SOCKADDR_IN  saUdpServer;
   saUdpServer.sin_family = AF_INET;
   saUdpServer.sin_addr = nodeAddress;
   saUdpServer.sin_port = m_udpPort;
   err = ::bind( m_socket, (SOCKADDR*)&saUdpServer, sizeof(saUdpServer) );
   if (err == SOCKET_ERROR) {
      CheckLastError( "Failed to bind to the specified interface and UDP port.\n"
                        "Windows returned: \"%s\"\n"
                        "\n"
                        "This usually means that some other process is already bound "
                        "to the interface and port in a way that does not allow sharing.\n"
                        "\n"
                        "VTS may be able to transmit, but it will not be able to receive.\n"
                        "\n"
                        "In order to resolve the problem, close both VTS and the other "
                        "process, then restart VTS." );

      // To consider this fatal, clean up and go home (and change the error message)
      //   Close();
      //   return;
      // Or continue, but send-only (per the error message)
   }
   else
   {
      // Allow receive thread to run below
      m_Continue = true;
   }

   // Pack the host address and port into the portLocalAddr
   portLocalAddr.addrType = localStationAddr;
   portLocalAddr.addrLen = sizeof(unsigned long) + sizeof(unsigned short);
   memcpy( portLocalAddr.addrAddr, &nodeAddress.S_un, 4 );
   *(unsigned short *)(portLocalAddr.addrAddr + 4) = m_udpPort;

   // Get the broadcast address.
   portBroadcastAddr.addrType = localStationAddr;
   portBroadcastAddr.addrLen = sizeof(unsigned long) + sizeof(unsigned short);
   memcpy( portBroadcastAddr.addrAddr, &broadcastAddress.S_un, 4 );
   *(unsigned short *)(portBroadcastAddr.addrAddr + 4) = m_udpPort;

   if (m_Continue)
   {
      // Start receive thread suspended
      m_Thread =
         AfxBeginThread( WinIPThreadFunc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED );

      // Don't let windows delete the thread object
      m_Thread->m_bAutoDelete = FALSE;

      // Let it run
      m_Thread->ResumeThread();
   }

   // Clear the port status, all is well
   portStatus = 0;
}

//=============================================================================
// Shut down whatever parts of the port are active
void WinIP::Close()
{
   if (m_Continue)
   {
      // Thread is running: tell it to exit.
      m_Continue = false;

      // Closing the socket should force the recvfrom() to fail
      TRACE( "Shutting down IP port\n" );
      ::shutdown( m_socket, SD_BOTH );
      ::closesocket( m_socket );

      // See if our receive thread is still around
      int loopCount;
      for (loopCount = 0; loopCount < 5; loopCount++)
      {
         DWORD dwExitCode;
         ::GetExitCodeThread( m_Thread->m_hThread, &dwExitCode );

         CString msg;
         TRACE( "Waiting for IP receive thread 0x%X to exit.  %s",
                m_Thread->m_hThread, GetSystemErrorMsg( msg, "%s", dwExitCode ) );
         if (dwExitCode != STILL_ACTIVE)
         {
            break;
         }

         // The thread is still running; wait for it to go away
         ::WaitForSingleObject( m_Thread->m_hThread, 1000 );
      }

      if (loopCount == 5)
      {
         AfxMessageBox( "WARNING: terminating WinIP thread" );
         ::TerminateThread( m_Thread->m_hThread, 0 );
      }

      // The thread has terminated. Delete the CWinThread object.
      delete m_Thread;
   }
   else if (m_socket != INVALID_SOCKET)
   {
      ::shutdown( m_socket, SD_BOTH );
      ::closesocket( m_socket );
   }

   m_Thread = NULL;
   m_socket = INVALID_SOCKET;
}

//=============================================================================
//
// WinIP::~WinIP
//
WinIP::~WinIP()
{
   Close();
}

//=============================================================================
//
// WinIP::Indication
//
// This function is called when the client wants to send some data out the
// port.  The address must be a localStation or a localBroadcast.
//
void WinIP::Indication( const BACnetNPDU &pdu )
{
   BACnetOctet *msg;

   // check for nothing to send
   if (pdu.pduLen == 0)
      return;

   // allocate a buffer big enough for the address and data
   msg = new BACnetOctet[ pdu.pduLen + 6 ];

   // set up the socket address for sending
   if (pdu.pduAddr.addrType == localBroadcastAddr) {
      memcpy( msg, portBroadcastAddr.addrAddr, 4 );
      *(unsigned short *)(msg+4) = m_udpPort;
   }
   else if (pdu.pduAddr.addrType == globalBroadcastAddr) {
      // TODO: do we really want this (rather than localBroadcastAddr)?
      // "Global" is actually more about DNET/DADR
      // rather than the IP address.
      *(unsigned long *)msg = 0xFFFFFFFF;
      *(unsigned short *)(msg+4) = m_udpPort;
   }
   else if (pdu.pduAddr.addrType == localStationAddr) {
      memcpy( msg, pdu.pduAddr.addrAddr, 6 );
   } else {
      delete[] msg;
      return;
   }

   memcpy( msg + 6, pdu.pduData, pdu.pduLen );
   SendData( msg, pdu.pduLen + 6 );

   delete[] msg;
}

//=============================================================================
//
// WinIP::SendData
//
// This function is called with the data to send out the port.
//
void WinIP::SendData( BACnetOctet *data, int len )
{
   int         err;
   SOCKADDR_IN saUdpServer;

   // let the application filter it
   FilterData( data, len, portSending );

   // check for nothing to send
   if (len <= 6)
      return;

   // Check for port init failed
   if (portStatus != 0)
      return;

   // set up the socket address for sending
   saUdpServer.sin_family = AF_INET;
   saUdpServer.sin_addr.s_addr = *(unsigned long *)data;
   saUdpServer.sin_port = *(unsigned short *)(data + 4); 

   // send it out
   err = sendto( m_socket, (const char *)(data + 6), len - 6, 0,
                 (SOCKADDR *)&saUdpServer, sizeof(SOCKADDR_IN)
               ); 
   if (err == SOCKET_ERROR)
      CheckLastError( "sendto() failed: %s" );
}

//=============================================================================
//
// WinIPThreadFunc
//
#define MAX_MSGLEN   65536

UINT WinIPThreadFunc( LPVOID pParam )
{
   WinIPPtr       pServer = (WinIPPtr)pParam;
   SOCKADDR_IN    saUdpClient;
   char           achBuffer[MAX_MSGLEN];
   BACnetOctet    *msg, *dst;

   while (pServer->m_Continue)
   {
      // Receive a datagram on the bound port
      int clientSize = sizeof( SOCKADDR_IN );
      int nBytes = recvfrom( pServer->m_socket, achBuffer, MAX_MSGLEN, 0,
                             (SOCKADDR FAR *)&saUdpClient, &clientSize );
      if (nBytes == SOCKET_ERROR) {
         int rxError = GetLastError();
         CString msg;
         GetSystemErrorMsg( msg, "IP receive thread exiting due to error: %s", rxError );
         if (pServer->m_Continue)
         {
            // Unexpected error while running: we are going to break out of the
            // loop, so show a dialog, since we won't receive any more messages.
            CWnd *pMainWindow = AfxGetMainWnd();
            if (pMainWindow)
            {
               pMainWindow->MessageBox( msg );
            }
         }
         else
         {
            // Expected error during shutdown
            TRACE( "%s\n", (LPCTSTR)msg );
         }
         break;
      } else {
         // Allocate a buffer just for the filter
         msg = dst = new BACnetOctet[ nBytes + 6 ];

         // Copy in the source address and data
         *(unsigned long *)dst = *(unsigned long *)&saUdpClient.sin_addr;
         dst += 4;
         *(unsigned short *)dst = saUdpClient.sin_port;
         dst += 2;
         memcpy( dst, achBuffer, nBytes );

         // Let the application filter it
         pServer->FilterData( (unsigned char *)msg, nBytes + 6, BACnetPort::portReceiving );

         delete[] msg;

         // Pass the contents up to the client
         pServer->Response(
            BACnetNPDU( BACnetIPAddr( ntohl( *(unsigned long *)&saUdpClient.sin_addr),
                                      ntohs(saUdpClient.sin_port) ),
                        (unsigned char *)achBuffer, nBytes ) );
      }
   }

   // finished
   TRACE( "IP receive thread exiting\n" );
   return 0;
}

//=============================================================================
// CheckLastError
//
// pTheFormat must have %s to show the text version of the error
//
void CheckLastError( const char *pTheFormat )
{
   int stat = GetLastError();
   if (stat == 0)
      return;

   CString errMsg;
   GetSystemErrorMsg( errMsg, pTheFormat, stat );
   AfxMessageBox( errMsg );
}

//=============================================================================
// Get the Windows message text for nError.
// Also called for MS/TP
//
// pTheFormat must have %s to show the text version of the error
//
const char* GetSystemErrorMsg( CString &theResult, const char *pTheFormat, int nError )
{
   LPTSTR pResult = NULL;
   DWORD retval = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                 FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
                                 nError,
                                 0,                // Any language
                                 (LPTSTR)&pResult, // gets OS-allocated buffer
                                 0,                // no minimum size
                                 NULL );

   // The message has a CRLF at the end.  Kill it
   char *pNewLine = strpbrk( pResult, "\r\n" );
   if (pNewLine)
   {
      *pNewLine = 0;
   }

   theResult.Format( pTheFormat, pResult );
   LocalFree( pResult );
   return theResult;
}
