VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Begin VB.Form VTSform 
   Appearance      =   0  'Flat
   BackColor       =   &H00808080&
   Caption         =   "Visual Test Shell"
   ClientHeight    =   4995
   ClientLeft      =   2505
   ClientTop       =   2550
   ClientWidth     =   5310
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   8.25
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H80000008&
   Icon            =   "VTS.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4995
   ScaleWidth      =   5310
   WhatsThisButton =   -1  'True
   WhatsThisHelp   =   -1  'True
   Begin VB.ListBox StatusList 
      BackColor       =   &H00808080&
      ForeColor       =   &H00FFFFFF&
      Height          =   300
      IntegralHeight  =   0   'False
      Left            =   1830
      TabIndex        =   6
      Top             =   0
      Visible         =   0   'False
      Width           =   3495
   End
   Begin VB.Timer Timer1 
      Interval        =   5000
      Left            =   120
      Top             =   4440
   End
   Begin MSComDlg.CommonDialog CMD 
      Left            =   210
      Top             =   3675
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327681
      CancelError     =   -1  'True
   End
   Begin VB.Image VTS16 
      BorderStyle     =   1  'Fixed Single
      Height          =   1860
      Left            =   0
      Picture         =   "VTS.frx":030A
      Top             =   0
      Visible         =   0   'False
      Width           =   1860
   End
   Begin VB.Label VTSLabel 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Visual Test Shell"
      BeginProperty Font 
         Name            =   "Times New Roman"
         Size            =   36
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   2475
      Index           =   0
      Left            =   2835
      TabIndex        =   4
      Top             =   210
      Visible         =   0   'False
      Width           =   2010
      WordWrap        =   -1  'True
   End
   Begin VB.Label CopyrightLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "©1995-2000, National Institute of Standards and Technology. Portions of this software were developed for NIST by PolarSoft® Inc."
      BeginProperty Font 
         Name            =   "Times New Roman"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   855
      Index           =   0
      Left            =   2310
      TabIndex        =   1
      Top             =   3030
      Width           =   2745
   End
   Begin VB.Label VersionLabel 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "v0.00"
      BeginProperty Font 
         Name            =   "Times New Roman"
         Size            =   12
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   300
      Index           =   0
      Left            =   3450
      TabIndex        =   0
      Top             =   2670
      Width           =   615
   End
   Begin VB.Label CopyrightLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "©1995-2000, National Institute of Standards and Technology. Portions of this software were developed for NIST by PolarSoft® Inc."
      BeginProperty Font 
         Name            =   "Times New Roman"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00000000&
      Height          =   855
      Index           =   1
      Left            =   2340
      TabIndex        =   3
      Top             =   3060
      Width           =   2745
   End
   Begin VB.Label VersionLabel 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "v0.00"
      BeginProperty Font 
         Name            =   "Times New Roman"
         Size            =   12
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00000000&
      Height          =   300
      Index           =   1
      Left            =   3480
      TabIndex        =   2
      Top             =   2700
      Width           =   615
   End
   Begin VB.Label VTSLabel 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Visual Test Shell"
      BeginProperty Font 
         Name            =   "Times New Roman"
         Size            =   36
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00000000&
      Height          =   2475
      Index           =   1
      Left            =   2895
      TabIndex        =   5
      Top             =   270
      Visible         =   0   'False
      Width           =   2010
      WordWrap        =   -1  'True
   End
   Begin VB.Image VTS256 
      BorderStyle     =   1  'Fixed Single
      Height          =   5310
      Left            =   3675
      Picture         =   "VTS.frx":1FAC
      Top             =   3780
      Visible         =   0   'False
      Width           =   5310
   End
   Begin VB.Menu FileMenu 
      Caption         =   "&File"
      Begin VB.Menu FileOpen 
         Caption         =   "&Open PDU file..."
      End
      Begin VB.Menu FileSave 
         Caption         =   "&Save PDU File"
         Begin VB.Menu FileSaveRx 
            Caption         =   "save &Received PDUs..."
         End
         Begin VB.Menu FileSaveTx 
            Caption         =   "save &Transmitted PDUs..."
         End
      End
      Begin VB.Menu FileSep 
         Caption         =   "-"
      End
      Begin VB.Menu FileExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu EditMenu 
      Caption         =   "&Edit"
      Enabled         =   0   'False
      Begin VB.Menu EditCut 
         Caption         =   "Cu&t"
      End
      Begin VB.Menu EditCopy 
         Caption         =   "&Copy"
      End
      Begin VB.Menu EditPaste 
         Caption         =   "&Paste"
      End
      Begin VB.Menu EditDel 
         Caption         =   "Delete"
      End
   End
   Begin VB.Menu ViewMenu 
      Caption         =   "&View"
      Begin VB.Menu ViewTargetDevice 
         Caption         =   "Target &Device Window"
      End
      Begin VB.Menu ViewSep0 
         Caption         =   "-"
      End
      Begin VB.Menu ViewTrace 
         Caption         =   "Trace &Window"
      End
      Begin VB.Menu ViewTransmit 
         Caption         =   "&Transmitted Frames"
         Checked         =   -1  'True
      End
      Begin VB.Menu ViewClearTx 
         Caption         =   "&0 Clear Transmit Window"
      End
      Begin VB.Menu ViewReceive 
         Caption         =   "&Received Frames"
         Checked         =   -1  'True
      End
      Begin VB.Menu ViewClearRx 
         Caption         =   "&1 Clear Receive Window"
      End
      Begin VB.Menu ViewSep2 
         Caption         =   "-"
      End
      Begin VB.Menu ViewNetworkLayer 
         Caption         =   "&Network Layer Info"
         Checked         =   -1  'True
      End
      Begin VB.Menu ViewHexadecimal 
         Caption         =   "&Hexadecimal"
         Checked         =   -1  'True
      End
      Begin VB.Menu ViewVerbose 
         Caption         =   "&Verbose"
         Checked         =   -1  'True
      End
   End
   Begin VB.Menu ServiceMenu 
      Caption         =   "&AL"
      Begin VB.Menu ServiceAcknowledgeAlarm 
         Caption         =   "Acknowledge Alarm"
      End
      Begin VB.Menu ServiceCOV 
         Caption         =   "COV Notification/Subscription"
      End
      Begin VB.Menu ServiceEventNotification 
         Caption         =   "Event Notification"
      End
      Begin VB.Menu ServiceGetAlarmSummary 
         Caption         =   "Get Alarm Summary"
      End
      Begin VB.Menu ServiceGetEnrollmentSummary 
         Caption         =   "Get Enrollment Summary"
      End
      Begin VB.Menu ServiceAtomicFile 
         Caption         =   "Atomic Read/Write File"
      End
      Begin VB.Menu ServiceListElement 
         Caption         =   "Add/Remove List Element"
      End
      Begin VB.Menu ServiceCreateDeleteObject 
         Caption         =   "Create/Delete Object"
      End
      Begin VB.Menu ServiceReadWriteProperty 
         Caption         =   "Read/Write Property"
      End
      Begin VB.Menu ServiceReadPropertyConditional 
         Caption         =   "Read Property Conditional"
      End
      Begin VB.Menu ServiceDeviceControl 
         Caption         =   "Device Communication Control"
      End
      Begin VB.Menu ServicePrivateTransfer 
         Caption         =   "Private Transfer"
      End
      Begin VB.Menu ServiceTextMessage 
         Caption         =   "Text Message"
      End
      Begin VB.Menu ServiceReinitializeDevice 
         Caption         =   "Reinitialize Device"
      End
      Begin VB.Menu ServiceVirtualTerminal 
         Caption         =   "Virtual Terminal"
      End
      Begin VB.Menu ServiceSecurity 
         Caption         =   "Authenticate/Request Key"
         Enabled         =   0   'False
      End
      Begin VB.Menu ServiceWho 
         Caption         =   "Who-Is, I-Am, Who-Has, I-Have"
      End
      Begin VB.Menu ServiceTimeSynchronization 
         Caption         =   "Time Synchronization"
      End
      Begin VB.Menu AckPDU 
         Caption         =   "Simple Ack, Error PDU"
      End
   End
   Begin VB.Menu NLMenu 
      Caption         =   "&NL"
      Begin VB.Menu rawNPDU 
         Caption         =   "Raw NPDU"
      End
      Begin VB.Menu WhoIsRouter 
         Caption         =   "Who Is Router To Network"
      End
      Begin VB.Menu IAmRouter 
         Caption         =   "I Am Router To Network"
      End
      Begin VB.Menu ICouldBeRouter 
         Caption         =   "I Could Be Router To Network"
      End
      Begin VB.Menu RejectMessage 
         Caption         =   "Reject Message To Network"
      End
      Begin VB.Menu RouterBusy 
         Caption         =   "Router Busy To Network"
      End
      Begin VB.Menu RouterAvailable 
         Caption         =   "Router Available To Network"
      End
      Begin VB.Menu InitRoutingTable 
         Caption         =   "Initialize Routing Table"
      End
      Begin VB.Menu InitRoutingTableAck 
         Caption         =   "Initialize Routing Table Ack"
      End
      Begin VB.Menu EstablishConnection 
         Caption         =   "Establish Connection To Network"
      End
      Begin VB.Menu DisconnectConnection 
         Caption         =   "Disconnect Connection To Network"
      End
   End
   Begin VB.Menu PTPMenu 
      Caption         =   "&PTP"
      Enabled         =   0   'False
      Begin VB.Menu PTPConnectPeer 
         Caption         =   "&Connect to PTP Peer"
      End
      Begin VB.Menu PTPDisconnectPeer 
         Caption         =   "&Disconnect from PTP Peer"
      End
   End
   Begin VB.Menu TestMenu 
      Caption         =   "&Test"
      Begin VB.Menu SBParseMenu 
         Caption         =   "&Select Script"
      End
      Begin VB.Menu TestSBTests 
         Caption         =   "Script &Based Test"
      End
      Begin VB.Menu TestSep1 
         Caption         =   "-"
      End
      Begin VB.Menu PicsExploreMenu 
         Caption         =   "PICS &Explore..."
         Begin VB.Menu PicsSetupMenu 
            Caption         =   "&Read PICS"
         End
         Begin VB.Menu TestSep2 
            Caption         =   "-"
         End
         Begin VB.Menu PicsObjDBMenu 
            Caption         =   "&Object Database"
            Enabled         =   0   'False
         End
         Begin VB.Menu PicsPInfoMenu 
            Caption         =   "&Property Info"
            Enabled         =   0   'False
         End
         Begin VB.Menu PicsMiscMenu 
            Caption         =   "&Miscellaneous"
            Enabled         =   0   'False
         End
      End
      Begin VB.Menu PicsTestMenu 
         Caption         =   "PICS &Tests..."
         Enabled         =   0   'False
         Begin VB.Menu ConsMenu 
            Caption         =   "&Check Consistancy"
         End
         Begin VB.Menu RdPropMenu 
            Caption         =   "&ReadProperty Test"
         End
         Begin VB.Menu WrtPropMenu 
            Caption         =   "&WriteProperty Test"
         End
         Begin VB.Menu ListElemMenu 
            Caption         =   "&Add/RemoveListElement Test"
         End
      End
      Begin VB.Menu TestSep3 
         Caption         =   "-"
      End
      Begin VB.Menu PicsGlobalMenu 
         Caption         =   "Set&up Tests"
      End
      Begin VB.Menu ScanLogMenu 
         Caption         =   "Scan &Logfile"
      End
   End
   Begin VB.Menu HelpMenu 
      Caption         =   "&Help"
      Begin VB.Menu HelpTopics 
         Caption         =   "&Help Topics for Visual Test Shell"
      End
      Begin VB.Menu HelpSep 
         Caption         =   "-"
      End
      Begin VB.Menu HelpAbout 
         Caption         =   "&About Visual Test Shell"
      End
   End
End
Attribute VB_Name = "VTSform"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim VTS256Color

Private Sub AckPDU_Click()
  ShowBiModal sackForm
End Sub

Private Sub ConsMenu_Click()
  If gDB.DataBase = 0 Then  'database is empty
    Dim e As Integer
    e = PicsError(eNODATABASE, "")
    Exit Sub
  End If

  gState.TestNr = cConsTest           'perform consistency test
  gState.SubTestNr = 0                'initial
  gState.ActiveObject = gDB.DataBase  'first object

   PicsTestForm.Show 0 'show modeless
End Sub

Private Sub DisconnectConnection_Click()
  ShowNL Disconnect_Connection_To_Network, "Disconnect_Connection_To_Network"
End Sub

Private Sub EstablishConnection_Click()
  ShowNL Establish_Connection_To_Network, "Establish_Connection_To_Network"
End Sub

Private Sub FileExit_Click()
  ExitVTS
End Sub

Private Sub FileOpen_Click()
Dim f$
  On Error GoTo foerr
  CMD.DialogTitle = "Open PDU Data File"
  CMD.Filter = "Text Data|*.HEX"
  CMD.Flags = OFN_FILEMUSTEXIST Or OFN_PATHMUSTEXIST
  CMD.Action = 1                        'file open
  f$ = CMD.FileName
  If f$ = "" Then Exit Sub
  ReadAPDUs f$

foerr:
  Exit Sub
End Sub

Private Sub FileSaveRx_Click()
  SavePDUs "Received PDUs", TraceForm.RxHex, TraceForm.RxList
End Sub

Private Sub FileSaveTx_Click()
  SavePDUs "Transmitted PDUs", TraceForm.TxHex, TraceForm.TxList
End Sub

Private Sub Form_Load()
  KeyPreview = True                             'so we can get F1             ***223
  If GetDeviceCaps(hdc, 12) < 8 Then
    VTS256Color = False
  Else
    VTS256Color = True
  End If
  VersionLabel(0) = vtsversion
  VersionLabel(1) = vtsversion
  HelpMenu.Caption = Chr$(8) + "&Help"
  BroadcastDA = Chr$(255) + Chr$(255) + Chr$(255) + Chr$(255) + Chr$(255) + Chr$(255)
  If VTS256Color Then                           'use 256 colors
    VTS256.Stretch = True
    VTS256.Move 0, 0, ScaleWidth, ScaleHeight
    VTS256.visible = True
    AboutForm.AboutImage.Picture = VTS256.Picture
  Else                                          'use 16 colors
    VTS16.visible = True
    VTSLabel(0).visible = True
    VTSLabel(1).visible = True
  End If
  StatusList.Clear
  Status Format$(Now, "dd-mmm-yy hh:nn:ss") + " VTS " + vtsversion + " restarted"
End Sub

'Format a data packet into a display list according
'to the prevailing display format
'in:    dl      the display list to put the result in
'       dp      the data packet
Private Sub Formatpacket(dl As Control, dp As NALSpacket)
Dim c$, b, i, fl, w
  Select Case CurrentDisplayFormat
    Case 0                              'hex
'      dl.AddItem Format$(dp.mpLength) + "-------------------------"
'      dl.AddItem hexn(6,dp.mpDA) + " " + hexn(6,dp.mpSA) + " " + hex4(dp.mpLength)
'      dl.AddItem hex2(dp.mpDSAP) + " " + hex2(dp.mpSSAP) + " " + hex2(dp.mpLLCControl) + " " + hex2(dp.mpVersion) + " " + hex2(dp.mpNPCI)
      w = dl.Width \ TextWidth("00 ")   'number of bytes that will fit on a line formatted
'      fl = (dp.mpLength - 19)           'formatted length of the packet
      i = 1                             'format npdu bytes
      b = 0                             'bytes on the line so far
      c$ = ""
      Do While i < fl
'        c$ = c$ + hex2(Mid$(dp.mpNpdu, i, 1)) + " "
        i = i + 1                       'next index
        b = b + 1
        If b = w Then
          dl.AddItem c$
          b = 0
          c$ = ""
        End If
      Loop
      If b Then dl.AddItem c$
    Case 1                              'one liner
    Case 2                              'verbose
  End Select
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
  ExitVTS
End Sub

Private Sub Form_Resize()
  If ScaleWidth >= StatusList.Left Then                         '***220 Begin
    StatusList.Width = ScaleWidth - StatusList.Left
  Else
    StatusList.Width = 0
  End If
  If ScaleWidth >= StatusList.Top Then
    StatusList.Height = ScaleHeight - StatusList.Top
  Else
    StatusList.Height = 0
  End If                                                        '***220 End
End Sub

Private Sub HelpAbout_Click()
  AboutForm.Show modal
End Sub

Private Sub HelpTopics_Click()
  HtmlHelp 0, VTSdir + "vtshelp.chm", 0, 0      '                ***223
End Sub

Private Sub IAmRouter_Click()
  ShowNL I_Am_Router_To_Network, "I_Am_Router_To_Network"
End Sub

Private Sub ICouldBeRouter_Click()
  ShowNL I_Could_Be_Router_To_Network, "I_Could_Be_Router_To_Network"
End Sub

Private Sub InitRoutingTable_Click()
  ShowNL Initialize_Routing_Table, "Initialize_Routing_Table"
End Sub

Private Sub InitRoutingTableAck_Click()
  ShowNL Initialize_Routing_Table_Ack, "Initialize_Routing_Table_Ack"
End Sub

Private Sub ListElemMenu_Click()
Dim ALE As Integer, RLE As Integer

If gDB.DataBase = 0 Then  'database is empty
  Dim e As Integer
  e = PicsError(eNODATABASE, "")
  Exit Sub
End If

ALE = Asc(Mid$(gDB.BACnetStandardServices, 9, 1)) > 0 'AddListElement Service supported?
RLE = Asc(Mid$(gDB.BACnetStandardServices, 10, 1)) > 0 'RemoveListElement Service supported?

If (gDB.BACnetConformanceClass = 4) Or (ALE And RLE) Then

  gState.TestNr = cListElementTest    'perform Add/Remove List Element test
  gState.SubTestNr = 0                'initial
  gState.ActiveObject = gDB.DataBase  'first object

  ShowBiModal PicsTestForm            'show PicsTestForm AND TargetForm
  TargetForm.ZOrder 0
Else
  e = PicsError(eLISTSERVICE, "")
  Exit Sub
End If
End Sub

Private Sub PicsGlobalMenu_Click()
  PicsGlobalForm.Show 0 'show modeless
End Sub

Private Sub PicsMiscMenu_Click()
  PicsMiscForm.Show 'show modeless
End Sub

Private Sub PicsObjDBMenu_Click()
  PicsObjDBForm.Show 'show modeless
End Sub

Private Sub PicsPInfoMenu_Click()
  PicsPropInfoForm.Show 'show modeless
End Sub

Private Sub PicsSetupMenu_Click()
  PicsSetupForm.Show modal 'show modal
End Sub

Private Sub PTPConnectPeer_Click()
  PTPForm.DisconnectFrame.visible = False
  PTPForm.ConnectFrame.visible = True
  PTPForm.Show modal
  If DevDo Then ptpConnect PTPForm.ConnectionPassword.Text
End Sub

Private Sub PTPDisconnectPeer_Click()
  PTPForm.ConnectFrame.visible = False
  PTPForm.DisconnectFrame.visible = True
  PTPForm.Show modal
  If DevDo Then ptpDisconnect PTPForm.DisconnectReason.ListIndex
End Sub

Private Sub RawNPDU_Click()
  ShowBiModal rawForm
End Sub

Private Sub RdPropMenu_Click()
  If gDB.DataBase = 0 Then  'database is empty
    Dim e As Integer
    e = PicsError(eNODATABASE, "")
    Exit Sub
  End If

  gState.TestNr = cRdPropTest         'perform read property test
  gState.SubTestNr = 0                'initial
  gState.ActiveObject = gDB.DataBase  'first object

  ShowBiModal PicsTestForm            'show PicsTestForm AND TargetForm
  TargetForm.ZOrder 0
End Sub

Private Sub RejectMessage_Click()
  ShowNL Reject_Message_To_Network, "Reject_Message_To_Network"
End Sub

Private Sub RouterAvailable_Click()
  ShowNL Router_Available_To_Network, "Router_Available_To_Network"
End Sub

Private Sub RouterBusy_Click()
  ShowNL Router_Busy_To_Network, "Router_Busy_To_Network"
End Sub

Private Sub SBParseMenu_Click()
  SBReadForm.Show 1 'show modal
End Sub

Private Sub ScanLogMenu_Click()
Dim e As Integer

  If (Trim$(gDefPicsLogFile) = "") Then
    e = PicsError(eNOLOGFILE, "")
    Exit Sub
  End If
  logScanFile gDefPicsLogTitle, gDefPicsLogFile
End Sub

Private Sub ServiceAcknowledgeAlarm_Click()
  ShowBiModal AckForm
End Sub

Private Sub ServiceAtomicFile_Click()
  ShowBiModal FileForm
End Sub

Private Sub ServiceCOV_Click()
  ShowBiModal COVForm
End Sub

Private Sub ServiceCreateDeleteObject_Click()
  ShowBiModal ObjForm
End Sub

Private Sub ServiceDeviceControl_Click()
  DevDo = DeviceCommunicationControl
  ShowBiModal DeviceForm
End Sub

Private Sub ServiceEventNotification_Click()
  ShowBiModal EventForm
End Sub

Private Sub ServiceGetAlarmSummary_Click()
  DevDo = GetAlarmSummary
  ShowBiModal DeviceForm
End Sub

Private Sub ServiceGetEnrollmentSummary_Click()
  ShowBiModal EnrollForm
End Sub

Private Sub ServiceListElement_Click()
  ReadPropForm.PropFn.ListIndex = 2
  ShowBiModal ReadPropForm
End Sub

Private Sub ServicePrivateTransfer_Click()
  ShowBiModal PrivateForm
End Sub

Private Sub ServiceReadPropertyConditional_Click()
  ShowBiModal ReadCondForm
End Sub

Private Sub ServiceReadWriteProperty_Click()
  ReadPropForm.PropFn.ListIndex = 0
  ShowBiModal ReadPropForm
End Sub

Private Sub ServiceReinitializeDevice_Click()
  DevDo = ReinitDevice
  ShowBiModal DeviceForm
End Sub

Private Sub ServiceTextMessage_Click()
  ShowBiModal MsgForm
End Sub

Private Sub ServiceTimeSynchronization_Click()
  ShowBiModal TimeSynchForm
End Sub

Private Sub ServiceVirtualTerminal_Click()
  ShowBiModal VTForm
End Sub

Private Sub ServiceWho_Click()
  ShowBiModal WhoForm
End Sub

Private Sub ShowNL(fn, Msg$)
  nlmsg = fn
  NLForm.Caption = Msg$
  ShowBiModal NLForm
End Sub

Private Sub Status(s$)
  StatusList.AddItem s$
  StatusList.ListIndex = StatusList.NewIndex
End Sub

Private Sub TestSBTests_Click()
  SBTForm.Show
End Sub

Private Sub Timer1_Timer()
Dim r, intimer, RxT, n
  If intimer Then Exit Sub                      'we've reentered
  intimer = True
  If Initialized And Not apduinuse Then         'normal background stuff
    n = NLOOPS
    Do While n > 0
      r = NALSAPIgetapdu(rnp, Len(rapdu), rapdu)
      If r = NALSE_Success Then                   'we've got something
        apdu = Left$(rapdu, rnp.nalsAlen)         'get the part we want
        If apdu <> "" Then
          RxT = Asc(rnp.nalsRxType)
          If RxT <> 0 Then
            If gPicsFilter = 0 Then               'special PICS filter
              InterpretNPDU rnp, TraceForm.RxList, TraceForm.RxHex
              InterpretAPDU TraceForm.RxList, TraceForm.RxHex
            End If
          End If
          SBReceiveIndicate RxT, rnp, apdu        'special handler for script based tests
          'special handler for PICS based tests
          If Asc(rnp.nalsInvokeID) = gPicsInvokeId Then
            If gPicsFilter = 1 Then
              InterpretNPDU rnp, TraceForm.RxList, TraceForm.RxHex
              InterpretAPDU TraceForm.RxList, TraceForm.RxHex
            End If
            PicsInterpretApdu rnp, apdu
          End If
        End If
      ElseIf r = NALSE_ReceivedPacketTooLarge Then 'can't fit it in rapdu
        Status "Packet Too Large, InvID=" + Format$(rnp.nalsInvokeID) + ",P=" + Format$(rnp.nalsLport) + ",S=" + Format$(rnp.nalsSnet) + ":" + hexn(Asc(rnp.nalsSlen), rnp.nalsSadr)
      Else
        n = 1                                      'we're done
      End If
      n = n - 1
    Loop
  Else                                             'our first time through
    Initialized = True
    VersionLabel(0).visible = False
    CopyrightLabel(0).visible = False
    VTSLabel(0).visible = False
    VersionLabel(1).visible = False
    CopyrightLabel(1).visible = False
    VTSLabel(1).visible = False
    If VTS256Color Then
      VTS256.BorderStyle = 1
      VTS256.Move 0, 0, VTS16.Width, VTS16.Height
    End If
    Width = (Width - ScaleWidth) + VTS16.Width + TextWidth(StatusList.Text) + 240 ' ***218
    Height = (Height - ScaleHeight) + VTS16.Height  '                       ***218
    StatusList.Move VTS16.Width, 0, ScaleWidth - VTS16.Width, VTS16.Height '***218
    StatusList.visible = True                       '                       ***218
    Timer1.Interval = 200
'    TraceForm.Show
  End If
  intimer = False
End Sub

Private Sub ViewClearRx_Click()
  ClearRxTrace
End Sub

Private Sub ViewClearTx_Click()
  ClearTxTrace
End Sub

Private Sub ViewHexadecimal_Click()
  ViewHexadecimal.Checked = Not ViewHexadecimal.Checked
  If ViewReceive.Checked Or ViewTransmit.Checked Then
    TraceForm.Show
  Else
    TraceForm.Hide
  End If
End Sub

Private Sub ViewNetworkLayer_Click()
  ViewNetworkLayer.Checked = Not ViewNetworkLayer.Checked
  If ViewReceive.Checked Or ViewTransmit.Checked Then
    TraceForm.Show
  Else
    TraceForm.Hide
  End If
End Sub

Private Sub ViewReceive_Click()
  ViewReceive.Checked = Not ViewReceive.Checked
  If ViewReceive.Checked Then
    TraceForm.Show
  Else
    If ViewTransmit.Checked Then
      TraceForm.Show
    Else
      TraceForm.Hide
    End If
  End If
End Sub

Private Sub ViewTargetDevice_Click()
  ViewTargetDevice.Checked = Not ViewTargetDevice.Checked
  If ViewTargetDevice.Checked Then
    TargetForm.Show
  Else
    TargetForm.Hide
  End If
End Sub

Private Sub ViewTrace_Click()
  ViewTrace.Checked = Not ViewTrace.Checked
  TraceForm.visible = ViewTrace.Checked
End Sub

Private Sub ViewTransmit_Click()
  ViewTransmit.Checked = Not ViewTransmit.Checked
  If ViewTransmit.Checked Then
    ViewTrace.Checked = True
    TraceForm.Show
  Else
    If ViewReceive.Checked Then
      ViewTrace.Checked = True
      TraceForm.Show
    Else
      ViewTrace.Checked = False
      TraceForm.Hide
    End If
  End If
End Sub

Private Sub ViewVerbose_Click()
  ViewVerbose.Checked = Not ViewVerbose.Checked
End Sub

Private Sub WhoIsRouter_Click()
  ShowNL Who_Is_Router_To_Network, "Who_Is_Router_To_Network"
End Sub

Private Sub WrtPropMenu_Click()
  If gDB.DataBase = 0 Then  'database is empty
    Dim e As Integer
    e = PicsError(eNODATABASE, "")
    Exit Sub
  End If

  gState.TestNr = cWrtPropTest        'perform write property test
  gState.SubTestNr = 0                'initial
  gState.ActiveObject = gDB.DataBase  'first object

  ShowBiModal PicsTestForm            'show PicsTestForm AND TargetForm
  TargetForm.ZOrder 0
End Sub

