VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form DAForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Device Address"
   ClientHeight    =   3120
   ClientLeft      =   2295
   ClientTop       =   5415
   ClientWidth     =   6930
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
   ScaleHeight     =   3120
   ScaleWidth      =   6930
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   330
      Left            =   5775
      TabIndex        =   0
      Top             =   105
      Width           =   750
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   330
      Left            =   5775
      TabIndex        =   1
      Top             =   525
      Width           =   750
   End
   Begin Threed.SSPanel ProcIDPanel 
      Height          =   255
      Left            =   3045
      TabIndex        =   38
      Top             =   105
      Visible         =   0   'False
      Width           =   2535
      _Version        =   65536
      _ExtentX        =   4471
      _ExtentY        =   450
      _StockProps     =   15
      Caption         =   "Process Identifier"
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
      Begin VB.TextBox ProcessIdentifier 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   225
         Left            =   1680
         MaxLength       =   7
         TabIndex        =   5
         Text            =   "0000001"
         Top             =   15
         Width           =   840
      End
   End
   Begin Threed.SSPanel TimePanel 
      Height          =   225
      Index           =   1
      Left            =   1890
      TabIndex        =   37
      Top             =   1890
      Width           =   1920
      _Version        =   65536
      _ExtentX        =   3387
      _ExtentY        =   397
      _StockProps     =   15
      Caption         =   " To"
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
      Begin VB.TextBox Hours 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   195
         Index           =   1
         Left            =   750
         MaxLength       =   3
         TabIndex        =   20
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
         Index           =   1
         Left            =   1035
         MaxLength       =   3
         TabIndex        =   21
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
         Index           =   1
         Left            =   1320
         MaxLength       =   3
         TabIndex        =   22
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
         Index           =   1
         Left            =   1605
         MaxLength       =   2
         TabIndex        =   23
         Text            =   "00"
         Top             =   15
         Width           =   285
      End
   End
   Begin Threed.SSPanel TimePanel 
      Height          =   225
      Index           =   0
      Left            =   1890
      TabIndex        =   36
      Top             =   1575
      Width           =   1920
      _Version        =   65536
      _ExtentX        =   3387
      _ExtentY        =   397
      _StockProps     =   15
      Caption         =   " From"
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
      Begin VB.TextBox Hundredths 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   195
         Index           =   0
         Left            =   1605
         MaxLength       =   2
         TabIndex        =   19
         Text            =   "00"
         Top             =   15
         Width           =   285
      End
      Begin VB.TextBox Seconds 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   195
         Index           =   0
         Left            =   1320
         MaxLength       =   3
         TabIndex        =   18
         Text            =   "00."
         Top             =   15
         Width           =   285
      End
      Begin VB.TextBox Minutes 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   195
         Index           =   0
         Left            =   1035
         MaxLength       =   3
         TabIndex        =   17
         Text            =   "00:"
         Top             =   15
         Width           =   285
      End
      Begin VB.TextBox Hours 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   195
         Index           =   0
         Left            =   750
         MaxLength       =   3
         TabIndex        =   16
         Text            =   "00:"
         Top             =   15
         Width           =   285
      End
   End
   Begin Threed.SSPanel SessionKeyPanel 
      Height          =   255
      Left            =   3465
      TabIndex        =   35
      Top             =   2835
      Visible         =   0   'False
      Width           =   3375
      _Version        =   65536
      _ExtentX        =   5953
      _ExtentY        =   450
      _StockProps     =   15
      Caption         =   " Session Key    0x"
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
      Begin VB.TextBox SessionKey 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         BorderStyle     =   0  'None
         ForeColor       =   &H00000080&
         Height          =   225
         Left            =   1590
         MaxLength       =   16
         TabIndex        =   2
         Text            =   "0000000000000000"
         Top             =   15
         Width           =   1725
      End
   End
   Begin Threed.SSFrame DAFrame 
      Height          =   1065
      Left            =   420
      TabIndex        =   28
      Top             =   420
      Width           =   5160
      _Version        =   65536
      _ExtentX        =   9102
      _ExtentY        =   1879
      _StockProps     =   14
      Caption         =   "Device Address"
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
      Alignment       =   1
      Begin VB.CommandButton NewAliasButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "new alias"
         Height          =   330
         Left            =   3990
         TabIndex        =   32
         TabStop         =   0   'False
         Top             =   630
         Width           =   1065
      End
      Begin Threed.SSPanel Panel3D1 
         Height          =   330
         Left            =   105
         TabIndex        =   33
         Top             =   630
         Width           =   3795
         _Version        =   65536
         _ExtentX        =   6694
         _ExtentY        =   582
         _StockProps     =   15
         Caption         =   " Alias"
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
         Begin VB.ComboBox BoundAlias 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            Height          =   315
            Left            =   525
            Sorted          =   -1  'True
            TabIndex        =   34
            TabStop         =   0   'False
            Top             =   15
            Width           =   3255
         End
      End
      Begin Threed.SSPanel NETpanel 
         Height          =   255
         Left            =   105
         TabIndex        =   31
         Top             =   315
         Width           =   1485
         _Version        =   65536
         _ExtentX        =   2619
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   " Network"
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
         Begin VB.TextBox Network 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   225
            Left            =   840
            MaxLength       =   5
            TabIndex        =   7
            Text            =   "00000"
            Top             =   15
            Width           =   630
         End
      End
      Begin Threed.SSPanel Panel3D2 
         Height          =   255
         Left            =   1680
         TabIndex        =   30
         Top             =   315
         Width           =   3375
         _Version        =   65536
         _ExtentX        =   5953
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   " MAC Address   0x"
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
         Begin VB.TextBox MACAddress 
            Appearance      =   0  'Flat
            BackColor       =   &H00C0C0C0&
            BorderStyle     =   0  'None
            ForeColor       =   &H00000080&
            Height          =   225
            Left            =   1590
            MaxLength       =   16
            TabIndex        =   8
            Text            =   "0000000000000000"
            Top             =   15
            Width           =   1725
         End
      End
   End
   Begin Threed.SSPanel InstancePanel 
      Height          =   255
      Left            =   420
      TabIndex        =   29
      Top             =   105
      Visible         =   0   'False
      Width           =   2535
      _Version        =   65536
      _ExtentX        =   4471
      _ExtentY        =   450
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
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   225
         Left            =   1680
         MaxLength       =   7
         TabIndex        =   4
         Text            =   "0000001"
         Top             =   15
         Width           =   840
      End
   End
   Begin Threed.SSCheck Transitions 
      Height          =   225
      Index           =   2
      Left            =   1890
      TabIndex        =   27
      Top             =   2835
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " to-normal"
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
   Begin Threed.SSCheck Transitions 
      Height          =   225
      Index           =   1
      Left            =   1890
      TabIndex        =   26
      Top             =   2625
      Width           =   1485
      _Version        =   65536
      _ExtentX        =   2619
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " to-fault"
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
   Begin Threed.SSCheck Transitions 
      Height          =   225
      Index           =   0
      Left            =   1890
      TabIndex        =   25
      Top             =   2415
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " to-offnormal"
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
   Begin Threed.SSCheck ConfirmedNotifications 
      Height          =   225
      Left            =   1890
      TabIndex        =   24
      Top             =   2205
      Width           =   2430
      _Version        =   65536
      _ExtentX        =   4286
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " Confirmed Notifications?"
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
   Begin Threed.SSCheck Days 
      Height          =   225
      Index           =   6
      Left            =   420
      TabIndex        =   15
      Top             =   2835
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " Sunday"
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
   Begin Threed.SSCheck Days 
      Height          =   225
      Index           =   5
      Left            =   420
      TabIndex        =   14
      Top             =   2625
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " Saturday"
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
   Begin Threed.SSCheck Days 
      Height          =   225
      Index           =   4
      Left            =   420
      TabIndex        =   13
      Top             =   2415
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " Friday"
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
   Begin Threed.SSCheck Days 
      Height          =   225
      Index           =   3
      Left            =   420
      TabIndex        =   12
      Top             =   2205
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " Thursday"
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
   Begin Threed.SSCheck Days 
      Height          =   225
      Index           =   2
      Left            =   420
      TabIndex        =   11
      Top             =   1995
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " Wednesday"
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
   Begin Threed.SSCheck Days 
      Height          =   225
      Index           =   1
      Left            =   420
      TabIndex        =   10
      Top             =   1785
      Width           =   1485
      _Version        =   65536
      _ExtentX        =   2619
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " Tuesday"
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
   Begin Threed.SSCheck Days 
      Height          =   225
      Index           =   0
      Left            =   420
      TabIndex        =   9
      Top             =   1575
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   " Monday"
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
   Begin Threed.SSOption DevAddrOption 
      Height          =   255
      Left            =   105
      TabIndex        =   6
      TabStop         =   0   'False
      Top             =   735
      Visible         =   0   'False
      Width           =   225
      _Version        =   65536
      _ExtentX        =   397
      _ExtentY        =   450
      _StockProps     =   78
      Caption         =   "Option3D1"
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
   Begin Threed.SSOption DevInstOption 
      Height          =   255
      Left            =   105
      TabIndex        =   3
      Top             =   105
      Visible         =   0   'False
      Width           =   225
      _Version        =   65536
      _ExtentX        =   397
      _ExtentY        =   450
      _StockProps     =   78
      Caption         =   "Option3D1"
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
Attribute VB_Name = "DAForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim InForm, nheight

Private Sub BoundAlias_Click()
Dim d$, l$, n$
  l$ = BoundAlias.Text
  d$ = NxtC(l$, Chr$(9))
  n$ = NxtC(l$, Chr$(9))
  Network.Text = n$
  MACAddress.Text = l$
  BoundAlias.Text = d$
End Sub

Private Sub BoundAlias_GotFocus()
  GoSelect BoundAlias
End Sub

Private Sub BoundAlias_KeyPress(KeyAscii As Integer)
  If KeyAscii = 13 Then
    KeyAscii = 0
    NewAliasButton_Click
  End If
End Sub

Private Sub CancelButton_Click()
  InForm = False
  Hide
End Sub

Private Sub DeviceInstance_GotFocus()
  GoSelect DeviceInstance
End Sub

'This is a MODAL form
'in:    ParseType       is the parse type ptSkeys,ptTsrecip,ptDabind, or ptReciplist
'       DAString        contains the parameters for each type summarized below:
'         ptSkeys       SessionKey,network,macaddress
'         ptTsrecip     Device,deviceinstance
'               or      Address,macaddress
'         ptDabind      Device,deviceinstance,Address,macaddress
'         ptReciplist   Recipient,(daysof week),(hh:mm:ss.dd,hh:mm:ss.dd),Device,deviceinstance,procid,confirmed,(transitions)
'               or      Recipient,(daysof week),(hh:mm:ss.dd,hh:mm:ss.dd),Address,macaddress,procid,confirmed,(transitions)
'out:   DAString        modified
Private Sub Form_Activate()
Dim i, j, d$, c$, isinst As Boolean                       '               ***218

  If InForm Then Exit Sub
  InForm = True
  If DevInstOption.Value Then isinst = True Else isinst = False '         ***218
  DevInstOption.visible = False
  DevAddrOption.visible = False
  InstancePanel.visible = False
  SessionKeyPanel.visible = False
  ProcIDPanel.visible = False
  i = (Height - ScaleHeight) + TimePanel(0).Top
  d$ = DAString
  BoundAlias.ListIndex = -1
  BoundAlias.Text = ""
  Select Case ParseType
  Case ptSkeys
    Caption = "Session Key"
    SessionKeyPanel.visible = True
    DAFrame.Caption = "Peer Address"
    Height = i
    If (d$ = "") Or (Nxt(d$) <> "SessionKey") Then      'default initial values
      SessionKey.Text = "0000000000000000"
      Network.Text = "00000"
      MACAddress.Text = "0000000000000000"
    Else                                                'parse for initial values
      SessionKey.Text = Nxt(d$)
      Network.Text = Nxt(d$)
      MACAddress.Text = d$
    End If
  Case ptTsrecip, ptRecip
    Select Case ParseType
      Case ptTsrecip
        Caption = "Time Synch Recipient"
      Case ptRecip
        Caption = "Recipient"
    End Select
    DAFrame.Caption = "Device Address"
    InstancePanel.visible = True
    DevInstOption.visible = True
    DevAddrOption.visible = True
    Height = i
    DeviceInstance.Text = "0000001"
    Network.Text = "00000"
    MACAddress.Text = "0000000000000000"
    c$ = Nxt(d$)
    If c$ = "Device" Then
      DeviceInstance.Text = Nxt(d$)
      DevInstOption.Value = True
    ElseIf c$ = "Address" Then
      Network.Text = Nxt(d$)
      MACAddress.Text = d$
      DevAddrOption.Value = True
    Else
      DoEvents
      DevInstOption.Value = isinst
      DevAddrOption.Value = Not isinst
    End If
  Case ptDabind
    Caption = "Device Address Binding"
    DAFrame.Caption = "Device Address"
    InstancePanel.visible = True
    Height = i
    DeviceInstance.Text = "0000001"
    Network.Text = "00000"
    MACAddress.Text = "0000000000000000"
    c$ = Nxt(d$)
    If c$ = "Device" Then DeviceInstance.Text = Nxt(d$)
    c$ = Nxt(d$)
    If c$ = "Address" Then
      Network.Text = Nxt(d$)
      MACAddress.Text = d$
    End If
  Case ptReciplist
    Caption = "Recipient"
    DAFrame.Caption = "Receiving Device"
    ProcIDPanel.visible = True
    InstancePanel.visible = True
    DevInstOption.visible = True
    DevAddrOption.visible = True
    Height = nheight
    ProcessIdentifier.Text = "0000001"
    DeviceInstance.Text = "0000001"
    Network.Text = "00000"
    MACAddress.Text = "0000000000000000"
    If Nxt(d$) = "Recipient" Then
      d$ = Mid$(d$, 2)                                  'skip (
      Do
        c$ = Nxt(d$)
        If Right$(c$, 1) = ")" Then
          j = True
          c$ = Left$(c$, Len(c$) - 1)
        Else
          j = False
        End If
        If c$ <> "" Then
          For i = 0 To 6
            If LTrim$(Days(i).Caption) = c$ Then
              Days(i).Value = True
              Exit For
            End If
          Next i
        End If
      Loop Until j
      d$ = Mid$(d$, 2)                                  'skip (
      Hours(0).Text = NxtC(d$, ":") + ":"
      Minutes(0).Text = NxtC(d$, ":") + ":"
      Seconds(0).Text = NxtC(d$, ".") + "."
      Hundredths(0).Text = Nxt(d$)
      Hours(1).Text = NxtC(d$, ":") + ":"
      Minutes(1).Text = NxtC(d$, ":") + ":"
      Seconds(1).Text = NxtC(d$, ".") + "."
      Hundredths(1).Text = NxtC(d$, "),")
      c$ = Nxt(d$)
      If c$ = "Device" Then
        DeviceInstance.Text = Nxt(d$)
        DevInstOption.Value = True
      ElseIf c$ = "Address" Then
        Network.Text = Nxt(d$)
        MACAddress.Text = d$
        DevAddrOption.Value = True
      End If
      ProcessIdentifier.Text = Nxt(d$)
      ConfirmedNotifications.Value = (Nxt(d$) = "Yes")
      d$ = Mid$(d$, 2)                                  'skip (
      Do
        c$ = Nxt(d$)
        If Right$(c$, 1) = ")" Then
          j = True
          c$ = Left$(c$, Len(c$) - 1)
        Else
          j = False
        End If
        If c$ <> "" Then
          For i = 0 To 2
            If LTrim$(Transitions(i).Caption) = c$ Then
              Transitions(i).Value = True
              Exit For
            End If
          Next i
        End If
      Loop Until j
    End If
  Case Else
    InForm = False
    Hide
  End Select
End Sub

Private Sub Form_Load()
  nheight = Height
  SessionKeyPanel.Move InstancePanel.Left, InstancePanel.Top
  ReadAliases BoundAlias, BoundAliasFile
End Sub

Private Sub Hours_Change(Index As Integer)
  If IsNumeric(Hours(Index).Text) Then
    If Val(Hours(Index).Text) > 23 Then
      Beep
      Hours(Index).ForeColor = Red
      Exit Sub
    End If
  End If
  Hours(Index).ForeColor = Green
End Sub

Private Sub Hours_GotFocus(Index As Integer)
  GoSelect Hours(Index)
End Sub

Private Sub Hours_KeyPress(Index As Integer, KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    Hours(Index).Text = "??:"
    Minutes(Index).SetFocus
  End If
End Sub

Private Sub Hours_LostFocus(Index As Integer)
  If Right$(Hours(Index).Text, 1) <> ":" Then Hours(Index).Text = Hours(Index).Text + ":"
End Sub

Private Sub Hundredths_GotFocus(Index As Integer)
  GoSelect Hundredths(Index)
End Sub

Private Sub Hundredths_KeyPress(Index As Integer, KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    Hundredths(Index).Text = "??"
  End If
End Sub

Private Sub MACAddress_GotFocus()
  GoSelect MACAddress
End Sub

Private Sub Minutes_Change(Index As Integer)
  If IsNumeric(Minutes(Index).Text) Then
    If Val(Minutes(Index).Text) > 59 Then
      Beep
      Minutes(Index).ForeColor = Red
      Exit Sub
    End If
  End If
  Minutes(Index).ForeColor = Green
End Sub

Private Sub Minutes_GotFocus(Index As Integer)
  GoSelect Minutes(Index)
End Sub

Private Sub Minutes_KeyPress(Index As Integer, KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    Minutes(Index).Text = "??:"
    Seconds(Index).SetFocus
  End If
End Sub

Private Sub Minutes_LostFocus(Index As Integer)
  If Right$(Minutes(Index).Text, 1) <> ":" Then Minutes(Index).Text = Minutes(Index).Text + ":"
End Sub

Private Sub Network_GotFocus()
  GoSelect Network
End Sub

Private Sub NewAliasButton_Click()
  If BoundAlias.Text = "" Then
    Beep
  Else
    BoundAlias.AddItem BoundAlias.Text + Chr$(9) + Network.Text + Chr$(9) + MACAddress.Text
    SaveAliases BoundAlias, BoundAliasFile
  End If
End Sub

Private Sub OKButton_Click()
Dim i
  Select Case ParseType
    Case ptSkeys
      DAString = "SessionKey," + String$(16 - Len(SessionKey.Text), "0") + SessionKey.Text + "," + Network.Text + "," + MACAddress.Text
    Case ptTsrecip, ptRecip
      If DevInstOption.Value Then
        DAString = "Device," + DeviceInstance.Text
      Else
        DAString = "Address," + Network.Text + "," + MACAddress.Text
      End If
    Case ptDabind
        DAString = "Device," + DeviceInstance.Text + ",Address," + Network.Text + "," + MACAddress.Text
    Case ptReciplist
      DAString = "Recipient,("
      For i = 0 To 6
        If Days(i).Value Then DAString = DAString + LTrim$(Days(i).Caption) + ","
      Next i
      If Right$(DAString, 1) = "," Then DAString = Left$(DAString, Len(DAString) - 1)
      DAString = DAString + "),(" + Hours(0).Text + Minutes(0).Text + Seconds(0).Text + Hundredths(0).Text + "," + Hours(1).Text + Minutes(1).Text + Seconds(1).Text + Hundredths(1).Text + "),"
      If DevInstOption.Value Then
        DAString = DAString + "Device," + DeviceInstance.Text
      Else
        DAString = DAString + "Address," + Network.Text + "," + MACAddress.Text
      End If
      DAString = DAString + "," + ProcessIdentifier.Text + ","
      If ConfirmedNotifications.Value Then DAString = DAString + "Yes,(" Else DAString = DAString + "No,("
      For i = 0 To 2
        If Transitions(i).Value Then DAString = DAString + LTrim$(Transitions(i).Caption) + ","
      Next i
      If Right$(DAString, 1) = "," Then DAString = Left$(DAString, Len(DAString) - 1)
      DAString = DAString + ")"
  End Select
  InForm = False
  Hide
End Sub

Private Sub ProcessIdentifier_GotFocus()
  GoSelect ProcessIdentifier
End Sub

Private Sub Seconds_Change(Index As Integer)
  If IsNumeric(Seconds(Index).Text) Then
    If Val(Seconds(Index).Text) > 59 Then
      Beep
      Seconds(Index).ForeColor = Red
      Exit Sub
    End If
  End If
  Seconds(Index).ForeColor = Green
End Sub

Private Sub Seconds_GotFocus(Index As Integer)
  GoSelect Seconds(Index)
End Sub

Private Sub Seconds_KeyPress(Index As Integer, KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    Seconds(Index).Text = "??."
    Hundredths(Index).SetFocus
  End If
End Sub

Private Sub Seconds_LostFocus(Index As Integer)
  If Right$(Seconds(Index).Text, 1) <> "." Then Seconds(Index).Text = Seconds(Index).Text + "."
End Sub

Private Sub SessionKey_GotFocus()
  GoSelect SessionKey
End Sub

