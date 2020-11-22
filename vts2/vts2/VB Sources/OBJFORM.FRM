VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "Threed32.ocx"
Begin VB.Form ObjForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Create/Delete Object"
   ClientHeight    =   3435
   ClientLeft      =   60
   ClientTop       =   345
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
   ScaleHeight     =   3435
   ScaleWidth      =   5250
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton ClearButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "clear"
      Height          =   285
      Left            =   4410
      TabIndex        =   12
      Top             =   840
      Width           =   750
   End
   Begin VB.CommandButton DeleteButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "delete"
      Height          =   285
      Left            =   4410
      TabIndex        =   16
      Top             =   1155
      Width           =   750
   End
   Begin VB.CommandButton AddButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "add"
      Height          =   285
      Left            =   4410
      TabIndex        =   15
      Top             =   1470
      Width           =   750
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   285
      Left            =   4410
      TabIndex        =   1
      Top             =   105
      Width           =   735
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   285
      Left            =   4410
      TabIndex        =   0
      Top             =   420
      Width           =   735
   End
   Begin Threed.SSPanel plpanel 
      Height          =   1620
      Left            =   105
      TabIndex        =   13
      Top             =   1785
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
         TabIndex        =   14
         Top             =   15
         Width           =   5025
      End
   End
   Begin Threed.SSFrame ObjIDFrame 
      Height          =   960
      Left            =   105
      TabIndex        =   5
      Top             =   735
      Width           =   4215
      _Version        =   65536
      _ExtentX        =   7435
      _ExtentY        =   1693
      _StockProps     =   14
      Caption         =   "Object Identifier"
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
         Left            =   105
         TabIndex        =   6
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
            TabIndex        =   7
            Top             =   15
            Width           =   3030
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
   Begin Threed.SSOption DeleteObject 
      Height          =   255
      Left            =   120
      TabIndex        =   2
      TabStop         =   0   'False
      Top             =   360
      Width           =   1575
      _Version        =   65536
      _ExtentX        =   2778
      _ExtentY        =   450
      _StockProps     =   78
      Caption         =   "DeleteObject"
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
   Begin Threed.SSOption CreateObject 
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   1575
      _Version        =   65536
      _ExtentX        =   2778
      _ExtentY        =   450
      _StockProps     =   78
      Caption         =   "CreateObject"
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
Attribute VB_Name = "ObjForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Dim currentobjecttype
Dim objtype
Dim nheight




Private Sub AddButton_Click()
  If objtype = -1 Then
      Beep
      MsgBox "You must specify an object type!", 48, "No Object Type"
      ObjectType.SetFocus
      Exit Sub
  End If
  PVObjectType = objtype
  PVPropVal = ""
  PVNeedsRelation = False
  PVPropCanChange = True
  PropValForm.Show modal
  If PVPropVal <> "" Then plist.AddItem PVPropVal
End Sub

Private Sub ClearButton_Click()
  If MsgBox("Are you sure that you want to erase all the entries in the properties list?", 33, "Clear Property List") <> 1 Then Exit Sub
  plist.Clear
End Sub

Private Sub CloseButton_Click()
  HideBiModal ObjForm
End Sub

Private Sub CreateObject_Click(Value As Integer)
  If Value Then
    ClearButton.visible = True
    DeleteButton.visible = True
    AddButton.visible = True
    Height = nheight
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

Private Sub DeleteObject_Click(Value As Integer)
  If Value Then
    ClearButton.visible = False
    DeleteButton.visible = False
    AddButton.visible = False
    Height = (Height - ScaleHeight) + plpanel.Top
  End If
End Sub

Private Sub Form_Load()
  Move 0, 0
  nheight = Height
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
  PVWriteProperty = False
End Sub

Private Sub ObjectInstance_Change()
Dim rl As Long
  If (ObjectInstance.Text = "") And DeleteObject.Value Then
    ObjectInstance.Text = Format$(MinDeviceInstance)
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
      ObjectType.SelLength = 5
    End If
End Sub

Private Sub ObjectType_KeyPress(KeyAscii As Integer)
  If currentobjecttype <> 0 Then
    KeyAscii = 0
    Exit Sub
  End If
End Sub

Private Sub SendButton_Click()
Dim apdu As String * 4096, alen, ins As Long  ' changed "apdu as string * 409" to "apdu as string 4096"
  ins = Val(ObjectInstance.Text)
  If DeleteObject.Value Then
    If (ObjectInstance.Text = "") Or (ins < MinDeviceInstance) Or (ins > 4194303) Then
      Beep
      MsgBox "You must specify an object instance from" + Str$(MinDeviceInstance) + " to 4194303!", 48, "Bad Object Instance"
      ObjectInstance.SetFocus
      Exit Sub
    End If
    If objtype = -1 Then
      Beep
      MsgBox "You must specify an object type!", 48, "No Object Type Selected"
      ObjectType.SetFocus
      Exit Sub
    End If
    alen = eDeleteObjectService(apdu, vbOBJECTID(objtype, ins))
  Else
    alen = eCreateObjectService(apdu, 4096, vbOBJECTID(objtype, ins), (ObjectInstance.Text <> ""), plist.hwnd)
  End If
  If alen Then
    SendNPDU Left$(apdu, alen), True
  Else
    Beep
    MsgBox "Object could not be encoded! Object too Long?", 48, "Encode Error"
    ObjectType.SetFocus
  End If
End Sub

