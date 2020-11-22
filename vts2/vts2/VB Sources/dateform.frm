VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form DateForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Date"
   ClientHeight    =   3690
   ClientLeft      =   300
   ClientTop       =   630
   ClientWidth     =   5685
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
   ScaleHeight     =   3690
   ScaleWidth      =   5685
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   435
      Left            =   3990
      TabIndex        =   32
      Top             =   735
      Width           =   1590
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   435
      Left            =   3990
      TabIndex        =   31
      Top             =   195
      Width           =   1590
   End
   Begin Threed.SSFrame FromFrame 
      Height          =   1065
      Left            =   105
      TabIndex        =   33
      Top             =   105
      Width           =   3795
      _Version        =   65536
      _ExtentX        =   6694
      _ExtentY        =   1879
      _StockProps     =   14
      Caption         =   "FromDate"
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
      Begin Threed.SSPanel YearPanel 
         Height          =   330
         Left            =   2310
         TabIndex        =   45
         Top             =   630
         Width           =   540
         _Version        =   65536
         _ExtentX        =   953
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
         Begin VB.TextBox YearBox 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   300
            Index           =   0
            Left            =   15
            MaxLength       =   4
            TabIndex        =   46
            Text            =   "1995"
            Top             =   15
            Width           =   510
         End
      End
      Begin Threed.SSPanel MonthPanel 
         Height          =   330
         Left            =   945
         TabIndex        =   43
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
            TabIndex        =   44
            Top             =   15
            Width           =   1245
         End
      End
      Begin Threed.SSPanel DayPanel 
         Height          =   330
         Left            =   105
         TabIndex        =   41
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
            TabIndex        =   42
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel DOWPanel 
         Height          =   330
         Left            =   2940
         TabIndex        =   39
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
            TabIndex        =   40
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel TimePanel 
         Height          =   225
         Left            =   2520
         TabIndex        =   34
         Top             =   210
         Visible         =   0   'False
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
            Left            =   870
            MaxLength       =   2
            TabIndex        =   35
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
            Left            =   585
            MaxLength       =   3
            TabIndex        =   36
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
            Left            =   300
            MaxLength       =   3
            TabIndex        =   37
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
            Left            =   15
            MaxLength       =   3
            TabIndex        =   38
            Text            =   "00:"
            Top             =   15
            Width           =   285
         End
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
         TabIndex        =   13
         Top             =   420
         Width           =   540
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
         TabIndex        =   14
         Top             =   420
         Width           =   1065
      End
      Begin VB.Label YearLabel 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
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
         TabIndex        =   15
         Top             =   420
         Width           =   540
      End
      Begin VB.Label DOWLabel 
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
         TabIndex        =   16
         Top             =   420
         Width           =   750
      End
   End
   Begin Threed.SSFrame WeekFrame 
      Height          =   1065
      Left            =   105
      TabIndex        =   21
      Top             =   2415
      Visible         =   0   'False
      Width           =   3795
      _Version        =   65536
      _ExtentX        =   6694
      _ExtentY        =   1879
      _StockProps     =   14
      Caption         =   "Week and Day"
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
      Begin Threed.SSPanel Panel3D12 
         Height          =   330
         Left            =   2940
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
         Begin VB.ComboBox DowList 
            Appearance      =   0  'Flat
            Height          =   300
            Index           =   2
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   27
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel Panel3D11 
         Height          =   330
         Left            =   1470
         TabIndex        =   24
         Top             =   630
         Width           =   1380
         _Version        =   65536
         _ExtentX        =   2434
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
         Begin VB.ComboBox WeekList 
            Appearance      =   0  'Flat
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   25
            Top             =   15
            Width           =   1350
         End
      End
      Begin Threed.SSPanel Panel3D10 
         Height          =   330
         Left            =   105
         TabIndex        =   22
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
            Index           =   2
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   23
            Top             =   15
            Width           =   1245
         End
      End
      Begin VB.Label Label12 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "week"
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
         Left            =   1470
         TabIndex        =   30
         Top             =   420
         Width           =   1170
      End
      Begin VB.Label Label11 
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
         Left            =   105
         TabIndex        =   29
         Top             =   420
         Width           =   1065
      End
      Begin VB.Label Label9 
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
         TabIndex        =   28
         Top             =   420
         Width           =   750
      End
   End
   Begin Threed.SSFrame CalFrame 
      Height          =   1065
      Left            =   3990
      TabIndex        =   9
      Top             =   1260
      Width           =   1590
      _Version        =   65536
      _ExtentX        =   2805
      _ExtentY        =   1879
      _StockProps     =   14
      Caption         =   "CalendarEntry"
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
      Begin Threed.SSOption CalWeekNDay 
         Height          =   225
         Left            =   105
         TabIndex        =   12
         Top             =   735
         Width           =   1380
         _Version        =   65536
         _ExtentX        =   2434
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "WeekNDay"
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
      Begin Threed.SSOption CalDateRange 
         Height          =   225
         Left            =   105
         TabIndex        =   11
         Top             =   525
         Width           =   1380
         _Version        =   65536
         _ExtentX        =   2434
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "DateRange"
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
      Begin Threed.SSOption CalDate 
         Height          =   225
         Left            =   105
         TabIndex        =   10
         Top             =   315
         Width           =   1065
         _Version        =   65536
         _ExtentX        =   1879
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Date"
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
   Begin Threed.SSFrame ToFrame 
      Height          =   1065
      Left            =   105
      TabIndex        =   0
      Top             =   1260
      Visible         =   0   'False
      Width           =   3795
      _Version        =   65536
      _ExtentX        =   6694
      _ExtentY        =   1879
      _StockProps     =   14
      Caption         =   "ToDate"
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
      Begin Threed.SSPanel Panel3D8 
         Height          =   330
         Left            =   2310
         TabIndex        =   7
         Top             =   630
         Width           =   540
         _Version        =   65536
         _ExtentX        =   953
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
         Begin VB.TextBox YearBox 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   300
            Index           =   1
            Left            =   15
            MaxLength       =   4
            TabIndex        =   8
            Text            =   "1995"
            Top             =   15
            Width           =   510
         End
      End
      Begin Threed.SSPanel Panel3D7 
         Height          =   330
         Left            =   945
         TabIndex        =   5
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
            TabIndex        =   6
            Top             =   15
            Width           =   1245
         End
      End
      Begin Threed.SSPanel Panel3D6 
         Height          =   330
         Left            =   105
         TabIndex        =   3
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
            Index           =   1
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   4
            Top             =   15
            Width           =   720
         End
      End
      Begin Threed.SSPanel Panel3D5 
         Height          =   330
         Left            =   2940
         TabIndex        =   1
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
            TabIndex        =   2
            Top             =   15
            Width           =   720
         End
      End
      Begin VB.Label Label8 
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
         TabIndex        =   20
         Top             =   420
         Width           =   750
      End
      Begin VB.Label Label7 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
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
         TabIndex        =   19
         Top             =   420
         Width           =   540
      End
      Begin VB.Label Label6 
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
         TabIndex        =   18
         Top             =   420
         Width           =   1065
      End
      Begin VB.Label Label5 
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
         TabIndex        =   17
         Top             =   420
         Width           =   540
      End
   End
End
Attribute VB_Name = "DateForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim nheight
Dim TTop, TLeft

Private Sub CalDate_Click(Value As Integer)
  If Value Then
    FromFrame.Caption = "Date"
    FromFrame.visible = True
    ToFrame.visible = False
    WeekFrame.visible = False
  End If
End Sub

Private Sub CalDateRange_Click(Value As Integer)
  If Value Then
    FromFrame.Caption = "FromDate"
    FromFrame.visible = True
    ToFrame.visible = True
    WeekFrame.visible = False
  End If
End Sub

Private Sub CalWeekNDay_Click(Value As Integer)
  If Value Then
    FromFrame.visible = False
    ToFrame.visible = False
    WeekFrame.visible = True
  End If
End Sub

Private Sub CancelButton_Click()
  Hide
End Sub

Private Sub CheckDayandDow(Index As Integer)
Dim CheckDate As Variant, dow
  DowList(Index).ForeColor = Black
  If Index < 2 Then
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
  Else                                  'special case of WeekNDay
    WeekList.ForeColor = Black
    If (MonthList(Index).ListIndex > 0) And (WeekList.ListIndex = 5) Then
      Beep                              'caution because February in most years doesn't have week 29-31!
      WeekList.ForeColor = Yellow
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
Dim y, f$
  If YearBox(Index).Text = "Any" Then
    CheckYearBox = False
    Exit Function
  End If
  y = Val(YearBox(Index).Text)
  If Index = 0 Then f$ = FromFrame.Caption Else f$ = ToFrame.Caption
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

Private Sub DayList_Click(Index As Integer)
  CheckDayandDow Index
End Sub

Private Sub DowList_Click(Index As Integer)
  CheckDayandDow Index
End Sub

Private Sub Form_Activate()
Dim c$, d$, i, dp
  DateForm.Caption = "Date"             'the "normal" cases
  dp = True                             'normally date panels in FromDate are visible
  d$ = LCase$(DateString)
  Select Case ParseType
    Case ptDT                           'date and time
      TimePanel.visible = True
      TimePanel.Move TLeft, TTop
      CalFrame.visible = False
      WeekFrame.visible = False
      ToFrame.visible = False
      FromFrame.Caption = "DateTime"
      Height = nheight - ToFrame.Height
      If Nxt(d$) = "datetime" Then      'it's in a normal format
        ParseDate 0, d$                 'parse date info from it
        hours.Text = NxtC(d$, ":")
        minutes.Text = NxtC(d$, ":")
        seconds.Text = NxtC(d$, ".")
        Hundredths.Text = d$
      Else                              'must default it
        hours.Text = "??:"
        minutes.Text = "??:"
        seconds.Text = "??."
        Hundredths.Text = "??"
        MonthList(0).ListIndex = 0
        DayList(0).ListIndex = 0
        DowList(0).ListIndex = 0
        YearBox(0).Text = Format$(Now, "yyyy")
      End If
    Case ptD                            'date
      TimePanel.visible = False
      CalFrame.visible = False
      WeekFrame.visible = False
      ToFrame.visible = False
      FromFrame.Caption = "Date"
      Height = nheight - ToFrame.Height
      If Nxt(d$) <> ":" Then            'it's in a normal format
        ParseDate 0, d$                 'parse date info from it
      Else                              'must default it
        MonthList(0).ListIndex = 0
        DayList(0).ListIndex = 0
        DowList(0).ListIndex = 0
        YearBox(0).Text = Format$(Now, "yyyy")
      End If
    Case ptT                           'time
      DateForm.Caption = "Time"
      TimePanel.Move DayLabel.Left, DayLabel.Top
      TimePanel.visible = True
      dp = False                        'don't want date panels to be visible
      CalFrame.visible = False
      WeekFrame.visible = False
      ToFrame.visible = False
      FromFrame.Caption = "Time"
      Height = nheight - ToFrame.Height
      If Nxt(d$) = ":" Then             'it's in a normal format
        hours.Text = NxtC(d$, ":")
        minutes.Text = NxtC(d$, ":")
        seconds.Text = NxtC(d$, ".")
        Hundredths.Text = d$
      Else                              'must default it
        hours.Text = "??:"
        minutes.Text = "??:"
        seconds.Text = "??."
        Hundredths.Text = "??"
      End If
    Case ptDtrange                      'date range
      TimePanel.visible = False
      CalFrame.visible = False
      WeekFrame.visible = False
      ToFrame.visible = True
      FromFrame.Caption = "FromDate"
      Height = nheight
      If Nxt(d$) = "daterange" Then     'it's in a normal format
        ParseDate 0, d$                 'parse date info from it
        ParseDate 1, d$                 'parse date info from it
      Else                              'must default it
        For i = 0 To 1
          MonthList(i).ListIndex = 0
          DayList(i).ListIndex = 0
          DowList(i).ListIndex = 0
          YearBox(i).Text = Format$(Now, "yyyy")
        Next i
      End If
    Case ptCalist                       'calendarentry
      TimePanel.visible = False
      CalFrame.visible = True
      Height = nheight
      Select Case Nxt(d$)
        Case "daterange"
          CalDateRange.Value = True     'select it (this will make the right panels visible)
          ParseDate 0, d$               'parse date info from it
          ParseDate 1, d$               'parse date info from it
        Case "weeknday"
          CalWeekNDay.Value = True      'select it (this will make the right panels visible)
          c$ = Nxt(d$)
          For i = 0 To MonthList(2).ListCount - 1
            If c$ = LCase$(MonthList(2).List(i)) Then
              MonthList(2).ListIndex = i
              Exit For
            End If
          Next i
          c$ = Nxt(d$)
          For i = 0 To WeekList.ListCount - 1
            If c$ = LCase$(WeekList.List(i)) Then
              WeekList.ListIndex = i
              Exit For
            End If
          Next i
          For i = 0 To DowList(2).ListCount - 1
            If d$ = LCase$(DowList(2).List(i)) Then
              DowList(2).ListIndex = i
              Exit For
            End If
          Next i
        Case Else                       'default to date
          CalDate.Value = True          'select it (this will make the right panels visible)
          ParseDate 0, d$               'parse date info from it
      End Select
  End Select
  DayPanel.visible = dp
  MonthPanel.visible = dp
  YearPanel.visible = dp
  DowPanel.visible = dp
  DayLabel.visible = dp
  MonthLabel.visible = dp
  YearLabel.visible = dp
  DowLabel.visible = dp
End Sub

Private Sub Form_Load()
Dim i
  nheight = (Height - ScaleHeight) + WeekFrame.Top
  Height = nheight
  WeekFrame.Move FromFrame.Left, FromFrame.Top
  For i = 0 To 2
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
  WeekList.Clear
  WeekList.AddItem "Any"
  WeekList.AddItem "1st-7th"
  WeekList.AddItem "8th-14th"
  WeekList.AddItem "15th-21st"
  WeekList.AddItem "22nd-28th"
  WeekList.AddItem "29th-31st"
  WeekList.AddItem "Last 7 Days"
  WeekList.ListIndex = 0
  TTop = TimePanel.Top
  TLeft = TimePanel.Left
End Sub

Private Sub Hours_Change()
  If IsNumeric(hours.Text) Then
    If Val(hours.Text) > 23 Then
      Beep
      hours.ForeColor = Red
      Exit Sub
    End If
  End If
  hours.ForeColor = Green
End Sub

Private Sub Hours_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    hours.Text = "??:"
    minutes.SetFocus
  End If
End Sub

Private Sub Hours_LostFocus()
  If Right$(hours.Text, 1) <> ":" Then hours.Text = hours.Text + ":"
End Sub

Private Sub Hundredths_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    Hundredths.Text = "??"
  End If
End Sub

Private Sub Minutes_Change()
  If IsNumeric(minutes.Text) Then
    If Val(minutes.Text) > 59 Then
      Beep
      minutes.ForeColor = Red
      Exit Sub
    End If
  End If
  minutes.ForeColor = Green
End Sub

Private Sub Minutes_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    minutes.Text = "??:"
    seconds.SetFocus
  End If
End Sub

Private Sub Minutes_LostFocus()
  If Right$(minutes.Text, 1) <> ":" Then minutes.Text = minutes.Text + ":"
End Sub

Private Sub MonthList_Click(Index As Integer)
Dim i, j, y, sday
  If Index = 2 Then                     'special case of WeekNDay
    CheckDayandDow 2
    Exit Sub
  End If
  CheckForLeapYear Index
End Sub

Private Sub OKButton_Click()
Dim fmt
  fmt = 0
  Select Case ParseType
    Case ptD                            'date
      fmt = 6
    Case ptT                            'time
      fmt = 5
    Case ptDT                           'date and time
      fmt = 4
    Case ptCalist                       'calendarentry
      If CalDate.Value Then
        fmt = 1
      ElseIf CalDateRange.Value Then
        fmt = 2
      Else
        fmt = 3
      End If
    Case ptDtrange                      'date range
      fmt = 2
  End Select

  Select Case fmt
    Case 1                              'date only
      If CheckYearBox(0) Then Exit Sub  'cancelled
      DateString = "Date," + DayList(0).Text + "," + MonthList(0).Text + "," + YearBox(0).Text + "," + DowList(0).Text
    Case 2                              'daterange
      If CheckYearBox(0) Then Exit Sub  'cancelled
      If CheckYearBox(1) Then Exit Sub  'cancelled
      DateString = "DateRange," + DayList(0).Text + "," + MonthList(0).Text + "," + YearBox(0).Text + "," + DowList(0).Text + "," + DayList(1).Text + "," + MonthList(1).Text + "," + YearBox(1).Text + "," + DowList(1).Text
    Case 3                              'weeknday
      DateString = "WeekNDay," + MonthList(2).Text + "," + WeekList.Text + "," + DowList(2).Text
    Case 4                              'date and time
      If CheckYearBox(0) Then Exit Sub  'cancelled
      If (hours.ForeColor = Red) Or (minutes.ForeColor = Red) Or (seconds.ForeColor = Red) Then
        Beep
        MsgBox "The time specified in the " + FromFrame.Caption + "is not a valid BACnet time!", 48, "Invalid Time"
        Exit Sub
      End If
      DateString = "DateTime," + DayList(0).Text + "," + MonthList(0).Text + "," + YearBox(0).Text + "," + DowList(0).Text + "," + hours.Text + minutes.Text + seconds.Text + Hundredths.Text
    Case 5                              'time
      DateString = hours.Text + minutes.Text + seconds.Text + Hundredths.Text
    Case 6                              'date only
      If CheckYearBox(0) Then Exit Sub  'cancelled
      DateString = DayList(0).Text + "," + MonthList(0).Text + "," + YearBox(0).Text + "," + DowList(0).Text
    Case Else
      DateString = ""                   'return null string
  End Select
  Hide
End Sub

Private Sub ParseDate(Index As Integer, d$)
Dim c$, m$, i
  c$ = Nxt(d$)
  m$ = Nxt(d$)
  YearBox(Index).Text = Nxt(d$)
  For i = 0 To MonthList(Index).ListCount - 1
    If m$ = LCase$(MonthList(Index).List(i)) Then
      MonthList(Index).ListIndex = i
      Exit For
    End If
  Next i
  If c$ = "any" Then DayList(Index).ListIndex = 0 Else DayList(Index).ListIndex = Val(c$)
  For i = 0 To DowList(Index).ListCount - 1
    If d$ = LCase$(DowList(Index).List(i)) Then
      DowList(Index).ListIndex = i
      Exit For
    End If
  Next i
End Sub

Private Sub Seconds_Change()
  If IsNumeric(seconds.Text) Then
    If Val(seconds.Text) > 59 Then
      Beep
      seconds.ForeColor = Red
      Exit Sub
    End If
  End If
  seconds.ForeColor = Green
End Sub

Private Sub Seconds_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H39) Then 'not a digit
    KeyAscii = 0
    seconds.Text = "??."
    Hundredths.SetFocus
  End If
End Sub

Private Sub Seconds_LostFocus()
  If Right$(seconds.Text, 1) <> "." Then seconds.Text = seconds.Text + "."
End Sub

Private Sub WeekList_Click()
  CheckDayandDow 2
End Sub

Private Sub YearBox_Change(Index As Integer)
Dim y
  YearBox(Index).ForeColor = Green
  If IsNumeric(YearBox(Index).Text) Then
    y = Val(YearBox(Index).Text)
    If (y > 2154) Or ((y > 99) And (y < 1900)) Then
      Beep
      YearBox(Index).ForeColor = Red
    End If
  ElseIf Len(YearBox(Index).Text) > 0 Then
    YearBox(Index).Text = "Any"
  End If
  CheckForLeapYear Index
End Sub

