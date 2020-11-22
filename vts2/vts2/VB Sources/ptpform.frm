VERSION 5.00
Begin VB.Form PTPForm 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "PTP Peer"
   ClientHeight    =   1800
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4185
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1800
   ScaleWidth      =   4185
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.Frame DisconnectFrame 
      Caption         =   "Disconnect Reason"
      Height          =   735
      Left            =   120
      TabIndex        =   4
      Top             =   960
      Visible         =   0   'False
      Width           =   2895
      Begin VB.ComboBox DisconnectReason 
         Height          =   315
         ItemData        =   "ptpform.frx":0000
         Left            =   240
         List            =   "ptpform.frx":0010
         Style           =   2  'Dropdown List
         TabIndex        =   5
         Top             =   360
         Width           =   2415
      End
   End
   Begin VB.CommandButton CancelButton 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3240
      TabIndex        =   3
      Top             =   600
      Width           =   855
   End
   Begin VB.CommandButton OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   3240
      TabIndex        =   2
      Top             =   120
      Width           =   855
   End
   Begin VB.Frame ConnectFrame 
      Caption         =   "Connection Password"
      Height          =   735
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Visible         =   0   'False
      Width           =   2895
      Begin VB.TextBox ConnectionPassword 
         Height          =   285
         Left            =   240
         TabIndex        =   1
         Top             =   360
         Width           =   2415
      End
   End
End
Attribute VB_Name = "PTPForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Private Sub CancelButton_Click()
  DevDo = 0
  Hide
End Sub

Private Sub ConnectionPassword_GotFocus()
  GoSelect ConnectionPassword
End Sub

Private Sub Form_Load()
  DisconnectReason.ListIndex = 0
  Height = (Height - ScaleHeight) + DisconnectFrame.Top
  DisconnectFrame.Top = ConnectFrame.Top
End Sub

Private Sub OKButton_Click()
  DevDo = 1
  Hide
End Sub
