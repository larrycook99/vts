VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form TraceForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   Caption         =   "Packet Trace"
   ClientHeight    =   5235
   ClientLeft      =   2535
   ClientTop       =   2100
   ClientWidth     =   5220
   BeginProperty Font 
      Name            =   "Courier New"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H80000008&
   Icon            =   "TRACE.frx":0000
   LinkTopic       =   "Form1"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   5235
   ScaleWidth      =   5220
   Visible         =   0   'False
   Begin VB.Timer Timer1 
      Interval        =   50
      Left            =   4740
      Top             =   4725
   End
   Begin VB.ComboBox VendorNames 
      Appearance      =   0  'Flat
      BackColor       =   &H00FFFFFF&
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00000000&
      Height          =   300
      Left            =   105
      Style           =   2  'Dropdown List
      TabIndex        =   12
      Top             =   4830
      Visible         =   0   'False
      Width           =   3030
   End
   Begin VB.ListBox TxOneline 
      Appearance      =   0  'Flat
      ForeColor       =   &H00800000&
      Height          =   450
      Left            =   525
      TabIndex        =   11
      Top             =   1890
      Visible         =   0   'False
      Width           =   2010
   End
   Begin VB.ListBox RxOneline 
      Appearance      =   0  'Flat
      ForeColor       =   &H000000FF&
      Height          =   450
      Left            =   525
      TabIndex        =   10
      Top             =   1365
      Visible         =   0   'False
      Width           =   2010
   End
   Begin VB.ListBox RxHex 
      Appearance      =   0  'Flat
      ForeColor       =   &H000000FF&
      Height          =   450
      Left            =   540
      TabIndex        =   2
      Top             =   150
      Visible         =   0   'False
      Width           =   2010
   End
   Begin VB.ListBox TxHex 
      Appearance      =   0  'Flat
      ForeColor       =   &H00800000&
      Height          =   450
      Left            =   525
      TabIndex        =   3
      Top             =   735
      Visible         =   0   'False
      Width           =   2010
   End
   Begin VB.ListBox TxList 
      Appearance      =   0  'Flat
      ForeColor       =   &H00800000&
      Height          =   450
      Left            =   2730
      TabIndex        =   4
      Top             =   735
      Visible         =   0   'False
      Width           =   2010
   End
   Begin VB.ListBox RxList 
      Appearance      =   0  'Flat
      ForeColor       =   &H000000FF&
      Height          =   450
      Left            =   2730
      TabIndex        =   5
      Top             =   105
      Visible         =   0   'False
      Width           =   2010
   End
   Begin Threed.SSCheck UseHex 
      Height          =   225
      Left            =   105
      TabIndex        =   6
      Top             =   1365
      Visible         =   0   'False
      Width           =   225
      _Version        =   65536
      _ExtentX        =   397
      _ExtentY        =   397
      _StockProps     =   78
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
   Begin VB.Label TxVbar 
      Appearance      =   0  'Flat
      BackColor       =   &H00000000&
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
      Height          =   450
      Left            =   2625
      MousePointer    =   9  'Size W E
      TabIndex        =   9
      Top             =   735
      Visible         =   0   'False
      Width           =   45
   End
   Begin VB.Label RxVbar 
      Appearance      =   0  'Flat
      BackColor       =   &H00000000&
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
      Height          =   450
      Left            =   2625
      MousePointer    =   9  'Size W E
      TabIndex        =   8
      Top             =   105
      Visible         =   0   'False
      Width           =   45
   End
   Begin VB.Label HexLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "h e x"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00808080&
      Height          =   645
      Left            =   150
      TabIndex        =   7
      Top             =   735
      Visible         =   0   'False
      Width           =   120
   End
   Begin VB.Label TxLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Tx"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FF0000&
      Height          =   195
      Left            =   0
      TabIndex        =   1
      Top             =   420
      Width           =   330
   End
   Begin VB.Label RxLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Rx"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H000000FF&
      Height          =   195
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   330
   End
End
Attribute VB_Name = "TraceForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Dim ChangingHex, Dragging, DragList, Moving
Dim RxW3, TxW3
Dim DownX As Single, DownY As Single

Dim LastRxHexTop, LastRxListTop, LastTxHexTop, LastTxListTop

Private Sub Form_Activate()
  RedrawFrameWindows
  LastRxHexTop = RxHex.TopIndex
  LastRxListTop = RxList.TopIndex
  LastTxHexTop = TxHex.TopIndex
  LastTxListTop = TxList.TopIndex
End Sub

Private Sub Form_Load()
  ReadVendors TraceForm.VendorNames             '                             ***218
  TraceForm.Move VTSform.Left, VTSform.Top + VTSform.Height, VTSform.Width '  ***218
End Sub

Private Sub Form_MouseMove(Button As Integer, Shift As Integer, X As Single, y As Single)
Dim bias As Single, nw, mw
  If (Not Dragging) Or Moving Then Exit Sub
  bias = (X - DownX)
  mw = ScaleWidth - (RxLabel.Width * 2)
  Moving = True
  If DragList Then                              'TxVbar
    nw = TxHex.Width + bias
    If (nw > (TextWidth("C") * 3)) And (nw < mw) Then
      TxHex.Width = TxHex.Width + bias
      TxVbar.Left = TxHex.Left + TxHex.Width
      TxList.Left = TxVbar.Left + TxVbar.Width
      TxList.Width = TxList.Width - bias
    End If
  Else                                          'RxVbar
    nw = RxHex.Width + bias
    If (nw > (TextWidth("C") * 3)) And (nw < mw) Then
      RxHex.Width = RxHex.Width + bias
      RxVbar.Left = RxHex.Left + RxHex.Width
      RxList.Left = RxVbar.Left + RxVbar.Width
      RxList.Width = RxList.Width - bias
    End If
  End If
  DownX = X
  DownY = y
  Moving = False
End Sub

Private Sub Form_MouseUp(Button As Integer, Shift As Integer, X As Single, y As Single)
  Dragging = False
  If DragList Then TxVbar.BackColor = Black Else RxVbar.BackColor = Black
End Sub

Private Sub Form_Resize()
  RedrawFrameWindows
End Sub

Private Sub Form_Unload(Cancel As Integer)
  VTSform.ViewReceive.Checked = False
  VTSform.ViewTransmit.Checked = False
End Sub

Private Sub HexLabel_MouseMove(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseMove Button, Shift, X + HexLabel.Left, y + HexLabel.Top
End Sub

Private Sub HexLabel_MouseUp(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseUp Button, Shift, X + HexLabel.Left, y + HexLabel.Top
End Sub

Private Sub RedrawFrameWindows()
Dim sh, h, w, l, vw, i, c$
  If (Not Initialized) Or (WindowState = 1) Then Exit Sub
    l = RxLabel.Width
    vw = RxVbar.Width
    w = ScaleWidth - l
    sh = ScaleHeight
    RxW3 = w \ 3
    TxW3 = RxW3
    HexLabel.Top = RxLabel.Height * 2
    UseHex.Top = HexLabel.Top + HexLabel.Height
    HexLabel.visible = True
    UseHex.visible = True
    ChangingHex = True
    UseHex.Value = False
    ChangingHex = False
    RxOneline.visible = False
    TxOneline.visible = False
    If VTSform.ViewTransmit.Checked And VTSform.ViewReceive.Checked Then
      h = sh / 2
      RxLabel.Move 0, 0
      RxLabel.visible = True
      TxLabel.Move 0, h
      TxLabel.visible = True
      If VTSform.ViewVerbose.Checked Then
        If VTSform.ViewHexadecimal.Checked And TraceForm.WindowState <> 1 Then
          ChangingHex = True
          UseHex.Value = True
          ChangingHex = False
          TxHex.Move l, h, TxW3, sh - h
          TxVbar.Move l + TxW3, h, vw, sh - h
          TxList.Move l + TxW3 + vw, h, w - (TxW3 + vw), sh - h
          RxHex.Move l, 0, RxW3, h
          RxVbar.Move l + RxW3, 0, vw, h
          RxList.Move l + RxW3 + vw, 0, w - (RxW3 + vw), h
          TxHex.visible = True
          RxHex.visible = True
          RxVbar.visible = True
          TxVbar.visible = True
        ElseIf TraceForm.WindowState <> 1 Then
          RxList.Move l, 0, w, h
          TxList.Move l, h, w, sh - h
          TxHex.visible = False
          RxHex.visible = False
          RxVbar.visible = False
          TxVbar.visible = False
        End If
        RxList.visible = True
        TxList.visible = True
      Else                                      'one line mode
        RxOneline.Move l, 0, w, h
        TxOneline.Move l, h, w, sh - h
        TxHex.visible = False
        RxHex.visible = False
        RxVbar.visible = False
        TxVbar.visible = False
        RxList.visible = False
        TxList.visible = False
        HexLabel.visible = False
        UseHex.visible = False
        RxOneline.visible = True
        TxOneline.visible = True
      End If
    ElseIf VTSform.ViewTransmit.Checked Then
      RxList.visible = False
      RxLabel.visible = False
      RxHex.visible = False
      RxVbar.visible = False
      TxLabel.Move 0, 0
      If VTSform.ViewVerbose.Checked Then
        If VTSform.ViewHexadecimal.Checked Then
          ChangingHex = True
          UseHex.Value = True
          ChangingHex = False
          TxHex.Move l, 0, TxW3, sh
          TxVbar.Move l + TxW3, 0, vw, sh
          TxList.Move l + TxW3 + vw, 0, w - (TxW3 + vw), sh
          TxHex.visible = True
          TxVbar.visible = True
        Else
          TxList.Move l, 0, w, sh
          TxHex.visible = False
          TxVbar.visible = False
        End If
        TxList.visible = True
      Else                                      'one line mode
        TxOneline.Move l, 0, w, sh
        TxHex.visible = False
        TxVbar.visible = False
        TxList.visible = False
        HexLabel.visible = False
        UseHex.visible = False
        TxOneline.visible = True
      End If
      TxLabel.visible = True
    ElseIf VTSform.ViewReceive.Checked Then
      TxList.visible = False
      TxLabel.visible = False
      TxHex.visible = False
      TxVbar.visible = False
      RxLabel.Move 0, 0
      If VTSform.ViewVerbose.Checked Then
        If VTSform.ViewHexadecimal.Checked Then
          ChangingHex = True
          UseHex.Value = True
          ChangingHex = False
          RxHex.Move l, 0, RxW3, sh
          RxVbar.Move l + RxW3, 0, vw, sh
          RxList.Move l + RxW3 + vw, 0, w - (RxW3 + vw), sh
          RxHex.visible = True
          RxVbar.visible = True
        Else
          RxList.Move l, 0, w, sh
          RxHex.visible = False
          RxVbar.visible = False
        End If
        RxList.visible = True
      Else                                      'one line mode
        RxOneline.Move l, 0, w, sh
        RxHex.visible = False
        RxVbar.visible = False
        RxList.visible = False
        HexLabel.visible = False
        UseHex.visible = False
        RxOneline.visible = True
      End If
      RxLabel.visible = True
    Else
      TxHex.visible = False
      TxList.visible = False
      TxVbar.visible = False
      RxHex.visible = False
      RxList.visible = False
      RxVbar.visible = False
      HexLabel.visible = False
      UseHex.visible = False
    End If

  RxOneline.Clear
  TxOneline.Clear
  If Not VTSform.ViewVerbose.Checked Then       'one line mode
    If RxOneline.visible Then                   'need to fill RX list
      For i = 0 To RxList.ListCount - 1
        c$ = Left$(RxList.List(i), 1)
        If (VTSform.ViewNetworkLayer.Checked And (c$ = ">")) Or (InStr(" .>-", c$) = 0) Then
          RxOneline.AddItem RxList.List(i)
          RxOneline.ItemData(RxOneline.NewIndex) = i
        End If
      Next i
    End If
    If TxOneline.visible Then                   'need to fill Tx list
      For i = 0 To TxList.ListCount - 1
        c$ = Left$(TxList.List(i), 1)
        If (VTSform.ViewNetworkLayer.Checked And (c$ = ">")) Or (InStr(" .>-", c$) = 0) Then
          TxOneline.AddItem TxList.List(i)
          TxOneline.ItemData(TxOneline.NewIndex) = i
        End If
      Next i
    End If
  End If
End Sub

Private Sub RxHex_Click()
  RxList.ListIndex = RxHex.ListIndex
End Sub

Private Sub RxHex_MouseMove(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseMove Button, Shift, X + RxHex.Left, y + RxHex.Top
End Sub

Private Sub RxHex_MouseUp(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseUp Button, Shift, X + RxHex.Left, y + RxHex.Top
End Sub

Private Sub RxLabel_DblClick()
  ClearRxTrace
End Sub

Private Sub RxList_Click()
  If RxList.ListIndex < RxHex.ListCount Then RxHex.ListIndex = RxList.ListIndex
End Sub

Private Sub RxList_MouseMove(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseMove Button, Shift, X + RxList.Left, y + RxList.Top
End Sub

Private Sub RxList_MouseUp(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseUp Button, Shift, X + RxList.Left, y + RxList.Top
End Sub

Private Sub RxOneline_DblClick()
  RxList.TopIndex = RxOneline.ItemData(RxOneline.ListIndex)
  VTSform.ViewVerbose.Checked = True
  RedrawFrameWindows
End Sub

Private Sub RxVbar_MouseDown(Button As Integer, Shift As Integer, X As Single, y As Single)
  StartDrag 0, X + RxVbar.Left, y + RxVbar.Top
End Sub

Private Sub RxVbar_MouseMove(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseMove Button, Shift, X + RxVbar.Left, y + RxVbar.Top
End Sub

Private Sub RxVbar_MouseUp(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseUp Button, Shift, X + RxVbar.Left, y + RxVbar.Top
End Sub

Private Sub StartDrag(vbar, X As Single, y As Single)
Dim r
  DragList = vbar
  Dragging = True
  If vbar Then TxVbar.BackColor = Grey Else RxVbar.BackColor = Grey
  DownX = X
  DownY = y
End Sub

Private Sub Timer1_Timer()
'  This timer coordinates the scrolling of the RX/TX list
'  boxes with their hex counterparts
  If RxHex.TopIndex <> LastRxHexTop Then
    RxList.TopIndex = RxHex.TopIndex
  ElseIf RxList.TopIndex <> LastRxListTop Then
    RxHex.TopIndex = RxList.TopIndex
  End If
  If TxHex.TopIndex <> LastTxHexTop Then
    TxList.TopIndex = TxHex.TopIndex
  ElseIf TxList.TopIndex <> LastTxListTop Then
    TxHex.TopIndex = TxList.TopIndex
  End If
  LastRxHexTop = RxHex.TopIndex
  LastRxListTop = RxList.TopIndex
  LastTxHexTop = TxHex.TopIndex
  LastTxListTop = TxList.TopIndex
End Sub

Private Sub TxHex_Click()
  TxList.ListIndex = TxHex.ListIndex
End Sub

Private Sub TxHex_MouseMove(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseMove Button, Shift, X + TxHex.Left, y + TxHex.Top
End Sub

Private Sub TxHex_MouseUp(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseUp Button, Shift, X + TxHex.Left, y + TxHex.Top
End Sub

Private Sub TxLabel_DblClick()
  ClearTxTrace
End Sub

Private Sub TxList_Click()
  If TxList.ListIndex < TxHex.ListCount Then TxHex.ListIndex = TxList.ListIndex
End Sub

Private Sub TxList_MouseMove(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseMove Button, Shift, X + TxList.Left, y + TxList.Top
End Sub

Private Sub TxList_MouseUp(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseUp Button, Shift, X + TxList.Left, y + TxList.Top
End Sub

Private Sub TxOneline_DblClick()
  TxList.TopIndex = TxOneline.ItemData(TxOneline.ListIndex)
  VTSform.ViewVerbose.Checked = True
  RedrawFrameWindows
End Sub

Private Sub TxVbar_MouseDown(Button As Integer, Shift As Integer, X As Single, y As Single)
  StartDrag 1, X + TxVbar.Left, y + TxVbar.Top
End Sub

Private Sub TxVbar_MouseMove(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseMove Button, Shift, X + TxVbar.Left, y + TxVbar.Top
End Sub

Private Sub TxVbar_MouseUp(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseUp Button, Shift, X + TxVbar.Left, y + TxVbar.Top
End Sub

Private Sub UseHex_Click(Value As Integer)
  If ChangingHex Then Exit Sub
  VTSform.ViewHexadecimal.Checked = Value
  RedrawFrameWindows
End Sub

Private Sub UseHex_MouseMove(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseMove Button, Shift, X + UseHex.Left, y + UseHex.Top
End Sub

Private Sub UseHex_MouseUp(Button As Integer, Shift As Integer, X As Single, y As Single)
  Form_MouseUp Button, Shift, X + UseHex.Left, y + UseHex.Top
End Sub

