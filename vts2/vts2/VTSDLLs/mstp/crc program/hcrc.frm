VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Generate MS/TP Header CRC"
   ClientHeight    =   555
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4050
   LinkTopic       =   "Form1"
   ScaleHeight     =   555
   ScaleWidth      =   4050
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "go"
      Height          =   285
      Left            =   1320
      TabIndex        =   2
      Top             =   120
      Width           =   615
   End
   Begin VB.TextBox Text1 
      Height          =   285
      Left            =   120
      TabIndex        =   0
      Text            =   "0101020000"
      Top             =   120
      Width           =   1095
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "XX"
      Height          =   195
      Left            =   2040
      TabIndex        =   1
      Top             =   180
      Width           =   210
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Private Sub Command1_Click()
Dim i, d&, crclo, t1
  crclo = &HFF
  For i = 1 To Len(Text1.Text) Step 2
    d = Val("&h" + Mid$(Text1.Text, i, 2))
    crclo = (crclo Xor d) And 255
    d = crclo And 255
    d = d * 2
    d = d Xor crclo
    d = d * 2
    d = d Xor crclo
    d = d * 2
    d = d Xor crclo
    d = d * 2
    d = d Xor crclo
    d = d * 2
    d = d Xor crclo
    d = d * 2
    d = d Xor crclo
    d = d * 2
    d = d Xor crclo
    d = d And &H1FE
    t1 = (d \ 256) And 255
    crclo = (d Xor t1) And 255
    Debug.Print Hex$(crclo)
  Next i
  Label1.Caption = Right$("0" + Hex$(crclo Xor &HFF), 2)
End Sub
