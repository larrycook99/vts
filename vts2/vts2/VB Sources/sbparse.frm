VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form SBparse 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Parsing Status"
   ClientHeight    =   2175
   ClientLeft      =   1980
   ClientTop       =   2055
   ClientWidth     =   4575
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
   LinkTopic       =   "SBparse"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2175
   ScaleWidth      =   4575
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Close"
      Height          =   372
      Left            =   3480
      TabIndex        =   10
      Top             =   1620
      Width           =   972
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "&Break"
      Default         =   -1  'True
      Height          =   372
      Left            =   2280
      TabIndex        =   5
      Top             =   1620
      Width           =   972
   End
   Begin Threed.SSPanel StatusPanel 
      Height          =   315
      Left            =   120
      TabIndex        =   8
      Top             =   1080
      Width           =   4335
      _Version        =   65536
      _ExtentX        =   7646
      _ExtentY        =   556
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
      Begin VB.ComboBox StatusList 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   9
         Top             =   15
         Width           =   4308
      End
   End
   Begin Threed.SSPanel ErrorPanel 
      Height          =   255
      Left            =   3480
      TabIndex        =   7
      Top             =   480
      Width           =   975
      _Version        =   65536
      _ExtentX        =   1720
      _ExtentY        =   450
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
      Alignment       =   3
      Autosize        =   3
   End
   Begin Threed.SSPanel LineNumber 
      Height          =   255
      Left            =   1320
      TabIndex        =   0
      Top             =   480
      Width           =   975
      _Version        =   65536
      _ExtentX        =   1720
      _ExtentY        =   450
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
      Alignment       =   3
      Autosize        =   3
   End
   Begin Threed.SSPanel ScriptFile 
      Height          =   255
      Left            =   1320
      TabIndex        =   1
      Top             =   120
      Width           =   3135
      _Version        =   65536
      _ExtentX        =   5530
      _ExtentY        =   450
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
   End
   Begin VB.Label ErrorLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Errors"
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   2760
      TabIndex        =   6
      Top             =   480
      Width           =   735
   End
   Begin VB.Label StatusLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Status"
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   840
      Width           =   1215
   End
   Begin VB.Label LineNumberLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Line number"
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   480
      Width           =   1215
   End
   Begin VB.Label ScriptFileLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Script file"
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   120
      Width           =   1215
   End
End
Attribute VB_Name = "SBparse"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub CancelButton_Click()
gCancelParse = 1 'break pressed
End Sub

Private Sub CloseButton_Click()
  gCancelParse = 2
  Unload SBparse
End Sub

Private Sub Form_Activate()
  SBparse.Refresh
End Sub

Private Sub Form_Load()
  gCancelParse = 0 'no cancel
End Sub

