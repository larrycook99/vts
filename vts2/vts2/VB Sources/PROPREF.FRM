VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "Threed32.ocx"
Begin VB.Form PropRefForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Edit Property References"
   ClientHeight    =   5865
   ClientLeft      =   1590
   ClientTop       =   1500
   ClientWidth     =   5310
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
   ScaleHeight     =   5865
   ScaleWidth      =   5310
   ShowInTaskbar   =   0   'False
   Begin VB.ComboBox priority 
      Appearance      =   0  'Flat
      Height          =   300
      Left            =   4410
      Style           =   2  'Dropdown List
      TabIndex        =   21
      Top             =   1260
      Visible         =   0   'False
      Width           =   720
   End
   Begin VB.CommandButton ClearButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "clear"
      Height          =   285
      Left            =   2730
      TabIndex        =   2
      Top             =   3885
      Visible         =   0   'False
      Width           =   645
   End
   Begin VB.CommandButton DeleteButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "delete"
      Height          =   285
      Left            =   3465
      TabIndex        =   4
      Top             =   3885
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton AddButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "add"
      Height          =   285
      Left            =   4305
      TabIndex        =   5
      Top             =   3885
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   375
      Left            =   4440
      TabIndex        =   1
      Top             =   525
      Width           =   735
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   4440
      TabIndex        =   0
      Top             =   105
      Width           =   735
   End
   Begin Threed.SSFrame ActFrame 
      Height          =   780
      Left            =   105
      TabIndex        =   27
      Top             =   3015
      Width           =   3795
      _Version        =   65536
      _ExtentX        =   6694
      _ExtentY        =   1376
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
      Begin Threed.SSPanel PostDelayPanel 
         Height          =   330
         Left            =   210
         TabIndex        =   31
         Top             =   375
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
         Begin VB.TextBox PostDelay 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   240
            Left            =   30
            MaxLength       =   10
            TabIndex        =   32
            Top             =   0
            Width           =   1110
         End
      End
      Begin Threed.SSCheck WriteSuccessCheck 
         Height          =   225
         Left            =   1785
         TabIndex        =   28
         Top             =   345
         Width           =   1800
         _Version        =   65536
         _ExtentX        =   3175
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Write Successful"
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
      Begin Threed.SSCheck QuitCheck 
         Height          =   225
         Left            =   1785
         TabIndex        =   29
         Top             =   135
         Width           =   1800
         _Version        =   65536
         _ExtentX        =   3175
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Quit on Failure"
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
      Begin Threed.SSCheck PostDelayCheck 
         Height          =   225
         Left            =   210
         TabIndex        =   30
         Top             =   135
         Width           =   1275
         _Version        =   65536
         _ExtentX        =   2249
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Post Delay"
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
   Begin Threed.SSFrame ObjIDFrame 
      Height          =   1485
      Left            =   105
      TabIndex        =   13
      Top             =   105
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   2619
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
      Begin Threed.SSPanel DInstancePanel 
         Height          =   330
         Left            =   3255
         TabIndex        =   26
         Top             =   315
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
         Begin VB.TextBox DInstance 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   240
            Left            =   30
            MaxLength       =   7
            TabIndex        =   22
            Text            =   "0"
            Top             =   75
            Width           =   795
         End
      End
      Begin Threed.SSPanel Panel3D2 
         Height          =   330
         Left            =   3255
         TabIndex        =   16
         Top             =   840
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
            TabIndex        =   17
            Text            =   "0"
            Top             =   75
            Width           =   795
         End
      End
      Begin Threed.SSPanel Panel3D4 
         Height          =   330
         Left            =   105
         TabIndex        =   14
         Top             =   840
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
            TabIndex        =   15
            Top             =   15
            Width           =   3030
         End
      End
      Begin Threed.SSCheck DInstanceCheck 
         Height          =   225
         Left            =   1365
         TabIndex        =   24
         Top             =   315
         Width           =   1800
         _Version        =   65536
         _ExtentX        =   3175
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Device Object ID"
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
      Begin VB.Label DInstanceLabel 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "instance"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   3255
         TabIndex        =   25
         Top             =   105
         Width           =   840
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
         TabIndex        =   20
         Top             =   630
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
         Left            =   210
         TabIndex        =   19
         Top             =   630
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
         Left            =   3255
         TabIndex        =   18
         Top             =   630
         Width           =   840
      End
   End
   Begin Threed.SSPanel plpanel 
      Height          =   1620
      Left            =   105
      TabIndex        =   11
      Top             =   4200
      Width           =   5160
      _Version        =   65536
      _ExtentX        =   9102
      _ExtentY        =   2858
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
         Height          =   1590
         Left            =   15
         MultiSelect     =   2  'Extended
         TabIndex        =   12
         Top             =   15
         Width           =   5130
      End
   End
   Begin Threed.SSFrame PropFrame 
      Height          =   1275
      Left            =   105
      TabIndex        =   7
      Top             =   1680
      Width           =   5160
      _Version        =   65536
      _ExtentX        =   9102
      _ExtentY        =   2249
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
      Begin VB.CommandButton EditButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Edit"
         Height          =   285
         Left            =   4200
         TabIndex        =   36
         Top             =   840
         Visible         =   0   'False
         Width           =   750
      End
      Begin Threed.SSPanel ValuePanel 
         Height          =   330
         Left            =   105
         TabIndex        =   37
         Top             =   840
         Width           =   4005
         _Version        =   65536
         _ExtentX        =   7064
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
         Begin VB.Label Value 
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   300
            Left            =   15
            TabIndex        =   38
            Top             =   15
            Width           =   3975
         End
      End
      Begin Threed.SSPanel ArrayPanel 
         Height          =   330
         Left            =   3780
         TabIndex        =   34
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
            TabIndex        =   35
            Top             =   0
            Width           =   1110
         End
      End
      Begin Threed.SSPanel PIPanel 
         Height          =   345
         Left            =   105
         TabIndex        =   8
         Top             =   480
         Width           =   3585
         _Version        =   65536
         _ExtentX        =   6324
         _ExtentY        =   609
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
         Alignment       =   1
         Autosize        =   3
         Begin VB.ComboBox PropertyIdentifier 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   315
            Left            =   15
            TabIndex        =   6
            Top             =   15
            Width           =   3555
         End
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
         TabIndex        =   33
         Top             =   210
         Visible         =   0   'False
         Width           =   1170
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
         TabIndex        =   9
         Top             =   210
         Width           =   165
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
         TabIndex        =   3
         Top             =   210
         Width           =   1500
      End
   End
   Begin Threed.SSCheck MultipleRefs 
      Height          =   225
      Left            =   105
      TabIndex        =   10
      Top             =   3885
      Width           =   2535
      _Version        =   65536
      _ExtentX        =   4471
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Multiple References?"
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
      TabIndex        =   23
      Top             =   1050
      Visible         =   0   'False
      Width           =   600
   End
End
Attribute VB_Name = "PropRefForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Dim PropId As Long
Dim ptype
Dim nheight
Dim InForm

Private Sub AddButton_Click()
Dim p$, i
  If ObjIDFrame.visible Then
    If ObjectType.ListIndex = -1 Then
      Beep
      MsgBox "You must specify an object type!", 48, "No Object Type"
      ObjectType.SetFocus
      Exit Sub
    End If
    If PropertyIdentifier.ListIndex = -1 Then
      Beep
      MsgBox "You must specify a property!", 48, "No Property Selected"
      PropertyIdentifier.SetFocus
      Exit Sub
    End If
    p$ = PropertyIdentifier.Text
    If ArrayPanel.visible Then p$ = p$ + "[" + ArrayIndex.Text + "]"
    plist.AddItem p$
  Else
    plist.AddItem PropertyIdentifier.Text
  End If
  plist.ItemData(plist.NewIndex) = PropId 'save propid because it's hard to get again ***218
  plist.ListIndex = plist.NewIndex        '                                           ***218
End Sub

Private Sub CancelButton_Click()
  PropertyIdentifier.Clear
  plist.Clear
  PRString = ""
  InForm = False
'  HideBiModal PropRefForm
  Hide                        'changed to this because we are called from PropVal form ***218
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
Dim i, p$, savept, primtag
  Value.ForeColor = Green
  Select Case ptype
  Case ptT
    primtag = 11
  Case ptD
    primtag = 12
  Case ptBits
    primtag = 8
  Case ptEbool
    primtag = 1
  Case ptET
    primtag = 9
  Case ptFloat
    primtag = 4
  Case ptUW, ptUD, ptU127, ptU16
    primtag = 2
  Case ptSW, ptSsint  ' MAG add ptSsint to case 13 FEB 2001
    primtag = 3
  Case ptS10, ptS32, ptS64, ptS132
    primtag = 7
  Case Else
    Value.ForeColor = Red
    Value.Caption = "this property cannot be referenced!"
    Exit Sub
  End Select
  p$ = ""
  Select Case primtag
  Case 0                        'null
    p$ = "(0)Null"
  Case 1                        'boolean
    EnumPointer = 1
    EnumForm.Move Left + SchedForm.Left + (Width - ScaleWidth) / 2, Top + SchedForm.Top + (Height - ScaleHeight) / 2
    EnumForm.Show modal
    If EnumValue <> -1 Then p$ = "(" + Right$(Str$(primtag), 1) + ")" + Format$(EnumValue)
  Case 2, 3, 4, 5, 6, 7         'uint, int, real, dbl, octet str, char str
    savept = ParseType
    ParseType = ptS132
    NumsForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    NumsForm.Show modal
    If NumsString <> "" Then p$ = "(" + Right$(Str$(primtag), 1) + ")" + NumsString
    ParseType = savept
  Case 8                        'bit string
    BitNames = ""
    BitsForm.Move Left + SchedForm.Left + (Width - ScaleWidth) / 2, Top + SchedForm.Top + (Height - ScaleHeight) / 2
    BitsForm.Show modal
    If BitNames <> "" Then p$ = "(" + Right$(Str$(primtag), 1) + ")" + "," + BitNames + "," + Format$(BitsValue)
  Case 9                        'enum
    EnumPointer = 0             'allow selection from List
    EnumForm.Move Left + SchedForm.Left + (Width - ScaleWidth) / 2, Top + SchedForm.Top + (Height - ScaleHeight) / 2
    EnumForm.Show modal
    If EnumValue <> -1 Then p$ = "(" + Right$(Str$(primtag), 1) + ")" + Format$(EnumValue)
  Case 10, 11                   'date, time
    savept = ParseType
    If primtag = 10 Then
      ParseType = ptD
    Else
      ParseType = ptT
    End If
    DateForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    DateForm.Show modal
    If DateString <> "" Then p$ = "(" + Right$(Str$(primtag), 2) + ")" + DateString
    ParseType = savept
  Case 12                         'object id
    ObjIDForm.Move SchedForm.Left + (Width - ScaleWidth) / 2, SchedForm.Top + (Height - ScaleHeight) / 2
    ObjIDForm.Show modal
    If ObjIDString <> "" Then p$ = "(" + Right$(Str$(primtag), 2) + ")" + ObjIDString
  End Select
  If p$ <> "" Then Value.Caption = p$
End Sub

Private Sub Form_Activate()
  If Not (InForm) Then
    InForm = True
    If ObjIDFrame.visible Then ObjectType_Click
    ShowPlist
  End If
End Sub

Private Sub Form_Load()
Dim i
  Top = 0
  Left = 0
  nheight = Height
  PropId = -1
  priority.AddItem "none"
  For i = 1 To 16
    priority.AddItem Format$(i)
  Next i
  priority.ListIndex = 0
End Sub

Private Sub MultipleRefs_Click(Value As Integer)
  ShowPlist
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
  PropertyIdentifier.Clear
  np = VTSAPIgetpropinfo(ObjectType.ListIndex, NumProps, "", 0&, 0&, 0&, 0&)
  For i = 0 To np - 1
      PropId = VTSAPIgetpropinfo(ObjectType.ListIndex, i, pn, ptype, 0&, 0&, 0&)
      PropertyIdentifier.AddItem pn
      PropertyIdentifier.ItemData(PropertyIdentifier.NewIndex) = i
  Next i
  If PropertyIdentifier.ListCount > 0 Then PropertyIdentifier.ListIndex = 0

End Sub

Private Sub ObjectType_GotFocus()
    If Left$(ObjectType.Text, Len(propcolon)) = propcolon Then
      ObjectType.SelStart = Len(propcolon)
      ObjectType.SelLength = 5
    End If

End Sub

Private Sub OKButton_Click()
Dim p$, i
  If ActFrame.visible Then
    If Value.ForeColor = Red Or Value.Caption = "" Then
      Beep
      MsgBox "You must specify a value!", 48, "No Value"
      EditButton.SetFocus
      Exit Sub
    End If
    PRString = ""
    plist.Clear
    plist.ListIndex = -1
    AddButton_Click
    If plist.ListIndex = -1 Then Exit Sub
    If DInstanceCheck.Value Then
      PRString = "(DEVICE," + DInstance.Text + ")|"
    End If
    PRString = PRString + "(" + ObjectType.Text + "," + ObjectInstance.Text + "),"
    PRString = PRString + plist.List(plist.ListIndex) + "=" + Trim$(Value.Caption) + ","
    If priority.Text <> "none" Then PRString = PRString + priority.Text
    PRString = PRString + ","
    If PostDelayCheck.Value Then PRString = PRString + PostDelay.Text
    PRString = PRString + ","
    If QuitCheck.Value Then PRString = PRString + "Yes," Else PRString = PRString + "No,"
    If WriteSuccessCheck.Value Then PRString = PRString + "Yes" Else PRString = PRString + "No"
  Else
    If ObjIDFrame.visible Then
      PRString = "(" + ObjectType.Text + "," + ObjectInstance.Text + "),("
      For i = 0 To plist.ListCount - 1
        PRString = PRString + plist.List(i) + ","
      Next i
      If Right$(PRString, 1) = "," Then PRString = Left$(PRString, Len(PRString) - 1)
      PRString = PRString + ")"
    Else
      If Not MultipleRefs Then
        plist.Clear
        plist.ListIndex = -1
        AddButton_Click
        If plist.ListIndex = -1 Then Exit Sub
        PRString = plist.List(plist.ListIndex)
      End If
    End If
  End If
  InForm = False
'  HideBiModal PropRefForm
  Hide                        'changed to this because we are called from PropVal form ***218
End Sub

Private Sub PropertyIdentifier_Click()
Dim i, pflags, pid As Long, pn As String * 64
  If ObjIDFrame.visible Then
    i = PropertyIdentifier.ListIndex
    pid = VTSAPIgetpropinfo(ObjectType.ListIndex, i, pn, ptype, 0&, pflags, 0&)
    If pflags And propIsArray Then
      ArrayPanel.visible = True
      ArrayLabel.visible = True
    Else
      ArrayPanel.visible = False
      ArrayLabel.visible = False
    End If
    PropIDHex.Caption = "0x" + Hex$(pid)
  Else
    PropertyIdentifier.ForeColor = Black
    If PropertyIdentifier.ListIndex >= 0 Then
      PropId = PropertyIdentifier.ListIndex
      PropIDHex.Caption = "0x" + Hex$(PropId)
    Else
      PropIDHex.Caption = ""
      PropId = -1
    End If
  End If
End Sub

Private Sub PropertyIdentifier_GotFocus()
  If Not (ObjIDFrame.visible) Then
    If Left$(PropertyIdentifier.Text, Len(propcolon)) = propcolon Then
      PropertyIdentifier.SelStart = Len(propcolon)
      PropertyIdentifier.SelLength = 5
    End If
  End If
End Sub

Private Sub ShowPlist()
  If MultipleRefs.Value Then
    Height = nheight
  Else
    Height = (Height - ScaleHeight) + plpanel.Top
  End If
  DeleteButton.visible = MultipleRefs.Value
  AddButton.visible = MultipleRefs.Value
  ClearButton.visible = MultipleRefs.Value
End Sub

