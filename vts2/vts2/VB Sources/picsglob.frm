VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form PicsGlobalForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "PICS Setup"
   ClientHeight    =   4365
   ClientLeft      =   2460
   ClientTop       =   2175
   ClientWidth     =   5400
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
   Icon            =   "PICSGLOB.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4365
   ScaleWidth      =   5400
   Begin VB.TextBox TimeText 
      Height          =   288
      Left            =   1680
      TabIndex        =   17
      Top             =   1440
      Width           =   972
   End
   Begin VB.Timer TimeoutTimer 
      Enabled         =   0   'False
      Interval        =   250
      Left            =   4860
      Top             =   900
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Cancel"
      Height          =   312
      Left            =   4260
      TabIndex        =   2
      Top             =   540
      Width           =   972
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Activate"
      Height          =   312
      Left            =   4260
      TabIndex        =   1
      Top             =   120
      Width           =   972
   End
   Begin Threed.SSFrame Frame3D3 
      Height          =   2235
      Left            =   120
      TabIndex        =   9
      Top             =   1980
      Width           =   5115
      _Version        =   65536
      _ExtentX        =   9022
      _ExtentY        =   3942
      _StockProps     =   14
      Caption         =   "PICS Parameter"
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
      Begin VB.CheckBox FilterCheck 
         Caption         =   "filter PICS trace window"
         Height          =   192
         Left            =   2280
         TabIndex        =   10
         Top             =   420
         Width           =   2412
      End
      Begin VB.TextBox StopAfterText 
         Height          =   288
         Left            =   1080
         TabIndex        =   14
         Top             =   360
         Width           =   372
      End
      Begin VB.TextBox F3Text 
         Height          =   312
         Left            =   3600
         TabIndex        =   18
         Top             =   1680
         Width           =   972
      End
      Begin VB.TextBox F2Text 
         Height          =   312
         Left            =   2460
         TabIndex        =   19
         Top             =   1680
         Width           =   972
      End
      Begin VB.TextBox F1Text 
         Height          =   312
         Left            =   1320
         TabIndex        =   7
         Top             =   1680
         Width           =   972
      End
      Begin VB.TextBox U3Text 
         Height          =   312
         Left            =   3600
         TabIndex        =   4
         Top             =   1140
         Width           =   972
      End
      Begin VB.TextBox U2Text 
         Height          =   312
         Left            =   2460
         TabIndex        =   3
         Top             =   1140
         Width           =   972
      End
      Begin VB.TextBox U1Text 
         Height          =   312
         Left            =   1320
         TabIndex        =   0
         Top             =   1140
         Width           =   972
      End
      Begin VB.Label Label6 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "PICS Test Values:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1320
         TabIndex        =   8
         Top             =   840
         Width           =   1692
      End
      Begin VB.Label Label4 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Real:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   300
         TabIndex        =   12
         Top             =   1740
         Width           =   912
      End
      Begin VB.Label Label3 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Unsigned:"
         ForeColor       =   &H00000000&
         Height          =   252
         Left            =   120
         TabIndex        =   13
         Top             =   1200
         Width           =   1032
      End
      Begin VB.Label Label5 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "errors"
         ForeColor       =   &H00000000&
         Height          =   252
         Left            =   1500
         TabIndex        =   16
         Top             =   420
         Width           =   792
      End
      Begin VB.Label Label2 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Stop after:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   15
         Top             =   420
         Width           =   912
      End
   End
   Begin Threed.SSFrame Frame3D1 
      Height          =   1035
      Left            =   60
      TabIndex        =   5
      Top             =   120
      Width           =   4035
      _Version        =   65536
      _ExtentX        =   7117
      _ExtentY        =   1826
      _StockProps     =   14
      Caption         =   "Log File"
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
      Begin VB.CommandButton SelectButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "&Select"
         Height          =   312
         Left            =   1380
         TabIndex        =   6
         Top             =   630
         Width           =   972
      End
      Begin VB.TextBox LogFileText 
         BackColor       =   &H00000000&
         ForeColor       =   &H0000C000&
         Height          =   288
         Left            =   120
         TabIndex        =   11
         Top             =   300
         Width           =   3732
      End
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "TimeOut (ms):"
      ForeColor       =   &H80000008&
      Height          =   192
      Left            =   120
      TabIndex        =   20
      Top             =   1500
      Width           =   1392
   End
End
Attribute VB_Name = "PicsGlobalForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub CancelButton_Click()
  Unload PicsGlobalForm
End Sub

Private Sub FilterCheck_Click()
OkButton.Enabled = True
CancelButton.Caption = "&Cancel"
End Sub

Private Sub Form_Load()
  OkButton.Enabled = False
  TimeoutTimer.Interval = gPicsInterval
  TimeText.Text = CStr(gPicsInterval)
  StopAfterText.Text = CStr(gStopAfterErr)
  FilterCheck.Value = gPicsFilter
  LogFileText.Text = gDefPicsLogFile
  U1Text = CStr(gU(0))
  U2Text = CStr(gU(1))
  U3Text = CStr(gU(2))
  F1Text = CStr(gF(0))
  F2Text = CStr(gF(1))
  F3Text = CStr(gF(2))
End Sub

Private Sub LogFileText_Change()
OkButton.Enabled = True
CancelButton.Caption = "&Cancel"
End Sub

Private Sub OKButton_Click()
Dim e As Integer

OkButton.Enabled = False

On Error Resume Next
  gPicsInterval = CInt(TimeText.Text)
  If gPicsInterval <= 0 Then gPicsInterval = 1
  gStopAfterErr = CInt(StopAfterText.Text)
  If gStopAfterErr <= 0 Then gStopAfterErr = 1
  gU(0) = CInt(U1Text)
  gU(1) = CInt(U2Text)
  gU(2) = CInt(U3Text)
  gF(0) = CSng(F1Text)
  gF(1) = CSng(F2Text)
  gF(2) = CSng(F3Text)
On Error GoTo 0

TimeoutTimer.Interval = gPicsInterval
gPicsFilter = FilterCheck.Value

gDefPicsLogFile = LogFileText.Text
gDefPicsLogFile = Trim$(gDefPicsLogFile)
CutFilePath gDefPicsLogTitle, gDefPicsLogFile

CancelButton.Caption = "&Close"

End Sub

Private Sub SelectButton_Click()

On Error GoTo GlobSelCancelErr

 'get new PicsFile
 VTSform.CMD.FileName = gDefPicsLogFile
 VTSform.CMD.DialogTitle = "Select Log File"
 VTSform.CMD.Filter = "Log Files (*.log)|*.log|All Files (*.*)|*.*"
 VTSform.CMD.DefaultExt = "*.log"
 'File And Path Must Exist, Hides Read Only Box
 VTSform.CMD.Flags = &H4
 'Open Common Dialog
 VTSform.CMD.Action = 1

 'Store FileName
 LogFileText.Text = VTSform.CMD.FileName
 CancelButton.Caption = "&Cancel"
 Exit Sub

GlobSelCancelErr:
  Exit Sub
  
End Sub

Private Sub StopAfterText_Change()
OkButton.Enabled = True
CancelButton.Caption = "&Cancel"
End Sub

Private Sub TimeoutTimer_Timer()
'you're time is over
TimeoutTimer.Enabled = False 'stop timer
gPicsTimeOut = True
End Sub

Private Sub TimeText_Change()
OkButton.Enabled = True
CancelButton.Caption = "&Cancel"
End Sub

