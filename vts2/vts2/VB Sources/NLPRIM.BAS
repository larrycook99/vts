Attribute VB_Name = "NLPRIM"
DefInt A-Z
Option Explicit

Sub SendNL()
Dim i, j, p, ul, u$
  p = Val(Left$(TargetForm.Port.Text, 3))
  NLpkt.maLport = Chr$(p)                       'get port number
  NLpkt.maLsap = Chr$(BACnetLSAP)
  ul = NALSAPIgetlen(p)
  NLpkt.maAlen = Chr$(ul)                       'get used length for port
  If ul > 1 Then u$ = " octets" Else u$ = " octet"
  If TargetForm.GlobalRoute.Value Then          'this is a global broadcast
    NLpkt.maBroadcast = Chr$(1)
  ElseIf TargetForm.RemoteRoute.Value Then      'this one is routed
    If TargetForm.RouterUnknown.Value Then
      NLpkt.maBroadcast = Chr$(1)               'broadcast it
    Else
      NLpkt.maBroadcast = Chr$(0)               'don't broadcast
      j = Len(TargetForm.RouterMACAddress.Text)
      If j > (ul * 2) Then
        Beep
        MsgBox "The port [" + TargetForm.Port.Text + "] uses MAC addresses which are" + Str$(ul) + u$ + " long. You must use a shorter MAC address!", 48, "MAC Address Too Long"
        TargetForm.RouterMACAddress.SetFocus
        Exit Sub
      End If
      If j And 1 Then
        u$ = "0" + TargetForm.RouterMACAddress.Text
        j = j + 1
      Else
        u$ = TargetForm.RouterMACAddress.Text
      End If
    End If
  ElseIf TargetForm.BroadcastDest.Value Then    'it's a local broadcast
    NLpkt.maBroadcast = Chr$(1)                 'broadcast it
  Else                                          'it's just local
      NLpkt.maBroadcast = Chr$(0)               'don't broadcast
      j = Len(TargetForm.DestMACAddress.Text)
      If j > (ul * 2) Then
        Beep
        MsgBox "The port [" + TargetForm.Port.Text + "] uses MAC addresses which are" + Str$(ul) + u$ + " long. You must use a shorter MAC address!", 48, "MAC Address Too Long"
        TargetForm.DestMACAddress.SetFocus
        Exit Sub
      End If
      If j And 1 Then
        u$ = "0" + TargetForm.DestMACAddress.Text
        j = j + 1
      Else
        u$ = TargetForm.DestMACAddress.Text
      End If
  End If
  If NLpkt.maBroadcast = Chr$(0) Then
    For i = 1 To j / 2
      Mid$(NLpkt.maDadr, i, 1) = Chr$(Val("&h" + Mid$(u$, 1 + ((i - 1) * 2), 2)))
    Next i
  End If
  SendNLpkt
End Sub

Sub SendNLpkt()
Dim i
  Screen.MousePointer = HourGlass
  i = NALSAPItransmit(NLpkt)                    'try to send it
  Screen.MousePointer = Normal
  If i Then                                     'an error occurred
    MsgBox "Transmit failed due to error" + Str$(i), 48, "Transmit Error"
  End If
End Sub

'in:    port            the port number to send on
'       priority        0..3
'       dnet            0=local, FFFF=global, 1-FFFE=route to that network
'       dadr$           hex string DADR, e.g. "112233445566" or "broadcast" or "" (same as broadcast)
'                       if dnet=local   dadr$ is local destination
'                       if dnet=global  dadr$ is ignored
'                       if dnet=route   dadr$ is ultimate DADR
'       radr$           (only used if dnet=route)
'                       hex string router address, e.g. "112233445566" or "broadcast" or "" (same as broadcast)
'       snet            0=don't include source routing info
'                       1..FFFE=this is SNET
'       sadr$           (only used if snet>0), hex string SADR          use "" if snet=0
'       np              pointer into NLpkt
'       nlmsg           network layer message type
Sub SetupNL(Port, priority, DNET, DADR$, RADR$, SNET, SADR$, np, nlmsg)
Dim i, maxn, nsdu, ul, d$
  NLpkt.maNlen = 0
  np = 1
  StuffByte np, BACnetVersion
  nsdu = NSDUisNLmessage + priority
  StuffByte np, nsdu                            'we'll update this later
  maxn = NALSAPImaxNPDUlength(Port)              'max length for this port
  If LCase$(DADR$) = "broadcast" Then DADR$ = ""
  If LCase$(RADR$) = "broadcast" Then RADR$ = ""
  If DNET <> 0 Then                             'need to add DNET stuff
    nsdu = nsdu Or NSDUhasDNET
    StuffWord np, DNET
    If (DNET <> &HFFFF) And (DADR$ <> "") Then  'specific remote destination
      If Len(DADR$) And 1 Then DADR$ = "0" + DADR$
      StuffByte np, Len(DADR$) / 2              'the DLEN
      For i = 1 To Len(DADR$) / 2               'the DADR
       StuffByte np, Val("&h" + Mid$(DADR$, 1 + ((i - 1) * 2), 2))
      Next i
    Else                                        'no DADR needed, DLEN=0
      StuffByte np, 0                           'the DLEN=0
    End If
  End If
  If SNET Then                                  'we need SNET, SLEN, SADR
    nsdu = nsdu Or NSDUhasSNET
    StuffWord np, SNET                          'the SNET
    If Len(SADR$) And 1 Then SADR$ = "0" + SADR$
    StuffByte np, Len(SADR$) / 2                'the SLEN
    For i = 1 To Len(SADR$) / 2                 'the SADR
      StuffByte np, Val("&h" + Mid$(SADR$, 1 + ((i - 1) * 2), 2))
    Next i
  End If
  If nsdu And NSDUhasDNET Then StuffByte np, 255 'init HopCount
  StuffByte np, nlmsg                           'network layer message type

  NLpkt.maLport = Chr$(Port)                    'get port number
  NLpkt.maLsap = Chr$(BACnetLSAP)
  ul = NALSAPIgetlen(Port)
  NLpkt.maAlen = Chr$(ul)                       'get used length for port
  If DNET = &HFFFF Then                         'this is a global broadcast
    NLpkt.maBroadcast = Chr$(1)
  ElseIf DNET <> 0 Then                         'this one is routed
    If RADR$ = "" Then                          'router is unknown
      NLpkt.maBroadcast = Chr$(1)               'broadcast it
    Else
      NLpkt.maBroadcast = Chr$(0)               'don't broadcast
      d$ = RADR$
    End If
  ElseIf TargetForm.BroadcastDest.Value Then    'it's a local broadcast
    NLpkt.maBroadcast = Chr$(1)                 'broadcast it
  Else                                          'it's just local
    NLpkt.maBroadcast = Chr$(0)                 'don't broadcast
    d$ = DADR$
  End If
  If NLpkt.maBroadcast = Chr$(0) Then
    If Len(d$) > (ul * 2) Then
      Beep
      MsgBox "Port" + Str$(Port) + " uses MAC addresses which are" + Str$(ul) + " octet(s) long. You must use a shorter MAC address!", 48, "MAC Address Too Long"
      Exit Sub
    End If
    If Len(d$) And 1 Then d$ = "0" + d$
    For i = 1 To Len(d$) / 2
      Mid$(NLpkt.maDadr, i, 1) = Chr$(Val("&h" + Mid$(d$, 1 + ((i - 1) * 2), 2)))
    Next i
  End If
End Sub

'stuff a byte into NLpkt.maNPDU at character offset atoffset (1-based)
Sub StuffByte(atoffset, ByVal b)
  Mid$(NLpkt.maNPDU, atoffset, 1) = Chr$(b)
  NLpkt.maNlen = NLpkt.maNlen + 1
  atoffset = atoffset + 1
End Sub

'stuff a big endian word into NLpkt.maNPDU at character offset atoffset (1-based)
Sub StuffWord(atoffset, ByVal word&)
  Mid$(NLpkt.maNPDU, atoffset, 2) = Chr$((word And &HFFFF&) \ 256&) + Chr$(word And 255)
  NLpkt.maNlen = NLpkt.maNlen + 2
  atoffset = atoffset + 2
End Sub

