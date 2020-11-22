VERSION 5.00
Begin VB.Form PicsOkForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "PICS Parsing"
   ClientHeight    =   1755
   ClientLeft      =   2475
   ClientTop       =   3390
   ClientWidth     =   3735
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
   Icon            =   "PICSOKFO.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1755
   ScaleWidth      =   3735
   ShowInTaskbar   =   0   'False
   Begin VB.Timer PicsOkTimer 
      Interval        =   3000
      Left            =   240
      Top             =   1200
   End
   Begin VB.CommandButton OkButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&Ok"
      Height          =   372
      Left            =   1440
      TabIndex        =   0
      Top             =   1200
      Width           =   972
   End
   Begin VB.Label PicsOkLab 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      ForeColor       =   &H80000008&
      Height          =   612
      Left            =   960
      TabIndex        =   1
      Top             =   240
      Width           =   2172
      WordWrap        =   -1  'True
   End
End
Attribute VB_Name = "PicsOkForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Load()
DrawStdIcon 10, 10, PicsOkForm.hwnd, cIDI_ASTERISK
End Sub

Private Sub Form_Paint()
DrawStdIcon 20, 30, PicsOkForm.hwnd, cIDI_ASTERISK
End Sub

Private Sub OKButton_Click()
Unload PicsOkForm
End Sub

Private Sub PicsOkTimer_Timer()
OKButton_Click
End Sub

