VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form FileForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "AtomicRead/WriteFile"
   ClientHeight    =   4905
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5220
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
   ScaleHeight     =   4905
   ScaleWidth      =   5220
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton DeleteButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Delete"
      Height          =   315
      Left            =   4410
      TabIndex        =   18
      Top             =   1995
      Visible         =   0   'False
      Width           =   735
   End
   Begin VB.CommandButton AddButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Add"
      Height          =   315
      Left            =   4410
      TabIndex        =   17
      Top             =   2415
      Visible         =   0   'False
      Width           =   735
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   315
      Left            =   4410
      TabIndex        =   3
      Top             =   75
      Width           =   735
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Height          =   315
      Left            =   4410
      TabIndex        =   2
      Top             =   465
      Width           =   735
   End
   Begin Threed.SSPanel DataPanel 
      Height          =   330
      Left            =   105
      TabIndex        =   15
      Top             =   2415
      Visible         =   0   'False
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
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
      Alignment       =   1
      Begin VB.TextBox DataBox 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   270
         Left            =   30
         TabIndex        =   16
         Top             =   30
         Width           =   4155
      End
   End
   Begin Threed.SSPanel DataListPanel 
      Height          =   2010
      Left            =   105
      TabIndex        =   13
      Top             =   2835
      Visible         =   0   'False
      Width           =   5055
      _Version        =   65536
      _ExtentX        =   8916
      _ExtentY        =   3545
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
      Alignment       =   1
      Autosize        =   3
      Begin VB.ListBox DataList 
         Appearance      =   0  'Flat
         Height          =   1980
         Left            =   15
         TabIndex        =   14
         Top             =   15
         Width           =   5025
      End
   End
   Begin Threed.SSPanel InstancePanel 
      Height          =   315
      Left            =   2205
      TabIndex        =   4
      Top             =   210
      Width           =   2115
      _Version        =   65536
      _ExtentX        =   3731
      _ExtentY        =   556
      _StockProps     =   15
      Caption         =   "File Instance"
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
      Alignment       =   1
      Begin VB.TextBox FileInstance 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   285
         Left            =   1260
         MaxLength       =   7
         TabIndex        =   5
         Text            =   "0000001"
         Top             =   15
         Width           =   840
      End
   End
   Begin Threed.SSFrame Frame3D1 
      Height          =   1590
      Left            =   105
      TabIndex        =   6
      Top             =   735
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   2805
      _StockProps     =   14
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
      Begin Threed.SSPanel CountPanel 
         Height          =   315
         Left            =   210
         TabIndex        =   11
         Top             =   1155
         Width           =   3060
         _Version        =   65536
         _ExtentX        =   5398
         _ExtentY        =   556
         _StockProps     =   15
         Caption         =   "Octet Count"
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
         Alignment       =   1
         Begin VB.TextBox FileCount 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   285
            Left            =   1860
            MaxLength       =   10
            TabIndex        =   12
            Text            =   "0000000001"
            Top             =   15
            Width           =   1170
         End
      End
      Begin Threed.SSPanel FileStartPanel 
         Height          =   315
         Left            =   210
         TabIndex        =   7
         Top             =   735
         Width           =   3060
         _Version        =   65536
         _ExtentX        =   5398
         _ExtentY        =   556
         _StockProps     =   15
         Caption         =   "File Start Position"
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
         Alignment       =   1
         Begin VB.TextBox FileStart 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   285
            Left            =   1860
            MaxLength       =   10
            TabIndex        =   8
            Text            =   "0000000000"
            Top             =   15
            Width           =   1170
         End
      End
      Begin Threed.SSCheck AppendToFile 
         Height          =   225
         Left            =   2310
         TabIndex        =   20
         Top             =   420
         Visible         =   0   'False
         Width           =   960
         _Version        =   65536
         _ExtentX        =   1693
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Append"
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
         Alignment       =   1
      End
      Begin Threed.SSOption RecordAccess 
         Height          =   225
         Left            =   210
         TabIndex        =   10
         TabStop         =   0   'False
         Top             =   450
         Width           =   1800
         _Version        =   65536
         _ExtentX        =   3175
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Record Access"
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
      Begin Threed.SSOption StreamAccess 
         Height          =   225
         Left            =   210
         TabIndex        =   9
         Top             =   210
         Width           =   1800
         _Version        =   65536
         _ExtentX        =   3175
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Stream Access"
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
         Value           =   -1  'True
      End
   End
   Begin Threed.SSCommand HexButton 
      Height          =   330
      Left            =   4410
      TabIndex        =   19
      Top             =   1575
      Visible         =   0   'False
      Width           =   750
      _Version        =   65536
      _ExtentX        =   1323
      _ExtentY        =   582
      _StockProps     =   78
      Caption         =   "Text"
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
   Begin Threed.SSOption AtomicReadFile 
      Height          =   225
      Left            =   105
      TabIndex        =   1
      Top             =   105
      Width           =   1800
      _Version        =   65536
      _ExtentX        =   3175
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "AtomicReadFile"
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Value           =   -1  'True
   End
   Begin Threed.SSOption AtomicWriteFile 
      Height          =   225
      Left            =   105
      TabIndex        =   0
      TabStop         =   0   'False
      Top             =   420
      Width           =   1800
      _Version        =   65536
      _ExtentX        =   3175
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "AtomicWriteFile"
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
End
Attribute VB_Name = "FileForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim HexData, nheight


Private Sub AddButton_Click()
  If DataBox.Text <> "" Then
    DataList.AddItem DataBox.Text
    CountPanel.Caption = "Record Count=" + Format$(DataList.ListCount)
  End If
End Sub

Private Sub AppendToFile_Click(Value As Integer)
  ChangeAppend Value
End Sub

Private Sub AtomicReadFile_Click(Value As Integer)
  If Value Then
    HexButton.visible = False
    DeleteButton.visible = False
    AddButton.visible = False
    DataPanel.visible = False
    DataListPanel.visible = False
    AppendToFile.visible = False
    FileCount.visible = True
    If Val(FileStart.Text) < 0 Then FileStart.Text = "0"
    ChangeAppend False
    If StreamAccess.Value Then
      CountPanel.Caption = "Octet Count"
    Else
      CountPanel.Caption = "Record Count"
    End If
    Height = (Height - ScaleHeight) + DataPanel.Top
  End If
End Sub

Private Sub AtomicWriteFile_Click(Value As Integer)
Dim i
  If Value Then
    HexButton.visible = True
    DataPanel.visible = True
    AppendToFile.visible = True
    ChangeAppend AppendToFile.Value
    FileCount.visible = False
    If StreamAccess.Value Then
      DeleteButton.visible = False
      AddButton.visible = False
      DataListPanel.visible = False
      If HexData Then i = Len(DataBox.Text) / 2 Else i = Len(DataBox.Text)
      CountPanel.Caption = "Octet Count=" + Format$(i)
      Height = nheight - DataListPanel.Height
    Else
      DeleteButton.visible = True
      AddButton.visible = True
      DataListPanel.visible = True
      CountPanel.Caption = "Record Count=" + Format$(DataList.ListCount)
      Height = nheight
    End If
  End If
End Sub

Private Sub ChangeAppend(ByVal Value)
  If StreamAccess.Value Then
    FileStartPanel.Caption = "File Start Position"
  Else
    FileStartPanel.Caption = "File Start Record"
  End If
  If AtomicWriteFile.Value And Value Then
      FileStart.visible = False
      FileStart.Text = "-1"
      FileStartPanel.Caption = FileStartPanel.Caption + "=EOF"
      Exit Sub
  End If
  FileStart.Text = "0"
  FileStart.visible = True
End Sub

Private Sub CloseButton_Click()
  HideBiModal FileForm
End Sub

Private Sub DataBox_Change()
Dim i
  If AtomicWriteFile.Value And StreamAccess Then
    i = Len(DataBox.Text)
    If HexData Then i = i / 2
    CountPanel.Caption = "Octet Count=" + Format$(i)
  End If
End Sub

Private Sub DeleteButton_Click()
  If DataList.ListIndex >= 0 Then
    DataList.RemoveItem DataList.ListIndex
    CountPanel.Caption = "Record Count=" + Format$(DataList.ListCount)
  End If
End Sub

Private Sub FileInstance_Change()
Dim ins As Long
  If FileInstance.Text = "" Then Exit Sub
  ins = Val(FileInstance.Text)
  If (ins < MinDeviceInstance) Or (ins > 4194303) Then
    Beep
    FileInstance.ForeColor = Red
  Else
    FileInstance.ForeColor = Green
  End If
End Sub

Private Sub FileInstance_GotFocus()
  GoSelect FileInstance
End Sub

Private Sub Form_Load()
  nheight = Height
  Height = (Height - ScaleHeight) + DataPanel.Top
End Sub

Private Sub HexButton_Click()
Dim i

  HexData = Not HexData
  If HexData Then
    HexButton.Caption = "Hex"
    DataBox.Text = SwitchToHex(DataBox.Text)
    If RecordAccess.Value Then
      For i = 0 To DataList.ListCount - 1
        DataList.List(i) = SwitchToHex(DataList.List(i))
      Next i
    End If
  Else
    HexButton.Caption = "Text"
    DataBox.Text = SwitchToText(DataBox.Text)
    If RecordAccess.Value Then
      For i = 0 To DataList.ListCount - 1
        DataList.List(i) = SwitchToText(DataList.List(i))
      Next i
    End If
  End If
End Sub

Private Sub RecordAccess_Click(Value As Integer)
  If Value Then
    ChangeAppend AppendToFile.Value
    If AtomicReadFile.Value Then AtomicReadFile_Click True
    If AtomicWriteFile.Value Then AtomicWriteFile_Click True
  End If
End Sub

Private Sub SendButton_Click()
Dim alen, apdu As String * 4096, fins As Long
  If FileInstance.Text = "" Then
    Beep
    MsgBox "You must specify a File Instance!", 48, "No File Instance Specified"
    FileInstance.SetFocus
    Exit Sub
  End If
  fins = Val(FileInstance.Text)
  If (fins < MinDeviceInstance) Or (fins > 4194303) Then
    Beep
    MsgBox "The File Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid File Instance"
    FileInstance.SetFocus
    Exit Sub
  End If
  If FileStart.Text = "" Then
    Beep
    MsgBox "You must specify a " + FileStartPanel.Caption, 48, "No " + FileStartPanel.Caption + " Specified"
    FileStart.SetFocus
    Exit Sub
  End If
  If AtomicWriteFile.Value And StreamAccess.Value Then
    DataList.Clear
    DataList.AddItem DataBox.Text               'pass string always in DataList
  End If
  If AtomicReadFile.Value Then
    alen = eAtomicReadFileService(apdu, fins, RecordAccess.Value, Val(FileStart.Text), Val(FileCount.Text))
  Else
    alen = eAtomicWriteFileService(apdu, 4096, fins, RecordAccess.Value, Val(FileStart.Text), HexData, DataList.hwnd)
  End If
  If alen = 0 Then
    Beep
    MsgBox "Too much data, try deleting some records and send a smaller PDU!", 48, "PDU Too Large"
  Else
    SendNPDU Left$(apdu, alen), True
  End If
End Sub

Private Sub StreamAccess_Click(Value As Integer)
  If Value Then
    ChangeAppend AppendToFile.Value
    If AtomicReadFile.Value Then AtomicReadFile_Click True
    If AtomicWriteFile.Value Then AtomicWriteFile_Click True
  End If
End Sub

Private Function SwitchToHex(ByVal a$) As String
Dim i, h$
  h$ = ""
  If a$ <> "" Then
      For i = 1 To Len(a$)
        h$ = h$ + Right$("0" + Hex$(Asc(Mid$(a$, i, 1))), 2)
      Next i
  End If
  SwitchToHex = h$
End Function

Private Function SwitchToText(ByVal a$) As String
Dim i, h$
  h$ = ""
  If a$ <> "" Then
    If Len(a$) And 1 Then a$ = a$ + "0"
    For i = 1 To Len(a$) Step 2
        h$ = h$ + Chr$(Val("&H" + Mid$(a$, i, 2)))
    Next i
  End If
  SwitchToText = h$
End Function

