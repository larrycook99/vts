VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form MsgForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Confirmed Text Message"
   ClientHeight    =   3570
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5160
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
   ScaleHeight     =   3570
   ScaleWidth      =   5160
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   372
      Left            =   4290
      TabIndex        =   3
      Top             =   600
      Width           =   816
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   372
      Left            =   4290
      TabIndex        =   4
      Top             =   120
      Width           =   816
   End
   Begin Threed.SSPanel InstancePanel 
      Height          =   315
      Left            =   1260
      TabIndex        =   11
      Top             =   1050
      Width           =   2220
      _Version        =   65536
      _ExtentX        =   3916
      _ExtentY        =   556
      _StockProps     =   15
      Caption         =   "Source Device"
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
      Begin VB.TextBox DeviceInstance 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   285
         Left            =   1365
         MaxLength       =   7
         TabIndex        =   12
         Text            =   "0000001"
         Top             =   15
         Width           =   840
      End
   End
   Begin Threed.SSPanel MsgPanel 
      Height          =   2010
      Left            =   105
      TabIndex        =   0
      Top             =   1470
      Width           =   4950
      _Version        =   65536
      _ExtentX        =   8731
      _ExtentY        =   3545
      _StockProps     =   15
      ForeColor       =   65535
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
      Begin VB.TextBox Message 
         Appearance      =   0  'Flat
         Height          =   1980
         Left            =   15
         ScrollBars      =   2  'Vertical
         TabIndex        =   9
         Top             =   15
         Width           =   4920
      End
   End
   Begin Threed.SSPanel ClassPanel 
      Height          =   330
      Left            =   1260
      TabIndex        =   7
      Top             =   630
      Width           =   2850
      _Version        =   65536
      _ExtentX        =   5027
      _ExtentY        =   582
      _StockProps     =   15
      ForeColor       =   8421504
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
      Begin VB.TextBox MessageClass 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   255
         Left            =   30
         TabIndex        =   8
         Top             =   30
         Width           =   2715
      End
   End
   Begin Threed.SSPanel PriorityPanel 
      Height          =   330
      Left            =   3570
      TabIndex        =   5
      Top             =   1050
      Width           =   1485
      _Version        =   65536
      _ExtentX        =   2619
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
      Begin VB.ComboBox Priority 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   6
         Top             =   15
         Width           =   1455
      End
   End
   Begin Threed.SSCheck Confirmed 
      Height          =   225
      Left            =   105
      TabIndex        =   10
      Top             =   105
      Width           =   1800
      _Version        =   65536
      _ExtentX        =   3175
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Confirmed?"
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
   Begin VB.Label ClassLabel 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "message class"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00000000&
      Height          =   195
      Left            =   105
      TabIndex        =   1
      Top             =   630
      Width           =   1035
   End
   Begin VB.Label MessageLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "message"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   195
      Left            =   210
      TabIndex        =   2
      Top             =   1260
      Width           =   750
   End
End
Attribute VB_Name = "MsgForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Private Sub CloseButton_Click()
  HideBiModal MsgForm
End Sub

Private Sub DeviceInstance_Change()
Dim ins As Long
  If DeviceInstance.Text = "" Then Exit Sub
  ins = Val(DeviceInstance.Text)
  If (ins < MinDeviceInstance) Or (ins > 4194303) Then
    Beep
    DeviceInstance.ForeColor = Red
  Else
    DeviceInstance.ForeColor = Green
  End If
End Sub

Private Sub DeviceInstance_GotFocus()
  GoSelect DeviceInstance
End Sub

Private Sub Form_Load()
  priority.Clear
  priority.AddItem "normal"
  priority.AddItem "urgent"
  priority.ListIndex = 0
End Sub

Private Sub MessageClass_GotFocus()
  GoSelect MessageClass
End Sub

Private Sub Priority_Click()
  If priority.ListIndex = 1 Then priority.ForeColor = Red Else priority.ForeColor = Black
End Sub

Private Sub SendButton_Click()
Dim alen, apdu As String * 4096

  If DeviceInstance.ForeColor = Red Then
    Beep
    MsgBox "Source Device Instance must be" + Str$(MinDeviceInstance) + "..4194303", 48, "Invalid Device Instance"
    Exit Sub
  End If
  alen = eTextMessageService(apdu, 4096, Confirmed.Value, Val(DeviceInstance.Text), priority.ListIndex, MessageClass.Text, MESSAGE.Text)
  If alen = 0 Then
    Beep
    MsgBox "The text message is too large to be encoded in the current version of VTS. Try sending a smaller message!", 48, "Message Too Big"
    Exit Sub
  End If
  SendNPDU Left$(apdu, alen), Confirmed.Value
End Sub

