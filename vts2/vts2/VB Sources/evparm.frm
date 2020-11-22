VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form EventParamsForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   Caption         =   "Event Parameters"
   ClientHeight    =   3435
   ClientLeft      =   840
   ClientTop       =   2400
   ClientWidth     =   6540
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
   ScaleHeight     =   3435
   ScaleWidth      =   6540
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Height          =   375
      Left            =   5670
      TabIndex        =   44
      Top             =   210
      Width           =   735
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   375
      Left            =   5670
      TabIndex        =   47
      Top             =   630
      Width           =   735
   End
   Begin VB.ComboBox EventType 
      Appearance      =   0  'Flat
      Height          =   300
      Left            =   2940
      Style           =   2  'Dropdown List
      TabIndex        =   2
      Top             =   210
      Width           =   2610
   End
   Begin Threed.SSFrame BitFrame 
      Height          =   2535
      Left            =   210
      TabIndex        =   36
      Top             =   4725
      Visible         =   0   'False
      Width           =   5370
      _Version        =   65536
      _ExtentX        =   9472
      _ExtentY        =   4471
      _StockProps     =   14
      Caption         =   "Bit Change"
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
      Begin VB.CommandButton AddButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "add"
         Height          =   285
         Left            =   4515
         TabIndex        =   39
         Top             =   1050
         Width           =   750
      End
      Begin VB.CommandButton DeleteButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "delete"
         Height          =   285
         Left            =   4515
         TabIndex        =   40
         Top             =   1680
         Width           =   750
      End
      Begin VB.CommandButton ClearButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "clear"
         Height          =   285
         Left            =   4515
         TabIndex        =   41
         Top             =   1995
         Width           =   750
      End
      Begin Threed.SSPanel nsPanel 
         Height          =   330
         Left            =   105
         TabIndex        =   51
         Top             =   3045
         Visible         =   0   'False
         Width           =   3900
         _Version        =   65536
         _ExtentX        =   6879
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
            TabIndex        =   52
            Top             =   15
            Width           =   2535
         End
      End
      Begin Threed.SSPanel psPanel 
         Height          =   330
         Left            =   105
         TabIndex        =   49
         Top             =   2625
         Visible         =   0   'False
         Width           =   3900
         _Version        =   65536
         _ExtentX        =   6879
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
            TabIndex        =   50
            Top             =   15
            Width           =   2535
         End
      End
      Begin Threed.SSPanel bsPanel 
         Height          =   435
         Left            =   105
         TabIndex        =   45
         Top             =   945
         Width           =   3900
         _Version        =   65536
         _ExtentX        =   6879
         _ExtentY        =   767
         _StockProps     =   15
         Caption         =   "bit string"
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
         Begin VB.TextBox BitString 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   210
            Left            =   840
            MaxLength       =   7
            TabIndex        =   46
            Text            =   "0"
            Top             =   105
            Width           =   2940
         End
      End
      Begin Threed.SSPanel bmPanel 
         Height          =   435
         Left            =   105
         TabIndex        =   42
         Top             =   420
         Width           =   3900
         _Version        =   65536
         _ExtentX        =   6879
         _ExtentY        =   767
         _StockProps     =   15
         Caption         =   "bit mask"
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
         Begin VB.TextBox BitMask 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   210
            Left            =   840
            MaxLength       =   7
            TabIndex        =   43
            Text            =   "0"
            Top             =   105
            Width           =   2940
         End
      End
      Begin Threed.SSPanel plpanel 
         Height          =   1035
         Left            =   105
         TabIndex        =   37
         Top             =   1470
         Width           =   4320
         _Version        =   65536
         _ExtentX        =   7620
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
            TabIndex        =   38
            Top             =   15
            Width           =   4290
         End
      End
      Begin VB.Label Label11 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "E"
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
         Left            =   4095
         TabIndex        =   48
         Top             =   1050
         Width           =   330
      End
   End
   Begin Threed.SSFrame COVFrame 
      Height          =   1065
      Left            =   210
      TabIndex        =   1
      Top             =   3570
      Visible         =   0   'False
      Width           =   4005
      _Version        =   65536
      _ExtentX        =   7064
      _ExtentY        =   1879
      _StockProps     =   14
      Caption         =   "Change of Value"
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
      Begin Threed.SSPanel Panel3D3 
         Height          =   225
         Left            =   210
         TabIndex        =   32
         Top             =   630
         Width           =   3480
         _Version        =   65536
         _ExtentX        =   6138
         _ExtentY        =   397
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
         Alignment       =   2
         Autosize        =   3
         Begin VB.TextBox COVValue 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   195
            Left            =   15
            MaxLength       =   7
            TabIndex        =   33
            Text            =   "0"
            Top             =   15
            Width           =   3450
         End
      End
      Begin Threed.SSOption RefPropIncSelect 
         Height          =   225
         Left            =   1470
         TabIndex        =   35
         TabStop         =   0   'False
         Top             =   315
         Width           =   2325
         _Version        =   65536
         _ExtentX        =   4101
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Ref Property Increment"
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
      Begin Threed.SSOption BitMaskSelect 
         Height          =   225
         Left            =   210
         TabIndex        =   34
         Top             =   315
         Width           =   1170
         _Version        =   65536
         _ExtentX        =   2064
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Bit Mask"
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
   Begin Threed.SSPanel Panel3D8 
      Height          =   225
      Left            =   1260
      TabIndex        =   3
      Top             =   210
      Width           =   960
      _Version        =   65536
      _ExtentX        =   1693
      _ExtentY        =   397
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
      Alignment       =   2
      Autosize        =   3
      Begin VB.TextBox TimeDelay 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   195
         Left            =   15
         MaxLength       =   7
         TabIndex        =   4
         Text            =   "0"
         Top             =   15
         Width           =   930
      End
   End
   Begin Threed.SSFrame LimitFrame 
      Height          =   2850
      Left            =   210
      TabIndex        =   0
      Top             =   525
      Visible         =   0   'False
      Width           =   5370
      _Version        =   65536
      _ExtentX        =   9472
      _ExtentY        =   5027
      _StockProps     =   14
      Caption         =   "Limits"
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
      Begin Threed.SSPanel LimitPanel 
         Height          =   1065
         Left            =   105
         TabIndex        =   22
         Top             =   1680
         Width           =   2955
         _Version        =   65536
         _ExtentX        =   5212
         _ExtentY        =   1879
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
         Begin Threed.SSPanel Panel3D6 
            Height          =   225
            Left            =   1155
            TabIndex        =   5
            Top             =   630
            Width           =   1590
            _Version        =   65536
            _ExtentX        =   2805
            _ExtentY        =   397
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
            Alignment       =   2
            Autosize        =   3
            Begin VB.TextBox DeadBand 
               Alignment       =   1  'Right Justify
               Appearance      =   0  'Flat
               BackColor       =   &H00000000&
               BorderStyle     =   0  'None
               ForeColor       =   &H0000FF00&
               Height          =   195
               Left            =   15
               MaxLength       =   7
               TabIndex        =   6
               Text            =   "0"
               Top             =   15
               Width           =   1560
            End
         End
         Begin Threed.SSPanel Panel3D5 
            Height          =   225
            Left            =   1155
            TabIndex        =   25
            Top             =   420
            Width           =   1590
            _Version        =   65536
            _ExtentX        =   2805
            _ExtentY        =   397
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
            Alignment       =   2
            Autosize        =   3
            Begin VB.TextBox HighLimit 
               Alignment       =   1  'Right Justify
               Appearance      =   0  'Flat
               BackColor       =   &H00000000&
               BorderStyle     =   0  'None
               ForeColor       =   &H0000FF00&
               Height          =   195
               Left            =   15
               MaxLength       =   7
               TabIndex        =   26
               Text            =   "0"
               Top             =   15
               Width           =   1560
            End
         End
         Begin Threed.SSPanel Panel3D7 
            Height          =   225
            Left            =   1155
            TabIndex        =   23
            Top             =   210
            Width           =   1590
            _Version        =   65536
            _ExtentX        =   2805
            _ExtentY        =   397
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
            Alignment       =   2
            Autosize        =   3
            Begin VB.TextBox LowLimit 
               Alignment       =   1  'Right Justify
               Appearance      =   0  'Flat
               BackColor       =   &H00000000&
               BorderStyle     =   0  'None
               ForeColor       =   &H0000FF00&
               Height          =   195
               Left            =   15
               MaxLength       =   7
               TabIndex        =   24
               Text            =   "0"
               Top             =   15
               Width           =   1560
            End
         End
         Begin VB.Label Label5 
            Appearance      =   0  'Flat
            AutoSize        =   -1  'True
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "deadband"
            ForeColor       =   &H00808080&
            Height          =   195
            Left            =   105
            TabIndex        =   27
            Top             =   630
            Width           =   855
         End
         Begin VB.Label Label3 
            Appearance      =   0  'Flat
            AutoSize        =   -1  'True
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "high limit"
            ForeColor       =   &H00808080&
            Height          =   195
            Left            =   105
            TabIndex        =   31
            Top             =   420
            Width           =   765
         End
         Begin VB.Label Label8 
            Appearance      =   0  'Flat
            AutoSize        =   -1  'True
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "low limit"
            ForeColor       =   &H00808080&
            Height          =   195
            Left            =   105
            TabIndex        =   29
            Top             =   210
            Width           =   960
         End
      End
      Begin Threed.SSPanel ObjIDPanel 
         Height          =   1275
         Left            =   105
         TabIndex        =   7
         Top             =   315
         Width           =   5055
         _Version        =   65536
         _ExtentX        =   8916
         _ExtentY        =   2249
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
         Begin Threed.SSPanel Panel3D1 
            Height          =   330
            Left            =   0
            TabIndex        =   17
            Top             =   840
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
               TabIndex        =   18
               Top             =   15
               Width           =   3555
            End
         End
         Begin Threed.SSPanel OArrayPanel 
            Height          =   330
            Left            =   3675
            TabIndex        =   15
            Top             =   840
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
               TabIndex        =   16
               Top             =   105
               Width           =   1110
            End
         End
         Begin Threed.SSPanel Panel3D2 
            Height          =   330
            Left            =   3150
            TabIndex        =   10
            Top             =   210
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
               TabIndex        =   11
               Text            =   "0"
               Top             =   75
               Width           =   795
            End
         End
         Begin Threed.SSPanel Panel3D4 
            Height          =   330
            Left            =   0
            TabIndex        =   8
            Top             =   210
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
               TabIndex        =   9
               Top             =   15
               Width           =   3030
            End
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
            Left            =   3045
            TabIndex        =   21
            Top             =   630
            Width           =   165
         End
         Begin VB.Label OArrayLabel 
            Alignment       =   2  'Center
            Appearance      =   0  'Flat
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "array index"
            ForeColor       =   &H00808080&
            Height          =   195
            Left            =   3675
            TabIndex        =   20
            Top             =   630
            Visible         =   0   'False
            Width           =   1170
         End
         Begin VB.Label Label6 
            Appearance      =   0  'Flat
            AutoSize        =   -1  'True
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "property identifier"
            ForeColor       =   &H00808080&
            Height          =   195
            Left            =   105
            TabIndex        =   19
            Top             =   630
            Width           =   1500
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
            Left            =   2520
            TabIndex        =   14
            Top             =   0
            Width           =   165
         End
         Begin VB.Label Label1 
            Appearance      =   0  'Flat
            AutoSize        =   -1  'True
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "object type"
            ForeColor       =   &H00808080&
            Height          =   195
            Left            =   105
            TabIndex        =   13
            Top             =   0
            Width           =   960
         End
         Begin VB.Label Label4 
            Alignment       =   2  'Center
            Appearance      =   0  'Flat
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "instance"
            ForeColor       =   &H00808080&
            Height          =   195
            Left            =   3150
            TabIndex        =   12
            Top             =   0
            Width           =   840
         End
      End
   End
   Begin VB.Label Label7 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "secs"
      ForeColor       =   &H00808080&
      Height          =   195
      Left            =   2310
      TabIndex        =   28
      Top             =   210
      Width           =   405
   End
   Begin VB.Label Label2 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "time delay"
      ForeColor       =   &H00808080&
      Height          =   195
      Left            =   210
      TabIndex        =   30
      Top             =   210
      Width           =   870
   End
End
Attribute VB_Name = "EventParamsForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Private Sub AddButton_Click()
  If bsPanel.visible Then
    If BitString.Text <> "" Then
      plist.AddItem BitString.Text
      plist.ListIndex = plist.NewIndex
    End If
  Else
    plist.AddItem "(" + Trim$(Str$(PropertyStates.ListIndex)) + ")" + PropertyStates.Text + "/(" + Trim$(Str$(NewState.ListIndex)) + ")" + NewState.Text
    plist.ListIndex = plist.NewIndex
  End If
End Sub

Private Sub BitMask_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H31) Then 'not a binary digit
    KeyAscii = 0
  End If
End Sub

Private Sub BitMaskSelect_Click(Value As Integer)
  COVValue.Text = ""
End Sub

Private Sub BitString_KeyPress(KeyAscii As Integer)
  If (KeyAscii < &H30) Or (KeyAscii > &H31) Then 'not a binary digit
    KeyAscii = 0
  End If
End Sub

Private Sub CancelButton_Click()
  EVString = ""
  Hide
End Sub

Private Sub ClearButton_Click()
  If MsgBox("Are you sure that you want to erase all the entries in the properties list?", 33, "Clear Property List") <> 1 Then Exit Sub
  plist.Clear
End Sub

Private Sub COVValue_KeyPress(KeyAscii As Integer)
  If BitMaskSelect.Value Then
    If (KeyAscii < &H30) Or (KeyAscii > &H31) Then 'not a binary digit
      KeyAscii = 0
    End If
  End If
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

Private Sub EventType_Click()
  Select Case EventType.ListIndex
    Case 0                                      'change-of-bitstring
      BitFrame.visible = True
      COVFrame.visible = False
      LimitFrame.visible = False
      psPanel.visible = False
      NSPanel.visible = False
      bmPanel.visible = True
      bsPanel.visible = True
    Case 1                                      'change-of-state
      BitFrame.visible = True
      COVFrame.visible = False
      LimitFrame.visible = False
      psPanel.visible = True
      NSPanel.visible = True
      bmPanel.visible = False
      bsPanel.visible = False
    Case 2                                      'change-of-value
      BitFrame.visible = False
      COVFrame.visible = True
      LimitFrame.visible = False
    Case 3                                      'command-failure
      BitFrame.visible = False
      COVFrame.visible = False
      LimitFrame.visible = True
      LimitFrame.Caption = "Command Failure"
      ObjIDPanel.visible = True
      LimitPanel.visible = False
    Case 4                                      'floating-limit
      BitFrame.visible = False
      COVFrame.visible = False
      LimitFrame.visible = True
      LimitFrame.Caption = "Floating Limit"
      LimitPanel.Top = ObjIDPanel.Top + ObjIDPanel.Height + 90
      ObjIDPanel.visible = True
      LimitPanel.visible = True
    Case 5                                      'out-of-range
      BitFrame.visible = False
      COVFrame.visible = False
      LimitFrame.visible = True
      LimitFrame.Caption = "Out of Range"
      LimitPanel.Top = ObjIDPanel.Top
      ObjIDPanel.visible = False
      LimitPanel.visible = True
  End Select


End Sub

Private Sub Form_Load()
  BitFrame.Top = LimitFrame.Top
  COVFrame.Top = LimitFrame.Top
  psPanel.Top = bmPanel.Top
  NSPanel.Top = bsPanel.Top
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
  EventType.Clear
  EventType.AddItem "change-of-bitstring"
  EventType.AddItem "change-of-state"
  EventType.AddItem "change-of-value"
  EventType.AddItem "command-failure"
  EventType.AddItem "floating-limit"
  EventType.AddItem "out-of-range"
  EventType.ListIndex = 0
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
End Sub

Private Sub ObjectType_Click()
Dim c$, np As Long, i, pn As String * 64, ptype, PropId As Long
  ObjIDHex.Caption = "0x" + Hex$(ObjectType.ListIndex)
  If ObjectType.ListIndex > 0 Then
    OPropertyIdentifier.Clear
    np = VTSAPIgetpropinfo(ObjectType.ListIndex, NumProps, "", 0&, 0&, 0&, 0&)
    For i = 0 To np - 1
      PropId = VTSAPIgetpropinfo(ObjectType.ListIndex, i, pn, ptype, 0&, 0&, 0&)
      OPropertyIdentifier.AddItem pn
      OPropertyIdentifier.ItemData(OPropertyIdentifier.NewIndex) = i
    Next i
    If OPropertyIdentifier.ListCount > 0 Then OPropertyIdentifier.ListIndex = 0
  Else
    ObjIDHex.Caption = ""
  End If

End Sub

Private Sub OKButton_Click()
Dim s$, t$, i
  If TimeDelay.Text = "" Then
    Beep
    MsgBox "You must enter a time delay value!", 48, "Invalid Time Delay"
    Exit Sub
  End If
  EVString = ""
  Select Case EventType.ListIndex
    Case 0                                      'change-of-bitstring
      If plist.ListCount = 0 Or BitMask.Text = "" Then
        Beep
        MsgBox "You must enter a bit mask and at least one bit string!", 48, "Missing Value(s)"
        Exit Sub
      End If
      s$ = "change-of-bitstring," + Trim$(TimeDelay.Text) + ","
      t$ = Trim$(BitMask.Text)
      s$ = s$ + "(8)" + Trim$(Str$(Len(t$))) + "/0b" + t$ + ","
      For i = 0 To plist.ListCount - 1
        t$ = Trim$(plist.List(i))
        s$ = s$ + "(8)" + Trim$(Str$(Len(t$))) + "/0b" + t$
        If i <> plist.ListCount - 1 Then s$ = s$ + ","
      Next i
    Case 1                                      'change-of-state
      If plist.ListCount = 0 Then
        Beep
        MsgBox "You must enter at least one change of state value!", 48, "Missing Value(s)"
        Exit Sub
      End If
      s$ = "change-of-state," + Trim$(TimeDelay.Text) + ","
      For i = 0 To plist.ListCount - 1
        s$ = s$ + Trim$(plist.List(i))
        If i <> plist.ListCount - 1 Then s$ = s$ + ","
      Next i
    Case 2                                      'change-of-value
      If COVValue.Text = "" Then
        Beep
        MsgBox "You must enter a change of value!", 48, "Missing Value"
        Exit Sub
      End If
      s$ = "change-of-value," + Trim$(TimeDelay.Text) + ","
      t$ = Trim$(COVValue.Text)
      If BitMaskSelect.Value Then
        s$ = s$ + "(8)" + Trim$(Str$(Len(t$))) + "/0b" + t$
      Else
        s$ = s$ + "(4)" + t$
      End If
    Case 3                                      'command-failure
      If ObjectType.Text = "" Or OPropertyIdentifier.Text = "" Or ObjectInstance.Text = "" Then
        Beep
        MsgBox "You must select an object type and property!", 48, "Missing Parameter"
        Exit Sub
      End If
      s$ = "command-failure," + Trim$(TimeDelay.Text) + ","
      s$ = s$ + "(" + ObjectType.Text + "," + ObjectInstance.Text + ")"
      s$ = s$ + "," + OPropertyIdentifier.Text
      If OArrayPanel.visible Then s$ = s$ + "[" + Trim$(OArrayIndex.Text) + "]"
    Case 4                                      'floating-limit
      If ObjectType.Text = "" Or OPropertyIdentifier.Text = "" Or ObjectInstance.Text = "" Then
        Beep
        MsgBox "You must select an object type and property!", 48, "Missing Parameter"
        Exit Sub
      End If
      If LowLimit.Text = "" Or HighLimit.Text = "" Or DeadBand.Text = "" Then
        Beep
        MsgBox "You must select limits and deadband values!", 48, "Missing Value(s)"
        Exit Sub
      End If
      s$ = "floating-limit," + Trim$(TimeDelay.Text) + ","
      s$ = s$ + "(" + ObjectType.Text + "," + ObjectInstance.Text + ")"
      s$ = s$ + "," + OPropertyIdentifier.Text
      If OArrayPanel.visible Then s$ = s$ + "[" + Trim$(OArrayIndex.Text) + "]"
      s$ = s$ + "," + LowLimit.Text + "," + HighLimit.Text + "," + DeadBand.Text
    Case 5                                      'out-of-range
      If LowLimit.Text = "" Or HighLimit.Text = "" Or DeadBand.Text = "" Then
        Beep
        MsgBox "You must select limits and deadband values!", 48, "Missing Value(s)"
        Exit Sub
      End If
      s$ = "out-of-range," + Trim$(TimeDelay.Text) + ","
      s$ = s$ + LowLimit.Text + "," + HighLimit.Text + "," + DeadBand.Text
  End Select
  EVString = s$
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

Private Sub PropertyStates_Click()
Dim i
  i = VTSAPIgetpropertystates(PropertyStates.ListIndex, NewState.hwnd)
  If i Then NewState.ListIndex = 0
End Sub

Private Sub RefPropIncSelect_Click(Value As Integer)
  COVValue.Text = ""
End Sub

