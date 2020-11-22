VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "Threed32.ocx"
Begin VB.Form PropValForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Edit Property Value"
   ClientHeight    =   4335
   ClientLeft      =   2070
   ClientTop       =   5010
   ClientWidth     =   6030
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
   ScaleHeight     =   4335
   ScaleWidth      =   6030
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton EditButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Edit"
      Height          =   330
      Left            =   5250
      TabIndex        =   18
      Top             =   1680
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   375
      Left            =   5250
      TabIndex        =   1
      Top             =   630
      Width           =   735
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   5250
      TabIndex        =   0
      Top             =   105
      Width           =   735
   End
   Begin Threed.SSFrame ListFrame 
      Height          =   1785
      Left            =   105
      TabIndex        =   50
      Top             =   7350
      Visible         =   0   'False
      Width           =   5895
      _Version        =   65536
      _ExtentX        =   10398
      _ExtentY        =   3149
      _StockProps     =   14
      Caption         =   "List Frame"
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
      Begin VB.CommandButton LClearButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Clear"
         Height          =   330
         Left            =   5040
         TabIndex        =   51
         Top             =   1260
         Width           =   750
      End
      Begin VB.CommandButton LDeleteButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Delete"
         Height          =   330
         Left            =   5040
         TabIndex        =   52
         Top             =   840
         Width           =   750
      End
      Begin VB.CommandButton LAddButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Add"
         Height          =   330
         Left            =   5040
         TabIndex        =   53
         Top             =   420
         Width           =   750
      End
      Begin Threed.SSPanel Panel3D5 
         Height          =   1230
         Left            =   105
         TabIndex        =   54
         Top             =   420
         Width           =   4845
         _Version        =   65536
         _ExtentX        =   8546
         _ExtentY        =   2170
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
         Autosize        =   3
         Begin VB.ListBox llist 
            Appearance      =   0  'Flat
            Height          =   1200
            Left            =   15
            MultiSelect     =   2  'Extended
            TabIndex        =   55
            Top             =   15
            Width           =   4815
         End
      End
      Begin VB.Label Label7 
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
         Left            =   3150
         TabIndex        =   56
         Top             =   210
         Width           =   165
      End
   End
   Begin Threed.SSPanel PriorityPanel 
      Height          =   330
      Left            =   4410
      TabIndex        =   46
      Top             =   1680
      Visible         =   0   'False
      Width           =   750
      _Version        =   65536
      _ExtentX        =   1323
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
      Autosize        =   3
      Begin VB.ComboBox priority 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   47
         Top             =   15
         Visible         =   0   'False
         Width           =   720
      End
   End
   Begin Threed.SSPanel VTPanel 
      Height          =   1035
      Left            =   105
      TabIndex        =   31
      Top             =   6300
      Visible         =   0   'False
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   1826
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
      Autosize        =   3
      Begin VB.ListBox VTClass 
         Appearance      =   0  'Flat
         Height          =   1005
         Left            =   15
         MultiSelect     =   2  'Extended
         TabIndex        =   32
         Top             =   15
         Width           =   4185
      End
   End
   Begin Threed.SSPanel BigPanel 
      Height          =   960
      Left            =   105
      TabIndex        =   29
      Top             =   5250
      Visible         =   0   'False
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   1693
      _StockProps     =   15
      ForeColor       =   8388736
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
      Begin VB.Label BigLabel 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         ForeColor       =   &H00800080&
         Height          =   930
         Left            =   15
         TabIndex        =   30
         Top             =   15
         Width           =   4185
         WordWrap        =   -1  'True
      End
   End
   Begin Threed.SSPanel ptpanel 
      Height          =   330
      Left            =   4200
      TabIndex        =   27
      Top             =   1080
      Visible         =   0   'False
      Width           =   1800
      _Version        =   65536
      _ExtentX        =   3175
      _ExtentY        =   582
      _StockProps     =   15
      Caption         =   "Panel3D1"
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
      Begin VB.ComboBox PTlist 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   28
         Top             =   15
         Width           =   1770
      End
   End
   Begin Threed.SSFrame ObjIDFrame 
      Height          =   960
      Left            =   105
      TabIndex        =   19
      Top             =   1680
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   1693
      _StockProps     =   14
      Caption         =   "Object Identifier"
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
      Begin Threed.SSPanel Panel3D4 
         Height          =   330
         Left            =   105
         TabIndex        =   22
         Top             =   420
         Width           =   3060
         _Version        =   65536
         _ExtentX        =   5398
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
         Autosize        =   3
         Begin VB.ComboBox ObjectType 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   300
            Left            =   15
            Sorted          =   -1  'True
            Style           =   2  'Dropdown List
            TabIndex        =   23
            Top             =   15
            Width           =   3030
         End
      End
      Begin Threed.SSPanel Panel3D2 
         Height          =   330
         Left            =   3255
         TabIndex        =   20
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
            TabIndex        =   21
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
         TabIndex        =   26
         Top             =   210
         Width           =   840
      End
      Begin VB.Label Label1 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "object type"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   210
         TabIndex        =   25
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
         TabIndex        =   24
         Top             =   210
         Width           =   165
      End
   End
   Begin Threed.SSPanel relationpanel 
      Height          =   330
      Left            =   105
      TabIndex        =   16
      Top             =   1080
      Visible         =   0   'False
      Width           =   4005
      _Version        =   65536
      _ExtentX        =   7064
      _ExtentY        =   582
      _StockProps     =   15
      Caption         =   "Panel3D1"
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
      Begin VB.ComboBox relation 
         Appearance      =   0  'Flat
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   17
         Top             =   15
         Width           =   3975
      End
   End
   Begin Threed.SSPanel NumberPanel 
      Height          =   330
      Left            =   105
      TabIndex        =   12
      Top             =   4410
      Visible         =   0   'False
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   582
      _StockProps     =   15
      ForeColor       =   8388736
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
      Begin VB.TextBox Number 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   240
         Left            =   30
         MaxLength       =   10
         TabIndex        =   13
         Top             =   75
         Width           =   4080
      End
   End
   Begin Threed.SSPanel EnumPanel 
      Height          =   330
      Left            =   105
      TabIndex        =   10
      Top             =   4830
      Visible         =   0   'False
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
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
         TabIndex        =   11
         Top             =   15
         Width           =   4185
      End
   End
   Begin Threed.SSFrame PropFrame 
      Height          =   960
      Left            =   105
      TabIndex        =   8
      Top             =   0
      Width           =   5055
      _Version        =   65536
      _ExtentX        =   8916
      _ExtentY        =   1693
      _StockProps     =   14
      Caption         =   "Property"
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
      Begin Threed.SSPanel Panel3D3 
         Height          =   330
         Left            =   105
         TabIndex        =   5
         Top             =   420
         Width           =   3585
         _Version        =   65536
         _ExtentX        =   6324
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
         Autosize        =   3
         Begin VB.ComboBox PropertyIdentifier 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   2
            Top             =   15
            Width           =   3555
         End
      End
      Begin Threed.SSPanel ArrayPanel 
         Height          =   330
         Left            =   3780
         TabIndex        =   9
         Top             =   420
         Visible         =   0   'False
         Width           =   1170
         _Version        =   65536
         _ExtentX        =   2064
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
         Begin VB.TextBox ArrayIndex 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   240
            Left            =   30
            MaxLength       =   10
            TabIndex        =   6
            Top             =   75
            Width           =   1110
         End
      End
      Begin VB.Label PropIDHex 
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
         Left            =   3150
         TabIndex        =   7
         Top             =   210
         Width           =   165
      End
      Begin VB.Label ArrayLabel 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "array index"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   3780
         TabIndex        =   3
         Top             =   210
         Visible         =   0   'False
         Width           =   1170
      End
      Begin VB.Label Label2 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "property identifier"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   210
         TabIndex        =   4
         Top             =   210
         Width           =   1500
      End
   End
   Begin Threed.SSFrame OPropFrame 
      Height          =   1785
      Left            =   105
      TabIndex        =   33
      Top             =   2535
      Visible         =   0   'False
      Width           =   5895
      _Version        =   65536
      _ExtentX        =   10398
      _ExtentY        =   3149
      _StockProps     =   14
      Caption         =   "Object Property"
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
      Begin VB.CommandButton AddButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Add"
         Height          =   330
         Left            =   5040
         TabIndex        =   43
         Top             =   420
         Visible         =   0   'False
         Width           =   750
      End
      Begin VB.CommandButton DeleteButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Delete"
         Height          =   330
         Left            =   5040
         TabIndex        =   44
         Top             =   840
         Visible         =   0   'False
         Width           =   750
      End
      Begin VB.CommandButton ClearButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Clear"
         Height          =   330
         Left            =   5040
         TabIndex        =   45
         Top             =   1260
         Visible         =   0   'False
         Width           =   750
      End
      Begin Threed.SSPanel OPlistPanel 
         Height          =   840
         Left            =   105
         TabIndex        =   41
         Top             =   840
         Visible         =   0   'False
         Width           =   4845
         _Version        =   65536
         _ExtentX        =   8546
         _ExtentY        =   1482
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
         Autosize        =   3
         Begin VB.ListBox plist 
            Appearance      =   0  'Flat
            Height          =   810
            Left            =   15
            MultiSelect     =   2  'Extended
            TabIndex        =   42
            Top             =   15
            Width           =   4815
         End
      End
      Begin Threed.SSPanel OArrayPanel 
         Height          =   330
         Left            =   3780
         TabIndex        =   36
         Top             =   420
         Visible         =   0   'False
         Width           =   1170
         _Version        =   65536
         _ExtentX        =   2064
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
         Begin VB.TextBox OArrayIndex 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   240
            Left            =   30
            MaxLength       =   10
            TabIndex        =   37
            Top             =   105
            Width           =   1110
         End
      End
      Begin Threed.SSPanel Panel3D1 
         Height          =   330
         Left            =   105
         TabIndex        =   34
         Top             =   420
         Width           =   3585
         _Version        =   65536
         _ExtentX        =   6324
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
         Autosize        =   3
         Begin VB.ComboBox OPropertyIdentifier 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   35
            Top             =   15
            Width           =   3555
         End
      End
      Begin VB.Label Label6 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "property identifier"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   210
         TabIndex        =   40
         Top             =   210
         Width           =   1500
      End
      Begin VB.Label OArrayLabel 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "array index"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   3780
         TabIndex        =   39
         Top             =   210
         Visible         =   0   'False
         Width           =   1170
      End
      Begin VB.Label OPropIDHex 
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
         Left            =   3150
         TabIndex        =   38
         Top             =   210
         Width           =   165
      End
   End
   Begin Threed.SSCheck relinquish 
      Height          =   225
      Left            =   4410
      TabIndex        =   49
      Top             =   2100
      Width           =   1275
      _Version        =   65536
      _ExtentX        =   2249
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Relinquish?"
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
   Begin VB.Label prioritylabel 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "priority"
      ForeColor       =   &H00808080&
      Height          =   195
      Left            =   4410
      TabIndex        =   48
      Top             =   1440
      Visible         =   0   'False
      Width           =   600
   End
   Begin VB.Label NumberLabel 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
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
      Height          =   225
      Left            =   105
      TabIndex        =   14
      Top             =   7455
      Width           =   4005
   End
   Begin VB.Label ValueLabel 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Type of Value"
      ForeColor       =   &H00808080&
      Height          =   225
      Left            =   105
      TabIndex        =   15
      Top             =   1440
      Width           =   4215
   End
End
Attribute VB_Name = "PropValForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Dim EditType                                    'code for what kind of value this property has
Dim MinNumber As Long, MaxNumber As Long
Dim Activated, vtop
Dim EnteringValue As String
Dim IncomingPropID, opheight
Dim dpt() As defaultpt
Dim FlagNames As String

Private Sub AddButton_Click()
Dim p$, i
  If ObjectType.ListIndex = -1 Then
    Beep
    MsgBox "You must specify an object type!", 48, "No Object Type"
    ObjectType.SetFocus
    Exit Sub
  End If
  If OPropertyIdentifier.ListIndex = -1 Then
    Beep
    MsgBox "You must specify a property!", 48, "No Property Selected"
    OPropertyIdentifier.SetFocus
    Exit Sub
  End If
  If ParseType = ptLopref Then p$ = "(" + ObjectType.Text + "," + ObjectInstance.Text + ")," Else p$ = ""
  p$ = p$ + OPropertyIdentifier.Text
  If OArrayPanel.visible Then p$ = p$ + "[" + OArrayIndex.Text + "]"
  plist.AddItem p$
End Sub

Private Sub CancelButton_Click()
  Activated = False
  Hide
End Sub

Private Sub ClearButton_Click()
  If MsgBox("Are you sure that you want to erase all the entries in the properties list?", 33, "Clear Property List") <> 1 Then Exit Sub
  plist.Clear
End Sub

Private Sub DeleteButton_Click()
Dim i
  i = 0
  Do While i < plist.ListCount
    If plist.Selected(i) Then
      plist.RemoveItem i
    Else
      i = i + 1
    End If
  Loop
End Sub

Private Sub EditButton_Click()
  Select Case EditType
    Case 7                              'octet of T or F flags
      BitNames = FlagNames
      BitsForm.Show modal
      ValueLabel.Caption = "0x" + Hex$(BitsValue) + " (" + BitNames + ")"
    Case 8                              'Calist
      EditCalString
    Case 10                             'Skeys, Tsrecip, Dabind, Reciplist, Recip
      EditUsingDA
    Case 15                             'evparams
      EditUsingEV
  End Select
End Sub

Private Sub EditCalString()
      CalForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
      CalString = BigLabel.Caption
      CalForm.Show modal
      BigPanel.visible = True
      BigLabel.Caption = CalString
End Sub

Private Sub EditUsingDA()
  DAForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
  DAString = BigLabel.Caption
  DAForm.Show modal
  BigPanel.visible = True
  BigLabel.Caption = DAString
End Sub

Private Sub EditUsingDT()
  DateForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
  DateString = NumberPanel.Caption
  DateForm.Show modal
  NumberPanel.visible = True
  Number.visible = False
  ValueLabel.visible = True
  ValueLabel.Caption = "date/time"
  NumberPanel.Caption = DateString
End Sub

Private Sub EditUsingEV()
  EventParamsForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
  EVString = BigLabel.Caption
  EventParamsForm.Show modal
  BigPanel.visible = True
  BigLabel.Caption = EVString
End Sub

'find out the edit type given a parsetype from VTSAPI
'in:    ParseType   parse type returned from VTSAPIgetpropinfo
'       v$          the value string
'       ep          enumeration table index returned from VTSAPIgtepropinfo
'out:   EditType set to:
'       -1          not editable
'       else        the edit type
'Note:  ep MUST BE filled only with values returned by VTSAPIgetpropinfo. Since
'       VTSAPIgetenumtable expects this to be an index, VERY BAD things will
'       happen if this doesn't actually point to an enumeration table in the proper format!
Private Sub FindEditType(v$, ByVal ep)
Dim nenums, i, ename As String * 64, e$, es, emax As Long
  EnumPanel.visible = False
  NumberPanel.visible = False
  NumberLabel.visible = False
  Number.visible = True
  BigPanel.visible = False
  EditButton.visible = False
  ObjIDFrame.visible = False
  OPropFrame.visible = False
  ListFrame.visible = False
  VTPanel.visible = False
  ValueLabel.ForeColor = Grey
  Select Case ParseType
    Case ptObjID                                'an object identifier
      ObjIDFrame.visible = True
      ValueLabel.Caption = ""
      EditType = 0
      v$ = Mid$(v$, 2)                          'strip leading (
      e$ = Nxt(v$)                              'get objtype
      ObjectType.ListIndex = -1
      ObjectInstance.Text = Format$(Val(Nxt(v$)))
      For i = 0 To ObjectType.ListCount - 1
        If e$ = ObjectType.List(i) Then
          ObjectType.ListIndex = i
          Exit For
        End If
      Next i
    Case ptDT, ptT, ptD, ptDtrange              'date/time, date, time, datereange
      EditType = 5
      EditUsingDT
    Case ptPropref, ptSetref                    'obj property reference, setpoint reference
      ObjIDFrame.visible = True
      ValueLabel.Caption = "Object Property Reference"
      EditType = 6
      v$ = Mid$(v$, 3)                          'strip leading ((
      e$ = Nxt(v$)                              'get objtype
      ObjectType.ListIndex = -1
      ObjectInstance.Text = Format$(Val(Nxt(v$)))
      For i = 0 To ObjectType.ListCount - 1
        If e$ = ObjectType.List(i) Then
          ObjectType.ListIndex = i
          Exit For
        End If
      Next i
      DoEvents                                  'think this is needed to get ObjectType_Click to occur
      e$ = NxtC(v$, ")")                        'get property name
      For i = 0 To OPropertyIdentifier.ListCount - 1
        If e$ = OPropertyIdentifier.List(i) Then
          OPropertyIdentifier.ListIndex = i
          Exit For
        End If
      Next i
      AddButton.visible = False
      DeleteButton.visible = False
      ClearButton.visible = False
      OPropFrame.Height = OPlistPanel.Top
      OPlistPanel.visible = False
      OPropFrame.visible = True
    Case ptRaslist
      ListFrame.visible = True
      ListFrame.Caption = "List of Read Access Specs"
      EditType = 12
    Case ptLopref
      ValueLabel.Caption = "List of Obj Property Refs"
      ObjIDFrame.visible = True
      EditType = 13
      v$ = Mid$(v$, 2)                          'strip leading (
      ObjectType.ListIndex = -1
      ObjectInstance.Text = ""
      OPropertyIdentifier.ListIndex = -1
      plist.Clear
      Do
        e$ = Nxt$(v$) + "," + Nxt$(v$) + "," + Nxt$(v$) 'should get ((obtype,inst),propname)
        If e$ <> ",," Then
          plist.AddItem e$
        Else
          Exit Do
        End If
      Loop
      AddButton.visible = True
      DeleteButton.visible = True
      ClearButton.visible = True
      OPropFrame.Height = opheight
      OPlistPanel.visible = True
      OPropFrame.visible = True
    Case ptVTcl                                 'vt classes
      ValueLabel.Caption = "List of VT Classes"
      Do
        e$ = Nxt(v$)                            'get next class
        For i = 0 To VTClass.ListCount - 1
          If e$ = VTClass.List(i) Then
            VTClass.Selected(i) = True
            Exit For
          End If
        Next i
      Loop Until v$ = ""
      VTPanel.visible = True
      EditType = 11
    Case ptSkeys, ptTsrecip, ptDabind, ptReciplist
      ListFrame.visible = True
      Select Case ParseType
        Case ptSkeys
          ListFrame.Caption = "List of Session Keys"
        Case ptTsrecip
          ListFrame.Caption = "List of TimeSync Recips"
        Case ptDabind
          ListFrame.Caption = "List of Device Adr Bindings"
        Case ptReciplist
          ListFrame.Caption = "List of Recipients"
      End Select
      EditType = 14
    Case ptStavals
      ListFrame.visible = True
      ListFrame.Caption = "List of State Values"
      EditType = 14
    Case ptAct                          'action array
      ListFrame.visible = True
      ListFrame.Caption = "List of Actions"
      EditType = 14
    Case ptRecip
      BigPanel.Top = vtop
      BigLabel.Caption = v$
      EditButton.visible = True
      ValueLabel.Caption = "Recipient"
      EditType = 10
      EditUsingDA
    Case ptCalist                               'calendar entry list
      BigPanel.Top = vtop
      BigPanel.Caption = v$
      EditButton.visible = True
      ValueLabel.Caption = "List of CalendarEntries"
      EditType = 8
      EditCalString
    Case ptBits                                 'octet of T or F flags
      EditType = 7
      nenums = VTSAPIgetenumtable(ep, 0, es, emax, ename)
      BitNames = ""
      For i = 0 To nenums - 1
        nenums = VTSAPIgetenumtable(ep, i, es, emax, ename)
        BitNames = BitNames + RTrim$(ename) + ","
      Next i
      FlagNames = BitNames
      BitsValue = Val(v$)
      BitsForm.Move Left + PropFrame.Left + (Width - ScaleWidth) / 2, Top + ValueLabel.Top + (Height - ScaleHeight) / 2
      BitsForm.Show modal
      ValueLabel.Caption = "0x" + Hex$(BitsValue) + " (" + BitNames + ")"
      EditButton.visible = True
    Case ptEbool, ptET, ptPab                   'boolean,an enumeration table,priority array bpv
      nenums = VTSAPIgetenumtable(ep, 0, es, emax, ename)
      EnumList.Clear
      For i = 0 To nenums - 1
        nenums = VTSAPIgetenumtable(ep, i, es, emax, ename)
        EnumList.AddItem ename
        e$ = RTrim$(ename)
        If e$ = v$ Then EnumList.ListIndex = i
      Next i
      If EnumList.ListIndex = -1 Then EnumList.ListIndex = 0
      EnumPanel.visible = True
      If ParseType = ptEbool Then
        ValueLabel.Caption = "boolean"
        EditType = 9
      Else
        ValueLabel.Caption = "enumeration"
        EditType = 1
      End If
    Case ptFloat, ptPaf                         'float,priority array flt
      ValueLabel.Caption = "real"
      Number.Text = v$
      NumberPanel.visible = True
      Number.MaxLength = 0
      EditType = 2
    Case ptUW, ptPau                            'unsigned word,priority array uw
      ValueLabel.Caption = "unsigned"
      Number.Text = v$
      NumberPanel.visible = True
      FormatMinMaxNumber 0, 65535
      Number.MaxLength = 5
      EditType = 3
    Case ptUD                                   'unsigned dword
      ValueLabel.Caption = "unsigned"
      Number.Text = v$
      NumberPanel.visible = True
      FormatMinMaxNumber 0, 0
      Number.MaxLength = 10
      EditType = 3
    Case ptSW, ptSsint                          'signed word  MAG add ptSsint 13 FEB 2001
      ValueLabel.Caption = "integer"
      Number.Text = v$
      NumberPanel.visible = True
      FormatMinMaxNumber -32768, 32767
      Number.MaxLength = 6
      EditType = 3
    Case ptU127                                 '1..127
      ValueLabel.Caption = "unsigned"
      Number.Text = v$
      NumberPanel.visible = True
      FormatMinMaxNumber 1, 127
      Number.MaxLength = 3
      EditType = 3
    Case ptS10                                  'char [10]
      ValueLabel.Caption = "character string"
      Number.Text = v$
      NumberPanel.visible = True
      Number.MaxLength = 10
      EditType = 4
    Case ptS32                                  'char [32]
      ValueLabel.Caption = "character string"
      Number.Text = v$
      NumberPanel.visible = True
      Number.MaxLength = 32
      EditType = 4
    Case ptS64                                  'char [64]
      ValueLabel.Caption = "character string"
      Number.Text = v$
      NumberPanel.visible = True
      Number.MaxLength = 64
      EditType = 4
    Case ptS132                                 'char [132]
      ValueLabel.Caption = "character string"
      Number.Text = v$
      NumberPanel.visible = True
      Number.MaxLength = 132
      EditType = 4
    Case ptActext, ptStatext                    'action_text array,state text array
      ValueLabel.Caption = "character string"
      Number.Text = v$
      NumberPanel.visible = True
      Number.MaxLength = 0
      EditType = 4
    Case ptU16                                  '1..16
      ValueLabel.Caption = "unsigned"
      Number.Text = v$
      NumberPanel.visible = True
      FormatMinMaxNumber 1, 16
      Number.MaxLength = 3
      EditType = 3
    Case ptWsched                              'weekly schedule
      EditType = 5
      SchedForm.PeriodPanel.visible = False
      SchedForm.priority.visible = False
      SchedForm.prioritylabel.visible = False
      SchedForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
      SchedForm.Show modal
      NumberPanel.visible = True
      Number.visible = False
      ValueLabel.visible = True
      ValueLabel.Caption = "time/values"
      NumberPanel.Caption = SchedString
    Case ptXsched                              'weekly schedule
      EditType = 5
      SchedForm.PeriodPanel.visible = True
      SchedForm.priority.visible = True
      SchedForm.prioritylabel.visible = True
      SchedForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
      SchedForm.Show modal
      NumberPanel.visible = True
      Number.visible = False
      ValueLabel.visible = True
      ValueLabel.Caption = "exception schedules"
      NumberPanel.Caption = SchedString
    Case ptEvparm                           'event params
      BigPanel.Top = vtop
      BigLabel.Caption = v$
      EditButton.visible = True
      ValueLabel.Caption = "Event Parameters"
      EditType = 15
      EditUsingEV
    Case Else
      ValueLabel.ForeColor = Red
      ValueLabel.Caption = "This property is not editable!"
      Beep
      EditType = -1
  End Select
End Sub

Private Sub Form_Activate()
Dim otyp, c$, ip$, v$, np As Long, i, j, ptype, pflags, pid As Long, ep, pn As String * 64
  If Activated Then Exit Sub                    'we are already active
  Activated = True
'we enter the form with PVObjectType set to the prevailing object type
'and PVPropVal containing the string to edit in the form:
'propname=value
'or
'propname[arrayindex]=value

  IncomingPropID = 0
  v$ = PVPropVal
  i = InStr(v$, "|")
  If i Then v$ = Left$(v$, i - 1)
  relationpanel.visible = PVNeedsRelation
  If PVNeedsRelation Then                       'must do something more complex
    i = InStr(v$, "!")
    If i Then
      c$ = NxtC(v$, "!=")
      relation.ListIndex = 1
    Else
      i = InStr(v$, "<")
      If i Then
        If Mid$(v$, i + 1, 1) = "=" Then
          c$ = NxtC(v$, "<=")
          relation.ListIndex = 4
        Else
          c$ = NxtC(v$, "<")
          relation.ListIndex = 2
        End If
      Else
        i = InStr(v$, ">")
        If i Then
          If Mid$(v$, i + 1, 1) = "=" Then
            c$ = NxtC(v$, ">=")
            relation.ListIndex = 5
          Else
            c$ = NxtC(v$, ">")
            relation.ListIndex = 3
          End If
        Else
          c$ = NxtC(v$, "==")
          relation.ListIndex = 0
        End If
      End If
    End If
  Else                                          'normal case
    If PVObjectType >= KnownObjectTypes Then
      Beep
      MsgBox "This version does not allow editing of property values for unknown object types! The object type you have currently selected (" + Format$(PVObjectType) + ") is not currently supported", 48, "Unknown Object Type"
      Hide
      Exit Sub
    End If
    c$ = NxtC(v$, "=")
  End If
  ip$ = NxtC(c$, "[")
  ArrayPanel.visible = False
  ArrayLabel.visible = False
  ArrayIndex.Text = NxtC(c$, "]")
  PropIDHex.Caption = ""
  PropertyIdentifier.Clear
  EnteringValue = v$
  If PVNeedsRelation Then
    For i = 0 To KnownProps - 1
      PropertyIdentifier.AddItem PropName(i)
      j = PropertyIdentifier.NewIndex
      PropertyIdentifier.ItemData(j) = i
      If ip$ = PropName(i) Then PropertyIdentifier.ListIndex = i
    Next i
  Else
    np = VTSAPIgetpropinfo(PVObjectType, NumProps, "", 0&, 0&, 0&, 0&)
    For i = 0 To np - 1
      pid = VTSAPIgetpropinfo(PVObjectType, i, pn, ptype, 0&, pflags, ep)
      PropertyIdentifier.AddItem pn
      j = PropertyIdentifier.NewIndex
      PropertyIdentifier.ItemData(j) = pid
      c$ = Left$(pn, InStr(pn, Chr$(0)) - 1)
      If c$ = ip$ Then                          'this is our guy
        IncomingPropID = j
        PropertyIdentifier.ListIndex = j
      End If
    Next i
  End If
  If PropertyIdentifier.ListIndex < 0 Then PropertyIdentifier.ListIndex = 0
  PropertyIdentifier.ForeColor = Black
End Sub

Private Sub Form_Load()
Dim i
  Move Width - ScaleWidth, Height - ScaleHeight
  vtop = ObjIDFrame.Top
  NumberPanel.Top = vtop
  EnumPanel.Top = vtop
  BigPanel.Top = vtop
  VTPanel.Top = vtop
  ListFrame.Top = vtop + (priority.Height * 1.5)
  NumberLabel.Top = vtop + (NumberPanel.Height * 1.5)
  opheight = OPropFrame.Height
  relation.Clear
  relation.AddItem "== (equal)"
  relation.AddItem "!= (not-equal)"
  relation.AddItem "<  (less-than)"
  relation.AddItem ">  (greater-than)"
  relation.AddItem "<= (less-than-or-equal)"
  relation.AddItem ">= (greater-than-or-equal)"
  PTlist.Clear
  PTlist.ListIndex = -1
  ObjectInstance.Text = Format$(MinDeviceInstance)
  ObjectType.Clear
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

  PropRefForm.ObjectInstance.Text = Format$(MinDeviceInstance)
  PropRefForm.ObjectType.Clear
  PropRefForm.ObjectType.AddItem "ANALOG-INPUT"
  PropRefForm.ObjectType.AddItem "ANALOG-OUTPUT"
  PropRefForm.ObjectType.AddItem "ANALOG-VALUE"
  PropRefForm.ObjectType.AddItem "BINARY-INPUT"
  PropRefForm.ObjectType.AddItem "BINARY-OUTPUT"
  PropRefForm.ObjectType.AddItem "BINARY-VALUE"
  PropRefForm.ObjectType.AddItem "CALENDAR"
  PropRefForm.ObjectType.AddItem "COMMAND"
  PropRefForm.ObjectType.AddItem "DEVICE"
  PropRefForm.ObjectType.AddItem "EVENT-ENROLLMENT"
  PropRefForm.ObjectType.AddItem "FILE"
  PropRefForm.ObjectType.AddItem "GROUP"
  PropRefForm.ObjectType.AddItem "LOOP"
  PropRefForm.ObjectType.AddItem "MULTISTATE-INPUT"
  PropRefForm.ObjectType.AddItem "MULTISTATE-OUTPUT"
  PropRefForm.ObjectType.AddItem "NOTIFICATION-CLASS"
  PropRefForm.ObjectType.AddItem "PROGRAM"
  PropRefForm.ObjectType.AddItem "SCHEDULE"
  PropRefForm.ObjectType.ListIndex = -1
  
  priority.AddItem "none"
  For i = 1 To 16
    priority.AddItem Format$(i)
  Next i
  priority.ListIndex = 0

  VTClass.Clear
  VTClass.AddItem "default-terminal"
  VTClass.AddItem "ansi-x3.64"
  VTClass.AddItem "dec-vt52"
  VTClass.AddItem "dec-vt100"
  VTClass.AddItem "dec-vt220"
  VTClass.AddItem "hp-700/94"
  VTClass.AddItem "ibm-3130"
  VTClass.ListIndex = -1
End Sub

Private Sub FormatMinMaxNumber(low&, high&)
  MinNumber = low&
  MaxNumber = high&
  NumberLabel.Caption = Str$(low&) + " to" + Str$(high&)
End Sub

Private Sub LAddButton_Click()
  Select Case ParseType
    Case ptSkeys, ptTsrecip, ptDabind, ptReciplist
      DAString = ""                     '                           ***218
      DAForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
      DAForm.Show modal
      If DAString <> "" Then
        llist.AddItem DAString
      End If
    Case ptStavals
      NumsForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
      NumsForm.Show modal
      If NumsString <> "" Then
        llist.AddItem NumsString
      End If
    Case ptRaslist
      PropRefForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
      PropRefForm.ObjIDFrame.visible = True
      PropRefForm.PropertyIdentifier.Clear
      PropRefForm.MultipleRefs.visible = False
      PropRefForm.MultipleRefs.Value = True
      PropRefForm.DInstanceLabel.visible = False
      PropRefForm.DInstancePanel.visible = False
      PropRefForm.DInstanceCheck.visible = False
      PropRefForm.prioritylabel.visible = False
      PropRefForm.priority.visible = False
      PropRefForm.prioritylabel.visible = False
      PropRefForm.ActFrame.visible = False
      PropRefForm.EditButton.visible = False
      PropRefForm.ValuePanel.visible = False
      PropRefForm.Caption = "Edit Property References"
      PropRefForm.Show modal
      If PRString <> "" Then
        llist.AddItem PRString
      End If
    Case ptAct
      PropRefForm.Move PropValForm.Left + (Width - ScaleWidth), PropValForm.Top + (Height - ScaleHeight)
      PropRefForm.ObjIDFrame.visible = True
      PropRefForm.PropertyIdentifier.Clear
      PropRefForm.MultipleRefs.visible = False
      PropRefForm.MultipleRefs.Value = False
      PropRefForm.DInstanceLabel.visible = True
      PropRefForm.DInstancePanel.visible = True
      PropRefForm.DInstanceCheck.visible = True
      PropRefForm.prioritylabel.visible = True
      PropRefForm.priority.visible = True
      PropRefForm.prioritylabel.visible = True
      PropRefForm.ActFrame.visible = True
      PropRefForm.EditButton.visible = True
      PropRefForm.ValuePanel.visible = True
      PropRefForm.Caption = "Edit Action"
      PropRefForm.Show modal
      If PRString <> "" Then
        llist.AddItem PRString
      End If
    End Select
End Sub

Private Sub LClearButton_Click()
  If MsgBox("Are you sure that you want to erase all the entries in the properties list?", 33, "Clear Property List") <> 1 Then Exit Sub
  llist.Clear
End Sub

Private Sub LDeleteButton_Click()
Dim i
  i = 0
  Do While i < llist.ListCount
    If llist.Selected(i) Then
      llist.RemoveItem i
    Else
      i = i + 1
    End If
  Loop

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

Private Sub ObjectType_Click()
Dim c$, np As Long, i, pn As String * 64, ptype, PropId As Long
  ObjIDHex.Caption = "0x" + Hex$(ObjectType.ListIndex)
  If ParseType <> ptLopref Then plist.Clear
  OPropertyIdentifier.Clear
  np = VTSAPIgetpropinfo(ObjectType.ListIndex, NumProps, "", 0&, 0&, 0&, 0&)
  For i = 0 To np - 1
      PropId = VTSAPIgetpropinfo(ObjectType.ListIndex, i, pn, ptype, 0&, 0&, 0&)
      OPropertyIdentifier.AddItem pn
      OPropertyIdentifier.ItemData(OPropertyIdentifier.NewIndex) = i
  Next i
  If OPropertyIdentifier.ListCount > 0 Then OPropertyIdentifier.ListIndex = 0
End Sub

Private Sub ObjectType_GotFocus()
    If Left$(ObjectType.Text, Len(propcolon)) = propcolon Then
      ObjectType.SelStart = Len(propcolon)
      ObjectType.SelLength = 5
    End If
End Sub

Private Sub OKButton_Click()
Dim i
  PVPropVal = PropertyIdentifier.Text
  If ArrayPanel.visible Then
    If ArrayIndex.Text <> "" Then
      PVPropVal = PVPropVal + "[" + ArrayIndex.Text + "]"
    Else
      Beep
      MsgBox "You must specify a non-zero Array Index!", 48, "Invalid Array Index"
      ArrayIndex.SetFocus
      Exit Sub
    End If
  End If
  If PVNeedsRelation Then                       'make complex relation
    PVPropVal = PVPropVal + Left$(relation.Text, 2)
  Else                                          'it's simple
    PVPropVal = PVPropVal + "="
  End If
  If PVWriteProperty And relinquish.Value Then
    PVPropVal = PVPropVal + "NULL"
  Else
    Select Case EditType
      Case 0                                      'object identifier
        PVPropVal = PVPropVal + "(" + ObjectType.Text + "," + ObjectInstance.Text + ")"
      Case 5                                      'date/time
        PVPropVal = PVPropVal + NumberPanel.Caption
      Case 6                                      'propref
        PVPropVal = PVPropVal + "((" + ObjectType.Text + "," + ObjectInstance.Text + ")," + OPropertyIdentifier.Text
        If OArrayPanel.visible Then
          If OArrayIndex.Text <> "" Then
            PVPropVal = PVPropVal + "[" + OArrayIndex.Text + "]"
          Else
            Beep
            MsgBox "You must specify a non-zero Array Index!", 48, "Invalid Array Index"
            ArrayIndex.SetFocus
            Exit Sub
          End If
        End If
        PVPropVal = PVPropVal + ")"
      Case 13                                     'lopref
        PVPropVal = PVPropVal + "("
        For i = 0 To plist.ListCount - 1
          PVPropVal = PVPropVal + plist.List(i) + ","
        Next i
        If Right$(PVPropVal, 1) = "," Then PVPropVal = Left$(PVPropVal, Len(PVPropVal) - 1)
        PVPropVal = PVPropVal + ")"
      Case 12, 14                                 'raslist, skeys, tsrecip, dabind, stavals
        PVPropVal = PVPropVal + "("
        For i = 0 To llist.ListCount - 1
          PVPropVal = PVPropVal + llist.List(i) + "|"
        Next i
        If Right$(PVPropVal, 1) = "|" Then PVPropVal = Left$(PVPropVal, Len(PVPropVal) - 1)
        PVPropVal = PVPropVal + ")"
      Case 1, 9                                   'enumeration, boolean
        PVPropVal = PVPropVal + EnumList.Text
      Case 2, 3, 4                                'float,number,string
        PVPropVal = PVPropVal + Number.Text
      Case 7                                      'octet of flags
        PVPropVal = PVPropVal + ValueLabel.Caption
      Case 8, 10, 15                              'calist, reciplist, evparams
        PVPropVal = PVPropVal + BigLabel.Caption
      Case 11                                     'vtclass list
        For i = 0 To VTClass.ListCount - 1
          If VTClass.Selected(i) Then PVPropVal = PVPropVal + VTClass.List(i) + ","
        Next i
        If Right$(PVPropVal, 1) = "," Then PVPropVal = Left$(PVPropVal, Len(PVPropVal) - 1)
      Case Else                                   'not editable
    End Select
  End If
  If PVNeedsRelation Then
    PVPropVal = PVPropVal + "|" + PTlist.Text + "|" + Format$(dpt(PTlist.ListIndex).dep)
  ElseIf PVWriteProperty And (priority.Text <> "none") Then
    PVPropVal = PVPropVal + "@" + priority.Text
  End If
  Activated = False
  Hide
End Sub

Private Sub OPropertyIdentifier_Click()
Dim i, pflags, pid As Long, pn As String * 64
  i = OPropertyIdentifier.ListIndex
  pid = VTSAPIgetpropinfo(ObjectType.ListIndex, i, pn, 0&, 0&, pflags, 0&)
  If pflags And propIsArray Then
    OArrayPanel.visible = True
    OArrayLabel.visible = True
  Else
    OArrayPanel.visible = False
    OArrayLabel.visible = False
  End If
  OPropIDHex.Caption = "0x" + Hex$(pid)
End Sub

Private Sub PropertyIdentifier_Click()
Dim c$, ip$, np As Long, i, j, k, n, pflags, pid As Long, ep, pn As String * 64
  i = PropertyIdentifier.ListIndex
  If IncomingPropID And (i <> IncomingPropID) And (PVPropCanChange = False) Then    'he selected an invalid propid
    PropertyIdentifier.ListIndex = IncomingPropID       'force it back
    i = IncomingPropID
  End If
  If PVNeedsRelation Then
    relinquish.visible = False
    PriorityPanel.visible = False
    priority.visible = False
    prioritylabel.visible = False
    pid = PropertyIdentifier.ItemData(i)
    n = VTSAPIgetdefaultparsetype(pid, PTlist.hwnd)
    If n = 1 Then
      ReDim dpt(1) As defaultpt
      c$ = PTlist.List(0)
      i = InStr(c$, Chr(9))
      c$ = Mid$(c$, i + 1)
      j = InStr(c$, Chr(9))
      ParseType = Val(Left$(c$, j - 1))
      c$ = Mid$(c$, j + 1)
      j = InStr(c$, Chr(9))
      ep = Val(Left$(c$, j - 1))
      pflags = Val(Mid$(c$, j + 1))
      dpt(0).dep = ep
      dpt(0).dpflags = pflags
      dpt(0).dptype = ParseType
      PTlist.List(0) = Left$(PTlist.List(0), i - 1)
      ptpanel.visible = False
    Else
      ReDim dpt(n) As defaultpt
      For k = 0 To n - 1
        c$ = PTlist.List(k)
        i = InStr(c$, Chr(9))
        c$ = Mid$(c$, i + 1)
        j = InStr(c$, Chr(9))
        dpt(k).dptype = Val(Left$(c$, j - 1))
        c$ = Mid$(c$, j + 1)
        j = InStr(c$, Chr(9))
        dpt(k).dep = Val(Left$(c$, j - 1))
        dpt(k).dpflags = Val(Mid$(c$, j + 1))
        PTlist.List(k) = Left$(PTlist.List(k), i - 1)
      Next k
      ParseType = dpt(0).dptype
      ep = dpt(0).dep
      pflags = dpt(0).dpflags
      ptpanel.visible = True
    End If
    PTlist.ListIndex = 0
  Else
    pid = VTSAPIgetpropinfo(PVObjectType, i, pn, ParseType, 0&, pflags, ep)
    If PVWriteProperty Then
      relinquish.Value = False
      relinquish.visible = (pflags And propIsCommandable)
      PriorityPanel.visible = True
      priority.visible = True
      prioritylabel.visible = True
    Else
      relinquish.visible = False
      PriorityPanel.visible = False
      priority.visible = False
      prioritylabel.visible = False
    End If
  End If
  If pflags And propIsArray Then
    ArrayPanel.visible = True
    ArrayLabel.visible = True
  Else
    ArrayPanel.visible = False
    ArrayLabel.visible = False
  End If
  PropIDHex.Caption = "0x" + Hex$(pid)
  FindEditType EnteringValue, ep
End Sub

Private Sub PTlist_Click()
  ParseType = dpt(PTlist.ListIndex).dptype
  If dpt(PTlist.ListIndex).dpflags And propIsArray Then
    ArrayPanel.visible = True
    ArrayLabel.visible = True
  Else
    ArrayPanel.visible = False
    ArrayLabel.visible = False
  End If
  
  FindEditType EnteringValue, dpt(PTlist.ListIndex).dep
End Sub

Private Sub relinquish_Click(Value As Integer)
  If (ParseType = ptFloat) Or (ParseType = ptUW) Then
    NumberPanel.visible = Not (Value)
    ValueLabel.visible = Not (Value)
  ElseIf ParseType = ptET Then
    EnumPanel.visible = Not (Value)
    ValueLabel.visible = Not (Value)
  End If
End Sub

