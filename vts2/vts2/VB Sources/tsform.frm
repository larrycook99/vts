VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form TimeSynchForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Time Synchronization"
   ClientHeight    =   1200
   ClientLeft      =   45
   ClientTop       =   375
   ClientWidth     =   5025
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
   ScaleHeight     =   1200
   ScaleWidth      =   5025
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   372
      Left            =   4080
      TabIndex        =   13
      Top             =   600
      Width           =   816
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   372
      Left            =   4080
      TabIndex        =   14
      Top             =   120
      Width           =   816
   End
   Begin Threed.SSFrame FromFrame 
      Height          =   1065
      Left            =   105
      TabIndex        =   0
      Top             =   0
      Width           =   3795
      _Version        =   65536
      _ExtentX        =   6694
      _ExtentY        =   1879
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
      Begin Threed.SSPanel TimePanel 
         Height          =   225
         Left            =   2535
         TabIndex        =   15
         Top             =   210
         Width           =   1155
         _Version        =   65536
         _ExtentX        =   2037
         _ExtentY        =   397
         _StockProps     =   15
         ForeColor       =   16777215
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
         Begin VB.TextBox Hours 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   195
            Left            =   15
            MaxLength       =   3
            TabIndex        =   19
            Text            =   "00:"
            Top             =   15
            Width           =   285
         End
         Begin VB.TextBox Minutes 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   195
            Left            =   300
            MaxLength       =   3
            TabIndex        =   17
            Text            =   "00:"
            Top             =   15
            Width           =   285
         End
         Begin VB.TextBox Seconds 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   195
            Left            =   585
            MaxLength       =   3
            TabIndex        =   16
            Text            =   "00."
            Top             =   15
            Width           =   285
         End
         Begin VB.TextBox Hundredths 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   195
            Left            =   870
            MaxLength       =   2
            TabIndex        =   18
            Text            =   "00"
            Top             =   15
            Width           =   285
         End
      End
      Begin Threed.SSPanel Panel3D4 
         Height          =   330
         Left            =   2940
         TabIndex        =   7
         Top             =   630
         Width           =   750
         _Version        =   65536
         _ExtentX        =   1323
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
         Begin VB.ComboBox DowList 
            Appearance      =   0  'Flat
            Height          =   300
            Left            =   12
            Style           =   2  'Dropdown List
            TabIndex        =   8
            Top             =   12
            Width           =   720
         End
      End
      Begin Threed.SSPanel Panel3D1 
         Height          =   330
         Left            =   105
         TabIndex        =   5
         Top             =   630
         Width           =   750
         _Version        =   65536
         _ExtentX        =   1323
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
         Begin VB.ComboBox DayList 
            Appearance      =   0  'Flat
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   6
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel Panel3D2 
         Height          =   330
         Left            =   945
         TabIndex        =   3
         Top             =   630
         Width           =   1275
         _Version        =   65536
         _ExtentX        =   2249
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
         Begin VB.ComboBox MonthList 
            Appearance      =   0  'Flat
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   4
            Top             =   15
            Width           =   1245
         End
      End
      Begin Threed.SSPanel Panel3D3 
         Height          =   315
         Left            =   2310
         TabIndex        =   1
         Top             =   630
         Width           =   540
         _Version        =   65536
         _ExtentX        =   953
         _ExtentY        =   556
         _StockProps     =   15
         ForeColor       =   8421504
         BackColor       =   -2147483633
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.26
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         BevelOuter      =   1
         Alignment       =   0
         Begin VB.TextBox YearBox 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   288
            Left            =   15
            MaxLength       =   4
            TabIndex        =   2
            Text            =   "1995"
            Top             =   15
            Width           =   516
         End
      End
      Begin VB.Label Label4 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "weekday"
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
         Left            =   2940
         TabIndex        =   12
         Top             =   420
         Width           =   750
      End
      Begin VB.Label Label3 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "year"
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
         Left            =   2310
         TabIndex        =   11
         Top             =   420
         Width           =   300
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "month"
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
         Left            =   945
         TabIndex        =   10
         Top             =   420
         Width           =   1065
      End
      Begin VB.Label Label1 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "day"
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
         Left            =   105
         TabIndex        =   9
         Top             =   420
         Width           =   540
      End
   End
End
Attribute VB_Name = "TimeSynchForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Private Sub CheckDayandDow()
Dim CheckDate As Variant, dow
  DowList.ForeColor = Black
    'a real day was selected and we need to check if DowList is valid for that day
      CheckDate = DateSerial(Val(YearBox.Text), MonthList.ListIndex + 1, DayList.ListIndex + 1)
      dow = WeekDay(CheckDate) - 1
      If dow = 0 Then dow = 7           'converts it to 1=Monday, 7=Sunday (The BACnet way)
      If dow <> (DowList.ListIndex + 1) Then
        Beep
        DowList.ForeColor = Red         'complain, but let him do it anyway
      End If
End Sub

Private Sub CheckForLeapYear()
Dim i, j, y, sday
  Select Case MonthList.ListIndex
'    Case 0, 1, 3, 5, 7, 8, 10, 12      '           ***219 Begin
    Case 0, 2, 4, 6, 7, 9, 11
      j = 31
'    Case 4, 6, 9, 11
    Case 3, 5, 8, 10
      j = 30
'    Case 2
    Case 1                              '           ***219 End
      y = Val(YearBox.Text)
      If ((y Mod 400) = 0) Or (((y Mod 4) = 0) And ((y Mod 100) <> 0)) Then j = 29 Else j = 28
  End Select
  sday = DayList.ListIndex
  If sday < 0 Then sday = 0
  DayList.Clear
  For i = 1 To j
    DayList.AddItem Format$(i)
  Next i
  If sday > j Then
    Beep
    MsgBox "You've switched to a month which doesn't have the day" + Str$(sday) + ", so the day has been changed to 1!", 48, "Invalid Day for New Month"
    sday = 0
    DayList.SetFocus
  End If
  DayList.ListIndex = sday
End Sub

'Validate the date
'out:   true if user cancels due to error
'       false if ok
Private Function CheckYearBox() As Integer
Dim y
  If Not IsNumeric(YearBox.Text) Then
    Beep
    MsgBox "You must specify a year!", 48, "No Year Specified"
    YearBox.SetFocus
    CheckYearBox = True
    Exit Function
  End If
  y = Val(YearBox.Text)
  If y < 100 Then y = y + 1900
  If (y < 1900) Or (y > 2154) Then
    Beep
    MsgBox "The year specified for " + Caption + " (" + YearBox.Text + ") is not a valid year in BACnet encoding!", 48, "Invalid Year"
    YearBox.SetFocus
    CheckYearBox = True                  'force cancel
    Exit Function
  End If
  If DowList.ForeColor = Red Then        'not really the correct day of week
    Beep
    If MsgBox("The day of week specified for " + Caption + " (" + DowList.Text + ") is not correct for " + MonthList.Text + " " + DayList.Text + "," + Format$(y) + "! Although this date can be encoded in BACnet, it can never occur. Are you sure that you want to use this date?", 33, "Impossible Date") <> 1 Then
      YearBox.SetFocus
      CheckYearBox = True                'force cancel
      Exit Function
    End If
  End If
  YearBox.Text = Format$(y)              'leave it as a 19xx type of format
  CheckYearBox = False
End Function

Private Sub CloseButton_Click()
  HideBiModal TimeSynchForm
End Sub

Private Sub DayList_Click()
  CheckDayandDow
End Sub

Private Sub DowList_Click()
  CheckDayandDow
End Sub

Private Sub Form_Activate()
Dim n As Variant, dow
  n = Now
  Hours.Text = Format$(n, "hh:")
  Minutes.Text = Format$(n, "nn:")
  Seconds.Text = Format$(n, "ss.")
  Hundredths.Text = "00"
  MonthList.ListIndex = Month(n) - 1
  CheckForLeapYear                            '       ***219
  DayList.ListIndex = Day(n) - 1
  dow = WeekDay(n) - 2
  If dow = -1 Then dow = 6
  DowList.ListIndex = dow
  YearBox.Text = Format$(n, "yyyy")
End Sub

Private Sub Form_Load()
    MonthList.Clear
    MonthList.AddItem "January"
    MonthList.AddItem "February"
    MonthList.AddItem "March"
    MonthList.AddItem "April"
    MonthList.AddItem "May"
    MonthList.AddItem "June"
    MonthList.AddItem "July"
    MonthList.AddItem "August"
    MonthList.AddItem "September"
    MonthList.AddItem "October"
    MonthList.AddItem "November"
    MonthList.AddItem "December"
    MonthList.ListIndex = 0
    DowList.Clear
    DowList.AddItem "Mon"
    DowList.AddItem "Tue"
    DowList.AddItem "Wed"
    DowList.AddItem "Thu"
    DowList.AddItem "Fri"
    DowList.AddItem "Sat"
    DowList.AddItem "Sun"
    DowList.ListIndex = 0
End Sub

Private Sub Hours_Change()
  If IsNumeric(Hours.Text) Then
    If Val(Hours.Text) > 23 Then
      Beep
      Hours.ForeColor = Red
      Exit Sub
    End If
  End If
  Hours.ForeColor = Green
End Sub

Private Sub Hours_GotFocus()
 GoSelect Hours
End Sub

Private Sub Hours_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    Hours.Text = "??:"
    Minutes.SetFocus
  End If
End Sub

Private Sub Hundredths_GotFocus()
  GoSelect Hundredths
End Sub

Private Sub Hundredths_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    Hundredths.Text = "??"
  End If
End Sub

Private Sub Minutes_Change()
  If IsNumeric(Minutes.Text) Then
    If Val(Minutes.Text) > 59 Then
      Beep
      Minutes.ForeColor = Red
      Exit Sub
    End If
  End If
  Minutes.ForeColor = Green
End Sub

Private Sub Minutes_GotFocus()
  GoSelect Minutes
End Sub

Private Sub Minutes_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    Minutes.Text = "??:"
    Seconds.SetFocus
  End If
End Sub

Private Sub MonthList_Click()
  CheckForLeapYear
End Sub

Private Sub Seconds_Change()
  If IsNumeric(Seconds.Text) Then
    If Val(Seconds.Text) > 59 Then
      Beep
      Seconds.ForeColor = Red
      Exit Sub
    End If
  End If
  Seconds.ForeColor = Green
End Sub

Private Sub Seconds_GotFocus()
  GoSelect Seconds
End Sub

Private Sub Seconds_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    Seconds.Text = "??."
    Hundredths.SetFocus
  End If
End Sub

Private Sub SendButton_Click()
Dim alen, apdu As String * 128
  If CheckYearBox() Then Exit Sub
  alen = eTimeSynchService(apdu, Val(YearBox.Text), MonthList.ListIndex + 1, DayList.ListIndex + 1, DowList.ListIndex + 1, Val(Hours.Text), Val(Minutes.Text), Val(Seconds.Text), Val(Hundredths.Text))
  SendNPDU Left$(apdu, alen), False
End Sub

Private Sub YearBox_Change()
Dim y
  YearBox.ForeColor = Green
  If IsNumeric(YearBox.Text) Then
    y = Val(YearBox.Text)
    If (y > 2154) Or ((y > 99) And (y < 1900)) Then
      Beep
      YearBox.ForeColor = Red
    End If
  Else
    YearBox.ForeColor = Red
  End If
  CheckForLeapYear
End Sub

Private Sub YearBox_GotFocus()
  GoSelect YearBox
End Sub

