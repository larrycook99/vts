VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form BitsForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Bit Flags"
   ClientHeight    =   2205
   ClientLeft      =   5820
   ClientTop       =   1710
   ClientWidth     =   4095
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
   ScaleHeight     =   2205
   ScaleWidth      =   4095
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
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
      Left            =   3675
      TabIndex        =   11
      Top             =   60
      Width           =   330
   End
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
      Left            =   3255
      TabIndex        =   10
      Top             =   60
      Width           =   330
   End
   Begin Threed.SSPanel Panel3D1 
      Height          =   225
      Left            =   105
      TabIndex        =   1
      Top             =   105
      Width           =   1065
      _Version        =   65536
      _ExtentX        =   1879
      _ExtentY        =   397
      _StockProps     =   15
      Caption         =   "value:0x"
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
      Begin VB.TextBox HexValue 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         ForeColor       =   &H0000FF00&
         Height          =   195
         Left            =   750
         MaxLength       =   2
         TabIndex        =   2
         Text            =   "00"
         Top             =   15
         Width           =   300
      End
   End
   Begin Threed.SSCheck BitFlags 
      Height          =   225
      Index           =   7
      Left            =   105
      TabIndex        =   0
      Top             =   1890
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Flag Description"
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
   Begin Threed.SSCheck BitFlags 
      Height          =   225
      Index           =   6
      Left            =   105
      TabIndex        =   9
      Top             =   1680
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Flag Description"
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
   Begin Threed.SSCheck BitFlags 
      Height          =   225
      Index           =   5
      Left            =   105
      TabIndex        =   8
      Top             =   1470
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Flag Description"
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
   Begin Threed.SSCheck BitFlags 
      Height          =   225
      Index           =   4
      Left            =   105
      TabIndex        =   7
      Top             =   1260
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Flag Description"
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
   Begin Threed.SSCheck BitFlags 
      Height          =   225
      Index           =   3
      Left            =   105
      TabIndex        =   6
      Top             =   1050
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Flag Description"
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
   Begin Threed.SSCheck BitFlags 
      Height          =   225
      Index           =   2
      Left            =   105
      TabIndex        =   5
      Top             =   840
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Flag Description"
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
   Begin Threed.SSCheck BitFlags 
      Height          =   225
      Index           =   1
      Left            =   105
      TabIndex        =   4
      Top             =   630
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Flag Description"
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
   Begin Threed.SSCheck BitFlags 
      Height          =   225
      Index           =   0
      Left            =   105
      TabIndex        =   3
      Top             =   420
      Visible         =   0   'False
      Width           =   3480
      _Version        =   65536
      _ExtentX        =   6138
      _ExtentY        =   397
      _StockProps     =   78
      Caption         =   "Flag Description"
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
Attribute VB_Name = "BitsForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim Changing

Private Sub BitFlags_Click(Index As Integer, Value As Integer)
Dim i, j, v
  If Changing Then Exit Sub
  Changing = True
  j = 1
  v = 0
  For i = 0 To 7
    If BitFlags(i).Value Then v = v + j
    j = j * 2
  Next i
  HexValue.Text = Right$("0" + Hex$(v), 2)
  Changing = False
End Sub

Private Sub CancelButton_Click()
Dim i, mask
  mask = 1
  BitNames = ""
  For i = 0 To 7
    If BitFlags(i).visible And (BitsValue And mask) Then BitNames = BitNames + BitFlags(i).Caption + ","
    mask = mask * 2
  Next i
  If Right$(BitNames, 1) = "," Then BitNames = Left$(BitNames, Len(BitNames) - 1)
  Hide
End Sub

Private Sub Form_Activate()
Dim i, n$, c$

  n$ = BitNames
  For i = 0 To 7
    c$ = Nxt(n$)                        'get next bit name
    If c$ = "" Then
      BitFlags(i).visible = False
    Else
      BitFlags(i).Caption = c$
      BitFlags(i).visible = True
    End If
  Next i
  Changing = False
  HexValue.Text = Right$("0" + Hex$(BitsValue), 2)
End Sub

Private Sub HexValue_Change()
Dim i, j, v
  If Changing Then Exit Sub
  Changing = True
  j = 1
  v = Val("&H" + HexValue.Text)
  For i = 0 To 7
    BitFlags(i).Value = (v And j)
    j = j * 2
  Next i
  Changing = False
End Sub

Private Sub OKButton_Click()
Dim i
  BitsValue = Val("&H" + HexValue.Text)
  BitNames = ""
  For i = 0 To 7
    If BitFlags(i).visible And BitFlags(i).Value Then BitNames = BitNames + BitFlags(i).Caption + ","
  Next i
  If Right$(BitNames, 1) = "," Then BitNames = Left$(BitNames, Len(BitNames) - 1)
  Hide
End Sub

