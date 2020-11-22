VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form CalForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "CalendarEntries"
   ClientHeight    =   2220
   ClientLeft      =   1815
   ClientTop       =   3750
   ClientWidth     =   6645
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
   ScaleHeight     =   2220
   ScaleWidth      =   6645
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton EditButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Edit"
      Height          =   330
      Left            =   5775
      TabIndex        =   6
      Top             =   1365
      Width           =   750
   End
   Begin VB.CommandButton DeleteButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Delete"
      Height          =   330
      Left            =   5775
      TabIndex        =   5
      Top             =   1785
      Width           =   750
   End
   Begin VB.CommandButton AddButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Add"
      Height          =   330
      Left            =   5775
      TabIndex        =   4
      Top             =   945
      Width           =   750
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   330
      Left            =   5775
      TabIndex        =   3
      Top             =   525
      Width           =   750
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   330
      Left            =   5775
      TabIndex        =   2
      Top             =   105
      Width           =   750
   End
   Begin Threed.SSPanel Panel3D1 
      Height          =   2010
      Left            =   105
      TabIndex        =   0
      Top             =   105
      Width           =   5475
      _Version        =   65536
      _ExtentX        =   9657
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
      Alignment       =   0
      Autosize        =   3
      Begin VB.ListBox Calist 
         Appearance      =   0  'Flat
         Height          =   1980
         Left            =   15
         MultiSelect     =   2  'Extended
         TabIndex        =   1
         Top             =   15
         Width           =   5445
      End
   End
End
Attribute VB_Name = "CalForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim InForm

Private Sub AddButton_Click()
Dim i, ns

  ns = -1
  For i = 0 To calist.ListCount - 1
    If calist.Selected(i) Then
      ns = i
      Exit For
    End If
  Next i
  DateString = ""
  DateForm.Move CalForm.Left + (Width - ScaleWidth), CalForm.Top + (Height - ScaleHeight)
  DateForm.Show modal
  If DateString <> "" Then
    If ns Then calist.AddItem DateString Else calist.AddItem DateString, ns
  End If
End Sub

Private Sub Calist_DblClick()
  EditButton_Click
End Sub

Private Sub CancelButton_Click()
  InForm = False
  Hide
End Sub

Private Sub DeleteButton_Click()
Dim i, ns

  ns = 0
  For i = 0 To calist.ListCount - 1
    If calist.Selected(i) Then ns = ns + 1
  Next i
  If ns > 1 Then                       'many selected
    Beep
    If MsgBox("Are you sure you want to remove" + Str$(ns) + " entries from this list?", 33, "Delete Multiple CalendarEntries") <> 1 Then Exit Sub
  End If
  i = 0
  Do While i < calist.ListCount
    If calist.Selected(i) Then calist.RemoveItem i Else i = i + 1
  Loop
End Sub

Private Sub EditButton_Click()
Dim i, ns

  ns = 0
  For i = 0 To calist.ListCount - 1
    If calist.Selected(i) Then ns = ns + 1
  Next i
  If ns = 1 Then                        'only one selected
    DateString = calist.List(calist.ListIndex)
    DateForm.Move CalForm.Left + (Width - ScaleWidth), CalForm.Top + (Height - ScaleHeight)
    DateForm.Show modal
    If DateString <> calist.List(calist.ListIndex) Then
      calist.List(calist.ListIndex) = DateString
    End If
  Else                                  'can't edit multiples or none
    Beep
  End If
End Sub

'This is a MODAL form
'in:    CalString       contains calentry| calentry|...calentry
'                       note that we depend on a space after the |
'                       each calentry can be:
'                       Date,day,month,year,dow         where month and dow are spelled, ANY is allowed
'                       Daterange,startday,month,year,dow,endday,month,year,dow
'                       Weeknday,month,weekofmonth,dow
'out:   CalString       same format is input (unchanged if cancelled)
Private Sub Form_Activate()
Dim i, j, c$
  If InForm Then Exit Sub
  InForm = True
  ParseType = ptCalist
  i = 1
  calist.Clear
  Do Until i > Len(CalString)
    j = InStr(i, CalString, "|")
    If j Then
      calist.AddItem Mid$(CalString, i, j - i)
      i = j + 2                         'skip | and space
    Else
      calist.AddItem Mid$(CalString, i)
      Exit Do
    End If
  Loop
End Sub

Private Sub Form_Click()
  calist.ListIndex = -1
End Sub

Private Sub OKButton_Click()
Dim c$, i
  CalString = ""
  For i = 0 To calist.ListCount - 1
    CalString = CalString + calist.List(i) + "| "
  Next i
  InForm = False
  Hide
End Sub

