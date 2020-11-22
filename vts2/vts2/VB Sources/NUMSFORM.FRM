VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "Threed32.ocx"
Begin VB.Form NumsForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Numbers and Strings"
   ClientHeight    =   840
   ClientLeft      =   2835
   ClientTop       =   2100
   ClientWidth     =   4290
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
   ScaleHeight     =   840
   ScaleWidth      =   4290
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.CommandButton CancelButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "û"
      BeginProperty Font 
         Name            =   "Wingdings"
         Size            =   13.5
         Charset         =   2
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   330
      Left            =   3615
      TabIndex        =   3
      Top             =   105
      Width           =   330
   End
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "ü"
      Default         =   -1  'True
      BeginProperty Font 
         Name            =   "Wingdings"
         Size            =   13.5
         Charset         =   2
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   330
      Left            =   3930
      TabIndex        =   2
      Top             =   105
      Width           =   330
   End
   Begin Threed.SSPanel NSPanel 
      Height          =   330
      Left            =   105
      TabIndex        =   0
      Top             =   105
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
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
      Alignment       =   0
      Begin VB.TextBox NumsValue 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   630
         Left            =   15
         MaxLength       =   132
         MultiLine       =   -1  'True
         ScrollBars      =   1  'Horizontal
         TabIndex        =   1
         Top             =   15
         Width           =   3450
      End
   End
End
Attribute VB_Name = "NumsForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim NSbaseheight
Dim MaxValue As Long


Private Sub CancelButton_Click()
  NumsString = ""
  Hide
End Sub

Private Sub Form_Activate()
Dim i
  MaxValue = 0                          'means no upper bound
  NumsValue.ForeColor = Green
  Select Case ParseType
    Case ptFloat                        'float
      NumsValue.MaxLength = 32
      NumsValue.Text = Format$(NumsFloat)
    Case ptUW                           'uword
      NumsValue.MaxLength = 6           '0x0000 or 00000
      NumsValue.Text = Format$(NumsInt And &HFFFF&)
      MaxValue = &HFFFF&
    Case ptUD                           'udword
      NumsValue.MaxLength = 10          '0x00000000 or 0000000000
      NumsValue.Text = Format$(NumsInt)
    Case ptSW                           'sword
      NumsValue.MaxLength = 6           '0x0000 or -00000
      NumsValue.Text = Format$(NumsInt)
      MaxValue = &H7FFF&
    Case ptU127                         'u127
      NumsValue.MaxLength = 4           '0x00 or 127
      NumsValue.Text = Format$(NumsInt And &H7F&)
      MaxValue = &H7F&
    Case ptU16                          'u16
      NumsValue.MaxLength = 3           '0x0 or 16
      NumsValue.Text = Format$(NumsInt And &HF&)
      MaxValue = 16
    Case ptS10                          's10
      NumsValue.MaxLength = 10
      NumsValue.Text = NumsString
    Case ptS32                          's32
      NumsValue.MaxLength = 32
      NumsValue.Text = NumsString
    Case ptS64                          's64
      NumsValue.MaxLength = 64
      NumsValue.Text = NumsString
    Case ptS132                         's132
      NumsValue.MaxLength = 132
      NumsValue.Text = NumsString
    Case ptSsint                        'ssint add case MAG 13 FEB 2001
      NumsValue.MaxLength = 4
      NumsValue.Text = NumsString
  End Select
  If NumsValue.MaxLength > 32 Then
    NSPanel.Height = 2 * NSbaseheight
  Else
    NSPanel.Height = NSbaseheight
  End If
End Sub

Private Sub Form_Load()
  NSbaseheight = NSPanel.Height
End Sub

Private Sub NumsValue_Change()
Dim v$, r As Long, b1

  Select Case ParseType
    Case ptFloat                         'float
      If IsNumeric(v$) Then
        NumsValue.ForeColor = Green
      Else
        NumsValue.ForeColor = Red
      End If
    Case ptUW, ptUD, ptSW, ptU127, ptU16, ptSsint 'uword, udword, sword, u127, u16, ssint MAG add ssint 13 FEB 2001
      If LCase$(Left$(NumsValue.Text, 2)) = "0x" Then
        v$ = "&H" + Mid$(NumsValue.Text, 3)
      Else
        v$ = NumsValue.Text
      End If
      NumsValue.ForeColor = Red
      If IsNumeric(v$) Then
        r = Val(v$)
        If (MaxValue = 0) Or (r <= MaxValue) Then
          b1 = (ParseType = ptU16) Or (ParseType = ptU127)      'these are 1..MaxValue
          If (b1 And (r > 0)) Or (Not b1) Then
            NumsValue.ForeColor = Green
          End If
        End If
      End If
  End Select
End Sub

Private Sub NumsValue_KeyPress(KeyAscii As Integer)
  If ParseType = ptBits Then
    If (KeyAscii < &H30) Or (KeyAscii > &H31) Then 'not a binary digit
      KeyAscii = 0
    End If
  End If
End Sub

Private Sub OKButton_Click()
Dim v$, r As Long, b1

  If LCase$(Left$(NumsValue.Text, 2)) = "0x" Then
    v$ = "&H" + Mid$(NumsValue.Text, 3)
  Else
    v$ = NumsValue.Text
  End If
  Select Case ParseType
    Case ptFloat                        'float
      If IsNumeric(v$) Then
        NumsFloat = Val(v$)
        NumsString = v$
      Else
        Beep                            'complain about bad format
        Exit Sub
      End If
    Case ptUW, ptUD, ptSW, ptU127, ptU16, ptSsint 'uword, udword, sword, u127, u16  MAG add ptSsint 13 FEB 2001
      If IsNumeric(v$) Then
        r = Val(v$)
        NumsString = v$
        b1 = (ParseType = ptU16) Or (ParseType = ptU127) 'these are 1..MaxValue
        If (MaxValue = 0) Or (r <= MaxValue) Then
          If (b1 And (r > 0)) Or (Not b1) Then
            NumsInt = r
          Else
            Beep
            MsgBox "Value must be 1.." + Str$(MaxValue), 48, "Out of Range"
            Exit Sub
          End If
        Else
          Beep
          MsgBox "Value must be less than or equal to" + Str$(MaxValue), 48, "Out of Range"
          Exit Sub
        End If
      Else
        Beep                            'complain about bad format
        Exit Sub
      End If
    Case ptS10, ptS32, ptS64, ptS132, ptStavals, ptBits    's10, s32, s64, s132, stavals, binary
      NumsString = NumsValue.Text
  End Select
  Hide
End Sub

