VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "Threed32.ocx"
Begin VB.Form ReadPropForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Read/Write Property"
   ClientHeight    =   4785
   ClientLeft      =   3600
   ClientTop       =   4185
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
   ScaleHeight     =   4785
   ScaleWidth      =   5250
   ShowInTaskbar   =   0   'False
   Begin VB.ComboBox VendorNames 
      Appearance      =   0  'Flat
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   315
      Left            =   2400
      Style           =   2  'Dropdown List
      TabIndex        =   28
      Top             =   120
      Width           =   2775
   End
   Begin VB.CommandButton ClearButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "clear"
      Height          =   285
      Left            =   2835
      TabIndex        =   22
      Top             =   2805
      Visible         =   0   'False
      Width           =   645
   End
   Begin VB.CommandButton DeleteButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "delete"
      Height          =   285
      Left            =   3570
      TabIndex        =   21
      Top             =   2805
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton AddButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "add"
      Height          =   285
      Left            =   4410
      TabIndex        =   20
      Top             =   2805
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   375
      Left            =   4440
      TabIndex        =   1
      Top             =   600
      Width           =   735
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   375
      Left            =   4410
      TabIndex        =   0
      Top             =   1080
      Width           =   735
   End
   Begin Threed.SSPanel Panel3D1 
      Height          =   330
      Left            =   105
      TabIndex        =   26
      Top             =   105
      Width           =   2220
      _Version        =   65536
      _ExtentX        =   3916
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
      Alignment       =   1
      Autosize        =   3
      Begin VB.ComboBox PropFn 
         Appearance      =   0  'Flat
         BackColor       =   &H00FFFFFF&
         ForeColor       =   &H00000000&
         Height          =   300
         Left            =   15
         Style           =   2  'Dropdown List
         TabIndex        =   27
         Top             =   15
         Width           =   2190
      End
   End
   Begin Threed.SSPanel plpanel 
      Height          =   1620
      Left            =   105
      TabIndex        =   18
      Top             =   3120
      Width           =   5055
      _Version        =   65536
      _ExtentX        =   8916
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
         TabIndex        =   19
         Top             =   15
         Width           =   5025
      End
   End
   Begin Threed.SSFrame PropFrame 
      Height          =   1170
      Left            =   105
      TabIndex        =   12
      Top             =   1575
      Width           =   5055
      _Version        =   65536
      _ExtentX        =   8916
      _ExtentY        =   2064
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
         Height          =   255
         Left            =   3000
         TabIndex        =   24
         Top             =   120
         Visible         =   0   'False
         Width           =   615
      End
      Begin Threed.SSPanel PIPanel 
         Height          =   345
         Left            =   120
         TabIndex        =   15
         Top             =   480
         Width           =   3570
         _Version        =   65536
         _ExtentX        =   6297
         _ExtentY        =   609
         _StockProps     =   15
         ForeColor       =   8388736
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
         Autosize        =   3
         Begin VB.ComboBox PropertyIdentifier 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   315
            Left            =   15
            TabIndex        =   25
            Top             =   15
            Width           =   3540
         End
      End
      Begin Threed.SSPanel ArrayPanel 
         Height          =   330
         Left            =   3780
         TabIndex        =   13
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
            TabIndex        =   14
            Top             =   75
            Width           =   1110
         End
      End
      Begin Threed.SSCheck AlwaysSendArray 
         Height          =   225
         Left            =   105
         TabIndex        =   23
         Top             =   840
         Width           =   4845
         _Version        =   65536
         _ExtentX        =   8546
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Always send array index with single property?"
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
         TabIndex        =   16
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
         TabIndex        =   2
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
         TabIndex        =   3
         Top             =   210
         Width           =   1500
      End
   End
   Begin Threed.SSFrame ObjIDFrame 
      Height          =   960
      Left            =   105
      TabIndex        =   5
      Top             =   525
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
      Begin Threed.SSPanel Panel3D2 
         Height          =   330
         Left            =   3255
         TabIndex        =   8
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
            TabIndex        =   9
            Text            =   "0"
            Top             =   75
            Width           =   795
         End
      End
      Begin Threed.SSPanel Panel3D4 
         Height          =   345
         Left            =   120
         TabIndex        =   6
         Top             =   480
         Width           =   3045
         _Version        =   65536
         _ExtentX        =   5371
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
            TabIndex        =   7
            Top             =   15
            Width           =   3015
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
         TabIndex        =   4
         Top             =   210
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
         TabIndex        =   11
         Top             =   210
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
         TabIndex        =   10
         Top             =   210
         Width           =   840
      End
   End
   Begin Threed.SSCheck RWMultiple 
      Height          =   225
      Left            =   105
      TabIndex        =   17
      Top             =   2835
      Width           =   2640
      _Version        =   65536
      _ExtentX        =   4657
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Read Multiple Properties?"
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
Attribute VB_Name = "ReadPropForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Dim currentobjecttype
Dim objtype, ptype, pflags, pet, PropId As Long
Dim nheight

Private Sub AddButton_Click()
Dim p$, i
  If objtype = -1 Then
    Beep
    MsgBox "You must specify an object type!", 48, "No Object Type"
    ObjectType.SetFocus
    Exit Sub
  End If
  If PropId = -1 Then
    Beep
    MsgBox "Sorry, current version does not support proprietary properties!", 48, "Proprietary Property"
    Exit Sub
  End If
  
  p$ = "(" + ObjectType.Text + "," + ObjectInstance.Text + ") "
  Select Case PropFn.ListIndex
    Case 0                                      'read property
      p$ = p$ + PropertyIdentifier.Text
      If ArrayPanel.visible Then p$ = p$ + "[" + ArrayIndex.Text + "]"
    Case 1                                      'write property
      p$ = p$ + PIPanel.Caption
      i = InStr(p$, "=")
      If i = 0 Or Mid$(p$, i + 1) = "" Then
        Beep
        MsgBox "You must specify a value!", 48, "No Property Value"
        ObjectType.SetFocus
        Exit Sub
      End If
      If (ArrayIndex.Text <> "" And Val(ArrayIndex.Text) <> 0) Then
        p$ = p$ + "@" + ArrayIndex.Text
      End If
    Case 2, 3                                   'add/remove list element
      PVObjectType = objtype
      PVPropVal = PropertyIdentifier.Text
      PVNeedsRelation = False
      PVPropCanChange = False
      PropValForm.Show modal
      p$ = Mid$(PVPropVal, InStr(PVPropVal, "=") + 1)
  End Select
  If p$ <> "" Then
    plist.AddItem p$
    plist.ItemData(plist.NewIndex) = PropId       'save propid because it's hard to get again
    plist.ListIndex = plist.NewIndex
  End If
End Sub

Private Sub AlwaysSendArray_Click(Value As Integer)
  PropertyIdentifier_Click
End Sub

Private Sub ArrayIndex_GotFocus()
  GoSelect ArrayIndex
End Sub

Private Sub ClearButton_Click()
Dim n$
  If PropFn.ListIndex > 1 Then n$ = "Items" Else n$ = "Properties"
  If MsgBox("Are you sure that you want to erase all the entries in the " + n$ + " list?", 33, "Clear " + n$ + " List") <> 1 Then Exit Sub
  plist.Clear
End Sub

Private Sub CloseButton_Click()
  HideBiModal ReadPropForm
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
  If PropFn.ListIndex = 1 Then                  'write property
    If objtype = -1 Then
      Beep
      MsgBox "You must specify an object type!", 48, "No Object Type"
      ObjectType.SetFocus
      Exit Sub
    End If
    If PropId = -1 Then
      Beep
      MsgBox "Sorry, current version does not support proprietary properties!", 48, "Proprietary Property"
      Exit Sub
    End If
    PVObjectType = objtype
    PVPropVal = PIPanel.Caption
    PVNeedsRelation = False
    PVPropCanChange = True
    PropValForm.Show modal
    PIPanel.Caption = PVPropVal
  End If
  ObjectType.SetFocus
End Sub

Private Sub Form_Activate()
  ShowPlist
End Sub

Private Sub Form_Load()
Dim i                                 '                                       ***218 Begin
  VendorNames.Clear
  For i = 0 To WhoForm.VendorNames.ListCount - 1
    VendorNames.AddItem WhoForm.VendorNames.List(i)
    VendorNames.ItemData(VendorNames.NewIndex) = WhoForm.VendorNames.ItemData(i)
  Next i                              '                                       ***218 End
  
  Move 0, 0
  nheight = Height
  PropFn.Clear
  PropFn.AddItem "Read Property"
  PropFn.AddItem "Write Property"
  PropFn.AddItem "Add List Element"
  PropFn.AddItem "Remove List Element"
  PropFn.ListIndex = 0

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
  objtype = -1
  PropId = -1
  PVPropVal = ""
  EditButton.Left = ArrayPanel.Left + ArrayPanel.Width - EditButton.Width
  EditButton.Top = AlwaysSendArray.Top
  RWMultiple.Value = False
  RWMultiple.visible = True
End Sub

Private Sub GetPropIDs()
Dim c$, np As Long, i, pn As String * 64, ptype
  ObjectType.ForeColor = Black
  currentobjecttype = ObjectType.ListIndex
  If currentobjecttype Then
    ObjIDHex.Caption = "0x" + Hex$(currentobjecttype - 1)
    objtype = currentobjecttype - 1
    PropertyIdentifier.Clear
    np = VTSAPIgetpropinfo(objtype, NumProps, "", 0&, 0&, 0&, 0&)
    PropId = -1
    For i = 0 To np - 1
      PropId = VTSAPIgetpropinfo(objtype, i, pn, ptype, 0&, 0&, 0&)
      If PropFn.ListIndex > 1 Then              'add/remove list element
        Select Case ptype
          Case ptCalist, ptVTcl, ptSkeys, ptTsrecip, ptDabind, ptReciplist, ptXsched, ptWsched, ptRaslist, ptStavals, ptLopref
            PropertyIdentifier.AddItem pn
            PropertyIdentifier.ItemData(PropertyIdentifier.NewIndex) = i
        End Select
      Else
        PropertyIdentifier.AddItem pn
        PropertyIdentifier.ItemData(PropertyIdentifier.NewIndex) = i
      End If
    Next i
    If PropertyIdentifier.ListCount > 0 Then PropertyIdentifier.ListIndex = 0
    If (PropertyIdentifier.ListCount = 0) And (PropFn.ListIndex > 1) Then
      PropertyIdentifier.Text = "No List Properties!"
      PropertyIdentifier.ForeColor = Red
    Else
      PropertyIdentifier.ForeColor = Black
    End If
  Else
    ObjIDHex.Caption = ""
    objtype = -1
    PropId = -1
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
Dim c$, p$, i, n, v                         '                           ***218 Begin

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
    PropertyIdentifier.Clear
    PropertyIdentifier.ForeColor = Red
    PropertyIdentifier.Text = "Can't Show Properties!"
    v = VendorNames.ListIndex - 1
    If v > 0 Then                           'there is a non-ashrae vendor selected
      c$ = "Obj" + Format$(v) + "\" + Format$(objtype)
      n = Val(GetPref(c$, "Props"))         'how many props are there?
      For i = 0 To n - 1
        p$ = GetPref(c$, Format$(i))        'try to get prop name
        If p$ <> "" Then
          PropertyIdentifier.AddItem Nxt$(p$)
          PropertyIdentifier.ItemData(PropertyIdentifier.NewIndex) = i
        End If
      Next i
      If n Then PropertyIdentifier.ListIndex = 0
    End If
  End If
  ObjIDHex.Caption = "0x" + Hex$(objtype)   '                           ***218 End
End Sub

Private Sub ObjectType_Click()
  ObjectType.ForeColor = Black
  currentobjecttype = ObjectType.ListIndex
  If currentobjecttype Then
    VendorNames.ListIndex = 1               'ASHRAE
    ObjIDHex.Caption = "0x" + Hex$(currentobjecttype - 1)
    objtype = currentobjecttype - 1
    GetPropIDs
  Else
    ObjIDHex.Caption = ""
    objtype = -1
    PropId = -1
  End If
End Sub

Private Sub ObjectType_GotFocus()
    If Left$(ObjectType.Text, Len(propcolon)) = propcolon Then
      ObjectType.SelStart = Len(propcolon)
      ObjectType.SelLength = 5
    End If
End Sub

Private Sub ObjectType_KeyPress(KeyAscii As Integer)
  If currentobjecttype <> 0 Then
    KeyAscii = 0
    Exit Sub
  End If
End Sub

Private Sub PropertyIdentifier_Click()
Dim pn As String * 64, i, v, c$, p$
  PropertyIdentifier.ForeColor = Black
  i = PropertyIdentifier.ListIndex
  If i >= 0 Then
    If objtype < 128 Then
      PropId = VTSAPIgetpropinfo(objtype, PropertyIdentifier.ItemData(i), pn, ptype, 0&, pflags, pet)
    Else
      If PropertyIdentifier.ListIndex >= 0 Then
        v = VendorNames.ListIndex - 1
        If v > 0 Then                         'there is a non-ashrae vendor selected
          c$ = "Obj" + Format$(v) + "\" + Format$(objtype)
          p$ = GetPref(c$, Format$(PropertyIdentifier.ItemData(i))) 'try to get prop name,id
          If p$ <> "" Then
            Nxt$ p$
            PropId = Val(p$)
          Else
            GoTo ukprop
          End If
        Else
          GoTo ukprop
        End If
      Else
        GoTo ukprop
      End If
    End If
    PropIDHex.Caption = "0x" + Hex$(PropId)
  Else
ukprop:
    PropIDHex.Caption = ""
    PropId = -1
    Exit Sub
  End If
  If (pflags And propIsArray) Or AlwaysSendArray.Value Then
    ArrayPanel.visible = True
    ArrayLabel.visible = True
  Else
    ArrayPanel.visible = False
    ArrayLabel.visible = False
  End If
End Sub

Private Sub PropertyIdentifier_GotFocus()
    If Left$(PropertyIdentifier.Text, Len(propcolon)) = propcolon Then
      PropertyIdentifier.SelStart = Len(propcolon)
      PropertyIdentifier.SelLength = 5
    End If
End Sub

Private Sub PropFn_Click()
  Caption = PropFn.Text
  ArrayIndex.Text = ""
  plist.Clear
  Select Case PropFn.ListIndex
  Case 0                                        'read property
    PVWriteProperty = False
    EditButton.visible = False
    PIPanel.Caption = ""
    ArrayPanel.visible = AlwaysSendArray.Value
    ArrayLabel.visible = AlwaysSendArray.Value
    PropertyIdentifier.visible = True
    AlwaysSendArray.visible = True
    RWMultiple.Caption = "Read Multiple Properties?"
    RWMultiple.Value = False
    RWMultiple.visible = True
    Label2.Caption = "property identifier"
  Case 1                                        'write property
    PVWriteProperty = True
    PVPropVal = ""
    EditButton.visible = True
    PIPanel.Caption = PVPropVal
    ArrayLabel.visible = False
    ArrayPanel.visible = False
    PropertyIdentifier.visible = False
    AlwaysSendArray.visible = False
    RWMultiple.Caption = "Write Multiple Properties?"
    RWMultiple.Value = False
    RWMultiple.visible = True
    Label2.Caption = "property identifier=value"
  Case 2, 3                                     'add/remove list element
    PVWriteProperty = False
    EditButton.visible = False
    PIPanel.Caption = ""
    ArrayPanel.visible = False
    ArrayLabel.visible = False
    PropertyIdentifier.visible = True
    AlwaysSendArray.visible = False
    RWMultiple.Value = True
    RWMultiple.visible = False
    Label2.Caption = "property identifier"
  End Select
  GetPropIDs
End Sub

Private Sub RWMultiple_Click(Value As Integer)
  ShowPlist
End Sub

Private Sub SendButton_Click()
Dim apdu As String * 4096, alen, i, j, k, m, needarray, a$, p$, t$    '       ***236

  If RWMultiple.Value Then
    If plist.ListCount = 0 Then                 'we're using the list
      If PropFn.ListIndex < 2 Then
        p$ = "property list"
        t$ = "No Properties to Read/Write"
      Else
        p$ = "elements list"
        t$ = "No Elements to Add/Remove"
      End If
      Beep
      MsgBox "The " + p$ + " is empty! You must add to it first by using the [add] button.", 48, t$
      AddButton.SetFocus
      Exit Sub
    End If
  Else
    If objtype = -1 Then
      Beep
      MsgBox "You must specify an object type!", 48, "No Object Type"
      ObjectType.SetFocus
      Exit Sub
    End If
    If PropId = -1 Then
      Beep
      MsgBox "Sorry, current version does not support proprietary properties!", 48, "Proprietary Property"
      Exit Sub
    End If
  End If
  Select Case PropFn.ListIndex
  Case 0, 1                                     'read/write property
    If RWMultiple.Value Then                    'Read Property Multiple
    '-----------------------Multiple Read/Write-----------------------------
      ReDim pl(plist.ListCount) As PropList     'make an array to convey the info to encode
      For i = 0 To plist.ListCount - 1
        If PropFn.ListIndex = 0 Then            'read property
          pl(i).plPropID = plist.ItemData(i)    'copy saved propid
        Else
          p$ = plist.List(i)
          j = InStr(p$, ")")
' NOTE: This is required because declaring a string in a user type struct that is
'       to be passed to a DLL as an array JUST DOESN'T WORK PROPERLY!!!!!!!
          t$ = LTrim$(Mid$(p$, j + 1, Len(p$) - j)) 'property name=value
          For j = 0 To Len(t$) - 1
            pl(i).plPropVal(j) = Asc(Mid$(t$, j + 1, 1))
          Next j
          pl(i).plPropVal(j) = 0
'          pl(i).plPropVal = LTrim$(Mid$(p$, j + 1, Len(p$) - j)) 'property name=value
'..........................................................................................
        End If
        p$ = plist.List(i)                      'get (objecttype,instance) propname
        j = InStr(p$, ",")
        t$ = Mid$(p$, 2, j - 2)                 't$ is object name
        For k = 1 To ObjectType.ListCount - 1
          If ObjectType.List(k) = t$ Then       'found it
            pl(i).plObjectID = vbOBJECTID(k - 1, Val(Mid$(p$, j + 1)))
            j = InStr(p$, "[")                  '                       ***236 Begin
            pl(i).plArrayIndex = -1
            If j And (PropFn.ListIndex = 0) Then  'have an array index and read property
              m = InStr(j + 1, p$, "]")
              If m Then                         'found [...]
                a$ = Mid$(p$, j + 1, m - (j + 1))
                If a$ <> "" Then
                  pl(i).plArrayIndex = Val(a$)
                End If
              End If
            End If                              '                       ***236 End
            Exit For
          End If
        Next k
      Next i
      If PropFn.ListIndex = 0 Then              'read property
        alen = eReadPropMultipleService(apdu, plist.ListCount, pl(0))
      Else
        alen = eWritePropMultipleService(apdu, Len(apdu), plist.ListCount, pl(0))
      End If
    Else
    '-----------------------Single Read/Write-----------------------------
      If PropFn.ListIndex = 0 Then              'read property
        needarray = ((ArrayIndex.Text <> "") And ArrayPanel.visible) Or AlwaysSendArray.Value
        alen = eReadPropService(apdu, vbOBJECTID(objtype, Val(ObjectInstance)), PropId, needarray, Val(ArrayIndex))
        'MsgBox "APDU length " & Len(apdu) & " contents '" & apdu & "'"
      Else                                      'write property
        i = InStr(PVPropVal, "=")
        If (i = 0) Or (i <> 0 And Right$(PVPropVal, i) = "") Then
          Beep
          MsgBox "You must specify a value!", 48, "No Value Specified"
          Exit Sub
        End If
        p$ = PVPropVal
        'MsgBox "Sending: " & p
        If (ArrayIndex.Text <> "" And Val(ArrayIndex.Text) <> 0) Then
          p$ = p$ + "@" + ArrayIndex.Text
        End If
        alen = eWritePropService(apdu, Len(apdu), objtype, Val(ObjectInstance), p$)
      End If
    End If
  Case 2, 3                                     'add/remove list element
    alen = eListElementService(apdu, Len(apdu), PropFn.ListIndex - 2, vbOBJECTID(objtype, Val(ObjectInstance)), PropId, needarray, Val(ArrayIndex), ptype, pflags, pet, plist.hwnd)
  End Select
  If alen Then
    SendNPDU Left$(apdu, alen), True
  Else
    Beep
    MsgBox "Object could not be encoded! Object too Long?", 48, "Encode Error"
    ObjectType.SetFocus
    Exit Sub
  End If
End Sub

Private Sub ShowPlist()
  If RWMultiple.Value Then
    If PropFn.ListIndex < 2 Then Caption = PropFn.Text + " Multiple"
    Height = nheight
  Else
    Caption = PropFn.Text
    Height = (Height - ScaleHeight) + plpanel.Top
  End If
  DeleteButton.visible = RWMultiple.Value
  AddButton.visible = RWMultiple.Value
  ClearButton.visible = RWMultiple.Value
End Sub

