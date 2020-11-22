Attribute VB_Name = "SBPARSE1"
Option Explicit
                     
' Type declaration
Type NataType
  Name As String
  Value As String
  Comment As String
End Type

Type TestNode
  Section As Integer
  nr As String
  Comment As String
  DepList As String
  Reference As String
  FirstPacket As Integer
  passed As Integer
  ' 0   not tested
  ' 1   passed
  ' 2   failed
  ' 3   not tested, depencies failed
  ' 4   not tested, depencies not tested
  ' 5   not tested, depencies not found
  Result As String
End Type

Type PacketNode
  Command As String * 1
  ' 0   Send
  ' 3   Or-Send
  ' 4   And-Send
  ' 129 Expect
  ' 131 Or-Expect
  ' 132 And-Expect
  CaseLvl As String * 1
  ' 0   No Case
  ' 1-15    CaseLevel
  ' &128    CaseLevelMarker
  Port As Integer
  DLC As Integer
  NL As Integer
  AL As Integer
End Type

' The control types
'   0   Not present, Any
'   1   Equal
'   2   Lower
'   3   Lower equal
'   4   Higher
'   5   Higher equal
'   6   Unequal

Type DLC_Control
  DA As String * 1
  SA As String * 1
  LSAP As String * 1
End Type

Type NL_Control
  Version As String * 1
  DNET As String * 1
  DADR As String * 1
  SNET As String * 1
  SADR As String * 1
  HOPCNT As String * 1
  Message As String * 1
  priority As String * 1
  DER As String * 1
  VendorID As String * 1
  DATA As String ' Specical Control Field
  ' first byte  control type
  ' second byte length low
  ' third byte  length high
End Type

Type AL_Control
  pdu As String * 1
  Flags As String * 1
  MaxResp As String * 1
  invokeID As String * 1
  SequenceNr As String * 1
  WindowSize As String * 1
  Service As String * 1
  DATA As String ' Special Control Field
End Type

Type DLCframeType
  Ctrl As DLC_Control
  DA As String * 8      ' first character is length
  SA As String * 8      ' first character is length
  LSAP As String * 1
End Type

Type NLframeType
  Ctrl As NL_Control
  Version As String * 1
  DNET As Integer
  DADR As String * 8    ' first character is length
  SNET As Integer
  SADR As String * 8    ' first character is length
  HOPCNT As String * 1
  Message As String * 1
  priority As String * 1
  DER As String * 1
  VendorID As Integer
  DATA As String
End Type

Type ALframeType
  Ctrl As AL_Control
  pdu As String * 1
  Flags As String * 1
  MaxResp As String * 1
  invokeID As String * 1
  SequenceNr As String * 1
  WindowSize As String * 1
  Service As String * 1
  DATA As String
End Type

Type RXframeType            ' received data
  RxType As String * 1
  Checked As String * 1     ' 0 if not checked
  Port As Integer
  ExpectReply As Integer
  Broadcast As Integer
  invokeID As String * 1
  Pri As Integer
  DNET As Integer
  DADR As String * 8    ' first character is length
  SNET As Integer
  SADR As String * 8    ' first character is length
  RADR As String * 8    ' first character is length
  DATA As String
End Type

' Globals declaration for the testing
Global DefScriptFile As String
' Progress Variable
Global TestInProgress As Integer

' Globals for the Parser
Global SBNata() As NataType
Global SBTest() As TestNode
Global SBpacket() As PacketNode
Global SBframeDLC() As DLCframeType
Global SBframeNL() As NLframeType
Global SBframeAL() As ALframeType
Global SBrecList() As RXframeType     'list for received data

Global gCancelParse As Integer

' local modules vars for exchange
Dim LineNo As Integer
Dim ErrCnt As Integer


Private Sub AddNataComment(ni%, s$)
  If ni >= 0 Then
    SBNata(ni).Comment = SBNata(ni).Comment & s$ & Chr$(32)
  End If
End Sub

Private Sub AddNewSection(e$)
  SBTForm.SectionList.AddItem Chr$(215) & Chr$(9) & e$
End Sub

Private Function AddNewTest(e$) As Integer
Dim ti As Integer

  On Error Resume Next
  AddNewTest = -1

  ti = UBound(SBTest)
  ReDim Preserve SBTest(ti + 1)

  If Err = 0 Then
    On Error GoTo 0

    SBTest(ti).Section = CurrentSection()
    SBTest(ti).nr = e$
    SBTest(ti).FirstPacket = -1
    AddNewTest = ti
  End If
End Function

Private Sub AddTestComment(ti%, s$)
  If ti >= 0 Then
    SBTest(ti).Comment = SBTest(ti).Comment & s$ & Chr$(32)
  End If
End Sub

Private Sub AddTestDeps(ti%, s$)
  If ti >= 0 Then
    SBTest(ti).DepList = SBTest(ti).DepList & s$ & Chr$(32)
  End If
End Sub

Private Sub AddTestRefs(ti%, s$)
  If ti >= 0 Then
    SBTest(ti).Reference = SBTest(ti).Reference & s$ & Chr$(32)
  End If
End Sub

Sub AdjustLogFile(sf$, lf$)
Dim s1%, s2%, l%

  s2 = 0
  Do
    s1 = s2 + 1
    s2 = InStr(s1, sf, "\")
  Loop Until (s2 = 0)

  s2 = InStr(s1, sf, ".")
  If s2 = 0 Then s2 = Len(sf) + 1

  l = s2 - s1
  If l > 4 Then
    lf = Left$(sf, s1 + 3)
  Else
    lf = Left$(sf, s1 + l - 1)
  End If

  lf = lf + Format$(Now, "mmdd") + ".LOG"
End Sub

Private Function AdjustValue(e$) As String
Dim i As Integer, fl%
Dim s$, c As String * 1
Dim n As Integer
Dim e2 As String

 'remove all white spaces (gdud)
   
  e2$ = ""
  n = Len(e$)
  For i = 1 To n
    c = Mid$(e$, i, 1)
    If (c <> " ") And (c <> Chr$(9)) Then
      e2$ = e2$ + c
    End If
  Next i
  AdjustValue = e2$


  ' Reset variables
'  s$ = ""
'  fl% = True

  ' Search for spaces, tabs and comma
'  For i = 1 To Len(e2$)
'    c = Mid$(e2$, i, 1)
'    If (c <> " ") And (c <> Chr$(9)) And (c <> ",") Then
'      s$ = s$ + c
'      fl% = True
'    ElseIf fl% Then
'      s$ = s$ + ","
'      fl% = False
'    End If
'  Next

'  AdjustValue = s$

End Function

Function Bin(t$) As Long
Dim l As Long
Dim i%
  
  l = 0
  For i = 1 To Len(t$)
    If (Mid$(t$, i, 1) < "0") Or (Mid$(t$, i, 1) > "1") Then Exit For
    l = l + (Asc(Mid$(t$, i, 1)) - 48)
    l = l * 2
  Next
End Function

Private Function CheckDest(e$) As Integer
Dim cd%
  
  cd% = 0
  Select Case UCase$(e$)
    Case "UNKNOWN", "BROADCAST", "LOCAL-BROADCAST"
      e$ = "BROADCAST"
    Case "GLOBAL-BROADCAST"
      e$ = "BROADCAST"
      cd = 1
  End Select

  CheckDest = cd
End Function

Private Function CheckSign(e$, s$, b%) As Integer
Dim f%, i%

  f = 0
  CheckSign = 0

  For i = b To Len(e$)
    If (f = 0) And (Mid$(e$, i, Len(s$)) = s$) Then
      CheckSign = i
      Exit For
    End If
    If (Mid$(e$, i, 1) = "'") And (f >= 0) Then f = 1 - f
    If (Mid$(e$, i, 1) = Chr$(34)) And (f <= 0) Then f = -1 - f
  Next
End Function

Private Function CheckValue(e$, ByVal i%) As String
Dim t$, k%

  ' is it a PICS identifier?
   If (Left$(e$, 1) = "{") And (Right$(e$, 1) = "}") Then
    GetPicsIdent e$
    CheckValue = e$
    Exit Function
  End If

  ' is it indexed here?
  If CheckSign(e$, ",", 1) > 0 Then
    t$ = GetIndexValue(e$, i%)
  Else
    t$ = e$
  End If

  ' is it an identifier?
  Do   ' MAG this loop will run infinitely on e$ == T,F,T,F -fixed
    k = SearchNata(t$)
    If (k >= 0) Then t$ = RTrim$(SBNata(k).Value)
  Loop Until (k = -1)

  ' or is the index here ?
  If (i > 0) Or (CheckSign(t$, ",", 1) > 0) Then
    t$ = GetIndexValue(t$, i%)
  End If

  CheckValue = t$
End Function

Sub ClearGlobalVars(killnata%)
  If killnata Then ReDim SBNata(0) ' clear NameTable
  ReDim SBTest(0)           ' clear Test Table
  ReDim SBpacket(0)         ' clear Packet Table
  ReDim SBframeDLC(0)       ' clear DLC frame table
  ReDim SBframeNL(0)        ' clear NL frame table
  ReDim SBframeAL(0)        ' clear AL frame table
  ReDim SBrecList(0)        ' clear packet receive buffer
End Sub

Private Function CurrentNata() As Integer
  CurrentNata = UBound(SBNata) - 1
End Function

Private Function CurrentSection() As Integer
  CurrentSection = SBTForm.SectionList.ListCount - 1
End Function

Private Function CurrentTest() As Integer
  CurrentTest = UBound(SBTest) - 1
End Function

Private Function DecodeNetworkType(v$) As Integer
  If UCase$(v$) = "ETHERNET" Then
    DecodeNetworkType = 1
  ElseIf UCase$(v$) = "ARCNET" Then
    DecodeNetworkType = 2
  ElseIf UCase$(v$) = "MS/TP" Then
    DecodeNetworkType = 3
  ElseIf UCase$(v$) = "LONTALK" Then
    DecodeNetworkType = 4
  Else
    DecodeNetworkType = 0
  End If
End Function

Private Sub Display(s$)
Dim Msg$

  If Len(s$) > 0 Then
    ErrCnt = ErrCnt + 1
    Msg$ = Str$(LineNo) & " : Error : " + s$
  End If

  If Not SBSetup.visible Then
    SBparse.LineNumber.Caption = Str$(LineNo)
    If Len(s$) > 0 Then
      SBparse.ErrorPanel.Caption = Str$(ErrCnt)
      SBparse.StatusList.AddItem Msg$
    End If
  Else
    If Len(s$) > 0 Then SBSetup.InfoList.AddItem Msg$
  End If
End Sub

Private Function EnterNata(k$, e$) As Integer
Dim ni As Integer
  On Error Resume Next

  EnterNata = -1
  ni = UBound(SBNata)

  ReDim Preserve SBNata(ni + 1)
  If Err = 0 Then
    On Error GoTo 0

    SBNata(ni).Name = k$
    SBNata(ni).Value = AdjustValue(e$)
    EnterNata = ni
  End If
End Function

Private Function Get_BitString(s$, e$) As Integer
Dim i%, r$, b%, v%

  i = 1 ' starting position one
  r$ = "" ' collect string

  Do
    ' get one boolean after the other
    r$ = r$ & GetBoolean(e$, i)
    i = i + 1
  Loop Until Len(CheckValue(e$, i)) = 0

  s$ = "" ' result string
  b = 7 ' starting at position 8
  v = 0 ' value zero

  ' check the whole string
  For i = 1 To Len(r$)

    ' if position zero reached goto
    If b < 0 Then b = 7

    ' add the value
    v = v + Asc(Mid$(r$, i, 1)) * 2 ^ b

    ' goto next position
    b = b - 1

    ' a complete octet read?
    If b < 0 Then
      s$ = s$ & Chr$(v)
      v = 0
    End If
  Next

  If (b >= 0) Then s$ = s$ & Chr$(v)
  s$ = Chr$(b + 1) & s$

  Get_BitString = Len(s$)
End Function

Private Function Get_CharSet(e$) As String
Dim cs%, c$

  cs = 0
  c$ = UCase$(CheckValue(e$, 1))

  Select Case c$
    Case "ANSI", "ANSIX3.4", "ANSIX3.4"
      cs = 0
    Case "IBM", "MICROSOFT", "IBM/MICROSOFT", "DBCS"
      cs = 1
    Case "JIS", "JISC", "JISC6626", "C6626"
      cs = 2
    Case "ISO", "ISO10646", "UCS-4"
      cs = 3
    Case "UCS-2"
      cs = 4
    Case "ISO8859-1"
      cs = 5
    Case Else
      ' the trick to get a number for the charset code
      ' is to code it into hexadecimal with a starting
      ' '&' to encode a hexadecimal number in VB
      cs = GetNumber(c$, 1)
  End Select

  Get_CharSet = Chr$(cs)
End Function

Function Get_Date(s$, e$) As Integer
Dim i%, c$, t$, v%, f%, cf%
Static d$(4)

  ' following a miniparser for following cases
  ' [Sunday,] 1/24/91
  ' [Sunday,] January 24, 91
  ' [Sunday,] 24.1.91
  ' [Sunday,] 24. January 1991

  f = False ' european date format flag
  cf = False ' collector flag
  v = 1 ' collector index
  t$ = "" ' collector string

  ' resetting storage buffers
  For i = 1 To 4
    d$(i) = ""
  Next

  i = 1
  Do While (i <= Len(e$))
    c$ = UCase$(Mid$(e$, i, 1))

    Select Case c$
      Case " " ' Space
        If cf Then
          
          Select Case v
            Case 1
              d$(2) = t$
              f = False
              v = 2
            Case 2
              d$(2) = t$
              f = False
            Case 3
              If f Then
                d$(3) = t$
              Else
                Display "Date format mismatch"
              End If
            Case 4
              Display "Date format mismatch"
          End Select

          cf = False
          v = v + 1
        End If

      Case "," ' Comma
        If cf Then

          Select Case v
            Case 1
              d$(1) = t$ ' day of week
            Case 2, 4
              Display "Date format mismatch"
            Case 3
              If Not f Then
                d$(3) = t$ ' day
              Else
                Display "Date format mismatch"
              End If
          End Select

          cf = False
          v = v + 1
        End If

      Case "." ' Point
        If cf Then
          Select Case v
            Case 1
              d$(2) = t$
              f = True
              v = 2
            Case 2
              d$(2) = t$
              f = True
            Case 3
              If f Then
                d$(3) = t$
              Else
                Display "Date format mismatch"
              End If
            Case 4
              Display "Date format mismatch"
          End Select

          cf = False
          v = v + 1
        End If

      Case "/", "\" ' Slash
        If cf Then

          Select Case v
            Case 1
              d$(2) = t$
              f = False
              v = 2
            Case 2
              d$(2) = t$
              f = False
            Case 3
              If Not f Then
                d$(3) = t$
              Else
                Display "Date format mismatch"
              End If
            Case 4
              Display "Date format mismatch"
          End Select

          cf = False
          v = v + 1
        End If

      Case Else ' mormal chars
        If Not cf Then
          cf = True
          t$ = c$
        Else
          t$ = t$ & c$
        End If
    End Select

    i = i + 1
  Loop

  ' last value should be year in any case
  If v = 4 Then
    d$(4) = t$
  Else
    Display "Date format mismatch"
  End If

  ' okay, a lot of work is done, now get the values

  ' get the year
  t$ = UCase$(d$(4))
  If InStr(t$, "X") Or (Len(t$) = 0) Then
    v = 255 ' any year
  Else
    v = Val(t$) ' year
  End If
  If (v > 1900) Then v = v - 1900
  If (v > 255) Then v = 255
  s$ = Chr$(v)

  ' get the month
  If f Then
    t$ = UCase$(d$(3))
  Else
    t$ = UCase$(d$(2))
  End If
  Select Case t$
    Case "JANUARY": v = 1
    Case "FEBRUARY": v = 2
    Case "MARCH": v = 3
    Case "APRIL": v = 4
    Case "MAY": v = 5
    Case "JUNE": v = 6
    Case "JULY": v = 7
    Case "AUGUST": v = 8
    Case "SEPTEMBER": v = 9
    Case "OCTOBER": v = 10
    Case "NOVEMBER": v = 11
    Case "DECEMBER": v = 12
    Case Else
      If InStr(t$, "X") Or (Len(t$) = 0) Then
        v = 255 ' any month
      Else
        v = Val(t$)
      End If
  End Select
  If (v > 255) Then v = 255
  s$ = s$ & Chr$(v)

  ' get the day of month
  If f Then
    t$ = UCase$(d$(2))
  Else
    t$ = UCase$(d$(3))
  End If
  If InStr(t$, "X") Or (Len(t$) = 0) Then
    v = 255 ' any day
  Else
    v = Val(t$)
  End If
  If (v > 255) Then v = 255
  s$ = s$ & Chr$(v)

  ' get the day of week
  t$ = UCase$(d$(1))
  Select Case t$
    Case "MON", "MONDAY": v = 1
    Case "TUE", "TUESDAY": v = 2
    Case "WED", "WEDNESDAY": v = 3
    Case "THU", "THURSDAY": v = 4
    Case "FRI", "FRIDAY": v = 5
    Case "SAT", "SATURDAY": v = 6
    Case "SUN", "SUNDAY": v = 7
    Case Else
      If InStr(t$, "X") Or (Len(t$) = 0) Then
        v = 255 ' any weekday
      Else
        v = Val(t$)
      End If
  End Select
  If (v > 255) Then v = 255
  s$ = s$ & Chr$(v)

  Get_Date = Len(s$)
End Function

Function Get_Double(s$, e$) As Integer
Dim v As Double

  v = Val(e$)
  s$ = String$(8, 0)
  
  vbeDOUBLE s$, v

  Get_Double = Len(s$)
End Function

Private Function Get_Integer(s$, e$, l%) As Integer
Dim v&, i%

  v = GetNumber(e$, 1)

  If l = 0 Then
    If (v > 0) And (v < 256) Then
      l = 1
    ElseIf (v > 0) And (v < 65536) Then
      l = 2
    ElseIf (v > 0) Then
      l = 4
    ElseIf (v >= -128) And (v < 128) Then
      v = (v And &HFF)
      l = 1
    ElseIf (v >= -32768) And (v < 32768) Then
      v = (v And &HFFFF)
      l = 2
    Else
      l = 4
    End If
  End If

  Select Case l
    Case 1 ' 8bit
      If (v < 0) Then v = 0
      If (v > 255) Then v = 255
      s$ = Chr$(v)
      Get_Integer = 1
    Case 2 ' 16bit
      If (v < 0) Then v = 0
      If (v > 65535) Then v = 65535
      s$ = Chr$(Hi(v)) & Chr$(Lo(v))
    Case 4 ' 32bit
      i = (v \ &H10000)
      v = (v And &HFFFF)
      s$ = Chr$(Hi(i)) & Chr$(Lo(i)) & Chr$(Hi(v)) & Chr$(Lo(v))
  End Select

  Get_Integer = l
End Function

Private Function Get_Object(s$, e$) As Integer
Dim p$, v%, i&

  p$ = LCase$(CheckValue(e$, 1))
  Select Case p$ ' the object-types are hardcoded
    Case "analog-input": v = 0
    Case "analog-output": v = 1
    Case "analog-value": v = 2
    Case "binary-input": v = 3
    Case "binary-output": v = 4
    Case "binary-value": v = 5
    Case "calendar": v = 6
    Case "command": v = 7
    Case "device": v = 8
    Case "event-enrollment": v = 9
    Case "file": v = 10
    Case "group": v = 11
    Case "loop": v = 12
    Case "multistate-input": v = 13
    Case "multistate-output": v = 14
    Case "notification-class": v = 15
    Case "program": v = 16
    Case "schedule": v = 17
    Case Else
      v = GetNumber(p$, 1)
  End Select

  ' get the instance for this object
  i = GetNumber(e$, 2)

  ' check and correct values
  If v < 0 Then v = 0
  If v > 1023 Then v = 1023 ' 10 bits
  If i < 0 Then i = 0
  If i > 4194303 Then i = 4194303 ' 22 bits

  On Error Resume Next
  'save the present object identifier
  'used to retrieve data from the PICS
  gObjectId = (CLng(v) * CLng(2 ^ 22)) + i
  On Error GoTo 0

  ' now store information in a string
  s$ = Chr$(v \ 4) & Chr$(((v And 3) * 64) Or (i \ 65536))
  s$ = s$ & Chr$((i \ 256) And 255) & Chr$(i And 255)
  Get_Object = 4
End Function

Private Function Get_Property(s$, e$) As Integer
Dim p$, v%

  p$ = LCase$(CheckValue(e$, 1))
  Select Case p$
    Case "acked-states": v = 0
    Case "ack-required": v = 1
    Case "action": v = 2
    Case "action-text": v = 3
    Case "active-text": v = 4
    Case "active-vt-sessions": v = 5
    Case "alarm-value": v = 6
    Case "alarm-values": v = 7
    Case "all": v = 8
    Case "all-writes-successful": v = 9
    Case "apdu-segment-timout": v = 10
    Case "apdu-timout": v = 11
    Case "application-software-version": v = 12
    Case "archive": v = 13
    Case "bias": v = 14
    Case "change-of-state-count": v = 15
    Case "change-of-state-time": v = 16
    Case "notification-class": v = 17
    Case "controlled-variable-reference": v = 19
    Case "controlled-variable-units": v = 20
    Case "controlled-variable-value": v = 21
    Case "cov-increment": v = 22
    Case "datelist": v = 23
    Case "daylight-savings-status": v = 24
    Case "deadband": v = 25
    Case "derivative-constant": v = 26
    Case "derivative-constant-units": v = 27
    Case "description": v = 28
    Case "description-of-halt": v = 29
    Case "device-address-binding": v = 30
    Case "device-type": v = 31
    Case "effective-period": v = 32
    Case "elapsed-active-time": v = 33
    Case "error-limit": v = 34
    Case "event-enabled": v = 35
    Case "event-state": v = 36
    Case "event-type": v = 37
    Case "exception-schedule": v = 38
    Case "fault-values": v = 39
    Case "feedback-value": v = 40
    Case "file-access-method": v = 41
    Case "file-size": v = 42
    Case "file-type": v = 43
    Case "firmware-revision": v = 44
    Case "high-limit": v = 45
    Case "inactive-text": v = 46
    Case "in-process": v = 47
    Case "instance-of": v = 48
    Case "integral-constant": v = 49
    Case "integral-constant-units": v = 50
    Case "issue-confirmed-notifications": v = 51
    Case "limit-enable": v = 52
    Case "list-of-group-members": v = 53
    Case "list-of-object-property-references": v = 54
    Case "list-of-session-keys": v = 55
    Case "local-date": v = 56
    Case "local-time": v = 57
    Case "location": v = 58
    Case "low-limit": v = 59
    Case "manipulated-variable-reference": v = 60
    Case "maximum-output": v = 61
    Case "max-apdu-length-accepted": v = 62
    Case "max-info-frames": v = 63
    Case "max-master": v = 64
    Case "max-pres-value": v = 65
    Case "minimum-off-time": v = 66
    Case "minimum-on-time": v = 67
    Case "minimum-output": v = 68
    Case "min-pres-value": v = 69
    Case "model-name": v = 70
    Case "modification-date": v = 71
    Case "notify-type": v = 72
    Case "number-of-apdu-retries": v = 73
    Case "number-of-states": v = 74
    Case "object-identifier": v = 75
    Case "object-list": v = 76
    Case "object-name": v = 77
    Case "object-property-reference": v = 78
    Case "object-type": v = 79
    Case "optional": v = 80
    Case "out-of-service": v = 81
    Case "output-units": v = 82
    Case "polarity": v = 84
    Case "present-value": v = 85
    Case "priority": v = 86
    Case "priority-array": v = 87
    Case "priority-for-writing": v = 88
    Case "process-identifier": v = 89
    Case "program-change": v = 90
    Case "program-location": v = 91
    Case "program-state": v = 92
    Case "proportional-constant": v = 93
    Case "proportional-constant-units": v = 94
    Case "protocol-conformance-class": v = 95
    Case "protocol-object-types-supported": v = 96
    Case "protocol-services-supported": v = 97
    Case "protocol-version": v = 98
    Case "read-only": v = 99
    Case "reason-for-halt": v = 100
    Case "recipient": v = 101
    Case "recipient-list": v = 102
    Case "reliability": v = 103
    Case "relinquish-default": v = 104
    Case "required": v = 105
    Case "resolution": v = 106
    Case "segmentation-supported": v = 107
    Case "setpoint": v = 108
    Case "setpoint-reference": v = 109
    Case "state-text": v = 110
    Case "status-flags": v = 111
    Case "system-status": v = 112
    Case "time-delay": v = 113
    Case "time-of-active-time-reset": v = 114
    Case "time-of-state-count-reset": v = 115
    Case "time-synchronization-recipients": v = 116
    Case "units": v = 117
    Case "update-interval": v = 118
    Case "utc-offset": v = 119
    Case "vendor-identifier": v = 120
    Case "vendor-name": v = 121
    Case "vt-classes-supported": v = 122
    Case "weekly-schedule": v = 123
    Case Else
      v = GetNumber(p$, 1)
  End Select

  'check and correct values
  If v < 0 Then v = 0
  If v > 255 Then v = 255
  
  'store information
  s$ = Chr$(v)
  Get_Property = 1
End Function

Function Get_Single(s$, e$) As Integer
Dim v As Single, l As Long

  v = Val(e$)
  s$ = String$(4, 0)

  vbeREAL s$, v

  Get_Single = Len(s$)
End Function

Function Get_Time(s$, e$) As Integer
Dim t$, c$, v%, i%, cf%
Static d$(5)

  ' reset all time vars
  For i = 1 To 5
    d$(i) = ""
  Next

  cf% = False
  v = 1

  i = 1
  Do While (i <= Len(e$))
    c$ = UCase$(Mid$(e$, i, 1))

    Select Case c$
      Case " " ' Space
        If cf Then
          If (v < 5) Then
            d$(v) = t$
            v = 4
          Else
            Display "Time format mismatch"
          End If

          cf = False
          v = v + 1
        End If

      Case ":" ' Colon
        If cf Then

          If (v < 3) Then
            d$(v) = t$
          Else
            Display "Time format mismatch"
          End If

          cf = False
          v = v + 1
        End If

      Case "." ' Point
        If cf Then

          Select Case v
            Case 1
              d$(3) = t$
              v = 3
            Case 2
              d$(3) = t$
              d$(2) = d$(1)
              d$(1) = ""
              v = 3
            Case 3
              d$(3) = t$
            Case 4, 5
              Display "Time format mismatch"
          End Select

          cf = False
          v = v + 1
        End If

      Case Else ' normal chars
        If Not cf Then
          cf = True
          t$ = c$
        Else
          t$ = t$ & c$
        End If
    End Select

    i = i + 1
  Loop

  d$(v) = t$

  ' okay, a lot of work is done, now get the values

  ' get the hour
  t$ = UCase$(d$(1))
  If InStr(t$, "X") Or (Len(t$) = 0) Then
    v = 255 ' any hour
  Else
    v = Val(t$) ' year
  End If
  If UCase$(d$(5)) = "PM" Then
    If (v <> 12) Then v = v + 12
  ElseIf UCase$(d$(5)) = "AM" Then
    If (v = 12) Then v = 0
  End If
  If (v > 255) Then v = 255
  s$ = Chr$(v)

  ' get the minute
  t$ = UCase$(d$(2))
  If InStr(t$, "X") Or (Len(t$) = 0) Then
    v = 255 ' any minute
  Else
    v = Val(t$)
  End If
  If (v > 255) Then v = 255
  s$ = s$ & Chr$(v)

  ' get the seconds
  t$ = UCase$(d$(3))
  If InStr(t$, "X") Or (Len(t$) = 0) Then
    v = 255 ' any second
  Else
    v = Val(t$)
  End If
  If (v > 255) Then v = 255
  s$ = s$ & Chr$(v)

  ' get the hundreds of the seconds
  t$ = UCase$(d$(4))
  If InStr(t$, "X") Or (Len(t$) = 0) Then
    v = 255 ' any weekday
  Else
    v = Val(t$)
  End If
  If (v > 255) Then v = 255
  s$ = s$ & Chr$(v)

  Get_Time = Len(s$)
End Function

Private Function GetAbortReason(e$) As Integer
Dim m$, nr%

  ' check for a number
  m$ = CheckValue(e$, 1)

  If (Left$(m$, 1) = "&") Or (Left$(m$, 1) >= "0" And Left$(m$, 1) <= "9") Then
    GetAbortReason = GetNumber(m$, 1)
    Exit Function
  End If
  
  ' every reason is hard coded ...
  Select Case UCase(m$)
    Case "OTHER": nr = 0
    Case "BUFFER-OVERFLOW": nr = 1
    Case "INVALID-APDU-IN-THIS-STATE": nr = 2
    Case "PREEMPTED-BY-HIGHER-PRIORITY-TASK": nr = 3
    Case "SEGMENTATION-NOT-SUPPORTED": nr = 4
    Case Else
      nr = 0
  End Select

  GetAbortReason = nr
End Function

Private Function GetALframe(cp%) As Integer
Dim tp%

  On Error Resume Next
  GetALframe = SBpacket(cp).AL

  If SBpacket(cp).AL = -1 Then
    tp = UBound(SBframeAL)
    ReDim Preserve SBframeAL(tp + 1)
    If Err = 0 Then
      On Error GoTo 0

      SBpacket(cp).AL = tp
      GetALframe = tp
      ' Clear Controls
      SBframeAL(tp).Ctrl.pdu = Chr$(0)
      SBframeAL(tp).Ctrl.Flags = Chr$(0)
      SBframeAL(tp).Ctrl.MaxResp = Chr$(0)
      SBframeAL(tp).Ctrl.invokeID = Chr$(0)
      SBframeAL(tp).Ctrl.SequenceNr = Chr$(0)
      SBframeAL(tp).Ctrl.WindowSize = Chr$(0)
      SBframeAL(tp).Ctrl.Service = Chr$(0)
      SBframeAL(tp).Ctrl.DATA = ""
    Else
      Display "AL table full"
    End If
  End If
End Function

Private Sub GetALrelStuff(tp%, k%, e$)
' tp = SBframeAL-packet
' k = keyword (-80)
' e$ = extension for this keyword
Dim t%, l%, a%, s$, ct$


  'check for identifier (gdud)
  e = ReplaceIdent(e$)

  ' catch control sign
  ct$ = GetControl(e$)

  ' search for tag - must be in decimal
  t = -1
  If (CheckSign(e$, ",", 1) > 0) And (Left$(e$, 1) >= "0") And (Left$(e$, 1) <= "9") Then
    t = GetNumber(e$, 1)
    e$ = Trim$(Mid$(e$, CheckSign(e$, ",", 1) + 1))
  End If
  If (k = 10) And (Left$(e$, 1) >= "0") And (Left$(e$, 1) <= "9") Then
    t = GetNumber(e$, 1)
    e$ = ""
  End If

  Select Case k
    Case 1 ' ObjectIdentifier
      l = Get_Object(s$, e$) ' get length for l% and data in s$
      a = 12 ' application tag 12
    Case 2 ' DeviceIdentifier
      l = Get_Object(s$, "device," & e$)
      a = 12 ' application tag 12
    Case 3 ' PropertyIdentifier
      If (t < 0) Then
        Display "Property must be a context tag!"
        t = 1 ' set default context tag 1
      End If
      l = Get_Property(s$, e$)
    Case 8 ' OpenTag
      s$ = ""
      t = GetNumber(e$, 1)
      l = -1
    Case 9 ' CloseTag
      s$ = ""
      t = GetNumber(e$, 1)
      l = -2
    Case 10 ' Null
      s$ = ""
      l = 0
      a = 0
    Case 11 ' Boolean
      l = Asc(GetBoolean(e$, 1))
      If (t < 0) Then ' decoding appl. type boolean
        s$ = ""
        a = 1 ' Application Tag Boolean
      Else ' decoding context type boolean
        s$ = Chr$(l)
        l = 1 ' length of boolean value
      End If
    Case 12 ' Unsigned
      l = Get_Integer(s$, e$, 0)
      a = 2
    Case 13 ' Unsigned8
      l = Get_Integer(s$, e$, 1)
      a = 2
    Case 14 ' Unsigned16
      l = Get_Integer(s$, e$, 2)
      a = 2
    Case 15 ' Unsigned32
      l = Get_Integer(s$, e$, 4)
      a = 2
    Case 16 ' Integer
      l = Get_Integer(s$, e$, 0)
      a = 3
    Case 17 ' Real
      l = Get_Single(s$, e$)
      a = 4
    Case 18 ' Double
      l = Get_Double(s$, e$)
      a = 5
    Case 19 ' Octet String
      s$ = GetStuff(e$, 1)
      l = Len(s$)
      a = 6
    Case 20 ' Character String

      If CheckSign(e$, ",", 1) Then  ' Character set endcoding included
        s$ = Get_CharSet(e$)
        s$ = s$ & GetStuff(e$, 2)
      Else ' just the charset, so claim it's ANSI X3.4
        s$ = Chr$(0) & GetStuff(e$, 1)
      End If
        l = Len(s$)
        a = 7
    Case 21 ' Bit String
      l = Get_BitString(s$, e$)
      a = 8
    Case 22 ' Enumerated
      l = Get_Integer(s$, e$, 0)
      a = 9
    Case 23 ' Date
      l = Get_Date(s$, e$)
      a = 10
    Case 24 ' Time
      l = Get_Time(s$, e$)
      a = 11
  End Select

  ' check for extended length
  If (l > 4) Then
    s$ = Chr$(l) & s$
    l = 5
  ElseIf (l = -1) Then
    l = 6 ' Opening Tag
  ElseIf (l = -2) Then
    l = 7 ' Closing Tag
  End If

  ' check for extended tag
  If (t >= 0) Then  ' context tag specified
    If t > 14 Then
      s$ = Chr$(t) & s$
      t = 248
    Else
      t = (t * 16) + 8
    End If
  Else
    t = (a * 16) ' use application tag
  End If

  ' assemble string to be stored
  s$ = Chr$(t Or l) & s$
  
  ' store information and control-word
  l = Len(s$)
  SBframeAL(tp).Ctrl.DATA = SBframeAL(tp).Ctrl.DATA & ct$ & Chr$(Lo(l)) & Chr$(Hi(l))
  SBframeAL(tp).DATA = SBframeAL(tp).DATA & s$
End Sub

Private Function GetBoolean(e$, i%) As String
Dim t$, b%

  t$ = CheckValue(e$, i%)
  Select Case UCase(t$)
    Case "YES", "Y", "TRUE", "T": b = 1
    Case "NO", "N", "FALSE", "F": b = 0
    Case Else
      b = GetNumber(t$, 1)
      If (b <> 0) Then b = 1
    End Select

  GetBoolean = Chr$(b)
End Function

Private Function GetControl(e$) As String
Dim v%

   v = 1
   Select Case Left$(e$, 1)
     Case "?"
       v = 0
       e$ = ""

     Case "<"
       v = 2
       e$ = Right$(e$, Len(e$) - 1)
       If (Len(e$) > 0) And Left$(e$, 1) = "=" Then
         v = 3
         e$ = Right$(e$, Len(e$) - 1)
       ElseIf (Len(e$) > 0) And Left$(e$, 1) = ">" Then
         v = 6
         e$ = Right$(e$, Len(e$) - 1)
       End If

     Case ">"
       v = 4
       e$ = Right$(e$, Len(e$) - 1)
       If (Len(e$) > 0) And Left$(e$, 1) = "=" Then
         v = 5
         e$ = Right$(e$, Len(e$) - 1)
       End If
    
     Case "!"
       If (Len(e$) > 1) And Mid$(e$, 2, 1) = "=" Then
         v = 6
         e$ = Right$(e$, Len(e$) - 2)
       End If
   End Select

   e$ = LTrim$(e$)
   GetControl = Chr$(v)
End Function

Private Function GetDLCframe(cp%) As Integer
Dim tp%

  On Error Resume Next
  GetDLCframe = SBpacket(cp).DLC

  If SBpacket(cp).DLC = -1 Then
    tp = UBound(SBframeDLC)
    ReDim Preserve SBframeDLC(tp + 1)
    If Err = 0 Then
      On Error GoTo 0

      SBpacket(cp).DLC = tp
      GetDLCframe = tp
      ' Clear Controls
      SBframeDLC(tp).Ctrl.DA = Chr$(0)
      SBframeDLC(tp).Ctrl.SA = Chr$(0)
      SBframeDLC(tp).Ctrl.LSAP = Chr$(0)
    Else
      Display "DLC table full"
    End If
  End If
End Function

Private Sub GetExpression(s$, k%, e$)
Dim i, l As Integer
Dim c As String * 1

  i = 1
  l = Len(s$)

  ' Extract first word as keyword
  If (i < l) Then c = Left$(s$, 1)
  'If (Left(s, 2) = "BI") Then
    'MsgBox "GE: enter s = '" & s & "' key = " & k
  'End If
  Do While (i < l) And (c <> " ") And (c <> Chr$(9))
    i = i + 1
    If (i < l) Then c = Mid$(s$, i, 1)
  Loop
  'If (Left(s, 2) = "BI") Then
    'MsgBox "GE: after loop 1"
    'End If
  ' split off the keyword from string
  If (i < l) Then
    e$ = Right$(s$, l - i)
    s$ = UCase$(Left$(s$, i - 1))
  Else
    e$ = ""
    s$ = UCase$(s$)
  End If
  'If (Left(s, 2) = "BI") Then
      'MsgBox "GE: before loop 2"
    'End If
      
  ' find the beginning of the extension
  If (Len(e$) > 0) Then c = Left$(e$, 1)
  Do While (Len(e$) > 0) And ((c = " ") Or (c = Chr$(9) Or (c = "=")))
    e$ = Right$(e$, Len(e$) - 1)
    If (Len(e$) > 0) Then c = Left$(e$, 1)
  Loop
  'If (Left(s, 2) = "BI") Then
      'MsgBox "GE: after loop 2"
    'End If

  ' cut off any comment at the end of the line
  i = CheckSign(e$, "--", 1)
  If (i > 0) Then e$ = Left$(e$, i - 1)
  i = CheckSign(e$, ";", 1)
  If (i > 0) Then e$ = Left$(e$, i - 1)
  MyTrim e$ 'remove leading/trailing spaces/tabs
  'e$ = Trim$(e$)

  ' you MUST NOT change any number below this comment
  If Left$(s$, 2) = "--" Then
    k = 9
  ElseIf Left$(s$, 1) = "(" Then
    k = 7
  ElseIf Left$(s$, 1) = ")" Then
    k = 8
  Else: Select Case s$
      Case "SETUP": k = 1
      Case "SECTION": k = 2
      Case "COMMENT": k = 9
      Case "TEST": k = 10
      Case "DEPS", "DEPENDENCIES": k = 11
      Case "REF", "REFERENCE": k = 12
      Case "SEND": k = 13
      Case "EXPECT": k = 14
      Case "reserved": k = 15
      Case "OR": k = 16
      Case "AND": k = 17
      Case "CASE": k = 20
      Case "ENDCASE": k = 21
      Case "NET", "NETWORK": k = 30
      Case "DA": k = 31
      Case "SA": k = 32
      Case "LSAP": k = 33
      Case "reserved": k = 34
      Case "DEST", "DESTINATION": k = 35
      Case "ROUTER": k = 36
      Case "VERSION": k = 40
      Case "DNET": k = 41
      Case "DADR": k = 42
      Case "SNET": k = 43
      Case "SADR": k = 44
      Case "HOPCNT": k = 45
      Case "MSG", "MESSAGE": k = 46
      Case "PRIO", "PRIORITY": k = 47
      Case "DER": k = 48
      Case "VENDORID": k = 49
      Case "NL", "NLDATA": k = 50
      Case "PDU", "PDU-TYPE": k = 60
      Case "PDU-FLAGS", "FLAGS": k = 61
      Case "MAXRESP", "MAXRESPONSE", "MAXSIZE": k = 62
      Case "INVOKEID": k = 63
      Case "SEQUENCENR": k = 64
      Case "WINDOWSIZE": k = 65
      Case "AL", "ALDATA": k = 69
      Case "SERVICE", "ERROR", "ERROR-CHOICE": k = 70
      Case "REJECT-REASON": k = 71
      Case "ABORT-REASON": k = 72
      Case "OBJECT", "OBJECTIDENTIFIER": k = 81
      Case "DEVICE", "DEVICEIDENTIFIER": k = 82
      Case "PROPERTY", "PROPERTYIDENTIFIER": k = 83
      Case "OPEN", "OPENTAG", "OPENINGTAG": k = 88
      Case "CLOSE", "CLOSETAG", "CLOSINGTAG": k = 89
      Case "NULL": k = 90
      Case "BOOL", "BOOLEAN": k = 91
      Case "UNSIGNED": k = 92
      Case "UNSIGNED8": k = 93
      Case "UNSIGNED16": k = 94
      Case "UNSIGNED32": k = 95
      Case "INTEGER": k = 96
      Case "REAL", "SINGLE": k = 97
      Case "DOUBLE": k = 98
      Case "OCTETSTRING": k = 99
      Case "CHARSTRING", "CHARACTERSTRING": k = 100
      Case "BITSTRING": k = 101
      Case "ENUM", "ENUMERATED": k = 102
      Case "DATE": k = 103
      Case "TIME": k = 104
      Case Else: k = 0
    End Select
  End If
End Sub

Function GetIndexValue(e$, i%) As String
Dim l1%, l2%

  GetIndexValue = ""
  If i <= 0 Then i = 1

  ' get the right index
  l2 = 0
  Do
    i = i - 1
    l1 = l2 + 1
    l2 = CheckSign(e$, ",", l1)
  Loop Until (i = 0) Or (l2 = 0)
  If l2 = 0 Then l2 = Len(e$) + 1

  ' then extract the value
  If i = 0 Then
    GetIndexValue = Trim$(Mid$(e$, l1, l2 - l1))
  End If
End Function

Private Function GetMAC(e$, typ%) As String
Dim t$, k%, n$

  t = CheckValue(e$, 1)
  If UCase$(t) = "BROADCAST" Then
    If (typ > 0) And (typ < 5) Then
      n$ = Chr$(0) & Chr$(typ)
    Else
      Display "Unknown network type"
    End If
  Else
    n$ = Chr$(Int(Len(t$) / 2))
    For k = 1 To Len(t$) Step 2
      If IsHex(Mid$(t$, k, 2)) Then
        n$ = n$ + Chr$(Val("&H" & Mid$(t$, k, 2)))
      Else
        Display "Illegal Hex Value"
      End If
    Next

    ' now check for correct length
    Select Case typ
      Case 1
        If (Len(n$) <> 7) Then Display "Wrong Ethernet mac length"
      Case 2, 3
        If (Len(n$) <> 2) Then Display "Wrong ARCNET or MS/TP mac length"
      Case 4
        If (Len(n$) <> 3) Then Display "Wrong LonTalk mac length"
      Case Else
        Display "Unknown mac length"
    End Select
  End If

  GetMAC = n$
End Function

Private Function GetMaxResp(e$) As String
Dim s%

  s = GetNumber(e$, 1)
  If s > 15 Then
    Select Case s
      Case Is <= 50: s = 0 ' Minimum Message Size
      Case Is <= 128: s = 1
      Case Is <= 206: s = 2 ' fits in a LonTalk frame
      Case Is <= 480: s = 3 ' fits in a ARCNET frame
      Case Is <= 1024: s = 4
      Case Is <= 1476: s = 5 ' fits in a ISO 8802-3 frame
      Case Is > 1476: s = 6 ' all others reserved by ASHRAE
    End Select
  End If

  GetMaxResp = Chr$(s)
End Function

Private Function GetMSG(e$) As String
Dim i%, t$, r$

  ' get the first value
  t$ = CheckValue(e$, 1)

  ' now correct any seperator
  For i = 1 To Len(t$)
    If Mid$(t$, i, 1) = "_" Then Mid$(t$, i, 1) = "-"
  Next

  ' search for names
  Select Case UCase(t$)
    Case "WHO-IS-ROUTER-TO-NETWORK", "WHO'S-RTN"
      r$ = Chr$(Who_Is_Router_To_Network)
      
    Case "I-AM-ROUTER-TO-NETWORK", "I'M-RTN"
      r$ = Chr$(I_Am_Router_To_Network)

    Case "I-COULD-BE-ROUTER-TO-NETWORK", "I-CLD-BE-RTN"
      r$ = Chr$(I_Could_Be_Router_To_Network)

    Case "REJECT-MESSAGE-TO-NETWORK", "REJ-MTN"
      r$ = Chr$(Reject_Message_To_Network)

    Case "ROUTER-BUSY-TO-NETWORK", "RBTN"
      r$ = Chr$(Router_Busy_To_Network)

    Case "ROUTER-AVAILABLE-TO-NETWORK", "RATN"
      r$ = Chr$(Router_Available_To_Network)

    Case "INITIALIZE-ROUTING-TABLE", "IRT"
      r$ = Chr$(Initialize_Routing_Table)

    Case "INITIALIZE-ROUTING-TABLE-ACK", "IRT-ACK"
      r$ = Chr$(Initialize_Routing_Table_Ack)

    Case "ESTABLISH-CONNECTION-TO-NETWORK", "ECTN"
      r$ = Chr$(Establish_Connection_To_Network)

    Case "DISCONNECT-CONNECTION-TO-NETWORK", "DCTN"
      r$ = Chr$(Disconnect_Connection_To_Network)

    Case Else
      i = GetNumber(t$, 1)
      If (i >= 0) And (i < 128) Then
        r$ = Chr$(i)
      Else
        Display "Illegeal message number"
        r$ = Chr$(127)
      End If
  End Select

  GetMSG = r$
End Function

Private Sub GetMSGparm(tp%, ext$)
Dim v%, d%, t$, l%

  l = Len(SBframeNL(tp).DATA)
  v = 2
  Do
    t$ = CheckValue(ext$, v)
    If Len(t$) > 0 Then
      Select Case Asc(SBframeNL(tp).Message)
        Case Who_Is_Router_To_Network, I_Am_Router_To_Network, Router_Busy_To_Network, Router_Available_To_Network, Disconnect_Connection_To_Network
          d = GetNumber(t$, 1) ' dnets
          SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(Hi(d)) + Chr$(Lo(d))
          
        Case I_Could_Be_Router_To_Network
          Select Case v
            Case 2 ' dnet
              d = GetNumber(t$, 1)
              SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(Hi(d)) + Chr$(Lo(d))
              
            Case 3 ' performance index
              d = GetNumber(t$, 1)
              If (d >= 0) And (d < 256) Then
                SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(d)
              Else
                Display "Illegal performance index"
              End If
          End Select

        Case Reject_Message_To_Network
          Select Case v
            Case 2 ' reject reason
              d = GetNumber(t$, 1)
              If (d >= 0) And (d < 256) Then ' valid values from 0 to 5
                SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(d)
              Else
                Display "Illegal reject reason"
              End If
            Case 3 ' dnet
              d = GetNumber(t$, 1)
              SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(Hi(d)) + Chr$(Lo(d))
          End Select

        Case Initialize_Routing_Table, Initialize_Routing_Table_Ack
          Select Case Int((v - 3) Mod 3)
            Case -1 ' number of ports
              d = GetNumber(t$, 1)
              If (d >= 0) And (d < 256) Then
                SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(d)
              Else
                Display "Illegal number of ports"
              End If
            Case 0 ' dnet
              d = GetNumber(t$, 1)
              SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(Hi(d)) + Chr$(Lo(d))
            Case 1 ' port id
              d = GetNumber(t$, 1)
              If (d >= 0) And (d < 256) Then
                SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(d)
              Else
                Display "Illegal port id"
              End If
            Case 2 ' PortInfo
              t$ = GetStuff(t$, 1)
              d = Len(t$)
              If (d >= 0) And (d < 256) Then
                SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(d) + t$
              Else
                Display "Illegal port info length"
              End If
          End Select

        Case Establish_Connection_To_Network
          Select Case v
            Case 2 ' dnet
              d = GetNumber(t$, 1)
              SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(Hi(d)) + Chr$(Lo(d))
            Case 3 ' termination time
              d = GetNumber(t$, 1)
              If (d >= 0) And (d < 256) Then
                SBframeNL(tp).DATA = SBframeNL(tp).DATA + Chr$(d)
              Else
                Display "Illegal termination time"
              End If
          End Select
      End Select
    End If ' If Len(t$) > 0
    v = v + 1
  Loop Until Len(t$) = 0

  ' some datas added?
  If (l < Len(SBframeNL(tp).DATA)) Then
    l = Len(SBframeNL(tp).DATA) - l
    SBframeNL(tp).Ctrl.DATA = SBframeNL(tp).Ctrl.DATA & SBframeNL(tp).Ctrl.Message
    SBframeNL(tp).Ctrl.DATA = SBframeNL(tp).Ctrl.DATA & Chr$(Lo(l)) & Chr$(Hi(l))
  End If

End Sub

Private Function GetNLframe(cp%) As Integer
Dim tp%

  On Error Resume Next
  GetNLframe = SBpacket(cp).NL

  If SBpacket(cp).NL = -1 Then
    tp = UBound(SBframeNL)
    ReDim Preserve SBframeNL(tp + 1)
    If Err = 0 Then
      On Error GoTo 0

      SBpacket(cp).NL = tp
      GetNLframe = tp
      ' Clear Controls
      SBframeNL(tp).Ctrl.Version = Chr$(0)
      SBframeNL(tp).Ctrl.DNET = Chr$(0)
      SBframeNL(tp).Ctrl.DADR = Chr$(0)
      SBframeNL(tp).Ctrl.SNET = Chr$(0)
      SBframeNL(tp).Ctrl.SADR = Chr$(0)
      SBframeNL(tp).Ctrl.HOPCNT = Chr$(0)
      SBframeNL(tp).Ctrl.Message = Chr$(0)
      SBframeNL(tp).Ctrl.priority = Chr$(0)
      SBframeNL(tp).Ctrl.DER = Chr$(0)
      SBframeNL(tp).Ctrl.VendorID = Chr$(0)
      SBframeNL(tp).Ctrl.DATA = ""
    Else
      Display "NL table full"
    End If
  End If
End Function


Private Function GetNumber(e$, i%) As Long
Dim t$

  t$ = CheckValue(e$, i%)

  If UCase$(Left$(t$, 2)) = "0X" Then
    Mid$(t$, 1, 2) = "&H"
  ElseIf UCase$(Left$(t$, 2)) = "0O" Then
    Mid$(t$, 1, 2) = "&O"
  ElseIf (UCase$(Left$(t$, 2)) = "0B") Or (UCase$(Left$(t$, 2)) = "&B") Then
    GetNumber = Bin(Mid$(t$, 3))
    Exit Function
  End If

  GetNumber = Val(t$)
End Function

Private Function GetPDU(e$) As Integer
Dim s$, p%

  ' check for a number
  s$ = UCase$(CheckValue(e$, 1))
  If (Left$(s$, 1) = "&") Or (Left$(s$, 1) >= "0" And Left$(s$, 1) <= "9") Then
    GetPDU = GetNumber(s$, 1)
    Exit Function
  End If
  
  For p = 1 To Len(s$)
    If (Mid$(s$, p, 1) = "_") Then Mid$(s$, p, 1) = "-"
  Next

  If s$ = "CONFIRMED-REQUEST" Then
    p = 0
  ElseIf s$ = "UNCONFIRMED-REQUEST" Then
    p = 1
  ElseIf s$ = "SIMPLEACK" Then
    p = 2
  ElseIf s$ = "COMPLEXACK" Then
    p = 3
  ElseIf s$ = "SEGMENTACK" Then
    p = 4
  ElseIf s$ = "ERROR" Then
    p = 5
  ElseIf s$ = "REJECT" Then
    p = 6
  ElseIf s$ = "ABORT" Then
    p = 7
  Else ' any other set to confirmed request
    p = 0
  End If

  GetPDU = p
End Function

'This function is used to read a value from the PICS
'database. The value will be encoded according to its
'ASN.1 production.
'in:  Expr      PICS identifier in curly braces, e.g {IDENT["["ArrayIndex"]"] [,tag-nr]}
'out: Expr      encoded property value. Syntax: X'7505AFFE'
Sub GetPicsIdent(Expr As String)
Dim PropId As Integer, TagNr As Integer, pos1 As Integer
Dim e As Integer, Msg As PVMessage, ArrayIndex As Integer
Dim t As String, i As Integer, pos2 As Integer

If (Not gDBloaded) Then    'database not loaded!
  Expr$ = "1"              'default, no meaning
  Display "No Database! Select a PICS file!"
  Exit Sub
End If
    
TagNr = -1
On Error Resume Next
  Expr = LCase$(Trim$(Mid$(Expr, 2, Len(Expr) - 2)))  'remove parenthesis
  'retrieve the tag number: --------------
  pos1 = InStr(Expr, ",")                             'tag separated by comma
  If pos1 > 0 Then                                    'there is a tag
    t = Trim$(Mid$(Expr, pos1 + 1))
    If pos1 > 1 Then Expr = Mid$(Expr, 1, pos1 - 1)   'remove tag
    If Left$(t, 2) = "0x" Then                        'hex tag
      Mid$(t, 1, 2) = "&h"
      TagNr = Val(t)
    ElseIf Left$(t, 2) = "0o" Then                    'octal tag
      Mid$(t, 1, 2) = "&o"
      TagNr = Val(t)
    ElseIf (Left$(t, 2) = "0b") Or (Left$(t$, 2) = "&b") Then 'binary tag
      TagNr = Bin(Mid$(t, 3))
    End If
  End If
  'retrieve the array index: -------------
  pos1 = InStr(Expr, "[") 'open array index
  pos2 = InStr(Expr, "]") 'close array index
  ArrayIndex = -1
  If (pos1 > 0) And (pos2 > (pos1 + 1)) Then
    ArrayIndex = CInt(Trim$(Mid$(Expr, pos1 + 1, pos2 - pos1 - 1)))
    Expr = Mid$(Expr, 1, pos1 - 1) 'remove array index
  End If

On Error GoTo 0

'lookup the property name table
PropId = -1 'not found
For i = LBound(gPropNameTable) To UBound(gPropNameTable)
  If StrComp(gPropNameTable(i), Expr, 0) = 0 Then 'case sensitive (faster?)
    PropId = i
    Exit For
  End If
Next i

If (PropId = -1) Then
  Display "Property: " & Expr & " unknown!"
  Expr = "1"
  Exit Sub
End If

'encode the property value: ----------------

Msg.Action = cASN_1_ANY_PROP               'encode ASN Property Value
Msg.PropId = PropId                        'this Property
Msg.Obj = GetpObj(gDB.DataBase, gObjectId) 'get the pointer to the object
Msg.TagNr = TagNr                          'use this tag number
Msg.ArrayIndex = ArrayIndex                'use this array index
Msg.BufferLen = cSLen                      'length of Buffer

If Msg.Obj = 0 Then
  Dim s As String
  Dim c As Integer
  c = LoadString(7799, s, cSLen): AdjustString s
  Display s
  Expr = "1"
  Exit Sub
End If

Expr = Space(cApduLen)
e = GetPropValue(Expr, Msg)                'encode Property
Expr = Mid$(Expr, 1, Msg.BufferLen)

'convert Expr to hex string representation:
Expr = "X'" & HexStr(Expr, 1, Len(Expr)) & "'"

End Sub

Function GetPriority(e$) As Integer
Dim t$, nr%

  ' get the first value
  t$ = CheckValue(e$, 1)

  ' search for names
  Select Case UCase(t$)
    Case "NORMAL": nr = 0
    Case "URGENT": nr = 1
    Case "CRITICAL": nr = 2
    Case "LIFESAFETY": nr = 3
    Case Else
      nr = GetNumber(t$, 1)
  End Select

  GetPriority = nr
End Function

Private Function GetRejectReason(e$) As Integer
Dim m$, nr%

  ' check for a number
  m$ = CheckValue(e$, 1)

  If (Left$(m$, 1) = "&") Or (Left$(m$, 1) >= "0" And Left$(m$, 1) <= "9") Then
    GetRejectReason = GetNumber(m$, 1)
    Exit Function
  End If
  
  ' every reason is hard coded ...
  Select Case UCase(m$)
    Case "OTHER": nr = 0
    Case "BUFFER-OVERFLOW": nr = 1
    Case "INCONSISTENT-PARAMETERS": nr = 2
    Case "INVALID-PARAMETER-DATA-TYPE": nr = 3
    Case "INVALID-TAG": nr = 4
    Case "MISSING-REQUIRED-PARAMETER": nr = 5
    Case "PARAMETER-OUT-OF-RANGE": nr = 6
    Case "TOO-MANY-ARGUMENTS": nr = 7
    Case "UNDEFINED-ENUMERATION": nr = 8
    Case "UNRECOGNIZED-SERVICE": nr = 9
    Case Else
      nr = 0
  End Select

  GetRejectReason = nr
End Function

Private Function GetService(e$, s%) As Integer
Dim nr%, m$

  ' check for a number
  m$ = CheckValue(e$, 1)
  If (Left$(m$, 1) = "&") Or (Left$(m$, 1) >= "0" And Left$(m$, 1) <= "9") Then
    GetService = GetNumber(m$, 1)
    Exit Function
  End If
  
  ' every service is hard coded ...
  If (s = 0) Or (s = 2) Or (s = 3) Or (s = 5) Then
    Select Case UCase(m$)
      Case "ACKNOWLEDGEALARM": nr = 0
      Case "CONFIRMEDCOVNOTIFICATION": nr = 1
      Case "CONFIRMEDEVENTNOTIFICATION": nr = 2
      Case "GETALARMSUMMARY": nr = 3
      Case "GETENROLLMENTSUMMARY": nr = 4
      Case "SUBSCRIBECOV": nr = 5
      Case "ATOMICREADFILE": nr = 6
      Case "ATOMICWRITEFILE": nr = 7
      Case "ADDLISTELEMENT": nr = 8
      Case "REMOVELISTELEMENT": nr = 9
      Case "CREATEOBJECT": nr = 10
      Case "DELETEOBJECT": nr = 11
      Case "READPROPERTY": nr = 12
      Case "READPROPERTYCONDITIONAL": nr = 13
      Case "READPROPERTYMULTIPLE": nr = 14
      Case "WRITEPROPERTY": nr = 15
      Case "WRITEPROPERTYMULTIPLE": nr = 16
      Case "DEVICECOMMUNICATIONCONTROL": nr = 17
      Case "CONFIRMEDPRIVATETRANSFER": nr = 18
      Case "CONFIRMEDTEXTMESSAGE": nr = 19
      Case "REINITIALIZEDEVICE": nr = 20
      Case "VTOPEN": nr = 21
      Case "VTCLOSE": nr = 22
      Case "VTDATA": nr = 23
      Case "AUTHENTICATE": nr = 24
      Case "REQUESTKEY": nr = 25
      Case Else: s = -1
    End Select
  ElseIf s = 1 Then ' unconfirmed service
    Select Case UCase(m$)
      Case "I-AM": nr = 0
      Case "I-HAVE": nr = 1
      Case "UNCONFIRMEDCOVNOTIFICATION": nr = 2
      Case "UNCONFIRMEDEVENTNOTIFICATION": nr = 3
      Case "UNCONFIRMEDPRIVATETRANSFER": nr = 4
      Case "UNCONFIRMEDTEXTMESSAGE": nr = 5
      Case "TIMESYNCHRONIZATION": nr = 6
      Case "WHO-HAS": nr = 7
      Case "WHO-IS": nr = 8
      Case Else: s = -1
    End Select
  Else
    s = -1 ' Wrong parameter for this pdu
  End If

  GetService = nr
End Function

Private Function GetStuff(e$, i%) As String
Dim t$, r$, k%
Dim c As Integer

  ' check for identifier
  t$ = CheckValue(e$, i%)
  r$ = ""

  ' check for Hex/Dec/Asc/Bin-Data
  If (Mid$(t$, 2, 1) = "'") And (Right$(t$, 1) = "'") Then
    Select Case UCase(Left$(t$, 1))
      Case "X" ' Hexadecimal
        c = Len(t$) - 1
        For k = 3 To c Step 2
          If IsHex(Mid$(t$, k, 2)) Then
            r$ = r$ + Chr$(Val("&H" & Mid$(t$, k, 2)))
          Else
            Display "Illegal Hex Value"
          End If
        Next
      Case "D" ' Decimal
        For k = 3 To Len(t$) - 1 Step 3
          If IsDec(Mid$(t$, k, 3)) Then
            r$ = r$ + Chr$(Val(Mid$(t$, k, 3)))
          Else
            Display "Illegal Dec Value"
          End If
        Next
      Case "A" ' Ascii
        r$ = Mid$(t$, 3, Len(t$) - 3)
      Case "B" ' Binary
        For k = 3 To Len(t$) - 1 Step 8
          If IsBin(Mid$(t$, k, 8)) Then
            r$ = r$ + Chr$(Bin(Mid$(t$, k, 8)))
          Else
            Display "Illegal Bin Value"
          End If
        Next
      Case Else ' Unkown
        Display "Unknown data specification"
    End Select
  ElseIf (Asc(t$) = 34) And (Right$(t$, 1) = Chr$(34)) Then
    If Len(t$) > 2 Then r$ = Mid$(t$, 2, Len(t$) - 2)
  Else
    k = Val(t$)
    If (k >= 0) And (k < 256) Then
      r$ = Chr$(k)
    Else
      Display "Illegal Stuff Byte"
    End If
  End If

  GetStuff = r$
End Function

Function Hi(ByVal v As Integer) As Integer
  Hi = ((v And &HFF00) \ 256&) And 255
End Function

Sub InitSBT()
  ' setting the most important variable
  TestInProgress = -1

  ' initialize the receive list buffer
  ReDim SBrecList(0)
End Sub

Private Function IsBin(t$) As Integer
Dim i%

  IsBin = True
  For i = 1 To Len(t$)
    If (Mid$(t$, i, 1) < "0") Or (Mid$(t$, i, 1) > "1") Then
      IsBin = False
      Exit For
    End If
  Next
End Function

Private Function IsDec(t$) As Integer
Dim i%

  IsDec = True
  For i = 1 To Len(t$)
    If (Mid$(t$, i, 1) < "0") Or (Mid$(t$, i, 1) > "9") Then
      IsDec = False
      Exit For
    End If
  Next
End Function

Private Function IsHex(t$) As Integer
Dim i%, c As String * 1

  IsHex = True
  For i = 1 To Len(t$)
    c = UCase(Mid$(t$, i, 1))
    If (c < "0") Or (c > "F") Or ((c > "9") And (c < "A")) Then
      IsHex = False
      Exit For
    End If
  Next
End Function

Function Lo(ByVal v As Integer) As Integer
  Lo = v And 255
End Function

Sub MyTrim(e As String)
Dim l As Integer, i As Integer
Dim ch As Integer
l = Len(e)
For i = 1 To l
  ch = Asc(Mid$(e, i, 1))
  If (ch <> 32) And (ch <> 9) Then 'not a space/tab
    Exit For
  End If
Next i
e = Mid$(e, i, l)
For i = l To 1 Step -1
  ch = Asc(Mid$(e, i, 1))
  If (ch <> 32) And (ch <> 9) Then 'not a space/tab
    Exit For
  End If
Next i
e = Mid$(e, 1, i)
End Sub

Private Function ParsePacket(ci%, csl%, s$) As Integer
Dim e%, cp%, tp%, net%, dtyp%, styp%
Dim ext$, key%, v%, Service%, l%

  ' standard wrong
  ParsePacket = -1

  ' make a new packet
  cp = UBound(SBpacket)
  On Error Resume Next
  ReDim Preserve SBpacket(cp + 1)
  If Err <> 0 Then Exit Function
  On Error GoTo 0

  ' check for previous packets
  If (ci > 2) Then ' And/Or
    If (cp > 0) Then ' Exist previous packet
      ci = (Asc(SBpacket(cp - 1).Command) And 128) + ci
    Else
      Display "And/Or without Send/Expect"
    End If
  ElseIf (ci = 1) Then ' Expect
    ci = 129
  End If
  ' check for case level marker packets
  If (csl And 128) And (ci <> 129) Then
    Display "Expect packet expected"
  End If
  ' set new packet to default values
  SBpacket(cp).Command = Chr$(ci)
  SBpacket(cp).CaseLvl = Chr$(csl)
  SBpacket(cp).DLC = -1
  SBpacket(cp).NL = -1
  SBpacket(cp).AL = -1
  ' clear network types
  net = 0
  dtyp = 0
  styp = 0
  ' clear sercice choice
  Service = -1

  ' search for begin of packet
  e = True
  Do While e And (CheckSign(s$, "(", 1) = 0)
    e = ReadLine(s$)
  Loop

  ' get first keyword
  If e Then
    s$ = Trim$(Right$(s$, Len(s$) - CheckSign(s$, "(", 1)))
  End If

  Do While e
    If e Then

      ' cut off any comment at the end of the line
      v = CheckSign(s$, "--", 1)
      If (v > 0) Then s$ = Left$(s$, v - 1)
      v = CheckSign(s$, ";", 1)
      If (v > 0) Then s$ = Left$(s$, v - 1)
      s$ = Trim$(s$)

      ' check for closing bracket
      If CheckSign(s$, ")", 1) > 0 Then
        s$ = RTrim$(Left$(s$, CheckSign(s$, ")", 1) - 1))
        e = False
      End If

      If Len(s$) > 0 Then
        ' Parse String
        GetExpression s$, key, ext$

        ' Update status information
        Display ""
        
        Select Case key
          ' general section
          Case 9 ' Comment
            ' ignore comment
          Case 30 ' NETWORK
            If net = 0 Then
              SBpacket(cp).Port = GetNumber(ext$, 3)
              net = DecodeNetworkType(CheckValue(ext$, 2))
            Else
              Display "Double network identifier"
            End If

          ' DLC section
          Case 31 ' DA
            If net > 0 Then
              tp = GetDLCframe(cp)
              If tp < 0 Then Exit Function
              SBframeDLC(tp).Ctrl.DA = GetControl(ext$)
              SBframeDLC(tp).DA = GetMAC(ext$, net)
            Else
              Display "NETWORK not specified correctly"
            End If
          Case 32 ' SA
            If net > 0 Then
              tp = GetDLCframe(cp)
              If tp < 0 Then Exit Function
              SBframeDLC(tp).Ctrl.SA = GetControl(ext$)
              SBframeDLC(tp).SA = GetMAC(ext$, net)
            Else
              Display "NETWORK not specified correctly"
            End If
          Case 33 ' LSAP
            tp = GetDLCframe(cp)
            If tp < 0 Then Exit Function
            SBframeDLC(tp).Ctrl.LSAP = GetControl(ext$)
            v = GetNumber(ext$, 1)
            If (v >= 0) And (v < 256) Then
              SBframeDLC(tp).LSAP = Chr$(v)
            Else
              Display "Illegal LSAP number"
            End If
          Case 35 ' DESTINATION
            ' If it's a single parameter, check for identifier
            If CheckSign(ext$, ",", 1) = 0 Then
              v = SearchNata(ext$)
              If (v >= 0) Then ext$ = RTrim$(SBNata(v).Value)
            End If
            ' check for destination specifications
            If CheckSign(ext$, ",", 1) > 0 Then ' DNET/DADR value
              tp = GetNLframe(cp)
              If tp < 0 Then Exit Function
              dtyp = DecodeNetworkType(CheckValue(ext$, 3))
              If (dtyp <> 0) Then
                SBframeNL(tp).Ctrl.DADR = Chr$(1) ' equal
                SBframeNL(tp).Ctrl.DNET = Chr$(1) ' equal
                SBframeNL(tp).DADR = GetMAC(CheckValue(ext$, 2), dtyp)
                SBframeNL(tp).DNET = GetNumber(ext$, 1)
                tp = GetDLCframe(cp)
                If tp < 0 Then Exit Function
                If Asc(SBframeDLC(tp).Ctrl.DA) = 0 Then
                  SBframeDLC(tp).Ctrl.DA = Chr$(1) ' equal
                  SBframeDLC(tp).DA = GetMAC("BROADCAST", net)
                End If
              Else
                Display "Destination not defined correctly"
              End If
            Else ' only MAC specified
              tp = GetDLCframe(cp)
              If tp < 0 Then Exit Function
              v = CheckDest(ext$)
              SBframeDLC(tp).Ctrl.DA = Chr$(1) ' equal
              SBframeDLC(tp).DA = GetMAC(ext$, net)
              If v = 1 Then
                tp = GetNLframe(cp)
                If tp < 0 Then Exit Function
                SBframeNL(tp).Ctrl.DNET = Chr$(1) ' equal
                SBframeNL(tp).DNET = &HFFFF ' global-broadcast
              End If
            End If
          Case 36 ' ROUTER
            tp = GetDLCframe(cp)
            If tp < 0 Then Exit Function
            v = CheckDest(ext$)
            If v <> 1 Then ' other than global broadcast
              SBframeDLC(tp).Ctrl.DA = Chr$(1) ' equal
              SBframeDLC(tp).DA = GetMAC(ext$, net)
            Else
              Display "Global broadcast not allowed"
            End If

          ' NL section
          Case 40 ' VERSION
            tp = GetNLframe(cp)
            If tp < 0 Then Exit Function
            SBframeNL(tp).Ctrl.Version = GetControl(ext$)
            v = GetNumber(ext$, 1)
            If (v >= 0) And (v < 256) Then
              SBframeNL(tp).Version = Chr$(v)
            Else
              Display "Illegal version number"
            End If
          Case 41 ' DNET
            tp = GetNLframe(cp)
            If tp < 0 Then Exit Function
            SBframeNL(tp).Ctrl.DNET = GetControl(ext$)
            SBframeNL(tp).DNET = GetNumber(ext$, 1)
            dtyp = DecodeNetworkType(CheckValue(ext$, 2))
            If (dtyp = 0) And (SBframeNL(tp).DNET = -1) Then
              dtyp = 1 ' set Ethernet as default
            End If
          Case 42 ' DADR
            If dtyp > 0 Then
              tp = GetNLframe(cp)
              If tp < 0 Then Exit Function
              SBframeNL(tp).Ctrl.DADR = GetControl(ext$)
              SBframeNL(tp).DADR = GetMAC(ext$, dtyp)
            Else
              Display "DNET not specified correctly"
            End If
          Case 43 ' SNET
            tp = GetNLframe(cp)
            If tp < 0 Then Exit Function
            SBframeNL(tp).Ctrl.SNET = GetControl(ext$)
            SBframeNL(tp).SNET = GetNumber(ext$, 1)
            styp = DecodeNetworkType(CheckValue(ext$, 2))
            If (styp = 0) And (SBframeNL(tp).SNET = -1) Then
              styp = 1 ' set Ethernet as default
            End If
          Case 44 ' SADR
            If styp > 0 Then
              tp = GetNLframe(cp)
              If tp < 0 Then Exit Function
              SBframeNL(tp).Ctrl.SADR = GetControl(ext$)
              SBframeNL(tp).SADR = GetMAC(ext$, styp)
            Else
              Display "SNET not specified correctly"
            End If
          Case 45 ' HOPCNT
            If dtyp > 0 Then
              tp = GetNLframe(cp)
              If tp < 0 Then Exit Function
              SBframeNL(tp).Ctrl.HOPCNT = GetControl(ext$)
              v = GetNumber(ext$, 1)
              If (v >= 0) And (v < 256) Then
                SBframeNL(tp).HOPCNT = Chr$(v)
              Else
                Display "Illegal HopCnt number"
              End If
            Else
              Display "DNET not specified correctly"
            End If
          Case 46 ' MESSAGE
            tp = GetNLframe(cp)
            If tp < 0 Then Exit Function
            SBframeNL(tp).Ctrl.Message = GetControl(ext$)
            SBframeNL(tp).Message = GetMSG(ext$)
            If CheckSign(ext$, ",", 1) > 0 Then
              GetMSGparm tp, ext$
            End If
          Case 47 ' PRIORITY
            tp = GetNLframe(cp)
            If tp < 0 Then Exit Function
            SBframeNL(tp).Ctrl.priority = GetControl(ext$)
            v = GetPriority(ext$)
            If (v >= 0) And (v < 4) Then
              SBframeNL(tp).priority = Chr$(v)
            Else
              Display "Illegal Priority number"
            End If
          Case 48 ' DER
            tp = GetNLframe(cp)
            If tp < 0 Then Exit Function
            SBframeNL(tp).Ctrl.DER = GetControl(ext$)
            SBframeNL(tp).DER = GetBoolean(ext$, 1)
          Case 49 ' VENDORID
            tp = GetNLframe(cp)
            If tp < 0 Then Exit Function
            If Asc(SBframeNL(tp).Ctrl.Message) Then
              SBframeNL(tp).Ctrl.VendorID = GetControl(ext$)
              SBframeNL(tp).VendorID = GetNumber(ext$, 1)
            Else
              Display "Message not defined"
            End If
          Case 50 ' NLDATA
            tp = GetNLframe(cp)
            If tp < 0 Then Exit Function
            SBframeNL(tp).Ctrl.DATA = SBframeNL(tp).Ctrl.DATA & GetControl(ext$)
            l = Len(SBframeNL(tp).DATA)
            v = 1
            Do
              ' first collecting all stuff, then set the control data
              SBframeNL(tp).DATA = SBframeNL(tp).DATA & GetStuff(ext$, v)
              v = v + 1
            Loop Until (Len(CheckValue(ext$, v)) = 0)
            l = Len(SBframeNL(tp).DATA) - l
            SBframeNL(tp).Ctrl.DATA = SBframeNL(tp).Ctrl.DATA & Chr$(Lo(l)) & Chr$(Hi(l))

          ' AL section
          Case 60 ' PDU
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            SBframeAL(tp).Ctrl.pdu = GetControl(ext$)
            v = GetPDU(ext$) ' get the pdu and check its range
            If (v >= 0) And (v < 16) Then
              SBframeAL(tp).pdu = Chr$(v)
            Else
              Display "Illegal PDU-Type number"
            End If
          Case 61 ' PDU-FLAGS
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            SBframeAL(tp).Ctrl.Flags = GetControl(ext$)
            If CheckSign(ext$, ",", 1) = 0 Then
              l = GetNumber(ext$, 1)
            Else
              v = 1
              l = 0
              Do
                l = l * 2 + Asc(GetBoolean(ext$, v))
                v = v + 1
              Loop Until (Len(CheckValue(ext$, v)) = 0)
            End If
            If (l >= 0) And (l < 16) Then
              SBframeAL(tp).Flags = Chr$(l)
            Else
              Display "Illegal PDU flags"
            End If
          Case 62 ' MaxResp
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            If Asc(SBframeAL(tp).Ctrl.pdu) = 0 Then
              SBframeAL(tp).Ctrl.pdu = Chr$(1) ' equal
              SBframeAL(tp).pdu = Chr$(0) ' confirmed-request
            ElseIf Asc(SBframeAL(tp).pdu) <> 0 Then
              Display "PDU mismatch"
            End If
            SBframeAL(tp).Ctrl.MaxResp = GetControl(ext$)
            SBframeAL(tp).MaxResp = GetMaxResp(ext$)
          Case 63 ' InvokeID
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            If Asc(SBframeAL(tp).Ctrl.pdu) = 0 Then
              Display "PDU missing"
            ElseIf Asc(SBframeAL(tp).pdu) = 1 Then
              Display "PDU mismatch"
            Else
              SBframeAL(tp).Ctrl.invokeID = GetControl(ext$)
              v = GetNumber(ext$, 1)
              If (v >= 0) And (v < 256) Then
                SBframeAL(tp).invokeID = Chr$(v)
              Else
                Display "Illegal InvokeID"
              End If
            End If
          Case 64 ' SEQUENCENR
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            If Asc(SBframeAL(tp).Ctrl.pdu) = 0 Then
              Display "PDU missing"
            ElseIf (Asc(SBframeAL(tp).pdu) <> 0) And (Asc(SBframeAL(tp).pdu) <> 3) And (Asc(SBframeAL(tp).pdu) <> 4) Then
              Display "PDU mismatch"
            Else
              SBframeAL(tp).Ctrl.SequenceNr = GetControl(ext$)
              v = GetNumber(ext$, 1)
              If (v >= 0) And (v < 256) Then
                SBframeAL(tp).SequenceNr = Chr$(v)
              Else
                Display "Illegal sequence nr"
              End If
            End If
          Case 65 ' WINDOWSIZE
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            If Asc(SBframeAL(tp).Ctrl.pdu) = 0 Then
              Display "PDU missing"
            ElseIf (Asc(SBframeAL(tp).pdu) <> 0) And (Asc(SBframeAL(tp).pdu) <> 3) And (Asc(SBframeAL(tp).pdu) <> 4) Then
              Display "PDU mismatch"
            Else
              SBframeAL(tp).Ctrl.WindowSize = GetControl(ext$)
              v = GetNumber(ext$, 1)
              If (v >= 0) And (v < 256) Then ' legal size from 1 to 127
                SBframeAL(tp).WindowSize = Chr$(v)
              Else
                Display "Illegal window size"
              End If
            End If
          Case 69 ' ALDATA
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            SBframeAL(tp).Ctrl.DATA = SBframeAL(tp).Ctrl.DATA & GetControl(ext$)
            l = Len(SBframeAL(tp).DATA)
            v = 1
            Do
              ' first collecting all stuff, then set the control data
              SBframeAL(tp).DATA = SBframeAL(tp).DATA & GetStuff(ext$, v)
              v = v + 1
            Loop Until (Len(CheckValue(ext$, v)) = 0)
            l = Len(SBframeAL(tp).DATA) - l
            SBframeAL(tp).Ctrl.DATA = SBframeAL(tp).Ctrl.DATA & Chr$(Lo(l)) & Chr$(Hi(l))

          Case 70 ' Service
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            If Asc(SBframeAL(tp).Ctrl.pdu) > 0 Then
              v = Asc(SBframeAL(tp).pdu)
            Else
              v = 0 ' confirmed servive
            End If
            SBframeAL(tp).Ctrl.Service = GetControl(ext$)
            Service = GetService(ext$, v)
            If (v >= 0) Then ' function returned no error ?
              SBframeAL(tp).Service = Chr$(Service)
              If Asc(SBframeAL(tp).Ctrl.pdu) = 0 Then
                SBframeAL(tp).Ctrl.pdu = Chr$(1) ' equal
                SBframeAL(tp).pdu = Chr$(v) ' confirmed-request
              End If
            Else
              Display "PDU and service mismatch"
            End If
          Case 71 ' REJECT-REASON
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            If Asc(SBframeAL(tp).Ctrl.pdu) = 0 Then
              SBframeAL(tp).Ctrl.pdu = Chr$(1) ' equal
              SBframeAL(tp).pdu = Chr$(6) ' reject-pdu
            ElseIf Asc(SBframeAL(tp).pdu) <> 6 Then
              Display "PDU mismatch"
            End If
            SBframeAL(tp).Ctrl.Service = GetControl(ext$)
            Service = GetRejectReason(ext$)
            SBframeAL(tp).Service = Chr$(Service)
          Case 72 ' ABORT-REASON
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            If Asc(SBframeAL(tp).Ctrl.pdu) = 0 Then
              SBframeAL(tp).Ctrl.pdu = Chr$(1) ' equal
              SBframeAL(tp).pdu = Chr$(7) ' abort-pdu
            ElseIf Asc(SBframeAL(tp).pdu) <> 7 Then
              Display "PDU mismatch"
            End If
            SBframeAL(tp).Ctrl.Service = GetControl(ext$)
            Service = GetAbortReason(ext$)
            SBframeAL(tp).Service = Chr$(Service)

          Case 80 To 120 ' AL-Service related stuff
            tp = GetALframe(cp)
            If tp < 0 Then Exit Function
            If Service >= 0 Then
              GetALrelStuff tp, key - 80, ext$
            Else
              Display "PDU/Service missing"
            End If
            
          ' Anything else
          Case Else
            Display "Unknown packet keyword"
        End Select

      End If ' If Len(s$) > 0 Then

      ' Get next String or finish
      If e Then
        e = ReadLine(s$)
      Else
        s$ = ")"
      End If
    End If ' If e Then
  Loop ' Do While

  ' checks for the only requiered parameter
  If (net = 0) Then
    Display "NETWORK not specified correctly"
  End If

  If (s$ <> ")") Then
    Display "Incomplete packet description"
  Else
    ParsePacket = cp
  End If
End Function

Function ParseScript(ByVal mode%) As Integer
' mode = -1  Reread the Setup Section
' mode =  0  Normal Parse
' mode = +1  Reparse without the Setup Section
Dim key As Integer
Dim pState As Integer
Dim s$, e$, t%, cslevel%

  ' show that we're thinking
  Screen.MousePointer = HourGlass

  ' Disable error breaks
  On Error Resume Next

  ' Default return is error
  ParseScript = 0

  ' Open file and parse
  If Len(DefScriptFile) > 0 Then
    Open DefScriptFile For Input As #1 Len = 4096
    If Err = 0 Then
      ' Enable Error Breaks
      On Error GoTo 0

      If (mode = 0) Then
        ' Show window
        SBparse.Show
        SBparse.Refresh
        SBparse.ScriptFile.Caption = DefScriptFile
      Else
        SBSetup.InfoList.Clear
      End If

      ' Initiate varibales
      SBTForm.SectionList.Clear
      ClearGlobalVars (mode <> 1)
      ErrCnt = 0
      LineNo = 0
      pState = 0

      Do While ReadLine(s$)
        GetExpression s$, key, e$
        DoEvents
        If (gCancelParse <> 0) Then
          Display "Parsing cancelled!"
          SelectFirst SBparse         '                                     ***222
          If (gCancelParse = 2) Then
            ParseScript = -1 'show results
          Else
            ParseScript = 0 'don't show results
          End If
          Close #1
          Screen.MousePointer = Normal
          Exit Function
        End If

        ' Update status information
        Display ""

        Select Case key
          Case 0 ' <UNKNOWN>
            If (mode <> 1) Then ' Reparse with knew idents
              If pState = 128 Then
                If SearchNata(s$) < 0 Then
                  If EnterNata(s$, e$) < 0 Then
                    Display "Nata full"
                  End If
                Else
                  Display "Redfined identifier"
                End If
              Else
                Display "Unknown keyword"
              End If
            End If

          Case 1 ' SETUP
            If (mode <> 1) Then
              If (pState = 0) And (Len(e$) > 0) Then
                SBTForm.Caption = e$
              Else
                SBTForm.Caption = "Script Based Testing"
              End If
              pState = 128
            End If

          Case 2 ' SECTION
            If (mode <> -1) Then ' Reread identifier
              If Len(e$) > 0 Then
                pState = 64
                AddNewSection e$
              Else
                Display "Illegal section header"
              End If
            End If

          Case 7, 8 ' Open and close bracket
            If (mode <> -1) Then
              Display "Illegal open/closed brackets"
            End If

          Case 9 ' Comment
            If pState = 128 Then ' SETUP
              If (mode <> 1) Then
                AddNataComment CurrentNata(), e$
              End If
            ElseIf pState = 96 Then ' TEST
              If (mode <> -1) Then
                AddTestComment CurrentTest(), e$
              End If
            End If

          Case 10 ' TEST
            If (mode <> -1) Then
              If (pState And 64) Then
                If Len(e$) > 0 Then
                  If AddNewTest(e$) >= 0 Then
                    cslevel = 0
                    pState = 96
                  Else
                    Display "Test table full"
                  End If
                Else
                  Display "Illegal test header"
                End If
              Else
                Display "Missing section header"
              End If
            End If

          Case 11, 12 ' DEPENCIES, REFERENCE
            If (mode <> -1) Then
              If (pState And 32) Then
                If Len(e$) > 0 Then
                  If key = 11 Then
                    AddTestDeps CurrentTest(), e$
                  Else
                    AddTestRefs CurrentTest(), e$
                  End If
                Else
                  Display "Illegal deps/refs extension"
                End If
              Else
                Display "Missing test header"
              End If
            End If

          Case 13 To 17 ' SEND ...
            If (mode <> -1) Then
              If (pState And 32) Then
                t% = ParsePacket(key - 13, cslevel, e$)
                If t% >= 0 Then
                  ' remove case start marker
                  cslevel = cslevel And 127
                  If SBTest(CurrentTest()).FirstPacket < 0 Then
                    SBTest(CurrentTest()).FirstPacket = t
                  End If
                Else
                  Display "Packet table full"
                End If
              Else
                Display "Missing test header"
              End If
            End If

          Case 20 ' CASE
            If (mode <> -1) Then
              If (pState And 32) Then
                ' Set case level to e$
                If SetCaseLevel(cslevel, e$) Then
                  ' mark begin of a new case
                  cslevel = cslevel Or 128
                Else
                  Display "Illegal case level"
                End If
              Else
                Display "Missing test header"
              End If
            End If

          Case 21 ' ENDCASE
            If (mode <> -1) Then
              If (pState And 32) Then
                If (cslevel > 0) And (cslevel < 16) Then
                  cslevel = cslevel - 1
                Else
                  Display "Illegal endcase"
                End If
              Else
                Display "Missing test header"
              End If
            End If

          Case Else ' all other
            If (mode <> -1) Then
              Display "Illegal keyword"
            End If
        End Select
      Loop ' Do While ReadLine

      ' Close the file
      Close #1

      ' Hide the window, if everything is okay
      If ErrCnt = 0 Then
        If (mode = 0) Then Unload SBparse
        ParseScript = 1
      Else
        ' otherwise show the trouble we've made
        If (mode = 0) Then SelectFirst SBparse.StatusList       '               ***222
        ParseScript = -1
      End If
    End If
  End If

  ' show that we're finished
  Screen.MousePointer = Normal
End Function

Private Function ReadLine(s$) As Integer
  Do
    If Not EOF(1) Then
      Line Input #1, s$
      LineNo = LineNo + 1
      s$ = tabtrim(s$)  ' MAG 07 APR 2001
      s$ = Trim(s$)
    End If
  Loop Until EOF(1) Or (Len(s$) > 0) And (Left$(s$, 1) <> ";")

  ReadLine = Not EOF(1)
End Function

Function tabtrim(s$) As String  ' Added by MAG 07 APR 2001 as fix for extra tabs problem (extraneous database entries)
Dim i As Integer
Dim s2 As String

i = 1
Do While (i <= Len(s$))
    If (Mid(s$, i, 1) = Chr(9)) Then
        s2$ = s2$ + Chr(32) ' 9 is tab, 32 is space MAG 07 APR 2001
    Else
        s2$ = s2$ + Mid(s$, i, 1)
    End If
    i = i + 1
Loop
tabtrim$ = s2$
End Function
'This function is used to extract the values of all identifiers
'in an expression string. The expression is assumed not to have any
'comments.
'in     expr: expression string who contains identifiers
'out    expr: expression with the identifier values
Function ReplaceIdent(Expr As String) As String
Dim pos As Integer, ident As String, NoMoreIdent As Integer
Dim e As String, l As Integer, r As Integer

NoMoreIdent = False 'assume there are identifiers
e = ""
l = 1 'left bound of identifier
r = 1 'right bound of identifier
Expr = Expr & "," 'mark the end of expression

Do
  r = InStr(l, Expr, ",", 1)
  'extract ident
  If (l >= r) Then
    NoMoreIdent = True
  Else
    ident = Trim$(Mid$(Expr, l, r - l))
    l = r + 1 'new left bound
    pos = SearchNata(ident$)
    If (pos >= 0) Then
      ident$ = RTrim$(SBNata(pos).Value)
    End If
    e = e & "," & ident
  End If

Loop Until NoMoreIdent
ReplaceIdent = Mid$(e, 2) 'expressions and extracted identifiers

End Function

Sub SBReceiveIndicate(ByVal RxT As Integer, n As NALSpacket, ByVal apdu$)
Dim recCount%

  ' If no test is in progress then exit without collecting
  If TestInProgress = -1 Then Exit Sub

  ' collecting and counting the stuff
  recCount = UBound(SBrecList)
  ReDim Preserve SBrecList(recCount + 1)
  If n.nalsExpectReply > 1 Then  ' MAG 05 FEB 01 fix for overflow error- variable sent here unitialized.
    n.nalsExpectReply = 1
  End If

  SBrecList(recCount).RxType = Chr$(RxT)
  SBrecList(recCount).Checked = Chr$(0) ' False
  SBrecList(recCount).Port = Asc(n.nalsLport)
  SBrecList(recCount).ExpectReply = n.nalsExpectReply
  SBrecList(recCount).Broadcast = n.nalsBroadcast
  SBrecList(recCount).invokeID = n.nalsInvokeID
  SBrecList(recCount).Pri = n.nalsPri
  SBrecList(recCount).DNET = n.nalsDnet
  SBrecList(recCount).DADR = n.nalsDlen & n.nalsDadr
  SBrecList(recCount).SNET = n.nalsSnet
  SBrecList(recCount).SADR = n.nalsSlen & n.nalsSadr
  SBrecList(recCount).RADR = n.nalsRlen & n.nalsRadr
  SBrecList(recCount).DATA = apdu$

End Sub

Private Function SearchNata(s$) As Integer
Dim ni As Integer

  ni = UBound(SBNata) - 1
  Do While (ni >= 0)
    If (s$ = SBNata(ni).Name) Then Exit Do
    ni = ni - 1
  Loop

  SearchNata = ni
End Function

Private Function SetCaseLevel(cslevel%, e$) As Integer
Dim i%

   i = 0
   cslevel = 0
   Do
     cslevel = cslevel + 1
     i = CheckSign(e$, ".", i + 1)
   Loop Until (i = 0)

   SetCaseLevel = (i < 16)
End Function

