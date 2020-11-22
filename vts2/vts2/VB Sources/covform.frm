VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form COVForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "COV Notification/Subscription"
   ClientHeight    =   6480
   ClientLeft      =   270
   ClientTop       =   435
   ClientWidth     =   5850
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
   ScaleHeight     =   6480
   ScaleWidth      =   5850
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton DeleteButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Delete"
      Height          =   330
      Left            =   4935
      TabIndex        =   3
      Top             =   3465
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton EditButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Edit"
      Height          =   330
      Left            =   4935
      TabIndex        =   4
      Top             =   3045
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton AddButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Add"
      Height          =   330
      Left            =   4935
      TabIndex        =   5
      Top             =   2625
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   375
      Left            =   4935
      TabIndex        =   1
      Top             =   585
      Width           =   735
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   375
      Left            =   4935
      TabIndex        =   0
      Top             =   105
      Width           =   735
   End
   Begin Threed.SSFrame MainFrame 
      Height          =   3555
      Left            =   315
      TabIndex        =   9
      Top             =   630
      Width           =   4410
      _Version        =   65536
      _ExtentX        =   7779
      _ExtentY        =   6271
      _StockProps     =   14
      Caption         =   "Subscription"
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
      Begin Threed.SSFrame COVFrame 
         Height          =   1590
         Left            =   120
         TabIndex        =   31
         Top             =   3600
         Visible         =   0   'False
         Width           =   4215
         _Version        =   65536
         _ExtentX        =   7435
         _ExtentY        =   2805
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
         Begin Threed.SSPanel Panel3D5 
            Height          =   330
            Left            =   1470
            TabIndex        =   34
            Top             =   960
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
            Begin VB.TextBox TimeRemaining 
               Appearance      =   0  'Flat
               BackColor       =   &H00000000&
               BorderStyle     =   0  'None
               ForeColor       =   &H0000FF00&
               Height          =   270
               Left            =   30
               MaxLength       =   10
               TabIndex        =   35
               Text            =   "0"
               Top             =   45
               Width           =   1005
            End
         End
         Begin Threed.SSPanel InstancePanel 
            Height          =   315
            Left            =   105
            TabIndex        =   32
            Top             =   525
            Width           =   2430
            _Version        =   65536
            _ExtentX        =   4286
            _ExtentY        =   556
            _StockProps     =   15
            Caption         =   " Initiating Device"
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
               Left            =   1575
               MaxLength       =   7
               TabIndex        =   33
               Text            =   "0000001"
               Top             =   15
               Width           =   840
            End
         End
         Begin Threed.SSCheck ConfirmedCOV 
            Height          =   225
            Left            =   105
            TabIndex        =   2
            Top             =   240
            Width           =   2430
            _Version        =   65536
            _ExtentX        =   4286
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
            Value           =   -1  'True
         End
         Begin VB.Label Label6 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "time remaining"
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
            TabIndex        =   36
            Top             =   945
            Width           =   1275
            WordWrap        =   -1  'True
         End
      End
      Begin Threed.SSFrame SubFrame 
         Height          =   1830
         Left            =   105
         TabIndex        =   22
         Top             =   1590
         Width           =   4215
         _Version        =   65536
         _ExtentX        =   7435
         _ExtentY        =   3228
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
         Alignment       =   1
         Begin Threed.SSFrame ResubscribeFrame 
            Height          =   960
            Left            =   120
            TabIndex        =   25
            Top             =   720
            Width           =   3900
            _Version        =   65536
            _ExtentX        =   6879
            _ExtentY        =   1693
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
            Begin Threed.SSPanel Panel3D1 
               Height          =   330
               Left            =   1155
               TabIndex        =   27
               Top             =   525
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
               Begin VB.TextBox Lifetime 
                  Appearance      =   0  'Flat
                  BackColor       =   &H00000000&
                  BorderStyle     =   0  'None
                  ForeColor       =   &H0000FF00&
                  Height          =   270
                  Left            =   30
                  MaxLength       =   10
                  TabIndex        =   28
                  Text            =   "0"
                  Top             =   45
                  Width           =   1005
               End
            End
            Begin Threed.SSCheck ConfirmedNotifications 
               Height          =   225
               Left            =   210
               TabIndex        =   29
               Top             =   210
               Width           =   3060
               _Version        =   65536
               _ExtentX        =   5398
               _ExtentY        =   397
               _StockProps     =   78
               Caption         =   "Issue Confirmed Notifications?"
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
            Begin Threed.SSCheck Permanent 
               Height          =   225
               Left            =   2310
               TabIndex        =   26
               Top             =   630
               Width           =   1485
               _Version        =   65536
               _ExtentX        =   2619
               _ExtentY        =   397
               _StockProps     =   78
               Caption         =   "Permanent"
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
               Value           =   -1  'True
            End
            Begin VB.Label Label5 
               Alignment       =   1  'Right Justify
               Appearance      =   0  'Flat
               BackColor       =   &H80000005&
               BackStyle       =   0  'Transparent
               Caption         =   "subscription lifetime"
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
               TabIndex        =   30
               Top             =   510
               Width           =   855
               WordWrap        =   -1  'True
            End
         End
         Begin Threed.SSOption Resubscribe 
            Height          =   330
            Left            =   105
            TabIndex        =   24
            Top             =   420
            Width           =   2325
            _Version        =   65536
            _ExtentX        =   4101
            _ExtentY        =   582
            _StockProps     =   78
            Caption         =   "Subscribe/Resubscribe"
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
         Begin Threed.SSOption CancelSubscription 
            Height          =   225
            Left            =   105
            TabIndex        =   23
            TabStop         =   0   'False
            Top             =   210
            Width           =   2220
            _Version        =   65536
            _ExtentX        =   3916
            _ExtentY        =   397
            _StockProps     =   78
            Caption         =   "Cancel Subscription"
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
      Begin Threed.SSFrame ObjIDFrame 
         Height          =   960
         Left            =   105
         TabIndex        =   13
         Top             =   630
         Width           =   4215
         _Version        =   65536
         _ExtentX        =   7435
         _ExtentY        =   1693
         _StockProps     =   14
         Caption         =   "monitoredObjectIdentifier"
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
            TabIndex        =   16
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
               TabIndex        =   17
               Top             =   15
               Width           =   3030
            End
         End
         Begin Threed.SSPanel Panel3D2 
            Height          =   330
            Left            =   3255
            TabIndex        =   14
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
               TabIndex        =   15
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
            TabIndex        =   20
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
            TabIndex        =   19
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
            TabIndex        =   18
            Top             =   210
            Width           =   165
         End
      End
      Begin Threed.SSPanel ProcPanel 
         Height          =   330
         Left            =   1470
         TabIndex        =   10
         Top             =   210
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
            TabIndex        =   11
            Text            =   "0"
            Top             =   45
            Width           =   1005
         End
      End
      Begin VB.Label Label3 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "subscriber process identifier "
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
         Left            =   105
         TabIndex        =   12
         Top             =   210
         Width           =   1275
         WordWrap        =   -1  'True
      End
   End
   Begin Threed.SSFrame ValueFrame 
      Height          =   2175
      Left            =   315
      TabIndex        =   6
      Top             =   4200
      Visible         =   0   'False
      Width           =   5370
      _Version        =   65536
      _ExtentX        =   9472
      _ExtentY        =   3836
      _StockProps     =   14
      Caption         =   "listOfValues"
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
      Begin Threed.SSPanel Panel3D3 
         Height          =   1815
         Left            =   105
         TabIndex        =   7
         Top             =   210
         Width           =   5160
         _Version        =   65536
         _ExtentX        =   9102
         _ExtentY        =   3201
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
         Begin VB.ListBox ValueList 
            Appearance      =   0  'Flat
            Height          =   1785
            Left            =   15
            TabIndex        =   21
            Top             =   15
            Width           =   5130
         End
      End
   End
   Begin Threed.SSOption NotifyCOV 
      Height          =   225
      Left            =   105
      TabIndex        =   37
      TabStop         =   0   'False
      Top             =   420
      Width           =   3060
      _Version        =   65536
      _ExtentX        =   5398
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Issue COV Notification"
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
   Begin Threed.SSOption SubscribeCOV 
      Height          =   330
      Left            =   105
      TabIndex        =   8
      Top             =   105
      Width           =   2955
      _Version        =   65536
      _ExtentX        =   5212
      _ExtentY        =   582
      _StockProps     =   78
      Caption         =   "Subscribe for COV Notification"
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
   Begin VB.Label Label2 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Note: see comment in the General section about hidden frame!"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H000000FF&
      Height          =   645
      Left            =   3255
      TabIndex        =   38
      Top             =   0
      Visible         =   0   'False
      Width           =   1590
      WordWrap        =   -1  'True
   End
End
Attribute VB_Name = "COVForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit
'------------------------Important Note------------------------
'The SSFrame called MainFrame contains two sub frames called
'SubFrame and COVFrame which overlay the same area at runtime.
'To make changes to the layout of COVFrame, you must make
'MainFrame larger during editing so that you can see COVFrame!
'--------------------------------------------------------------
Dim currentobjecttype
Dim objtype

Private Sub AddButton_Click()
Dim i
  PVObjectType = objtype
  PVPropVal = ""
  PVPropCanChange = True
  PropValForm.Show modal
  If ValueList.ListIndex < 0 Then i = ValueList.ListCount Else i = ValueList.ListIndex
  If PVPropVal <> "" Then ValueList.AddItem PVPropVal, i
End Sub

Private Sub CloseButton_Click()
  HideBiModal COVForm
End Sub

Private Sub DeleteButton_Click()
  If ValueList.ListIndex >= 0 Then ValueList.RemoveItem ValueList.ListIndex
End Sub

Private Sub EditButton_Click()
  If ValueList.ListIndex >= 0 Then
    PVObjectType = objtype
    PVPropVal = ValueList.List(ValueList.ListIndex)
    PVPropCanChange = True
    PropValForm.Show modal
    If PVPropVal <> "" Then ValueList.List(ValueList.ListIndex) = PVPropVal
  End If
End Sub

Private Sub Form_Load()
  ObjectInstance.Text = Format$(MinDeviceInstance)
  ObjectType.Clear
  ObjectType.AddItem "(proprietary):"
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
  COVFrame.Top = SubFrame.Top
  PVWriteProperty = False
End Sub

Private Sub Lifetime_Change()
  Permanent.Value = (Val(Lifetime.Text) = 0)
End Sub

Private Sub Lifetime_GotFocus()
  GoSelect Lifetime
End Sub

Private Sub NotifyCOV_Click(Value As Integer)
  If Value Then
    SubFrame.visible = False
    MainFrame.Caption = "Notification"
    COVFrame.visible = True
    ValueFrame.visible = True
    AddButton.visible = True
    EditButton.visible = True
    DeleteButton.visible = True
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

  If currentobjecttype = 0 Then             'its proprietary
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
  currentobjecttype = ObjectType.ListIndex
  If currentobjecttype Then
    ObjIDHex.Caption = "0x" + Hex$(currentobjecttype - 1)
    objtype = currentobjecttype - 1
  Else
    ObjIDHex.Caption = ""
    objtype = -1
  End If
End Sub

Private Sub ObjectType_GotFocus()
    If Left$(ObjectType.Text, Len(propcolon)) = propcolon Then
      ObjectType.SelStart = Len(propcolon)
      ObjectType.SelLength = Len(ObjectType.Text) - Len(propcolon)
    End If
End Sub

Private Sub ObjectType_KeyPress(KeyAscii As Integer)
  If currentobjecttype <> 0 Then
    KeyAscii = 0
    Exit Sub
  End If
End Sub

Private Sub Permanent_Click(Value As Integer)
  If Value Then
    Lifetime.Text = "0"
  End If
End Sub

Private Sub ProcID_GotFocus()
  GoSelect ProcID
End Sub

Private Sub Resubscribe_Click(Value As Integer)
  ResubscribeFrame.visible = Value
End Sub

Private Sub SendButton_Click()
Dim apdu As String * 4096, alen, conf
  If objtype = -1 Then
      Beep
      MsgBox "You must specify an object type!", 48, "No Object Type"
      ObjectType.SetFocus
      Exit Sub
  End If
  If ObjectInstance.ForeColor = Red Then
      Beep
      MsgBox "The instance must be" + Str$(MinDeviceInstance) + "..4194303!", 48, "Invalid Instance"
      ObjectInstance.SetFocus
      Exit Sub
  End If
  If SubscribeCOV.Value Then                    'it's a subscription
    alen = eSubscribeCOVService(apdu, Val(ProcID.Text), vbOBJECTID(objtype, Val(ObjectInstance)), CancelSubscription.Value, ConfirmedNotifications.Value, Val(Lifetime.Text))
    conf = True
  Else                                          'it's a notification
    If DeviceInstance.ForeColor = Red Then
      Beep
      MsgBox "The instance must be" + Str$(MinDeviceInstance) + "..4194303!", 48, "Invalid Instance"
      DeviceInstance.SetFocus
      Exit Sub
    End If
    conf = ConfirmedCOV.Value
    alen = eCOVNotificationService(apdu, 4096, conf, Val(ProcID.Text), Val(DeviceInstance.Text), vbOBJECTID(objtype, Val(ObjectInstance)), Val(TimeRemaining.Text), ValueList.hwnd)
    If alen = 0 Then
      Beep
      MsgBox "The COV Notification is too complex to encode in this version! Try removing some of items in the listOfValues.", 48, "Encoding Too Large"
      ValueList.SetFocus
      Exit Sub
    End If
  End If
  SendNPDU Left$(apdu, alen), conf
End Sub

Private Sub SubscribeCOV_Click(Value As Integer)
  If Value Then
    COVFrame.visible = False
    ValueFrame.visible = False
    AddButton.visible = False
    EditButton.visible = False
    DeleteButton.visible = False
    MainFrame.Caption = "Subscription"
    SubFrame.visible = True
  End If
End Sub

