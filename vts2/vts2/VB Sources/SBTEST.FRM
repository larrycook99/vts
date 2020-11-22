VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "Threed32.ocx"
Begin VB.Form SBTForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Script Base Testing"
   ClientHeight    =   4650
   ClientLeft      =   4005
   ClientTop       =   4110
   ClientWidth     =   12390
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
   Icon            =   "SBTEST.frx":0000
   LinkTopic       =   "SBTForm"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4650
   ScaleWidth      =   12390
   Begin VB.CommandButton SetupButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Setup"
      Height          =   375
      Left            =   6120
      TabIndex        =   2
      Top             =   1320
      Width           =   1095
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   375
      Left            =   6120
      TabIndex        =   1
      Top             =   720
      Width           =   1095
   End
   Begin VB.CommandButton BeginButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Begin"
      Default         =   -1  'True
      Height          =   375
      Left            =   6120
      TabIndex        =   0
      Top             =   240
      Width           =   1095
   End
   Begin Threed.SSFrame StatusFrame 
      Height          =   2775
      Left            =   120
      TabIndex        =   10
      Top             =   1800
      Width           =   12015
      _Version        =   65536
      _ExtentX        =   21193
      _ExtentY        =   4895
      _StockProps     =   14
      Caption         =   "Status"
      ForeColor       =   0
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Begin Threed.SSPanel ResultsPanel 
         Height          =   450
         Left            =   120
         TabIndex        =   18
         Top             =   2160
         Width           =   6855
         _Version        =   65536
         _ExtentX        =   12091
         _ExtentY        =   794
         _StockProps     =   15
         ForeColor       =   65280
         BackColor       =   0
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         BevelOuter      =   1
         Alignment       =   0
         Autosize        =   3
         Begin VB.ListBox ResultsList 
            Appearance      =   0  'Flat
            BeginProperty Font 
               Name            =   "MS Sans Serif"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   -1  'True
               Strikethrough   =   0   'False
            EndProperty
            Height          =   420
            Left            =   15
            TabIndex        =   7
            Top             =   15
            Width           =   6825
         End
      End
      Begin Threed.SSPanel ContentsPanel 
         Height          =   450
         Left            =   120
         TabIndex        =   16
         Top             =   1320
         Width           =   6855
         _Version        =   65536
         _ExtentX        =   12091
         _ExtentY        =   794
         _StockProps     =   15
         ForeColor       =   65280
         BackColor       =   0
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         BevelOuter      =   1
         Alignment       =   0
         Autosize        =   3
         Begin VB.ListBox ContentsList 
            Appearance      =   0  'Flat
            BeginProperty Font 
               Name            =   "MS Sans Serif"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   420
            Left            =   15
            TabIndex        =   6
            Top             =   15
            Width           =   6825
         End
      End
      Begin Threed.SSPanel TestPanel 
         Height          =   450
         Left            =   120
         TabIndex        =   12
         Top             =   480
         Width           =   11295
         _Version        =   65536
         _ExtentX        =   19923
         _ExtentY        =   794
         _StockProps     =   15
         ForeColor       =   65280
         BackColor       =   0
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         BevelOuter      =   1
         Alignment       =   0
         Autosize        =   3
         Begin VB.ListBox TestList 
            Appearance      =   0  'Flat
            Height          =   420
            Left            =   15
            TabIndex        =   5
            Top             =   15
            Width           =   11265
         End
      End
      Begin VB.Label ResultsLabel 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Results"
         ForeColor       =   &H80000008&
         Height          =   255
         Left            =   120
         TabIndex        =   19
         Top             =   1920
         Width           =   1335
      End
      Begin VB.Label ContentsLabel 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Contents"
         ForeColor       =   &H80000008&
         Height          =   255
         Left            =   120
         TabIndex        =   17
         Top             =   1080
         Width           =   1335
      End
      Begin VB.Label ReferenceLabel 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Reference"
         ForeColor       =   &H80000008&
         Height          =   255
         Left            =   4320
         TabIndex        =   15
         Top             =   240
         Width           =   2655
      End
      Begin VB.Label DepenciesLabel 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Dependencies"
         ForeColor       =   &H00000000&
         Height          =   255
         Left            =   1800
         TabIndex        =   14
         Top             =   240
         Width           =   2175
      End
      Begin VB.Label TestLabel 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Test"
         ForeColor       =   &H80000008&
         Height          =   255
         Left            =   120
         TabIndex        =   13
         Top             =   240
         Width           =   1695
      End
   End
   Begin Threed.SSFrame SectionFrame 
      Height          =   735
      Left            =   120
      TabIndex        =   8
      Top             =   960
      Width           =   5775
      _Version        =   65536
      _ExtentX        =   10186
      _ExtentY        =   1296
      _StockProps     =   14
      Caption         =   "Enabled Sections"
      ForeColor       =   0
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Begin Threed.SSPanel SectionPanel 
         Height          =   255
         Left            =   120
         TabIndex        =   11
         Top             =   240
         Width           =   5535
         _Version        =   65536
         _ExtentX        =   9763
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "Panel3D1"
         ForeColor       =   65280
         BackColor       =   0
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         BevelOuter      =   1
         Alignment       =   0
         Autosize        =   3
         Begin VB.ListBox SectionList 
            Appearance      =   0  'Flat
            Height          =   225
            Left            =   15
            TabIndex        =   4
            Top             =   15
            Width           =   5505
         End
      End
   End
   Begin Threed.SSCheck LogCheck 
      Height          =   255
      Left            =   2400
      TabIndex        =   9
      Top             =   300
      Width           =   2175
      _Version        =   65536
      _ExtentX        =   3836
      _ExtentY        =   450
      _StockProps     =   78
      Caption         =   "Write results to log file"
      ForeColor       =   8421504
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin Threed.SSCheck StopCheck 
      Height          =   255
      Left            =   240
      TabIndex        =   3
      Top             =   300
      Width           =   2175
      _Version        =   65536
      _ExtentX        =   3836
      _ExtentY        =   450
      _StockProps     =   78
      Caption         =   " Stop after each test"
      ForeColor       =   8421504
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin VB.Label DebugLabel 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "DEBUG"
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   4800
      TabIndex        =   20
      Top             =   720
      Width           =   1095
   End
End
Attribute VB_Name = "SBTForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

' Various
Const Status_Visible = 5055
Const Status_Invisible = 2160

' Internal State
Dim DebugState As Integer


Private Sub AdjustText(s$, l%)
  
  ' check, if text is too long
  If (Len(s$) > 3) And (TextWidth(s$) > l) Then
    Do
      s$ = Left$(s$, Len(s$) - 4) + "..."
    Loop Until (Len(s$) = 3) Or (TextWidth(s$) <= l)
  End If

  ' add tabs, if text is too short
  Do While TextWidth(s$) < l
    s$ = s$ + Chr$(9)
  Loop
End Sub

Private Sub AssembleAPDU(apdu$, ByVal AL%)
Dim a As ALframeType
Dim pdu%, v%
  
  a = SBframeAL(AL)

  pdu = 0 ' store PDU-Type
  If Asc(a.Ctrl.pdu) Then pdu = Asc(a.pdu)

  If Asc(a.Ctrl.Flags) Then ' PDU-Type and Flags
    apdu$ = Chr$((pdu * 16) Or Asc(a.Flags))
  Else
    apdu$ = Chr$(pdu * 16)
  End If

  If (pdu = 0) Then ' MaxResponse
    If Asc(a.Ctrl.MaxResp) Then
      apdu$ = apdu$ & a.MaxResp
    Else
      apdu$ = apdu$ & Chr$(0)
    End If
  End If
    
  If (pdu <> 1) Then ' InvodeID
    If Asc(a.Ctrl.invokeID) Then
      apdu$ = apdu$ & a.invokeID
    Else
      apdu$ = apdu$ & Chr$(0)
    End If
  End If

  If (pdu = 0) Or (pdu = 3) Or (pdu = 4) Then ' sequence-nr and window-size
    If Asc(a.Ctrl.SequenceNr) Or Asc(a.Ctrl.WindowSize) Or (pdu = 4) Then
      If Asc(a.Ctrl.SequenceNr) Then
        apdu$ = apdu$ & a.SequenceNr
      Else
        apdu$ = apdu$ & Chr$(0)
      End If
      If Asc(a.Ctrl.WindowSize) Then
        apdu$ = apdu$ & a.WindowSize
      Else
        apdu$ = apdu$ & Chr$(1)
      End If
      If (pdu <> 4) Then
        Mid$(apdu$, 1) = Chr$(Asc(Left$(apdu$, 1)) Or 8)
      End If
    End If
  End If

  If (pdu <> 4) Then ' Choice parameter
    If Asc(a.Ctrl.Service) Then
      apdu$ = apdu$ & a.Service
    Else
      apdu$ = apdu$ & Chr$(0)
    End If
  End If

  ' and now the remaining stuff
  If Len(a.Ctrl.DATA) > 0 Then
    apdu$ = apdu$ & a.DATA
  End If
End Sub

Private Sub BeginButton_Click()
Dim k%

If (TestInProgress = -1) Then
    ' clear all results
    For k = 0 To UBound(SBTest) - 1
      SBTest(k).passed = 0
      SBTest(k).Result = ""
    Next

    ' determine with which test to start
    TestInProgress = TestList.ListIndex / 2
    If TestInProgress < 0 Then TestInProgress = 0
    CloseButton.Caption = "Stop"
    BeginButton.Enabled = False

    ' starting tests ...
    DoTests
  Else
    ' must be the continue button
    BeginButton.Caption = "Begin"
    BeginButton.Enabled = False

    ' continue tests ...
    TestInProgress = TestInProgress + 1
    DoTests
  End If
End Sub

Function SBByte$(v$)
  SBByte$ = LTrim$(Str$(Asc(v$)))
End Function

Private Function CheckALframe(Port%, DLC%, NL%, AL%) As Integer
Dim recCount%, ok%, nr%
Dim tmpDLC As DLCframeType
Dim tmpNL As NLframeType
Dim tmpAL As ALframeType

  CheckALframe = False ' default is failed
  recCount = UBound(SBrecList) - 1
  For nr = 0 To recCount
    If Not Asc(SBrecList(nr).Checked) Then
      If (SBrecList(nr).Port = Port) Then
        ok = True

        ' checking the DLC part - reduced
        If (DLC >= 0) Then
          ExtractDLC nr, tmpDLC
          If ok And Asc(SBframeDLC(DLC).Ctrl.SA) Then
            ok = cmpMAC(SBframeDLC(DLC).SA, tmpDLC.SA, SBframeDLC(DLC).Ctrl.SA)
          End If
        End If

        ' checking the NL part - reduced
        If ok And (NL >= 0) Then
          ExtractNL nr, tmpNL
          If ok And Asc(SBframeNL(NL).Ctrl.DNET) Then
            ok = cmpInt(SBframeNL(NL).DNET, tmpNL.DNET, SBframeNL(NL).Ctrl.DNET)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.DADR) Then
            ok = cmpMAC(SBframeNL(NL).DADR, tmpNL.DADR, SBframeNL(NL).Ctrl.DADR)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.SNET) Then
            ok = cmpInt(SBframeNL(NL).SNET, tmpNL.SNET, SBframeNL(NL).Ctrl.SNET)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.SADR) Then
            ok = cmpMAC(SBframeNL(NL).SADR, tmpNL.SADR, SBframeNL(NL).Ctrl.SADR)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.priority) Then
            ok = cmpOctet(SBframeNL(NL).priority, tmpNL.priority, SBframeNL(NL).Ctrl.priority)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.DER) Then
            ok = cmpOctet(SBframeNL(NL).DER, tmpNL.DER, SBframeNL(NL).Ctrl.DER)
          End If
        End If

        ' checking the AL part
        If ok And (AL >= 0) Then
          ExtractAL nr, tmpAL
          If Asc(SBframeAL(AL).Ctrl.pdu) Then
            ok = cmpOctet(SBframeAL(AL).pdu, tmpAL.pdu, SBframeAL(AL).Ctrl.pdu)
          End If
          If ok And Asc(SBframeAL(AL).Ctrl.Flags) Then
            ok = cmpOctet(SBframeAL(AL).Flags, tmpAL.Flags, SBframeAL(AL).Ctrl.Flags)
          End If
          If ok And Asc(SBframeAL(AL).Ctrl.MaxResp) Then
            ok = cmpOctet(SBframeAL(AL).MaxResp, tmpAL.MaxResp, SBframeAL(AL).Ctrl.MaxResp)
          End If
          If ok And Asc(SBframeAL(AL).Ctrl.invokeID) Then
            ok = cmpOctet(SBframeAL(AL).invokeID, tmpAL.invokeID, SBframeAL(AL).Ctrl.invokeID)
          End If
          If ok And Asc(SBframeAL(AL).Ctrl.SequenceNr) Then
            ok = cmpOctet(SBframeAL(AL).SequenceNr, tmpAL.SequenceNr, SBframeAL(AL).Ctrl.SequenceNr)
          End If
          If ok And Asc(SBframeAL(AL).Ctrl.WindowSize) Then
            ok = cmpOctet(SBframeAL(AL).WindowSize, tmpAL.WindowSize, SBframeAL(AL).Ctrl.WindowSize)
          End If
          If ok And Asc(SBframeAL(AL).Ctrl.Service) Then
            ok = cmpOctet(SBframeAL(AL).Service, tmpAL.Service, SBframeAL(AL).Ctrl.Service)
          End If
          If ok And (Len(SBframeAL(AL).Ctrl.DATA) > 0) Then
            ok = cmpDATA(SBframeAL(AL).DATA, tmpAL.DATA, SBframeAL(AL).Ctrl.DATA)
          End If
        End If

        ' check for success
        If ok Then
          SBrecList(nr).Checked = Chr$(1)
          CheckALframe = True
          Exit For
        End If

      End If
    End If
  Next
End Function

Private Function CheckFrame(f%) As Integer
Dim Port%, DLC%, NL%, AL%
Dim recCount%, i%

  Port = SBpacket(f).Port
  DLC = SBpacket(f).DLC
  NL = SBpacket(f).NL
  AL = SBpacket(f).AL

  ' check for silence
  If (AL = -1) And (NL = -1) And (DLC = -1) Then
    CheckFrame = True ' default is passed
    recCount = UBound(SBrecList) - 1
    For i = 0 To recCount
      ' we may check this packet
      If Not Asc(SBrecList(i).Checked) Then
        ' now check ...
        If SBrecList(i).Port = Port Then
          ' Failed
          CheckFrame = False
          Exit For
        End If
      End If
    Next
  ElseIf (AL <> -1) Then
    ' check for a application layer packet
    CheckFrame = CheckALframe(Port, DLC, NL, AL)
  Else
    ' check for a network layer packet
    CheckFrame = CheckNLframe(Port, DLC, NL)
  End If
End Function

Private Function CheckFrameLevel(ccl%, cf%, ByVal lf%) As Integer
Dim ncl%, l%, f% ' new-case-level

  ' default is ok
  CheckFrameLevel = True

  ' search for a correct case-level
  f = cf
  l = -1

  Do
    ncl = Asc(SBpacket(f).CaseLvl)
    If (ncl And 128) Then ' new case-level-marker
      If (l = -1) And (ncl And 15) > ccl Then
        ccl = (ncl And 15)
        Exit Do
      Else
        l = (ncl And 15)
      End If
    Else ' no case-level-marker
      If (ncl And 15) <> l Then
        If (ncl And 15) <= ccl Then
          ccl = (ncl And 15)
          Exit Do
        End If
      End If
    End If
    f = f + 1 ' catch next frame
  Loop Until (f > lf)

  If (f > lf) Then
    CheckFrameLevel = False ' no frame found
  Else
    cf = f ' pass result to calling function
  End If
End Function

Private Function CheckNLframe(Port%, DLC%, NL%) As Integer
Dim recCount%, nr%, ok%
Dim tmpDLC As DLCframeType
Dim tmpNL As NLframeType

  CheckNLframe = False ' default is failed
  recCount = UBound(SBrecList) - 1
  For nr = 0 To recCount
    If Not Asc(SBrecList(nr).Checked) Then
      If (SBrecList(nr).Port = Port) Then
        ok = True
        ' checking the DLC part
        If (DLC >= 0) Then
          ExtractDLC nr, tmpDLC
          If Asc(SBframeDLC(DLC).Ctrl.DA) Then
            ok = cmpMAC(SBframeDLC(DLC).DA, tmpDLC.DA, SBframeDLC(DLC).Ctrl.DA)
          End If
          If ok And Asc(SBframeDLC(DLC).Ctrl.SA) Then
            ok = cmpMAC(SBframeDLC(DLC).SA, tmpDLC.SA, SBframeDLC(DLC).Ctrl.SA)
          End If
          If ok And Asc(SBframeDLC(DLC).Ctrl.LSAP) Then
            ok = cmpOctet(SBframeDLC(DLC).LSAP, tmpDLC.LSAP, SBframeDLC(DLC).Ctrl.LSAP)
          End If
        End If

        ' checking the NL part
        If ok And (NL >= 0) Then
          ExtractNL nr, tmpNL
          If Asc(SBframeNL(NL).Ctrl.Version) Then
            ok = cmpOctet(SBframeNL(NL).Version, tmpNL.Version, SBframeNL(NL).Ctrl.Version)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.DNET) Then
            ok = cmpInt(SBframeNL(NL).DNET, tmpNL.DNET, SBframeNL(NL).Ctrl.DNET)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.DADR) Then
            ok = cmpMAC(SBframeNL(NL).DADR, tmpNL.DADR, SBframeNL(NL).Ctrl.DADR)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.SNET) Then
            ok = cmpInt(SBframeNL(NL).SNET, tmpNL.SNET, SBframeNL(NL).Ctrl.SNET)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.SADR) Then
            ok = cmpMAC(SBframeNL(NL).SADR, tmpNL.SADR, SBframeNL(NL).Ctrl.SADR)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.HOPCNT) Then
            ok = cmpOctet(SBframeNL(NL).HOPCNT, tmpNL.HOPCNT, SBframeNL(NL).Ctrl.HOPCNT)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.Message) Then
            ok = cmpOctet(SBframeNL(NL).Message, tmpNL.Message, SBframeNL(NL).Ctrl.Message)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.priority) Then
            ok = cmpOctet(SBframeNL(NL).priority, tmpNL.priority, SBframeNL(NL).Ctrl.priority)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.DER) Then
            ok = cmpOctet(SBframeNL(NL).DER, tmpNL.DER, SBframeNL(NL).Ctrl.DER)
          End If
          If ok And Asc(SBframeNL(NL).Ctrl.VendorID) Then
            ok = cmpInt(SBframeNL(NL).VendorID, tmpNL.VendorID, SBframeNL(NL).Ctrl.VendorID)
          End If
          If ok And (Len(SBframeNL(NL).Ctrl.DATA) > 0) Then
            ok = cmpDATA(SBframeNL(NL).DATA, tmpNL.DATA, SBframeNL(NL).Ctrl.DATA)
          End If
        End If

        If ok Then
          SBrecList(nr).Checked = Chr$(1)
          CheckNLframe = True
          Exit For
        End If
      End If
    End If
  Next
End Function

Private Sub ChooseTest(ti%)
Dim Lo%, Hi%

  If (ti < 0) Or (ti >= UBound(SBTest)) Then Exit Sub

  ' adjust section display
  If SectionList.ListIndex <> SBTest(ti).Section Then
    If SectionList.ListCount > SBTest(ti).Section Then
      SectionList.TopIndex = SBTest(ti).Section
      SectionList.ListIndex = SBTest(ti).Section
    End If
  End If

  ' adjust test display
  Lo = 2 * ti
  Hi = Lo + 1
  If (TestList.ListIndex < Lo) Or (TestList.ListIndex > Hi) Then
    If TestList.ListCount > Lo Then
      ' make sure that we can see both lines
      TestList.TopIndex = Lo
      TestList.ListIndex = Lo
    End If
  End If

  ' adjust results display, same values for lo and hi
  If (ResultsList.ListIndex < Lo) Or (ResultsList.ListIndex > Hi) Then
    If ResultsList.ListCount > Lo Then
      ' make sure that we can see both lines
      ResultsList.TopIndex = Lo
      ResultsList.ListIndex = Lo
    End If
  End If

  ' adjust contents display
  Lo = SBTest(ti).FirstPacket
  If ti < UBound(SBTest) - 1 Then
    Hi = SBTest(ti + 1).FirstPacket - 1
  Else
    Hi = UBound(SBpacket) - 1
  End If
  If (ContentsList.ListIndex < Lo) Or (ContentsList.ListIndex > Hi) Then
    If ContentsList.ListCount > Lo Then
      ContentsList.TopIndex = Lo
      ContentsList.ListIndex = Lo
    End If
  End If
End Sub

Private Sub CloseButton_Click()
  If (TestInProgress >= 0) Then
    ' must be the stop button
    TestInProgress = -1
    CloseButton.Caption = "Close"
    BeginButton.Caption = "Begin"
    BeginButton.Enabled = True
  Else
    ' Clearing up memory
    ClearGlobalVars True
    ' and make a proper exit
    Unload SBTForm
  End If
End Sub

Private Function cmpDATA(src$, dst$, cmp$) As Integer
Dim s$, d$, c$, di%, l%, i%, res%

  ' special compare for exitens of any data
  If cmp$ = Chr$(4) & Chr$(1) & Chr$(0) Then
    cmpDATA = (Len(dst$) > 0)
    Exit Function
  End If

  res = True
  i = 1
  di = 1
  Do While res And (i < Len(cmp$))
    c$ = Mid$(cmp$, i, 1)
    l = Asc(Mid$(cmp$, i + 1, 1)) + 256 * Asc(Mid$(cmp$, i + 2, 1))
    s$ = Mid$(src$, di, l)
    d$ = Mid$(dst$, di, l)
    i = i + 3
    di = di + l

    Select Case Asc(c$)
      Case 0 ' Not present, any value
        res = True
      Case 1 ' Equal
        res = (d$ = s$)
      Case 2 ' Lower
        res = (d$ < s$)
      Case 3 ' Lower equal
        res = (d$ <= s$)
      Case 4 ' Higher
        res = (d$ > s$)
      Case 5 ' Higher equal
        res = (d$ >= s$)
      Case 6 ' Unequal
        res = (d$ <> s$)
    End Select
  Loop

  cmpDATA = res
End Function

Private Function cmpInt(src%, dst%, cmp$) As Integer

  Select Case Asc(cmp$)
    Case 0 ' Not present, any value
      cmpInt = True
    Case 1 ' Equal
      cmpInt = (dst = src)
    Case 2 ' Lower
      cmpInt = (dst < src)
    Case 3 ' Lower equal
      cmpInt = (dst <= src)
    Case 4 ' Higher
      cmpInt = (dst > src)
    Case 5 ' Higher equal
      cmpInt = (dst >= src)
    Case 6 ' Unequal
      cmpInt = (dst <> src)
  End Select

End Function

Private Function cmpMAC(src$, dst$, cmp$) As Integer
Dim s$, d$

  s$ = Mid$(src$, 2, Asc(src$))
  d$ = Mid$(dst$, 2, Asc(dst$))

  Select Case Asc(cmp$)
    Case 0 ' Not present, any value
      cmpMAC = True
    Case 1 ' Equal
      cmpMAC = (d$ = s$)
    Case 2 ' Lower
      cmpMAC = (d$ < s$)
    Case 3 ' Lower equal
      cmpMAC = (d$ <= s$)
    Case 4 ' Higher
      cmpMAC = (d$ > s$)
    Case 5 ' Higher equal
      cmpMAC = (d$ >= s$)
    Case 6 ' Unequal
      cmpMAC = (d$ <> s$)
  End Select

End Function

Private Function cmpOctet(src$, dst$, cmp$) As Integer

  Select Case Asc(cmp$)
    Case 0 ' Not present, any value
      cmpOctet = True
    Case 1 ' Equal
      cmpOctet = (Asc(dst$) = Asc(src$))
    Case 2 ' Lower
      cmpOctet = (Asc(dst$) < Asc(src$))
    Case 3 ' Lower equal
      cmpOctet = (Asc(dst$) <= Asc(src$))
    Case 4 ' Higher
      cmpOctet = (Asc(dst$) > Asc(src$))
    Case 5 ' Higher equal
      cmpOctet = (Asc(dst$) >= Asc(src$))
    Case 6 ' Unequal
      cmpOctet = (Asc(dst$) <> Asc(src$))
  End Select

End Function

Private Sub ContentsList_Click()
  ChooseTest FindTestContent(Int(ContentsList.ListIndex))
End Sub

Private Sub ContentsList_DblClick()
Dim m$, ci%, i%

  ci = ContentsList.ListIndex
  If (ci >= 0) And (ci < ContentsList.ListCount) Then
    m$ = ContentsList.List(ci)
    
    Do
      i = InStr(m$, ",")
      If (i > 0) Then
         Mid$(m$, i, 1) = Chr$(10)
      End If
    Loop Until (i = 0)

    MsgBox m$, 64, "Packet No. " & Str$(ci + 1)
  End If
End Sub

Private Function Ctrl$(c$)
  Select Case Asc(c$)
    Case 1: Ctrl = "="
    Case 2: Ctrl = "<"
    Case 3: Ctrl = "<="
    Case 4: Ctrl = ">"
    Case 5: Ctrl = ">="
    Case 6: Ctrl = "!="
    Case Else: Ctrl = "?"
  End Select
End Function

Private Sub DebugLabel_DblClick()
  DebugState = Not DebugState
  If DebugState Then
    DebugLabel.Caption = "DEBUG"

    If Not SetupButton.Enabled Then
      ' Altough in case of error, make the lists
      MakeTestList
      MakeContentsList
      MakeResultList

      ' but leave the BeginButton untouched
      SetupButton.Enabled = True
      SBTForm.Height = Status_Visible
    End If
  Else
    DebugLabel.Caption = ""
  End If
End Sub

Private Function DetMac(m$) As String
  If Asc(m$) Then
    DetMac = Mid$(m$, 2, Asc(m$))
  Else
    Select Case Asc(Mid$(m$, 2, 1))
      Case 1 ' Ethernet
        DetMac = Chr$(255) & Chr$(255) & Chr$(255) & Chr$(255) & Chr$(255) & Chr$(255)
      Case 2 ' Arcnet
        DetMac = Chr$(0)
      Case 3 ' MS/TP
        DetMac = Chr$(255)
      Case 4 ' LonTalk
        DetMac = Chr$(0) & Chr$(0)
    End Select
  End If
End Function

Private Function DoParse()
Dim Result%

DoParse = False 'not parsed

  ' Clear the sections displayed
  SBTForm!SectionList.Clear

  Result = ParseScript(0)
  If Result > 0 Then
    MakeTestList
    MakeContentsList
    MakeResultList

    SBTForm!BeginButton.Default = True
    SBTForm!BeginButton.Enabled = True
    SBTForm!SetupButton.Enabled = True
    SBTForm.Height = Status_Visible
    DoParse = True 'parsed
  Else
    SBTForm.Height = Status_Invisible
    SBTForm!BeginButton.Enabled = False
    SBTForm!SetupButton.Enabled = False
    SBTForm!BeginButton.Default = False
    If Result < 0 Then
      ' Show the troubles we've made
      SBparse.Hide
      SBparse.Show 1
      DoParse = False 'not parsed
    End If
  End If

End Function

Private Sub DoTests()
Dim current%, frame1%, frame2%
Dim d$, v%, k%

  Do While (TestInProgress < UBound(SBTest))
    ' Ask for Input
    DoEvents
    If (TestInProgress = -1) Then
      Exit Sub
    Else
      current = TestInProgress
    End If

    ' display current test
    If current * 2 < TestList.ListCount Then
      TestList.TopIndex = current * 2
      TestList.ListIndex = current * 2
    End If

    ' first check if the section of this test is enabled
    If Asc(SectionList.List(SBTest(current).Section)) <> 32 Then

      ' say to the protocol which test we are processing
      If LogCheck.Value Then
        ' open file
        Open gDefPicsLogFile For Append Access Write Lock Write As 2
        ' write test header to file
        Print #2, "TEST " & SBTest(current).nr
        Print #2, "------------------------------------------"
      End If

      ' check depencies for this test
      v = 0
      d$ = Trim$(SBTest(current).DepList)
      If (Len(d$) > 0) And (LCase$(d$) <> "none") Then
        Do
          v = v + 1 ' get next index
          k = v ' v would be modified by GetIndexValue
          d$ = GetIndexValue(SBTest(current).DepList, k)
          If Len(d$) > 0 Then
            k = GetTestWithNumber(d$, SBTest(current).Section)
            If (k >= 0) Then
              ' check test
              Select Case SBTest(k).passed
                Case 0, Is > 2 ' not tested
                  v = 4 ' depencies not tested
                  d$ = "Dependency test " & d$ & " not tested."
                  Exit Do
                Case 2 ' failed
                  v = 3 ' depencies failed - error
                  d$ = "Dependency test " & d$ & " failed."
                  Exit Do
              End Select
            Else ' k < 0
              v = 5 ' depencies not found - error
              d$ = "Dependency test " & d$ & " not found."
              Exit Do ' leave the Loop
            End If
          Else ' Len(d$) = 0
            v = 0 ' no depencies error
            Exit Do ' leave the loop
          End If ' Len(d$) > 0
        Loop
      End If ' Len(d$) > 0 ...

      If v = 0 Then
        ' go through the test frame after frame
        frame1 = SBTest(current).FirstPacket
        If current < UBound(SBTest) - 1 Then
          frame2 = SBTest(current + 1).FirstPacket - 1
        Else
          frame2 = UBound(SBpacket) - 1
        End If

        If LogCheck.Value Then
          ' write packet sequence to file
          For k = frame1 To frame2
            Print #2, Value$(k + 1) & ") " & ContentsList.List(k)
          Next
          Print #2, "------------------------------------------"
        End If

        v = TestFrames(frame1, frame2, d$)

        If LogCheck.Value Then
          ' write all received packets
          For k = 0 To (UBound(SBrecList) - 1)
            Print #2, ShowRecFrame(k)
          Next
          Print #2, "------------------------------------------"
        End If
      End If ' v = 0

      ' store result of test
      SBTest(current).passed = v
      SBTest(current).Result = d$
      
      ' Display new results
      UpdateResults current
    
      If LogCheck.Value Then
         ' write seperator to file
         Print #2, "=========================================="
         ' close file
         Close 2
      End If

      ' at the end of each test, check if we should wait
      If (current < UBound(SBTest) - 1) And StopCheck.Value Then
        BeginButton.Caption = "Continue"
        BeginButton.Enabled = True
        Exit Sub
      End If
    End If ' Asc(Section...) <> 32

    ' goto the next test
    If TestInProgress <> -1 Then
      TestInProgress = TestInProgress + 1
    End If
  Loop

  ' Tests a finished
  TestInProgress = -1
  CloseButton.Caption = "Close"
  BeginButton.Enabled = True

End Sub

Private Sub ExpectDelay()
Dim e As Integer

  e = TimerStart(gPicsInterval)
  Do
    DoEvents
  Loop Until gPicsTimeOut
  e = TimerStop()

End Sub

Private Sub ExtractAL(ByVal fi%, frame As ALframeType)
Dim t%, apdu$, m$

  If Asc(SBrecList(fi).RxType) Then ' AL packet
    apdu$ = SBrecList(fi).DATA
    
    ' extract now all values from the apdu
    t = Asc(ExtractString(apdu$, 1))
    frame.Flags = Chr$(t And &HF)
    t = (t \ 16)
    frame.pdu = Chr$(t)

    If (t = 0) Then ' for conf-req only
      frame.MaxResp = ExtractString(apdu$, 1)
    End If

    If (t <> 1) Then ' for all except unconf-req
      frame.invokeID = ExtractString(apdu$, 1)
    End If

    ' for seg-Ack or SEG-Flag is set
    If (t = 4) Or (Asc(frame.Flags) And 8) Then
      frame.SequenceNr = ExtractString(apdu$, 1)
      frame.WindowSize = ExtractString(apdu$, 1)
    End If

    ' every pdu
    frame.Service = ExtractString(apdu$, 1)
    frame.DATA = apdu$
  End If

  If DebugState Then
    m$ = "PDU = " & pdutype$(frame.pdu) & Chr$(10)
    m$ = m$ & "Flags = " & SBByte$(frame.Flags) & Chr$(10)
    m$ = m$ & "MaxResp = " & SBByte$(frame.MaxResp) & Chr$(10)
    m$ = m$ & "InvokeID = " & SBByte$(frame.invokeID) & Chr$(10)
    m$ = m$ & "SequenceNr = " & SBByte$(frame.SequenceNr) & Chr$(10)
    m$ = m$ & "WindowSize = " & SBByte$(frame.WindowSize) & Chr$(10)
    m$ = m$ & "Service = " & Srvc$(frame.Service, frame.pdu) & Chr$(10)
    m$ = m$ & "ASDU = " & Chr$(10)
    For t = 1 To Len(frame.DATA)
      m$ = m$ & hex2(Mid$(frame.DATA, t, 1)) & " "
      If t Mod 8 = 0 Then m$ = m$ & Chr$(10)
    Next
    MsgBox m$, 64, "ExtractAL - Packet" & Str$(fi)
  End If
End Sub

Private Sub ExtractDLC(ByVal fi%, frame As DLCframeType)
Dim m$

  ' this is particular for al packets not always correct
  frame.SA = SBrecList(fi).SADR

  ' this is particular for al packets not always correct
  If SBrecList(fi).Broadcast Then
    frame.DA = Chr$(0) & Chr$(0)
  Else
    frame.DA = SBrecList(fi).DADR
  End If

  ' NALSERV can only receive 0x82 packets
  frame.LSAP = Chr$(BACnetLSAP)

  If DebugState Then
    m$ = "DA = " & Mac$(frame.DA) & Chr$(10)
    m$ = m$ & "SA = " & Mac$(frame.SA) & Chr$(10)
    m$ = m$ & "LSAP = " & SBByte$(frame.LSAP) & Chr$(10)
    MsgBox m$, 64, "ExtractDLC - Packet" & Str$(fi)
  End If
End Sub

Private Sub ExtractNL(ByVal fi%, frame As NLframeType)
Dim Control%, npdu$, l%, m$

  If Asc(SBrecList(fi).RxType) = 0 Then ' NL packet
    npdu$ = SBrecList(fi).DATA
    
    ' extract now all values from the npdu
    frame.Version = ExtractString(npdu$, 1)
    Control = Asc(ExtractString(npdu$, 1))
    frame.priority = Chr$(Control And 3)
    frame.DER = Chr$((Control And 4) \ 4)

    If (Control And NSDUhasDNET) Then
      frame.DNET = Asc(ExtractString(npdu$, 1)) * 256 + Asc(ExtractString(npdu$, 1))
      l = Asc(ExtractString(npdu$, 1))
      frame.DADR = Chr$(l) & ExtractString(npdu$, l)
    End If

    If (Control And NSDUhasSNET) Then
      frame.SNET = Asc(ExtractString(npdu$, 1)) * 256 + Asc(ExtractString(npdu$, 1))
      l = Asc(ExtractString(npdu$, 1))
      frame.SADR = Chr$(l) & ExtractString(npdu$, l)
    End If

    If (Control And NSDUhasDNET) Then
      frame.HOPCNT = ExtractString(npdu$, 1)
    End If

    If (Control And NSDUisNLmessage) Then
      frame.Message = ExtractString(npdu$, 1)
      If Asc(frame.Message) And &H80 Then
        frame.VendorID = Asc(ExtractString(npdu$, 1)) * 256 + Asc(ExtractString(npdu$, 1))
      End If
    End If

    frame.DATA = npdu$

  Else ' AL packet
    frame.DNET = SBrecList(fi).DNET ' not always correct
    frame.DADR = SBrecList(fi).DADR ' not always correct
    frame.SNET = SBrecList(fi).SNET ' not always correct
    frame.SADR = SBrecList(fi).SADR ' not always correct
    frame.DER = Chr$(SBrecList(fi).ExpectReply)
    frame.priority = Chr$(SBrecList(fi).Pri)
  End If

  If DebugState Then
    m$ = "Ver = " & SBByte$(frame.Version) & Chr$(10)
    m$ = m$ & "DNET = " & Value$(frame.DNET) & Chr$(10)
    m$ = m$ & "DADR = " & Mac$(frame.DADR) & Chr$(10)
    m$ = m$ & "SNET = " & Value$(frame.SNET) & Chr$(10)
    m$ = m$ & "SADR = " & Mac$(frame.SADR) & Chr$(10)
    m$ = m$ & "HOPCNT = " & SBByte$(frame.HOPCNT) & Chr$(10)
    m$ = m$ & "Msg = " & Msg$(frame.Message) & Chr$(10)
    m$ = m$ & "Prio = " & SBByte$(frame.priority) & Chr$(10)
    m$ = m$ & "DER = " & SBByte$(frame.DER) & Chr$(10)
    m$ = m$ & "Vendor = " & Value$(frame.VendorID) & Chr$(10)
    m$ = m$ & "NSDU = " & Chr$(10)
    For l = 1 To Len(frame.DATA)
      m$ = m$ & hex2(Mid$(frame.DATA, l, 1)) & " "
      If l Mod 8 = 0 Then m$ = m$ & Chr$(10)
    Next
    MsgBox m$, 64, "ExtractNL - Packet" & Str$(fi)
  End If
End Sub

Private Function ExtractString(src$, ByVal l%) As String

  If Len(src$) > l Then
    ExtractString = Left$(src$, l)
    src$ = Mid$(src$, l + 1)
  Else
    ExtractString = src$
    src$ = ""
  End If

End Function

Private Function FindTestContent(ci%) As Integer
Dim i%

  FindTestContent = -1
  If (ci < 0) Then Exit Function

  i = 0
  Do While (i < UBound(SBTest)) And (ci >= SBTest(i).FirstPacket)
    i = i + 1
  Loop

  FindTestContent = i - 1
End Function

Private Function FindTestSection(si%) As Integer
Dim i%

  FindTestSection = -1
  If (si < 0) Then Exit Function

  i = 0
  Do While (i < UBound(SBTest)) And (si <> SBTest(i).Section)
    i = i + 1
  Loop

  If i < UBound(SBTest) Then FindTestSection = i
End Function

Private Sub Form_Load()
Dim e As Integer
  ' Setting Debug State
  If DebugState Then
    DebugLabel.Caption = "DEBUG"
  Else
    DebugLabel.Caption = ""
  End If

' Setting Progess Variable
  TestInProgress = -1

  e = DoParse()

End Sub

Private Function GetNextFrame(ccl%, cf%, lf%) As Integer
Dim ncl%, ncm%

  ' the default is ok
  GetNextFrame = True

  If Asc(SBpacket(cf).CaseLvl) And 128 Then ' fail was case-marker
    cf = cf + 1
    Do While (cf <= lf)
      ncl = Asc(SBpacket(cf).CaseLvl)
      If ncl And 128 Then
        If (ncl And 15) = ccl Then Exit Do ' found
        If (ncl And 15) < ccl Then cf = lf ' failed
      End If
      cf = cf + 1
    Loop
  Else ' fail was no case-marker
    cf = cf + 1
    Do While (cf <= lf)
      ncl = Asc(SBpacket(cf).CaseLvl)
      ncm = Asc(SBpacket(cf).Command)
      If (ncm = 131) And (ncl = ccl) Then Exit Do ' found
      If (ncm <> 132) Or (ncl <> ccl) Then cf = lf ' failed
      cf = cf + 1
    Loop
  End If

  If (cf > lf) Then
    GetNextFrame = False
  Else
    ccl = (ncl And 15)
  End If
End Function

Private Function GetTestWithNumber(d$, sec%) As Integer
Dim i%

  GetTestWithNumber = -1
  For i = 0 To UBound(SBTest) - 1
    If (SBTest(i).nr = d$) And (SBTest(i).Section = sec%) Then
      GetTestWithNumber = i
      Exit For
    End If
  Next
End Function

Private Sub LogCheck_Click(Value As Integer)
Dim e As Integer

If LogCheck.Value <> 0 Then
  If (Trim$(gDefPicsLogFile) = "") Then
     e = PicsError(eNOLOGFILE, "")
     LogCheck.Value = 0
     Exit Sub
  End If
End If

End Sub

Private Function Mac$(v$)
Dim t$, i%
  t$ = ""
  If Asc(v$) = 0 Then
    t$ = "BCAST"
  ElseIf Asc(v$) > 7 Then
    t$ = "UNDEF"
  Else
    For i = 2 To Asc(v$) + 1
      t$ = t$ + Right$("00" + Hex$(Asc(Mid$(v$, i, 1))), 2)
    Next
  End If
  Mac = t$
End Function

Private Sub MakeContentsList()
Dim i%, j%, m$, tp%

  SBTForm!ContentsList.Clear

  For i = 0 To UBound(SBpacket) - 1

    If (Asc(SBpacket(i).CaseLvl) And 128) Then
      m$ = "-" ' a new case level
    ElseIf Asc(SBpacket(i).CaseLvl) Then
      m$ = " " ' intend for case level
    Else
      m$ = "" ' no case level
    End If

    For j = 1 To (Asc(SBpacket(i).CaseLvl) And 15)
      m$ = m$ & "_"
    Next

    Select Case (Asc(SBpacket(i).Command) And 15)
      Case 0: m$ = m$ & "SEND"
      Case 1: m$ = m$ & "XPCT"
      Case 2: m$ = m$ & "RES."
      Case 3: m$ = m$ & "OR"
      Case 4: m$ = m$ & "AND"
    End Select
    m$ = m$ & "(" & Value$(SBpacket(i).Port) & ")"
    
    If SBpacket(i).DLC >= 0 Then
      tp = SBpacket(i).DLC

      If Asc(SBframeDLC(tp).Ctrl.DA) > 0 Then
        m$ = m$ & ",DA" & Ctrl$(SBframeDLC(tp).Ctrl.DA)
        m$ = m$ & Mac$(SBframeDLC(tp).DA)
      End If
      If Asc(SBframeDLC(tp).Ctrl.SA) > 0 Then
        m$ = m$ & ",SA" & Ctrl$(SBframeDLC(tp).Ctrl.SA)
        m$ = m$ & Mac$(SBframeDLC(tp).SA)
      End If
      If Asc(SBframeDLC(tp).Ctrl.LSAP) > 0 Then
        m$ = m$ & ",LSAP" & Ctrl$(SBframeDLC(tp).Ctrl.LSAP)
        m$ = m$ & SBByte$(SBframeDLC(tp).LSAP)
      End If
    End If

    If SBpacket(i).NL >= 0 Then
      tp = SBpacket(i).NL

      If Asc(SBframeNL(tp).Ctrl.Version) > 0 Then
        m$ = m$ & ",Ver" & Ctrl$(SBframeNL(tp).Ctrl.Version)
        m$ = m$ & SBByte$(SBframeNL(tp).Version)
      End If
      If Asc(SBframeNL(tp).Ctrl.DNET) > 0 Then
        m$ = m$ & ",DNET" & Ctrl$(SBframeNL(tp).Ctrl.DNET)
        m$ = m$ & Value$(SBframeNL(tp).DNET)
      End If
      If Asc(SBframeNL(tp).Ctrl.DADR) > 0 Then
        m$ = m$ & ",DADR" & Ctrl$(SBframeNL(tp).Ctrl.DADR)
        m$ = m$ & Mac$(SBframeNL(tp).DADR)
      End If
      If Asc(SBframeNL(tp).Ctrl.SNET) > 0 Then
        m$ = m$ & ",SNET" & Ctrl$(SBframeNL(tp).Ctrl.SNET)
        m$ = m$ & Value$(SBframeNL(tp).SNET)
      End If
      If Asc(SBframeNL(tp).Ctrl.SADR) > 0 Then
        m$ = m$ & ",SADR" & Ctrl$(SBframeNL(tp).Ctrl.SADR)
        m$ = m$ & Mac$(SBframeNL(tp).SADR)
      End If
      If Asc(SBframeNL(tp).Ctrl.HOPCNT) > 0 Then
        m$ = m$ & ",HOPCNT" & Ctrl$(SBframeNL(tp).Ctrl.HOPCNT)
        m$ = m$ & SBByte$(SBframeNL(tp).HOPCNT)
      End If
      If Asc(SBframeNL(tp).Ctrl.Message) > 0 Then
        m$ = m$ & ",Msg" & Ctrl$(SBframeNL(tp).Ctrl.Message)
        m$ = m$ & Msg$(SBframeNL(tp).Message)
      End If
      If Asc(SBframeNL(tp).Ctrl.priority) > 0 Then
        m$ = m$ & ",Prio" & Ctrl$(SBframeNL(tp).Ctrl.priority)
        m$ = m$ & SBByte$(SBframeNL(tp).priority)
      End If
      If Asc(SBframeNL(tp).Ctrl.DER) > 0 Then
        m$ = m$ & ",DER" & Ctrl$(SBframeNL(tp).Ctrl.DER)
        m$ = m$ & SBByte$(SBframeNL(tp).DER)
      End If
      If Asc(SBframeNL(tp).Ctrl.VendorID) > 0 Then
        m$ = m$ & ",Vendor" & Ctrl$(SBframeNL(tp).Ctrl.VendorID)
        m$ = m$ & Value$(SBframeNL(tp).VendorID)
      End If
      If Len(SBframeNL(tp).Ctrl.DATA) > 0 Then
        m$ = m$ & ",NL" & Ctrl$(SBframeNL(tp).Ctrl.DATA)
        m$ = m$ & Stuff$(SBframeNL(tp).DATA)
      End If
    End If

    If SBpacket(i).AL >= 0 Then
      tp = SBpacket(i).AL

      If Asc(SBframeAL(tp).Ctrl.pdu) > 0 Then
        m$ = m$ & ",PDU" & Ctrl$(SBframeAL(tp).Ctrl.pdu)
        m$ = m$ & pdutype$(SBframeAL(tp).pdu)
      End If

      If Asc(SBframeAL(tp).Ctrl.Flags) > 0 Then
        m$ = m$ & ",Flags" & Ctrl$(SBframeAL(tp).Ctrl.Flags)
        m$ = m$ & SBByte$(SBframeAL(tp).pdu)
      End If

      If Asc(SBframeAL(tp).Ctrl.MaxResp) > 0 Then
        m$ = m$ & ",MaxResp" & Ctrl$(SBframeAL(tp).Ctrl.MaxResp)
        m$ = m$ & SBByte$(SBframeAL(tp).MaxResp)
      End If

      If Asc(SBframeAL(tp).Ctrl.invokeID) > 0 Then
        m$ = m$ & ",InvokeID" & Ctrl$(SBframeAL(tp).Ctrl.invokeID)
        m$ = m$ & SBByte$(SBframeAL(tp).invokeID)
      End If

      If Asc(SBframeAL(tp).Ctrl.SequenceNr) > 0 Then
        m$ = m$ & ",SeqNr" & Ctrl$(SBframeAL(tp).Ctrl.SequenceNr)
        m$ = m$ & SBByte$(SBframeAL(tp).SequenceNr)
      End If

      If Asc(SBframeAL(tp).Ctrl.WindowSize) > 0 Then
        m$ = m$ & ",WinSize" & Ctrl$(SBframeAL(tp).Ctrl.WindowSize)
        m$ = m$ & SBByte$(SBframeAL(tp).WindowSize)
      End If

      If Asc(SBframeAL(tp).Ctrl.Service) > 0 Then
        m$ = m$ & ",Service" & Ctrl$(SBframeAL(tp).Ctrl.Service)
        If Asc(SBframeAL(tp).Ctrl.pdu) > 0 Then
          m$ = m$ & Srvc$(SBframeAL(tp).Service, SBframeAL(tp).pdu)
        Else
          m$ = m$ & Srvc$(SBframeAL(tp).Service, Chr$(0))
        End If
      End If

      If Len(SBframeAL(tp).Ctrl.DATA) > 0 Then
        m$ = m$ & ",AL" & Ctrl$(SBframeAL(tp).Ctrl.DATA)
        m$ = m$ & Stuff$(SBframeAL(tp).DATA)
      End If
    End If

    If (SBpacket(i).DLC < 0) And (SBpacket(i).NL < 0) And (SBpacket(i).AL < 0) Then
      m$ = m$ & " nothing"
    End If

    ContentsList.AddItem m$
  Next

  If ContentsList.ListCount > 0 Then
    ContentsList.ListIndex = 0
  End If
End Sub

Private Sub MakeResultList()
Dim i%, m$

  ResultsList.Clear

  For i = 0 To UBound(SBTest) - 1
    m$ = "Test " & SBTest(i).nr & " - Section"
    m$ = m$ & Str$(SBTest(i).Section + 1) & " - "
    Select Case SBTest(i).passed
      Case 0: m$ = m$ & "not tested"
      Case 1: m$ = m$ & "passed"
      Case 2: m$ = m$ & "failed"
      Case 3: m$ = m$ & "not tested, depencies failed"
      Case 4: m$ = m$ & "not tested, depencies not tested"
      Case 5: m$ = m$ & "not tested, depencies not found"
    End Select
    ResultsList.AddItem m$
    If Len(SBTest(i).Result) > 0 Then
      ResultsList.AddItem Chr$(32) & SBTest(i).Result
    Else
      ResultsList.AddItem " - no results -"
    End If
  Next

  If ResultsList.ListCount > 0 Then
    ResultsList.ListIndex = 0
  End If
End Sub

Private Sub MakeTestList()
Dim i%, s$
  
  SBTForm!TestList.Clear

  For i = 0 To UBound(SBTest) - 1
    s$ = RTrim$(SBTest(i).nr)
    AdjustText s$, 1350

    s$ = s$ + RTrim(SBTest(i).DepList)
    AdjustText s$, 3600

    s$ = s$ + RTrim(SBTest(i).Reference)

    SBTForm!TestList.AddItem s$
    SBTForm!TestList.AddItem Chr$(32) & SBTest(i).Comment
  Next

  If SBTForm!TestList.ListCount > 0 Then
    SBTForm!TestList.ListIndex = 0
  End If
End Sub

Private Function Msg$(v$)
  Select Case Asc(v$)
    Case Who_Is_Router_To_Network
      Msg = "Who's-RTN"
    Case I_Am_Router_To_Network
      Msg = "I'm-RTN"
    Case I_Could_Be_Router_To_Network
      Msg = "I-Cld-Be-RTN"
    Case Reject_Message_To_Network
      Msg = "Rej-MTN"
    Case Router_Busy_To_Network
      Msg = "RBTN"
    Case Router_Available_To_Network
      Msg = "RATN"
    Case Initialize_Routing_Table
      Msg = "IRT"
    Case Initialize_Routing_Table_Ack
      Msg = "IRT-Ack"
    Case Establish_Connection_To_Network
      Msg = "ECTN"
    Case Disconnect_Connection_To_Network
      Msg = "DCTN"
    Case Else
      Msg = SBByte(v$)
  End Select
End Function

Private Sub NLByte(pkt As MApacket, i%, ByVal v%)
  Mid$(pkt.maNPDU, i, 1) = Chr$(v And 255)
  pkt.maNlen = pkt.maNlen + 1
  i = i + 1
End Sub

Private Sub NLWord(pkt As MApacket, i%, ByVal v%)
  Mid$(pkt.maNPDU, i, 2) = Chr$(Hi(v)) + Chr$(Lo(v))
  pkt.maNlen = pkt.maNlen + 2
  i = i + 2
End Sub

Private Function pdutype$(pt$)
  Select Case Asc(pt$)
    Case 0: pdutype = "Confirmed-Request"
    Case 1: pdutype = "Unconfirmed-Request"
    Case 2: pdutype = "Simple-ACK"
    Case 3: pdutype = "Complex-ACK"
    Case 4: pdutype = "Segment-ACK"
    Case 5: pdutype = "Error"
    Case 6: pdutype = "Reject"
    Case 7: pdutype = "Abort"
    Case Else
      pdutype = Str$(Asc(pt$))
  End Select
End Function

Private Sub ResetRecList()
Dim recCount%, i%

  recCount = UBound(SBrecList)
  For i = 0 To recCount - 1
    SBrecList(i).Checked = Chr$(0)
  Next
End Sub

Private Sub ResultsList_Click()
  ChooseTest Int(ResultsList.ListIndex / 2)
End Sub

Private Sub ResultsList_DblClick()
Dim m$, ti%

  ti = Int(ResultsList.ListIndex) Or 1
  If (ti >= 0) And (ti < ResultsList.ListCount) Then
    m$ = ResultsList.List(ti)
    MsgBox m$, 64, "Test Sequence No. " & Str$(ti \ 2)
  End If
End Sub

Private Sub SectionList_Click()
  ChooseTest FindTestSection(Int(SectionList.ListIndex))
End Sub

Private Sub SectionList_DblClick()
  Dim s As String
  s = SectionList.List(SectionList.ListIndex)
  If Left$(s, 1) = Chr$(32) Then
    s = Chr$(215) + Right$(s, Len(s) - 1)
  Else
    s = Chr$(32) + Right$(s, Len(s) - 1)
  End If
  SectionList.List(SectionList.ListIndex) = s
End Sub

Private Function SendALFrame(Port%, DLC%, NL%, pdu$) As Integer
Dim frame As NALSpacket
Dim m$, i%, j

  ' let's fill the structure as far as we can
  ' retrieve some information for it
  frame.nalsLport = Chr$(Port)
  frame.nalsRequestorisserver = False

  ' default information
  frame.nalsExpectReply = False
  frame.nalsPri = 0
  frame.nalsDnet = 0       ' local
  frame.nalsDlen = Chr$(0) ' Broadcast
  frame.nalsSlen = Chr$(0) ' no source routing
  frame.nalsRlen = Chr$(0) ' local or broadcast

  If (NL >= 0) Then
    If Asc(SBframeNL(NL).Ctrl.DER) Then
      frame.nalsExpectReply = Asc(SBframeNL(NL).DER)
    End If
    If Asc(SBframeNL(NL).Ctrl.priority) Then
      frame.nalsPri = Asc(SBframeNL(NL).priority)
    End If
    If Asc(SBframeNL(NL).Ctrl.DNET) Then
      frame.nalsDnet = SBframeNL(NL).DNET
    End If
    If Asc(SBframeNL(NL).Ctrl.DADR) Then
      frame.nalsDlen = Left$(SBframeNL(NL).DADR, 1)
      frame.nalsDadr = Mid$(SBframeNL(NL).DADR, 2)
    End If
    If Asc(SBframeNL(NL).Ctrl.SNET) Then
      frame.nalsSnet = SBframeNL(NL).SNET
    End If
    If Asc(SBframeNL(NL).Ctrl.SADR) Then
      frame.nalsSlen = Left$(SBframeNL(NL).SADR, 1)
      frame.nalsSadr = Mid$(SBframeNL(NL).SADR, 2)
    End If
  End If
   
  If (DLC >= 0) Then
    If (NL >= 0) Then
      If Asc(SBframeNL(NL).Ctrl.DNET) Then
        If Asc(SBframeDLC(DLC).Ctrl.DA) Then
          frame.nalsRlen = Left$(SBframeDLC(DLC).DA, 1)
          frame.nalsRadr = Mid$(SBframeDLC(DLC).DA, 2)
        End If
      Else
        If Asc(SBframeDLC(DLC).Ctrl.DA) Then
          frame.nalsDlen = Left$(SBframeDLC(DLC).DA, 1)
          frame.nalsDadr = Mid$(SBframeDLC(DLC).DA, 2)
        End If
      End If
    Else
      If Asc(SBframeDLC(DLC).Ctrl.DA) Then
        frame.nalsDlen = Left$(SBframeDLC(DLC).DA, 1)
        frame.nalsDadr = Mid$(SBframeDLC(DLC).DA, 2)
      End If
    End If
  End If

  ' set the apdu len
  frame.nalsAlen = Len(pdu$)

  ' sending and displaying this packet:
  If DebugState Then
    ' show packet
    m$ = "nalsLport = " & SBByte$(frame.nalsLport) & Chr$(10)
    m$ = m$ & "nalsExpectReply = " & Value$(frame.nalsExpectReply) & Chr$(10)
    m$ = m$ & "nalsInvokeID = " & SBByte$(frame.nalsInvokeID) & Chr$(10)
    m$ = m$ & "nalsPri = " & Value$(frame.nalsPri) & Chr$(10)
    m$ = m$ & "nalsDnet = " & Value$(frame.nalsDnet) & Chr$(10)
    m$ = m$ & "nalsDadr = " & hexn(Asc(frame.nalsDlen), frame.nalsDadr) & Chr$(10)
    m$ = m$ & "nalsSnet = " & Value$(frame.nalsSnet) & Chr$(10)
    m$ = m$ & "nalsSadr = " & hexn(Asc(frame.nalsSlen), frame.nalsSadr) & Chr$(10)
    m$ = m$ & "nalsRadr = " & hexn(Asc(frame.nalsRlen), frame.nalsRadr) & Chr$(10)
    m$ = m$ & "nalsAlen = " & Value$(frame.nalsAlen) & Chr$(10)
    m$ = m$ & "nalsAPDU:" & Chr$(10)
    For i = 1 To frame.nalsAlen
      m$ = m$ & hex2(Mid$(pdu$, i, 1)) & " "
      If i Mod 8 = 0 Then m$ = m$ & Chr$(10)
    Next
    If MsgBox(m$, 65, "SendALframe") = 1 Then
      ' the following code is a copy from SendNPDU --!--
      apduinuse = True
      apdu = pdu$
'      SendALFrame = (NALSAPIcopyrequest(frame, pdu$) = 0)          ***213 Begin
      ' correct the information as really send
'      If (Asc(pdu$) And &HF0) = 0 Then
'        Mid$(apdu, 2, 1) = Chr$(NALSAPIgetmaxresponse(Port))
'        Mid$(apdu, 3, 1) = frame.nalsInvokeID
'      End If
'      InterpretNPDU frame, TraceForm.TxList, TraceForm.TxHex
'      InterpretAPDU TraceForm.TxList, TraceForm.TxHex
       j = NALSAPIcopyrequest(frame, pdu$)
       SendALFrame = (j = 0)
      ' correct the information as really send
      If (j = NALSInvalidPort) Then
        MsgBox "Invalid Port", 0, "Invalid Port"
      Else
        If (Asc(pdu$) And &HF0) = 0 Then
          Mid$(apdu, 2, 1) = Chr$(NALSAPIgetmaxresponse(Port))
          Mid$(apdu, 3, 1) = frame.nalsInvokeID
        End If
        InterpretNPDU frame, TraceForm.TxList, TraceForm.TxHex
        InterpretAPDU TraceForm.TxList, TraceForm.TxHex
      End If                                        '              ***213 End
      apduinuse = False
      ' --!--
    Else
      SendALFrame = True
    End If
  Else
    ' the following code is a copy from SendNPDU --!--
    apduinuse = True
    apdu = pdu$
'    SendALFrame = (NALSAPIcopyrequest(frame, pdu$) = 0)           ***213 Begin
    ' correct the information as really send
'    If (Asc(pdu$) And &HF0) = 0 Then
'      Mid$(apdu, 2, 1) = Chr$(NALSAPIgetmaxresponse(Port))
'      Mid$(apdu, 3, 1) = frame.nalsInvokeID
'    End If
'    InterpretNPDU frame, TraceForm.TxList, TraceForm.TxHex
'    InterpretAPDU TraceForm.TxList, TraceForm.TxHex
    j = NALSAPIcopyrequest(frame, pdu$)
    SendALFrame = (j = 0)
    ' correct the information as really send
    If (j = NALSInvalidPort) Then
        MsgBox "Invalid Port", 0, "Invalid Port"
    Else
      If (Asc(pdu$) And &HF0) = 0 Then
        Mid$(apdu, 2, 1) = Chr$(NALSAPIgetmaxresponse(Port))
        Mid$(apdu, 3, 1) = frame.nalsInvokeID
      End If
      InterpretNPDU frame, TraceForm.TxList, TraceForm.TxHex
      InterpretAPDU TraceForm.TxList, TraceForm.TxHex
    End If
    apduinuse = False                               '               ***213 End
    ' --!--
  End If
End Function

Private Function SendFrame(f%) As Integer
Dim Port%, DLC%, NL%, AL%
Dim apdu$

  Port = SBpacket(f).Port
  DLC = SBpacket(f).DLC
  NL = SBpacket(f).NL
  AL = SBpacket(f).AL

  If (AL <> -1) Then
    AssembleAPDU apdu, AL
    ' Send an AL-Packet
    SendFrame = SendALFrame(Port, DLC, NL, apdu)
  Else
    ' send a NL-Packet
    SendFrame = SendNLframe(Port, DLC, NL)
  End If
End Function

Private Function SendNLframe(Port%, DLC%, NL%) As Integer
Dim frame As MApacket
Dim np%, v%, c%, nsdu%
Dim m$

  ' setup the importend things
  frame.maLport = Chr$(Port)
  frame.maAlen = Chr$(NALSAPIgetlen(Port))
  frame.maNlen = 0
  np = 1

  ' DLC standard
  frame.maBroadcast = Chr$(1)
  frame.maSadr = ""
  frame.maLsap = Chr$(BACnetLSAP)

  ' modify if defined
  If (DLC >= 0) Then
    If Asc(SBframeDLC(DLC).Ctrl.DA) Then
      If Asc(SBframeDLC(DLC).DA) Then ' not a broadcast
        frame.maBroadcast = Chr$(0)
        frame.maDadr = DetMac(SBframeDLC(DLC).DA)
      End If
    End If
    If Asc(SBframeDLC(DLC).Ctrl.SA) Then
      ' due to a NALSERV problem, this will not work...
      If Asc(SBframeDLC(DLC).SA) Then
        frame.maSadr = Mid$(SBframeDLC(DLC).SA, 2)
      End If
    End If
    If Asc(SBframeDLC(DLC).Ctrl.LSAP) Then
      frame.maLsap = SBframeDLC(DLC).LSAP
    End If
  End If ' (dlc >= 0)

  ' NL standard
  v = BACnetVersion
  If (NL >= 0) Then
    If Asc(SBframeNL(NL).Ctrl.Version) Then
      v = Asc(SBframeNL(NL).Version)
    End If
  End If
  NLByte frame, np, v
  
  ' remember control byte
  nsdu = 0
  c = np
  np = np + 1

  If (NL >= 0) Then
    ' DNET, DLEN and DADR
    If Asc(SBframeNL(NL).Ctrl.DNET) Then
      nsdu = nsdu Or NSDUhasDNET
      NLWord frame, np, SBframeNL(NL).DNET
      If Asc(SBframeNL(NL).Ctrl.DADR) Then
        NLByte frame, np, Asc(SBframeNL(NL).DADR)
        For v = 1 To Asc(SBframeNL(NL).DADR)
          NLByte frame, np, Asc(Mid$(SBframeNL(NL).DADR, v + 1, 1))
        Next
      Else
        NLByte frame, np, 0
      End If
    End If

    ' SNET, SLEN and SADR
    If Asc(SBframeNL(NL).Ctrl.DNET) Then
      nsdu = nsdu Or NSDUhasSNET
      NLWord frame, np, SBframeNL(NL).SNET
      If Asc(SBframeNL(NL).Ctrl.SADR) Then
        NLByte frame, np, Asc(SBframeNL(NL).SADR)
        For v = 1 To Asc(SBframeNL(NL).SADR)
          NLByte frame, np, Asc(Mid$(SBframeNL(NL).SADR, v + 1, 1))
        Next
      Else
        NLByte frame, np, 0
      End If
    End If

    ' HOPCNT
    If (nsdu And NSDUhasDNET) Then
      If Asc(SBframeNL(NL).Ctrl.HOPCNT) Then
        NLByte frame, np, Asc(SBframeNL(NL).HOPCNT)
      Else
        NLByte frame, np, 255
      End If
    End If

    ' Message
    If Asc(SBframeNL(NL).Ctrl.Message) Then
      nsdu = nsdu Or NSDUisNLmessage
      v = Asc(SBframeNL(NL).Message)
      If Asc(SBframeNL(NL).Ctrl.VendorID) Then
        NLByte frame, np, v Or 128
        NLWord frame, np, SBframeNL(NL).VendorID
      Else
        NLByte frame, np, v
      End If
    End If

    ' Prio and DER
    If Asc(SBframeNL(NL).Ctrl.priority) Then
      nsdu = nsdu Or Asc(SBframeNL(NL).priority)
    End If
    If Asc(SBframeNL(NL).Ctrl.DER) Then
      nsdu = nsdu Or Asc(SBframeNL(NL).DER)
    End If
  End If ' (nl >= 0)

  ' store control byte
  NLByte frame, c, nsdu

  ' and other NL stuff
  If (NL > 0) Then
    If Len(SBframeNL(NL).Ctrl.DATA) > 0 Then
      For v = 1 To Len(SBframeNL(NL).DATA)
        NLByte frame, np, Asc(Mid$(SBframeNL(NL).DATA, v, 1))
      Next
    End If
  End If

  ' now we've got all and can send this packet
  If DebugState Then
    ' show packet
    m$ = "maLport = " & SBByte$(frame.maLport) & Chr$(10)
    m$ = m$ & "maAlen = " & SBByte$(frame.maAlen) & Chr$(10)
    m$ = m$ & "maLsap = " & SBByte$(frame.maLsap) & Chr$(10)
    m$ = m$ & "maBroadcast = " & SBByte$(frame.maBroadcast) & Chr$(10)
    m$ = m$ & "maDadr = " & hexn(Asc(frame.maAlen), frame.maDadr) & Chr$(10)
    m$ = m$ & "maSadr = " & hexn(Asc(frame.maAlen), frame.maSadr) & Chr$(10)
    m$ = m$ & "maNlen = " & Value$(frame.maNlen) & Chr$(10)
    m$ = m$ & "maNPDU:" & Chr$(10)
    For v = 1 To frame.maNlen
      m$ = m$ & hex2(Mid$(frame.maNPDU, v, 1)) & " "
      If v Mod 8 = 0 Then m$ = m$ & Chr$(10)
    Next
    If MsgBox(m$, 65, "SendNLframe") = 1 Then
      SendNLframe = (NALSAPItransmit(frame) = 0)
    Else
      SendNLframe = True
    End If
  Else
    SendNLframe = (NALSAPItransmit(frame) = 0)
  End If
End Function

Private Sub SetupButton_Click()
Dim recCount%, i%

  If DebugState And (TestInProgress <> -1) Then
    ' Disable the continue of the tests
    BeginButton.Enabled = False

    ' Prepare the receiving list
    SectionList.Clear

    recCount = UBound(SBrecList) - 1
    For i = 0 To recCount
      SectionList.AddItem ShowRecFrame(i)
    Next
  Else
    SBSetup.Show modal

    ' how we know, if the setup was changed
    MakeTestList
    MakeContentsList
    MakeResultList
  End If
End Sub

Private Function ShowRecFrame(ByVal i%) As String
Dim m$

  m$ = Value$(i + 1) & ") "
  Select Case Asc(SBrecList(i).RxType)
    Case 0: m$ = m$ & "NL("
    Case 1: m$ = m$ & "AL("
    Case Else: m$ = m$ & "RF("
  End Select
  m$ = m$ & Value$(SBrecList(i).Port) & ") "
  m$ = m$ & Value$(SBrecList(i).DNET) & "."
  m$ = m$ & Mac$(SBrecList(i).DADR) & ","
  m$ = m$ & Value$(SBrecList(i).SNET) & "."
  m$ = m$ & Mac$(SBrecList(i).SADR) & ","
  ' the Rlen/Radr seem to contain only junk
  'm$ = m$ & Mac$(SBrecList(i).RADR) & ","
  m$ = m$ & Stuff$(SBrecList(i).DATA)

  ShowRecFrame = m$
End Function

Private Function Srvc$(s$, pt$)
Dim i%
  
  ' I'm using functions from the TRACE.BAS module here
  Select Case Asc(pt$)
    Case 0, 2, 3, 5
      i = Asc(s$)
      If (i > 25) Then
        Srvc$ = Str$(i)
      Else
        Srvc = ConfirmedService(Asc(s$))
      End If
    Case 1
      i = Asc(s$)
      If (i > 8) Then
        Srvc$ = Str$(i)
      Else
        Srvc = UnconfirmedService(Asc(s$))
      End If
    Case 4
      Srvc = "none"
    Case 6, 7
      Srvc = Str$(Asc(s$))
    Case Else
      Srvc = Str$(Asc(s$))
  End Select
End Function

Private Function Stuff$(v$)
Dim t$, i%
  t$ = ""
  For i = 1 To Len(v$)
    t$ = t$ + Right$("00" + Hex$(Asc(Mid$(v$, i, 1))), 2)
  Next
  Stuff = t$
End Function

Private Function TestFrames(f1%, f2%, r$) As Integer
Dim ccl% ' new-case-level, current-case-level
Dim lastfailed%, sendfailed%

  ' show that we're thinking
  Screen.MousePointer = HourGlass

  ' disable window that nobody can interfere
  VTSform.Enabled = False

  ' initialize
  TestFrames = 1 ' default test passed
  r$ = "" ' not error results
  ccl = 0 ' current-case-level set to zero
  lastfailed = False
  sendfailed = False

  ' loop through all frames
  Do While (f1 <= f2)

    ' now check for special commands
    If Asc(SBpacket(f1).Command) = 255 Then
      ' check SBpacket(cf).Port for extra command
      ' and DLC, NL and AL for extra parameter

    Else ' Command <> 255
        
      ' determine which frame is to check next
      If lastfailed Then ' get next frame on level
        If Not GetNextFrame(ccl, f1, f2) Then
          r$ = r$ & "no other packets."
          TestFrames = 2
          Exit Do ' Finished and failed
        End If
      Else ' process with next frame
        If Not CheckFrameLevel(ccl, f1, f2) Then
          r$ = r$ & "last frame."
          Exit Do ' Finished and passed
        End If
      End If

      ' display current frame we process
      If f1 < ContentsList.ListCount Then
        ContentsList.ListIndex = f1
      End If

      ' in this case we should have a valid frame
      Select Case Asc(SBpacket(f1).Command)
        Case 0 ' SEND
          ' clear receive buffer
          ReDim SBrecList(0)
          sendfailed = Not SendFrame(f1)
          If sendfailed Then
            r$ = r$ & "SEND(" & Value$(f1 + 1) & ") failed, "
          Else
            r$ = r$ & "SEND(" & Value$(f1 + 1) & ") sent, "
          End If
        Case 3 ' OR-SEND
          If sendfailed Then
            ' clear receive buffer
            ReDim SBrecList(0)
            sendfailed = Not SendFrame(f1)
            If sendfailed Then
              r$ = r$ & "OR-SEND(" & Value$(f1 + 1) & ") failed, "
            Else
              r$ = r$ & "OR-SEND(" & Value$(f1 + 1) & ") sent, "
            End If
          End If
        Case 4 ' AND-SEND
          sendfailed = Not SendFrame(f1)
          If sendfailed Then
            r$ = r$ & "AND-SEND(" & Value$(f1 + 1) & ") failed, "
          Else
            r$ = r$ & "AND-SEND(" & Value$(f1 + 1) & ") sent, "
          End If
        Case 129 ' EXPECT
          ' check all received packets
          ResetRecList
          ' wait for time peroid
          ExpectDelay
          ' if not passed then lastfailed = True
          lastfailed = Not CheckFrame(f1)
          If lastfailed Then
            r$ = r$ & "EXPECT(" & Value$(f1 + 1) & ") failed, "
          Else
            r$ = r$ & "EXPECT(" & Value$(f1 + 1) & ") found, "
          End If
        Case 131 ' OR-EXPECT
          If lastfailed Then
            ' check all received packets
            ResetRecList
            ' if not passed then lastfailed = True
            lastfailed = Not CheckFrame(f1)
            If lastfailed Then
              r$ = r$ & "OR-EXPECT(" & Value$(f1 + 1) & ") failed, "
            Else
              r$ = r$ & "OR-EXPECT(" & Value$(f1 + 1) & ") found, "
            End If
          End If
        Case 132 ' AND-EXPECT
          ' check next received packets
          ' if not passed then lastfailed = True
          lastfailed = Not CheckFrame(f1)
          If lastfailed Then
            r$ = r$ & "AND-EXPECT(" & Value$(f1 + 1) & ") failed, "
          Else
            r$ = r$ & "AND-EXPECT(" & Value$(f1 + 1) & ") found, "
          End If
      End Select
    End If ' If (Command <> 255)

    If Not lastfailed Then f1 = f1 + 1 ' catch next frame
  Loop ' While (f1 <= f2)

  If Right$(r$, 1) <> "." Then
    r$ = r$ & "passed."
  End If

  ' enable window
  VTSform.Enabled = True

  ' show that we're finished
  Screen.MousePointer = Normal
End Function

Private Sub TestList_Click()
  ChooseTest Int(TestList.ListIndex / 2)
End Sub

Private Sub TestList_DblClick()
Dim m$, ti%

  ti = Int(TestList.ListIndex) Or 1
  If (ti >= 0) And (ti < TestList.ListCount) Then
    m$ = Trim$(TestList.List(ti))
    MsgBox m$, 64, "Test No. " & SBTest(ti \ 2).nr
  End If
End Sub

Private Sub UpdateResults(i%)
Dim m$, r$

  m$ = "Test " & SBTest(i).nr & " - Section"
  m$ = m$ & Str$(SBTest(i).Section + 1) & " - "
  Select Case SBTest(i).passed
    Case 0: m$ = m$ & "not tested"
    Case 1: m$ = m$ & "passed"
    Case 2: m$ = m$ & "failed"
    Case 3: m$ = m$ & "not tested, dependencies failed"
    Case 4: m$ = m$ & "not tested, dependencies not tested"
    Case 5: m$ = m$ & "not tested, dependencies not found"
  End Select

  If LogCheck.Value Then
    ' this is a good place to write the results in a file
    Print #2, m$
    r$ = ""
    If SBTest(i).passed > 1 Then
      r = "error: "
    End If
    r$ = r$ & SBTest(i).Result
    If Len(r$) > 0 Then Print #2, r$
  End If

  If i < ResultsList.ListCount Then
    ResultsList.List(2 * i) = m$
    If Len(SBTest(i).Result) > 0 Then
      ResultsList.List(2 * i + 1) = Chr$(32) & SBTest(i).Result
    Else
      ResultsList.List(2 * i + 1) = " - no results -"
    End If
  End If
End Sub

Private Function Value$(ByVal v%)
  If (v < 0) Or (v > 255) Then
    Value$ = Right$("0000" + Hex$(v), 4)
  Else
    Value$ = LTrim$(Str$(v))
  End If
End Function

