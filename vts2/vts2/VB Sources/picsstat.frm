VERSION 5.00
Begin VB.Form PicsTestStatusForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   Caption         =   "Pics Test Status"
   ClientHeight    =   4050
   ClientLeft      =   1725
   ClientTop       =   3510
   ClientWidth     =   7380
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
   Icon            =   "PICSSTAT.frx":0000
   LinkTopic       =   "Form1"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4050
   ScaleWidth      =   7380
   Begin VB.ListBox StatusList 
      Height          =   3765
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   7335
   End
End
Attribute VB_Name = "PicsTestStatusForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Resize()
'set the new size for the StatusList
StatusList.Move 0, 0, Me.ScaleWidth, Me.ScaleHeight
End Sub

Private Sub StatusList_Click()

Select Case gState.ActiveList
  Case cTEST:
    PicsTestForm!TestList.ListIndex = StatusList.ListIndex
  Case cCONTENT:
    PicsTestForm!ContentList.ListIndex = StatusList.ListIndex
  Case cRESULT:
    PicsTestForm!ResultList.ListIndex = StatusList.ListIndex
End Select

End Sub

