Attribute VB_Name = "NTRACE"
DefInt A-Z
Option Explicit

Global Const MaxTraceLines = 500
Global Const sep = "---------------------------------"
Global apdu As String
Global apduinuse
Global SavedHex As String
Global IndentLevel
Global PropName(124) As String
Global Const KnownProps = 124
Global Const MaxPDNesting = 8
Global PDstack()
Global CurrentLine As Long
Global enumx(8) As String
Global TraceList As Control, TraceHex As Control
Global LastObjType

'These are flags returned for properties by VTSAPIgetpropinfo:
Global Const propIsArray = &H100
Global Const propIsCommandable = &H200

'These are ParseType values from VTSAPI's STDOBJPR.H:
Global Const ptObjID = 0                'an object identifier
Global Const ptET = 1                   'an enumeration table
Global Const ptFloat = 2                'float
Global Const ptBits = 3                 'octet of T or F flags
Global Const ptUW = 4                   'unsigned word
Global Const ptUD = 5                   'unsigned dword
Global Const ptSW = 6                   'signed word
Global Const ptU127 = 7                 '1..127
Global Const ptS10 = 8                  'char [10]
Global Const ptS32 = 9                  'char [32]
Global Const ptS64 = 10                 'char [64]
Global Const ptS132 = 11                'char [132]
Global Const ptPropref = 12             'obj property reference
Global Const ptDT = 13                  'date/time
Global Const ptCalist = 14              'list of calendarentry
Global Const ptAct = 15                 'action array
Global Const ptActext = 16              'action_text array
Global Const ptPss = 17                 'protocol services supported bitstring
Global Const ptPos = 18                 'protocol objects supported bitstring
Global Const ptVTcl = 19                'vt classes
Global Const ptEvparm = 20              'event parameter
Global Const ptSkeys = 21               'session keys
Global Const ptTsrecip = 22             'time synch recipients
Global Const ptDabind = 23              'device address bindings
Global Const ptRecip = 24               'recipient
Global Const ptPaf = 25                 'priority array flt
Global Const ptStatext = 26             'state text array
Global Const ptPab = 27                 'priority array bpv
Global Const ptPau = 28                 'priority array uw
Global Const ptReciplist = 29           'list of BACnetDestination
Global Const ptXsched = 30              'exception schedule: array[] of specialevent
Global Const ptWsched = 31              'weekly schedule: array[7] of list of timevalue
Global Const ptU16 = 32                 '1..16
Global Const ptSetref = 33              'setpoint reference
Global Const ptRaslist = 34             'list of readaccessspecs
Global Const ptStavals = 35             'list of unsigned
Global Const ptDtrange = 36             'range of dates
Global Const ptLopref = 37              'list of object prop refs
Global Const ptEbool = 38               'boolean
Global Const ptD = 39                   'date
Global Const ptT = 40                   'time
Global Const ptLooref = 41              'list of objectid references MAG exists in original VTSAPI32, but not here
Global Const ptSsint = 42               'signed short int MAG 13 FEB 2001
Global Const ptNone = 255               'no parsing required, runtime generated value

Global Const Min_NPDU_Length = 2
Global Const BroadcastDNET = &HFFFF
Global Const BACnetLSAP = &H82
Global Const BACnetVersion = 1

'NLMsg types
Global Const Who_Is_Router_To_Network = 0
Global Const I_Am_Router_To_Network = 1
Global Const I_Could_Be_Router_To_Network = 2
Global Const Reject_Message_To_Network = 3
Global Const Router_Busy_To_Network = 4
Global Const Router_Available_To_Network = 5
Global Const Initialize_Routing_Table = 6
Global Const Initialize_Routing_Table_Ack = 7
Global Const Establish_Connection_To_Network = 8
Global Const Disconnect_Connection_To_Network = 9
                
Global Const NORMAL_MESSAGE = 0
Global Const URGENT_MESSAGE = 1
Global Const CRITICAL_EQUIPMENT_MESSAGE = 2
Global Const LIFE_SAFETY_MESSAGE = 3

'Object Types
Global Const Device_Object = 8

Type notifyvalue
  nvFlags As Integer                            'status flags
  nvNumBits As Integer
  nvEnum(0 To 1) As Integer
  nvBits As Long
  nvReal(0 To 2) As Single
End Type

Type etimestamp
  tsType As Integer                             '0=time, 1=sequence number, 2=date/time
  tsYear As Integer
  tsMonth As Integer
  tsDay As Integer
  tsDow As Integer
  tsHour As Integer
  tsMinute As Integer
  tsSecond As Integer
  tsHundredth As Integer
End Type

Type MApacket
  maLink As Long
  maTrace As Long                               'packet sequence identifier
  maLport As String * 1                         'the logical port within LSL to/from
  maAlen As String * 1                          'the length of sadr and dadr really used
  maLsap As String * 1                          'the link service access point
  maBroadcast As String * 1                     '0=no, 1=yes
  maReserved As String * 6                      'reserved
  maDadr As String * 8                          'octet string destination address
  maSadr As String * 8                          'octet string source address
  maNlen As Integer                             'length of maNPDU in octets
  maNPDU As String * 1497                       'max sized NPDU
  mapad2 As String * 3                          'pad
End Type

Type NALSpacket
  nalsNext As Long                              'linked list pointer
  nalsExpectReply As Long                       'C Booleans are 32-bit
  nalsRequestorisserver As Long                 'C Booleans are 32-bit
  nalsBroadcast As Long                         'C Booleans are 32-bit
  nalsRxType As String * 1
  nalsInvokeID As String * 1
  nalsRxFlag As Integer
  nalsPri As Integer
  nalsDnet As Integer
  nalsDlen As String * 1
  nalsDadr As String * 8                        'octet string destination address
  nalsRlen As String * 1
  nalsRadr As String * 8                        'octet string router address
  nalsSnet As Integer
  nalsSlen As String * 1
  nalsSadr As String * 8                        'octet string source address
  nalsLport As String * 1                       'the logical port
  nalsAlen As Integer                           'apdu length
  nalsAPDU As Long                              'pointer to APDU
End Type

Type NPCI
  npciVersion   As String * 1
  npciControl   As String * 1
  npciDNET      As Integer
  npciDLEN      As String * 1
  npciDADR      As String * 8
End Type

Type PropList
  plObjectID    As Long
  plPropID      As Long
  plArrayIndex  As Long
'  plPropVal     As String * 256
  plPropVal(0 To 255)     As Byte               'note: this is in actuality a string!!!!!! (See horseshit comment in read property form)
End Type
        
Type defaultpt
  dptype        As Integer
  dep           As Integer
  dpflags       As Integer
End Type

Global Const NSDUisNLmessage = &H80             '1==NL message, 0=APDU
Global Const NSDUhasDNET = &H20                 '1==DNET, DLEN, HopCount present
Global Const NSDUhasSNET = &H8                  '1==SNET, SLEN present
Global Const NSDUexpectsreply = &H4
Global Const NSDUpriority = &H3                 'mask for priority field

'MACAPI Errors
Global Const MAE_Success = 0
Global Const MAE_NoFreeBuffers = 1
Global Const MAE_LSAPinUse = 2
Global Const MAE_NoFreeHooks = 3
Global Const MAE_LSLNotFound = 4
Global Const MAE_NotEnoughDOSMemory = 5
Global Const MAE_CantGetSelector = 6
Global Const MAE_NotYetInitialized = 7
Global Const MAE_CantRegisterPID = 8
Global Const MAE_LSAPNotHooked = 9

'NALSAPI errors
Global Const NALSE_Success = 0
Global Const NALSE_NoFreeTimers = 1000
Global Const NALSE_CantCreateWindow = 1001
Global Const NALSE_FailedClassRegistration = 1002
Global Const NALSE_NLserverAlreadyRegistered = 1003
Global Const NALSE_ALserverAlreadyRegistered = 1004
Global Const NALSE_IncompatibleMACAPI = 1005
Global Const NALSE_UnknownPDUType = 1006
Global Const NALSE_InvalidPDUType = 1007
Global Const NALSE_InvalidRequestThisState = 1008
Global Const NALSE_NoFreeSpaceAvailable = 1009
Global Const NALSE_NoFreeInvokeIDs = 1010
Global Const NALSE_Timeout = 1011
Global Const NALSE_ReceivedPacketTooLarge = 1012
Global Const NALSE_TransmitError = 1013
Global Const NALSE_InvalidDestinationAddr = 1014
Global Const NALSE_InvalidResponse = 1015
Global Const NALSE_AbortReceived = 1016
Global Const NALSE_RejectReceived = 1017
Global Const NALSE_ErrorReceived = 1018
Global Const NALSE_NoMoreSegments = 1019
Global Const NALSE_CannotSendSegmentedCACK = 1020
Global Const NALSE_TransmitPacketTooLarge = 1021
Global Const NALSE_NoPacketsAvailable = 1022
Global Const NALSE_InhibitTX = 1998                                 '   ***209

Sub AbortReason(pdutype)
Dim c$, r As Long
  If pdutype And 1 Then c$ = "Abort from Server: invID=" Else c$ = "Abort from Client: invID="
  c$ = c$ + Format$(dByte(2)) + ", "
  If Len(apdu) = 3 Then                        'not using proprietary abort
    r = dByte(3)
  Else
    r = dWord(3)
  End If
  Select Case r
    Case 0
      c$ = c$ + "other"
    Case 1
      c$ = c$ + "buffer-overflow"
    Case 2
      c$ = c$ + "invalid-apdu-in-this-state"
    Case 3
      c$ = c$ + "preempted-by-higher-priority-task"
    Case 4
      c$ = c$ + "segmentation-not-supported"
    Case 5 - 63
      c$ = c$ + "BACnet reserved reason" + Str$(r)
    Case Else
      c$ = c$ + "proprietary reason" + Str$(r)
  End Select
  Ptrace c$
  Phex 1, Len(apdu)
End Sub

Sub AppendTrace(ByVal s$)
Dim i
  i = TraceList.ListCount - 1
  TraceList.List(i) = TraceList.List(i) + s$
End Sub

Function BACnetDate(aoff) As String
Dim y, yr$, m, d, dn$, dw
  y = dByte(aoff)
  If y = 255 Then yr$ = "any year" Else yr$ = Format$(1900 + y)
  m = dByte(aoff + 1)
  d = dByte(aoff + 2)
  If d = 255 Then dn$ = " any day" Else dn$ = Str$(d)
  dw = dByte(aoff + 3)
  BACnetDate = "(" + MonthName(m) + dn$ + ", " + yr$ + " " + DayName(dw) + ")"
End Function

Function BACnetWeekNDay(aoff) As String
Dim m, w, wn$, dw
  m = dByte(aoff)
  w = dByte(aoff + 1)
  Select Case w
    Case 1
      wn$ = "1st week"
    Case 2
      wn$ = "2nd week"
    Case 3
      wn$ = "3rd week"
    Case 4
      wn$ = "4th week"
    Case 5
      wn$ = "days 29-31"
    Case 6
      wn$ = "last week of the month"
    Case 255
      wn$ = " any week"
    Case Else
      wn$ = "invalid week (" + Format$(w) + ")"
  End Select
  dw = dByte(aoff + 2)
  BACnetWeekNDay = "(" + MonthName(m) + "," + wn$ + ", " + DayName(dw) + ")"
End Function

Sub BeginLevel(c$)
  If c$ = "" Then
    Ptrace LevelPrefix() + "("
  Else
    Ptrace LevelPrefix() + c$ + "=("
  End If
  IndentLevel = IndentLevel + 1
  Phex 0, 0
End Sub

Sub ClearRxTrace()
  TraceForm.RxList.Clear
  TraceForm.RxHex.Clear
  TraceForm.Show
End Sub

Sub ClearTxTrace()
  TraceForm.TxList.Clear
  TraceForm.TxHex.Clear
  TraceForm.Show
End Sub

Sub ComplexACKBody(svc, i)
Dim j, et, c$, t$, rc As Long, PropId As Long, ari& '                 ***218
  Select Case svc
    Case 3                                      'getAlarmSummary
      Do While i < Len(apdu)
        If dObjectIdentifier(-1, i, "objectIdentifier") Then Exit Sub
        If dEventState(-1, i, "alarmState") Then Exit Sub
        If dWord(i) <> &H8205 Then                'better be a bitstring with 5 bits leftover
          Ptrace "? Expected BitString(X'82') tag here"
          Perror i
          Exit Sub
        End If
        j = dByte(i + 2)                        'get eventtransitionbits
        c$ = LevelPrefix() + "acknowledgedTransitions=("
        If j And &H80 Then c$ = c$ + "to-offnormal,"
        If j And &H40 Then c$ = c$ + "to-fault,"
        If j And &H20 Then c$ = c$ + "to-normal,"
        If Right$(c$, 1) = "," Then c$ = Left$(c$, Len(c$) - 1)
        Ptrace c$ + ")"
        Phex i, i + 2
        i = i + 3
      Loop
    Case 4                                      'getEnrollmentSummary
      Do While i < Len(apdu)
        If dObjectIdentifier(-1, i, "objectIdentifier") Then Exit Sub
        If dEventType(-1, i, "eventType", et) Then Exit Sub
        If dEventState(-1, i, "eventState") Then Exit Sub
        If dUnsigned(i, "priority", rc) Then Exit Sub
        If (dByte(i) And &HF8) = &H20 Then      'we have a notificationClass
          If dUnsigned(i, "notificationClass", rc) Then Exit Sub
        End If
        If i < Len(apdu) Then
          Ptrace "---------------------------------------"
          Phex 0, 0
        End If
      Loop
    Case 6                                      'atomicFileRead
      j = dByte(i)
      If (j < &H10) Or (j > &H11) Then
        Ptrace "? Expected Boolean Tag X'10' or X'11' here"
        Perror i
        Exit Sub
      End If
      If j = &H10 Then c$ = "FALSE" Else c$ = "TRUE"
      Ptrace LevelPrefix() + "endOfFile=" + c$
      Phex i, i
      i = i + 1
      j = dByte(i)
      If j = &HE Then
        t$ = "streamAccess"
        c$ = "fileStartPosition"
        j = 0
      ElseIf j = &H1E Then
        t$ = "recordAccess"
        c$ = "fileStartRecord"
        j = 1
      Else
        Ptrace "? Expected Paired Delimiter Open Tag 0(streamAccess) or 1(recordAccess) here"
        Perror i
        Exit Sub
      End If
      Ptrace LevelPrefix() + "accessMethod=" + t$
      Phex i, i
      If IsntPDtag(j, i, 0) Then Exit Sub
        If dInteger(i, c$) Then Exit Sub
        If j Then                               'recordAccess
          If dUnsigned(i, "returnedRecordCount", rc) Then Exit Sub
          BeginLevel "fileRecordData"
          For et = 1 To rc
            If i > Len(apdu) Then Exit For
            If dOctetString(i, "record[" + Format$(et - 1) + "]", "(no data)", True) Then Exit Sub
          Next et
          EndLevel
        Else                                    'streamAccess
          If dOctetString(i, "fileData", "(no data)", True) Then Exit Sub
        End If
      If IsntPDtag(j, i, 1) Then Exit Sub
    Case 7                                      'atomicFileWrite
      If IsTag(0, i) Then
        c$ = "fileStartPosition"
        j = 0
      ElseIf IsTag(1, i) Then
        c$ = "fileStartRecord"
        j = 1
      Else
        Ptrace "? Expected Context Tag 0(fileStartPosition) or 1(fileStartRecord) here"
        Perror i
        Exit Sub
      End If
      j = dByte(i) And 7
      If (j < 1) Or (j > 4) Then
        Ptrace "? Invalid length for Signed Integer Tag here"
        Perror i
        Exit Sub
      End If
      Ptrace LevelPrefix() + c$ + "=" + Format$(vbLONG(Mid$(apdu, i + 1, j), j))
      Phex i, i + j
      i = i + j + 1
    Case 10                                     'createObject
        If dObjectIdentifier(-1, i, "objectIdentifier") Then Exit Sub
    Case 12                                     'readProperty
      If dObjectIdentifier(0, i, "objectIdentifier") Then Exit Sub
      If dProperty(1, i, "propertyIdentifier", False, t$, PropId, ari&) Then Exit Sub '***218
      If dByte(i) <> &H3E Then                  'we must have an open tag 3 here
        j = IsntPDtag(3, i, 0)                  'we know this fails, but it makes the error stuff we need
        Exit Sub
      End If
'      dAny i, "propertyValue", False, 0, 0           'ABSTRACT-SYNTAX.&Type
      dPropVal i, "propertyValue", PropId, ari& '                         ***218
    Case 13, 14                                 'readPropertyConditional,readPropertyMultiple
      BeginLevel "listOfReadAccessResults"
      Do While i < Len(apdu)
        If dObjectIdentifier(0, i, "objectIdentifier") Then Exit Sub
        If dByte(i) = &H1E Then                 'we have a listOfResults
          If IsntPDtag(1, i, 0) Then Exit Sub
          BeginLevel "listOfResults"
          Do While i < Len(apdu)
            If dByte(i) = &H1F Then Exit Do
            If dProperty(2, i, "", True, t$, PropId, ari&) Then Exit Sub '***218
            If dByte(i) = &H4E Then             'we have a property value
'              dAny i, t$, True, 0, 0 'ABSTRACT-SYNTAX.&Type
              dPropVal i, t$, PropId, ari&      '                         ***218
            Else                                'better be an error
              If IsntPDtag(5, i, 0) Then Exit Sub
              ErrorHeader t$ + "=Error ", i, False
              If IsntPDtag(5, i, 1) Then Exit Sub
            End If
          Loop
          If IsntPDtag(1, i, 1) Then Exit Sub
          EndLevel
        End If
      Loop
      EndLevel
    Case 18                                     'confirmedPrivateTransfer
      If dContext(0, i, "vendorID", 0, 0) Then Exit Sub
      If dContext(1, i, "serviceNumber", 0, 0) Then Exit Sub
      If dByte(i) = &H2E Then
        If IsntPDtag(2, i, 0) Then Exit Sub
        dAny i, "resultBlock", False, -1, 0        'ABSTRACT-SYNTAX.&Type
        If IsntPDtag(2, i, 1) Then Exit Sub
      End If
    Case 21                                     'vtOpen
      If dUnsigned(i, "remoteVTSessionIdentifier", rc) Then Exit Sub
    Case 23                                     'vtData
      enumx(0) = "FALSE"
      enumx(1) = "TRUE"
      If dEnum(0, i, "allNewDataAccepted", 2) Then Exit Sub
      If dByte(i - 1) = 0 Then
        If dContext(1, i, "acceptedOctetCount", 0, 0) Then Exit Sub
      End If
    Case 24                                     'authenticate
      If dUnsigned(i, "modifiedRandomNumber", rc) Then Exit Sub
  End Select
End Sub

Sub ConfirmedBody(svc, i)
Dim j, ae, et, evobjt, c$, d$, t$, rc As Long, PropId As Long, ari& '***218
  Select Case svc
    Case 0                                      'acknowledgeAlarm
      If dContext(0, i, "acknowledgingProcessIdentifier", 0, 0) Then Exit Sub
      If dObjectIdentifier(1, i, "eventObjectIdentifier") Then Exit Sub
      If dEventState(2, i, "eventStateAcknowledged") Then Exit Sub
      If dTimeStamp(3, i, "timeStamp") Then Exit Sub
      If dCharacterString(4, i, "acknowledgementSource") Then Exit Sub
      If dTimeStamp(5, i, "timeOfAcknowledgement") Then Exit Sub
    Case 1                                      'confirmedCOVNotification
      If dContext(0, i, "subscriberProcessIdentifier", 0, 0) Then Exit Sub
      If dObjectIdentifier(1, i, "initiatingDeviceIdentifier") Then Exit Sub
      If dObjectIdentifier(2, i, "monitoredObjectIdentifier") Then Exit Sub
      If dContext(3, i, "timeRemaining", 0, 0) Then Exit Sub
      If IsntPDtag(4, i, 0) Then Exit Sub       'must provide at least one value
      BeginLevel "listOfValues"
      Do While i <= Len(apdu)
        If dByte(i) = &H4F Then
          If IsntPDtag(4, i, 1) Then Exit Sub
          EndLevel
          Exit Sub                              'found closing delimiter
        End If
        If PropertyValue(i) Then Exit Sub
      Loop
      Ptrace "? Expected Paired Delimiter Close Tag 4 (X'4F') here!"
      Perror i
      IndentLevel = IndentLevel - 1
    Case 2                                      'confirmedEventNotification
      If dContext(0, i, "processIdentifier", 0, 0) Then Exit Sub
      If dObjectIdentifier(1, i, "initiatingDeviceIdentifier") Then Exit Sub
      If dObjectIdentifier(2, i, "eventObjectIdentifier") Then Exit Sub
      evobjt = LastObjType                      'remember object type for event object
      If dTimeStamp(3, i, "timeStamp") Then Exit Sub
      If dContext(4, i, "notificationClass", 0, 0) Then Exit Sub
      If dContext(5, i, "priority", 0, 255) Then Exit Sub
      If dEventType(6, i, "eventType", et) Then Exit Sub
      If IsTag(7, i) Then                       'messageText
        If dCharacterString(7, i, "messageText") Then Exit Sub
      End If
      If dNotifyType(8, i, "notifyType", ae) Then Exit Sub
      If ae Then                                'must have these parameters for alarm or event
        enumx(0) = "FALSE"
        enumx(1) = "TRUE"
        If dEnum(9, i, "ackRequired", 2) Then Exit Sub
        If dEventState(10, i, "fromState") Then Exit Sub
        If dEventState(11, i, "toState") Then Exit Sub
        If dNotificationParameters(12, i, "eventValues", et, evobjt) Then Exit Sub
      End If
'   Case 3                                      'getAlarmSummary has no parameters
    Case 4                                      'getEnrollmentSummary
      enumx(0) = "all"
      enumx(1) = "acked"
      enumx(2) = "not-acked"
      If dEnum(0, i, "acknowledgementFilter", 3) Then Exit Sub
      If IsTag(1, i) Then                       'we have an enrollment filter
        If IsntPDtag(1, i, 0) Then Exit Sub     'BACnetRecipientProcess
          If IsntPDtag(0, i, 0) Then Exit Sub   'recipient
            If IsTag(0, i) Then                 'recpient is a device object
              If dObjectIdentifier(0, i, "device") Then Exit Sub
            Else
              If IsntPDtag(1, i, 0) Then Exit Sub   'address
              If dNetwork(i, "network-number", rc) Then Exit Sub  '         ***218
              If dOctetString(i, "mac-address", "(broadcast)", False) Then Exit Sub
              If IsntPDtag(1, i, 1) Then Exit Sub
            End If
          If IsntPDtag(0, i, 1) Then Exit Sub
        If dContext(1, i, "processIdentifier", 0, 0) Then Exit Sub
        If IsntPDtag(1, i, 1) Then Exit Sub
      End If
      If IsTag(2, i) Then
        enumx(0) = "offnormal"
        enumx(1) = "fault"
        enumx(2) = "normal"
        enumx(3) = "all"
        enumx(4) = "active"
        If dEnum(2, i, "eventStateFilter", 5) Then Exit Sub
      End If
      If IsTag(3, i) Then
        If dEventType(3, i, "eventTypeFilter", et) Then Exit Sub
      End If
      If IsTag(4, i) Then
        If IsntPDtag(4, i, 0) Then Exit Sub
        If dContext(0, i, "priorityFilter(minPriority)", 0, 255) Then Exit Sub
        If dContext(1, i, "priorityFilter(maxPriority)", 0, 255) Then Exit Sub
        If IsntPDtag(4, i, 1) Then Exit Sub
      End If
      If IsTag(5, i) Then
        If dContext(5, i, "notificationClassFilter", 0, 0) Then Exit Sub
      End If
      Pflush
    Case 5                                      'subscribeCOV
      If dContext(0, i, "subscriberProcessIdentifier", 0, 0) Then Exit Sub
      If dObjectIdentifier(1, i, "monitoredObjectIdentifier") Then Exit Sub
      If IsTag(2, i) Then
        enumx(0) = "FALSE"
        enumx(1) = "TRUE"
        If dEnum(2, i, "issueConfirmedNotifications", 2) Then Exit Sub
      End If
      If IsTag(3, i) Then
        If dContext(3, i, "lifetime", 0, 0) Then Exit Sub
      End If
    Case 6                                      'atomicFileRead
      If dObjectIdentifier(-1, i, "fileIdentifier") Then Exit Sub
      j = dByte(i)
      If j = &HE Then
        t$ = "streamAccess"
        d$ = "fileStartPosition"
        c$ = "requestedOctetCount"
        j = 0
      ElseIf j = &H1E Then
        t$ = "recordAccess"
        d$ = "fileStartRecord"
        c$ = "requestedRecordCount"
        j = 1
      Else
        Ptrace "? Expected Paired Delimiter Open Tag 0(streamAccess) or 1(recordAccess) here"
        Perror i
        Exit Sub
      End If
      If IsntPDtag(j, i, 0) Then Exit Sub
      BeginLevel "accessMethod is " + t$
        If dInteger(i, d$) Then Exit Sub
        If dUnsigned(i, c$, rc) Then Exit Sub
      If IsntPDtag(j, i, 1) Then Exit Sub
      EndLevel
    Case 7                                      'atomicFileWrite
      If dObjectIdentifier(-1, i, "fileIdentifier") Then Exit Sub
      j = dByte(i)
      If j = &HE Then
        t$ = "streamAccess"
        j = 0
      ElseIf j = &H1E Then
        t$ = "recordAccess"
        j = 1
      Else
        Ptrace "? Expected Paired Delimiter Open Tag 0(streamAccess) or 1(recordAccess) here"
        Perror i
        Exit Sub
      End If
      If IsntPDtag(j, i, 0) Then Exit Sub
      BeginLevel "accessMethod is " + t$
      If j Then                                 'record access
        If dInteger(i, "fileStartRecord") Then Exit Sub
        If dUnsigned(i, "recordCount", rc) Then Exit Sub
        BeginLevel "fileRecordData"
        For ae = 1 To rc
          If dOctetString(i, "record[" + Format$(ae - 1) + "]", "(no data)", True) Then Exit Sub
        Next ae
        EndLevel
      Else                                      'stream access
        If dInteger(i, "fileStartPosition") Then Exit Sub
        If dOctetString(i, "fileData", "(no data)", True) Then Exit Sub
      End If
      If IsntPDtag(j, i, 1) Then Exit Sub
      EndLevel
    Case 8, 9                                   'addListElement,removeListElement
      If dObjectIdentifier(0, i, "objectIdentifier") Then Exit Sub
      If dProperty(1, i, "propertyIdentifier", False, t$, PropId, ari&) Then Exit Sub '***218
      If dByte(i) <> &H3E Then
        j = IsntPDtag(3, i, 0)                  'we know this will fail, but it generates the error message we need
        Exit Sub
      End If
'      dAny i, "listOfElements", False, 0, 0         'ABSTRACT-SYNTAX.&Type
      dPropVal i, "listOfElements", PropId, ari& '                                 ***218
    Case 10                                     'createObject
      If IsntPDtag(0, i, 0) Then Exit Sub
        If IsTag(0, i) Then
          If dObjectType(0, i, "objectType") Then Exit Sub
        Else
          If dObjectIdentifier(1, i, "objectIdentifier") Then Exit Sub
        End If
      If IsntPDtag(0, i, 1) Then Exit Sub
      If dByte(i) = &H1E Then                   'we have some initial values
        If IsntPDtag(1, i, 0) Then Exit Sub
        BeginLevel "listOfInitialValues"
        Do While i <= Len(apdu)
          If dByte(i) = &H1F Then
            If IsntPDtag(1, i, 1) Then Exit Sub
            EndLevel
            Exit Sub                              'found closing delimiter
          End If
          If PropertyValue(i) Then Exit Sub
        Loop
        Ptrace "? Expected Paired Delimiter Close Tag 1 (X'1F') here!"
        Perror i
        IndentLevel = IndentLevel - 1
      End If
      Pflush
    Case 11                                     'deleteObject
      If dObjectIdentifier(-1, i, "objectIdentifier") Then Exit Sub
    Case 12                                     'readProperty
      If dObjectIdentifier(0, i, "objectIdentifier") Then Exit Sub
      If dProperty(1, i, "propertyIdentifier", False, t$, PropId, ari&) Then Exit Sub '***218
    Case 13                                     'readPropertyConditional
      If IsntPDtag(0, i, 0) Then Exit Sub
      BeginLevel "objectSelectionCriteria"
        enumx(0) = "and"
        enumx(1) = "or"
        enumx(2) = "all"
        If dEnum(0, i, "selectionLogic", 3) Then Exit Sub
        If dByte(i) = &H1E Then                 'we've got a list of selection criteria
          If IsntPDtag(1, i, 0) Then Exit Sub
          BeginLevel "listOfSelectionCriteria"
          Do While i < Len(apdu)
            If dByte(i) = &H1F Then Exit Do     'done with this guy
            If dProperty(0, i, "propertyIdentifier", False, t$, PropId, ari&) Then Exit Sub '***218
            enumx(0) = "equal"
            enumx(1) = "not-equal"
            enumx(2) = "less-than"
            enumx(3) = "greater-than"
            enumx(4) = "less-than-or-equal"
            enumx(5) = "greater-than-or-equal"
            If dEnum(2, i, "relationSpecifier", 6) Then Exit Sub
            If dByte(i) = &H3E Then             'we have a comparison value
'              dAny i, "comparisonValue", True, 0, 0 'ABSTRACT-SYNTAX.&Type
              LastObjType = -1                  'unknown object type
              dPropVal i, "comparisonValue", PropId, ari& '                 ***218
            End If
          Loop
          If IsntPDtag(1, i, 1) Then Exit Sub
        End If
      If IsntPDtag(0, i, 1) Then Exit Sub
      EndLevel
      If dByte(i) = &H1E Then                   'list of prop refs was provided
        If IsntPDtag(1, i, 0) Then Exit Sub
          BeginLevel "listOfPropertyReferences"
          Do While i < Len(apdu)
            If dByte(i) = &H1F Then Exit Do
            If dProperty(0, i, "", False, t$, PropId, ari&) Then Exit Sub '***218
          Loop
        If IsntPDtag(1, i, 1) Then Exit Sub
        EndLevel
      End If
    Case 14                                     'readPropertyMultiple
      BeginLevel "listOfReadAccessSpecifications"
      Do While i < Len(apdu)
        If dObjectIdentifier(0, i, "objectIdentifier") Then Exit Sub
        If IsntPDtag(1, i, 0) Then Exit Sub
          BeginLevel "listOfPropertyReferences"
          Do While i < Len(apdu)
            If dByte(i) = &H1F Then Exit Do
            If dProperty(0, i, "", False, t$, PropId, ari&) Then Exit Sub '***218
          Loop
        If IsntPDtag(1, i, 1) Then Exit Sub
        EndLevel
      Loop
      EndLevel
    Case 15                                     'writeProperty
      If dObjectIdentifier(0, i, "objectIdentifier") Then Exit Sub
      If dProperty(1, i, "propertyIdentifier", False, t$, PropId, ari&) Then Exit Sub '***218
      If dByte(i) = &H3E Then                   'we've got a property value
        dPropVal i, "propertyValue", PropId, ari& '                             ***218
      Else
        If IsntPDtag(3, i, 0) Then Exit Sub     'we know this will fail
      End If
      If dByte(i) = &H49 Then                   'we have priority
        If dContext(4, i, "priority", 1, 16) Then Exit Sub
      End If
    Case 16                                     'writePropertyMultiple
      BeginLevel "listOfWriteAccessSpecifications"
      Do While i < Len(apdu)
        If dObjectIdentifier(0, i, "objectIdentifier") Then Exit Sub
        If IsntPDtag(1, i, 0) Then Exit Sub
          BeginLevel "listOfProperties"
          Do While i < Len(apdu)
            If dByte(i) = &H1F Then Exit Do
            If PropertyValue(i) Then Exit Sub
          Loop
        If IsntPDtag(1, i, 1) Then Exit Sub
        EndLevel
      Loop
      EndLevel
    Case 17                                     'deviceCommunicationControl
      If IsTag(0, i) Then
        If dContext(0, i, "timeDuration", 0, 65535) Then Exit Sub
      End If
      enumx(0) = "enable"
      enumx(1) = "disable"
      If dEnum(1, i, "enable-disable", 2) Then Exit Sub
      If IsTag(2, i) Then
        If dCharacterString(2, i, "password") Then Exit Sub
      End If
    Case 18                                     'confirmedPrivateTransfer
      If dContext(0, i, "vendorID", 0, 0) Then Exit Sub
      If dContext(1, i, "serviceNumber", 0, 0) Then Exit Sub
      If dByte(i) = &H2E Then
        Phex i, i
        BeginLevel "serviceParameters"
        Phex i + 1, Len(apdu)
        EndLevel
      End If
    Case 19                                     'confirmedTextMessage
      If dObjectIdentifier(0, i, "textMessageSourceDevice") Then Exit Sub
      If dByte(i) = &H1E Then                   'message class was provided
        If IsntPDtag(1, i, 0) Then Exit Sub
        If IsTag(0, i) Then                     'numeric class
          If dContext(0, i, "messageClass", 0, 0) Then Exit Sub
        ElseIf IsTag(1, i) Then                 'character class
          If dCharacterString(1, i, "messageClass") Then Exit Sub
        Else
          Ptrace "? Expected numeric or character messageClass here"
          Perror i
          Exit Sub
        End If
        If IsntPDtag(1, i, 1) Then Exit Sub
      End If
      enumx(0) = "normal"
      enumx(1) = "urgent"
      If dEnum(2, i, "messagePriority", 2) Then Exit Sub
      If dCharacterString(3, i, "message") Then Exit Sub
    Case 20                                     'reinitializeDevice
      enumx(0) = "coldstart"
      enumx(1) = "warmstart"
      If dEnum(0, i, "reinitializedStateOfDevice", 2) Then Exit Sub
      If IsTag(1, i) Then
        If dCharacterString(1, i, "password") Then Exit Sub
      End If
    Case 21                                     'vtOpen
      enumx(0) = "default-terminal"
      enumx(1) = "ansi-x3.64"
      enumx(2) = "dec-vt52"
      enumx(3) = "dec-vt100"
      enumx(4) = "dec-vt220"
      enumx(5) = "hp-700/94"
      enumx(6) = "ibm-3130"
      If dEnum(-1, i, "vtClass", 7) Then Exit Sub
      If dUnsigned(i, "localVTSessionIdentifier", rc) Then Exit Sub
    Case 22                                     'vtClose
      t$ = "listOfRemoteVTSessionIdentifiers=("
      j = i
      Do While i < Len(apdu)
        If dByte(i) <> &H21 Then
            Ptrace LevelPrefix() + t$
            Phex j, i - 1
            Ptrace "? Expected Unsigned Integer(X'21') tag here"
            Phex i, i
            Exit Sub
        End If
        t$ = t$ + Format$(dByte(i + 1))
        If (i + 1) < Len(apdu) Then t$ = t$ + ","
        i = i + 2
      Loop
      Ptrace LevelPrefix() + t$ + ")"
      Phex j, i - 1
    Case 23                                     'vtData
      If dUnsigned(i, "vtSessionIdentifier", rc) Then Exit Sub
      If dOctetString(i, "vtNewData", "(none)", True) Then Exit Sub
      If dUnsigned(i, "vtDataFlag", rc) Then Exit Sub           '   ***209
    Case 24                                     'authenticate
      If dContext(0, i, "pseudoRandomNumber", 0, 0) Then Exit Sub
      If IsTag(1, i) Then
        If dContext(1, i, "expectedInvokeID", 0, 255) Then Exit Sub
      End If
      If IsTag(2, i) Then
        If dCharacterString(2, i, "operatorName") Then Exit Sub
        If dCharacterString(3, i, "operatorPassword") Then Exit Sub
      End If
      If IsTag(4, i) Then
        enumx(0) = "FALSE"
        enumx(1) = "TRUE"
        If dEnum(4, i, "startEncipheredSession", 2) Then Exit Sub
      End If
    Case 25                                     'requestKey
      If dObjectIdentifier(-1, i, "requestingDeviceIdentifier") Then Exit Sub
      BeginLevel "requestingDeviceAddress"
        If dNetwork(i, "network-number", rc) Then Exit Sub  '             ***218
        If dOctetString(i, "mac-address", "(broadcast)", False) Then Exit Sub
      EndLevel
      If dObjectIdentifier(-1, i, "remoteDeviceIdentifier") Then Exit Sub
      BeginLevel "remoteDeviceAddress"
        If dNetwork(i, "network-number", rc) Then Exit Sub  '             ***218
        If dOctetString(i, "mac-address", "(broadcast)", False) Then Exit Sub
      EndLevel
  End Select
End Sub

Function ConfirmedService(svc) As String
  Select Case svc
    Case 0
      ConfirmedService = "acknowledgeAlarm"
    Case 1
      ConfirmedService = "confirmedCOVNotification"
    Case 2
      ConfirmedService = "confirmedEventNotification"
    Case 3
      ConfirmedService = "getAlarmSummary"
    Case 4
      ConfirmedService = "getEnrollmentSummary"
    Case 5
      ConfirmedService = "subscribeCOV"
    Case 6
      ConfirmedService = "atomicFileRead"
    Case 7
      ConfirmedService = "atomicFileWrite"
    Case 8
      ConfirmedService = "addListElement"
    Case 9
      ConfirmedService = "removeListElement"
    Case 10
      ConfirmedService = "createObject"
    Case 11
      ConfirmedService = "deleteObject"
    Case 12
      ConfirmedService = "readProperty"
    Case 13
      ConfirmedService = "readPropertyConditional"
    Case 14
      ConfirmedService = "readPropertyMultiple"
    Case 15
      ConfirmedService = "writeProperty"
    Case 16
      ConfirmedService = "writePropertyMultiple"
    Case 17
      ConfirmedService = "deviceCommunicationControl"
    Case 18
      ConfirmedService = "confirmedPrivateTransfer"
    Case 19
      ConfirmedService = "confirmedTextMessage"
    Case 20
      ConfirmedService = "reinitializeDevice"
    Case 21
      ConfirmedService = "vtOpen"
    Case 22
      ConfirmedService = "vtClose"
    Case 23
      ConfirmedService = "vtData"
    Case 24
      ConfirmedService = "authenticate"
    Case 25
      ConfirmedService = "requestKey"
    Case Else
      ConfirmedService = "Unknown Confirmed Service" + Str$(svc)
  End Select
End Function

Function CTag(ct)
  CTag = (ct * 16) + &H8
End Function

Function dActionList(i, c$)
Dim optag, t$, PropId As Long, ari&             '                           ***218
  dActionList = True
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  SaveHex i
  i = i + 1
  optag = optag + 1                             'remember closing tag
  If IsntPDtag(0, i, 0) Then Exit Function
  BeginLevel c$
  Do While i < Len(apdu)
    If dByte(i) = &HF Then Exit Do
    BeginLevel ""
    If dByte(i) = &HC Then                      'got an optional device id
      If dObjectIdentifier(0, i, "deviceIdentifier") Then Exit Function
      If LastObjType <> 8 Then
        Ptrace "? Only Device objects are allowed as deviceIdentifier!"
        Perror i - 5
        Exit Function
      End If
    End If
    If dObjectIdentifier(1, i, "objectIdentifier") Then Exit Function
    If dProperty(2, i, "propertyIdentifier", False, t$, PropId, ari&) Then Exit Function '***218
    If (dByte(i) And &HF0) = &H40 Then          'we've got a property value
      dPropVal i, "propertyValue", PropId, ari& '                               ***218
    Else
        Ptrace "? Expected Context tag X'4?' here!"
        Perror i
        Exit Function
    End If
    If dByte(i) = &H59 Then                     'we have priority
        If dContext(5, i, "priority", 1, 16) Then Exit Function
    End If
    If (dByte(i) And &HF0) = &H60 Then          'we have postdelay
        If dContext(6, i, "postDelay", 0, 0) Then Exit Function
    End If
    enumx(0) = "FALSE"
    enumx(1) = "TRUE"
    If dEnum(7, i, "quitOnFailure", 2) Then Exit Function
    If dEnum(8, i, "WriteSuccessful", 2) Then Exit Function
    EndLevel
  Loop
  If IsntPDtag(0, i, 1) Then Exit Function
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function
  EndLevel
  dActionList = False
End Function

Function dAddressBindings(i, c$)
Dim nn As Long, si, p$, optag, ul
  dAddressBindings = True
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  Ptrace LevelPrefix() + c$ + "=("
  IndentLevel = IndentLevel + 1
  Phex i, i
  i = i + 1
  si = i
  Do While i < Len(apdu)
    If dByte(i) = (optag + 1) Then Exit Do
    If dByte(i) <> &HC4 Then
      Ptrace "? Expected Object Identifier(X'C4') tag here"
      Perror i
      Exit Function
    End If
    p$ = "(" + ObjectIdentifier(i + 1) + ","
    i = i + 5
    
    ul = dByte(i)
    If (ul And &HF8) <> &H20 Then
      Ptrace "? Expected Unsigned Integer(X'2x') tag here"
      Perror i
      Exit Function
    End If
    ul = ul And 7                         'save length
    If (ul < 1) Or (ul > 2) Then
      Ptrace "? Invalid length for Unsigned Integer Tag here"
      Perror i
      Exit Function
    End If
    nn = vbDWORD(Mid$(apdu, i + 1, ul), ul)
    p$ = p$ + Format$(nn) + ","
    i = i + ul + 1

    ul = dByte(i)
    If (ul And &HF8) <> &H60 Then
      Ptrace "? Expected OctetString(X'6x') tag here"
      Perror i
      Exit Function
    End If
    i = i + 1
    ul = ul And 7                         'save length
    If (ul > 6) Then
      Ptrace "? Invalid length for OctetString Tag here"
      Perror i
      Exit Function
    End If
    If ul = 5 Then                        'extended length
      ul = dByte(i)
      i = i + 1
    ElseIf ul = 6 Then
      ul = dWord(i)
      i = i + 2
    End If
    If ul = 0 Then
      p$ = p$ + "broadcast"
    Else
      p$ = p$ + OctetString(i, ul)
    End If
    i = i + ul
    If dByte(i) = (optag + 1) Then            'last one
      p$ = p$ + "))"
    Else
      p$ = p$ + "),"
    End If
    Ptrace LevelPrefix() + p$
    Phex si, i - 1
    si = i
  Loop
  IndentLevel = IndentLevel - 1
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function
  Pflush
  dAddressBindings = False
End Function

'Try to decode any encoded element
'in:    i               index into apdu
'       c$              parameter title
'       noouterbracket  false=normal, true=suppress () at the outer layer
'       ptype           parsetype or -1 if unknown
'       pet             enum table index or 0 if unknown
Sub dAny(i, c$, ByVal noouterbracket, ByVal ptype, ByVal pet)
Dim l As Long, si, j, b, k, tag, tlv, pairlevel, opentag, closetag, nc, v$, n$, tw
Dim lv As Long, jl As Long, pn As String * 64
ReDim PDstack(MaxPDNesting)
  
  tw = TraceList.Width \ TraceForm.TextWidth("W")
  pairlevel = -1
  v$ = c$ + "="
  nc = False
  si = i
  Do While (i <= Len(apdu))
    n$ = ""
    tlv = dByte(i)
    i = i + 1
    tag = tlv \ 16
    If tag = 15 Then                            'extended tag
      tag = dByte(i)
      i = i + 1
    End If
    l = tlv And 7                               'length
    opentag = False
    closetag = False
    If l = 5 Then                               'extended length
      l = dByte(i)
      i = i + 1
      If l = 254 Then
        l = dWord(i)
        i = i + 2
      ElseIf l = 255 Then
        l = vbDWORD(Mid$(apdu, i, 4), 4)
        i = i + 4
      End If
    ElseIf (l = 6) Then                         'opening tag
      opentag = True
    ElseIf (l = 7) Then                         'closing tag
      closetag = True
    End If
    If tlv And 8 Then                           'context tag
      If (l >= 0) And (l < 5) Then
        If KnowPrimitiveInContext(tag, ptype, l) Then GoTo doprimitive
        n$ = Format$(tag, "\[0\]")
        If l > 0 Then n$ = n$ + Str$(vbDWORD(Mid$(apdu, i, l), l))
      ElseIf opentag Then
        If (Not noouterbracket) Or (pairlevel >= 0) Then
          If v$ <> "" Then
            If pairlevel = 0 Then
              Ptrace LevelPrefix() + v$
              v$ = "("
            Else
              Ptrace LevelPrefix() + v$ + "("
              v$ = ""
            End If
            If pairlevel < 0 Then
              Phex si, i - 1
              si = i
            Else
              Phex si, i - 2
              si = i - 1
            End If
          End If
        End If                                  '                                       %%%
        If Not (noouterbracket And (pairlevel < 0)) Then IndentLevel = IndentLevel + 1
        pairlevel = pairlevel + 1
        If pairlevel > UBound(PDstack) Then ReDim Preserve PDstack(pairlevel)
        PDstack(pairlevel) = tag                'remember we are starting a new tag level
        l = 0
      ElseIf closetag Then
        If PDstack(pairlevel) <> tag Then       'check if pairlevels match
          Ptrace "? Warning: mismatched Paired Delimiters! Found Close" + Str$(tag) + ", expected Close" + Str$(PDstack(pairlevel))
          Perror i
        End If
        If Right$(v$, 1) = "," Then v$ = Left$(v$, Len(v$) - 1)
        pairlevel = pairlevel - 1
        If (Not noouterbracket) Or (pairlevel >= 0) Then v$ = v$ + ")"
        Ptrace LevelPrefix() + v$
        Phex si, i - 1
        si = i
        v$ = ""
        If Not (noouterbracket And (pairlevel < 0)) Then IndentLevel = IndentLevel - 1
        l = 0
      Else                                      'l>4
        If KnowPrimitiveInContext(tag, ptype, l) Then GoTo doprimitive
        n$ = Format$(tag, "\[0\] ") + OctetString(i, l)
      End If
    Else                                        'primitive
doprimitive:
      Select Case tag
        Case 0                                  'NULL
          n$ = "NULL"
        Case 1                                  'Boolean
          If l Then n$ = "TRUE" Else n$ = "FALSE"
          l = 0
        Case 2                                  'Unsigned Integer
          n$ = Format$(vbDWORD(Mid$(apdu, i, l), l))
        Case 3                                  'Signed Integer
          n$ = Format$(vbLONG(Mid$(apdu, i, l), l))
        Case 4                                  'Real
          n$ = Format$(vbREAL(Mid$(apdu, i, 4)))
        Case 5                                  'Double
          n$ = Format$(vbDOUBLE(Mid$(apdu, i, 8)))
        Case 6                                  'Octet string
          n$ = OctetString(i, l)
        Case 7                                  'Character string
          k = l - 1                             'actual string length
          j = dByte(i)                          'string type code
          i = i + 1
          If j > 1 Then
            Ptrace "? String Type Not Currently Supported (" + Format$(j) + ")"
            Perror i - 1
            Exit Sub
          End If
          If j = 0 Then                         'regular ANSI string
            n$ = Chr$(34)                       'open quote
          Else                                  'DBCS string
            b = dWord(i)
            i = i + 2
            n$ = "[" + Format$(b) + "]" + Chr$(34)
            k = k - 2                         'account for code page
          End If
          n$ = n$ + Mid$(apdu, i, k) + Chr$(34)
          i = i + k
          l = 0
        Case 8                                  'Bit String
          n$ = "("
          b = &H80
          tlv = dByte(i)
          l = l - 1
          i = i + 1
          If l Then
            k = dByte(i)
            i = i + 1
            For j = 1 To (l * 8) - tlv          'number of bits to do
              If k And b Then n$ = n$ + "TRUE," Else n$ = n$ + "FALSE,"
              b = b \ 2
              If b = 0 Then
                b = &H80
                k = dByte(i)
                i = i + 1
              End If
            Next j
          End If
          If Right$(n$, 1) = "," Then n$ = Left$(n$, Len(n$) - 1)
          n$ = n$ + ")"
          l = 0
        Case 9                                  'enumeration
          lv = vbDWORD(Mid$(apdu, i, l), l)     'get the enumeration value
          If pet Then                           'we have a table to use
            j = VTSAPIgetenumtable(pet, lv, j, jl, pn)
            n$ = Format$(lv) + " (" + Left$(pn, InStr(pn, Chr$(0)) - 1) + ")"
          Else                                  'must be dumb
            n$ = "(Enum," + Format$(lv) + ")"
          End If
        Case 10                                 'Date
          j = dByte(i + 2)                      'day of month
          If j = 255 Then n$ = "(?," Else n$ = "(" + Format$(j) + ","
          j = dByte(i + 1)                      'month
          If j = 255 Then n$ = n$ + "?," Else n$ = n$ + Left$(MonthName(j), 3) + ","
          j = dByte(i)                          'year
          If j = 255 Then n$ = n$ + "?," Else n$ = n$ + Format$(1900 + j) + ","
          j = dByte(i + 3)                      'day of week
          If j = 255 Then n$ = n$ + "?)" Else n$ = n$ + DayName(j) + ")"
        Case 11                                 'Time
          j = dByte(i)                          'hours
          If j = 255 Then n$ = "(?," Else n$ = "(" + Format$(j) + ":"
          j = dByte(i + 1)                      'minutes
          If j = 255 Then n$ = n$ + "?," Else n$ = n$ + Format$(j) + ":"
          j = dByte(i + 2)                      'seconds
          If j = 255 Then n$ = n$ + "?," Else n$ = n$ + Format$(j) + "."
          j = dByte(i + 3)                      'day of week
          If j = 255 Then n$ = n$ + "?)" Else n$ = n$ + Format$(j) + ")"
        Case 12                                 'ObjectIdentifier
          n$ = ObjectIdentifier(i)
        Case Else
          v$ = v$ + n$
          If v$ <> "" Then
            Ptrace LevelPrefix() + v$
            Phex si, i - 1
            si = i
          End If
          v$ = ""
          Ptrace "? BACnet reserved tag" + Str$(tag) + " is not allowed here"
          Perror i - 1
      End Select
    End If
    i = i + l                                   'advance to the end of this tagged item
    If pairlevel < 0 Then
      v$ = v$ + n$
      If Right$(v$, 1) = "," Then v$ = Left$(v$, Len(v$) - 1)
      If v$ <> "" Then
        Ptrace LevelPrefix() + v$
        Phex si, i - 1
      End If
      Exit Sub                                  'we're done
    End If
    If Not (opentag Or closetag) Then n$ = n$ + ","
    If Len(LevelPrefix() + v$ + n$) > tw Then    'we've gotten too wide for a line
      Ptrace LevelPrefix() + v$
      Phex si, i - 1
      si = i
      v$ = n$
    Else
      v$ = v$ + n$
    End If
  Loop
End Sub

Function DayName(d) As String
  Select Case d
    Case 1
      DayName = "Monday"
    Case 2
      DayName = "Tuesday"
    Case 3
      DayName = "Wednesday"
    Case 4
      DayName = "Thursday"
    Case 5
      DayName = "Friday"
    Case 6
      DayName = "Saturday"
    Case 7
      DayName = "Sunday"
    Case 255
      DayName = "any day-of-week"
    Case Else
      DayName = "invalid day-of-week (" + Format$(d) + ")"
  End Select
End Function

Function dBitString(tag, i, c$)
Dim et, si, sl, ub, j, k, b, nbcl, v$
  dBitString = True
  sl = dByte(i)
  si = i
  If tag < 0 Then et = &H80 Else et = CTag(tag)
  i = i + 1
  If (sl And &HF8) <> et Then
    If tag < 0 Then
      Ptrace "? Expected BitString(X'8x') tag here"
    Else
      Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(tag) + "x') here"
    End If
    Perror i - 1
    Exit Function
  End If
  sl = sl And 7                         'save length
  If (sl = 0) Or (sl > 6) Then
    Ptrace "? Invalid length for BitString Tag here"
    Perror i - 1
    Exit Function
  End If
  If sl = 5 Then                        'extended length
    sl = dByte(i)
    i = i + 1
  ElseIf sl = 6 Then
    sl = dWord(i)
    i = i + 2
  End If
'here, sl is the length of the bitstring in octets
  If c$ = "" Then
    v$ = LevelPrefix() + "("
  Else
    v$ = LevelPrefix() + c$ + "=("
  End If
  b = &H80
  ub = ((sl - 1) * 8) - dByte(i)        'number of used bits
  i = i + 1
  k = dByte(i)                          'first octet
  i = i + 1
  nbcl = 0                              'number of bits on current line
  For j = 1 To ub                       'number of bits to do
    If k And b Then v$ = v$ + "T," Else v$ = v$ + "F,"
    b = b \ 2
    If b = 0 Then
      b = &H80
      k = dByte(i)
      i = i + 1
    End If
    nbcl = nbcl + 1
    If nbcl = 8 Then
      Ptrace v$
      Phex si, i - 1
      si = i
      v$ = "            "
      nbcl = 0
    End If
  Next j
  If nbcl Then v$ = Left$(v$, Len(v$) - 1)
  Ptrace v$ + ")"
  Phex si, i - 1
  dBitString = False
End Function

Function dByte(aoff)
  If aoff > Len(apdu) Then
    dByte = 0
  Else
    dByte = Asc(Mid$(apdu, aoff, 1))
  End If
End Function

Function dCalendarEntries(i, c$)
Dim nn As Long, si, p$, optag, ul
  dCalendarEntries = True
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  Ptrace LevelPrefix() + c$ + "=("
  IndentLevel = IndentLevel + 1
  Phex i, i
  i = i + 1
  si = i
  Do While i < Len(apdu)
    ul = dByte(i)
    If ul = (optag + 1) Then Exit Do
    If ul = &HC Then                            'date
      p$ = BACnetDate(i + 1)
      i = i + 5
    ElseIf ul = &H2B Then                       'WeekNDay
      p$ = BACnetWeekNDay(i + 1)
      i = i + 4
    Else                                        'daterange
      If IsntPDtag(1, i, 0) Then Exit Function
      ul = dByte(i)
      If ul <> &HA4 Then
        Ptrace "? Expected Date tag (X'A4') here"
        Perror i
        Exit Function
      End If
      p$ = "(" + BACnetDate(i + 1) + ","
      i = i + 5
      ul = dByte(i)
      If ul <> &HA4 Then
        Ptrace "? Expected Date tag (X'A4') here"
        Perror i
        Exit Function
      End If
      p$ = p$ + BACnetDate(i + 1) + ")"
      i = i + 5
      If IsntPDtag(1, i, 1) Then Exit Function
    End If

    If dByte(i) = (optag + 1) Then            'last one
      p$ = p$ + ")"
    Else
      p$ = p$ + ","
    End If
    SavedHex = ""
    Ptrace LevelPrefix() + p$
    Phex si, i - 1
    si = i
  Loop
  IndentLevel = IndentLevel - 1
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function
  Pflush
  dCalendarEntries = False
End Function

'decode a CharacterString primitive
'in:    tag     -1 if primitive data type tag (0x7X) else context tag
Function dCharacterString(tag, aoff, c$)
Dim et, AL As Long, sl As Long, i, st, cp, q$     '                                 ***218
  dCharacterString = True
  sl = dByte(aoff)
  If tag < 0 Then et = &H70 Else et = CTag(tag)
  i = aoff + 1
  If (sl And &HF8) <> et Then
    If tag < 0 Then
      Ptrace "? Expected CharacterString(X'7x') tag here"
    Else
      Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(tag) + "x') here"
    End If
    Perror i - 1
    Exit Function
  End If
  sl = sl And 7                         'save length
  If (sl = 0) Or (sl > 5) Then          '                                     ***218
    Ptrace "? Invalid length for CharacterString Tag here"
    Phex aoff, aoff
    Exit Function
  End If
  If sl = 5 Then                        'extended length                     ***218 Begin
    sl = dByte(i) And &HFF&
    i = i + 1
    If sl = 254 Then                    'word of extended length
      sl = dWord(i) And &HFFFF&
      i = i + 2
    ElseIf sl = 255 Then                'dword of extended length
      sl = dDword(i)
      i = i + 4
    End If                              '                                     ***218 End
  End If
  AL = sl - 1                           'actual string length
  st = dByte(i)                         'string type code
  i = i + 1
  If st > 1 Then
    Ptrace "? String Type Not Currently Supported (" + Format$(st) + ")"
    Phex aoff, i - 1
    Exit Function
  End If
  If c$ = "" Then q$ = "" Else q$ = LevelPrefix() + c$ + "="
  If st = 0 Then                        'regular ANSI string
    q$ = q$ + Chr$(34)                  'open quote
  Else                                  'DBCS string
    cp = dWord(i)
    i = i + 2
    q$ = q$ + "[" + Format$(cp) + "]" + Chr$(34)
    AL = AL - 2                         'account for code page
  End If

  Ptrace q$ + Mid$(apdu, i, AL) + Chr$(34)
  Phex aoff, (i + AL) - 1
  aoff = i + AL
  dCharacterString = False
End Function

Function dContext(tag, aoff, c$, minval&, maxval&)
Dim ul, et, n$, v As Long
  dContext = True
  et = ((tag * 16) + &H8)
  ul = dByte(aoff)
  If (ul And &HF8) <> et Then
    Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(et) + "') here"
    Perror aoff
    Exit Function
  End If
  ul = ul And 7                         'save length
  If maxval < 256 Then n$ = " octet" Else n$ = " octets"
  If (ul < 1) Or (ul > 4) Then
    Ptrace "? Invalid length for Enumeration here"
    Perror aoff
    Exit Function
  End If
  v = vbDWORD(Mid$(apdu, aoff + 1, ul), ul)
  If maxval Then
    If (v < minval) Or (v > maxval) Then
      Ptrace "? Enumerated value must be in the range" + Str$(minval) + " to" + Str$(maxval)
      Perror aoff
      Exit Function
    End If
  End If
  Ptrace LevelPrefix() + c$ + "=" + Format$(v)
  Phex aoff, aoff + ul
  aoff = aoff + ul + 1
  dContext = False
End Function

Function dDate(aoff, c$)
Dim y, yr$, m, d, dn$, dw
  If dByte(aoff) <> &HA4 Then
    Ptrace "? Expected Date(X'A4') tag here"
    Perror aoff
    dDate = True
    Exit Function
  End If
  y = dByte(aoff + 1)
  If y = 255 Then yr$ = "any year" Else yr$ = Format$(1900 + y)
  m = dByte(aoff + 2)
  d = dByte(aoff + 3)
  If d = 255 Then dn$ = " any day" Else dn$ = Str$(d)
  dw = dByte(aoff + 4)
  Ptrace LevelPrefix() + c$ + "=" + MonthName(m) + dn$ + ", " + yr$ + " (" + DayName(dw) + ")"
  Phex aoff, aoff + 4
  aoff = aoff + 5
  dDate = False
End Function

Function dDateRange(i, c$)
Dim p$, ul, optag, si
  dDateRange = True
  si = i
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  i = i + 1
  ul = dByte(i)
  If ul <> &HA4 Then
    Ptrace "? Expected Date tag (X'A4') here"
    Perror i
    Exit Function
  End If
  p$ = c$ + "=(" + BACnetDate(i + 1) + ","
  i = i + 5
  ul = dByte(i)
  If ul <> &HA4 Then
    Ptrace "? Expected Date tag (X'A4') here"
    Perror i
    Exit Function
  End If
  p$ = p$ + BACnetDate(i + 1) + ")"
  i = i + 5
  
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function 'better have a closing tag
  SavedHex = ""
  Ptrace LevelPrefix() + p$
  Phex si, i - 1
  dDateRange = False
End Function

Function dEnum(tag, aoff, c$, n)
Dim et, s, t$
  If tag < 0 Then et = &H91 Else et = CTag(tag) + 1
  If dByte(aoff) <> et Then
    If tag < 0 Then
      Ptrace "? Expected Enumeration(X'91') tag here"
    Else
      Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(et) + "') here"
    End If
    Perror aoff
    dEnum = True
    Exit Function
  End If
  s = dByte(aoff + 1)
  If s < n Then t$ = " (" + enumx(s) + ")" Else t$ = ""
  Ptrace LevelPrefix() + c$ + "=" + Format$(s) + t$
  Phex aoff, aoff + 1
  aoff = aoff + 2
  dEnum = False
End Function

Function dEventParameters(i, c$)
Dim t$, evt, optag
  dEventParameters = True
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  SaveHex i
  optag = optag \ 16
  i = i + 1
  evt = dByte(i)                                'this better be an open PD tag
  If (evt And &HF) <> &HE Then
    Ptrace "? Expected Open PD tag X'?E' here!"
    Perror i
    Exit Function
  End If
  evt = evt \ 16                                'get the event type
  SaveHex i
  BeginLevel c$
  AppendTrace "  --" + EventTypeName(evt)
  i = i + 1
  If dContext(0, i, "time-delay", 0, 0) Then Exit Function
  Select Case evt
    Case 0                                      'change-of-bitstring
      If dBitString(1, i, "bitmask") Then Exit Function
      If IsntPDtag(2, i, 0) Then Exit Function
      BeginLevel "list-of-bitstring-values"
      Do While i < Len(apdu)
        If dByte(i) = &H2F Then
          SaveHex i
          Exit Do
        End If
        If dBitString(-1, i, "") Then Exit Function
      Loop
      EndLevel
      i = i + 1                                 'skip past 2F
    Case 1                                      'change-of-state
      If IsntPDtag(1, i, 0) Then Exit Function
      BeginLevel "list-of-values"
      Do While i < Len(apdu)
        If dByte(i) = &H1F Then Exit Do
        If dPropertyStates(i, "") Then Exit Function
      Loop
      EndLevel
      i = i + 1                                 'skip past 1F
    Case 2                                      'change-of-value
      If IsntPDtag(1, i, 0) Then Exit Function
      If IsTag(0, i) Then
        If dBitString(0, i, "cov-criteria (bitmask)") Then Exit Function
      Else
        If dReal(1, i, "cov-criteria (referenced-property-increment)") Then Exit Function
      End If
      If IsntPDtag(1, i, 1) Then Exit Function
    Case 3                                      'command-failure
      If IsntPDtag(1, i, 0) Then Exit Function
      i = i - 1                                 'point to 1E open tag (dPropref expects it)
      If dPropref(i, "feedback-property-reference", False) Then Exit Function
    Case 4                                      'floating-limit
      If IsntPDtag(1, i, 0) Then Exit Function
      i = i - 1                                 'point to 1E open tag (dPropref expects it)
      If dPropref(i, "setpoint-reference", False) Then Exit Function
      If dReal(2, i, "low-diff-limit") Then Exit Function
      If dReal(3, i, "high-diff-limit") Then Exit Function
      If dReal(4, i, "deadband") Then Exit Function
    Case 5                                      'out-of-range
      If dReal(1, i, "low-limit") Then Exit Function
      If dReal(2, i, "high-limit") Then Exit Function
      If dReal(3, i, "deadband") Then Exit Function
  End Select
  If IsntPDtag(evt, i, 1) Then Exit Function    'each case uses paired delimiters which must match event type!
  If IsntPDtag(optag, i, 1) Then Exit Function  'better have a closing tag
  EndLevel
  dEventParameters = False
End Function

Function dEventState(tag, aoff, c$)
Dim s, t$, ul, et
  dEventState = True
  If tag < 0 Then et = &H91 Else et = CTag(tag) + 1
  ul = dByte(aoff)
  If (ul < et) Or (ul > (et + 1)) Then
    Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(et) + "' or X'" + Hex$(et + 1) + "') here"
    Perror aoff
    Exit Function
  End If
  ul = ul And 7
  If ul = 1 Then
    s = dByte(aoff + 1)
  Else
    s = dWord(aoff + 1)
  End If
  Select Case s
    Case 0
      t$ = "normal"
    Case 1
      t$ = "fault"
    Case 2
      t$ = "offnormal"
    Case 3
      t$ = "high-limit"
    Case 4
      t$ = "low-limit"
    Case 5 To 63
      t$ = "BACnet reserved event state"
    Case Else
      t$ = "proprietary event state"
  End Select
  Ptrace LevelPrefix() + c$ + "=" + Format$(s) + " (" + t$ + ")"
  Phex aoff, aoff + ul
  aoff = aoff + ul + 1
  dEventState = False
End Function

Function dEventType(tag, aoff, c$, et)
Dim t$, ul, etag
  dEventType = True
  If tag < 0 Then etag = &H91 Else etag = CTag(tag) + 1
  ul = dByte(aoff)
  If (ul < etag) Or (ul > (etag + 1)) Then
    Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(etag) + "' or X'" + Hex$(etag + 1) + "') here"
    Perror aoff
    Exit Function
  End If
  ul = ul And 7
  If ul = 1 Then
    et = dByte(aoff + 1)
  Else
    et = dWord(aoff + 1)
  End If
  Ptrace LevelPrefix() + c$ + "=" + EventTypeName(et)
  Phex aoff, aoff + ul
  aoff = aoff + ul + 1
  dEventType = False
End Function

Function dInteger(aoff, c$)
Dim ul
  dInteger = True
  ul = dByte(aoff)
  If (ul And &HF8) <> &H30 Then
    Ptrace "? Expected Signed Integer(X'3x') tag here"
    Perror aoff
    Exit Function
  End If
  ul = ul And 7                         'save length
  If (ul < 1) Or (ul > 4) Then
    Ptrace "? Invalid length for Signed Integer Tag here"
    Perror aoff
    Exit Function
  End If
  Ptrace LevelPrefix() + c$ + "=" + Format$(vbLONG(Mid$(apdu, aoff + 1, ul), ul))
  Phex aoff, aoff + ul
  aoff = aoff + ul + 1
  dInteger = False
End Function

Function dNetwork(aoff, c$, dw&)        '                           ***218 Begin
Dim ul
  dNetwork = True
  ul = dByte(aoff)
  If (ul And &HF8) <> &H20 Then
    Ptrace "? Expected Unsigned Integer(X'2x') tag here"
    Perror aoff
    Exit Function
  End If
  ul = ul And 7                         'save length
  If (ul < 1) Or (ul > 4) Then
    Ptrace "? Invalid length for Unsigned Integer Tag here"
    Perror aoff
    Exit Function
  End If
  dw& = vbDWORD(Mid$(apdu, aoff + 1, ul), ul)
  If dw& = &HFFFF& Then
    Ptrace LevelPrefix() + c$ + "=(broadcast DNET)"
  Else
    Ptrace LevelPrefix() + c$ + "=" + Format$(dw&)
  End If
  Phex aoff, aoff + ul
  aoff = aoff + ul + 1
  dNetwork = False
End Function                              '                         ***218 End

Function dNotificationParameters(tag, aoff, c$, evt, ByVal evobjt)
Dim t$, kpt
  dNotificationParameters = True
  If IsntPDtag(tag, aoff, 0) Then Exit Function    'need an open PD tag here
  'note: the IsntPDtag function advances aoff past the open PD tag!
  If (evt < 0) Or (evt > 5) Then evt = 6        'it's a complex event type
  If IsntPDtag(evt, aoff, 0) Then               'each case uses paired delimiters which must match event type!
    Ptrace "? (notificationParameters must match the eventType in this context)"
    Perror aoff
    Exit Function
  End If
  BeginLevel c$
  Select Case evt
    Case 0                                      'change-of-bitstring
      If dBitString(0, aoff, "referenced-bitstring") Then Exit Function
      If dStatusFlags(1, aoff, "status-flags") Then Exit Function
    Case 1                                      'change-of-state
      If IsntPDtag(0, aoff, 0) Then Exit Function 'new-state is inside PD tags
      If dPropertyStates(aoff, "new-state") Then Exit Function
      If IsntPDtag(0, aoff, 1) Then Exit Function
      If dStatusFlags(1, aoff, "status-flags") Then Exit Function
    Case 2                                      'change-of-value
      If IsntPDtag(0, aoff, 0) Then Exit Function
      If IsTag(0, aoff) Then
        If dBitString(0, aoff, "changed-bits") Then Exit Function
      Else
        If dReal(1, aoff, "changed-value") Then Exit Function
      End If
      If IsntPDtag(0, aoff, 1) Then Exit Function
      If dStatusFlags(1, aoff, "status-flags") Then Exit Function
    Case 3                                      'command-failure
      If (evobjt = 0) Or (evobjt = 1) Or (evobjt = 2) Then kpt = ptFloat Else kpt = ptUD
      If IsTag(0, aoff) Then
        dAny aoff, "command-value", False, kpt, 0 'ABSTRACT-SYNTAX.&Type
      Else
        Ptrace "? (expected context tag 0 (command-value) in this context)"
        Perror aoff
        Exit Function
      End If
      If dStatusFlags(1, aoff, "status-flags") Then Exit Function
      If IsTag(2, aoff) Then
        dAny aoff, "feedback-value", False, kpt, 0    'ABSTRACT-SYNTAX.&Type
      Else
        Ptrace "? (expected context tag 2 (feedback-value) in this context)"
        Perror aoff
        Exit Function
      End If
    Case 4                                      'floating-limit
      If dReal(0, aoff, "reference-value") Then Exit Function
      If dStatusFlags(1, aoff, "status-flags") Then Exit Function
      If dReal(2, aoff, "setpoint-value") Then Exit Function
      If dReal(3, aoff, "error-limit") Then Exit Function
    Case 5                                      'out-of-range
      If dReal(0, aoff, "exceeding-value") Then Exit Function
      If dStatusFlags(1, aoff, "status-flags") Then Exit Function
      If dReal(2, aoff, "deadband") Then Exit Function
      If dReal(3, aoff, "exceeded-limit") Then Exit Function
    Case Else                                   'complex event type
      BeginLevel c$
      Do While aoff <= Len(apdu)
        If dByte(aoff) = &H6F Then
          If IsntPDtag(6, aoff, 1) Then Exit Function
          EndLevel
          GoTo dNP99                            'found closing delimiter
        End If
        LastObjType = -1                        'unknown object type
        If PropertyValue(aoff) Then Exit Function
      Loop
      IndentLevel = IndentLevel - 1
  End Select
dNP99:
  If IsntPDtag(evt, aoff, 1) Then Exit Function 'each case uses paired delimiters which must match event type!
  If IsntPDtag(tag, aoff, 1) Then Exit Function 'better have a closing tag
  EndLevel
  dNotificationParameters = False
End Function

Function dNotifyType(tag, aoff, c$, ae)
Dim s, t$, et
  dNotifyType = True
  et = CTag(tag) + 1
  If dByte(aoff) <> et Then
    Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(et) + "') here"
    Perror aoff
    Exit Function
  End If
  s = dByte(aoff + 1)
  ae = False
  Select Case s
    Case 0
      t$ = "alarm"
      ae = True
    Case 1
      t$ = "event"
      ae = True
    Case 2
      t$ = "ack-notification"
    Case Else
      t$ = "unknown notify type"
  End Select
  Ptrace LevelPrefix() + c$ + "=" + Format$(s) + " (" + t$ + ")"
  Phex aoff, aoff + 1
  aoff = aoff + 2
  dNotifyType = False
End Function

'decode an ObjectIdentifier primitive
'in:    tag     -1 if primitive data type tag (0xC4) else context tag
Function dObjectIdentifier(tag, aoff, c$)
Dim et
  If tag < 0 Then et = &HC4 Else et = CTag(tag) + 4
  If dByte(aoff) <> et Then
    If tag < 0 Then
      Ptrace "? Expected Object Identifier(X'C4') tag here"
    Else
      Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(et) + "') here"
    End If
    Perror aoff
    dObjectIdentifier = True
    Exit Function
  End If
  Ptrace LevelPrefix() + c$ + "=" + ObjectIdentifier(aoff + 1)
  Phex aoff, aoff + 4
  aoff = aoff + 5
  dObjectIdentifier = False
End Function

Function dObjectType(tag, aoff, c$)
Dim ul, et, v As Long
  dObjectType = True
  et = ((tag * 16) + &H8)
  ul = dByte(aoff)
  If (ul And &HF8) <> et Then
    Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(et) + "') here"
    Perror aoff
    Exit Function
  End If
  ul = ul And 7                         'save length
  If (ul < 1) Or (ul > 2) Then
    Ptrace "? Invalid length for Enumeration here"
    Perror aoff
    Exit Function
  End If
  v = vbDWORD(Mid$(apdu, aoff + 1, ul), ul)
  LastObjType = v
  Ptrace LevelPrefix() + c$ + "=" + Format$(v) + " (" + ObjectType(v) + ")"
  Phex aoff, aoff + ul
  aoff = aoff + ul + 1
  dObjectType = False
End Function

Function dOctetString(i, c$, e$, trychars)
Dim ul As Long, j, si                   '                                     ***218
  dOctetString = True
  si = i
  ul = dByte(i)
  i = i + 1
  If (ul And &HF8) <> &H60 Then
    Ptrace "? Expected OctetString(X'6x') tag here"
    Perror si
    Exit Function
  End If
  ul = ul And 7                         'save length
  If (ul > 5) Then                      '                                     ***218
    Ptrace "? Invalid length for OctetString Tag here"
    Perror si
    Exit Function
  End If
  If ul = 5 Then                        'extended length
    ul = dByte(i) And &HFF&
    i = i + 1
    If ul = 254 Then                    'word of extended length
      ul = dWord(i) And &HFFFF&
      i = i + 2
    ElseIf ul = 255 Then                'dword of extended length
      ul = dDword(i)
      i = i + 4
    End If
  End If                                '                                     ***218 End
  If (ul = 0) And (e$ <> "") Then
    Ptrace LevelPrefix() + c$ + "=" + e$
  Else
    Ptrace LevelPrefix() + c$ + "=" + OctetString(i, ul)
  End If
  Phex si, (i + ul) - 1
  If trychars Then
    Ptrace LevelPrefix() + Chr$(34) + FilterUnprintables(Mid$(apdu, i, ul)) + Chr$(34)
    Phex 0, 0
  End If
  i = i + ul
  dOctetString = False
End Function

Function dPrimitive(i, pet) As String
Dim l As Long, j, b, k, tag, tlv, n$
Dim lv As Long, jl As Long, pn As String * 64
    n$ = ""
    tlv = dByte(i)
    i = i + 1
    tag = tlv \ 16
    If tag = 15 Then                            'extended tag
      tag = dByte(i)
      i = i + 1
    End If
    If (tlv And 8) Then
      l = 7                                     'force an error
    Else
      l = tlv And 7                             'length
    End If
    If l = 5 Then                               'extended length
      l = dByte(i)
      i = i + 1
      If l = 254 Then
        l = dWord(i)
        i = i + 2
      ElseIf l = 255 Then
        l = vbDWORD(Mid$(apdu, i, 4), 4)
        i = i + 4
      End If
    ElseIf (l > 5) Then                         'context or open/close tag
      dPrimitive = "(? expected primitive tag here)"
      Exit Function
    End If
    Select Case tag
        Case 0                                  'NULL
          n$ = "NULL"
        Case 1                                  'Boolean
          If l Then n$ = "TRUE" Else n$ = "FALSE"
          l = 0
        Case 2                                  'Unsigned Integer
          n$ = Format$(vbDWORD(Mid$(apdu, i, l), l))
        Case 3                                  'Signed Integer
          n$ = Format$(vbLONG(Mid$(apdu, i, l), l))
        Case 4                                  'Real
          n$ = Format$(vbREAL(Mid$(apdu, i, 4)))
        Case 5                                  'Double
          n$ = Format$(vbDOUBLE(Mid$(apdu, i, 8)))
        Case 6                                  'Octet string
          n$ = OctetString(i, l)
        Case 7                                  'Character string
          k = l - 1                             'actual string length
          j = dByte(i)                          'string type code
          i = i + 1
          If j > 1 Then
            Ptrace "? String Type Not Currently Supported (" + Format$(j) + ")"
            Perror i - 1
            Exit Function
          End If
          If j = 0 Then                         'regular ANSI string
            n$ = Chr$(34)                       'open quote
          Else                                  'DBCS string
            b = dWord(i)
            i = i + 2
            n$ = "[" + Format$(b) + "]" + Chr$(34)
            k = k - 2                         'account for code page
          End If
          n$ = n$ + Mid$(apdu, i, k) + Chr$(34)
          i = i + k
          l = 0
        Case 8                                  'Bit String
          n$ = "("
          b = &H80
          tlv = dByte(i)
          l = l - 1
          i = i + 1
          If l Then
            k = dByte(i)
            i = i + 1
            For j = 1 To (l * 8) - tlv          'number of bits to do
              If k And b Then n$ = n$ + "TRUE," Else n$ = n$ + "FALSE,"
              b = b \ 2
              If b = 0 Then
                b = &H80
                k = dByte(i)
                i = i + 1
              End If
            Next j
          End If
          If Right$(n$, 1) = "," Then n$ = Left$(n$, Len(n$) - 1)
          n$ = n$ + ")"
          l = 0
        Case 9                                  'enumeration
          lv = vbDWORD(Mid$(apdu, i, l), l)     'get the enumeration value
          If pet Then                           'we have a table to use
            j = VTSAPIgetenumtable(pet, lv, j, jl, pn)
            n$ = Format$(lv) + " (" + Left$(pn, InStr(pn, Chr$(0)) - 1) + ")"
          Else                                  'must be dumb
            n$ = "(Enum," + Format$(lv) + ")"
          End If
        Case 10                                 'Date
          j = dByte(i + 2)                      'day of month
          If j = 255 Then n$ = "(?," Else n$ = "(" + Format$(j) + ","
          j = dByte(i + 1)                      'month
          If j = 255 Then n$ = n$ + "?," Else n$ = n$ + Left$(MonthName(j), 3) + ","
          j = dByte(i)                          'year
          If j = 255 Then n$ = n$ + "?," Else n$ = n$ + Format$(1900 + j) + ","
          j = dByte(i + 3)                      'day of week
          If j = 255 Then n$ = n$ + "?)" Else n$ = n$ + DayName(j) + ")"
        Case 11                                 'Time
          j = dByte(i)                          'hours
          If j = 255 Then n$ = "(?," Else n$ = "(" + Format$(j) + ":"
          j = dByte(i + 1)                      'minutes
          If j = 255 Then n$ = n$ + "?," Else n$ = n$ + Format$(j) + ":"
          j = dByte(i + 2)                      'seconds
          If j = 255 Then n$ = n$ + "?," Else n$ = n$ + Format$(j) + "."
          j = dByte(i + 3)                      'day of week
          If j = 255 Then n$ = n$ + "?)" Else n$ = n$ + Format$(j) + ")"
        Case 12                                 'ObjectIdentifier
          n$ = ObjectIdentifier(i)
        Case Else
          n$ = "(? BACnet reserved tag" + Str$(tag) + " is not allowed here)"
    End Select
    i = i + l                                   'advance to the end of this tagged item
    dPrimitive = n$
End Function

Function dProperty(tag, i, c$, needname, n$, PropId As Long, ari As Long) ' ***218
Dim ul, ct, si, p$
  dProperty = True
  ari = &HFFFFFFFF                      'assume no array index              ***218
  si = i
  ul = dByte(i)
  ct = CTag(tag) + 1
  If (ul < ct) Or (ul > (ct + 3)) Then
    Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(ct) + "'..X'" + Hex$(ct + 3) + "') here"
    Perror i
    Exit Function
  End If
  ul = ul And 7                         'save length
  PropId = vbDWORD(Mid$(apdu, i + 1, ul), ul)
  i = i + ul + 1
  If PropId < 124 Then
    p$ = PropName(PropId)
  ElseIf PropId > 511 Then
    p$ = "(proprietary" + Str$(PropId) + ")"
  Else
    p$ = "(BACnet reserved property" + Str$(PropId) + ")"
  End If
  If IsTag(tag + 1, i) Then             'we have an array index
    ul = dByte(i) And 7
    If (ul > 0) And (ul < 5) Then
      ari = vbDWORD(Mid$(apdu, i + 1, ul), ul)  '                           ***218
      p$ = p$ + Format$(ari, "\[0\]")           '                           ***218
      i = i + ul + 1
    End If
  End If
  If needname Then
    n$ = p$
    For ct = si To i - 1
      SaveHex ct
    Next ct
  Else
    If c$ = "" Then
      Ptrace LevelPrefix() + p$
    Else
      Ptrace LevelPrefix() + c$ + "=" + p$
    End If
    Phex si, i - 1
  End If
  dProperty = False
End Function

Function dPropertyStates(i, c$)
Dim j, ct, ps, v As Long, es As String * 32, p$
  dPropertyStates = True
  If c$ = "" Then p$ = "" Else p$ = c$ + "="
  ps = dByte(i)
  ct = ps And &HF
  ps = ps \ 16
  If (ct < &H9) Or (ct > &HC) Then
    Ptrace "? Expected Context tag X'?9'..X'?C' here!"
    Perror i
    Exit Function
  End If
  ct = ct And 7                         'length
  v = vbDWORD(Mid$(apdu, i + 1, ct), ct)
  If ct < &HB Then                      'it's a valid length
    If VTSAPIgetpropertystate(ps, v, es) Then
      Ptrace LevelPrefix() + p$ + RTrim$(es)
      j = False
    Else
      j = True
    End If
  Else
    j = True
  End If
  If j Then                             'invalid length for enumeration value
    Ptrace LevelPrefix() + p$ + "(PropertyStates type" + Str$(ps) + "," + Format$(v) + ")"
  End If
  Phex i, i + ct
  i = i + ct + 1
  dPropertyStates = False
End Function

'try to decode a property reference within paired delimiter context tags
'in:    i       index into apdu
'       c$      parameter name
'       many    true if more than one allowed
'out:   true    if bogus
Function dPropref(i, c$, many)
Dim ul, ct, si, p$, optag, PropId As Long
  dPropref = True
  si = i
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  If many Then
    Ptrace LevelPrefix() + c$ + "=("
    IndentLevel = IndentLevel + 1
    Phex i, i
    si = i + 1
    c$ = ""
  End If
  i = i + 1
  Do While i < Len(apdu)
    If dByte(i) = (optag + 1) Then Exit Do
    If dByte(i) <> &HC Then
      Ptrace "? Expected Context Tag X'0C' here"
      Perror i
      Exit Function
    End If
    p$ = ObjectIdentifier(i + 1) + "."
    i = i + 5
  
    ul = dByte(i)
    ct = CTag(1) + 1
    If (ul < ct) Or (ul > (ct + 3)) Then
      Ptrace "? Expected Context Tag 1 (X'" + Hex$(ct) + "'..X'" + Hex$(ct + 3) + "') here"
      Perror i
      Exit Function
    End If

    ul = ul And 7                               'save length
    PropId = vbDWORD(Mid$(apdu, i + 1, ul), ul)
    i = i + ul + 1
    If PropId < 124 Then
      p$ = p$ + PropName(PropId)
    ElseIf PropId > 511 Then
      p$ = p$ + "(proprietary" + Str$(PropId) + ")"
    Else
      p$ = p$ + "(BACnet reserved property" + Str$(PropId) + ")"
    End If
    If IsTag(2, i) Then                         'we have an array index
      ul = dByte(i) And 7
      If (ul > 0) And (ul < 5) Then
        p$ = p$ + Format$(vbDWORD(Mid$(apdu, i + 1, ul), ul), "\[0\]")
        i = i + ul + 1
      End If
    End If
    If many Then                                'more than one allowed
      If dByte(i) = (optag + 1) Then            'last one
        p$ = p$ + ")"
      Else
        p$ = p$ + ","
      End If
    End If
    If c$ = "" Then
      Ptrace LevelPrefix() + p$
    Else
      Ptrace LevelPrefix() + c$ + "=" + p$
    End If
    Phex si, i - 1
    c$ = ""
    si = i
    If Not many Then Exit Do
  Loop
  If many Then IndentLevel = IndentLevel - 1
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function
  Pflush
  dPropref = False
End Function

'try to decode a property value in context of the property ID
'in:    i       index into apdu
'       c$      name of parameter
'       propid  the property identifier to use in context
'       ari     property array index or &hFFFFFFFF                              ***218
Sub dPropVal(i, c$, PropId As Long, ByVal ari&) '                               ***218
Dim ptype, pet, optag, j                        '                               ***218

  If VTSAPIgetdefaultpropinfo(LastObjType, PropId, ptype, pet) = 0 Then
    ptype = 0                                   'no info for this prop
    pet = 0
  End If
  Select Case ptype                             'some cases we can handle cleverly
    Case ptAct                                  'Cmd obj: Action is an array of (list of BACnetActionCommands)
      If dActionList(i, c$) Then Exit Sub
    Case ptXsched                               'Sched obj: exception-schedule is an array of SpecialEvents
      If dSpecialEvents(i, c$, pet) Then Exit Sub
    Case ptWsched                               'Sched obj: weekly-schedule is an array of (list of TimeVals)
      optag = dByte(i)                          'save opening tag
      If (optag And 15) <> &HE Then
        Ptrace "? Expected an Open Context Tag here"
        Perror i
        Exit Sub
      End If
      SaveHex i
      i = i + 1
      '                                                                       ***218 Begin
      If ari = &HFFFFFFFF Then                  'we've got the whole array
        BeginLevel c$
        For j = 1 To 7                          'array elements
          c$ = "[" + Format$(j) + "]"
          If dTimeVals(i, c$, pet) Then Exit Sub
        Next j
        EndLevel
      ElseIf ari = 0 Then                       'just the count m'am
        If dUnsigned(i, c$, ari) Then Exit Sub
        If ari <> 7 Then
          Ptrace "? Expected array length=7 here"
          Perror i
        End If
      Else                                      'a single array item
          If dTimeVals(i, c$, pet) Then Exit Sub
      End If                                    '                             ***218 End
      If IsntPDtag(optag \ 16, i, 1) Then Exit Sub
      Pflush
    Case ptDtrange                              'range of dates
      If dDateRange(i, c$) Then Exit Sub
    Case ptPropref                              'EE obj: object property reference
      If dPropref(i, c$, False) Then Exit Sub
    Case ptEvparm                               'EE obj: event parameters
      If dEventParameters(i, c$) Then Exit Sub
    Case ptCalist                               'Calendar obj: datelist property
      If dCalendarEntries(i, c$) Then Exit Sub
    Case ptSkeys                                'Device obj: list of session keys
      If dSessionKeys(i, c$) Then Exit Sub
    Case ptTsrecip, ptRecip                     'Device obj: time synch recipients
      If dRecipients(i, c$) Then Exit Sub
    Case ptDabind                               'Device obj: device address bindings
      If dAddressBindings(i, c$) Then Exit Sub
    Case ptRaslist                              'Group obj: list of read access specs
      If dReadAccessSpecs(i, c$) Then Exit Sub
    Case ptLopref                               'Sched obj: list of prop refs
      If dPropref(i, c$, True) Then Exit Sub
    Case Else                                   'some cases we just let dAny handle
      dAny i, c$, True, ptype, pet
  End Select
End Sub

'try to decode a property reference within paired delimiter context tags
'in:    i       index into apdu
'       c$      parameter name
'out:   true    if bogus
Function dReadAccessSpecs(i, c$)
Dim ul, ct, si, p$, optag, PropId As Long
  dReadAccessSpecs = True
  si = i
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  Ptrace LevelPrefix() + c$ + "=("
  IndentLevel = IndentLevel + 1
  Phex i, i
  i = i + 1
  si = i
  Do While i < Len(apdu)
    ul = dByte(i)
    If ul = (optag + 1) Then Exit Do
    If ul <> &HC Then
      Ptrace "? Expected Open Context Tag 0 here"
      Perror i
      Exit Function
    End If
    p$ = ObjectIdentifier(i + 1) + ",("
    i = i + 5
    Ptrace LevelPrefix() + p$
    Phex si, i - 1
    si = i
    IndentLevel = IndentLevel + 1
    If IsntPDtag(1, i, 0) Then Exit Function
    Do While i < Len(apdu)
      ul = dByte(i)
      If ul = &H1F Then Exit Do
      ct = CTag(0) + 1
      If (ul < ct) Or (ul > (ct + 3)) Then
        Ptrace "? Expected Context Tag 0 (X'" + Hex$(ct) + "'..X'" + Hex$(ct + 3) + "') here"
        Perror i
        Exit Function
      End If

      ul = ul And 7                               'save length
      PropId = vbDWORD(Mid$(apdu, i + 1, ul), ul)
      i = i + ul + 1
      If PropId < 124 Then
        p$ = PropName(PropId)
      ElseIf PropId > 511 Then
        p$ = "(proprietary" + Str$(PropId) + ")"
      Else
        p$ = "(BACnet reserved property" + Str$(PropId) + ")"
      End If
      If IsTag(1, i) Then                         'we have an array index
        ul = dByte(i) And 7
        If (ul > 0) And (ul < 5) Then
          p$ = p$ + Format$(vbDWORD(Mid$(apdu, i + 1, ul), ul), "\[0\]")
          i = i + ul + 1
        End If
      End If
      If dByte(i) = &H1F Then                   'last one
        p$ = p$ + ")"
      Else
        p$ = p$ + ","
      End If
      Ptrace LevelPrefix() + p$
      Phex si, i - 1
      si = i
    Loop
    If IsntPDtag(1, i, 1) Then Exit Function
    IndentLevel = IndentLevel - 1
  Loop
  EndLevel
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function
  Pflush
  dReadAccessSpecs = False
End Function

'decode a Real number primitive
'in:    tag     -1 if primitive data type tag (0x44) else context tag
Function dReal(tag, aoff, c$)
Dim et
  If tag < 0 Then et = &H44 Else et = CTag(tag) + 4
  If dByte(aoff) <> et Then
    If tag < 0 Then
      Ptrace "? Expected Real(X'44') tag here"
    Else
      Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(et) + "') here"
    End If
    Perror aoff
    dReal = True
    Exit Function
  End If
  Ptrace LevelPrefix() + c$ + "=" + Format$(vbREAL(Mid$(apdu, aoff + 1, 4)))
  Phex aoff, aoff + 4
  aoff = aoff + 5
  dReal = False
End Function

Function dRecipients(i, c$)
Dim nn As Long, si, p$, optag, ul
  dRecipients = True
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  Ptrace LevelPrefix() + c$ + "=("
  IndentLevel = IndentLevel + 1
  Phex i, i
  i = i + 1
  si = i
  Do While i < Len(apdu)
    If dByte(i) = (optag + 1) Then Exit Do
    If dByte(i) = &HC Then
      p$ = ObjectIdentifier(i + 1)
      If LastObjType <> 8 Then p$ = p$ + ",  --Warning: Recipient is Not a Device Object!"
      i = i + 5
    Else
      If IsntPDtag(1, i, 0) Then Exit Function
      ul = dByte(i)
      If (ul And &HF8) <> &H20 Then
        Ptrace "? Expected Unsigned Integer(X'2x') tag here"
        Perror i
        Exit Function
      End If
      ul = ul And 7                         'save length
      If (ul < 1) Or (ul > 2) Then
        Ptrace "? Invalid length for Unsigned Integer Tag here"
        Perror i
        Exit Function
      End If
      nn = vbDWORD(Mid$(apdu, i + 1, ul), ul)
      p$ = "(" + Format$(nn) + ","
      i = i + ul + 1

      ul = dByte(i)
      If (ul And &HF8) <> &H60 Then
        Ptrace "? Expected OctetString(X'6x') tag here"
        Perror i
        Exit Function
      End If
      i = i + 1
      ul = ul And 7                         'save length
      If (ul > 6) Then
        Ptrace "? Invalid length for OctetString Tag here"
        Perror i
        Exit Function
      End If
      If ul = 5 Then                        'extended length
        ul = dByte(i)
        i = i + 1
      ElseIf ul = 6 Then
        ul = dWord(i)
        i = i + 2
      End If
      If ul = 0 Then
        p$ = p$ + "broadcast)"
      Else
        p$ = p$ + OctetString(i, ul) + ")"
      End If
      i = i + ul
      If IsntPDtag(1, i, 1) Then Exit Function
    End If

    If dByte(i) = (optag + 1) Then            'last one
      p$ = p$ + ")"
    Else
      p$ = p$ + ","
    End If
    Ptrace LevelPrefix() + p$
    Phex si, i - 1
    si = i
  Loop
  IndentLevel = IndentLevel - 1
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function
  Pflush
  dRecipients = False
End Function

Function dSegmentation(aoff, c$)
Dim s, t$
  If dByte(aoff) <> &H91 Then
    Ptrace "? Expected Enumeration(X'91') tag here"
    Perror aoff
    dSegmentation = True
    Exit Function
  End If
  s = dByte(aoff + 1)
  Select Case s
    Case 0
      t$ = "segmented-both"
    Case 1
      t$ = "segmented-transmit"
    Case 2
      t$ = "segmented-receive"
    Case 3
      t$ = "no-segmentation"
    Case Else
      t$ = "unknown segmentation!"
  End Select
  Ptrace LevelPrefix() + c$ + "=" + Format$(s) + " (" + t$ + ")"
  Phex aoff, aoff + 1
  aoff = aoff + 2
  dSegmentation = False
End Function

Function dSessionKeys(i, c$)
Dim nn As Long, si, p$, optag, ul
  dSessionKeys = True
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  Ptrace LevelPrefix() + c$ + "=("
  IndentLevel = IndentLevel + 1
  Phex i, i
  i = i + 1
  si = i
  Do While i < Len(apdu)
    If dByte(i) = (optag + 1) Then Exit Do
    If dWord(i) <> &H6508 Then
      Ptrace "? Expected OctetString(X'6508') tag here"
      Perror i
      Exit Function
    End If
    p$ = "(" + OctetString(i + 2, 8) + ","
    i = i + 10
    
    ul = dByte(i)
    If (ul And &HF8) <> &H20 Then
      Ptrace "? Expected Unsigned Integer(X'2x') tag here"
      Perror i
      Exit Function
    End If
    ul = ul And 7                         'save length
    If (ul < 1) Or (ul > 2) Then
      Ptrace "? Invalid length for Unsigned Integer Tag here"
      Perror i
      Exit Function
    End If
    nn = vbDWORD(Mid$(apdu, i + 1, ul), ul)
    p$ = p$ + Format$(nn) + ","
    i = i + ul + 1

    ul = dByte(i)
    If (ul And &HF8) <> &H60 Then
      Ptrace "? Expected OctetString(X'6x') tag here"
      Perror i
      Exit Function
    End If
    i = i + 1
    ul = ul And 7                         'save length
    If (ul > 6) Then
      Ptrace "? Invalid length for OctetString Tag here"
      Perror i
      Exit Function
    End If
    If ul = 5 Then                        'extended length
      ul = dByte(i)
      i = i + 1
    ElseIf ul = 6 Then
      ul = dWord(i)
      i = i + 2
    End If
    If ul = 0 Then
      p$ = p$ + "broadcast"
    Else
      p$ = p$ + OctetString(i, ul)
    End If
    i = i + ul
    If dByte(i) = (optag + 1) Then            'last one
      p$ = p$ + "))"
    Else
      p$ = p$ + "),"
    End If
    Ptrace LevelPrefix() + p$
    Phex si, i - 1
    si = i
  Loop
  IndentLevel = IndentLevel - 1
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function
  Pflush
  dSessionKeys = False
End Function

Function dSpecialEvents(i, c$, pet)
Dim nn As Long, p$, optag, ul
  dSpecialEvents = True
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  SaveHex i
  i = i + 1
  optag = optag + 1                             'remember closing tag
  BeginLevel c$
  Do While i < Len(apdu)
    If dByte(i) = optag Then Exit Do            'found closing tag
    If dByte(i) = &H1C Then                     'calendarReference is choice [1]
      If dObjectIdentifier(1, i, "period.calendarReference") Then Exit Function
    Else
      If IsntPDtag(0, i, 0) Then Exit Function
      SavedHex = ""                             'don't need this
      i = i - 1                                 'back up to open tag (dCalendarEntries expects it)
      If dCalendarEntries(i, "period.calendarEntry") Then Exit Function
    End If
    If dTimeVals(i, "listOfTimeValues", pet) Then Exit Function
    If dContext(3, i, "eventPriority", 1, 16) Then Exit Function
  Loop
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function
  EndLevel
  dSpecialEvents = False
End Function

Function dStatusFlags(tag, aoff, c$)
Dim et, b, v$
  dStatusFlags = True
  If tag < 0 Then et = &H82 Else et = CTag(tag) + 2
  If dByte(aoff) <> et Then
    If tag < 0 Then
      Ptrace "? Expected BitString(X'82') tag here"
    Else
      Ptrace "? Expected Context Tag" + Str$(tag) + " (X'" + Hex$(et) + "') here"
    End If
    Perror aoff
    Exit Function
  End If
  v$ = ""
  If dByte(aoff + 1) <> 4 Then                  'first octet
    Ptrace "? Expected StatusFlags bitstring to have 4 unused bits"
    Perror aoff
    Exit Function
  End If
  b = dByte(aoff + 2)
  If b And &H80 Then v$ = v$ + "in-alarm,"
  If b And &H40 Then v$ = v$ + "fault,"
  If b And &H20 Then v$ = v$ + "overridden,"
  If b And &H10 Then v$ = v$ + "out-of-service,"
  If v$ <> "" Then v$ = Left$(v$, Len(v$) - 1)
  Ptrace LevelPrefix() + c$ + "=(" + v$ + ")"
  Phex aoff, aoff + 2
  aoff = aoff + 3
  dStatusFlags = False
End Function

Function dTime(aoff, c$, trailPD)
  If dByte(aoff) <> &HB4 Then
    Ptrace "? Expected Time(X'B4') tag here"
    Perror aoff
    dTime = True
    Exit Function
  End If
  Ptrace LevelPrefix() + c$ + "=" + TimeString(aoff + 1)
  Phex aoff, aoff + 4
  aoff = aoff + 5
  If trailPD >= 0 Then
    If IsntPDtag(trailPD, aoff, 1) Then Exit Function
  End If
dTime = False
End Function

Function dTimeStamp(tag, aoff, c$)
Dim t
  dTimeStamp = True
  If IsntPDtag(tag, aoff, 0) Then Exit Function
  t = dByte(aoff)                               'get tag
  If t = &HC Then                               'time choice
    Ptrace LevelPrefix() + c$ + "=" + TimeString(aoff + 1)
    Phex aoff, aoff + 4
    aoff = aoff + 5
  ElseIf (t = &H19) Or (t = &H1A) Then          'sequence number
    If dContext(1, aoff, c$, 0, 65535) Then Exit Function
  ElseIf t = &H2E Then                          'DateTime
    If IsntPDtag(2, aoff, 0) Then Exit Function
    BeginLevel "timeOfAcknowledgement"
    If dDate(aoff, "date") Then Exit Function
    If dTime(aoff, "time", 2) Then Exit Function
  Else
    Ptrace "? Expected Context Tag 0(time), 1(sequenceNumber) or 2(dateTime) here"
    Perror aoff
    Exit Function
  End If
  If IsntPDtag(tag, aoff, 1) Then Exit Function
  If t = &H2E Then EndLevel
  dTimeStamp = False
End Function

Function dTimeVals(i, c$, pet)
Dim nn As Long, si, p$, optag, ul
  dTimeVals = True
  optag = dByte(i)                              'save opening tag
  If (optag And 15) <> &HE Then
    Ptrace "? Expected an Open Context Tag here"
    Perror i
    Exit Function
  End If
  SaveHex i
  i = i + 1

  BeginLevel c$
  si = i
  Do While i < Len(apdu)
    If dByte(i) = (optag + 1) Then Exit Do      'found closing tag
    If dByte(i) <> &HB4 Then
      Ptrace "? Expected Time(X'B4') tag here"
      Perror i
      Exit Function
    End If
    p$ = "(" + TimeString(i + 1) + ","
    i = i + 5
    p$ = p$ + dPrimitive(i, pet)
    If dByte(i) = (optag + 1) Then              'last one
      p$ = p$ + ")"
    Else
      p$ = p$ + "),"
    End If
    Ptrace LevelPrefix() + p$
    Phex si, i - 1
    si = i
  Loop
  If IsntPDtag(optag \ 16, i, 1) Then Exit Function
  EndLevel
  dTimeVals = False
End Function

Function dUnsigned(aoff, c$, dw&)
Dim ul
  dUnsigned = True
  ul = dByte(aoff)
  If (ul And &HF8) <> &H20 Then
    Ptrace "? Expected Unsigned Integer(X'2x') tag here"
    Perror aoff
    Exit Function
  End If
  ul = ul And 7                         'save length
  If (ul < 1) Or (ul > 4) Then
    Ptrace "? Invalid length for Unsigned Integer Tag here"
    Perror aoff
    Exit Function
  End If
  dw& = vbDWORD(Mid$(apdu, aoff + 1, ul), ul)
  Ptrace LevelPrefix() + c$ + "=" + Format$(dw&)
  Phex aoff, aoff + ul
  aoff = aoff + ul + 1
  dUnsigned = False
End Function

Function dDword(aoff) As Long
  dDword = vbDWORD(Mid$(apdu, aoff, 4), 4)
End Function

Function dWord(aoff)
  dWord = vbWORD(Mid$(apdu, aoff, 2))
End Function

Sub EndLevel()
  Ptrace LevelPrefix() + ")"
  Phex 0, 0
  IndentLevel = IndentLevel - 1
End Sub

Function ErrorClass(ByVal r As Long) As String
  Select Case r
    Case 0
      ErrorClass = "device"
    Case 1
      ErrorClass = "object"
    Case 2
      ErrorClass = "property"
    Case 3
      ErrorClass = "resources"
    Case 4
      ErrorClass = "security"
    Case 5
      ErrorClass = "services"
    Case 6
      ErrorClass = "vt"
    Case 7 - 63
      ErrorClass = "BACnet reserved errorclass" + Str$(r)
    Case Else
      ErrorClass = "proprietary errorclass" + Str$(r)
  End Select
End Function

Function ErrorCode(ByVal r As Long) As String
  Select Case r
    Case 0
      ErrorCode = "other"
    Case 1
      ErrorCode = "authentication-failed"
    Case 2
      ErrorCode = "configuration-in-progress"
    Case 3
      ErrorCode = "device-busy"
    Case 4
      ErrorCode = "dynamic-creation-not-supported"
    Case 5
      ErrorCode = "file-access-denied"
    Case 6
      ErrorCode = "incompatible-security-levels"
    Case 7
      ErrorCode = "inconsistent parameters"
    Case 8
      ErrorCode = "inconsistent selection criterion"
    Case 9
      ErrorCode = "invalid data type"
    Case 10
      ErrorCode = "invalid-file-access-method"
    Case 11
      ErrorCode = "invalid-file-start-position"
    Case 12
      ErrorCode = "invalid-operator-name"
    Case 13
      ErrorCode = "invalid parameter data type"
    Case 14
      ErrorCode = "invalid-time-stamp"
    Case 15
      ErrorCode = "key-generation-error"
    Case 16
      ErrorCode = "missing required parameter"
    Case 17
      ErrorCode = "no objects of specified type"
    Case 18
      ErrorCode = "no space for object"
    Case 19
      ErrorCode = "no-space-to-add-list-element"
    Case 20
      ErrorCode = "no-space-to-write-property"
    Case 21
      ErrorCode = "no vt sessions available"
    Case 22
      ErrorCode = "property-is-not-a-list"
    Case 23
      ErrorCode = "object-deletion-not-permitted"
    Case 24
      ErrorCode = "object-identifier-already-exists"
    Case 25
      ErrorCode = "operational problem"
    Case 26
      ErrorCode = "password-failure"
    Case 27
      ErrorCode = "read access denied"
    Case 28
      ErrorCode = "security-not-supported"
    Case 29
      ErrorCode = "service request denied"
    Case 30
      ErrorCode = "timeout"
    Case 31
      ErrorCode = "unknown object"
    Case 32
      ErrorCode = "unknown property"
    Case 33
      ErrorCode = "unknown service"
    Case 34
      ErrorCode = "unknown vt class"
    Case 35
      ErrorCode = "unknown vt session"
    Case 36
      ErrorCode = "unsupported object type"
    Case 37
      ErrorCode = "value-out-of-range"
    Case 38
      ErrorCode = "vt session already closed"
    Case 39
      ErrorCode = "vt session termination failure"
    Case 40
      ErrorCode = "write access denied"
    Case 41
      ErrorCode = "character-set-not-supported"
    Case 42
      ErrorCode = "invalid-array-index"
    Case 43 - 255
      ErrorCode = "BACnet reserved errorcode" + Str$(r)
    Case Else
      ErrorCode = "proprietary errorcode" + Str$(r)
  End Select
End Function

Sub ErrorHeader(c$, aoff, needpd)
  If needpd Then
    If IsntPDtag(0, aoff, 0) Then Exit Sub
  End If
  Ptrace c$ + ErrorClass(GetTagValue(aoff)) + ":" + ErrorCode(GetTagValue(aoff))
  Phex 1, aoff - 1
  If needpd Then
    If IsntPDtag(0, aoff, 1) Then Exit Sub
  End If
End Sub

Sub ErrorReason()
Dim c$, i, j, svc, PropId As Long, ari&
  svc = dByte(3)
  c$ = "Error: invID=" + Format$(dByte(2)) + ", " + ConfirmedService(svc) + ", "
  i = 4

  Select Case svc
    Case 8, 9, 10                               'addListElement,removeListElement,createObject
      ErrorHeader c$, i, True
      If dContext(1, i, "firstFailedElementNumber", 0, 0) Then Exit Sub
    Case 16                                     'writePropertyMultiple
      ErrorHeader c$, i, True
      If IsntPDtag(1, i, 0) Then Exit Sub
        If dObjectIdentifier(0, i, "objectIdentifier") Then Exit Sub
        If dProperty(1, i, "firstFailedWriteAttempt", False, c$, PropId, ari&) Then Exit Sub '***218
      If IsntPDtag(1, i, 1) Then Exit Sub
    Case 18                                     'confirmedPrivateTransfer
      ErrorHeader c$, i, True
      If dContext(1, i, "vendorID", 0, 0) Then Exit Sub
      If dContext(2, i, "serviceNumber", 0, 0) Then Exit Sub
      If dByte(i) = &H2E Then
        If IsntPDtag(2, i, 0) Then Exit Sub
        dAny i, "serviceParameters", False, 0, 0    'ABSTRACT-SYNTAX.&Type
        If IsntPDtag(2, i, 1) Then Exit Sub
      End If
    Case 22                                     'vtClose
      ErrorHeader c$, i, True
      If i > Len(apdu) Then Exit Sub
      c$ = LevelPrefix() + "listOfVTSessionIdentifiers="
      For j = i To Len(apdu)
        c$ = c$ + Format$(dByte(j)) + ","
      Next j
      Ptrace Left$(c$, Len(c$) - 1)
      Phex i, Len(apdu)
    Case Else                                   'all others are not context tagged
      ErrorHeader c$, i, False
  End Select
End Sub

Function EventTypeName(et) As String
Dim t$
  Select Case et
    Case 0
      t$ = "change-of-bitstring"
    Case 1
      t$ = "change-of-state"
    Case 2
      t$ = "change-of-value"
    Case 3
      t$ = "command-failure"
    Case 4
      t$ = "floating-limit"
    Case 5
      t$ = "out-of-range"
    Case 6 To 63
      t$ = "BACnet reserved event type"
    Case Else
      t$ = "proprietary event type"
  End Select
  EventTypeName = Format$(et) + " (" + t$ + ")"
End Function

Function FilterUnprintables(ByVal p$) As String
Dim i, c$, r$
  r$ = ""
  For i = 1 To Len(p$)
    c$ = Mid$(p$, i, 1)
    If Asc(c$) < 32 Then
      r$ = r$ + "{" + hex2(c$) + "}"
    Else
      r$ = r$ + c$
    End If
  Next i
  FilterUnprintables = r$
End Function

'get a dword tag's value
'in:    tagoff  offset (1-based) in apdu to tlv octet
'out:   value of tag
'       tagoff  updated past the tagged value
Function GetTagValue&(tagoff)
Dim i
  i = dByte(tagoff) And 7          'get length part
  GetTagValue = vbDWORD(Mid$(apdu, tagoff + 1, i), i)
  tagoff = tagoff + i + 1
End Function

'convert the byte value in the first char of v$ to two hex chars
Function hex2(v$) As String
  hex2 = Right$("0" + Hex$(Asc(v$)), 2)
End Function

'convert an integer to a hex value 4 chars long
Function hex4(v As Integer) As String
  hex4 = Right$("000" + Hex$(v), 4)
End Function

'convert the N bytes of v$ into N*2 hex chars
Function hexn(n, v$) As String
Dim i, c$
  c$ = ""
  For i = 1 To n
    c$ = c$ + hex2(Mid$(v$, i, 1))
  Next i
  hexn = c$
End Function

Sub InitInterpreter()
  PropName(0) = "acked-transitions"
  PropName(1) = "ack-required"
  PropName(2) = "action"
  PropName(3) = "action-text"
  PropName(4) = "active-text"
  PropName(5) = "active-vt-sessions"
  PropName(6) = "alarm-value"
  PropName(7) = "alarm-values"
  PropName(8) = "all"
  PropName(9) = "all-writes-successful"
  PropName(10) = "apdu-segment-timout"
  PropName(11) = "apdu-timout"
  PropName(12) = "application-software-version"
  PropName(13) = "archive"
  PropName(14) = "bias"
  PropName(15) = "change-of-state-count"
  PropName(16) = "change-of-state-time"
  PropName(17) = "notification-class"
  PropName(18) = "reserved propertyID 18"
  PropName(19) = "controlled-variable-reference"
  PropName(20) = "controlled-variable-units"
  PropName(21) = "controlled-variable-value"
  PropName(22) = "cov-increment"
  PropName(23) = "datelist"
  PropName(24) = "daylight-savings-status"
  PropName(25) = "deadband"
  PropName(26) = "derivative-constant"
  PropName(27) = "derivative-constant-units"
  PropName(28) = "description"
  PropName(29) = "description-of-halt"
  PropName(30) = "device-address-binding"
  PropName(31) = "device-type"
  PropName(32) = "effective-period"
  PropName(33) = "elapsed-active-time"
  PropName(34) = "error-limit"
  PropName(35) = "event-enabled"
  PropName(36) = "event-state"
  PropName(37) = "event-type"
  PropName(38) = "exception-schedule"
  PropName(39) = "fault-values"
  PropName(40) = "feedback-value"
  PropName(41) = "file-access-method"
  PropName(42) = "file-size"
  PropName(43) = "file-type"
  PropName(44) = "firmware-revision"
  PropName(45) = "high-limit"
  PropName(46) = "inactive-text"
  PropName(47) = "in-process"
  PropName(48) = "instance-of"
  PropName(49) = "integral-constant"
  PropName(50) = "integral-constant-units"
  PropName(51) = "issue-confirmed-notifications"
  PropName(52) = "limit-enable"
  PropName(53) = "list-of-group-members"
  PropName(54) = "list-of-object-property-references"
  PropName(55) = "list-of-session-keys"
  PropName(56) = "local-date"
  PropName(57) = "local-time"
  PropName(58) = "location"
  PropName(59) = "low-limit"
  PropName(60) = "manipulated-variable-reference"
  PropName(61) = "maximum-output"
  PropName(62) = "max-apdu-length-accepted"
  PropName(63) = "max-info-frames"
  PropName(64) = "max-master"
  PropName(65) = "max-pres-value"
  PropName(66) = "minimum-off-time"
  PropName(67) = "minimum-on-time"
  PropName(68) = "minimum-output"
  PropName(69) = "min-pres-value"
  PropName(70) = "model-name"
  PropName(71) = "modification-date"
  PropName(72) = "notify-type"
  PropName(73) = "number-of-apdu-retries"
  PropName(74) = "number-of-states"
  PropName(75) = "object-identifier"
  PropName(76) = "object-list"
  PropName(77) = "object-name"
  PropName(78) = "object-property-reference"
  PropName(79) = "object-type"
  PropName(80) = "optional"
  PropName(81) = "out-of-service"
  PropName(82) = "output-units"
  PropName(83) = "event-parameters"
  PropName(84) = "polarity"
  PropName(85) = "present-value"
  PropName(86) = "priority"
  PropName(87) = "priority-array"
  PropName(88) = "priority-for-writing"
  PropName(89) = "process-identifier"
  PropName(90) = "program-change"
  PropName(91) = "program-location"
  PropName(92) = "program-state"
  PropName(93) = "proportional-constant"
  PropName(94) = "proportional-constant-units"
  PropName(95) = "protocol-conformance-class"
  PropName(96) = "protocol-object-types-supported"
  PropName(97) = "protocol-services-supported"
  PropName(98) = "protocol-version"
  PropName(99) = "read-only"
  PropName(100) = "reason-for-halt"
  PropName(101) = "recipient"
  PropName(102) = "recipient-list"
  PropName(103) = "reliability"
  PropName(104) = "relinquish-default"
  PropName(105) = "required"
  PropName(106) = "resolution"
  PropName(107) = "segmentation-supported"
  PropName(108) = "setpoint"
  PropName(109) = "setpoint-reference"
  PropName(110) = "state-text"
  PropName(111) = "status-flags"
  PropName(112) = "system-status"
  PropName(113) = "time-delay"
  PropName(114) = "time-of-active-time-reset"
  PropName(115) = "time-of-state-count-reset"
  PropName(116) = "time-synchronization-recipients"
  PropName(117) = "units"
  PropName(118) = "update-interval"
  PropName(119) = "utc-offset"
  PropName(120) = "vendor-identifier"
  PropName(121) = "vendor-name"
  PropName(122) = "vt-classes-supported"
  PropName(123) = "weekly-schedule"
End Sub

'interpret the string apdu and display results in tList
Sub InterpretAPDU(tlist As Control, hList As Control)
Dim pdutype, c$, i, svc
  Set TraceList = tlist
  Set TraceHex = hList
  pdutype = dByte(1)
  IndentLevel = 0
  Select Case (pdutype And &HF0)
    Case &H0                                    'confirmed request
      c$ = ""
      If pdutype And 8 Then c$ = c$ + "(Seg)"
      If pdutype And 4 Then c$ = c$ + "(More)" Else c$ = c$ + "(Last)"
      If pdutype And 2 Then c$ = c$ + "(SA)"
      c$ = c$ + "(mr" + Format$(MaxResp(dByte(2))) + "):"
      c$ = c$ + " invID=" + Format$(dByte(3))
      i = 4
      If pdutype And 8 Then
        c$ = c$ + ", seq=" + Format$(dByte(4)) + ", pws=" + Format$(dByte(5))
        i = 6
      End If
      svc = dByte(i)
      Ptrace ConfirmedService(svc) + c$
      Phex 1, i
      ConfirmedBody svc, i + 1
    Case &H10                                   'unconfirmed request
      svc = dByte(2)
      Ptrace UnconfirmedService(svc)
      Phex 1, 2
      UnconfirmedBody svc, 3
    Case &H20                                   'simple ACK
      Ptrace "SimpleACK: invID=" + Format$(dByte(2)) + ", " + ConfirmedService(dByte(3))
      Phex 1, 3
    Case &H30                                   'complex ACK
      c$ = "Complex ACK"
      If pdutype And 8 Then c$ = c$ + "(Seg)"
      If pdutype And 4 Then c$ = c$ + "(More)" Else c$ = c$ + "(Last)"
      c$ = c$ + ": invID=" + Format$(dByte(2))
      i = 3
      If pdutype And 8 Then
        c$ = c$ + ", seq=" + Format$(dByte(3)) + ", pws=" + Format$(dByte(4))
        i = 5
      End If
      svc = dByte(i)
      Ptrace c$ + "," + ConfirmedService(svc)
      Phex 1, i
      ComplexACKBody svc, i + 1
    Case &H40                                   'segment ACK
      If pdutype And 2 Then c$ = "Segment NAK " Else c$ = "Segment ACK "
      If pdutype And 1 Then c$ = c$ + "from Server: invID=" Else c$ = c$ + "from Client: invID="
      c$ = c$ + Format$(dByte(2)) + ", seq=" + Format$(dByte(3)) + ", aws=" + Format$(dByte(4))
      Ptrace c$
      Phex 1, 4
    Case &H50                                   'error
      ErrorReason
    Case &H60                                   'reject
      RejectReason
    Case &H70                                   'abort
      AbortReason pdutype
    Case Else
      Ptrace "Invalid PDU type [" + Hex$(pdutype) + "] ignored..."
      Phex 1, 8
  End Select
End Sub

Sub InterpretNPDU(np As NALSpacket, tlist As Control, hList As Control)
Dim c$, bp(0 To 1599) As Byte

  Set TraceList = tlist
  Set TraceHex = hList
  TraceHex.AddItem sep
  Ptrace sep
  TraceHex.AddItem ""
  Ptrace ">From port " + Format$(Asc(np.nalsLport)) + ", snet " + Format$(np.nalsSnet) + ":" + hexn(Asc(np.nalsSlen), np.nalsSadr)
  Select Case np.nalsDnet
    Case &HFFFF                                 'global broadcast
      c$ = "Global Broadcast"
    Case 0                                      'local
      If Asc(np.nalsDlen) = 0 Then
        c$ = "Broadcast"
      Else
        c$ = "Local:" + hexn(Asc(np.nalsDlen), np.nalsDadr)
      End If
    Case Else                                   'routed
      If Asc(np.nalsRlen) = 0 Then
        c$ = "Broadcast to Router:"
      Else
        c$ = "Route via " + hexn(Asc(np.nalsRlen), np.nalsRadr) + ":"
      End If
      c$ = c$ + ", to Net:" + Format$(np.nalsDnet)
      If Asc(np.nalsDlen) = 0 Then
        c$ = c$ + "[Remote Broadcast]"
      Else
        c$ = c$ + "[" + hexn(Asc(np.nalsDlen), np.nalsDadr) + "]"
      End If
  End Select
  TraceHex.AddItem ""
  Ptrace ">" + c$
End Sub

Function IsntPDtag(tag, aoff, opcl)
Dim ht, p$
  ht = ((tag * 16) + &HE + opcl)
  If opcl Then p$ = "Close" Else p$ = "Open"
  If dByte(aoff) <> ht Then
    Ptrace "? Expected Paired Delimiter " + p$ + " Tag" + Str$(tag) + " (X'" + Hex$(ht) + "') here"
    Perror aoff
    IsntPDtag = True
  Else
    SaveHex aoff
    aoff = aoff + 1
    IsntPDtag = False
  End If
End Function

Function IsTag(t, i)
  IsTag = ((dByte(i) And &HF8) = ((t * 16) + &H8))
End Function

Function KnowPrimitiveInContext(tag, ptype, l As Long)
  KnowPrimitiveInContext = True         'assume we know
  Select Case ptype
    Case ptObjID                        'an object identifier
        tag = 12
    Case ptET, ptEbool                  'an enumeration table, boolean
        tag = 9
    Case ptFloat                        'float
      If (l = 4) Then
        tag = 4                         'real
      ElseIf (l = 8) Then
        tag = 5                         'double
      Else
        KnowPrimitiveInContext = False  'unknown
      End If
    Case ptUW, ptUD, ptU127, ptU16      'unsigned word, dword, 1..127, 1..16
      tag = 2
    Case ptSW, ptSsint                           'signed word  MAG add ptSsint 13 FEB 2001
      tag = 3
    Case ptS10, ptS32, ptS64, ptS132, ptStatext   'char [10]..[132],state text array
      tag = 7
    Case ptPss, ptPos                   'protocol services supported bitstring, protocol objects supported bitstring
      tag = 8
    Case ptD                            'date
      tag = 10
    Case ptT                            'time
      tag = 11
    Case Else                           'unknown
      KnowPrimitiveInContext = False    'we don't know
  End Select
End Function

Function LevelPrefix() As String
  If IndentLevel <= 0 Then
    LevelPrefix = "..."
  Else
    LevelPrefix = Space$(IndentLevel * 6)
  End If
End Function

Function MaxResp(mr)
  Select Case mr
    Case 0
      MaxResp = 50
    Case 1
      MaxResp = 128
    Case 2
      MaxResp = 206
    Case 3
      MaxResp = 480
    Case 4
      MaxResp = 1024
    Case 5
      MaxResp = 1470
    Case Else
      MaxResp = 50
  End Select
End Function

Function MonthName(m) As String
  Select Case m
    Case 1
      MonthName = "January"
    Case 2
      MonthName = "February"
    Case 3
      MonthName = "March"
    Case 4
      MonthName = "April"
    Case 5
      MonthName = "May"
    Case 6
      MonthName = "June"
    Case 7
      MonthName = "July"
    Case 8
      MonthName = "August"
    Case 9
      MonthName = "September"
    Case 10
      MonthName = "October"
    Case 11
      MonthName = "November"
    Case 12
      MonthName = "December"
    Case 255
      MonthName = "any month"
    Case Else
      MonthName = "invalid month (" + Format$(m) + ")"
  End Select
End Function

Function ObjectIdentifier(aoff) As String
Dim inst As Long
  LastObjType = (dByte(aoff) * 4) + (dByte(aoff + 1) \ 64)
  inst = vbDWORD(Mid$(apdu, aoff + 1, 3), 3) And &H3FFFFF
  ObjectIdentifier = "(" + ObjectType(LastObjType) + "," + Format$(inst) + ")"
End Function

Function ObjectType(ByVal t) As String
  Select Case t
    Case 0
      ObjectType = "analog-input"
    Case 1
      ObjectType = "analog-output"
    Case 2
      ObjectType = "analog-value"
    Case 3
      ObjectType = "binary-input"
    Case 4
      ObjectType = "binary-output"
    Case 5
      ObjectType = "binary-value"
    Case 6
      ObjectType = "calendar"
    Case 7
      ObjectType = "command"
    Case 8
      ObjectType = "device"
    Case 9
      ObjectType = "event-enrollment"
    Case 10
      ObjectType = "file"
    Case 11
      ObjectType = "group"
    Case 12
      ObjectType = "loop"
    Case 13
      ObjectType = "multistate-input"
    Case 14
      ObjectType = "multistate-output"
    Case 15
      ObjectType = "notification-class"
    Case 16
      ObjectType = "program"
    Case 17
      ObjectType = "schedule"
    Case 18 To 127
      ObjectType = "BACnet reserved object type:" + Str$(t)
    Case Else
      ObjectType = "proprietary:" + Str$(t)
  End Select
End Function

'return a string X'hhhhhh...'
'in:    i       offset in apdu to octets
'       n       number of octets
Function OctetString(ByVal i, ByVal n&) As String           '                 ***218
Dim v$, j
  v$ = "X'"
  For j = i To (i + n) - 1
    v$ = v$ + Right$("0" + Hex$(dByte(j)), 2)
  Next j
  OctetString = v$ + "'"
End Function

Sub Perror(aoff)
  Phex aoff, aoff + 7
End Sub

Sub Pflush()
Dim i
  If SavedHex <> "" Then
    i = TraceHex.ListCount - 1
    TraceHex.List(i) = TraceHex.List(i) + SavedHex
    SavedHex = ""
  End If
End Sub

Sub Phex(stoff, endoff)
Dim i, c$
  c$ = SavedHex
  SavedHex = ""
  If stoff Then
    For i = stoff To endoff
      c$ = c$ + Right$("0" + Hex$(dByte(i)), 2) + " "
    Next i
  End If
  Do While TraceHex.ListCount > MaxTraceLines
    TraceHex.RemoveItem 0
  Loop
  TraceHex.AddItem c$
  If TraceList.Selected(TraceList.ListCount - 1) Then TraceHex.Selected(TraceHex.ListCount - 1) = True
End Sub

'out:   true if failed
Function PropertyValue(i)
Dim j, ul, si, c$, PropId As Long, p$, ari&   '                       ***218
  PropertyValue = True
  ul = dByte(i)
  si = i
  If (ul < &H9) Or (ul > &HC) Then
    Ptrace "? Expected Context Tag 0 (X'09'..X'0C') here"
    Perror si
    Exit Function
  End If
  ul = ul And 7                         'save length
  PropId = vbDWORD(Mid$(apdu, i + 1, ul), ul)
  i = i + ul + 1
  If PropId < 124 Then
    p$ = PropName(PropId)
  ElseIf PropId > 511 Then
    p$ = "(proprietary" + Str$(PropId) + ")"
  Else
    p$ = "(BACnet reserved property" + Str$(PropId) + ")"
  End If
  If IsTag(1, i) Then                   'we have an array index
    ul = dByte(i) And 7
    If (ul < 1) Or (ul > 4) Then
      Ptrace "? propertyArrayIndex length must be 1..4 octets"
      Perror si
      Exit Function
    End If
    ari& = vbDWORD(Mid$(apdu, i + 1, ul), ul) '                       ***218
    p$ = p$ + Format$(ari&, "\[0\]")          '                       ***218
    i = i + ul + 1
  End If
  For j = si To i - 1
    SaveHex j
  Next j
'  If IsntPDtag(2, i, 0) Then Exit Sub
'  dAny i, p$, True, 0, 0                   'ABSTRACT-SYNTAX.&Type
  dPropVal i, p$, PropId, ari&              '                         ***218
'  If IsntPDtag(2, i, 1) Then Exit Sub
  If dByte(i) = &H39 Then               'we have priority
    If dContext(3, i, "priority", 1, 16) Then Exit Function
  End If
  PropertyValue = False
End Function

Sub Ptrace(ByVal s$)
  Do While TraceList.ListCount > MaxTraceLines
    TraceList.RemoveItem 0
  Loop
  TraceList.AddItem s$
  If Left$(s$, 1) = "?" Then TraceList.Selected(TraceList.ListCount - 1) = True
End Sub

'read a text file and convert to hex octets
Sub ReadAPDUs(ByVal f$)
Dim fs, h, i, c$, v$, l$
  On Error Resume Next
  h = FreeFile
    Open f$ For Input As #h
    If Err Then
      Beep
      MsgBox "Can't read file " + f$ + " because:" + Chr$(13) + Chr$(10) + Error$(Err), 48, "Error Reading PDU File"
      Exit Sub
    End If
    TraceForm.RxHex.AddItem ""
    TraceForm.RxList.AddItem ">PDU(s) from file:" + f$
    fs = False
    apdu = ""
    CurrentLine = 0
    Do Until EOF(h)
      Line Input #h, l$                 'get a line from the file
      CurrentLine = CurrentLine + 1
      i = InStr(l$, "--")               'see if it has comments in it
      If i Then l$ = Left$(l$, i - 1)
      If (l$ <> "") And fs Then
        If l$ = ">" Then
          If apdu <> "" Then
            InterpretAPDU TraceForm.RxList, TraceForm.RxHex
            TraceForm.RxHex.AddItem ""
            TraceForm.RxList.AddItem sep
          End If
          apdu = ""
        End If
        v$ = ""
        i = 1
        Do While i <= Len(l$)
          c$ = LCase$(Mid$(l$, i, 1))
          If ((c$ >= "0") And (c$ <= "9")) Or ((c$ >= "a") And (c$ <= "f")) Then
            v$ = v$ + c$
            If Len(v$) = 2 Then
              apdu = apdu + Chr$(Val("&H" + v$))
              v$ = ""
            End If
          End If
          i = i + 1
        Loop
      ElseIf l$ = ">" Then
        fs = True
      End If
    Loop
    Close #h
    If apdu <> "" Then InterpretAPDU TraceForm.RxList, TraceForm.RxHex
End Sub

Sub RejectReason()
Dim c$, r As Long
  c$ = "Reject: invID=" + Format$(dByte(2)) + ", "
  If Len(apdu) = 3 Then                        'not using proprietary reject
    r = dByte(3)
  Else
    r = dWord(3)
  End If
  Select Case r
    Case 0
      c$ = c$ + "other"
    Case 1
      c$ = c$ + "buffer-overflow"
    Case 2
      c$ = c$ + "inconsistant-parameters"
    Case 3
      c$ = c$ + "invalid-parameter-data-type"
    Case 4
      c$ = c$ + "invalid-tag"
    Case 5
      c$ = c$ + "missing-required-parameter"
    Case 6
      c$ = c$ + "parameter-out-of-range"
    Case 7
      c$ = c$ + "too-many-arguments"
    Case 8
      c$ = c$ + "undefined-enumeration"
    Case 9
      c$ = c$ + "unrecognized-service"
    Case 10 - 63
      c$ = c$ + "BACnet reserved reason" + Str$(r)
    Case Else
      c$ = c$ + "proprietary reason" + Str$(r)
  End Select
  Ptrace c$
  Phex 1, Len(apdu)
End Sub

Sub SaveHex(aoff)
  SavedHex = SavedHex + Right$("0" + Hex$(dByte(aoff)), 2) + " "
End Sub

Sub SavePDUs(cap$, thex As ListBox, tlist As ListBox)
Dim f$, fn, i, c$

  If thex.ListCount = 0 Then
    Beep
    MsgBox "There is no PDU information to save for " + cap$, 48, "Nothing to Save"
    Exit Sub
  End If
  On Error GoTo fsperr
  VTSform.CMD.DialogTitle = "Save " + cap$
  VTSform.CMD.Filter = "Text Data|*.HEX"
  VTSform.CMD.Flags = OFN_HIDEREADONLY Or OFN_OVERWRITEPROMPT Or OFN_PATHMUSTEXIST
  VTSform.CMD.Action = 2                        'file save as
  f$ = VTSform.CMD.FileName
  If f$ = "" Then Exit Sub
  On Error Resume Next
  fn = FreeFile
  Open f$ For Output As #fn
  VTSform.MousePointer = 11
  If Err Then
    Beep
    MsgBox "Can't create file because:" + Chr$(13) + Chr$(10) + Error$(Err), 48, "Save Failed"
    GoTo fspdone
  End If
  For i = 0 To thex.ListCount - 1
    c$ = Left$(tlist.List(i), 1)
    If (c$ = "-") Then
      Print #fn, tlist.List(i)
    ElseIf c$ = ">" Then
      Print #fn, "--"; tlist.List(i)
    ElseIf (c$ = ".") Or (c$ = " ") Then
      Print #fn, TabToStop(4, thex.List(i), "--" + tlist.List(i))
    Else
      Print #fn, ">"
      Print #fn, TabToStop(4, thex.List(i), "--" + tlist.List(i))
    End If
  Next i
  Close #fn
fspdone:
  VTSform.MousePointer = 0

fsperr:
  Exit Sub
End Sub

'tab to a tab stop
'in:    c       tabstop to tab to (1,2,3,4 etc.)
'       s1$     1st string
'       s2$     2nd string
Function TabToStop(c, ByVal s1$, ByVal s2$) As String
Dim t, c8, p$
  c8 = (c * 8) - 2
  t = Len(s1$) \ 8
  If t >= c Then
    p$ = Left$(s1$, c8) + "  " + s2$
    Do
      s1$ = LTrim$(Mid$(s1$, c8 + 1))
      If s1$ = "" Then Exit Do
      t = Len(s1$) \ 8
      If t >= c Then
        p$ = p$ + Chr$(13) + Chr$(10) + Left$(s1$, c8)
      Else
        p$ = p$ + Chr$(13) + Chr$(10) + s1$
      End If
    Loop
    TabToStop = p$
  Else
    t = c - t
    TabToStop = s1$ + String$(t, 9) + s2$
  End If
End Function

Function TimeString(aoff) As String
Dim h, m, s, d, hr$, mn$, sc$, hu$
  h = dByte(aoff)
  m = dByte(aoff + 1)
  s = dByte(aoff + 2)
  d = dByte(aoff + 3)
  If h = 255 Then hr$ = "? :" Else hr$ = Format$(h, "00:")
  If m = 255 Then mn$ = "? :" Else mn$ = Format$(m, "00:")
  If s = 255 Then sc$ = "? ." Else sc$ = Format$(s, "00.")
  If d = 255 Then hu$ = "? " Else hu$ = Format$(d, "00")
  If (h > 23) And (h <> 255) Then hr$ = "(? Invalid Hours):"
  If (m > 59) And (m <> 255) Then mn$ = "(? Invalid Minutes):"
  If (s > 59) And (s <> 255) Then sc$ = "(? Invalid Seconds)."
  If (d > 99) And (d <> 255) Then hu$ = "(? Invalid Hundredths)"
  TimeString = hr$ + mn$ + sc$ + hu$
End Function

Sub UnconfirmedBody(svc, i)
Dim j, ae, et, evobjt, dw As Long               '                       ***218 Begin
  Select Case svc
    Case 0                                      'i-Am
      If dObjectIdentifier(-1, i, "iAmDeviceIdentifier") Then Exit Sub
      If dUnsigned(i, "maxAPDULengthAccepted", dw) Then Exit Sub
      If dSegmentation(i, "segmentationSupported") Then Exit Sub
      If dUnsigned(i, "vendorID", dw) Then Exit Sub
      For j = 0 To TraceForm.VendorNames.ListCount - 1
        If dw = TraceForm.VendorNames.ItemData(j) Then
          AppendTrace " (" + TraceForm.VendorNames.List(j) + ")"
          Exit Sub
        End If
      Next j
      AppendTrace " (unknown vendorID)"         '                       ***218 End
    Case 1                                      'i-Have
      If dObjectIdentifier(-1, i, "deviceIdentifier") Then Exit Sub
      If dObjectIdentifier(-1, i, "objectIdentifier") Then Exit Sub
      If dCharacterString(-1, i, "objectName") Then Exit Sub
    Case 2                                      'unconfirmedCOVnotification
      If dContext(0, i, "subscriberProcessIdentifier", 0, 0) Then Exit Sub
      If dObjectIdentifier(1, i, "initiatingDeviceIdentifier") Then Exit Sub
      If dObjectIdentifier(2, i, "monitoredObjectIdentifier") Then Exit Sub
      If dContext(3, i, "timeRemaining", 0, 0) Then Exit Sub
      If IsntPDtag(4, i, 0) Then Exit Sub       'must provide at least one value
      BeginLevel "listOfValues"
      Do While i <= Len(apdu)
        If dByte(i) = &H4F Then
          If IsntPDtag(4, i, 1) Then Exit Sub
          EndLevel
          Exit Sub                              'found closing delimiter
        End If
        If PropertyValue(i) Then Exit Sub
      Loop
      Ptrace "? Expected Paired Delimiter Close Tag 4 (X'4F') here!"
      Perror i
      IndentLevel = IndentLevel - 1
    Case 3                                      'unconfirmedEventNotification
      If dContext(0, i, "processIdentifier", 0, 0) Then Exit Sub
      If dObjectIdentifier(1, i, "initiatingDeviceIdentifier") Then Exit Sub
      If dObjectIdentifier(2, i, "eventObjectIdentifier") Then Exit Sub
      evobjt = LastObjType                      'remember object type for event object
      If dTimeStamp(3, i, "timeStamp") Then Exit Sub
      If dContext(4, i, "notificationClass", 0, 0) Then Exit Sub
      If dContext(5, i, "priority", 0, 255) Then Exit Sub
      If dEventType(6, i, "eventType", et) Then Exit Sub
      If IsTag(7, i) Then                       'messageText
        If dCharacterString(7, i, "messageText") Then Exit Sub
      End If
      If dNotifyType(8, i, "notifyType", ae) Then Exit Sub
      If ae Then                                'must have these parameters for alarm or event
        enumx(0) = "FALSE"
        enumx(1) = "TRUE"
        If dEnum(9, i, "ackRequired", 2) Then Exit Sub
        If dEventState(10, i, "fromState") Then Exit Sub
        If dEventState(11, i, "toState") Then Exit Sub
        If dNotificationParameters(12, i, "eventValues", et, evobjt) Then Exit Sub
      End If
    Case 4                                      'unconfirmedPrivateTransfer
      If dContext(0, i, "vendorID", 0, 0) Then Exit Sub
      If dContext(1, i, "serviceNumber", 0, 0) Then Exit Sub
      If dByte(i) = &H2E Then
        Phex i, i
        BeginLevel "serviceParameters"
        Phex i + 1, Len(apdu)
        EndLevel
      End If
    Case 5                                      'unconfirmedTextMessage
      If dObjectIdentifier(0, i, "textMessageSourceDevice") Then Exit Sub
      If dByte(i) = &H1E Then                   'message class was provided
        If IsntPDtag(1, i, 0) Then Exit Sub
        If IsTag(0, i) Then                     'numeric class
          If dContext(0, i, "messageClass", 0, 0) Then Exit Sub
        ElseIf IsTag(1, i) Then                 'character class
          If dCharacterString(1, i, "messageClass") Then Exit Sub
        Else
          Ptrace "? Expected numeric or character messageClass here"
          Perror i
          Exit Sub
        End If
        If IsntPDtag(1, i, 1) Then Exit Sub
      End If
      enumx(0) = "normal"
      enumx(1) = "urgent"
      If dEnum(2, i, "messagePriority", 2) Then Exit Sub
      If dCharacterString(3, i, "message") Then Exit Sub
    Case 6                                      'timeSynchronization
      If dDate(i, "date") Then Exit Sub
      If dTime(i, "time", -1) Then Exit Sub
    Case 7                                      'who-Has
      If IsTag(0, i) Then                       'we have a device instance range
        If dContext(0, i, "deviceInstanceRangeLowLimit", 0, 4194303) Then Exit Sub
        If dContext(1, i, "deviceInstanceRangeHighLimit", 0, 4194303) Then Exit Sub
      End If
      If IsTag(2, i) Then                       'we have an object identifier
        If dObjectIdentifier(2, i, "objectIdentifier") Then Exit Sub
      ElseIf IsTag(3, i) Then                   'we have an object name
        If dCharacterString(3, i, "objectName") Then Exit Sub
      Else                                      'it's a problem
        Ptrace "? Expected objectIdentifier or objectName here"
        Perror i
      End If
    Case 8                                      'who-Is
      If IsTag(0, i) Then                       'we have a device instance range
        If dContext(0, i, "deviceInstanceRangeLowLimit", 0, 4194303) Then Exit Sub
        If dContext(1, i, "deviceInstanceRangeHighLimit", 0, 4194303) Then Exit Sub
      End If
  End Select
End Sub

Function UnconfirmedService(svc) As String
  Select Case svc
    Case 0
      UnconfirmedService = "i-Am"
    Case 1
      UnconfirmedService = "i-Have"
    Case 2
      UnconfirmedService = "unconfirmedCOVnotification"
    Case 3
      UnconfirmedService = "unconfirmedEventNotification"
    Case 4
      UnconfirmedService = "unconfirmedPrivateTransfer"
    Case 5
      UnconfirmedService = "unconfirmedTextMessage"
    Case 6
      UnconfirmedService = "timeSynchronization"
    Case 7
      UnconfirmedService = "who-Has"
    Case 8
      UnconfirmedService = "who-Is"
    Case Else
      UnconfirmedService = "Unknown Unconfirmed Service" + Str$(svc)
  End Select
End Function

