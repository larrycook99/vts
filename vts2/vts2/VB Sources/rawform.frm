VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form rawForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Edit Raw NPDUs"
   ClientHeight    =   3075
   ClientLeft      =   1185
   ClientTop       =   1695
   ClientWidth     =   6255
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
   ScaleHeight     =   3075
   ScaleWidth      =   6255
   ShowInTaskbar   =   0   'False
   Begin Threed.SSPanel MainPanel 
      Height          =   2895
      Left            =   105
      TabIndex        =   0
      Top             =   105
      Width           =   6015
      _Version        =   65536
      _ExtentX        =   10610
      _ExtentY        =   5106
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
      Begin VB.CommandButton SendButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "Send"
         Default         =   -1  'True
         Height          =   375
         Left            =   4320
         TabIndex        =   1
         Top             =   120
         Width           =   735
      End
      Begin VB.CommandButton CloseButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Cancel          =   -1  'True
         Caption         =   "Close"
         Height          =   375
         Left            =   5160
         TabIndex        =   2
         Top             =   120
         Width           =   735
      End
      Begin VB.TextBox ServiceParameters 
         Height          =   2190
         Left            =   105
         MultiLine       =   -1  'True
         ScrollBars      =   3  'Both
         TabIndex        =   3
         Top             =   585
         Width           =   5790
      End
      Begin VB.Label Label1 
         Caption         =   "Raw NPDU:"
         Height          =   255
         Left            =   120
         TabIndex        =   4
         Top             =   240
         Width           =   1215
      End
   End
End
Attribute VB_Name = "rawForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z


Private Sub CloseButton_Click()
  HideBiModal rawForm
End Sub

Private Sub SendButton_Click()
Dim alen, p$, i
  p$ = NoSep(ServiceParameters.Text)
  If Len(ServiceParameters.Text) And 1 Then
    Beep
    If MsgBox("You have entered an odd number of hexadecimal digits for the ServiceParameters string. A trailing zero will be added automatically.", 33, "Missing Hex Digit") <> 1 Then Exit Sub
    p$ = p$ + "0"
  End If
  alen = Len(ServiceParameters.Text) \ 2
  If alen < 1 Or alen > 1476 Then
    Beep
    MsgBox "Object could not be encoded! Object too Long?", 48, "Encode Error"
    ServiceParameters.SetFocus
    Exit Sub
  End If
  For i = 1 To alen
    Mid$(NLpkt.maNPDU, i, 1) = Chr$(Val("&h" + Mid$(p$, 1 + ((i - 1) * 2), 2)))
  Next i
  NLpkt.maNlen = alen
  SendNL
End Sub

Private Sub ServiceParameters_KeyPress(KeyAscii As Integer)
Dim dig, alph, sep
  If (KeyAscii >= Asc("a")) And (KeyAscii <= Asc("f")) Then KeyAscii = KeyAscii And &HDF
  If (KeyAscii < Asc("0")) Or (KeyAscii > Asc("9")) Then dig = False Else dig = True
  If (KeyAscii < Asc("A")) Or (KeyAscii > Asc("F")) Then alph = False Else alph = True
  If (KeyAscii = 13) Or (KeyAscii = 32) Then sep = True Else sep = False
  If Not (dig Or alph Or sep) Then KeyAscii = 0
End Sub


