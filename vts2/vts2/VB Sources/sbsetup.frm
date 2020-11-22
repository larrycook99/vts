VERSION 5.00
Begin VB.Form SBSetup 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   Caption         =   "Script Based Setup"
   ClientHeight    =   2610
   ClientLeft      =   2010
   ClientTop       =   2340
   ClientWidth     =   6000
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
   Icon            =   "SBSETUP.frx":0000
   LinkTopic       =   "SBSetup"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2610
   ScaleWidth      =   6000
   Begin VB.ComboBox KeyWordList 
      Appearance      =   0  'Flat
      Height          =   288
      Left            =   60
      Style           =   2  'Dropdown List
      TabIndex        =   4
      Top             =   660
      Width           =   2772
   End
   Begin VB.ComboBox ValueList 
      Appearance      =   0  'Flat
      Height          =   315
      Left            =   3060
      TabIndex        =   9
      Text            =   "ValueList"
      Top             =   660
      Width           =   2772
   End
   Begin VB.CommandButton DeviceButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Device"
      Height          =   375
      Left            =   1440
      TabIndex        =   8
      Top             =   2160
      Width           =   1095
   End
   Begin VB.ListBox InfoList 
      Appearance      =   0  'Flat
      Height          =   1395
      Left            =   120
      TabIndex        =   7
      Top             =   2760
      Width           =   5775
   End
   Begin VB.CommandButton PortButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Ports"
      Height          =   375
      Left            =   120
      TabIndex        =   6
      Top             =   2160
      Width           =   1095
   End
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3480
      TabIndex        =   5
      Top             =   2160
      Width           =   1095
   End
   Begin VB.CommandButton OkButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   4800
      TabIndex        =   0
      Top             =   2160
      Width           =   1095
   End
   Begin VB.Label Explanation 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Explanation"
      ForeColor       =   &H00808080&
      Height          =   735
      Left            =   120
      TabIndex        =   3
      Top             =   1200
      Width           =   5775
      WordWrap        =   -1  'True
   End
   Begin VB.Label Value 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Value(s)"
      ForeColor       =   &H80000008&
      Height          =   252
      Left            =   3120
      TabIndex        =   2
      Top             =   300
      Width           =   852
   End
   Begin VB.Label Keyword 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "Keyword"
      ForeColor       =   &H80000008&
      Height          =   252
      Left            =   120
      TabIndex        =   1
      Top             =   300
      Width           =   852
   End
End
Attribute VB_Name = "SBSetup"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim ValueIndex As Integer
Dim ValueChanged As Integer

Dim ListVisible As Integer
Dim PortsVisible As Integer
Dim DevicesVisible As Integer

Private Sub CancelButton_Click()
  If CancelButton.Caption = "Cancel" Then
    Unload SBSetup
  Else
    If ParseScript(-1) = 1 Then
      ShowList False
      CancelButton.Caption = "Cancel"
    Else
      ShowList True
    End If
    MakeKeywordList
  End If
End Sub

Private Sub ChangeValue()
Dim vi%, ni%, s$
   ValueList.List(ValueIndex) = ValueList.Text
   ValueList.ListIndex = ValueIndex

   s = ""
   ni = KeyWordList.ListIndex

   For vi = 0 To ValueList.ListCount - 1
     s = s + ValueList.List(vi) + ","
   Next

   SBNata(ni).Value = Left$(s$, Len(s$) - 1)
End Sub

Private Sub Form_Activate()
  MakeKeywordList
End Sub

Private Sub Form_Load()
  ListVisible = False
  PortsVisible = False
  DevicesVisible = False

  SBSetup.Caption = SBTForm.Caption & " - Setup"
  MakeKeywordList
End Sub

Private Sub Form_Resize()
Dim h%

  ' don't make it smaller
  If Width < 6120 Then Width = 6120

  ' calculate the real ScaleHeight
  If ListVisible Then
    If Height < 4915 Then Height = 4915
    h = ScaleHeight - 1900
  Else
    If Height < 3015 Then Height = 3015
    h = ScaleHeight
  End If

  ' now adjust the other stuff, size
  Explanation.Move 120, 1200, ScaleWidth - 225, h - 1875
  InfoList.Move 120, h + 150, ScaleWidth - 225, 1590

  ' position only, fixed on height
  PortButton.Move 120, h - 450
  DeviceButton.Move 1440, h - 450

  ' position only, fixed on height and right
  OkButton.Move ScaleWidth - 1200, h - 450
  CancelButton.Move ScaleWidth - 2520, h - 450
End Sub

Private Sub KeyWordList_Click()
Dim i As Integer
  i = KeyWordList.ListIndex
  Explanation.Caption = SBNata(i).Comment
  MakeValueList i
End Sub

Private Sub MakeKeywordList()
Dim ni As Integer
  
  ' Start a new list
  KeyWordList.Clear

  For ni = 0 To UBound(SBNata) - 1
    KeyWordList.AddItem RTrim$(SBNata(ni).Name)
  Next

  ' Set up first Keyword if one exist
  If KeyWordList.ListCount > 0 Then
    KeyWordList.ListIndex = 0
    Explanation.Caption = SBNata(0).Comment
    MakeValueList 0
  End If
End Sub

Private Sub MakeValueList(i%)
Dim s$, j%

  ValueList.Clear
  s$ = SBNata(i%).Value

  j% = InStr(s$, ",")
  Do While (j% > 0)
    ValueList.AddItem Left$(s$, j% - 1)
    s$ = Right$(s$, Len(s$) - j%)
    j% = InStr(s$, ",")
  Loop

  ValueList.AddItem RTrim$(s$)
  ValueList.ListIndex = 0

  ValueIndex = 0
  ValueChanged = False
End Sub

Private Sub OKButton_Click()
  
  If ParseScript(1) <= 0 Then
    ShowList True
    ' disable the cancel
    CancelButton.Caption = "Reset"
  Else
    Unload SBSetup
  End If
End Sub

Private Sub PortButton_Click()
Dim pl$, m$, l%, f%, j%

  PortsVisible = Not PortsVisible
  ShowList PortsVisible

  ' If the list is visible, so we should update it
  If PortsVisible Then
    InfoList.Clear
    pl$ = Space$(512)

    ' get the port list
    l = NALSAPIgetportlist(pl$)
    f = 1
    Do While f < l
      j = InStr(f, pl$, Chr$(13) & Chr$(10))       'find next newline pair
      If j Then                                   'found one
        m$ = Mid$(pl$, f, j - f)
      Else                                        'this is the last string
        m$ = Mid$(pl$, f)
      End If
      InfoList.AddItem m$
      If j = 0 Then Exit Do
      f = j + 2                                   'skip nl pair
    Loop
  End If
End Sub

Private Sub ShowList(visible%)
  If ListVisible <> visible Then
    ListVisible = visible
    If ListVisible Then
      ' Make it visible
      Height = Height + 1900
    Else
      ' Make it invisible
      Height = Height - 1900
    End If
  End If
End Sub

Private Sub ValueList_Change()
  ValueChanged = True
End Sub

Private Sub ValueList_Click()
  ValueIndex = ValueList.ListIndex
End Sub

Private Sub ValueList_LostFocus()
  If ValueChanged Then ChangeValue
  ValueChanged = False
End Sub

