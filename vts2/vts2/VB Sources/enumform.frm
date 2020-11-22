VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form EnumForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Enumeration"
   ClientHeight    =   1875
   ClientLeft      =   2775
   ClientTop       =   6915
   ClientWidth     =   5130
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
   ScaleHeight     =   1875
   ScaleWidth      =   5130
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   330
      Left            =   4200
      TabIndex        =   1
      Top             =   420
      Width           =   855
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   330
      Left            =   4200
      TabIndex        =   0
      Top             =   60
      Width           =   855
   End
   Begin Threed.SSPanel EnumTypePanel 
      Height          =   330
      Left            =   105
      TabIndex        =   8
      Top             =   630
      Width           =   4005
      _Version        =   65536
      _ExtentX        =   7064
      _ExtentY        =   582
      _StockProps     =   15
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
      Autosize        =   3
      Begin VB.ComboBox EnumTypeList 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   9
         Top             =   15
         Width           =   3975
      End
   End
   Begin Threed.SSPanel EnumPanel 
      Height          =   330
      Left            =   105
      TabIndex        =   5
      Top             =   1260
      Width           =   4005
      _Version        =   65536
      _ExtentX        =   7064
      _ExtentY        =   582
      _StockProps     =   15
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
      Autosize        =   3
      Begin VB.ComboBox EnumList 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   6
         Top             =   15
         Width           =   3975
      End
   End
   Begin Threed.SSPanel PropPanel 
      Height          =   225
      Left            =   1470
      TabIndex        =   3
      Top             =   105
      Visible         =   0   'False
      Width           =   855
      _Version        =   65536
      _ExtentX        =   1508
      _ExtentY        =   397
      _StockProps     =   15
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
      Autosize        =   3
      Begin VB.TextBox ProprietaryEnum 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   195
         Left            =   15
         MaxLength       =   7
         TabIndex        =   4
         Text            =   "0000000"
         Top             =   15
         Width           =   825
      End
   End
   Begin Threed.SSCheck Proprietary 
      Height          =   225
      Left            =   105
      TabIndex        =   2
      Top             =   105
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Proprietary:"
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
   Begin VB.Label Label2 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Value"
      ForeColor       =   &H00808080&
      Height          =   225
      Left            =   105
      TabIndex        =   11
      Top             =   1050
      Width           =   1695
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Enumeration Type"
      ForeColor       =   &H00808080&
      Height          =   225
      Left            =   105
      TabIndex        =   10
      Top             =   420
      Width           =   1695
   End
   Begin VB.Label EnumHex 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   -1  'True
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00808080&
      Height          =   195
      Left            =   3735
      TabIndex        =   7
      Top             =   210
      Width           =   165
   End
End
Attribute VB_Name = "EnumForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Private Sub CancelButton_Click()
  EnumValue = -1
  Hide
End Sub

Private Sub EnumList_Click()
  EnumHex.Caption = Hex$(EnumList.ListIndex)
End Sub

Private Sub EnumTypeList_Click()
  EnumPointer = EnumTypeList.ListIndex + 1
  GetEnumValues
End Sub

Private Sub Form_Activate()
  If EnumPointer <> 0 Then
    EnumTypePanel.visible = False
    label1.visible = False
    GetEnumValues
  Else
    EnumTypePanel.visible = True
    label1.visible = True
    EnumTypeList.ListIndex = -1
    EnumList.Clear
  End If
End Sub

Private Sub Form_Load()
  EnumTypeList.Clear
  EnumTypeList.AddItem "Boolean"
  EnumTypeList.AddItem "Reliability"
  EnumTypeList.AddItem "Event State"
  EnumTypeList.AddItem "Engineering Units"
  EnumTypeList.AddItem "Event Notification Type"
  EnumTypeList.AddItem "Binary Present Value"
  EnumTypeList.AddItem "Polarity"
  EnumTypeList.AddItem "Device System Status"
  EnumTypeList.AddItem "Segmentation Option"
  EnumTypeList.AddItem "Event Type"
  EnumTypeList.AddItem "File Access Method"
  EnumTypeList.AddItem "Loop Action"
  EnumTypeList.AddItem "Program Error"
  EnumTypeList.AddItem "Program State"
  EnumTypeList.AddItem "Program Change"
  EnumTypeList.AddItem "Object Type"
  EnumTypeList.AddItem "Status Flag"
  EnumTypeList.AddItem "Event Transition"
  EnumTypeList.AddItem "Limit Enable"
  EnumTypeList.AddItem "VT Class"
  EnumTypeList.AddItem "Day of Week"
  EnumTypeList.AddItem "yes/no"
  EnumTypeList.AddItem "Calendar Type"
  EnumTypeList.AddItem "Month"
  EnumTypeList.AddItem "Week of Month"
  EnumTypeList.AddItem "Any Day of Week"
  EnumTypeList.AddItem "Binary Value with Null"
  EnumTypeList.ListIndex = -1
End Sub

Private Sub GetEnumValues()
Dim nenums, i, n, ename As String * 64
  nenums = VTSAPIgetenumtable(EnumPointer, 0, EnumStart, EnumMax, ename)
  EnumList.Clear
  For i = 0 To nenums - 1
    n = VTSAPIgetenumtable(EnumPointer, i, EnumStart, EnumMax, ename)
    EnumList.AddItem ename
  Next i
  Proprietary.visible = (EnumStart <> 0)        'it's extensible
  If EnumValue < nenums Then                    'it's a normal enumeration
    Proprietary.Value = False
    EnumList.ListIndex = EnumValue
  Else                                          'it's proprietary
    If (EnumValue >= EnumStart) And (EnumValue < EnumMax) Then
      ProprietaryEnum.ForeColor = Green
    Else
      ProprietaryEnum.ForeColor = Red
    End If
    Proprietary.Value = True
    ProprietaryEnum.Text = Format$(EnumValue)
  End If
End Sub

Private Sub OKButton_Click()
Dim r As Long

  If Proprietary.Value Then                     'user wants an extended enumeration
  'Note: we can only get here if it was, in fact, an extensible enumeration
    r = Val(ProprietaryEnum.Text)
    If r < EnumStart Then
      Beep
      MsgBox "You have specified a reserved value (" + Format$(r) + ") for this enumeration. Proprietary enumeration values must be greater than or equal to " + Format$(EnumStart) + " for this enumeration!", 48, "Reserved Enumeration"
      ProprietaryEnum.SetFocus
      Exit Sub
    End If
    If r > EnumMax Then
      Beep
      MsgBox "You have specified too large of a value (" + Format$(r) + ") for this enumeration. Proprietary enumeration values must be less than " + Format$(EnumMax) + " for this enumeration!", 48, "Invalid Enumeration"
      ProprietaryEnum.SetFocus
      Exit Sub
    End If
    EnumValue = r
  Else                                          'normal enumeration
    EnumValue = EnumList.ListIndex              'these are easy
  End If
  Hide
End Sub

Private Sub Proprietary_Click(Value As Integer)
  If Value Then                         'we've been checked
    EnumPanel.visible = False
    EnumHex.Caption = ""
    Proprietary.ForeColor = Black
    ProprietaryEnum.Text = ""
    ProprietaryEnum.ForeColor = Green
    PropPanel.visible = True
  Else                                  'unchecked
    Proprietary.ForeColor = Grey
    PropPanel.visible = False
    EnumPanel.visible = True
    EnumList.ListIndex = 0              'this makes OK return easier
  End If
End Sub

Private Sub ProprietaryEnum_Change()
Dim r As Long
  r = Val(ProprietaryEnum.Text)
  If (r < EnumStart) Or (r > EnumMax) Then
    ProprietaryEnum.ForeColor = Red
  Else
    ProprietaryEnum.ForeColor = Green
  End If
End Sub

Private Sub ProprietaryEnum_GotFocus()
  ProprietaryEnum.SelStart = 0
  ProprietaryEnum.SelLength = Len(ProprietaryEnum.Text)
End Sub

