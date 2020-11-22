VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form PrivateForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Edit PrivateTransfers"
   ClientHeight    =   2700
   ClientLeft      =   1185
   ClientTop       =   1695
   ClientWidth     =   5250
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
   ScaleHeight     =   2700
   ScaleWidth      =   5250
   ShowInTaskbar   =   0   'False
   Begin Threed.SSPanel MainPanel 
      Height          =   2535
      Left            =   105
      TabIndex        =   0
      Top             =   105
      Width           =   5055
      _Version        =   65536
      _ExtentX        =   8916
      _ExtentY        =   4471
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
         Left            =   4200
         TabIndex        =   7
         Top             =   105
         Width           =   735
      End
      Begin VB.CommandButton CloseButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Cancel          =   -1  'True
         Caption         =   "Close"
         Height          =   375
         Left            =   4200
         TabIndex        =   8
         Top             =   525
         Width           =   735
      End
      Begin VB.TextBox ServiceParameters 
         Height          =   1485
         Left            =   105
         ScrollBars      =   3  'Both
         TabIndex        =   9
         Top             =   945
         Width           =   4845
      End
      Begin Threed.SSPanel Panel3D1 
         Height          =   315
         Left            =   105
         TabIndex        =   5
         Top             =   525
         Width           =   1695
         _Version        =   65536
         _ExtentX        =   2990
         _ExtentY        =   556
         _StockProps     =   15
         Caption         =   " Service #"
         ForeColor       =   8421504
         BackColor       =   -2147483633
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.26
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         BevelOuter      =   1
         Alignment       =   1
         Begin VB.TextBox ServiceNumber 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   285
            Left            =   1050
            MaxLength       =   4
            TabIndex        =   6
            Top             =   0
            Width           =   630
         End
      End
      Begin Threed.SSPanel Panel3D3 
         Height          =   315
         Left            =   105
         TabIndex        =   3
         Top             =   105
         Width           =   1695
         _Version        =   65536
         _ExtentX        =   2990
         _ExtentY        =   556
         _StockProps     =   15
         Caption         =   " Vendor ID"
         ForeColor       =   8421504
         BackColor       =   -2147483633
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.26
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         BevelOuter      =   1
         Alignment       =   1
         Begin VB.TextBox VendorID 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            ForeColor       =   &H0000FF00&
            Height          =   285
            Left            =   1050
            MaxLength       =   5
            TabIndex        =   4
            Top             =   0
            Width           =   630
         End
      End
      Begin Threed.SSPanel Panel3D5 
         Height          =   330
         Left            =   1890
         TabIndex        =   1
         Top             =   105
         Width           =   2220
         _Version        =   65536
         _ExtentX        =   3916
         _ExtentY        =   582
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
         Begin VB.ComboBox VendorNames 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            ForeColor       =   &H00000000&
            Height          =   300
            Left            =   15
            Style           =   2  'Dropdown List
            TabIndex        =   2
            Top             =   15
            Width           =   2190
         End
      End
      Begin Threed.SSCheck Confirmed 
         Height          =   225
         Left            =   1890
         TabIndex        =   10
         Top             =   525
         Width           =   2220
         _Version        =   65536
         _ExtentX        =   3916
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Confirmed?"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
      End
   End
End
Attribute VB_Name = "PrivateForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z

Dim invid

Private Sub CloseButton_Click()
  HideBiModal PrivateForm
End Sub

Private Sub Form_Load()
Dim i
  VendorNames.Clear
  For i = 0 To WhoForm.VendorNames.ListCount - 1
    VendorNames.AddItem WhoForm.VendorNames.List(i)
    VendorNames.ItemData(VendorNames.NewIndex) = WhoForm.VendorNames.ItemData(i)  '***218
  Next i
End Sub

Private Sub SendButton_Click()
Dim apdu As String * 512, alen, p$
  If (VendorID.Text = "") Or (ServiceNumber.Text = "") Then
    Beep
    MsgBox "You must specify a VendorID and ServiceNumber!", 48, "Required Parameter Missing"
    Exit Sub
  End If
  p$ = NoSep(ServiceParameters.Text)
  If Len(ServiceParameters.Text) And 1 Then
    Beep
    If MsgBox("You have entered an odd number of hexadecimal digits for the ServiceParameters string. A trailing zero will be added automatically.", 33, "Missing Hex Digit") <> 1 Then Exit Sub
    p$ = p$ + "0"
  End If
  alen = ePrivateTransferService(apdu, Len(apdu), Confirmed.Value, Val(VendorID.Text), Val(ServiceNumber.Text), Len(p$) / 2, p$)
  If alen Then
    SendNPDU Left$(apdu, alen), True
  Else
    Beep
    MsgBox "Object could not be encoded! Object too Long?", 48, "Encode Error"
    ServiceParameters.SetFocus
    Exit Sub
  End If
End Sub

Private Sub ServiceParameters_KeyPress(KeyAscii As Integer)
Dim dig, alph, sep
  If (KeyAscii >= Asc("a")) And (KeyAscii <= Asc("f")) Then KeyAscii = KeyAscii And &HDF
  If (KeyAscii < Asc("0")) Or (KeyAscii > Asc("9")) Then dig = False Else dig = True
  If (KeyAscii < Asc("A")) Or (KeyAscii > Asc("F")) Then alph = False Else alph = True
  If (KeyAscii = 13) Or (KeyAscii = 32) Then sep = True Else sep = False
  If Not (dig Or alph Or sep) Then KeyAscii = 0
End Sub

Private Sub VendorID_Change()
Dim i, v                                          '                         ***218 Begin
  If invid Then Exit Sub
  invid = True
  If (VendorID.Text = "") Then
    VendorNames.ListIndex = 0
  Else
    v = Val(VendorID.Text)
    For i = 1 To VendorNames.ListCount - 1
      If VendorNames.ItemData(i) = v Then
        VendorNames.ListIndex = i
        GoTo vidc1
      End If
    Next i
    VendorNames.ListIndex = 0
  End If
vidc1:
  invid = False                                   '                         ***218 End
End Sub

Private Sub VendorNames_Click()
  If VendorNames.ListIndex > 0 Then
    VendorID.Text = Format$(VendorNames.ItemData(VendorNames.ListIndex))  '   ***218
'  Else
'    VendorID.Text = ""                           '                           ***218
  End If
End Sub

