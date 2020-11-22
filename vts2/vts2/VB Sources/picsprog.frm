VERSION 5.00
Begin VB.Form PicsProgressForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "PICS progressing..."
   ClientHeight    =   2595
   ClientLeft      =   6900
   ClientTop       =   510
   ClientWidth     =   3420
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
   Icon            =   "PICSPROG.frx":0000
   LinkTopic       =   "Form1"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2595
   ScaleWidth      =   3420
   Begin VB.HScrollBar ProgScroll 
      Enabled         =   0   'False
      Height          =   732
      Left            =   120
      TabIndex        =   0
      Top             =   1380
      Width           =   3192
   End
   Begin VB.Label DescriptionLab 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      ForeColor       =   &H80000008&
      Height          =   252
      Left            =   120
      TabIndex        =   7
      Top             =   1020
      Width           =   1932
   End
   Begin VB.Label Label2 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "max:"
      ForeColor       =   &H80000008&
      Height          =   192
      Left            =   2220
      TabIndex        =   6
      Top             =   2220
      Width           =   432
   End
   Begin VB.Label Label1 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "min:"
      ForeColor       =   &H80000008&
      Height          =   192
      Left            =   120
      TabIndex        =   5
      Top             =   2220
      Width           =   432
   End
   Begin VB.Label PresLab 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      ForeColor       =   &H80000008&
      Height          =   252
      Left            =   2160
      TabIndex        =   4
      Top             =   1020
      Width           =   1152
   End
   Begin VB.Label MaxLab 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      ForeColor       =   &H80000008&
      Height          =   252
      Left            =   2700
      TabIndex        =   3
      Top             =   2220
      Width           =   612
   End
   Begin VB.Label MinLab 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      ForeColor       =   &H80000008&
      Height          =   252
      Left            =   600
      TabIndex        =   2
      Top             =   2220
      Width           =   612
   End
   Begin VB.Label TitleLab 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      ForeColor       =   &H80000008&
      Height          =   732
      Left            =   120
      TabIndex        =   1
      Top             =   120
      Width           =   3192
   End
End
Attribute VB_Name = "PicsProgressForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

