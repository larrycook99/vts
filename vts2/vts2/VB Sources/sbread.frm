VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form SBReadForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   Caption         =   "Select Script File"
   ClientHeight    =   1275
   ClientLeft      =   1575
   ClientTop       =   4635
   ClientWidth     =   5565
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
   Icon            =   "SBREAD.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1275
   ScaleWidth      =   5565
   Begin VB.CommandButton SelectButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Select"
      Default         =   -1  'True
      Height          =   372
      Left            =   540
      TabIndex        =   3
      Top             =   720
      Width           =   972
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&OK"
      Height          =   372
      Left            =   3120
      TabIndex        =   2
      Top             =   720
      Width           =   972
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   372
      Left            =   4260
      TabIndex        =   1
      Top             =   720
      Width           =   972
   End
   Begin Threed.SSPanel FilePanel 
      Height          =   255
      Left            =   1620
      TabIndex        =   0
      Top             =   300
      Width           =   3615
      _Version        =   65536
      _ExtentX        =   6376
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
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Script File:"
      ForeColor       =   &H80000008&
      Height          =   252
      Left            =   180
      TabIndex        =   4
      Top             =   300
      Width           =   1272
   End
End
Attribute VB_Name = "SBReadForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub CancelButton_Click()
Unload SBReadForm
End Sub

Private Sub Form_Load()
  FilePanel.Caption = DefScriptFile
End Sub

Private Sub OKButton_Click()
  If (Len(FilePanel.Caption) > 0) Then
    'Store FileName
    DefScriptFile = FilePanel
    'Adjust to a new DefLogFile
    AdjustLogFile DefScriptFile, gDefPicsLogFile
    VTSform!TestSBTests.Enabled = True
    Unload SBReadForm
  End If
End Sub

Private Sub SelectButton_Click()
  On Error GoTo ChangeCancel
  ' get new ScriptFile
  VTSform.CMD.FileName = DefScriptFile
  VTSform.CMD.DialogTitle = "Select Script File"
  VTSform.CMD.Filter = "VTS Script Files|*.vts|All Files|*.*"
  VTSform.CMD.DefaultExt = "*.VTS"
  ' File And Path Must Exist, Hides Read Only Box
  VTSform.CMD.Flags = &H1804
  ' Open Common Dialog
  VTSform.CMD.Action = 1
  On Error GoTo 0

  FilePanel.Caption = VTSform.CMD.FileName

ChangeCancel:
  ' Cancel Button pressed
  Exit Sub

End Sub

