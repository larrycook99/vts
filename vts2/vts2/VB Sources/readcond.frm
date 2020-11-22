VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form ReadCondForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Read Property Conditional"
   ClientHeight    =   4260
   ClientLeft      =   8190
   ClientTop       =   2550
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
   ScaleHeight     =   4260
   ScaleWidth      =   5250
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton RefButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Refs"
      Default         =   -1  'True
      Height          =   255
      Left            =   2625
      TabIndex        =   2
      Top             =   840
      Visible         =   0   'False
      Width           =   735
   End
   Begin VB.CommandButton ClearButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "clear"
      Height          =   285
      Left            =   2835
      TabIndex        =   15
      Top             =   2205
      Visible         =   0   'False
      Width           =   645
   End
   Begin VB.CommandButton DeleteButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "delete"
      Height          =   285
      Left            =   3570
      TabIndex        =   14
      Top             =   2205
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton AddButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "add"
      Height          =   285
      Left            =   4410
      TabIndex        =   13
      Top             =   2205
      Visible         =   0   'False
      Width           =   750
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   375
      Left            =   4410
      TabIndex        =   1
      Top             =   525
      Width           =   735
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Height          =   375
      Left            =   4410
      TabIndex        =   0
      Top             =   105
      Width           =   735
   End
   Begin Threed.SSFrame Frame3D1 
      Height          =   615
      Left            =   120
      TabIndex        =   17
      Top             =   120
      Width           =   1575
      _Version        =   65536
      _ExtentX        =   2778
      _ExtentY        =   1085
      _StockProps     =   14
      Caption         =   "Selection Logic"
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
      Begin Threed.SSPanel Panel3D1 
         Height          =   330
         Left            =   360
         TabIndex        =   18
         Top             =   240
         Width           =   870
         _Version        =   65536
         _ExtentX        =   1535
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
         Begin VB.ComboBox SelectionLogic 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   5
            Top             =   15
            Width           =   840
         End
      End
      Begin VB.Label Label1 
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
         TabIndex        =   6
         Top             =   210
         Width           =   165
      End
   End
   Begin Threed.SSPanel plpanel 
      Height          =   1620
      Left            =   105
      TabIndex        =   11
      Top             =   2520
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
         Sorted          =   -1  'True
         TabIndex        =   12
         Top             =   15
         Width           =   5025
      End
   End
   Begin Threed.SSFrame CritFrame 
      Height          =   960
      Left            =   120
      TabIndex        =   7
      Top             =   1155
      Width           =   5055
      _Version        =   65536
      _ExtentX        =   8916
      _ExtentY        =   1693
      _StockProps     =   14
      Caption         =   "Selection Criteria"
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
         Height          =   270
         Left            =   4200
         TabIndex        =   16
         Top             =   480
         Width           =   735
      End
      Begin Threed.SSPanel CriteriaPanel 
         Height          =   255
         Left            =   105
         TabIndex        =   8
         Top             =   480
         Width           =   3990
         _Version        =   65536
         _ExtentX        =   7038
         _ExtentY        =   450
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
         Left            =   3570
         TabIndex        =   9
         Top             =   210
         Width           =   165
      End
      Begin VB.Label Label2 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "property identifier <rel> value"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   105
         TabIndex        =   3
         Top             =   250
         Width           =   2505
      End
   End
   Begin Threed.SSCheck IncludeRefs 
      Height          =   225
      Left            =   105
      TabIndex        =   4
      Top             =   840
      Width           =   2325
      _Version        =   65536
      _ExtentX        =   4101
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Include Reference List?"
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
   Begin Threed.SSCheck MultipleCriteria 
      Height          =   225
      Left            =   105
      TabIndex        =   10
      Top             =   2205
      Width           =   1710
      _Version        =   65536
      _ExtentX        =   3016
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Multiple Criteria?"
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
Attribute VB_Name = "ReadCondForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Dim nheight



Private Sub AddButton_Click()
  If CriteriaPanel.Caption <> "" Then
    plist.AddItem CriteriaPanel.Caption
    plist.ListIndex = plist.NewIndex
  End If
  SelectionLogic.SetFocus
End Sub

Private Sub ClearButton_Click()
  If MsgBox("Are you sure that you want to erase all the entries in the criteria list?", 33, "Clear Criteria List") <> 1 Then Exit Sub
  plist.Clear
  SelectionLogic.SetFocus
End Sub

Private Sub CloseButton_Click()
  HideBiModal ReadCondForm
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
  SelectionLogic.SetFocus
End Sub

Private Sub EditButton_Click()
Dim s$, i
  PVPropVal = CriteriaPanel.Caption
  PVNeedsRelation = True
  PropValForm.Show modal
  CriteriaPanel.Caption = PVPropVal
  SelectionLogic.SetFocus
  i = RelPos(PVPropVal)
  If i Then
    s$ = Left$(PVPropVal, i - 1)
    If s$ = "all" Or s$ = "optional" Or s$ = "required" Then
      Beep
      MsgBox "Invalid property selected (all, optional or required).", 48, "Invalid Property"
      PVPropVal = ""
      Exit Sub
    End If
    For i = 0 To KnownProps - 1
      If s$ = PropName(i) Then
        PropIDHex.Caption = "0x" + Hex$(i)
        Exit Sub
      End If
    Next i
  Else
    Beep
    MsgBox "A relational must be selected", 48, "Invalid Relation"
    PVPropVal = ""
  End If
End Sub

Private Sub Form_Activate()
  ShowPlist
End Sub

Private Sub Form_Load()
  Top = 0
  Left = 0
  nheight = Height
  SelectionLogic.Clear
  SelectionLogic.AddItem "AND"
  SelectionLogic.AddItem "OR"
  SelectionLogic.AddItem "ALL"
  SelectionLogic.ListIndex = 0
  PVPropVal = ""
  PVWriteProperty = False
End Sub

Private Sub IncludeRefs_Click(Value As Integer)
  RefButton.visible = Value
End Sub

Private Sub MultipleCriteria_Click(Value As Integer)
  ShowPlist
End Sub

Private Sub RefButton_Click()
Dim i, j
  PropRefForm.ObjIDFrame.visible = False
  PropRefForm.ArrayPanel.visible = False
  PropRefForm.ArrayLabel.visible = False
  PropRefForm.MultipleRefs.visible = True
  PropRefForm.DInstanceLabel.visible = False
  PropRefForm.DInstancePanel.visible = False
  PropRefForm.DInstanceCheck.visible = False
  PropRefForm.prioritylabel.visible = False
  PropRefForm.priority.visible = False
  PropRefForm.prioritylabel.visible = False
  PropRefForm.ActFrame.visible = False
  PropRefForm.EditButton.visible = False
  PropRefForm.valuePanel.visible = False
  PropRefForm.PropertyIdentifier.Clear
  PropRefForm.Caption = "Edit Property References"
  For i = 0 To KnownProps - 1
    PropRefForm.PropertyIdentifier.AddItem PropName(i)
    j = PropRefForm.PropertyIdentifier.NewIndex
    PropRefForm.PropertyIdentifier.ItemData(j) = i
  Next i
  PropRefForm.PropertyIdentifier.ListIndex = 0
  PropRefForm.Show modal
  SelectionLogic.SetFocus
End Sub

'This function returns the position in the source string of the
'relational specifier ==, !=, <, <=, >, >=
'if 0 is returned, a valid specifier was not found!
Private Function RelPos(ByVal s$) As Integer
Dim i, t$
  RelPos = 0
  i = InStr(s$, "=")
  If i Then                   '== or != or >= or <=
    t$ = Mid$(s$, i - 1, 1)
    If Mid$(s$, i + 1, 1) = "=" Then
      RelPos = i
    ElseIf t$ = "!" Or t$ = "<" Or t$ = ">" Then
      RelPos = i - 1
    Else: i = 0
    End If
  End If
  If i = 0 Then
    i = InStr(s$, "<")       '<
    If i And Mid$(s$, i + 1, 1) = " " Then RelPos = i Else i = 0
  End If
  If i = 0 Then
    i = InStr(s$, ">")       '>
    If i And Mid$(s$, i + 1, 1) = " " Then RelPos = i Else i = 0
  End If
End Function

Private Sub SelectionLogic_Click()
  If SelectionLogic.ListIndex = 2 Then
    CritFrame.visible = False
    MultipleCriteria.visible = False
    AddButton.visible = False
    DeleteButton.visible = False
    ClearButton.visible = False
    Height = (Height - ScaleHeight) + plpanel.Top
  Else
    CritFrame.visible = True
    MultipleCriteria.visible = True
    ShowPlist
  End If
End Sub

Private Sub SendButton_Click()
Dim apdu As String * 512, alen, i, j, ncrits, s$, hcrit, hrefs
  ncrits = 0                                'in case we have selection logic =ALL
  If SelectionLogic.ListIndex <> 2 Then     'either AND or OR
    If Not MultipleCriteria.Value Then
      plist.Clear
      AddButton_Click                       'add a single criterium to the list
    End If
    If plist.ListCount = 0 Then
      Beep
      MsgBox "The property list is empty! You must add items to it first by using the [add] button.", 48, "No Criteria Specified"
      Exit Sub
    End If
    ncrits = plist.ListCount
    For i = 0 To ncrits - 1                 'make sure each entry is property-id <rel> value form
        If j Then s$ = Left$(PVPropVal, j - 1)
      j = RelPos(plist.List(i))
      If j Then s$ = Mid$(plist.List(i), j + 2)
      If j = 0 Or s$ = "" Then
        Beep
        MsgBox "A relation and value must be selected", 48, "Invalid Relation or Value"
        Exit Sub
      End If
    Next i
  End If
  If ncrits = 0 Then                        'must be ALL selection logic, skip criteria
    hcrit = 0
  Else
    hcrit = plist.hwnd
  End If
  If (PropRefForm.plist.ListCount = 0) Or (IncludeRefs.Value = False) Then  'no references specified
    hrefs = 0
  ElseIf IncludeRefs.Value Then
    hrefs = PropRefForm.plist.hwnd
  End If

  alen = eReadPropConditionalService(apdu, Len(apdu), SelectionLogic.ListIndex, hcrit, hrefs)
  If alen Then
    SendNPDU Left$(apdu, alen), True
  Else
    Beep
    MsgBox "Object could not be encoded! Object too Long?", 48, "Encode Error"
    SelectionLogic.SetFocus
    Exit Sub
  End If
End Sub

Private Sub ShowPlist()
  If MultipleCriteria.Value Then
    Height = nheight
  Else
    Height = (Height - ScaleHeight) + plpanel.Top
  End If
  DeleteButton.visible = MultipleCriteria.Value
  AddButton.visible = MultipleCriteria.Value
  ClearButton.visible = MultipleCriteria.Value
End Sub

