VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form NLForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Network Layer"
   ClientHeight    =   3600
   ClientLeft      =   45
   ClientTop       =   375
   ClientWidth     =   6615
   ControlBox      =   0   'False
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
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   3600
   ScaleWidth      =   6615
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   330
      Left            =   5670
      TabIndex        =   11
      Top             =   420
      Width           =   750
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Height          =   330
      Left            =   5670
      TabIndex        =   10
      Top             =   60
      Width           =   750
   End
   Begin Threed.SSPanel ReasonPanel 
      Height          =   330
      Left            =   2310
      TabIndex        =   12
      Top             =   315
      Visible         =   0   'False
      Width           =   3165
      _Version        =   65536
      _ExtentX        =   5583
      _ExtentY        =   582
      _StockProps     =   15
      ForeColor       =   8421504
      BackColor       =   -2147483633
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
      Begin VB.ComboBox RejectReason 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   2
         Top             =   15
         Width           =   3135
      End
   End
   Begin Threed.SSPanel PerfPanel 
      Height          =   330
      Left            =   1680
      TabIndex        =   20
      Top             =   315
      Visible         =   0   'False
      Width           =   540
      _Version        =   65536
      _ExtentX        =   953
      _ExtentY        =   582
      _StockProps     =   15
      ForeColor       =   8421504
      BackColor       =   -2147483633
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
      Begin VB.TextBox PerfIndex 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   300
         Left            =   15
         MaxLength       =   3
         TabIndex        =   1
         Text            =   "0"
         Top             =   15
         Width           =   510
      End
   End
   Begin Threed.SSPanel NetPanel 
      Height          =   330
      Left            =   105
      TabIndex        =   18
      Top             =   315
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   582
      _StockProps     =   15
      ForeColor       =   8421504
      BackColor       =   -2147483633
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
      Begin VB.TextBox Networks 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   300
         Left            =   15
         TabIndex        =   0
         Top             =   15
         Width           =   1350
      End
   End
   Begin Threed.SSFrame RoutingFrame 
      Height          =   2640
      Left            =   105
      TabIndex        =   14
      Top             =   840
      Visible         =   0   'False
      Width           =   6420
      _Version        =   65536
      _ExtentX        =   11324
      _ExtentY        =   4657
      _StockProps     =   14
      Caption         =   "Routing Table"
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
      Alignment       =   1
      Begin VB.CommandButton DeleteButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Delete"
         Height          =   330
         Left            =   5565
         TabIndex        =   9
         Top             =   735
         Width           =   750
      End
      Begin VB.CommandButton AddButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Add"
         Height          =   330
         Left            =   5565
         TabIndex        =   8
         Top             =   2205
         Width           =   750
      End
      Begin Threed.SSPanel PortInfoPanel 
         Height          =   330
         Left            =   1575
         TabIndex        =   13
         Top             =   2205
         Width           =   3900
         _Version        =   65536
         _ExtentX        =   6879
         _ExtentY        =   582
         _StockProps     =   15
         ForeColor       =   8421504
         BackColor       =   -2147483633
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
         Begin VB.TextBox PortInfo 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   300
            Left            =   15
            MaxLength       =   255
            TabIndex        =   7
            Top             =   15
            Width           =   3870
         End
      End
      Begin Threed.SSPanel DNETPanel 
         Height          =   330
         Left            =   735
         TabIndex        =   25
         Top             =   2205
         Width           =   750
         _Version        =   65536
         _ExtentX        =   1323
         _ExtentY        =   582
         _StockProps     =   15
         ForeColor       =   8421504
         BackColor       =   -2147483633
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
         Begin VB.TextBox DNET 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   300
            Left            =   15
            MaxLength       =   5
            TabIndex        =   6
            Text            =   "00000"
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel PortIDPanel 
         Height          =   330
         Left            =   210
         TabIndex        =   24
         Top             =   2205
         Width           =   435
         _Version        =   65536
         _ExtentX        =   767
         _ExtentY        =   582
         _StockProps     =   15
         ForeColor       =   8421504
         BackColor       =   -2147483633
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
         Begin VB.TextBox PortID 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   300
            Left            =   15
            MaxLength       =   3
            TabIndex        =   5
            Text            =   "0"
            Top             =   15
            Width           =   405
         End
      End
      Begin Threed.SSPanel Rpanel 
         Height          =   1425
         Left            =   105
         TabIndex        =   15
         Top             =   735
         Width           =   5370
         _Version        =   65536
         _ExtentX        =   9472
         _ExtentY        =   2514
         _StockProps     =   15
         ForeColor       =   8421504
         BackColor       =   -2147483633
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
         Begin VB.ListBox Rtable 
            Appearance      =   0  'Flat
            Height          =   1395
            Left            =   15
            MultiSelect     =   2  'Extended
            TabIndex        =   16
            TabStop         =   0   'False
            Top             =   15
            Width           =   5340
         End
      End
      Begin Threed.SSCheck QueryAck 
         Height          =   225
         Left            =   105
         TabIndex        =   4
         Top             =   210
         Width           =   1065
         _Version        =   65536
         _ExtentX        =   1879
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Query"
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
      Begin VB.Label portinfolabel 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "port info"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   2835
         TabIndex        =   23
         Top             =   525
         Width           =   570
      End
      Begin VB.Label dnetlabel 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "DNET"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   840
         TabIndex        =   22
         Top             =   525
         Width           =   450
      End
      Begin VB.Label pidlabel 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "portID"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   210
         TabIndex        =   21
         Top             =   525
         Width           =   435
      End
   End
   Begin Threed.SSCheck Permanent 
      Height          =   225
      Left            =   3990
      TabIndex        =   3
      Top             =   105
      Visible         =   0   'False
      Width           =   1590
      _Version        =   65536
      _ExtentX        =   2805
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Permanent"
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
   Begin VB.Label PerfLabel 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Performance Index:"
      ForeColor       =   &H00808080&
      Height          =   195
      Left            =   1680
      TabIndex        =   19
      Top             =   105
      Width           =   1665
   End
   Begin VB.Label ToNetwork 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "To Network:"
      ForeColor       =   &H00808080&
      Height          =   195
      Left            =   105
      TabIndex        =   17
      Top             =   105
      Width           =   1065
   End
End
Attribute VB_Name = "NLForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim NeedNets                                    '0=net may be blank, 1=must have at least one
Dim Nwidth
Dim nheight

Private Sub AddButton_Click()
Dim p, d As Long
  If IsNumeric(PortID.Text) Then p = Val(PortID.Text) Else p = -1
  If (p < 0) Or (p > 255) Then
    Beep
    MsgBox "portID must be 0 to 255!", 48, "Invalid PortID"
    PortID.SetFocus
    Exit Sub
  End If
  If IsNumeric(DNET.Text) Then d = Val(DNET.Text) Else d = -1
  If (d < 0) Or (d > 65535) Then
    Beep
    MsgBox "DNET must be 0 to 65535!", 48, "Invalid DNET"
    DNET.SetFocus
    Exit Sub
  End If
  Rtable.AddItem PortID.Text + Chr$(9) + DNET.Text + Chr$(9) + PortInfo.Text
End Sub

Private Sub CloseButton_Click()
  HideBiModal NLForm
End Sub

Private Sub DeleteButton_Click()
Dim i, ns

  ns = 0
  For i = 0 To Rtable.ListCount - 1
    If Rtable.Selected(i) Then ns = ns + 1
  Next i
  If ns > 1 Then                       'many selected
    Beep
    If MsgBox("Are you sure you want to remove" + Str$(ns) + " entries from this list?", 33, "Delete Multiple Routing Table Entries") <> 1 Then Exit Sub
  End If
  i = 0
  Do While i < Rtable.ListCount
    If Rtable.Selected(i) Then Rtable.RemoveItem i Else i = i + 1
  Loop
End Sub

Private Sub DNET_GotFocus()
  GoSelect DNET
End Sub

Private Sub Form_Activate()
  NLpkt.maNPDU = String$(Len(NLpkt.maNPDU), 0)

  ToNetwork.visible = True
  NetPanel.visible = True
  RoutingFrame.visible = False
  PerfPanel.visible = False
  ReasonPanel.visible = False
  Permanent.visible = False
  PerfLabel.Caption = ""
  NeedNets = 0
  ToNetwork.Caption = "To Network:"
  NetPanel.Width = Nwidth
  PerfLabel.Left = PerfPanel.Left
  Height = (Height - ScaleHeight) + RoutingFrame.Top
  Select Case nlmsg
    Case Who_Is_Router_To_Network
    Case I_Am_Router_To_Network
      NeedNets = 1
'      TargetForm.BroadcastDest.Value = True
'      TargetForm.SpecificDest.Visible = False
'      TargetForm.MACpanel.Visible = False
'      TargetForm.aliaslabel.Visible = False
'      TargetForm.aliaspanel.Visible = False
'      TargetForm.NewAliasButton.Visible = False
      NetPanel.Width = Rtable.Width
      ToNetwork.Caption = "To Network(s):      net,net,net..."
    Case I_Could_Be_Router_To_Network
      NeedNets = 1
      PerfLabel.Caption = "Performance Index:"
      PerfPanel.visible = True
    Case Reject_Message_To_Network
      NeedNets = 1
      PerfLabel.Left = ReasonPanel.Left
      PerfLabel.Caption = "Reason:"
'      TargetForm.SpecificDest.Value = True
'      TargetForm.BroadcastDest.Visible = False
      ReasonPanel.visible = True
    Case Router_Busy_To_Network
'      TargetForm.BroadcastDest.Value = True
'      TargetForm.SpecificDest.Visible = False
'      TargetForm.MACpanel.Visible = False
'      TargetForm.aliaslabel.Visible = False
'      TargetForm.aliaspanel.Visible = False
'      TargetForm.NewAliasButton.Visible = False
      NetPanel.Width = Rtable.Width
      ToNetwork.Caption = "To Network(s):      net,net,net..."
    Case Router_Available_To_Network
'      TargetForm.BroadcastDest.Value = True
'      TargetForm.SpecificDest.Visible = False
'      TargetForm.MACpanel.Visible = False
'      TargetForm.aliaslabel.Visible = False
'      TargetForm.aliaspanel.Visible = False
'      TargetForm.NewAliasButton.Visible = False
      NetPanel.Width = Rtable.Width
      ToNetwork.Caption = "To Network(s):      net,net,net..."
    Case Initialize_Routing_Table
      ToNetwork.visible = False
      NetPanel.visible = False
      QueryAck.Caption = "Query"
      RoutingFrame.visible = True
      Height = nheight
    Case Initialize_Routing_Table_Ack
      ToNetwork.visible = False
      NetPanel.visible = False
      QueryAck.Caption = "Ack"
      RoutingFrame.visible = True
      Height = nheight
    Case Establish_Connection_To_Network
      Permanent.visible = True
      Permanent.Value = True
      PerfLabel.Caption = "Termination Time:"
      PerfPanel.visible = True
    Case Disconnect_Connection_To_Network
  End Select
End Sub

Private Sub Form_Load()
Dim di&
ReDim tabvals(2)
  tabvals(0) = 25
  tabvals(1) = 57
  nheight = Height
  di& = SendMessageArray(Rtable.hwnd, LB_SETTABSTOPS, 2, tabvals(0))
  Nwidth = NetPanel.Width
  Permanent.Move ReasonPanel.Left, ReasonPanel.Top
  RejectReason.Clear
  RejectReason.AddItem "Other"
  RejectReason.AddItem "Can't Route to DNET"
  RejectReason.AddItem "Router Busy"
  RejectReason.AddItem "Unknown NL Message"
  RejectReason.AddItem "Message Too Long To Route"
End Sub

Private Sub Networks_GotFocus()
  GoSelect Networks
End Sub

Private Sub Networks_KeyPress(KeyAscii As Integer)
  If KeyAscii = 13 Then
    KeyAscii = 0
    OKButton_Click
  End If
End Sub

Private Sub OKButton_Click()
Dim c$, n$, u$, i, j, nrte, DNET, p, maxn, nsdu, np
  NLpkt.maNlen = 0
  np = 1
  StuffByte np, BACnetVersion
  nsdu = NSDUisNLmessage + TargetForm.priority.ListIndex   'initially only this
  StuffByte np, nsdu                            'we'll update this later
  n$ = Networks.Text
  If TargetForm.Port.ListIndex < 0 Then         'no port selected
    Beep
    MsgBox "You must select a port to transmit on!", 48, "No Port Selected"
    TargetForm.LocalRoute.Value = True
    DoEvents
    TargetForm.Port.SetFocus
    Exit Sub
  End If
  p = Val(Left$(TargetForm.Port.Text, 3))       'get port number
  maxn = NALSAPImaxNPDUlength(p)                'max length for this port
  If TargetForm.GlobalRoute.Value Or TargetForm.RemoteRoute.Value Then
    nsdu = nsdu Or NSDUhasDNET
    If TargetForm.GlobalRoute.Value Then
      StuffWord np, BroadcastDNET
    Else
      StuffWord np, Val(TargetForm.Network.Text)
    End If
    If TargetForm.RemoteRoute.Value And TargetForm.SpecificDest.Value Then
      j = Len(TargetForm.DestMACAddress.Text)
      u$ = TargetForm.DestMACAddress.Text
      If j And 1 Then
        u$ = "0" + u$
        j = j + 1
      End If
      StuffByte np, j / 2                       'the DLEN
      For i = 1 To j / 2                        'the DADR
       StuffByte np, Val("&h" + Mid$(u$, 1 + ((i - 1) * 2), 2))
      Next i
    Else                                        'no DADR needed, DLEN=0
      StuffByte np, 0                           'the DLEN=0
    End If
  End If
  If TargetForm.IncludeSourceRouting.Value Then 'we need SNET, SLEN, SADR
    nsdu = nsdu Or NSDUhasSNET
    StuffWord np, Val(TargetForm.SourceNetwork.Text)  'the SNET
    j = Len(TargetForm.SourceMACAddress.Text)
    If j = 0 Then
      Beep
      MsgBox "The MAC Address may not be zero length for SADR!", 48, "Invalid SADR"
      TargetForm.SourceMACAddress.SetFocus
      Exit Sub
    End If
      u$ = TargetForm.SourceMACAddress.Text
      If j And 1 Then
        u$ = "0" + u$
        j = j + 1
      End If
      StuffByte np, j / 2                       'the DLEN
      For i = 1 To j / 2                        'the DADR
       StuffByte np, Val("&h" + Mid$(u$, 1 + ((i - 1) * 2), 2))
      Next i
  End If
  If nsdu And NSDUhasDNET Then StuffByte np, 255        'init HopCount
  StuffByte np, nlmsg                           'network layer message type
  Select Case nlmsg
    Case Who_Is_Router_To_Network
      If n$ <> "" Then                          'network is optional
        StuffWord np, Val(n$)
      End If
    Case I_Am_Router_To_Network
      If n$ = "" Then
        Beep
        MsgBox "You must specify at least one DNET to be a Router_To!", 48, "DNET Required"
        Networks.SetFocus
        Exit Sub
      End If
      Do Until n$ = ""
        c$ = Nxt(n$)
        StuffWord np, Val(c$)
      Loop
    Case I_Could_Be_Router_To_Network
      If n$ = "" Then
        Beep
        MsgBox "You must specify the DNET you Could_Be a Router_To!", 48, "DNET Required"
        Networks.SetFocus
        Exit Sub
      End If
      i = Val(PerfIndex.Text)
      If (i < 0) Or (i > 255) Then
        Beep
        MsgBox "Performance Index must range from 0 (High) to 255 (Low)!", 48, "Invalid Performance Index"
        PerfIndex.SetFocus
        Exit Sub
      End If
      StuffWord np, Val(n$)
      StuffByte np, i
    Case Reject_Message_To_Network
      If RejectReason.ListIndex < 0 Then
        Beep
        MsgBox "You must specify a Reject Reason!", 48, "No Reason Selected"
        RejectReason.SetFocus
        Exit Sub
      End If
      If n$ = "" Then
        Beep
        MsgBox "You must specify the DNET to send rejection to!", 48, "DNET Required"
        Networks.SetFocus
        Exit Sub
      End If
      StuffByte np, RejectReason.ListIndex
      StuffWord np, Val(n$)
    Case Router_Busy_To_Network, Router_Available_To_Network
      Do Until n$ = ""
        c$ = Nxt(n$)
        StuffWord np, Val(c$)
      Loop
    Case Initialize_Routing_Table, Initialize_Routing_Table_Ack
      If QueryAck.Value Then                    'query or Ack
        StuffByte np, 0                         'num ports=0 means query or ack
      Else
        If nlmsg = Initialize_Routing_Table Then c$ = "Initialize!" Else c$ = "Ack!"
        nrte = Rtable.ListCount
        If nrte < 1 Then
          Beep
          MsgBox "You must specify at least one Routing Table entry to " + c$, 48, "No Routing Table Entries"
          Rtable.SetFocus
          Exit Sub
        End If
        StuffByte np, nrte                      'we'll optimistically assume they'll all fit
        For i = 0 To nrte - 1
          n$ = Rtable.List(i)
          p = Val(NxtC(n$, Chr$(9)))            'port id
          DNET = Val(NxtC(n$, Chr$(9)))         'dnet
          If (np + Len(n$)) > maxn Then
            Beep
            MsgBox "This Routing Table can't fit into the maximum sized NPDU for port " + TargetForm.Port.Text, 48, "Routing Table Too Large"
            Rtable.ListIndex = i
            DeleteButton.SetFocus
            Exit Sub
          End If
          StuffWord np, DNET
          StuffByte np, p
          StuffByte np, Len(n$)
          Mid$(NLpkt.maNPDU, np, Len(n$)) = n$
          NLpkt.maNlen = NLpkt.maNlen + Len(n$)
          np = np + Len(n$)
        Next i
      End If
      If nlmsg = Initialize_Routing_Table Then nsdu = nsdu Or NSDUexpectsreply
    Case Establish_Connection_To_Network
      If n$ = "" Then
        Beep
        MsgBox "You must specify the DNET to connect to!", 48, "DNET Required"
        Networks.SetFocus
        Exit Sub
      End If
      i = Val(PerfIndex.Text)
      If (i < 0) Or (i > 255) Then
        Beep
        MsgBox "Termination Time must range from 0 (permanent) to 255 seconds!", 48, "Invalid Termination Time"
        PerfIndex.SetFocus
        Exit Sub
      End If
      StuffWord np, Val(n$)
      StuffByte np, i
    Case Disconnect_Connection_To_Network
      If n$ = "" Then
        Beep
        MsgBox "You must specify the DNET to disconnect from!", 48, "DNET Required"
        Networks.SetFocus
        Exit Sub
      End If
      StuffWord np, Val(n$)
  End Select
  Mid$(NLpkt.maNPDU, 2, 1) = Chr$(nsdu)
  SendNL
End Sub

Private Sub PerfIndex_Change()
Dim i
  i = Val(PerfIndex.Text)
  If (i < 0) Or (i > 255) Then
    PerfIndex.ForeColor = Red
  Else
    PerfIndex.ForeColor = Green
  End If
  Permanent.Value = (i = 0)
End Sub

Private Sub PerfIndex_GotFocus()
  PerfIndex.SelStart = 0
  PerfIndex.SelLength = Len(PerfIndex.Text)
End Sub

Private Sub Permanent_Click(Value As Integer)
  If Value Then
    PerfIndex.Text = "0"
  End If
End Sub

Private Sub PortID_GotFocus()
  GoSelect PortID
End Sub

Private Sub PortInfo_GotFocus()
  GoSelect PortInfo
End Sub

Private Sub PortInfo_KeyUp(KeyCode As Integer, Shift As Integer)
  If (KeyCode = 13) And (Shift = 0) Then
    KeyCode = 0
    AddButton_Click
  End If
End Sub

Private Sub QueryAck_Click(Value As Integer)
Dim v
  v = (Value = 0)
  Rpanel.visible = v
  DeleteButton.visible = v
  AddButton.visible = v
  pidlabel.visible = v
  dnetlabel.visible = v
  portinfolabel.visible = v
  PortIDPanel.visible = v
  DNETPanel.visible = v
  PortInfoPanel.visible = v
End Sub

