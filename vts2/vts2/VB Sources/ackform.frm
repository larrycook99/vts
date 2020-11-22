VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form AckForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Acknowledge Alarm"
   ClientHeight    =   4710
   ClientLeft      =   1875
   ClientTop       =   1755
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
   ScaleHeight     =   4710
   ScaleWidth      =   5460
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   375
      Left            =   4410
      TabIndex        =   22
      Top             =   1950
      Width           =   960
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   375
      Left            =   4395
      TabIndex        =   21
      Top             =   1470
      Width           =   960
   End
   Begin Threed.SSPanel Panel3D3 
      Height          =   345
      Left            =   2205
      TabIndex        =   41
      Top             =   945
      Width           =   3165
      _Version        =   65536
      _ExtentX        =   5583
      _ExtentY        =   609
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
      Autosize        =   3
      Begin VB.ComboBox AckState 
         Appearance      =   0  'Flat
         BackColor       =   &H00FFFFFF&
         ForeColor       =   &H00000000&
         Height          =   315
         Left            =   15
         TabIndex        =   2
         Top             =   15
         Width           =   3135
      End
   End
   Begin Threed.SSPanel Panel3D1 
      Height          =   330
      Left            =   2205
      TabIndex        =   59
      Top             =   525
      Width           =   3165
      _Version        =   65536
      _ExtentX        =   5583
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
      Begin VB.TextBox AckSource 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   270
         Left            =   30
         TabIndex        =   1
         Top             =   45
         Width           =   3030
      End
   End
   Begin Threed.SSFrame ObjIDFrame 
      Height          =   960
      Left            =   105
      TabIndex        =   53
      Top             =   1365
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   1693
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
         TabIndex        =   55
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
            Size            =   8.25
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
            TabIndex        =   3
            Top             =   15
            Width           =   3030
         End
      End
      Begin Threed.SSPanel Panel3D2 
         Height          =   330
         Left            =   3255
         TabIndex        =   54
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
            TabIndex        =   4
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
         TabIndex        =   58
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
         TabIndex        =   57
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
         TabIndex        =   56
         Top             =   210
         Width           =   165
      End
   End
   Begin Threed.SSPanel ProcPanel 
      Height          =   330
      Left            =   2205
      TabIndex        =   51
      Top             =   105
      Width           =   1065
      _Version        =   65536
      _ExtentX        =   1879
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
      Begin VB.TextBox ProcID 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   270
         Left            =   30
         MaxLength       =   10
         TabIndex        =   0
         Text            =   "0"
         Top             =   45
         Width           =   1005
      End
   End
   Begin Threed.SSFrame acktsFrame 
      Height          =   1065
      Left            =   105
      TabIndex        =   28
      Top             =   3570
      Width           =   5265
      _Version        =   65536
      _ExtentX        =   9287
      _ExtentY        =   1879
      _StockProps     =   14
      Caption         =   "timeOfAcknowledgement"
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
         Index           =   1
         Left            =   2625
         TabIndex        =   44
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
         Begin VB.TextBox Hundredths 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   195
            Index           =   1
            Left            =   870
            MaxLength       =   2
            TabIndex        =   16
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
            Index           =   1
            Left            =   585
            MaxLength       =   3
            TabIndex        =   15
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
            Index           =   1
            Left            =   300
            MaxLength       =   3
            TabIndex        =   14
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
            Index           =   1
            Left            =   15
            MaxLength       =   3
            TabIndex        =   13
            Text            =   "00:"
            Top             =   15
            Width           =   285
         End
      End
      Begin Threed.SSPanel YearPanel 
         Height          =   330
         Index           =   1
         Left            =   2310
         TabIndex        =   32
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
            Index           =   1
            Left            =   30
            MaxLength       =   4
            TabIndex        =   19
            Text            =   "1995"
            Top             =   45
            Width           =   585
         End
      End
      Begin Threed.SSPanel MonthPanel 
         Height          =   330
         Index           =   1
         Left            =   945
         TabIndex        =   31
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
            Index           =   1
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   18
            Top             =   15
            Width           =   1245
         End
      End
      Begin Threed.SSPanel DayPanel 
         Height          =   345
         Index           =   1
         Left            =   105
         TabIndex        =   30
         Top             =   630
         Width           =   750
         _Version        =   65536
         _ExtentX        =   1323
         _ExtentY        =   609
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
            Height          =   315
            Index           =   1
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   17
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel DowPanel 
         Height          =   330
         Index           =   1
         Left            =   3045
         TabIndex        =   29
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
            Index           =   1
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   20
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSOption ChoiceIsDateTime 
         Height          =   225
         Index           =   1
         Left            =   3885
         TabIndex        =   50
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
         Index           =   1
         Left            =   3885
         TabIndex        =   49
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
         Index           =   1
         Left            =   3885
         TabIndex        =   48
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
         Index           =   1
         Left            =   3045
         TabIndex        =   40
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
         Index           =   1
         Left            =   2310
         TabIndex        =   39
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
         Index           =   1
         Left            =   945
         TabIndex        =   38
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
         Index           =   1
         Left            =   105
         TabIndex        =   37
         Top             =   420
         Width           =   540
      End
   End
   Begin Threed.SSFrame tsFrame 
      Height          =   1065
      Left            =   105
      TabIndex        =   23
      Top             =   2415
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
      Begin Threed.SSPanel DowPanel 
         Height          =   330
         Index           =   0
         Left            =   3045
         TabIndex        =   27
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
            Index           =   0
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   12
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel DayPanel 
         Height          =   330
         Index           =   0
         Left            =   105
         TabIndex        =   26
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
            Index           =   0
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   9
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel MonthPanel 
         Height          =   330
         Index           =   0
         Left            =   945
         TabIndex        =   25
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
            Index           =   0
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   10
            Top             =   15
            Width           =   1245
         End
      End
      Begin Threed.SSPanel YearPanel 
         Height          =   330
         Index           =   0
         Left            =   2310
         TabIndex        =   24
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
            Index           =   0
            Left            =   30
            MaxLength       =   4
            TabIndex        =   11
            Text            =   "1995"
            Top             =   45
            Width           =   585
         End
      End
      Begin Threed.SSPanel TimePanel 
         Height          =   225
         Index           =   0
         Left            =   2625
         TabIndex        =   43
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
            Index           =   0
            Left            =   15
            MaxLength       =   3
            TabIndex        =   5
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
            Index           =   0
            Left            =   300
            MaxLength       =   3
            TabIndex        =   6
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
            Index           =   0
            Left            =   585
            MaxLength       =   3
            TabIndex        =   7
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
            Index           =   0
            Left            =   870
            MaxLength       =   2
            TabIndex        =   8
            Text            =   "00"
            Top             =   15
            Width           =   285
         End
      End
      Begin Threed.SSOption ChoiceIsDateTime 
         Height          =   225
         Index           =   0
         Left            =   3885
         TabIndex        =   47
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
         Index           =   0
         Left            =   3885
         TabIndex        =   46
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
         Index           =   0
         Left            =   3885
         TabIndex        =   45
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
         Index           =   0
         Left            =   3045
         TabIndex        =   36
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
         Index           =   0
         Left            =   2310
         TabIndex        =   35
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
         Index           =   0
         Left            =   945
         TabIndex        =   34
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
         Index           =   0
         Left            =   105
         TabIndex        =   33
         Top             =   420
         Width           =   540
      End
   End
   Begin VB.Label Label5 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "event state being acknowledged "
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
      Height          =   405
      Left            =   735
      TabIndex        =   42
      Top             =   945
      Width           =   1380
      WordWrap        =   -1  'True
   End
   Begin VB.Label Label3 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "acknowledgement source"
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
      Left            =   0
      TabIndex        =   60
      Top             =   525
      Width           =   2040
   End
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "acknowledging process identifier "
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
      Height          =   435
      Left            =   210
      TabIndex        =   52
      Top             =   105
      Width           =   1905
      WordWrap        =   -1  'True
   End
End
Attribute VB_Name = "AckForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim evstate, objtype

Private Sub AckState_Change()
Dim c$

  If AckState.ListIndex <= 0 Then             'its proprietary
    c$ = AckState.Text
    If Left$(c$, Len(propcolon)) = propcolon Then
      evstate = Val(Mid$(c$, Len(propcolon) + 1))
    Else
      evstate = 0
    End If
    If evstate < 64 Then
      Beep
      AckState.ForeColor = Red
    Else
      AckState.ForeColor = Black
    End If
  End If
End Sub

Private Sub AckState_Click()
  AckState.ForeColor = Black
  If AckState.ListIndex > 0 Then
    evstate = AckState.ListIndex - 1
  Else
    evstate = -1
  End If
End Sub

Private Sub AckState_GotFocus()
    If Left$(AckState.Text, Len(propcolon)) = propcolon Then
      AckState.SelStart = Len(propcolon)
      AckState.SelLength = 5
    End If
End Sub

Private Sub CheckDayandDow(Index As Integer)
Dim CheckDate As Variant, dow
  DowList(Index).ForeColor = Black
    If (DayList(Index).ListIndex > 0) And (MonthList(Index).ListIndex > 0) And IsNumeric(YearBox(Index).Text) And (DowList(Index).ListIndex > 0) Then
    'a real day was selected and we need to check if DowList is valid for that day
      CheckDate = DateSerial(Val(YearBox(Index).Text), MonthList(Index).ListIndex, DayList(Index).ListIndex)
      dow = WeekDay(CheckDate) - 1
      If dow = 0 Then dow = 7           'converts it to 1=Monday, 7=Sunday (The BACnet way)
      If dow <> DowList(Index).ListIndex Then
        Beep
        DowList(Index).ForeColor = Red  'complain, but let him do it anyway
      End If
    End If
End Sub

Private Sub CheckForLeapYear(Index)
Dim i, j, y, sday
  Select Case MonthList(Index).ListIndex
    Case 0, 1, 3, 5, 7, 8, 10, 12
      j = 31
    Case 4, 6, 9, 11
      j = 30
    Case 2
      y = Val(YearBox(Index).Text)
      If ((y Mod 400) = 0) Or (((y Mod 4) = 0) And ((y Mod 100) <> 0)) Then j = 29 Else j = 28
  End Select
  sday = DayList(Index).ListIndex
  If sday < 0 Then sday = 0
  DayList(Index).Clear
  DayList(Index).AddItem "Any"
  For i = 1 To j
    DayList(Index).AddItem Format$(i)
  Next i
  If sday > j Then
    Beep
    MsgBox "You've switched to a month which doesn't have the day" + Str$(sday) + ", so the day has been changed to <Any>!", 48, "Invalid Day for New Month"
    sday = 0
    DayList(Index).SetFocus
  End If
  DayList(Index).ListIndex = sday
End Sub

'Validate the date for Index
'out:   true if user cancels due to error
'       false if ok
Private Function CheckYearBox(Index As Integer) As Integer
Dim y As Long, f$
  If YearBox(Index).Text = "Any" Then
    CheckYearBox = False
    Exit Function
  End If
  y = Val(YearBox(Index).Text)
  If Index = 0 Then f$ = tsFrame.Caption Else f$ = acktsFrame.Caption
  If ChoiceIsSequence(Index).Value Then
    If (y < 0) Or (y > 65535) Then
      Beep
      MsgBox "The sequence number specified for " + f$ + " (" + YearBox(Index).Text + ") must be in the range 0..65535!", 48, "Invalid Sequence Number"
      CheckYearBox = True
    Else
      CheckYearBox = False
    End If
    Exit Function
  End If
  If y < 100 Then y = y + 1900
  If (y < 1900) Or (y > 2154) Then
    Beep
    MsgBox "The year specified for " + f$ + " (" + YearBox(Index).Text + ") is not a valid year in BACnet encoding!", 48, "Invalid Year"
    CheckYearBox = True                         'force cancel
    Exit Function
  End If
  If DowList(Index).ForeColor = Red Then        'not really the correct day of week
    Beep
    If MsgBox("The day of week specified for " + f$ + " (" + DowList(Index).Text + ") is not correct for " + MonthList(Index).Text + " " + DayList(Index).Text + "," + Format$(y) + "! Although this date can be encoded in BACnet, it can never occur. Are you sure that you want to use this date?", 33, "Impossible Date") <> 1 Then
      CheckYearBox = True                         'force cancel
      Exit Function
    End If
  End If
  YearBox(Index).Text = Format$(y)              'leave it as a 19xx type of format
  CheckYearBox = False
End Function

Private Sub ChoiceIsDateTime_Click(Index As Integer, Value As Integer)
  If Value Then
    TimePanel(Index).visible = True
    DayLabel(Index).visible = True
    DayPanel(Index).visible = True
    MonthLabel(Index).visible = True
    MonthPanel(Index).visible = True
    DowLabel(Index).visible = True
    DowPanel(Index).visible = True
    YearLabel(Index).visible = True
    YearPanel(Index).visible = True
    YearLabel(Index).Caption = "year"
    YearBox(Index).MaxLength = 4
  End If
End Sub

Private Sub ChoiceIsSequence_Click(Index As Integer, Value As Integer)
  If Value Then
    TimePanel(Index).visible = False
    DayLabel(Index).visible = False
    DayPanel(Index).visible = False
    MonthLabel(Index).visible = False
    MonthPanel(Index).visible = False
    DowLabel(Index).visible = False
    DowPanel(Index).visible = False
    YearLabel(Index).visible = True
    YearPanel(Index).visible = True
    YearLabel(Index).Caption = "sequence"
    YearBox(Index).MaxLength = 5
  End If
End Sub

Private Sub ChoiceIsTime_Click(Index As Integer, Value As Integer)
  If Value Then
    TimePanel(Index).visible = True
    DayLabel(Index).visible = False
    DayPanel(Index).visible = False
    MonthLabel(Index).visible = False
    MonthPanel(Index).visible = False
    DowLabel(Index).visible = False
    DowPanel(Index).visible = False
    YearLabel(Index).visible = False
    YearPanel(Index).visible = False
  End If
End Sub

Private Sub CloseButton_Click()
  HideBiModal AckForm
End Sub

Private Sub DayList_Click(Index As Integer)
  CheckDayandDow Index
End Sub

Private Sub DowList_Click(Index As Integer)
  CheckDayandDow Index
End Sub

Private Sub FillInTimeStamp(Index, ts As etimestamp)

  If ChoiceIsSequence(Index).Value Then
    ts.tsType = 1                               'choice is sequence
    ts.tsYear = Val(YearBox(Index).Text)
  Else
    If Left$(hours(Index).Text, 1) = "?" Then ts.tsHour = 255 Else ts.tsHour = Val(hours(Index).Text)
    If Left$(minutes(Index).Text, 1) = "?" Then ts.tsMinute = 255 Else ts.tsMinute = Val(minutes(Index).Text)
    If Left$(seconds(Index).Text, 1) = "?" Then ts.tsSecond = 255 Else ts.tsSecond = Val(seconds(Index).Text)
    If Left$(Hundredths(Index).Text, 1) = "?" Then ts.tsHundredth = 255 Else ts.tsHundredth = Val(Hundredths(Index).Text)
    If ChoiceIsTime(Index).Value Then
      ts.tsType = 0                             'choice is time
    Else                                        'choice is date/time
      ts.tsType = 2
    End If
    If DayList(Index).ListIndex = 0 Then ts.tsDay = 255 Else ts.tsDay = DayList(Index).ListIndex
    If MonthList(Index).ListIndex = 0 Then ts.tsMonth = 255 Else ts.tsMonth = MonthList(Index).ListIndex
    If DowList(Index).ListIndex = 0 Then ts.tsDow = 255 Else ts.tsDow = DowList(Index).ListIndex
    ts.tsYear = Val(YearBox(Index).Text)
  End If
End Sub

Private Sub Form_Load()
Dim i
  For i = 0 To 1
    MonthList(i).Clear
    MonthList(i).AddItem "Any"
    MonthList(i).AddItem "January"
    MonthList(i).AddItem "February"
    MonthList(i).AddItem "March"
    MonthList(i).AddItem "April"
    MonthList(i).AddItem "May"
    MonthList(i).AddItem "June"
    MonthList(i).AddItem "July"
    MonthList(i).AddItem "August"
    MonthList(i).AddItem "September"
    MonthList(i).AddItem "October"
    MonthList(i).AddItem "November"
    MonthList(i).AddItem "December"
    MonthList(i).ListIndex = 0
    DowList(i).Clear
    DowList(i).AddItem "Any"
    DowList(i).AddItem "Mon"
    DowList(i).AddItem "Tue"
    DowList(i).AddItem "Wed"
    DowList(i).AddItem "Thu"
    DowList(i).AddItem "Fri"
    DowList(i).AddItem "Sat"
    DowList(i).AddItem "Sun"
    DowList(i).ListIndex = 0
  Next i
  AckState.Clear
  AckState.AddItem "(proprietary):"
  AckState.AddItem "normal"
  AckState.AddItem "fault"
  AckState.AddItem "offnormal"
  AckState.AddItem "high-limit"
  AckState.AddItem "low-limit"
  evstate = -1
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

Private Sub Hours_Change(Index As Integer)
  If IsNumeric(hours(Index).Text) Then
    If Val(hours(Index).Text) > 23 Then
      Beep
      hours(Index).ForeColor = Red
      Exit Sub
    End If
  End If
  hours(Index).ForeColor = Green
End Sub

Private Sub Hours_GotFocus(Index As Integer)
  GoSelect hours(Index)
End Sub

Private Sub Hours_KeyPress(Index As Integer, KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    hours(Index).Text = "??:"
    minutes(Index).SetFocus
  End If
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

Private Sub Minutes_Change(Index As Integer)
  If IsNumeric(minutes(Index).Text) Then
    If Val(minutes(Index).Text) > 59 Then
      Beep
      minutes(Index).ForeColor = Red
      Exit Sub
    End If
  End If
  minutes(Index).ForeColor = Green
End Sub

Private Sub Minutes_GotFocus(Index As Integer)
  GoSelect minutes(Index)
End Sub

Private Sub Minutes_KeyPress(Index As Integer, KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    minutes(Index).Text = "??:"
    seconds(Index).SetFocus
  End If
End Sub

Private Sub MonthList_Click(Index As Integer)
Dim i, j, y, sday
  If Index = 2 Then                     'special case of WeekNDay
    CheckDayandDow 2
    Exit Sub
  End If
  CheckForLeapYear Index
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

Private Sub Seconds_Change(Index As Integer)
  If IsNumeric(seconds(Index).Text) Then
    If Val(seconds(Index).Text) > 59 Then
      Beep
      seconds(Index).ForeColor = Red
      Exit Sub
    End If
  End If
  seconds(Index).ForeColor = Green
End Sub

Private Sub Seconds_GotFocus(Index As Integer)
  GoSelect seconds(Index)
End Sub

Private Sub Seconds_KeyPress(Index As Integer, KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    seconds(Index).Text = "??."
    Hundredths(Index).SetFocus
  End If
End Sub

Private Sub SendButton_Click()
Dim apdu As String * 128, alen, ts As etimestamp, ackts As etimestamp
  If objtype = -1 Then
    Beep
    MsgBox "You must specify an object type!", 48, "No Object Type"
    ObjectType.SetFocus
    Exit Sub
  End If
  If evstate = -1 Then
    Beep
    MsgBox "You must specify an event state!", 48, "No Event State"
    AckState.SetFocus
    Exit Sub
  End If
  If CheckYearBox(0) Then Exit Sub
  If CheckYearBox(1) Then Exit Sub
  FillInTimeStamp 0, ts
  FillInTimeStamp 1, ackts
  alen = eAcknowledgeAlarmService(apdu, Val(procid.Text), vbOBJECTID(objtype, Val(ObjectInstance)), evstate, ts, AckSource.Text, ackts)
  SendNPDU Left$(apdu, alen), True
End Sub

Private Sub YearBox_Change(Index As Integer)
Dim y As Long, isbad
  isbad = False
  YearBox(Index).ForeColor = Green
  If IsNumeric(YearBox(Index).Text) Then
    y = Val(YearBox(Index).Text)
    If ChoiceIsSequence(Index).Value And ((y < 0) Or (y > 65535)) Then
      isbad = True
    ElseIf (y > 2154) Or ((y > 99) And (y < 1900)) Then
      isbad = True
    End If
  ElseIf (Len(YearBox(Index).Text) > 0) And Not ChoiceIsSequence(Index).Value Then
    YearBox(Index).Text = "Any"
  Else
    isbad = True
  End If
  If isbad Then
    Beep
    YearBox(Index).ForeColor = Red
  End If
  CheckForLeapYear Index
End Sub

