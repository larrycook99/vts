VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form WhoForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Who-Is/I-Am/Who-Has/I-Have"
   ClientHeight    =   4875
   ClientLeft      =   5685
   ClientTop       =   1515
   ClientWidth     =   5250
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
   ScaleHeight     =   4875
   ScaleWidth      =   5250
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Height          =   315
      Left            =   4410
      TabIndex        =   17
      Top             =   525
      Width           =   735
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   315
      Left            =   4410
      TabIndex        =   16
      Top             =   105
      Width           =   735
   End
   Begin Threed.SSPanel IAmPanel 
      Height          =   1275
      Left            =   105
      TabIndex        =   18
      Top             =   1995
      Visible         =   0   'False
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
      Autosize        =   3
      Begin Threed.SSPanel Panel3D6 
         Height          =   330
         Left            =   105
         TabIndex        =   34
         Top             =   450
         Width           =   3270
         _Version        =   65536
         _ExtentX        =   5768
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
         Autosize        =   3
         Begin VB.ComboBox Segmentation 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   35
            Top             =   15
            Width           =   3240
         End
      End
      Begin Threed.SSPanel Panel3D5 
         Height          =   330
         Left            =   1890
         TabIndex        =   19
         Top             =   840
         Width           =   3060
         _Version        =   65536
         _ExtentX        =   5398
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
         Autosize        =   3
         Begin VB.ComboBox VendorNames 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   10
            Top             =   15
            Width           =   3030
         End
      End
      Begin Threed.SSPanel Panel3D3 
         Height          =   315
         Left            =   105
         TabIndex        =   26
         Top             =   840
         Width           =   1695
         _Version        =   65536
         _ExtentX        =   2990
         _ExtentY        =   556
         _StockProps     =   15
         Caption         =   " Vendor ID"
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
         Begin VB.TextBox VendorID 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   285
            Left            =   1050
            MaxLength       =   5
            TabIndex        =   9
            Top             =   15
            Width           =   630
         End
      End
      Begin Threed.SSPanel Panel3D1 
         Height          =   315
         Left            =   105
         TabIndex        =   27
         Top             =   90
         Width           =   3270
         _Version        =   65536
         _ExtentX        =   5768
         _ExtentY        =   556
         _StockProps     =   15
         Caption         =   " Max APDU Length Accepted"
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
         Begin VB.TextBox MaxAPDULen 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   285
            Left            =   2730
            MaxLength       =   4
            TabIndex        =   8
            Text            =   "1470"
            Top             =   15
            Width           =   525
         End
      End
   End
   Begin Threed.SSPanel ObjIDPanel 
      Height          =   645
      Left            =   945
      TabIndex        =   28
      Top             =   3360
      Visible         =   0   'False
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   1138
      _StockProps     =   15
      Caption         =   "   object type"
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
      Begin Threed.SSPanel Panel3D4 
         Height          =   345
         Left            =   120
         TabIndex        =   30
         Top             =   240
         Width           =   3045
         _Version        =   65536
         _ExtentX        =   5371
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
         Autosize        =   3
         Begin VB.ComboBox ObjectType 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   315
            Left            =   15
            Sorted          =   -1  'True
            TabIndex        =   12
            Top             =   15
            Width           =   3015
         End
      End
      Begin Threed.SSPanel Panel3D2 
         Height          =   330
         Left            =   3255
         TabIndex        =   31
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
            TabIndex        =   13
            Text            =   "0000000"
            Top             =   75
            Width           =   795
         End
      End
      Begin VB.Label ObjIDHex 
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
         Left            =   1575
         TabIndex        =   33
         Top             =   0
         Width           =   165
      End
      Begin VB.Label Label4 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "instance"
         ForeColor       =   &H80000008&
         Height          =   195
         Left            =   3360
         TabIndex        =   32
         Top             =   0
         Width           =   735
      End
   End
   Begin Threed.SSPanel RangePanel 
      Height          =   1065
      Left            =   1995
      TabIndex        =   21
      Top             =   840
      Visible         =   0   'False
      Width           =   2220
      _Version        =   65536
      _ExtentX        =   3916
      _ExtentY        =   1879
      _StockProps     =   15
      Caption         =   "Range Limits  "
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
      Alignment       =   3
      Begin Threed.SSPanel LimitPanel 
         Height          =   330
         Index           =   1
         Left            =   1155
         TabIndex        =   23
         Top             =   630
         Width           =   960
         _Version        =   65536
         _ExtentX        =   1693
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
         Begin VB.TextBox RangeHigh 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   240
            Left            =   30
            MaxLength       =   7
            TabIndex        =   7
            Text            =   "4194303"
            Top             =   75
            Width           =   825
         End
      End
      Begin Threed.SSPanel LimitPanel 
         Height          =   330
         Index           =   0
         Left            =   1155
         TabIndex        =   22
         Top             =   210
         Width           =   960
         _Version        =   65536
         _ExtentX        =   1693
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
         Begin VB.TextBox RangeLow 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   240
            Left            =   30
            MaxLength       =   7
            TabIndex        =   6
            Text            =   "1"
            Top             =   75
            Width           =   795
         End
      End
      Begin VB.Label Label2 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "HighLimit:"
         ForeColor       =   &H80000008&
         Height          =   195
         Left            =   210
         TabIndex        =   25
         Top             =   735
         Width           =   855
      End
      Begin VB.Label Label1 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "Low Limit:"
         ForeColor       =   &H80000008&
         Height          =   195
         Left            =   210
         TabIndex        =   24
         Top             =   315
         Width           =   870
      End
   End
   Begin Threed.SSPanel InstancePanel 
      Height          =   315
      Left            =   1785
      TabIndex        =   20
      Top             =   105
      Visible         =   0   'False
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
         TabIndex        =   4
         Text            =   "0000001"
         Top             =   15
         Width           =   840
      End
   End
   Begin Threed.SSPanel ObjNamePanel 
      Height          =   645
      Left            =   945
      TabIndex        =   29
      Top             =   4095
      Visible         =   0   'False
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   1138
      _StockProps     =   15
      Caption         =   "  object name"
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
      Begin VB.ComboBox ObjectName 
         Appearance      =   0  'Flat
         BackColor       =   &H00FFFFFF&
         ForeColor       =   &H00000000&
         Height          =   315
         Left            =   105
         TabIndex        =   15
         Top             =   210
         Width           =   4050
      End
   End
   Begin Threed.SSCheck ShowObjID 
      Height          =   330
      Left            =   0
      TabIndex        =   11
      Top             =   3570
      Visible         =   0   'False
      Width           =   855
      _Version        =   65536
      _ExtentX        =   1508
      _ExtentY        =   582
      _StockProps     =   78
      Caption         =   "ObjID"
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
   Begin Threed.SSCheck ShowName 
      Height          =   330
      Left            =   0
      TabIndex        =   14
      Top             =   4305
      Visible         =   0   'False
      Width           =   855
      _Version        =   65536
      _ExtentX        =   1508
      _ExtentY        =   582
      _StockProps     =   78
      Caption         =   "Name"
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
   Begin Threed.SSCheck InstanceRange 
      Height          =   225
      Left            =   1785
      TabIndex        =   5
      Top             =   525
      Visible         =   0   'False
      Width           =   2535
      _Version        =   65536
      _ExtentX        =   4471
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Device Instance Range?"
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
   Begin Threed.SSOption Service 
      Height          =   225
      Index           =   3
      Left            =   105
      TabIndex        =   3
      TabStop         =   0   'False
      Top             =   735
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Who-Has"
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
   Begin Threed.SSOption Service 
      Height          =   225
      Index           =   2
      Left            =   105
      TabIndex        =   2
      Top             =   525
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Who-Is"
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
   Begin Threed.SSOption Service 
      Height          =   225
      Index           =   1
      Left            =   105
      TabIndex        =   1
      TabStop         =   0   'False
      Top             =   315
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "I-Have"
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
   Begin Threed.SSOption Service 
      Height          =   225
      Index           =   0
      Left            =   105
      TabIndex        =   0
      TabStop         =   0   'False
      Top             =   105
      Width           =   1380
      _Version        =   65536
      _ExtentX        =   2434
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "I-Am"
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
Attribute VB_Name = "WhoForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Const propcolon = "(proprietary):"
Const IAmButton = 0
Const IHaveButton = 1
Const WhoIsButton = 2
Const WhoHasButton = 3

Dim loaded
Dim selectedservice
Dim currentobjecttype
Dim inShowName, inShowObjID

Private Sub CloseButton_Click()
  HideBiModal WhoForm
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
Dim objshift
  loaded = False
  objshift = ObjIDPanel.Top - IAmPanel.Top
  IAmPanel.Top = SendButton.Top + (SendButton.Top - CloseButton.Top)
  ShowObjID.Top = ShowObjID.Top - objshift
  ShowName.Top = ShowName.Top - objshift
  ObjIDPanel.Top = ObjIDPanel.Top - objshift
  ObjNamePanel.Top = ObjNamePanel.Top - objshift
  Service(WhoIsButton).Value = 1
  DeviceInstance.Text = Format$(MinDeviceInstance, "0000000")
  ReadVendors WhoForm.VendorNames           '                             ***218
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
  Segmentation.Clear
  Segmentation.AddItem "Segmented Transmit and Receive"
  Segmentation.AddItem "Segmented Transmit"
  Segmentation.AddItem "Segmented Receive"
  Segmentation.AddItem "No Segmentation"
  Segmentation.ListIndex = 3
  loaded = True
End Sub

Private Sub InstanceRange_Click(Value As Integer)
  RangePanel.visible = Value
End Sub

Private Sub MaxAPDULen_Change()
Dim ma
  If MaxAPDULen.Text = "" Then Exit Sub
  ma = Val(MaxAPDULen.Text)
  If (ma < 50) Or (ma > 1470) Then
    Beep
    MaxAPDULen.ForeColor = Red
  Else
    MaxAPDULen.ForeColor = Green
  End If
End Sub

Private Sub MaxAPDULen_GotFocus()
  GoSelect MaxAPDULen
End Sub

Private Sub ObjectInstance_Change()
Dim oi As Long
  If ObjectInstance.Text = "" Then Exit Sub
  oi = Val(ObjectInstance.Text)
  If (oi < MinDeviceInstance) Or (oi > 4194303) Then
    Beep
    ObjectInstance.ForeColor = Red
  Else
    ObjectInstance.ForeColor = Green
  End If
End Sub

Private Sub ObjectInstance_GotFocus()
  GoSelect ObjectInstance
End Sub

Private Sub ObjectName_LostFocus()
Dim i
  For i = 0 To ObjectName.ListCount - 1
    If ObjectName.List(i) = ObjectName.Text Then Exit Sub       'already have it
  Next i
  If ObjectName.ListCount = 8 Then ObjectName.RemoveItem 0
  ObjectName.AddItem ObjectName.Text
End Sub

Private Sub ObjectType_Change()
Dim otyp, c$

  If currentobjecttype > 0 Then              'its a standard object
    otyp = currentobjecttype - 1
  Else                                          'its proprietary
    c$ = ObjectType.Text
    If Left$(c$, Len(propcolon)) = propcolon Then
      otyp = Val(Mid$(c$, 15))
    Else
      otyp = 0
    End If
    If otyp < 128 Then
      Beep
      ObjectType.ForeColor = Red
    Else
      ObjectType.ForeColor = Black
    End If
  End If
  ObjIDHex.Caption = "0x" + Hex$(otyp)
End Sub

Private Sub ObjectType_Click()
  ObjectType.ForeColor = Black
  currentobjecttype = ObjectType.ListIndex
  If currentobjecttype Then
    ObjIDHex.Caption = "0x" + Hex$(currentobjecttype - 1)
  Else
    ObjIDHex.Caption = ""
  End If
End Sub

Private Sub ObjectType_GotFocus()
    If Left$(ObjectType.Text, Len(propcolon)) = propcolon Then
      ObjectType.SelStart = Len(propcolon)
      ObjectType.SelLength = 5
    End If
End Sub

Private Sub RangeHigh_Change()
Dim rh As Long
  If RangeHigh.Text = "" Then
    RangeHigh.Text = "4194303"
    Exit Sub
  End If
  rh = Val(RangeHigh.Text)
  If (rh < MinDeviceInstance) Or (rh > 4194303) Or (rh < Val(RangeLow.Text)) Then
    Beep
    RangeHigh.ForeColor = Red
  Else
    RangeHigh.ForeColor = Green
  End If
End Sub

Private Sub RangeHigh_GotFocus()
  GoSelect RangeHigh
End Sub

Private Sub RangeLow_Change()
Dim rl As Long
  If RangeLow.Text = "" Then
    RangeLow.Text = Format$(MinDeviceInstance)
    Exit Sub
  End If
  rl = Val(RangeLow.Text)
  If (rl < MinDeviceInstance) Or (rl > 4194303) Or (rl > Val(RangeHigh.Text)) Then
    Beep
    RangeLow.ForeColor = Red
  Else
    RangeLow.ForeColor = Green
  End If
End Sub

Private Sub RangeLow_GotFocus()
  GoSelect RangeLow
End Sub

Private Sub SendButton_Click()
Dim alen, apdu As String * 128, ins1 As Long, ins2 As Long, otyp, obinst As Long, c$
  Select Case selectedservice
    Case IAmButton
      If DeviceInstance.Text = "" Then
        Beep
        MsgBox "You must specify a Device Instance!", 48, "No Device Instance Specified"
        DeviceInstance.SetFocus
        Exit Sub
      End If
      ins1 = Val(DeviceInstance.Text)
      If (ins1 < MinDeviceInstance) Or (ins1 > 4194303) Then
         Beep
         MsgBox "The Device Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid Device Instance"
         DeviceInstance.SetFocus
         Exit Sub
      End If
      If VendorID.Text = "" Then
        Beep
        MsgBox "You must specify a VendorID!", 48, "No VendorID Specified"
        VendorID.SetFocus
        Exit Sub
      End If
      alen = eIAmService(apdu, ins1, Val(MaxAPDULen.Text), Segmentation.ListIndex, Val(VendorID.Text))
    Case IHaveButton
      If DeviceInstance.Text = "" Then
        Beep
        MsgBox "You must specify a Device Instance!", 48, "No Device Instance Specified"
        DeviceInstance.SetFocus
        Exit Sub
      End If
      ins1 = Val(DeviceInstance.Text)
      If (ins1 < MinDeviceInstance) Or (ins1 > 4194303) Then
         Beep
         MsgBox "The Device Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid Device Instance"
         DeviceInstance.SetFocus
         Exit Sub
      End If
      If currentobjecttype > 0 Then              'its a standard object
        otyp = currentobjecttype - 1
      Else                                          'its proprietary
        c$ = ObjectType.Text
        If Left$(c$, Len(propcolon)) = propcolon Then
          otyp = Val(Mid$(c$, 15))
        Else
          otyp = 0
        End If
        If otyp < 128 Then
          Beep
          MsgBox "Proprietary object types must use values greater than 127!", 48, "Invalid Object Type"
          ObjectType.SetFocus
          Exit Sub
        End If
      End If
      If ObjectInstance.Text = "" Then
        Beep
        MsgBox "You must specify an Object Instance!", 48, "No Object Instance Specified"
        ObjectInstance.SetFocus
        Exit Sub
      End If
      ins2 = Val(ObjectInstance.Text)
      If (ins2 < MinDeviceInstance) Or (ins2 > 4194303) Then
         Beep
         MsgBox "The Object Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid Object Instance"
         ObjectInstance.SetFocus
         Exit Sub
      End If
      alen = eIHaveService(apdu, ins1, vbOBJECTID(otyp, ins2), ObjectName.Text)
    Case WhoIsButton
      If InstanceRange.Value Then
        If RangeLow.Text = "" Then
          Beep
          MsgBox "You must specify a Device Instance!", 48, "No Device Instance Specified"
          RangeLow.SetFocus
          Exit Sub
        End If
        ins1 = Val(RangeLow.Text)
        If (ins1 < MinDeviceInstance) Or (ins1 > 4194303) Then
           Beep
           MsgBox "The Device Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid Device Instance"
           RangeLow.SetFocus
           Exit Sub
        End If
        If RangeHigh.Text = "" Then
          Beep
          MsgBox "You must specify a Device Instance!", 48, "No Device Instance Specified"
          RangeHigh.SetFocus
          Exit Sub
        End If
        ins2 = Val(RangeHigh.Text)
        If (ins2 < MinDeviceInstance) Or (ins2 > 4194303) Then
           Beep
           MsgBox "The Device Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid Device Instance"
           RangeHigh.SetFocus
           Exit Sub
        End If
        If ins2 < ins1 Then
          Beep
          MsgBox "The High Limit must be larger than the Low Limit!", 48, "Invalid Range"
          RangeLow.SetFocus
          Exit Sub
        End If
      End If
      alen = eWhoIsService(apdu, InstanceRange.Value, ins1, ins2)
    Case WhoHasButton
      If InstanceRange.Value Then
        If RangeLow.Text = "" Then
          Beep
          MsgBox "You must specify a Device Instance!", 48, "No Device Instance Specified"
          RangeLow.SetFocus
          Exit Sub
        End If
        ins1 = Val(RangeLow.Text)
        If (ins1 < MinDeviceInstance) Or (ins1 > 4194303) Then
           Beep
           MsgBox "The Device Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid Device Instance"
           RangeLow.SetFocus
           Exit Sub
        End If
        If RangeHigh.Text = "" Then
          Beep
          MsgBox "You must specify a Device Instance!", 48, "No Device Instance Specified"
          RangeHigh.SetFocus
          Exit Sub
        End If
        ins2 = Val(RangeHigh.Text)
        If (ins2 < MinDeviceInstance) Or (ins2 > 4194303) Then
           Beep
           MsgBox "The Device Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid Device Instance"
           RangeHigh.SetFocus
           Exit Sub
        End If
        If ins2 < ins1 Then
          Beep
          MsgBox "The High Limit must be larger than the Low Limit!", 48, "Invalid Range"
          RangeLow.SetFocus
          Exit Sub
        End If
      End If
      If ShowObjID.Value Then                   'object ID
        If currentobjecttype > 0 Then              'its a standard object
          otyp = currentobjecttype - 1
        Else                                          'its proprietary
          c$ = ObjectType.Text
          If Left$(c$, Len(propcolon)) = propcolon Then
            otyp = Val(Mid$(c$, 15))
          Else
            otyp = 0
          End If
          If otyp < 128 Then
            Beep
            MsgBox "Proprietary object types must use values greater than 127!", 48, "Invalid Object Type"
            ObjectType.SetFocus
            Exit Sub
          End If
        End If
        If ObjectInstance.Text = "" Then
          Beep
          MsgBox "You must specify an Object Instance!", 48, "No Object Instance Specified"
          ObjectInstance.SetFocus
          Exit Sub
        End If
        obinst = Val(ObjectInstance.Text)
        If (obinst < MinDeviceInstance) Or (obinst > 4194303) Then
           Beep
           MsgBox "The Object Instance must range from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Invalid Object Instance"
           ObjectInstance.SetFocus
           Exit Sub
        End If
      Else
        otyp = &HFFFF
        obinst = &H3FFFFF
      End If
      alen = eWhoHasService(apdu, InstanceRange.Value, ins1, ins2, vbOBJECTID(otyp, obinst), ObjectName.Text)
  End Select
  SendNPDU Left$(apdu, alen), False
End Sub

Private Sub Service_Click(Index As Integer, Value As Integer)
  If Value Then                                 'this is the one getting selected
    selectedservice = Index
    Select Case Index
      Case IAmButton
        InstancePanel.visible = True
        IAmPanel.visible = True
        InstanceRange.visible = False
        RangePanel.visible = False
        ShowObjID.visible = False
        ShowName.visible = False
        ObjIDPanel.visible = False
        ObjNamePanel.visible = False
        If loaded Then DeviceInstance.SetFocus
      Case IHaveButton
        InstancePanel.visible = True
        IAmPanel.visible = False
        InstanceRange.visible = False
        RangePanel.visible = False
        ShowObjID.visible = False
        ShowName.visible = False
        ObjIDPanel.visible = True
        ObjNamePanel.visible = True
        If loaded Then ObjectType.SetFocus
      Case WhoIsButton
        InstancePanel.visible = False
        IAmPanel.visible = False
        InstanceRange.visible = True
        RangePanel.visible = InstanceRange.Value
        ShowObjID.visible = False
        ShowName.visible = False
        ObjIDPanel.visible = False
        ObjNamePanel.visible = False
        If loaded Then InstanceRange.SetFocus
      Case WhoHasButton
        InstancePanel.visible = False
        IAmPanel.visible = False
        InstanceRange.visible = True
        RangePanel.visible = InstanceRange.Value
        ShowObjID.visible = True
        ShowName.visible = True
        If (ShowObjID.Value = ShowName.Value) Then ShowName.Value = Not ShowObjID.Value
        ObjIDPanel.visible = ShowObjID.Value
        ObjNamePanel.visible = ShowName.Value
    End Select
  End If
End Sub

Private Sub ShowName_Click(Value As Integer)
  If inShowName Then Exit Sub
  inShowName = True
  ShowObjID.Value = Not Value
  inShowName = False
  ObjIDPanel.visible = Not Value
  ObjNamePanel.visible = Value
End Sub

Private Sub ShowObjID_Click(Value As Integer)
  If inShowObjID Then Exit Sub
  inShowObjID = True
  ShowName.Value = Not Value
  inShowObjID = False
  ObjNamePanel.visible = Not Value
  ObjIDPanel.visible = Value
End Sub

Private Sub VendorID_Change()
Dim i, v                                    '                       ***218 Begin
  If VendorID.Text = "" Then
    VendorNames.ListIndex = -1
    Exit Sub
  End If
  v = Val(VendorID.Text)
  For i = 0 To VendorNames.ListCount - 1
    If VendorNames.ItemData(i) = v Then
      VendorNames.ListIndex = i
      Exit Sub
    End If
  Next i
  VendorNames.ListIndex = 0                 '                       ***218 End
End Sub

Private Sub VendorID_GotFocus()
  GoSelect VendorID
End Sub

Private Sub VendorNames_Click()
  If VendorNames.ListIndex > 0 Then
    VendorID = Format$(VendorNames.ItemData(VendorNames.ListIndex)) '***218
  End If
End Sub

