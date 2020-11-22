VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Test MSTP MAC Layer"
   ClientHeight    =   1215
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   9270
   LinkTopic       =   "Form1"
   ScaleHeight     =   1215
   ScaleWidth      =   9270
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command2 
      Caption         =   "cancel"
      Height          =   255
      Left            =   3120
      TabIndex        =   7
      Top             =   840
      Width           =   975
   End
   Begin VB.TextBox address 
      Height          =   285
      Left            =   960
      TabIndex        =   6
      Top             =   840
      Width           =   855
   End
   Begin VB.CommandButton Command1 
      Caption         =   "send"
      Height          =   255
      Left            =   1920
      TabIndex        =   2
      Top             =   840
      Width           =   1095
   End
   Begin VB.TextBox outputdata 
      Height          =   285
      Left            =   600
      TabIndex        =   0
      Top             =   480
      Width           =   8535
   End
   Begin VB.Label Label1 
      Caption         =   "hexaddr"
      Height          =   255
      Index           =   2
      Left            =   120
      TabIndex        =   5
      Top             =   840
      Width           =   615
   End
   Begin VB.Label Label1 
      Caption         =   "out"
      Height          =   255
      Index           =   1
      Left            =   120
      TabIndex        =   4
      Top             =   480
      Width           =   255
   End
   Begin VB.Label Label1 
      Caption         =   "in"
      Height          =   255
      Index           =   0
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   255
   End
   Begin VB.Label inputdata 
      BackColor       =   &H00C0E0FF&
      Height          =   255
      Left            =   600
      TabIndex        =   1
      Top             =   120
      Width           =   8535
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Private Type MApacket
  link As Long
  trace As Long
  Port As Byte
  alen As Byte
  lsap As Byte
  broadcast As Byte
  resv(0 To 5) As Byte
  dadr(0 To 7) As Byte
  sadr(0 To 7) As Byte
  nlen As Integer
  npdu(0 To 1499) As Byte
End Type

Dim initialized As Boolean, invid As Byte
Dim mp As MApacket
Dim cancel As Boolean

Private Declare Function mstpInit Lib "mstp.dll" () As Long
Private Declare Sub mstpDeinit Lib "mstp.dll" ()
Private Declare Function mstpTransmit Lib "mstp.dll" (m As MApacket) As Long
Private Declare Function mstpReceive Lib "mstp.dll" (m As MApacket) As Boolean
Private Declare Sub mstpVarMaxMaster Lib "mstp.dll" (max As Long, ByVal writeit As Boolean)
Private Declare Sub mstpVarMaxInfoFrames Lib "mstp.dll" (max As Long, ByVal writeit As Boolean)

Private Declare Function LoadLibrary Lib "kernel32" Alias "LoadLibraryA" (ByVal lpLibFileName As String) As Long

Private Sub FindDLL(ByVal d$)
Dim r As Long
  r = LoadLibrary(d$)
  If r = 0 Then
    Beep
    MsgBox "Can't Load DLL [" + d$ + "]", 16, "DLL Load Failed"
    End
  End If
End Sub


Private Sub Command2_Click()
  cancel = True
End Sub

Private Sub Form_Load()
Dim r As Long, v
'  FindDLL "c:\vts\mstp.dll"
  r = mstpInit()
  If r = 0 Then
    initialized = True
    mstpVarMaxMaster r, False
    mstpVarMaxInfoFrames r, False
  Else
    MsgBox "Failed to initialize MSTP, error=" + Format$(r), 48
  End If
End Sub

Private Sub Form_QueryUnload(cancel As Integer, UnloadMode As Integer)
  If initialized Then mstpDeinit
End Sub

Private Sub SendAgain()
Dim s$, i As Integer
  mp.Port = 11
  mp.dadr(0) = Val("&H" + Right("0" + Trim$(address.Text), 2))
  If mp.dadr(0) = 255 Then
    mp.broadcast = 1
  Else
    mp.broadcast = 0
  End If
  s$ = Trim$(outputdata.Text)
  mp.nlen = Len(s$) \ 2
  For i = 0 To mp.nlen - 1
    mp.npdu(i) = Val("&H" + Mid(s$, (i * 2) + 1, 2))
  Next i
  mstpTransmit mp
End Sub

Private Sub Command1_Click()
Dim i, n$
  cancel = False
  inputdata.Caption = "waiting"
  inputdata.Refresh
  SendAgain
  Do While cancel = False
    DoEvents
    If mstpReceive(mp) Then
      n$ = ""
      n$ = "[" + Right$("0" + Hex$(mp.dadr(0)), 2) + "]"
      For i = 0 To mp.nlen - 1
        n$ = n$ + Right$("0" + Hex$(mp.npdu(i)), 2)
      Next i
      inputdata.Caption = n$
      inputdata.Refresh
      cancel = False
      Exit Sub
    End If
  Loop
  inputdata.Caption = "cancelled"
  inputdata.Refresh
End Sub


