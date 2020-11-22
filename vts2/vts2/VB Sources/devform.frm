VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form DeviceForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Device Communication Control"
   ClientHeight    =   1155
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4335
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
   ScaleHeight     =   1155
   ScaleWidth      =   4335
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   372
      Left            =   3450
      TabIndex        =   4
      Top             =   705
      Width           =   816
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   372
      Left            =   3450
      TabIndex        =   5
      Top             =   225
      Width           =   816
   End
   Begin Threed.SSPanel PasswordPanel 
      Height          =   330
      Left            =   945
      TabIndex        =   0
      Top             =   735
      Width           =   2010
      _Version        =   65536
      _ExtentX        =   3545
      _ExtentY        =   582
      _StockProps     =   15
      ForeColor       =   65535
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
      Begin VB.TextBox Password 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   255
         IMEMode         =   3  'DISABLE
         Left            =   30
         MaxLength       =   20
         PasswordChar    =   "*"
         TabIndex        =   1
         Top             =   30
         Width           =   1950
      End
   End
   Begin Threed.SSPanel DurationPanel 
      Height          =   330
      Left            =   105
      TabIndex        =   8
      Top             =   210
      Width           =   750
      _Version        =   65536
      _ExtentX        =   1323
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
      Begin VB.TextBox Duration 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   255
         Left            =   30
         MaxLength       =   5
         TabIndex        =   9
         Text            =   "0"
         Top             =   30
         Width           =   690
      End
   End
   Begin Threed.SSPanel OptionsPanel 
      Height          =   330
      Left            =   945
      TabIndex        =   6
      Top             =   210
      Width           =   2010
      _Version        =   65536
      _ExtentX        =   3545
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
      Begin VB.ComboBox DeviceOptions 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   7
         Top             =   15
         Width           =   1980
      End
   End
   Begin VB.Label DurationLabel 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "duration"
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
      TabIndex        =   2
      Top             =   0
      Width           =   570
   End
   Begin VB.Label PasswordLabel 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "password"
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
      Left            =   1050
      TabIndex        =   3
      Top             =   525
      Width           =   750
   End
End
Attribute VB_Name = "DeviceForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Private Sub CloseButton_Click()
  HideBiModal DeviceForm
End Sub

Private Sub Duration_GotFocus()
  GoSelect duration
End Sub

Private Sub Form_Activate()
  DeviceOptions.Clear
  Select Case DevDo
    Case ReinitDevice
      Caption = "Reinitialize Device"
      DeviceOptions.AddItem "Cold Start"
      DeviceOptions.AddItem "Warm Start"
      DeviceOptions.ListIndex = 0
      OptionsPanel.visible = True
      DurationLabel.visible = False
      DurationPanel.visible = False
      password.Text = ""
      PasswordLabel.visible = True
      PasswordPanel.visible = True
     Case DeviceCommunicationControl
      Caption = "Device Communication Control"
      DeviceOptions.AddItem "Enable Device"             'Note: Enable=0 is correct for this enumeration!
      DeviceOptions.AddItem "Disable Device"
      DeviceOptions.ListIndex = 0
      OptionsPanel.visible = True
      DurationLabel.visible = True
      DurationPanel.visible = True
      password.Text = ""
      PasswordLabel.visible = True
      PasswordPanel.visible = True
    Case GetAlarmSummary
      Caption = "Get Alarm Summary"
      OptionsPanel.visible = False
      DurationLabel.visible = False
      DurationPanel.visible = False
      PasswordLabel.visible = False
      PasswordPanel.visible = False
  End Select
End Sub

Private Sub Password_GotFocus()
  GoSelect password
End Sub

Private Sub SendButton_Click()
Dim alen, apdu As String * 128
  Select Case DevDo
    Case ReinitDevice
      alen = eReinitDeviceService(apdu, DeviceOptions.ListIndex, password.Text)
    Case DeviceCommunicationControl
      alen = eDeviceCommControlService(apdu, Val(duration.Text), DeviceOptions.ListIndex, password.Text)
    Case GetAlarmSummary
      alen = eGetAlarmSummaryService(apdu)
  End Select
  SendNPDU Left$(apdu, alen), True
End Sub

