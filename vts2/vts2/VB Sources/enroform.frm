VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form EnrollForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Get Enrollment Summary"
   ClientHeight    =   4935
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5625
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
   ScaleHeight     =   4935
   ScaleWidth      =   5625
   ShowInTaskbar   =   0   'False
   Begin VB.ComboBox ETFilter 
      Appearance      =   0  'Flat
      Height          =   300
      Left            =   2730
      Style           =   2  'Dropdown List
      TabIndex        =   21
      Top             =   2940
      Visible         =   0   'False
      Width           =   2745
   End
   Begin VB.ComboBox ESFilter 
      Appearance      =   0  'Flat
      Height          =   300
      Left            =   2730
      Style           =   2  'Dropdown List
      TabIndex        =   19
      Top             =   2520
      Visible         =   0   'False
      Width           =   2745
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   420
      Left            =   4515
      TabIndex        =   1
      Top             =   105
      Width           =   960
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   420
      Left            =   4515
      TabIndex        =   0
      Top             =   735
      Width           =   960
   End
   Begin Threed.SSFrame NotificationClassFilterFrame 
      Height          =   750
      Left            =   315
      TabIndex        =   31
      Top             =   4095
      Visible         =   0   'False
      Width           =   4080
      _Version        =   65536
      _ExtentX        =   7197
      _ExtentY        =   1323
      _StockProps     =   14
      Caption         =   "Notification Class Filter"
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
      Begin Threed.SSPanel Panel3D6 
         Height          =   255
         Left            =   210
         TabIndex        =   24
         Top             =   315
         Width           =   2745
         _Version        =   65536
         _ExtentX        =   4842
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "Notification Class"
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
         Begin VB.TextBox NotificationClass 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   225
            Left            =   2100
            MaxLength       =   5
            TabIndex        =   25
            Text            =   "00000"
            Top             =   0
            Width           =   630
         End
      End
   End
   Begin Threed.SSFrame PriorityFilterFrame 
      Height          =   750
      Left            =   315
      TabIndex        =   23
      Top             =   3255
      Visible         =   0   'False
      Width           =   4080
      _Version        =   65536
      _ExtentX        =   7197
      _ExtentY        =   1323
      _StockProps     =   14
      Caption         =   "Priority Filter"
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
      Begin Threed.SSPanel Panel3D4 
         Height          =   255
         Left            =   1680
         TabIndex        =   28
         Top             =   315
         Width           =   1275
         _Version        =   65536
         _ExtentX        =   2249
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "Maximum"
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
         Begin VB.TextBox MaxPriority 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   225
            Left            =   840
            MaxLength       =   3
            TabIndex        =   29
            Text            =   "255"
            Top             =   15
            Width           =   420
         End
      End
      Begin Threed.SSPanel Panel3D3 
         Height          =   255
         Left            =   210
         TabIndex        =   26
         Top             =   315
         Width           =   1275
         _Version        =   65536
         _ExtentX        =   2249
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "Minimum"
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
         Begin VB.TextBox MinPriority 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   225
            Left            =   840
            MaxLength       =   3
            TabIndex        =   27
            Text            =   "000"
            Top             =   15
            Width           =   420
         End
      End
   End
   Begin Threed.SSFrame EnrollmentFilterFrame 
      Height          =   1800
      Left            =   315
      TabIndex        =   6
      Top             =   630
      Visible         =   0   'False
      Width           =   4080
      _Version        =   65536
      _ExtentX        =   7197
      _ExtentY        =   3175
      _StockProps     =   14
      Caption         =   "Enrollment Filter"
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
      Begin Threed.SSPanel Panel3D1 
         Height          =   315
         Left            =   105
         TabIndex        =   16
         Top             =   1365
         Width           =   2850
         _Version        =   65536
         _ExtentX        =   5027
         _ExtentY        =   556
         _StockProps     =   15
         Caption         =   " Process Identifier"
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
         Begin VB.TextBox ProcessID 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   285
            Left            =   1680
            TabIndex        =   17
            Text            =   "1"
            Top             =   15
            Width           =   1140
         End
      End
      Begin Threed.SSPanel Panel3D2 
         Height          =   255
         Left            =   420
         TabIndex        =   14
         Top             =   1050
         Width           =   3375
         _Version        =   65536
         _ExtentX        =   5953
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "MAC Address    0x"
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
         Begin VB.TextBox SourceMACAddress 
            Appearance      =   0  'Flat
            BackColor       =   &H00C0C0C0&
            BorderStyle     =   0  'None
            ForeColor       =   &H00000080&
            Height          =   225
            Left            =   1590
            MaxLength       =   16
            TabIndex        =   15
            Text            =   "0000000000000000"
            Top             =   15
            Visible         =   0   'False
            Width           =   1725
         End
      End
      Begin Threed.SSPanel SNETpanel 
         Height          =   255
         Left            =   420
         TabIndex        =   12
         Top             =   735
         Width           =   1485
         _Version        =   65536
         _ExtentX        =   2619
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "Network"
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
         Begin VB.TextBox SourceNetwork 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   225
            Left            =   840
            MaxLength       =   5
            TabIndex        =   13
            Text            =   "00000"
            Top             =   15
            Visible         =   0   'False
            Width           =   630
         End
      End
      Begin Threed.SSPanel InstancePanel 
         Height          =   315
         Left            =   420
         TabIndex        =   7
         Top             =   315
         Width           =   2535
         _Version        =   65536
         _ExtentX        =   4471
         _ExtentY        =   556
         _StockProps     =   15
         Caption         =   " Device Instance"
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
            Left            =   1680
            MaxLength       =   7
            TabIndex        =   8
            Text            =   "0000001"
            Top             =   15
            Width           =   840
         End
      End
      Begin Threed.SSOption RecipIsAddress 
         Height          =   225
         Left            =   105
         TabIndex        =   11
         TabStop         =   0   'False
         Top             =   735
         Width           =   225
         _Version        =   65536
         _ExtentX        =   397
         _ExtentY        =   397
         _StockProps     =   78
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
      Begin Threed.SSOption RecipIsDevice 
         Height          =   225
         Left            =   105
         TabIndex        =   9
         Top             =   345
         Width           =   225
         _Version        =   65536
         _ExtentX        =   397
         _ExtentY        =   397
         _StockProps     =   78
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
   End
   Begin Threed.SSFrame Frame3D1 
      Height          =   540
      Left            =   105
      TabIndex        =   2
      Top             =   0
      Width           =   4320
      _Version        =   65536
      _ExtentX        =   7620
      _ExtentY        =   953
      _StockProps     =   14
      Caption         =   "Acknowledgement Filter"
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
      Begin Threed.SSOption AckFilter 
         Height          =   225
         Index           =   2
         Left            =   1890
         TabIndex        =   5
         TabStop         =   0   'False
         Top             =   210
         Width           =   1275
         _Version        =   65536
         _ExtentX        =   2249
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Not ACKed"
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
      Begin Threed.SSOption AckFilter 
         Height          =   225
         Index           =   1
         Left            =   945
         TabIndex        =   4
         TabStop         =   0   'False
         Top             =   210
         Width           =   960
         _Version        =   65536
         _ExtentX        =   1693
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "ACKed"
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
      Begin Threed.SSOption AckFilter 
         Height          =   225
         Index           =   0
         Left            =   210
         TabIndex        =   3
         Top             =   210
         Width           =   645
         _Version        =   65536
         _ExtentX        =   1138
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "All"
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
   End
   Begin Threed.SSCheck UseNotificationClassFilter 
      Height          =   225
      Left            =   105
      TabIndex        =   30
      Top             =   4200
      Width           =   3690
      _Version        =   65536
      _ExtentX        =   6509
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Use Notification Class Filter?"
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
   Begin Threed.SSCheck UsePriorityFilter 
      Height          =   225
      Left            =   105
      TabIndex        =   22
      Top             =   3360
      Width           =   2430
      _Version        =   65536
      _ExtentX        =   4286
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Use Priority Filter?"
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
   Begin Threed.SSCheck UseEventTypeFilter 
      Height          =   225
      Left            =   105
      TabIndex        =   20
      Top             =   3000
      Width           =   2430
      _Version        =   65536
      _ExtentX        =   4286
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Use Event Type Filter?"
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
   Begin Threed.SSCheck UseEventStateFilter 
      Height          =   225
      Left            =   105
      TabIndex        =   18
      Top             =   2580
      Width           =   2430
      _Version        =   65536
      _ExtentX        =   4286
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Use Event State Filter?"
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
   Begin Threed.SSCheck UseEnrollmentFilter 
      Height          =   225
      Left            =   105
      TabIndex        =   10
      Top             =   735
      Width           =   2430
      _Version        =   65536
      _ExtentX        =   4286
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Use Enrollment Filter?"
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
End
Attribute VB_Name = "EnrollForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Dim WhichAck

Private Sub AckFilter_Click(Index As Integer, Value As Integer)
  If Value Then WhichAck = Index
End Sub

Private Sub CloseButton_Click()
  HideBiModal EnrollForm
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
  WhichAck = 0
  ESFilter.Clear
  ESFilter.AddItem "offnormal"
  ESFilter.AddItem "fault"
  ESFilter.AddItem "normal"
  ESFilter.AddItem "all"
  ESFilter.AddItem "active"
  ESFilter.ListIndex = 3
  ETFilter.Clear
  ETFilter.AddItem "change-of-bitstring"
  ETFilter.AddItem "change-of-state"
  ETFilter.AddItem "change-of-value"
  ETFilter.AddItem "command-failure"
  ETFilter.AddItem "floating-limit"
  ETFilter.AddItem "out-of-range"
  ETFilter.ListIndex = 0
End Sub

Private Sub MaxPriority_Change()
Dim i
  i = Val(MaxPriority.Text)
  If (i < 0) Or (i > 255) Then MaxPriority.ForeColor = Red Else MaxPriority.ForeColor = Green
End Sub

Private Sub MaxPriority_GotFocus()
  GoSelect MaxPriority
End Sub

Private Sub MinPriority_Change()
Dim i
  i = Val(MinPriority.Text)
  If (i < 0) Or (i > 255) Then MinPriority.ForeColor = Red Else MinPriority.ForeColor = Green
End Sub

Private Sub MinPriority_GotFocus()
  GoSelect MinPriority
End Sub

Private Sub NotificationClass_GotFocus()
  GoSelect NotificationClass
End Sub

Private Sub ProcessID_GotFocus()
  GoSelect ProcessID
End Sub

Private Sub RecipIsAddress_Click(Value As Integer)
  If Value Then
    DeviceInstance.visible = False
    SourceNetwork.visible = True
    SourceMACAddress.visible = True
  End If
End Sub

Private Sub RecipIsDevice_Click(Value As Integer)
  If Value Then
    DeviceInstance.visible = True
    SourceNetwork.visible = False
    SourceMACAddress.visible = False
  End If
End Sub

Private Sub SendButton_Click()
Dim apdu As String * 128, alen, i, j, es, et, pfn, pfx, nf
Dim devinst&, net As Long, ProcID&, u$, mlen, maddr As String * 8
  If UseEnrollmentFilter.Value Then
    If RecipIsAddress.Value Then
      If Val(SourceNetwork.Text) < 1 Then
         Beep
         MsgBox "The Network must be >0!", 48, "Bad Network Number"
         SourceNetwork.SetFocus
         Exit Sub
      End If
      devinst = -2
      net = Val(SourceNetwork.Text)
      j = Len(SourceMACAddress.Text)
      If j And 1 Then
        j = j + 1
        u$ = "0" + SourceMACAddress.Text
      Else
        u$ = SourceMACAddress.Text
      End If
      mlen = j \ 2
      For i = 1 To j \ 2
        Mid$(maddr, i, 1) = Chr$(Val("&h" + Mid$(u$, 1 + ((i - 1) * 2), 2)))
      Next i
    Else
      devinst = Val(DeviceInstance.Text)
      If (devinst& < MinDeviceInstance) Or (devinst& > 4194303) Then
        Beep
        MsgBox "The Device Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid Device Instance"
        DeviceInstance.SetFocus
        Exit Sub
      End If
    End If
    If ProcessID.Text = "" Then
      Beep
      MsgBox "You must specify a Process Identifier!", 48, "No Process Identifier"
      ProcessID.SetFocus
      Exit Sub
    End If
    ProcID = Val(ProcessID.Text)
  Else
    devinst = -1
  End If
  If UseEventStateFilter.Value Then es = ESFilter.ListIndex Else es = -1
  If UseEventTypeFilter.Value Then et = ETFilter.ListIndex Else et = -1
  If UsePriorityFilter.Value Then
    pfn = Val(MinPriority.Text)
    pfx = Val(MaxPriority.Text)
    If pfn > pfx Then
      Beep
      MsgBox "The Minimum priority is greater than the Maximum priority!", 48, "Min/Max Reversed?"
      MinPriority.SetFocus
      Exit Sub
    End If
  Else
    pfn = -1
    pfx = -1
  End If
  If UseNotificationClassFilter.Value Then nf = Val(NotificationClass.Text) Else nf = -1
  alen = eGetEnrollmentSummaryService(apdu, WhichAck, devinst, net, mlen, maddr, ProcID, es, et, pfn, pfx, nf)
  SendNPDU Left$(apdu, alen), True
End Sub

Private Sub SourceMACAddress_GotFocus()
  GoSelect SourceMACAddress
End Sub

Private Sub SourceNetwork_GotFocus()
  GoSelect SourceNetwork
End Sub

Private Sub UseEnrollmentFilter_Click(Value As Integer)
  EnrollmentFilterFrame.visible = Value
End Sub

Private Sub UseEventStateFilter_Click(Value As Integer)
  ESFilter.visible = Value
End Sub

Private Sub UseEventTypeFilter_Click(Value As Integer)
  ETFilter.visible = Value
End Sub

Private Sub UseNotificationClassFilter_Click(Value As Integer)
  NotificationClassFilterFrame.visible = Value
End Sub

Private Sub UsePriorityFilter_Click(Value As Integer)
  PriorityFilterFrame.visible = Value
End Sub

