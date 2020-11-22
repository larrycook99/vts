VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form VTForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Virtual Terminal"
   ClientHeight    =   5205
   ClientLeft      =   1155
   ClientTop       =   1215
   ClientWidth     =   5685
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
   ScaleHeight     =   5205
   ScaleWidth      =   5685
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton VTDataButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "VT Data"
      Default         =   -1  'True
      Height          =   345
      Left            =   4515
      TabIndex        =   13
      Top             =   4080
      Visible         =   0   'False
      Width           =   960
   End
   Begin VB.CommandButton VTCloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "VT Close"
      Height          =   345
      Left            =   4530
      TabIndex        =   12
      Top             =   4500
      Visible         =   0   'False
      Width           =   960
   End
   Begin VB.CommandButton VTOpenButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "VT Open"
      Height          =   345
      Left            =   4620
      TabIndex        =   11
      Top             =   195
      Width           =   960
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   345
      Left            =   4620
      TabIndex        =   10
      Top             =   600
      Width           =   945
   End
   Begin Threed.SSFrame VTDataFrame 
      Height          =   855
      Left            =   315
      TabIndex        =   16
      Top             =   3990
      Visible         =   0   'False
      Width           =   4005
      _Version        =   65536
      _ExtentX        =   7064
      _ExtentY        =   1508
      _StockProps     =   14
      Caption         =   "VT Data Parameters"
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
      Alignment       =   1
      Begin Threed.SSPanel Panel3D3 
         Height          =   330
         Left            =   3360
         TabIndex        =   7
         Top             =   315
         Width           =   225
         _Version        =   65536
         _ExtentX        =   397
         _ExtentY        =   582
         _StockProps     =   15
         ForeColor       =   8421504
         BackColor       =   0
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
         Begin VB.TextBox DataFlag 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   250
            Left            =   15
            MaxLength       =   7
            TabIndex        =   14
            Text            =   "0"
            Top             =   45
            Width           =   120
         End
      End
      Begin Threed.SSPanel RemoteSessionFrame 
         Height          =   330
         Left            =   1680
         TabIndex        =   15
         Top             =   315
         Width           =   540
         _Version        =   65536
         _ExtentX        =   953
         _ExtentY        =   582
         _StockProps     =   15
         ForeColor       =   8388736
         BackColor       =   0
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
         Alignment       =   1
         Begin VB.TextBox RemoteSession 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   195
            Left            =   30
            MaxLength       =   7
            TabIndex        =   17
            Text            =   "0"
            Top             =   45
            Width           =   495
         End
      End
      Begin VB.Label Label3 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "data flag"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   2520
         TabIndex        =   18
         Top             =   360
         Width           =   780
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "remote session"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   315
         TabIndex        =   19
         Top             =   360
         Width           =   1305
      End
   End
   Begin Threed.SSFrame VTOpenFrame 
      Height          =   855
      Left            =   240
      TabIndex        =   2
      Top             =   105
      Width           =   4005
      _Version        =   65536
      _ExtentX        =   7064
      _ExtentY        =   1508
      _StockProps     =   14
      Caption         =   "VT Open Parameters"
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
      Alignment       =   1
      Begin Threed.SSPanel Panel3D4 
         Height          =   330
         Left            =   105
         TabIndex        =   5
         Top             =   420
         Width           =   3060
         _Version        =   65536
         _ExtentX        =   5398
         _ExtentY        =   582
         _StockProps     =   15
         ForeColor       =   8421504
         BackColor       =   0
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
         Begin VB.ComboBox VTClass 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   6
            Top             =   15
            Width           =   3030
         End
      End
      Begin Threed.SSPanel Panel3D2 
         Height          =   330
         Left            =   3255
         TabIndex        =   3
         Top             =   420
         Width           =   540
         _Version        =   65536
         _ExtentX        =   953
         _ExtentY        =   582
         _StockProps     =   15
         ForeColor       =   8421504
         BackColor       =   0
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
         Begin VB.TextBox LocalSession 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   255
            Left            =   30
            MaxLength       =   7
            TabIndex        =   4
            Text            =   "0"
            Top             =   45
            Width           =   475
         End
      End
      Begin VB.Label Label4 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "session"
         ForeColor       =   &H00808080&
         Height          =   225
         Left            =   3250
         TabIndex        =   9
         Top             =   210
         Width           =   645
      End
      Begin VB.Label Label1 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "VT class"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   105
         TabIndex        =   8
         Top             =   210
         Width           =   750
      End
   End
   Begin Threed.SSPanel VTtextFrame 
      Height          =   2535
      Left            =   105
      TabIndex        =   0
      Top             =   1155
      Visible         =   0   'False
      Width           =   5475
      _Version        =   65536
      _ExtentX        =   9657
      _ExtentY        =   4471
      _StockProps     =   15
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
      BevelInner      =   1
      Autosize        =   3
      Begin VB.TextBox VTtext 
         Appearance      =   0  'Flat
         BackColor       =   &H00000000&
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H0000FF00&
         Height          =   2385
         Left            =   75
         MultiLine       =   -1  'True
         TabIndex        =   1
         Top             =   75
         Width           =   5325
      End
   End
End
Attribute VB_Name = "VTForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
DefInt A-Z
Option Explicit

Dim lsession, rsession
Dim nheight, xheight

Private Sub CloseButton_Click()
  HideBiModal VTForm
End Sub

Private Sub Form_Load()
  nheight = (Height - ScaleHeight) + VTOpenFrame.Top + VTOpenFrame.Height + 105
  xheight = (Height - ScaleHeight) + VTtextFrame.Top + VTtextFrame.Height + 105
  Move 0, 0, Width, nheight
  VTCloseButton.Top = CloseButton.Top
  VTCloseButton.Left = CloseButton.Left
  VTDataButton.Top = VTOpenButton.Top
  VTDataButton.Left = VTOpenButton.Left
  VTDataFrame.Top = VTOpenFrame.Top
  VTDataFrame.Left = VTOpenFrame.Left
  
  VTClass.Clear
  VTClass.AddItem "default-terminal"
  VTClass.AddItem "ansi-x3.64"
  VTClass.AddItem "dec-vt52"
  VTClass.AddItem "dec-vt100"
  VTClass.AddItem "dec-vt220"
  VTClass.AddItem "hp-700/94"
  VTClass.AddItem "ibm-3130"
  VTClass.ListIndex = 0
End Sub

Private Sub VTCloseButton_Click()
Dim apdu As String * 512, alen
  alen = eVTCloseService(apdu, rsession)
  If alen Then
    SendNPDU Left$(apdu, alen), True
  Else
    Beep
    MsgBox "Object could not be encoded!", 48, "Encode Error"
    VTClass.SetFocus
    Exit Sub
  End If
  Height = nheight
  VTOpenFrame.visible = True
  VTOpenButton.visible = True
  CloseButton.visible = True
  VTCloseButton.visible = False
  VTDataButton.visible = False
  VTDataFrame.visible = False
  VTtextFrame.visible = False
End Sub

Private Sub VTDataButton_Click()
Dim apdu As String * 512, alen, flag, n
  If remotesession.visible Then
    rsession = Val(Trim$(remotesession.Text))
    If rsession < 0 Or rsession > 255 Then
      Beep
      MsgBox "Please enter a remote session from 0 to 255.", 48, "Invalid Session"
      Exit Sub
    End If
    RemoteSessionFrame.Caption = remotesession.Text
    remotesession.visible = False
  End If
  flag = Val(Trim$(dataflag.Text))
  n = Len(VTtext.Text)
  If n = 0 Then Exit Sub
  alen = eVTDataService(apdu, Len(apdu), rsession, flag, n, VTtext.Text)
  If alen Then
    SendNPDU Left$(apdu, alen), True
  Else
    Beep
    MsgBox "Object could not be encoded! Object too Long.", 48, "Encode Error"
    VTClass.SetFocus
    Exit Sub
  End If

  If flag = 0 Then
    dataflag.Text = "1"
  Else
    dataflag.Text = "0"
  End If
End Sub

Private Sub VTOpenButton_Click()
Dim apdu As String * 512, alen
  lsession = Val(Trim$(localsession.Text))
  If lsession < 0 Or lsession > 255 Then
    Beep
    MsgBox "Please enter a local session from 0 to 255.", 48, "Invalid Session"
  End If
  alen = eVTOpenService(apdu, VTClass.ListIndex, lsession)
  If alen Then
    SendNPDU Left$(apdu, alen), True
  Else
    Beep
    MsgBox "Object could not be encoded!", 48, "Encode Error"
    VTClass.SetFocus
    Exit Sub
  End If
  Height = xheight
  VTOpenFrame.visible = False
  VTOpenButton = False
  CloseButton.visible = False
  VTCloseButton.visible = True
  VTDataButton.visible = True
  VTDataFrame.visible = True
  VTtextFrame.visible = True
  VTtext.visible = True
End Sub
                 

