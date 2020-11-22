VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form SchedForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Schedules"
   ClientHeight    =   6600
   ClientLeft      =   1425
   ClientTop       =   2445
   ClientWidth     =   6810
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
   ScaleHeight     =   440
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   454
   ShowInTaskbar   =   0   'False
   Begin VB.ComboBox priority 
      Appearance      =   0  'Flat
      Height          =   300
      Left            =   5880
      Style           =   2  'Dropdown List
      TabIndex        =   38
      Top             =   1260
      Visible         =   0   'False
      Width           =   720
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   375
      Left            =   5880
      TabIndex        =   1
      Top             =   525
      Width           =   735
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Height          =   375
      Left            =   5880
      TabIndex        =   0
      Top             =   105
      Width           =   735
   End
   Begin Threed.SSFrame Frame3D1 
      Height          =   4740
      Left            =   210
      TabIndex        =   11
      Top             =   1680
      Width           =   6525
      _Version        =   65536
      _ExtentX        =   11509
      _ExtentY        =   8361
      _StockProps     =   14
      Caption         =   "Time Values"
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
      Begin VB.CommandButton ClearButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "clear"
         Height          =   285
         Left            =   5565
         TabIndex        =   31
         Top             =   3150
         Width           =   750
      End
      Begin VB.CommandButton DeleteButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "delete"
         Height          =   285
         Left            =   5565
         TabIndex        =   32
         Top             =   2835
         Width           =   750
      End
      Begin VB.CommandButton AddButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "add"
         Height          =   285
         Left            =   5565
         TabIndex        =   33
         Top             =   2520
         Width           =   750
      End
      Begin Threed.SSPanel plpanel 
         Height          =   1035
         Left            =   105
         TabIndex        =   34
         Top             =   3570
         Width           =   6315
         _Version        =   65536
         _ExtentX        =   11139
         _ExtentY        =   1826
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
         Begin VB.ListBox plist 
            Appearance      =   0  'Flat
            Height          =   1005
            Left            =   15
            MultiSelect     =   2  'Extended
            TabIndex        =   35
            Top             =   15
            Width           =   6285
         End
      End
      Begin Threed.SSPanel TimePanel 
         Height          =   225
         Left            =   105
         TabIndex        =   26
         Top             =   525
         Width           =   1185
         _Version        =   65536
         _ExtentX        =   2090
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
            Left            =   855
            MaxLength       =   2
            TabIndex        =   27
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
            TabIndex        =   28
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
            TabIndex        =   29
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
            TabIndex        =   30
            Text            =   "00:"
            Top             =   15
            Width           =   285
         End
      End
      Begin Threed.SSFrame PrimFrame 
         Height          =   3165
         Left            =   1575
         TabIndex        =   12
         Top             =   315
         Width           =   2745
         _Version        =   65536
         _ExtentX        =   4842
         _ExtentY        =   5583
         _StockProps     =   14
         Caption         =   "Primitive Value Type"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   0
            Left            =   105
            TabIndex        =   25
            Top             =   315
            Width           =   1065
            _Version        =   65536
            _ExtentX        =   1879
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Null"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   1
            Left            =   105
            TabIndex        =   24
            TabStop         =   0   'False
            Top             =   525
            Width           =   1065
            _Version        =   65536
            _ExtentX        =   1879
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Boolean"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   2
            Left            =   105
            TabIndex        =   23
            TabStop         =   0   'False
            Top             =   735
            Width           =   1380
            _Version        =   65536
            _ExtentX        =   2434
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Unsigned Int"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   3
            Left            =   105
            TabIndex        =   22
            TabStop         =   0   'False
            Top             =   945
            Width           =   1695
            _Version        =   65536
            _ExtentX        =   2990
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Signed Int"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   4
            Left            =   105
            TabIndex        =   21
            TabStop         =   0   'False
            Top             =   1155
            Width           =   1065
            _Version        =   65536
            _ExtentX        =   1879
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Real"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   5
            Left            =   105
            TabIndex        =   20
            TabStop         =   0   'False
            Top             =   1365
            Width           =   1065
            _Version        =   65536
            _ExtentX        =   1879
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Double"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   6
            Left            =   105
            TabIndex        =   19
            TabStop         =   0   'False
            Top             =   1575
            Width           =   1590
            _Version        =   65536
            _ExtentX        =   2805
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Octet String"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   7
            Left            =   105
            TabIndex        =   18
            TabStop         =   0   'False
            Top             =   1785
            Width           =   1905
            _Version        =   65536
            _ExtentX        =   3360
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Character String"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   8
            Left            =   105
            TabIndex        =   17
            TabStop         =   0   'False
            Top             =   1995
            Width           =   1275
            _Version        =   65536
            _ExtentX        =   2249
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Bit String"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   9
            Left            =   105
            TabIndex        =   16
            TabStop         =   0   'False
            Top             =   2205
            Width           =   1485
            _Version        =   65536
            _ExtentX        =   2619
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Enumerated"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   10
            Left            =   105
            TabIndex        =   15
            TabStop         =   0   'False
            Top             =   2415
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   11
            Left            =   105
            TabIndex        =   14
            TabStop         =   0   'False
            Top             =   2625
            Width           =   1065
            _Version        =   65536
            _ExtentX        =   1879
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Time"
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
         Begin Threed.SSOption PrimType 
            Height          =   225
            Index           =   12
            Left            =   105
            TabIndex        =   13
            TabStop         =   0   'False
            Top             =   2835
            Width           =   2430
            _Version        =   65536
            _ExtentX        =   4286
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "BACnet Object Identifier"
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
      Begin VB.Label Label1 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Time"
         ForeColor       =   &H80000008&
         Height          =   225
         Left            =   105
         TabIndex        =   36
         Top             =   315
         Width           =   855
      End
   End
   Begin Threed.SSFrame PeriodPanel 
      Height          =   1380
      Left            =   210
      TabIndex        =   2
      Top             =   210
      Width           =   5580
      _Version        =   65536
      _ExtentX        =   9843
      _ExtentY        =   2434
      _StockProps     =   14
      Caption         =   "Exception Period"
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
      Begin VB.CommandButton EditButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Edit"
         Default         =   -1  'True
         Height          =   375
         Left            =   4515
         TabIndex        =   6
         Top             =   315
         Width           =   735
      End
      Begin Threed.SSFrame PerFrame 
         Height          =   750
         Left            =   1680
         TabIndex        =   8
         Top             =   105
         Width           =   2220
         _Version        =   65536
         _ExtentX        =   3916
         _ExtentY        =   1323
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
         Begin Threed.SSOption CalEntry 
            Height          =   225
            Left            =   105
            TabIndex        =   10
            Top             =   210
            Width           =   1905
            _Version        =   65536
            _ExtentX        =   3360
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Calendar Entry"
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
         Begin Threed.SSOption CalRef 
            Height          =   225
            Left            =   105
            TabIndex        =   9
            TabStop         =   0   'False
            Top             =   420
            Width           =   2010
            _Version        =   65536
            _ExtentX        =   3545
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Calendar Reference"
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
      Begin Threed.SSPanel peroidpnl 
         Height          =   330
         Left            =   105
         TabIndex        =   3
         Top             =   945
         Width           =   5265
         _Version        =   65536
         _ExtentX        =   9287
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
         Begin VB.TextBox period 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   300
            Left            =   15
            TabIndex        =   4
            Top             =   15
            Width           =   5235
         End
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
         TabIndex        =   7
         Top             =   210
         Width           =   165
      End
      Begin VB.Label Label4 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "period"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   105
         TabIndex        =   5
         Top             =   735
         Width           =   840
      End
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
      Left            =   5880
      TabIndex        =   37
      Top             =   1050
      Visible         =   0   'False
      Width           =   600
   End
End
Attribute VB_Name = "SchedForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Dim primtag


Private Sub AddButton_Click()
Dim i, p$, savept
  If (hours.ForeColor = Red) Or (minutes.ForeColor = Red) Or (seconds.ForeColor = Red) Then
    Beep
    MsgBox "The time specified is not a valid BACnet time!", 48, "Invalid Time"
    Exit Sub
  End If
  p$ = ""
  Select Case primtag
  Case 0                        'null
    p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(0)Null"
  Case 1                        'boolean
    EnumPointer = 1
    EnumForm.Move Left + SchedForm.Left + (Width - ScaleWidth) / 2, Top + SchedForm.Top + (Height - ScaleHeight) / 2
    EnumForm.Show modal
    If EnumValue <> -1 Then p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(" + Right$(Str$(primtag), 1) + ")" + Format$(EnumValue)
  Case 2                        'uint
    savept = ParseType
    ParseType = ptUD
    NumsForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    NumsForm.Show modal
    If NumsString <> "" Then p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(" + Right$(Str$(primtag), 1) + ")" + NumsString
    ParseType = savept
  Case 3                         'int
    savept = ParseType
    ParseType = ptSW
    NumsForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    NumsForm.Show modal
    If NumsString <> "" Then p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(" + Right$(Str$(primtag), 1) + ")" + NumsString
    ParseType = savept
  Case 4, 5                     'real, dbl
    savept = ParseType
    ParseType = ptFloat
    NumsForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    NumsForm.Show modal
    If NumsString <> "" Then p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(" + Right$(Str$(primtag), 1) + ")" + NumsString
    ParseType = savept
  Case 6, 7                     'octet str, char str
    savept = ParseType
    ParseType = ptS132
    NumsForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    NumsForm.Show modal
    If NumsString <> "" Then p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(" + Right$(Str$(primtag), 1) + ")" + NumsString
    ParseType = savept
  Case 8                        'bit string
    savept = ParseType
    ParseType = ptBits
    NumsForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    NumsForm.Show modal
    If NumsString <> "" Then
      NumsString = Trim$(NumsString)
      p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(" + Right$(Str$(primtag), 1) + ")"
      p$ = p$ + Str$(Len(NumsString)) + "/0b" + NumsString
    End If
    ParseType = savept
  Case 9                        'enum
    EnumPointer = 0             'allow selection from List
    EnumForm.Move Left + SchedForm.Left + (Width - ScaleWidth) / 2, Top + SchedForm.Top + (Height - ScaleHeight) / 2
    EnumForm.Show modal
    If EnumValue <> -1 Then p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(" + Right$(Str$(primtag), 1) + ")" + Format$(EnumValue)
  Case 10, 11                   'date, time
    savept = ParseType
    If primtag = 10 Then
      ParseType = ptD
    Else
      ParseType = ptT
    End If
    DateForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    DateForm.Show modal
    If DateString <> "" Then p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(" + Right$(Str$(primtag), 2) + ")" + DateString
    ParseType = savept
  Case 12                         'object id
    ObjIDForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    ObjIDForm.Show modal
    If ObjIDString <> "" Then p$ = hours.Text + minutes.Text + seconds.Text + Hundredths.Text + "," + "(" + Right$(Str$(primtag), 2) + ")" + ObjIDString
  End Select
  
  If p$ <> "" Then
    plist.AddItem p$
    plist.ListIndex = plist.NewIndex
  End If
End Sub

Private Sub CancelButton_Click()
  plist.Clear
  SchedString = ""
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
Dim ptsave
  If CalEntry Then
    ptsave = ParseType
    ParseType = ptCalist
    DateForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, Top + SchedForm.Top + (Height - ScaleHeight) / 2
    DateForm.Show modal
    period.Text = DateString
    ptsave = ParseType
  Else  'CalRef
    ObjIDForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, Top + SchedForm.Top + (Height - ScaleHeight) / 2
    ObjIDForm.Show modal
    If ObjIDString <> "" Then period.Text = "ObjectId," + ObjIDString
  End If
End Sub

Private Sub Form_Load()
Dim i
  Top = 0
  Left = 0
  For i = 1 To 16
    priority.AddItem Format$(i)
  Next i
  priority.ListIndex = 0
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

Private Sub OKButton_Click()
Dim p$, i
  SchedString = ""
  p$ = ""
  If PeriodPanel.visible Then
    SchedString = SchedString + period.Text + "|"
    p$ = "#" + priority.Text
  End If
  SchedString = SchedString + "("
  For i = 0 To plist.ListCount - 1
    SchedString = SchedString + plist.List(i) + "|"
  Next i
  If Right$(SchedString, 1) = "|" Then SchedString = Left$(SchedString, Len(SchedString) - 1)
  SchedString = SchedString + ")"
  SchedString = SchedString + p$
  Hide
End Sub

Private Sub PrimType_Click(Index As Integer, Value As Integer)
  If Value Then primtag = Index
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

