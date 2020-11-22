VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form PicsSetupForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Read PICS"
   ClientHeight    =   1260
   ClientLeft      =   1905
   ClientTop       =   2145
   ClientWidth     =   5520
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
   Icon            =   "PICSSET.frx":0000
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1260
   ScaleWidth      =   5520
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   372
      Left            =   4140
      TabIndex        =   2
      Top             =   720
      Width           =   972
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&OK"
      Height          =   372
      Left            =   3000
      TabIndex        =   1
      Top             =   720
      Width           =   972
   End
   Begin VB.CommandButton SelectButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Select"
      Default         =   -1  'True
      Height          =   372
      Left            =   420
      TabIndex        =   0
      Top             =   720
      Width           =   972
   End
   Begin Threed.SSPanel PicsFilePanel 
      Height          =   255
      Left            =   1500
      TabIndex        =   3
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
      Caption         =   "PICS File:"
      ForeColor       =   &H80000008&
      Height          =   252
      Left            =   240
      TabIndex        =   4
      Top             =   300
      Width           =   1092
   End
End
Attribute VB_Name = "PicsSetupForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub CancelButton_Click()
  Unload PicsSetupForm
End Sub

Private Sub Form_Load()
PicsFilePanel.Caption = gDefPicsFile 'set last PICS filename
End Sub

Private Sub OKButton_Click()
Dim e As Integer, Action As Integer, c As Integer, Msg As String

'try to load new object database
e = LoadObjDB(CStr(PicsFilePanel.Caption))
If e <> eNOERROR Then
  Action = PicsError(e, " ")
Else
  Msg = Space$(80)
  c = LoadString(3000, Msg, 80) 'PICS file: ~1 parsed successfully!
  ReplaceText 1, Msg, gDefPicsTitle
  PicsOkForm!PicsOkLab.Caption = Msg
  PicsOkForm.Show 1 'modal
End If

If gDBloaded Then
  VTSform!PicsTestMenu.Enabled = True
  VTSform!PicsObjDBMenu.Enabled = True
  VTSform!PicsMiscMenu.Enabled = True
  VTSform!PicsPInfoMenu.Enabled = True
  If PicsMiscForm.visible Then
    Unload PicsMiscForm
  End If
  If PicsPropInfoForm.visible Then
    Unload PicsPropInfoForm
  End If
  If PicsObjDBForm.visible Then
    Unload PicsObjDBForm
  End If
Else
  VTSform!PicsTestMenu.Enabled = False
  VTSform!PicsObjDBMenu.Enabled = False
  VTSform!PicsMiscMenu.Enabled = False
  VTSform!PicsPInfoMenu.Enabled = False
End If


Unload PicsSetupForm
End Sub

Private Sub SelectButton_Click()

  On Error GoTo ChangeCancel
  ' get new PicsFile
  VTSform.CMD.FileName = gDefPicsFile
  VTSform.CMD.DialogTitle = "Select PICS File"
  VTSform.CMD.Filter = "PICS Files (*.tpi)|*.tpi|All Files (*.*)|*.*"
  VTSform.CMD.DefaultExt = "*.tpi"
  ' File And Path Must Exist, Hides Read Only Box
  VTSform.CMD.Flags = &H1804
  ' Open Common Dialog
  VTSform.CMD.Action = 1
  On Error GoTo 0

  ' Store FileName
  PicsFilePanel.Caption = VTSform.CMD.FileName
  Exit Sub
  
ChangeCancel:
  ' Cancel Button pressed
  Exit Sub

End Sub

