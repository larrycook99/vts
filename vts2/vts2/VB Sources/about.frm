VERSION 5.00
Begin VB.Form AboutForm 
   Appearance      =   0  'Flat
   BackColor       =   &H80000005&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "About Visual Test Shell"
   ClientHeight    =   1785
   ClientLeft      =   2235
   ClientTop       =   1395
   ClientWidth     =   6030
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
   Icon            =   "ABOUT.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1785
   ScaleWidth      =   6030
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton OKButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   5040
      TabIndex        =   0
      Top             =   120
      Width           =   855
   End
   Begin VB.Label Label2 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "©1995-2000, National Institute of Standards and Technology. Portions of this software were developed for NIST by PolarSoft® Inc."
      ForeColor       =   &H80000008&
      Height          =   1335
      Left            =   2040
      TabIndex        =   2
      Top             =   480
      Width           =   2775
   End
   Begin VB.Label AboutTitle 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Visual Test Shell v0.00"
      ForeColor       =   &H80000008&
      Height          =   195
      Left            =   2040
      TabIndex        =   1
      Top             =   120
      Width           =   1980
   End
   Begin VB.Image AboutImage 
      Appearance      =   0  'Flat
      Height          =   1800
      Left            =   0
      Picture         =   "ABOUT.frx":000C
      Stretch         =   -1  'True
      Top             =   0
      Width           =   1800
   End
End
Attribute VB_Name = "AboutForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Private Sub Form_Load()
  AboutTitle.Caption = "Visual Test Shell " + vtsversion
  AboutForm.Move (Screen.Width / 2 - Width / 2), (Screen.Height / 2 - Height / 2)
End Sub

Private Sub OKButton_Click()
  Unload AboutForm
End Sub

