VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Test PTP MAC Layer"
   ClientHeight    =   990
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   9045
   LinkTopic       =   "Form1"
   ScaleHeight     =   990
   ScaleWidth      =   9045
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "start"
      Height          =   255
      Left            =   7680
      TabIndex        =   4
      Top             =   120
      Width           =   1095
   End
   Begin VB.CheckBox Check1 
      Caption         =   "Start Sending Continuously"
      Height          =   195
      Left            =   3240
      TabIndex        =   2
      Top             =   240
      Width           =   3015
   End
   Begin VB.TextBox password 
      Height          =   285
      Left            =   1320
      TabIndex        =   1
      Top             =   120
      Width           =   1695
   End
   Begin VB.CommandButton ConnectButton 
      Caption         =   "Connect"
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1095
   End
   Begin VB.Label result 
      BackColor       =   &H00C0E0FF&
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   600
      Width           =   8775
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

Private Declare Function ptpInit Lib "ptp.dll" () As Long
Private Declare Sub ptpDeinit Lib "ptp.dll" ()
Private Declare Function ptpConnect Lib "ptp.dll" (ByVal pass$) As Long
Private Declare Function ptpDisconnect Lib "ptp.dll" (ByVal reason) As Long
Private Declare Function ptpTransmit Lib "ptp.dll" (m As MApacket) As Long
Private Declare Function ptpReceive Lib "ptp.dll" (m As MApacket) As Boolean

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

Private Sub ConnectButton_Click()
  ptpConnect password.Text
End Sub

Private Sub Form_Load()
Dim r As Long, v
'  FindDLL "c:\vts\ptp.dll"
  r = ptpInit()
  If r = 0 Then
    initialized = True
  Else
    MsgBox "Failed to initialize PTP, error=" + Format$(r), 48
  End If
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
  If initialized Then ptpDeinit
End Sub

Private Sub SendAgain()
  mp.Port = 10
  mp.nlen = 22
  mp.npdu(0) = 1
  mp.npdu(1) = &H2C
  mp.npdu(2) = 0
  mp.npdu(3) = 1
  mp.npdu(4) = 1
  mp.npdu(5) = 1  'was 3
  mp.npdu(6) = 0
  mp.npdu(7) = 2
  mp.npdu(8) = 1
  mp.npdu(9) = 5
  mp.npdu(10) = 255
  
  mp.npdu(11) = 0
  mp.npdu(12) = 0
  mp.npdu(13) = invid
  mp.npdu(14) = 12
  mp.npdu(15) = 12
  mp.npdu(16) = 0
  mp.npdu(17) = 0
  mp.npdu(18) = 0
  mp.npdu(19) = 12
  mp.npdu(20) = &H19
  mp.npdu(21) = 85
  ptpTransmit mp
  invid = (invid + 1) And 255
End Sub

Private Sub Command1_Click()
Dim i, n$
  result.Caption = ""
  SendAgain
  Do While Check1.Value
    DoEvents
    If ptpReceive(mp) Then
      n$ = ""
      For i = 0 To mp.nlen - 1
        n$ = n$ + Right$("0" + Hex$(mp.npdu(i)), 2)
      Next i
      result.Caption = n$
      result.Refresh
      SendAgain
    End If
  Loop
End Sub


