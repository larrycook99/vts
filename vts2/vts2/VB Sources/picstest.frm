VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form PicsTestForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "PICS Based Testing"
   ClientHeight    =   4890
   ClientLeft      =   1350
   ClientTop       =   1905
   ClientWidth     =   7380
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
   Icon            =   "PICSTEST.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4890
   ScaleWidth      =   7380
   Begin VB.CommandButton ContButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Continue"
      Height          =   372
      Left            =   120
      TabIndex        =   12
      Top             =   4320
      Width           =   972
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Close"
      Height          =   372
      Left            =   6195
      TabIndex        =   2
      Top             =   4320
      Width           =   972
   End
   Begin VB.CommandButton StopButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Stop"
      Enabled         =   0   'False
      Height          =   372
      Left            =   1320
      TabIndex        =   1
      Top             =   4320
      Width           =   972
   End
   Begin VB.CommandButton BeginButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Begin"
      Default         =   -1  'True
      Height          =   372
      Left            =   4935
      TabIndex        =   0
      Top             =   4320
      Width           =   972
   End
   Begin Threed.SSFrame Frame3D1 
      Height          =   2895
      Left            =   120
      TabIndex        =   3
      Top             =   1200
      Width           =   7095
      _Version        =   65536
      _ExtentX        =   12515
      _ExtentY        =   5106
      _StockProps     =   14
      Caption         =   "Status"
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
      Begin VB.ListBox TestList 
         Appearance      =   0  'Flat
         Height          =   420
         Left            =   240
         TabIndex        =   4
         Top             =   600
         Width           =   6735
      End
      Begin VB.ListBox ContentList 
         Appearance      =   0  'Flat
         ForeColor       =   &H00000000&
         Height          =   420
         Left            =   240
         TabIndex        =   5
         Top             =   1440
         Width           =   6735
      End
      Begin VB.ListBox ResultList 
         Appearance      =   0  'Flat
         Height          =   420
         Left            =   240
         TabIndex        =   6
         Top             =   2280
         Width           =   6735
      End
      Begin VB.Label Label3 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Result:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   240
         TabIndex        =   10
         Top             =   2040
         Width           =   1332
      End
      Begin VB.Label Label2 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Content:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   240
         TabIndex        =   9
         Top             =   1200
         Width           =   1092
      End
      Begin VB.Label Label1 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Test:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   240
         TabIndex        =   8
         Top             =   360
         Width           =   1092
      End
   End
   Begin Threed.SSCheck LogCheck 
      Height          =   252
      Left            =   120
      TabIndex        =   7
      Top             =   480
      Width           =   2532
      _Version        =   65536
      _ExtentX        =   4471
      _ExtentY        =   450
      _StockProps     =   78
      Caption         =   "Write results to log file:"
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
      Height          =   252
      Left            =   120
      TabIndex        =   11
      Top             =   120
      Width           =   3132
      _Version        =   65536
      _ExtentX        =   5530
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
   Begin VB.Label LogNameLab 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      ForeColor       =   &H80000008&
      Height          =   312
      Left            =   2760
      TabIndex        =   13
      Top             =   480
      Width           =   2412
   End
End
Attribute VB_Name = "PicsTestForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub BeginButton_Click()
Dim e As Integer

StopButton.Enabled = True
BeginButton.Enabled = False
ContButton.Enabled = False

If (BeginButton.Caption = "&Restart") Or (TestList.ListIndex = -1) Then
  SelectFirst TestList    'select first list item                   ***222
End If

BeginButton.Caption = "&Restart"

ResultList.Clear
ContentList.Clear

If PicsTestStatusForm.visible Then 'unload output window
  Unload PicsTestStatusForm
End If

'start the PICS Tests...

gState.ActiveList = -1               'no active list
gState.SubTestNr = 0                 'start of test
gState.StopAfterErr = gStopAfterErr  'reset error counter

gState.EndTest = False
Do Until gState.EndTest
  Screen.MousePointer = HourGlass
  e = DoPicsTest(gState) 'perform one particular test
  Screen.MousePointer = Normal
  'stop after one test cycle
  If ((StopCheck.Value) Or (StopButton.Enabled = False)) Then Exit Do
Loop

StopButton.Enabled = False
If gState.EndTest Then
  ContButton.Enabled = False
Else
  ContButton.Enabled = True
End If
BeginButton.Enabled = True

End Sub

Private Sub CloseButton_Click()
HideBiModal PicsTestForm
Unload PicsTestForm
End Sub

Private Sub ContButton_Click()
Dim e As Integer

StopButton.Enabled = True
BeginButton.Enabled = False
ContButton.Enabled = False

gState.EndTest = False
Do Until gState.EndTest
  Screen.MousePointer = HourGlass
  e = DoPicsTest(gState) 'perform one particular test
  Screen.MousePointer = Normal
  'stop after one test cycle
  If ((StopCheck.Value) Or (StopButton.Enabled = False)) Then Exit Do
Loop

StopButton.Enabled = False
If gState.EndTest Then
  ContButton.Enabled = False
Else
  ContButton.Enabled = True
End If
BeginButton.Enabled = True

End Sub

Private Sub ContentList_Click()
StatusInfoClick
End Sub

Private Sub ContentList_DblClick()
Dim i, c As Integer
On Error Resume Next
PicsTestStatusForm!StatusList.Clear 'clear listbox
c = ContentList.ListCount - 1
For i = 0 To c
  PicsTestStatusForm!StatusList.AddItem ContentList.List(i)
Next i
PicsTestStatusForm.Show 0 'show modeless
PicsTestStatusForm!StatusList.ListIndex = ContentList.ListIndex   '               ***222
PicsTestStatusForm!StatusList.TopIndex = ContentList.ListIndex
gState.ActiveList = cCONTENT
On Error GoTo 0
End Sub

Private Sub Form_Load()
Dim s As String, e As Integer

Move 0, 0 'move window to the left upper corner
Screen.MousePointer = HourGlass

ContButton.Enabled = False 'no continue
StopButton.Enabled = False 'no stop
BeginButton.Enabled = True 'start here
StopCheck.Value = False
LogCheck.Value = False

gState.StopAfterErr = gStopAfterErr
gState.LogFileNr = -1 'no log file

ReDim gTestLL(0 To 0)

Select Case gState.TestNr
  Case cConsTest:    'consistency tests
    consFillStatus
  Case cRdPropTest:  'read property tests
    wrtFillStatus
  Case cWrtPropTest: 'write property tests
    wrtFillStatus
  Case cListElementTest: 'Add/Remove List Element tests
    wrtFillStatus
End Select

Screen.MousePointer = Normal
End Sub

Private Sub Form_Unload(Cancel As Integer)

Unload PicsTestStatusForm 'close test status form
Screen.MousePointer = Normal
If gState.LogFileNr > 0 Then
  Close gState.LogFileNr
End If

End Sub

Private Sub LogCheck_Click(Value As Integer)
 
 If Value = 0 Then
   'close the log file
   If gState.LogFileNr > 0 Then
     On Error Resume Next
     Close gState.LogFileNr
     On Error GoTo 0
     gState.LogFileNr = -1
     LogNameLab.Caption = ""
   End If
 Else
   'open the log file
   gDefPicsLogFile = Trim$(gDefPicsLogFile)
   gState.LogFileNr = FreeFile
   On Error GoTo OpenLogError
   Open gDefPicsLogFile For Append As gState.LogFileNr
   If (Err = 0) Then LogNameLab.Caption = gDefPicsLogTitle
   On Error GoTo 0
 End If

Exit Sub

OpenLogError:
  Dim Action As Integer
  If (gDefPicsLogFile = "") Then
    Action = PicsError(eNOLOGFILE, gDefPicsLogFile)
    LogCheck.Value = 0 'uncheck
    Exit Sub
  Else
    Action = PicsError(eOPENLOGFILE, gDefPicsLogFile)
    If Action = 0 Then
      Resume 'try again
    Else
      LogCheck.Value = 0 'no log file
      gState.LogFileNr = -1
      Exit Sub 'forget it
    End If
  End If

End Sub

Private Sub ResultList_Click()
StatusInfoClick
End Sub

Private Sub ResultList_DblClick()
Dim i, c As Integer
On Error Resume Next
PicsTestStatusForm!StatusList.Clear 'clear listbox
c = ResultList.ListCount - 1
For i = 0 To c
  PicsTestStatusForm!StatusList.AddItem ResultList.List(i)
Next i
PicsTestStatusForm.Show 0 'show modeless
PicsTestStatusForm!StatusList.ListIndex = ResultList.ListIndex    '             ***222
PicsTestStatusForm!StatusList.TopIndex = ResultList.ListIndex
gState.ActiveList = cRESULT
On Error GoTo 0
End Sub

Private Sub StopButton_Click()
  StopButton.Enabled = False
End Sub

Private Sub TestList_Click()
gState.SubTestNr = 0
GetEntryPoint
StatusInfoClick
ContButton.Enabled = True
BeginButton.Caption = "&Restart"
End Sub

Private Sub TestList_DblClick()
Dim i, c As Integer
On Error Resume Next
PicsTestStatusForm!StatusList.Clear 'clear listbox
c = TestList.ListCount - 1
For i = 0 To c
  PicsTestStatusForm!StatusList.AddItem TestList.List(i)
Next i
PicsTestStatusForm.Show 0 'show modeless
PicsTestStatusForm!StatusList.ListIndex = TestList.ListIndex    '                 ***222
PicsTestStatusForm!StatusList.TopIndex = TestList.ListIndex
gState.ActiveList = cTEST
On Error GoTo 0
End Sub

