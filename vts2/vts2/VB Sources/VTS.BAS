Attribute VB_Name = "VTSglobal"
Option Explicit
DefInt A-Z
'------------------------------------------------------------------------------------
'Note:  version edits are marked as ***vxx for version v.xx

Global Const vtsversion = "v2.28"
Global Const PreferencesFile = "VTS.INI"
Global Const RouterAliasFile = "VTSROUTE.INI"
Global Const DestAliasFile = "VTSALIAS.INI"
Global Const BoundAliasFile = "VTSBIND.INI"
Global Const LB_SETTABSTOPS = &H192                         '                   ***222
Global Const NLOOPS = 20                                    'number of loops in timer

Global Const Black = 0
Global Const Grey = &H808080
Global Const Red = &HFF&
Global Const Green = &HFF00&
Global Const Yellow = &HFFFF&
Global Const MinDeviceInstance = 0                          '                   ***209
Global Const KnownObjectTypes = 18

Global Const HourGlass = 13                                 '                   ***218
'Global Const HourGlass = 11
Global Const Normal = 0
Global Const modal = 1
Global Const OFN_FILEMUSTEXIST = &H1000&
Global Const OFN_PATHMUSTEXIST = &H800&
Global Const OFN_HIDEREADONLY = &H4&
Global Const OFN_OVERWRITEPROMPT = &H2&

Global Const propcolon = "(proprietary):"

Global CurrentDisplayFormat                     '0=hex, 1=1 line, 2=verbose
Global BroadcastDA As String * 6
Global VTSdir As String
Global Initialized

'used with BitsForm:
Global BitNames As String
Global BitsValue
Global Bittbl(7)
'used with NumsForm:
Global ParseType
Global NumsFloat As Single
Global NumsString As String
Global NumsInt As Long
'used with DateForm:
Global DateString As String
'used with CalForm:
Global CalString As String
'used with DAForm:
Global DAString As String
'used with PropRefForm:
Global PRString As String
'used with EventParamsForm:
Global EVString As String
'used with SchedForm:
Global SchedString As String
'used with ObjIDForm:
Global ObjIDString As String
'used with PropForm:
Global PropObjectType
Global PropObjectInstance As Long
Global PropPropertyIdentifier As Long
Global PropArrayIndex As Long
'used with EnumForm:
Global EnumValue
Global EnumStart
Global EnumPointer                              'CAUTION: this gets pointer values from VTSAPIgetpropinfo only!
Global EnumMax As Long
'used with StdForm:
Global StdServices As String * 8                '64 bits
Global Const NumServices = 35
'used with NLForm:
Global nlmsg
Global NLpkt As MApacket
'used with TargetForm:
Global RFtop
Global ALpkt As NALSpacket
'used with DeviceForm, PTPForm:
Global DevDo
Global Const ReinitDevice = 0
Global Const DeviceCommunicationControl = 1
Global Const GetAlarmSummary = 2
'used with PropVal form:
Global PVObjectType
Global PVPropVal As String
Global PVNeedsRelation
Global PVPropId
Global PVPropCanChange
Global PVWriteProperty

'used in VTSform Timer routine
Global rnp As NALSpacket
Global rapdu As String * 8192                   'for now

Global Const NumProps = &HFFFF                  'use for propindex in VTSAPIgetpropinfo

Global Const NALSInvalidPort = 1023             '                                   ***213

'---------------------------------------VTSAPI32.DLL----------------------------
Public Declare Function VTSAPIgetpropinfo Lib "VTSAPI32.DLL" (ByVal objtype%, ByVal PropIndex%, ByVal pname$, ptype%, pgroup%, pflags%, pet%) As Long
Public Declare Function VTSAPIgetenumtable Lib "VTSAPI32.DLL" (ByVal wpet, ByVal Index%, propstart%, propmax&, ByVal ptext$)
Public Declare Function VTSAPIgetdefaultparsetype Lib "VTSAPI32.DLL" (ByVal PropId&, ByVal hList)
Public Declare Function VTSAPIgetpropertystates Lib "VTSAPI32.DLL" (ByVal choice%, ByVal hwnd&) ' MAG 26 FEB 2001 change hwnd% to hwnd&
Public Declare Function VTSAPIgetpropertystate Lib "VTSAPI32.DLL" (ByVal choice%, ByVal psenum%, ByVal ptext$)
Public Declare Function VTSAPIgetdefaultpropinfo Lib "VTSAPI32.DLL" (ByVal objtype, ByVal PropId&, ptype, pet)
Public Declare Sub ReadTextPICS Lib "VTSAPI32.DLL" (ByVal tp As String, pd As PicsDB, errc As Integer)
Public Declare Sub DeletePICSObject Lib "VTSAPI32.DLL" (ByVal p As Long)
'---------------------------------------NALSERV32.DLL----------------------------
Public Declare Function NALSAPIinit Lib "NALSERV32.DLL" (ver%)
Public Declare Function NALSAPIgetapdu Lib "NALSERV32.DLL" (n As NALSpacket, ByVal apmax, ByVal apdu$)
Public Declare Function NALSAPIcopyrequest Lib "NALSERV32.DLL" (n As NALSpacket, ByVal apdu$)
Public Declare Function NALSAPItransmit Lib "NALSERV32.DLL" (m As MApacket)
Public Declare Function NALSAPIgetportlist Lib "NALSERV32.DLL" (ByVal pl$)
Public Declare Function NALSAPIgetlog Lib "NALSERV32.DLL" (ByVal l$)
Public Declare Function NALSAPIgetlen Lib "NALSERV32.DLL" (ByVal Port)
Public Declare Function NALSAPIgetmaxresponse Lib "NALSERV32.DLL" (ByVal Port)
Public Declare Sub NALSAPIdeinit Lib "NALSERV32.DLL" ()
Public Declare Sub NALSAPIputlog Lib "NALSERV32.DLL" (ByVal l$)
Public Declare Sub NALSAPIreceivefilter Lib "NALSERV32.DLL" (ByVal filt)
Public Declare Function NALSAPImaxNPDUlength Lib "NALSERV32.DLL" (ByVal Port)
Public Declare Sub NALSAPIanytobyte Lib "NALSERV32.DLL" (ap As Any, bp As Byte, ByVal n%)
Public Declare Sub NALSAPIbytetoany Lib "NALSERV32.DLL" (bp As Byte, ap As Any, ByVal n%)
'------------------------------------------PTP.DLL-------------------------------   ***214
Public Declare Function ptpConnect Lib "ptp.dll" (ByVal pass$) As Long
Public Declare Function ptpDisconnect Lib "ptp.dll" (ByVal reason) As Long
'---------------------------------------BACPRIM32.DLL----------------------------
Public Declare Function eDWORD Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal v&, ByVal tag, ByVal sgnd)
Public Declare Function eENUM Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal v As Long) As Long
Public Declare Function eSubscribeCOVService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal ProcID&, ByVal ObjId&, ByVal Cancel, ByVal conf, ByVal Lifetime&)  '***218
Public Declare Function eCOVNotificationService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal apmax, ByVal conf, ByVal ProcID&, ByVal devinstance&, ByVal obid&, ByVal remaining, ByVal vlist)
Public Declare Function eEventNotificationService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal conf, ByVal ProcID&, ByVal devinstance&, ByVal obid&, ts As etimestamp, ByVal nc, ByVal Pri, ByVal evt, ByVal nt, ByVal Msg$, ByVal ackr, ByVal fst, ByVal tst, nv As notifyvalue, ByVal ps)
Public Declare Function eIAmService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal instance&, ByVal maxapdu, ByVal segm, ByVal vid)
Public Declare Function eIHaveService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal devinstance&, ByVal obid&, ByVal obname$)
Public Declare Function eWhoIsService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal haverange, ByVal rlow&, ByVal rhigh&)
Public Declare Function eWhoHasService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal haverange, ByVal rlow&, ByVal rhigh&, ByVal obid&, ByVal obname$)
Public Declare Function eReadPropService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal obid&, ByVal PropId&, ByVal havearray, ByVal arrindex&)
Public Declare Function eReadPropMultipleService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal nprops, pp As PropList)
Public Declare Function eReadPropConditionalService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal maxsize, ByVal selectlogic, ByVal hcrit, ByVal hrefs)
Public Declare Function eWritePropService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal maxsize, ByVal objtype, ByVal inst&, ByVal pstring$)
Public Declare Function eWritePropMultipleService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal maxsize, ByVal nprops, pp As PropList)
Public Declare Function eReinitDeviceService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal rstate, ByVal Password$)
Public Declare Function eTimeSynchService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal yr, ByVal mon, ByVal dy, ByVal dow, ByVal Hours, ByVal Minutes, ByVal Seconds, ByVal hund)
Public Declare Function eDeviceCommControlService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal Duration, ByVal enable, ByVal Password$)
Public Declare Function eGetAlarmSummaryService Lib "BACPRIM32.DLL" (ByVal apdu$)
Public Declare Function eAcknowledgeAlarmService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal ProcID&, ByVal obid&, ByVal evstate, ts As etimestamp, ByVal acksrc$, ackts As etimestamp)
Public Declare Function eTextMessageService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal apdusize, ByVal conf, ByVal devinst&, ByVal Pri, ByVal class$, ByVal Msg$)
Public Declare Function eAtomicReadFileService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal fins&, ByVal recacc, ByVal fstart&, ByVal cnt&)
Public Declare Function eAtomicWriteFileService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal apdumax, ByVal fins&, ByVal recacc, ByVal fstart&, ByVal HexData, ByVal hwnd&) ' add & to hwnd MAG 26 FEB 2001
Public Declare Function eDeleteObjectService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal ObjId&)
Public Declare Function eCreateObjectService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal apdumax, ByVal ObjId&, ByVal useinstance, ByVal hwnd&) ' add & to hwnd MAG
Public Declare Function eListElementService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal apdumax, ByVal svc, ByVal ObjId&, ByVal PropId&, ByVal needarray, ByVal aindex&, ByVal ptype, ByVal pflags, ByVal ep, ByVal hwnd&) ' add & to hwnd MAG
Public Declare Function eVTOpenService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal class, ByVal LocalSession)
Public Declare Function eVTDataService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal maxsize, ByVal RemoteSession, ByVal DataFlag, ByVal ndata, ByVal vtdata$)
Public Declare Function eVTCloseService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal RemoteSession)
Public Declare Function ePrivateTransferService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal maxsize, ByVal conf, ByVal vid, ByVal svcnum, ByVal nparms, ByVal svcparms$)
Public Declare Function eGetEnrollmentSummaryService Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal ackf, ByVal devinst&, ByVal net&, ByVal mlen, ByVal madr$, ByVal ProcID&, ByVal es, ByVal et, ByVal pfn, ByVal pfx, ByVal nf)
Public Declare Function vbDWORD Lib "BACPRIM32.DLL" (ByVal buf$, ByVal mlen) As Long
Public Declare Function vbLONG Lib "BACPRIM32.DLL" (ByVal buf$, ByVal mlen) As Long
Public Declare Function vbREAL Lib "BACPRIM32.DLL" (ByVal buf$) As Single
Public Declare Function vbDOUBLE Lib "BACPRIM32.DLL" (ByVal buf$) As Double
Public Declare Function vbWORD Lib "BACPRIM32.DLL" (ByVal buf$) As Integer
Public Declare Function vbOBJECTID& Lib "BACPRIM32.DLL" (ByVal obtype, ByVal obinst&)
Public Declare Sub vbeREAL Lib "BACPRIM32.DLL" (ByVal buf$, ByVal v As Single) ' MAG 30 JAN 01 v2.26 changed vbeReal to vbeREAL -fix script error
Public Declare Sub vbeDOUBLE Lib "BACPRIM32.DLL" (ByVal buf$, ByVal v As Double)
Public Declare Function eSimpleAckPDU Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal id As Byte, ByVal Service As Byte)
Public Declare Function eErrorPDU Lib "BACPRIM32.DLL" (ByVal apdu$, ByVal id, ByVal eClass As Long, ByVal eCode As Long, ByVal eService As Long)        '***209
'---------------------------------------HTML Help-------------------------------
Public Declare Function HtmlHelp Lib "hhctrl.ocx" Alias "HtmlHelpA" (ByVal hwnd As Long, ByVal hfile$, ByVal CMD As Long, ByVal dw As Long) As Long
'---------------------------------------Windows---------------------------------
Public Declare Function GetWindowsDirectory Lib "kernel32" Alias "GetWindowsDirectoryA" (ByVal lpBuffer As String, ByVal nSize As Long) As Long
Public Declare Function SendMessageArray Lib "user32" Alias "SendMessageA" (ByVal hwnd As Long, ByVal wMsg As Long, ByVal wParam As Long, lParam As Any) As Long
Public Declare Function LoadLibrary Lib "kernel32" Alias "LoadLibraryA" (ByVal lpLibFileName As String) As Long
Public Declare Function GetPrivateProfileInt Lib "kernel32" Alias "GetPrivateProfileIntA" (ByVal lpApplicationName As String, ByVal lpKeyName As String, ByVal nDefault As Long, ByVal lpFileName As String) As Long
Public Declare Function GetPrivateProfileString Lib "kernel32" Alias "GetPrivateProfileStringA" (ByVal lpApplicationName As String, ByVal lpKeyName As Any, ByVal lpDefault As String, ByVal lpReturnedString As String, ByVal nSize As Long, ByVal lpFileName As String) As Long
Public Declare Function WritePrivateProfileString Lib "kernel32" Alias "WritePrivateProfileStringA" (ByVal lpApplicationName As String, ByVal lpKeyName As Any, ByVal lpString As Any, ByVal lpFileName As String) As Long
Public Declare Function GetDeviceCaps Lib "gdi32" (ByVal hdc As Long, ByVal nIndex As Long) As Long
'---------------------------------------DUDAPI32.DLL----------------------------
Public Declare Sub MyDeletePICSObject Lib "dudapi32.DLL" (ByVal p As Long)
Public Declare Function LoadStringRes Lib "dudapi32.DLL" (ByVal id As Integer, ByVal Buffer As String, ByVal Size As Integer) As Integer
Public Declare Sub DrawStdIcon Lib "dudapi32.DLL" (ByVal X As Integer, ByVal y As Integer, ByVal hw As Long, ByVal id As Integer)
Public Declare Sub CpyGenObj Lib "dudapi32.DLL" (GenObj As generic_object, ByVal p As Long)
Public Declare Function GetNextGenObj Lib "dudapi32.DLL" (GenObj As generic_object) As Integer
Public Declare Function FindGenObj Lib "dudapi32.DLL" (ByVal GenObj As Long, ByVal Index As Integer) As Long
Public Declare Function GetPropNameSupported Lib "dudapi32.DLL" (ByVal PropName As String, ByVal i As Integer, ByVal objtype As Integer, ByVal propFlags As String, PropId As Long) As Integer
Public Declare Function GetPropName Lib "dudapi32.DLL" (ByVal PropId As Long, ByVal PropName As String) As Integer
Public Declare Function GetPropValue Lib "dudapi32.DLL" (ByVal Buffer As String, Msg As PVMessage) As Integer
Public Declare Function GetObjType Lib "dudapi32.DLL" (ByVal objtype As Integer, ByVal Buffer As String) As Integer
Public Declare Function GetEnumName Lib "dudapi32.DLL" (ByVal Buffer As String, ByVal objtype As Integer, ByVal PropId As Long, ByVal EnumVal As Integer) As Integer
Public Declare Function GetpObj Lib "dudapi32.DLL" (ByVal pdb As Long, ByVal ObjectId As Long) As Long
Public Declare Function CheckObjTypeDevPics Lib "dudapi32.DLL" (ByVal StdObj As String, ByVal pdbRoot As Long, ByVal resObjDev As String, ByVal resObjPICS As String) As Integer
Public Declare Function CheckConfClass Lib "dudapi32.DLL" (ByVal ConfClass As Integer, ByVal ApplServ As String, ByVal Result As String, ByVal root As Long, ByVal resObj As String, eol As Integer) As Integer
Public Declare Function CheckFunctionalGroup Lib "dudapi32.DLL" (ByVal FuncGroup As Long, ByVal ApplServ As String, ByVal Result As String, ByVal root As Long, ByVal resObj As String, eol As Integer) As Integer
Public Declare Function CMyInt Lib "dudapi32.DLL" (ByVal sInt As String) As Integer
Public Declare Function CMyBigInt Lib "dudapi32.DLL" (ByVal sInt As String) As Integer
Public Declare Function CMyBigLong Lib "dudapi32.DLL" (ByVal sLong As String) As Long
Public Declare Function DevApplServCheck Lib "dudapi32.DLL" (ByVal ApplServ As String, ByVal root As Long, ByVal resApplServ As String) As Integer
Public Declare Function GetObjIdRoot Lib "dudapi32.DLL" (ByVal pdbRoot As Long) As Long
Public Declare Function pIDinList Lib "dudapi32.DLL" (ByVal pdbRoot As Long, pid As Long, ObjId As Long) As Integer
Public Declare Function pDBinList Lib "dudapi32.DLL" (ByVal pidRoot As Long, pdb As Long, ObjId As Long) As Integer
Public Declare Sub SplitObjectId Lib "dudapi32.DLL" (ByVal ObjId As Long, objtype As Integer, ObjInst As Long)
Public Declare Sub InitDudapi Lib "dudapi32.DLL" ()
Public Declare Sub CloseDudapi Lib "dudapi32.DLL" ()

'select first item in a list or combo if list is non-empty
'in:  lb  is a listbox or combo
Public Sub SelectFirst(lb As Control)
  If lb.ListCount > 0 Then lb.ListIndex = 0
End Sub

Public Function GetPref(ByVal s$, ByVal k$) As String
Dim r, b As String * 128
  r = GetPrivateProfileString(s$, k$, "", b, 128, PreferencesFile)
  GetPref = Left$(b, r)
End Function

Public Sub FindDLL(ByVal d$)
Dim r As Long, l$
  l$ = VTSdir + d$
  r = LoadLibrary(l$)
  If r = 0 Then
    Beep
    MsgBox "Can't Load DLL [" + l$ + "]", 16, "DLL Load Failed"
    End
  End If
End Sub

'perform shutdown and clean up
Sub ExitVTS()
  VTSform.StatusList.Clear
  VTSform.StatusList.AddItem "Shutting down..."
  VTSform.StatusList.Refresh
  PicsDeInit                'remove all dynamic data
  NALSAPIdeinit
  End
End Sub

'select all the text in a TextBox (used for GotFocus events)
Sub GoSelect(tb As Control)
  tb.SelStart = 0
  If Len(tb.Text) < 0 Then
    tb.SelLength = 0
  Else
    tb.SelLength = Len(tb.Text)
  End If
End Sub

'Undo a ShowBimodal
Sub HideBiModal(f As Form)
  VTSform.Enabled = True
  f.Hide
  If VTSform.ViewTargetDevice.Checked Then
    TargetForm.Show
  Else
    TargetForm.Hide
  End If
End Sub

'Global Initialization goes here
Sub Main()
Dim i, j, w$
  If App.PrevInstance Then
    Beep
    MsgBox "You can only run one instance of VTS at a time!", vbCritical
    End
  End If
  j = 1
  For i = 0 To 7
    Bittbl(i) = j
    j = j * 2
  Next i
  
  w$ = Space$(144)                      'make room for it
  If GetWindowsDirectory(w$, 144) = 0 Then
    MsgBox "Unable to determine Windows directory", 16
    End
  End If
  i = InStr(w$, Chr$(0))
  If i Then w$ = Left$(w$, i - 1)
  w$ = LTrim$(RTrim$(w$))
  
  VTSdir = GetPref("General", "DLLs")
  If VTSdir = "" Then VTSdir = w$
  If Right$(VTSdir, 1) <> "\" Then VTSdir = VTSdir + "\"
  FindDLL "VTSAPI32.DLL"
  FindDLL "BACPRIM32.DLL"
  FindDLL "PTP.DLL"
  FindDLL "MSTP.DLL"
  FindDLL "NALSERV32.DLL"
  FindDLL "DUDAPI32.DLL"
  
  i = NALSAPIinit(j)
  If i <> NALSE_Success Then
    MsgBox "NALSAPI failed initialization with error" + Str$(i), 16, "Init Error"
    End
  End If

  InitInterpreter
  InitSBT                                       'init script tests
  PicsInit                                      'init PICS tests
  VTSform.Show
End Sub

'return a string describing a NALSERV or MACAPI error
Function NALSerror$(e)
Dim t$
  Select Case e
    Case NALSE_Success
      t$ = "Success"
    Case MAE_NoFreeBuffers
      t$ = "No Free Buffers"
    Case MAE_LSAPinUse
      t$ = "LSAP in Use"
    Case MAE_NoFreeHooks
      t$ = "No Free Hooks"
    Case MAE_LSLNotFound
      t$ = "Can't Find LSL"
    Case MAE_NotEnoughDOSMemory
      t$ = "Not Enough DOS Global Memory"
    Case MAE_CantGetSelector
      t$ = "Can't Get Selector"
    Case MAE_NotYetInitialized
      t$ = "MACAPI Not Yet Initialized"
    Case MAE_CantRegisterPID
      t$ = "Can't Register Protocol ID"
    Case MAE_LSAPNotHooked
      t$ = "LSAP Not Hooked"
    Case NALSE_NoFreeTimers
      t$ = "No Free Timers"
    Case NALSE_CantCreateWindow
      t$ = "Can't Create Window"
    Case NALSE_FailedClassRegistration
      t$ = "Failed Class Registration"
    Case NALSE_NLserverAlreadyRegistered
      t$ = "NLserver Already Registered"
    Case NALSE_ALserverAlreadyRegistered
      t$ = "ALserver Already Registered"
    Case NALSE_IncompatibleMACAPI
      t$ = "Incompatible MACAPI"
    Case NALSE_UnknownPDUType
      t$ = "Unknown PDU Type"
    Case NALSE_InvalidPDUType
      t$ = "Invalid PDU Type"
    Case NALSE_InvalidRequestThisState
      t$ = "Invalid Request for This State"
    Case NALSE_NoFreeSpaceAvailable
      t$ = "No Free Space Available"
    Case NALSE_NoFreeInvokeIDs
      t$ = "No Free Invoke IDs"
    Case NALSE_Timeout
      t$ = "Timeout"
    Case NALSE_ReceivedPacketTooLarge
      t$ = "Received Packet Too Large"
    Case NALSE_TransmitError
      t$ = "Transmit Failed"
    Case NALSE_InvalidDestinationAddr
      t$ = "Invalid Destination Address"
    Case NALSE_InvalidResponse
      t$ = "Invalid Response"
    Case NALSE_AbortReceived
      t$ = "Abort PDU Received"
    Case NALSE_RejectReceived
      t$ = "Reject PDU Received"
    Case NALSE_ErrorReceived
      t$ = "Error PDU Received"
    Case NALSE_NoMoreSegments
      t$ = "No More Segments"
    Case NALSE_CannotSendSegmentedCACK
      t$ = "Can't Send Segmented Complex Ack"
    Case NALSE_TransmitPacketTooLarge
      t$ = "Transmit Packet Too Large"
    Case NALSE_InhibitTX                                    '   ***209
      t$ = "Transmit Inhibited"                             '   ***209
    Case Else
      If e < 1000 Then t$ = "Unknown MACAPI " Else t$ = "Unknown NALSERV "
      t$ = t$ + "Error" + Str$(e)
  End Select
  NALSerror$ = t$
End Function

Function NoSep(ByVal p$) As String
Dim c, r$, i
  r$ = ""
  For i = 1 To Len(p$)
    c = Asc(Mid$(p$, i, 1))
    If (c <> 13) And (c <> 32) Then r$ = r$ + Chr$(c)
  Next i
  NoSep = r$
End Function

'Return the next comma-delimited string
Function Nxt$(l$)
Dim i
  i = InStr(l$, ",")
  If i = 0 Then                 'no comma or string empty
    Nxt$ = l$
    l$ = ""                     'so empty the string
  Else
    Nxt$ = Left$(l$, i - 1)
    l$ = Mid$(l$, i + 1)
  End If
End Function

'Return the next c$-delimited string
Function NxtC$(l$, c$)
Dim i
  i = InStr(l$, c$)
  If i = 0 Then                 'no comma or string empty
    NxtC$ = l$
    l$ = ""                     'so empty the string
  Else
    NxtC$ = Left$(l$, i - 1)
    l$ = Mid$(l$, i + Len(c$))
  End If
End Function

Sub ReadAliases(alist As Control, af$)
Dim f, l$, ma$
  alist.Clear
  On Error Resume Next
  Err = 0
  f = FreeFile
  Open VTSdir + af$ For Input As f
  If Err = 0 Then
    Do Until EOF(f)
      Line Input #f, l$
      'macaddr,alias description
      ma$ = Nxt(l$)                     'get macaddress
      alist.AddItem l$ + Chr$(9) + ma$
    Loop
  End If
  Close f
End Sub

Sub SaveAliases(alist As Control, af$)
Dim i, f, l$, d$
  On Error Resume Next
  Err = 0
  f = FreeFile
  d$ = VTSdir + af$
  Open d$ For Output As f
  If Err = 0 Then
    For i = 0 To alist.ListCount - 1
      l$ = alist.List(i)
      d$ = NxtC(l$, Chr$(9))
      'macaddr,alias description
      Print #f, l$; ","; d$
    Next i
  Else
    Beep
    MsgBox "Can't Save Aliases File " + d$ + Chr$(13) + Chr$(10) + Error$(Err), 48, "Save Aliases Failed"
  End If
  Close f
End Sub

Sub SendNPDU(pdu As String, ByVal reply)
Dim i, j, p, ul, u$
  p = Val(Left$(TargetForm.Port.Text, 3))
  ALpkt.nalsLport = Chr$(p)                     'get port number
  ALpkt.nalsExpectReply = reply                 'do we expect a reply?
  ALpkt.nalsRequestorisserver = False
  ALpkt.nalsPri = TargetForm.priority.ListIndex

'initialize dest address
  If TargetForm.BroadcastDest.Value Then        'it's broadcast on the end network
    ALpkt.nalsDlen = Chr$(0)                    'Dlen=0 means broadcast, in this case Dadr is not initialized
  Else                                          'it's specific on the end network
    j = Len(TargetForm.DestMACAddress.Text)
    If j > 16 Then
      Beep
      MsgBox "The Target Destination MAC Address is longer than the maximum allowed 8 octets!", 48, "Dest MAC Address Too Long"
      Exit Sub
    End If
    If j And 1 Then                             'round up to even number of digits
      u$ = "0" + TargetForm.DestMACAddress.Text
      j = j + 1
    Else
      u$ = TargetForm.DestMACAddress.Text
    End If
    ALpkt.nalsDlen = Chr$(j / 2)
    For i = 1 To j / 2
      Mid$(ALpkt.nalsDadr, i, 1) = Chr$(Val("&h" + Mid$(u$, 1 + ((i - 1) * 2), 2)))
    Next i
  End If
  'MsgBox "nalsDadr = " & ALpkt.nalsDadr
  
  If TargetForm.LocalRoute.Value Then           'just send this locally
    ALpkt.nalsDnet = 0                          'Dnet=0 means local NPDU
  ElseIf TargetForm.RemoteRoute.Value Then      'must be routed
    i = Val(TargetForm.Network.Text)
    If i = 0 Then
      Beep
      If MsgBox("Although you've selected Remote Routing, the Network number is zero which signifies local transmission without routing. If you proceed, then the message will only be transmitted on the local port.", 33, "No Network to Route To") <> 1 Then Exit Sub
    ElseIf i = &HFFFF Then
      Beep
      If MsgBox("Although you've selected Remote Routing, the Network number is 65535(0xFFFF) which signifies global broadcast on all DNETs. If you proceed, then the message will be broadcast on all ports.", 33, "Global Broadcast") <> 1 Then Exit Sub
    End If
    ALpkt.nalsDnet = i
    If (i <> 0) And (i <> &HFFFF) Then          'get routing parameters
      If TargetForm.RouterUnknown.Value Then    'must broadcast to the router
        ALpkt.nalsRlen = Chr$(0)                'Rlen=0 means broadcast locally, in this case Radr is unused
      Else                                      'use specified router MAC address
        ul = NALSAPIgetlen(p)
        ALpkt.nalsRlen = Chr$(ul)               'get used length for port
        If ul > 1 Then u$ = " octets" Else u$ = " octet"
        j = Len(TargetForm.RouterMACAddress.Text)
        If j > (ul * 2) Then
          Beep
          MsgBox "The port [" + TargetForm.Port.Text + "] uses MAC addresses which are" + Str$(ul) + u$ + " long. You must use a shorter Router MAC address!", 48, "MAC Address Too Long"
          Exit Sub
        End If
        If j And 1 Then
          u$ = "0" + TargetForm.RouterMACAddress.Text
          j = j + 1
        Else
          u$ = TargetForm.RouterMACAddress.Text
        End If
        For i = 1 To j / 2
          Mid$(ALpkt.nalsRadr, i, 1) = Chr$(Val("&h" + Mid$(u$, 1 + ((i - 1) * 2), 2)))
        Next i
      End If
    End If
  Else                                          'global
    ALpkt.nalsDnet = &HFFFF                     'Dnet=FFFF means global broadcast
  End If
  ALpkt.nalsAlen = Len(pdu)
  If TargetForm.IncludeSourceRouting.Value Then
    ALpkt.nalsSnet = Val(TargetForm.SourceNetwork.Text)
    j = Len(TargetForm.SourceMACAddress.Text)
    If j And 1 Then
      j = j + 1
      u$ = "0" + TargetForm.SourceMACAddress.Text
    Else
      u$ = TargetForm.SourceMACAddress.Text
    End If
    ALpkt.nalsSlen = Chr$(j / 2)
    For i = 1 To j / 2
      Mid$(ALpkt.nalsSadr, i, 1) = Chr$(Val("&h" + Mid$(u$, 1 + ((i - 1) * 2), 2)))
    Next i
  Else                                          'no source routing
    ALpkt.nalsSlen = Chr$(0)
  End If
  Screen.MousePointer = HourGlass
  apduinuse = True
    apdu = pdu
    ShowBiModal npview
    shownp ALpkt  ' MAG 03 MAY 2001
    'npview.shownp (ALpkt)

    i = NALSAPIcopyrequest(ALpkt, apdu)         'try to send it
    If (Asc(Left$(apdu, 1)) And &HF0) = 0 Then  'this was a Confirmed Request PDU
      Mid$(apdu, 2, 1) = Chr$(NALSAPIgetmaxresponse(Asc(ALpkt.nalsLport)))
      Mid$(apdu, 3, 1) = ALpkt.nalsInvokeID     'get returned invoke ID
    End If
    InterpretNPDU ALpkt, TraceForm.TxList, TraceForm.TxHex
    InterpretAPDU TraceForm.TxList, TraceForm.TxHex
    'special handler for PICS based tests
    If gPicsSend Then                           'filter special apdus
      If i = 0 Then
        PicsInterpretApdu ALpkt, apdu
        gPicsSend = False
      End If
    End If
  apduinuse = False
  Screen.MousePointer = Normal
  If i Then                                     'an error occurred
    MsgBox "Because: " + NALSerror$(i), 48, "Transmit Error"
  End If
End Sub

'Show TargetForm and another form while disabling VTSform
'(sort of like a Show Modal, but allowing TargetForm to remain active)
Sub ShowBiModal(f As Form)
  VTSform.Enabled = False
  TargetForm.Show
  f.Show
End Sub

Public Sub ReadVendors(vlist As Control)
Dim i, t$, maxv                                 '                       ***218 Begin
  vlist.Clear
  vlist.AddItem "(Unknown Vendor ID)"
  maxv = Val(GetPref("VendorID", "MaxID"))      'find out max used ID number
  For i = 0 To maxv
    t$ = Trim$(GetPref("VendorID", Format$(i)))
    If t$ <> "" Then
      vlist.AddItem t$
      vlist.ItemData(vlist.NewIndex) = i
    End If
  Next i
End Sub                                         '                       ***218 End

Sub shownp(np As NALSpacket)
Dim op$, i%, op2$

    ShowBiModal npview
    'npview.shownp (np)
    npview.npout.Clear
    npview.npout.AddItem ("nalsNext = " & np.nalsNext)
    npview.npout.AddItem ("nalsExpectReply = " & np.nalsExpectReply)
    npview.npout.AddItem ("nalsRequestorisserver = " & np.nalsRequestorisserver)
    npview.npout.AddItem ("nalsBroadcast = " & np.nalsBroadcast)
    npview.npout.AddItem ("nalsRxType = " & Asc(np.nalsRxType))
    npview.npout.AddItem ("nalsInvokeID = " & Asc(np.nalsInvokeID))
    npview.npout.AddItem ("nalsRxFlag = " & np.nalsRxFlag)
    npview.npout.AddItem ("nalsPri = " & np.nalsPri)
    npview.npout.AddItem ("nalsDnet = " & np.nalsDnet)
    npview.npout.AddItem ("nalsDlen = " & Asc(np.nalsDlen))
    If (Asc(np.nalsDlen) > 0) Then
        op = ""
        For i = 1 To Asc(np.nalsDlen)
            op2$ = Hex(Asc(Mid$(np.nalsDadr, i, 1))) & " "
            If Len(op2) < 3 Then op2 = "0" & op2
            op$ = op$ & op2$
            'op$ = op$ & Hex$(asc(Mid$(np.nalsDadr, i * 2 - 1, 1)) * 16 + asc(Mid$(np.nalsDadr, i * 2, 1)))
        Next i
        npview.npout.AddItem ("nalsDadr = " & op)
    End If
    npview.npout.AddItem ("nalsRlen = " & Asc(np.nalsRlen))
    If (Asc(np.nalsRlen) > 0) Then
        op = ""
        For i = 1 To Asc(np.nalsRlen)
            op2 = Hex(Asc(Mid$(np.nalsRadr, i, 1))) & " "
            If Len(op2) < 3 Then op2 = "0" & op2
            op$ = op$ & op2$
        Next i
        npview.npout.AddItem ("nalsRadr = " & op)
    End If
    'npview.npout.AddItem ("nalsRadr = " & np.nalsRadr)
    npview.npout.AddItem ("nalsSnet = " & np.nalsSnet)
    npview.npout.AddItem ("nalsSlen = " & Asc(np.nalsSlen))
    If (Asc(np.nalsSlen) > 0) Then
        op = ""
        For i = 1 To Asc(np.nalsSlen)
            op2 = Hex(Asc(Mid$(np.nalsSadr, i, 1))) & " "
            If Len(op2) < 3 Then op2 = "0" & op2
            op$ = op$ & op2$
        Next i
        npview.npout.AddItem ("nalsSadr = " & op)
    End If
    'npview.npout.AddItem ("nalsSadr = " & np.nalsSadr)
    npview.npout.AddItem ("nalsLport = " & Asc(np.nalsLport))
    npview.npout.AddItem ("nalsAlen = " & np.nalsAlen)
    If (np.nalsAlen > 0) Then
        op = ""
        For i = 1 To np.nalsAlen
            op2 = Hex(Asc(Mid$(apdu, i, 1))) & " "
            If Len(op2) < 3 Then op2 = "0" & op2
            op$ = op$ & op2$
        Next i
        npview.npout.AddItem ("nalsAPDU = " & op)
    End If
    'npview.npout.AddItem ("nalsAPDU = " & np.nalsAPDU)
  
  'nalsNext As Long                              'linked list pointer
  'nalsExpectReply As Long                       'C Booleans are 32-bit
  'nalsRequestorisserver As Long                 'C Booleans are 32-bit
  'nalsBroadcast As Long                         'C Booleans are 32-bit
  'nalsRxType As String * 1
  'nalsInvokeID As String * 1
  'nalsRxFlag As Integer
  'nalsPri As Integer
  'nalsDnet As Integer
  'nalsDlen As String * 1
  'nalsDadr As String * 8                        'octet string destination address
  'nalsRlen As String * 1
  'nalsRadr As String * 8                        'octet string router address
  'nalsSnet As Integer
  'nalsSlen As String * 1
  'nalsSadr As String * 8                        'octet string source address
  'nalsLport As String * 1                       'the logical port
  'nalsAlen As Integer                           'apdu length
  'nalsAPDU As Long                              'pointer to APDU

End Sub

