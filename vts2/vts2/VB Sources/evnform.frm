VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form EventForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Event Notification"
   ClientHeight    =   6180
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5460
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
   ScaleHeight     =   6180
   ScaleWidth      =   5460
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   330
      Left            =   4410
      TabIndex        =   12
      Top             =   1470
      Width           =   960
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   330
      Left            =   4410
      TabIndex        =   11
      Top             =   1050
      Width           =   960
   End
   Begin Threed.SSPanel nsPanel 
      Height          =   330
      Left            =   1890
      TabIndex        =   64
      Top             =   6615
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   582
      _StockProps     =   15
      Caption         =   "new-state"
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
      Begin VB.ComboBox NewState 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   1365
         Style           =   2  'Dropdown List
         TabIndex        =   65
         Top             =   15
         Width           =   2115
      End
   End
   Begin Threed.SSPanel psPanel 
      Height          =   330
      Left            =   1890
      TabIndex        =   62
      Top             =   6195
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   582
      _StockProps     =   15
      Caption         =   "property-states"
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
      Begin VB.ComboBox PropertyStates 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   1365
         Style           =   2  'Dropdown List
         TabIndex        =   63
         Top             =   15
         Width           =   2115
      End
   End
   Begin Threed.SSPanel valuePanel 
      Height          =   330
      Index           =   2
      Left            =   1890
      TabIndex        =   22
      Top             =   5775
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   582
      _StockProps     =   15
      Caption         =   "exceeding-value"
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
      Begin VB.TextBox valueBox 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   285
         Index           =   2
         Left            =   1890
         MaxLength       =   10
         TabIndex        =   23
         Text            =   "0"
         Top             =   0
         Width           =   1575
      End
   End
   Begin Threed.SSPanel valuePanel 
      Height          =   330
      Index           =   1
      Left            =   1890
      TabIndex        =   24
      Top             =   5355
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   582
      _StockProps     =   15
      Caption         =   "exceeding-value"
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
      Begin VB.TextBox valueBox 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   285
         Index           =   1
         Left            =   1890
         MaxLength       =   10
         TabIndex        =   25
         Text            =   "0"
         Top             =   0
         Width           =   1575
      End
   End
   Begin Threed.SSPanel valuePanel 
      Height          =   330
      Index           =   0
      Left            =   1890
      TabIndex        =   26
      Top             =   4935
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
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
      Alignment       =   1
      Begin VB.TextBox valueBox 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   285
         Index           =   0
         Left            =   1890
         MaxLength       =   10
         TabIndex        =   32
         Text            =   "0"
         Top             =   0
         Width           =   1575
      End
      Begin Threed.SSOption ChangedValue 
         Height          =   225
         Index           =   1
         Left            =   840
         TabIndex        =   67
         TabStop         =   0   'False
         Top             =   60
         Visible         =   0   'False
         Width           =   855
         _Version        =   65536
         _ExtentX        =   1508
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "value"
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
      Begin Threed.SSOption ChangedValue 
         Height          =   225
         Index           =   0
         Left            =   60
         TabIndex        =   66
         Top             =   60
         Visible         =   0   'False
         Width           =   615
         _Version        =   65536
         _ExtentX        =   1085
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "bits"
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
   Begin Threed.SSFrame statusflagsFrame 
      Height          =   1275
      Left            =   105
      TabIndex        =   59
      Top             =   4830
      Visible         =   0   'False
      Width           =   1695
      _Version        =   65536
      _ExtentX        =   2990
      _ExtentY        =   2249
      _StockProps     =   14
      Caption         =   "status-flags"
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
      Begin Threed.SSCheck statusFlags 
         Height          =   225
         Index           =   3
         Left            =   105
         TabIndex        =   39
         Top             =   945
         Width           =   1485
         _Version        =   65536
         _ExtentX        =   2619
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "out-of-service"
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
      Begin Threed.SSCheck statusFlags 
         Height          =   225
         Index           =   2
         Left            =   105
         TabIndex        =   52
         Top             =   735
         Width           =   1485
         _Version        =   65536
         _ExtentX        =   2619
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "overridden"
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
      Begin Threed.SSCheck statusFlags 
         Height          =   225
         Index           =   1
         Left            =   105
         TabIndex        =   61
         Top             =   525
         Width           =   1485
         _Version        =   65536
         _ExtentX        =   2619
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "fault"
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
      Begin Threed.SSCheck statusFlags 
         Height          =   225
         Index           =   0
         Left            =   105
         TabIndex        =   60
         Top             =   315
         Width           =   1485
         _Version        =   65536
         _ExtentX        =   2619
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "in-alarm"
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
   Begin Threed.SSPanel Panel3D9 
      Height          =   330
      Left            =   3885
      TabIndex        =   56
      Top             =   4515
      Width           =   1485
      _Version        =   65536
      _ExtentX        =   2619
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
      Alignment       =   1
      Begin VB.ComboBox toState 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   0
         Style           =   2  'Dropdown List
         TabIndex        =   57
         Top             =   15
         Width           =   1470
      End
   End
   Begin Threed.SSPanel Panel3D8 
      Height          =   330
      Left            =   1890
      TabIndex        =   54
      Top             =   4515
      Width           =   1485
      _Version        =   65536
      _ExtentX        =   2619
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
      Alignment       =   1
      Begin VB.ComboBox fromState 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   0
         Style           =   2  'Dropdown List
         TabIndex        =   55
         Top             =   15
         Width           =   1470
      End
   End
   Begin Threed.SSPanel Panel3D7 
      Height          =   960
      Left            =   105
      TabIndex        =   50
      Top             =   3465
      Width           =   5265
      _Version        =   65536
      _ExtentX        =   9287
      _ExtentY        =   1693
      _StockProps     =   15
      Caption         =   "messageText:"
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
      Begin VB.TextBox messageText 
         Appearance      =   0  'Flat
         BackColor       =   &H00FFC0C0&
         BorderStyle     =   0  'None
         Height          =   690
         Left            =   30
         MultiLine       =   -1  'True
         TabIndex        =   51
         Top             =   240
         Width           =   5205
      End
   End
   Begin Threed.SSPanel Panel3D3 
      Height          =   330
      Left            =   2730
      TabIndex        =   48
      Top             =   525
      Width           =   2640
      _Version        =   65536
      _ExtentX        =   4657
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
      Alignment       =   1
      Autosize        =   3
      Begin VB.ComboBox NotifyType 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   49
         Top             =   15
         Width           =   2610
      End
   End
   Begin Threed.SSPanel Panel3D1 
      Height          =   330
      Left            =   105
      TabIndex        =   46
      Top             =   3045
      Width           =   2640
      _Version        =   65536
      _ExtentX        =   4657
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
      Alignment       =   1
      Autosize        =   3
      Begin VB.ComboBox EventType 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   47
         Top             =   15
         Width           =   2610
      End
   End
   Begin Threed.SSPanel Panel3D5 
      Height          =   330
      Left            =   2730
      TabIndex        =   44
      Top             =   105
      Width           =   1275
      _Version        =   65536
      _ExtentX        =   2249
      _ExtentY        =   582
      _StockProps     =   15
      Caption         =   " Priority"
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
      Begin VB.TextBox Priority 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   285
         Left            =   840
         MaxLength       =   3
         TabIndex        =   45
         Text            =   "000"
         Top             =   15
         Width           =   420
      End
   End
   Begin Threed.SSPanel Panel3D6 
      Height          =   330
      Left            =   2835
      TabIndex        =   42
      Top             =   3045
      Width           =   2535
      _Version        =   65536
      _ExtentX        =   4471
      _ExtentY        =   582
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
         ForeColor       =   &H0000FF00&
         Height          =   285
         Left            =   1890
         MaxLength       =   5
         TabIndex        =   43
         Text            =   "00000"
         Top             =   0
         Width           =   630
      End
   End
   Begin Threed.SSPanel InstancePanel 
      Height          =   315
      Left            =   105
      TabIndex        =   40
      Top             =   525
      Width           =   2535
      _Version        =   65536
      _ExtentX        =   4471
      _ExtentY        =   556
      _StockProps     =   15
      Caption         =   " Initiating Device"
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
      Alignment       =   1
      Begin VB.TextBox DeviceInstance 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   285
         Left            =   1680
         MaxLength       =   7
         TabIndex        =   41
         Text            =   "0000001"
         Top             =   0
         Width           =   840
      End
   End
   Begin Threed.SSFrame ObjIDFrame 
      Height          =   855
      Left            =   105
      TabIndex        =   33
      Top             =   945
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   1508
      _StockProps     =   14
      Caption         =   "eventObjectIdentifier"
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
         Height          =   345
         Left            =   105
         TabIndex        =   35
         Top             =   420
         Width           =   3060
         _Version        =   65536
         _ExtentX        =   5398
         _ExtentY        =   609
         _StockProps     =   15
         ForeColor       =   8421504
         BackColor       =   0
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
         Autosize        =   3
         Begin VB.ComboBox ObjectType 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   315
            Left            =   15
            Sorted          =   -1  'True
            TabIndex        =   1
            Top             =   15
            Width           =   3030
         End
      End
      Begin Threed.SSPanel Panel3D2 
         Height          =   330
         Left            =   3255
         TabIndex        =   34
         Top             =   420
         Width           =   855
         _Version        =   65536
         _ExtentX        =   1508
         _ExtentY        =   582
         _StockProps     =   15
         ForeColor       =   8421504
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
         Begin VB.TextBox ObjectInstance 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   240
            Left            =   30
            MaxLength       =   7
            TabIndex        =   2
            Text            =   "0"
            Top             =   75
            Width           =   795
         End
      End
      Begin VB.Label Label4 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "instance"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   3255
         TabIndex        =   38
         Top             =   210
         Width           =   840
      End
      Begin VB.Label Label2 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "object type"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   210
         TabIndex        =   37
         Top             =   210
         Width           =   960
      End
      Begin VB.Label ObjIDHex 
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
         Left            =   2625
         TabIndex        =   36
         Top             =   210
         Width           =   165
      End
   End
   Begin Threed.SSPanel ProcPanel 
      Height          =   330
      Left            =   105
      TabIndex        =   31
      Top             =   105
      Width           =   2535
      _Version        =   65536
      _ExtentX        =   4471
      _ExtentY        =   582
      _StockProps     =   15
      Caption         =   " Process Identifier "
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
      Begin VB.TextBox ProcID 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         ForeColor       =   &H0000FF00&
         Height          =   285
         Left            =   1680
         MaxLength       =   10
         TabIndex        =   0
         Text            =   "0"
         Top             =   0
         Width           =   840
      End
   End
   Begin Threed.SSFrame tsFrame 
      Height          =   1065
      Left            =   105
      TabIndex        =   13
      Top             =   1890
      Width           =   5265
      _Version        =   65536
      _ExtentX        =   9287
      _ExtentY        =   1879
      _StockProps     =   14
      Caption         =   "timeStamp"
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
         Left            =   2625
         TabIndex        =   27
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
            TabIndex        =   3
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
            TabIndex        =   4
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
            TabIndex        =   5
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
            TabIndex        =   6
            Text            =   "00"
            Top             =   15
            Width           =   285
         End
      End
      Begin Threed.SSPanel DowPanel 
         Height          =   330
         Left            =   3045
         TabIndex        =   17
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
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   10
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel DayPanel 
         Height          =   330
         Left            =   105
         TabIndex        =   16
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
            TabIndex        =   7
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel MonthPanel 
         Height          =   330
         Left            =   945
         TabIndex        =   15
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
            TabIndex        =   8
            Top             =   15
            Width           =   1245
         End
      End
      Begin Threed.SSPanel YearPanel 
         Height          =   330
         Left            =   2310
         TabIndex        =   14
         Top             =   630
         Width           =   645
         _Version        =   65536
         _ExtentX        =   1138
         _ExtentY        =   582
         _StockProps     =   15
         ForeColor       =   8421504
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
         Begin VB.TextBox YearBox 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   270
            Left            =   30
            MaxLength       =   4
            TabIndex        =   9
            Text            =   "1995"
            Top             =   45
            Width           =   585
         End
      End
      Begin Threed.SSOption ChoiceIsDateTime 
         Height          =   225
         Left            =   3885
         TabIndex        =   30
         Top             =   630
         Width           =   1275
         _Version        =   65536
         _ExtentX        =   2249
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "date/time"
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
      Begin Threed.SSOption ChoiceIsSequence 
         Height          =   225
         Left            =   3885
         TabIndex        =   29
         TabStop         =   0   'False
         Top             =   420
         Width           =   1275
         _Version        =   65536
         _ExtentX        =   2249
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "sequence"
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
      Begin Threed.SSOption ChoiceIsTime 
         Height          =   225
         Left            =   3885
         TabIndex        =   28
         TabStop         =   0   'False
         Top             =   210
         Width           =   1275
         _Version        =   65536
         _ExtentX        =   2249
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "time"
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
      Begin VB.Label DowLabel 
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
         Left            =   3045
         TabIndex        =   21
         Top             =   420
         Width           =   750
      End
      Begin VB.Label YearLabel 
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
         TabIndex        =   20
         Top             =   420
         Width           =   300
      End
      Begin VB.Label MonthLabel 
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
         TabIndex        =   19
         Top             =   420
         Width           =   1065
      End
      Begin VB.Label DayLabel 
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
         TabIndex        =   18
         Top             =   420
         Width           =   540
      End
   End
   Begin Threed.SSCheck conf 
      Height          =   225
      Left            =   4095
      TabIndex        =   68
      Top             =   165
      Width           =   1275
      _Version        =   65536
      _ExtentX        =   2249
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Confirmed?"
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
      Alignment       =   1
   End
   Begin Threed.SSCheck ACKrequired 
      Height          =   225
      Left            =   105
      TabIndex        =   53
      Top             =   4515
      Width           =   1590
      _Version        =   65536
      _ExtentX        =   2805
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "ACK required?"
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
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "F"
      BeginProperty Font 
         Name            =   "Wingdings"
         Size            =   18
         Charset         =   2
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   330
      Left            =   3465
      TabIndex        =   58
      Top             =   4515
      Width           =   330
   End
End
Attribute VB_Name = "EventForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim evstate, objtype, nheight

Private Sub CheckDayandDow()
Dim CheckDate As Variant, dow
  DowList.ForeColor = Black
    If (DayList.ListIndex > 0) And (MonthList.ListIndex > 0) And IsNumeric(YearBox.Text) And (DowList.ListIndex > 0) Then
    'a real day was selected and we need to check if DowList is valid for that day
      CheckDate = DateSerial(Val(YearBox.Text), MonthList.ListIndex, DayList.ListIndex)
      dow = WeekDay(CheckDate) - 1
      If dow = 0 Then dow = 7           'converts it to 1=Monday, 7=Sunday (The BACnet way)
      If dow <> DowList.ListIndex Then
        Beep
        DowList.ForeColor = Red  'complain, but let him do it anyway
      End If
    End If
End Sub

Private Sub CheckForLeapYear()
Dim i, j, y, sday
  Select Case MonthList.ListIndex
    Case 0, 1, 3, 5, 7, 8, 10, 12
      j = 31
    Case 4, 6, 9, 11
      j = 30
    Case 2
      y = Val(YearBox.Text)
      If ((y Mod 400) = 0) Or (((y Mod 4) = 0) And ((y Mod 100) <> 0)) Then j = 29 Else j = 28
  End Select
  sday = DayList.ListIndex
  If sday < 0 Then sday = 0
  DayList.Clear
  DayList.AddItem "Any"
  For i = 1 To j
    DayList.AddItem Format$(i)
  Next i
  If sday > j Then
    Beep
    MsgBox "You've switched to a month which doesn't have the day" + Str$(sday) + ", so the day has been changed to <Any>!", 48, "Invalid Day for New Month"
    sday = 0
    DayList.SetFocus
  End If
  DayList.ListIndex = sday
End Sub

'Validate the date for Index
'out:   true if user cancels due to error
'       false if ok
Private Function CheckYearBox() As Integer
Dim y As Long, f$
  If YearBox.Text = "Any" Then
    CheckYearBox = False
    Exit Function
  End If
  y = Val(YearBox.Text)
  f$ = tsFrame.Caption
  If ChoiceIsSequence.Value Then
    If (y < 0) Or (y > 65535) Then
      Beep
      MsgBox "The sequence number specified for " + f$ + " (" + YearBox.Text + ") must be in the range 0..65535!", 48, "Invalid Sequence Number"
      CheckYearBox = True
    Else
      CheckYearBox = False
    End If
    Exit Function
  End If
  If y < 100 Then y = y + 1900
  If (y < 1900) Or (y > 2154) Then
    Beep
    MsgBox "The year specified for " + f$ + " (" + YearBox.Text + ") is not a valid year in BACnet encoding!", 48, "Invalid Year"
    CheckYearBox = True                         'force cancel
    Exit Function
  End If
  If DowList.ForeColor = Red Then        'not really the correct day of week
    Beep
    If MsgBox("The day of week specified for " + f$ + " (" + DowList.Text + ") is not correct for " + MonthList.Text + " " + DayList.Text + "," + Format$(y) + "! Although this date can be encoded in BACnet, it can never occur. Are you sure that you want to use this date?", 33, "Impossible Date") <> 1 Then
      CheckYearBox = True                         'force cancel
      Exit Function
    End If
  End If
  YearBox.Text = Format$(y)              'leave it as a 19xx type of format
  CheckYearBox = False
End Function

Private Sub ChoiceIsDateTime_Click(Value As Integer)
  If Value Then
    TimePanel.visible = True
    DayLabel.visible = True
    DayPanel.visible = True
    MonthLabel.visible = True
    MonthPanel.visible = True
    DowLabel.visible = True
    DowPanel.visible = True
    YearLabel.visible = True
    YearPanel.visible = True
    YearLabel.Caption = "year"
    YearBox.MaxLength = 4
  End If
End Sub

Private Sub ChoiceIsSequence_Click(Value As Integer)
  If Value Then
    TimePanel.visible = False
    DayLabel.visible = False
    DayPanel.visible = False
    MonthLabel.visible = False
    MonthPanel.visible = False
    DowLabel.visible = False
    DowPanel.visible = False
    YearLabel.visible = True
    YearPanel.visible = True
    YearLabel.Caption = "sequence"
    YearBox.MaxLength = 5
  End If
End Sub

Private Sub ChoiceIsTime_Click(Value As Integer)
  If Value Then
    TimePanel.visible = True
    DayLabel.visible = False
    DayPanel.visible = False
    MonthLabel.visible = False
    MonthPanel.visible = False
    DowLabel.visible = False
    DowPanel.visible = False
    YearLabel.visible = False
    YearPanel.visible = False
  End If
End Sub

Private Sub CloseButton_Click()
  HideBiModal EventForm
End Sub

Private Sub DayList_Click()
  CheckDayandDow
End Sub

Private Sub DeviceInstance_GotFocus()
  GoSelect DeviceInstance
End Sub

Private Sub DowList_Click()
  CheckDayandDow
End Sub

Private Sub EventType_Click()

  valuePanel(0).visible = False
  valuePanel(1).visible = False
  valuePanel(2).visible = False
  psPanel.visible = False
  nsPanel.visible = False
  ChangedValue(0).visible = False
  ChangedValue(1).visible = False
  Select Case EventType.ListIndex
    Case 0                                      'change-of-bitstring
      statusflagsFrame.visible = True
      valuePanel(0).Caption = "referenced-bitstring"
      valueBox(0).Text = ""
      valuePanel(0).visible = True
    Case 1                                      'change-of-state
      statusflagsFrame.visible = True
      psPanel.visible = True
      nsPanel.visible = True
    Case 2                                      'change-of-value
      statusflagsFrame.visible = True
      valuePanel(0).Caption = ""
      valueBox(0).Text = ""
      ChangedValue(0).visible = True
      ChangedValue(1).visible = True
      valuePanel(0).visible = True
    Case 3                                      'command-failure
      statusflagsFrame.visible = True
      valuePanel(0).Caption = "command-value"
      valueBox(0).Text = ""
      valuePanel(0).visible = True
      valuePanel(1).Caption = "feedback-value"
      valueBox(1).Text = ""
      valuePanel(1).visible = True
    Case 4                                      'floating-limit
      statusflagsFrame.visible = True
      valuePanel(0).Caption = "reference-value"
      valueBox(0).Text = ""
      valuePanel(0).visible = True
      valuePanel(1).Caption = "setpoint-value"
      valueBox(1).Text = ""
      valuePanel(1).visible = True
      valuePanel(2).Caption = "error-limit"
      valueBox(2).Text = ""
      valuePanel(2).visible = True
    Case 5                                      'out-of-range
      statusflagsFrame.visible = True
      valuePanel(0).Caption = "exceeding-value"
      valueBox(0).Text = ""
      valuePanel(0).visible = True
      valuePanel(1).Caption = "deadband"
      valueBox(1).Text = ""
      valuePanel(1).visible = True
      valuePanel(2).Caption = "exceeded-limit"
      valueBox(2).Text = ""
      valuePanel(2).visible = True
    Case Else                                   'complex event
  End Select
End Sub

Private Sub Form_Load()
Dim i
  
  nheight = Height
  psPanel.Move valuePanel(0).Left, valuePanel(0).Top
  nsPanel.Move valuePanel(1).Left, valuePanel(1).Top
  PropertyStates.Clear
  PropertyStates.AddItem "BOOLEAN"
  PropertyStates.AddItem "BinaryPV"
  PropertyStates.AddItem "EventType"
  PropertyStates.AddItem "Polarity"
  PropertyStates.AddItem "ProgramRequest"
  PropertyStates.AddItem "ProgramState"
  PropertyStates.AddItem "ProgramError"
  PropertyStates.AddItem "Reliability"
  PropertyStates.AddItem "EventState"
  PropertyStates.AddItem "DeviceStatus"
  PropertyStates.AddItem "EngineeringUnits"
  PropertyStates.ListIndex = 0
  MonthList.Clear
  MonthList.AddItem "Any"
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
  DowList.AddItem "Any"
  DowList.AddItem "Mon"
  DowList.AddItem "Tue"
  DowList.AddItem "Wed"
  DowList.AddItem "Thu"
  DowList.AddItem "Fri"
  DowList.AddItem "Sat"
  DowList.AddItem "Sun"
  DowList.ListIndex = 0
  NotifyType.Clear
  NotifyType.AddItem "alarm"
  NotifyType.AddItem "event"
  NotifyType.AddItem "ack-notification"
  NotifyType.ListIndex = 0
  EventType.Clear
  EventType.AddItem "change-of-bitstring"
  EventType.AddItem "change-of-state"
  EventType.AddItem "change-of-value"
  EventType.AddItem "command-failure"
  EventType.AddItem "floating-limit"
  EventType.AddItem "out-of-range"
  EventType.ListIndex = 0
  fromState.Clear
  fromState.AddItem "normal"
  fromState.AddItem "fault"
  fromState.AddItem "offnormal"
  fromState.AddItem "high-limit"
  fromState.AddItem "low-limit"
  fromState.ListIndex = 0
  toState.Clear
  toState.AddItem "normal"
  toState.AddItem "fault"
  toState.AddItem "offnormal"
  toState.AddItem "high-limit"
  toState.AddItem "low-limit"
  toState.ListIndex = 0
  ObjectInstance.Text = Format$(MinDeviceInstance)
  ObjectType.Clear
  ObjectType.AddItem propcolon
  ObjectType.AddItem "ANALOG-INPUT"
  ObjectType.AddItem "ANALOG-OUTPUT"
  ObjectType.AddItem "ANALOG-VALUE"
  ObjectType.AddItem "BINARY-INPUT"
  ObjectType.AddItem "BINARY-OUTPUT"
  ObjectType.AddItem "BINARY-VALUE"
  ObjectType.AddItem "CALENDAR"
  ObjectType.AddItem "COMMAND"
  ObjectType.AddItem "DEVICE"
  ObjectType.AddItem "EVENT-ENROLLMENT"
  ObjectType.AddItem "FILE"
  ObjectType.AddItem "GROUP"
  ObjectType.AddItem "LOOP"
  ObjectType.AddItem "MULTISTATE-INPUT"
  ObjectType.AddItem "MULTISTATE-OUTPUT"
  ObjectType.AddItem "NOTIFICATION-CLASS"
  ObjectType.AddItem "PROGRAM"
  ObjectType.AddItem "SCHEDULE"
  ObjectType.ListIndex = -1
  objtype = -1
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

Private Sub NotificationClass_GotFocus()
  GoSelect NotificationClass
End Sub

Private Sub NotifyType_Click()
  If NotifyType.ListIndex >= 2 Then
    Height = (Height - ScaleHeight) + ACKrequired.Top
  Else
    Height = nheight
  End If
End Sub

Private Sub ObjectInstance_Change()
Dim rl As Long
  If ObjectInstance.Text = "" Then
    ObjectInstance.Text = Format$(MinDeviceInstance)
    Exit Sub
  End If
  rl = Val(ObjectInstance.Text)
  If (rl < MinDeviceInstance) Or (rl > 4194303) Then
    Beep
    ObjectInstance.ForeColor = Red
  Else
    ObjectInstance.ForeColor = Green
  End If
End Sub

Private Sub ObjectInstance_GotFocus()
  GoSelect ObjectInstance
End Sub

Private Sub ObjectType_Change()
Dim c$

  If ObjectType.ListIndex = 0 Then             'its proprietary
    c$ = ObjectType.Text
    If Left$(c$, Len(propcolon)) = propcolon Then
      objtype = Val(Mid$(c$, 15))
    Else
      objtype = 0
    End If
    If objtype < 128 Then
      Beep
      ObjectType.ForeColor = Red
    Else
      ObjectType.ForeColor = Black
    End If
  End If
  ObjIDHex.Caption = "0x" + Hex$(objtype)
End Sub

Private Sub ObjectType_Click()
  ObjectType.ForeColor = Black
  If ObjectType.ListIndex > 0 Then
    ObjIDHex.Caption = "0x" + Hex$(ObjectType.ListIndex - 1)
    objtype = ObjectType.ListIndex - 1
  Else
    ObjIDHex.Caption = ""
    objtype = -1
  End If
End Sub

Private Sub ObjectType_GotFocus()
    If Left$(ObjectType.Text, Len(propcolon)) = propcolon Then
      ObjectType.SelStart = Len(propcolon)
      ObjectType.SelLength = 5
    End If
End Sub

Private Sub ObjectType_KeyPress(KeyAscii As Integer)
  If ObjectType.ListIndex <> 0 Then
    KeyAscii = 0
    Exit Sub
  End If
End Sub

Private Function ParseBits(nv As notifyvalue)
Dim i, j, b As Long, bc
  nv.nvNumBits = Len(valueBox(0).Text)
  If nv.nvNumBits = 0 Then
    Beep
    MsgBox "You must enter a bitstring showing which bits changed!", 48, "Missing Bitstring"
    valueBox(0).SetFocus
    Exit Function
  End If
  If nv.nvNumBits > 31 Then
    Beep
    MsgBox "Maximum of 31 bits supported!", 48, "Bitstring Too Long"
    valueBox(0).SetFocus
    Exit Function
  End If
  nv.nvBits = 0
  b = 128
  bc = 0
  For i = 1 To nv.nvNumBits
    j = Asc(Mid$(valueBox(0).Text, i, 1)) - 48
    If (j <> 0) And (j <> 1) Then
      Beep
      MsgBox "Must use only 0 or 1 in a Bitstring here!", 48, "Bad Bitstring Character"
      valueBox(0).SetFocus
      Exit Function
    End If
    If j = 1 Then nv.nvBits = nv.nvBits Or b
    bc = bc + 1
    If bc = 8 Then
      bc = 0
      If i < 31 Then b = b * 32768
    Else
      b = b \ 2
    End If
  Next i
End Function

Private Sub Priority_GotFocus()
  GoSelect Priority
End Sub

Private Sub ProcID_GotFocus()
  GoSelect ProcID
End Sub

Private Sub PropertyStates_Click()
Dim i
  i = VTSAPIgetpropertystates(PropertyStates.ListIndex, NewState.hwnd)
  If i Then NewState.ListIndex = 0
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
Dim apdu As String * 512, alen, i, j, ts As etimestamp, nv As notifyvalue
  If objtype = -1 Then
    Beep
    MsgBox "You must specify an object type!", 48, "No Object Type"
    ObjectType.SetFocus
    Exit Sub
  End If
  If CheckYearBox() Then Exit Sub
  If ChoiceIsSequence.Value Then
    ts.tsType = 1                               'choice is sequence
    ts.tsYear = Val(YearBox.Text)
  Else
    If Left$(Hours.Text, 1) = "?" Then ts.tsHour = 255 Else ts.tsHour = Val(Hours.Text)
    If Left$(Minutes.Text, 1) = "?" Then ts.tsMinute = 255 Else ts.tsMinute = Val(Minutes.Text)
    If Left$(Seconds.Text, 1) = "?" Then ts.tsSecond = 255 Else ts.tsSecond = Val(Seconds.Text)
    If Left$(Hundredths.Text, 1) = "?" Then ts.tsHundredth = 255 Else ts.tsHundredth = Val(Hundredths.Text)
    If ChoiceIsTime.Value Then
      ts.tsType = 0                             'choice is time
    Else                                        'choice is date/time
      ts.tsType = 2
    End If
    If DayList.ListIndex = 0 Then ts.tsDay = 255 Else ts.tsDay = DayList.ListIndex
    If MonthList.ListIndex = 0 Then ts.tsMonth = 255 Else ts.tsMonth = MonthList.ListIndex
    If DowList.ListIndex = 0 Then ts.tsDow = 255 Else ts.tsDow = DowList.ListIndex
    ts.tsYear = Val(YearBox.Text)
  End If
  j = 128
  nv.nvFlags = 0
  For i = 0 To 3
    nv.nvFlags = nv.nvFlags + (j And statusFlags(i).Value)
    j = j \ 2
  Next i
  Select Case EventType.ListIndex
    Case 0
      If ParseBits(nv) Then Exit Sub
    Case 1
      nv.nvEnum(0) = NewState.ListIndex
    Case 2
      If ChangedValue(1).Value Then             'it's a value
        nv.nvNumBits = 0
        nv.nvReal(0) = Val(valueBox(0).Text)
      Else                                      'it's bits
        If ParseBits(nv) Then Exit Sub
      End If
    Case 3
      If (objtype = 0) Or (objtype = 1) Or (objtype = 2) Then
        nv.nvReal(0) = Val(valueBox(0).Text)    'analog guys use real params
        nv.nvReal(1) = Val(valueBox(1).Text)
      Else
        nv.nvEnum(0) = Val(valueBox(0).Text)    'other types use enums
        nv.nvEnum(1) = Val(valueBox(1).Text)
      End If
    Case 4, 5
      nv.nvReal(0) = Val(valueBox(0).Text)
      nv.nvReal(1) = Val(valueBox(1).Text)
      nv.nvReal(2) = Val(valueBox(2).Text)
    Case Else
  End Select
  alen = eEventNotificationService(apdu, conf.Value, Val(ProcID.Text), Val(DeviceInstance.Text), vbOBJECTID(objtype, Val(ObjectInstance.Text)), ts, Val(NotificationClass.Text), Val(Priority.Text), EventType.ListIndex, NotifyType.ListIndex, messageText.Text, ACKrequired.Value, fromState.ListIndex, toState.ListIndex, nv, PropertyStates.ListIndex)
  SendNPDU Left$(apdu, alen), True
End Sub

Private Sub valueBox_Change(Index As Integer)
  If InStr(LCase$(valueBox(Index).Text), "active") Then
    MsgBox "The values 'active' and 'inactive' should be entered as 1 and 0!", 48
  End If
End Sub

Private Sub valueBox_GotFocus(Index As Integer)
  GoSelect valueBox(Index)
End Sub

Private Sub YearBox_Change()
Dim y As Long, isbad
  isbad = False
  YearBox.ForeColor = Green
  If IsNumeric(YearBox.Text) Then
    y = Val(YearBox.Text)
    If ChoiceIsSequence.Value And ((y < 0) Or (y > 65535)) Then
      isbad = True
    ElseIf (y > 2154) Or ((y > 99) And (y < 1900)) Then
      isbad = True
    End If
  ElseIf (Len(YearBox.Text) > 0) And Not ChoiceIsSequence.Value Then
    YearBox.Text = "Any"
  Else
    isbad = True
  End If
  If isbad Then
    Beep
    YearBox.ForeColor = Red
  End If
  CheckForLeapYear
End Sub

Private Sub YearBox_GotFocus()
  GoSelect YearBox
End Sub

