VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form TargetForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Target Device"
   ClientHeight    =   4515
   ClientLeft      =   7500
   ClientTop       =   855
   ClientWidth     =   5340
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
   Icon            =   "TARGET.frx":0000
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4515
   ScaleWidth      =   5340
   ShowInTaskbar   =   0   'False
   Begin Threed.SSFrame SourceFrame 
      Height          =   960
      Left            =   105
      TabIndex        =   28
      Top             =   3465
      Width           =   5160
      _Version        =   65536
      _ExtentX        =   9102
      _ExtentY        =   1693
      _StockProps     =   14
      Caption         =   "Source Routing"
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
      Begin Threed.SSPanel Panel3D2 
         Height          =   255
         Left            =   1680
         TabIndex        =   31
         Top             =   525
         Width           =   3375
         _Version        =   65536
         _ExtentX        =   5953
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "MAC Address    0x"
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
         Alignment       =   1
         Begin VB.TextBox SourceMACAddress 
            Appearance      =   0  'Flat
            BackColor       =   &H00C0C0C0&
            BorderStyle     =   0  'None
            ForeColor       =   &H00000080&
            Height          =   225
            Left            =   1590
            MaxLength       =   16
            TabIndex        =   32
            Text            =   "0000000000000000"
            Top             =   15
            Width           =   1725
         End
      End
      Begin Threed.SSPanel SNETpanel 
         Height          =   255
         Left            =   105
         TabIndex        =   29
         Top             =   525
         Width           =   1485
         _Version        =   65536
         _ExtentX        =   2619
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "Network"
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
         Alignment       =   1
         Begin VB.TextBox SourceNetwork 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   225
            Left            =   840
            MaxLength       =   5
            TabIndex        =   30
            Text            =   "00000"
            Top             =   15
            Width           =   630
         End
      End
      Begin Threed.SSCheck IncludeSourceRouting 
         Height          =   225
         Left            =   105
         TabIndex        =   33
         Top             =   210
         Width           =   2955
         _Version        =   65536
         _ExtentX        =   5212
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Include Source Routing?"
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
      End
   End
   Begin Threed.SSFrame RoutingFrame 
      Height          =   2220
      Left            =   105
      TabIndex        =   21
      Top             =   1155
      Width           =   5160
      _Version        =   65536
      _ExtentX        =   9102
      _ExtentY        =   3916
      _StockProps     =   14
      Caption         =   "Routing"
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
      Begin VB.CommandButton NewRouterButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "new alias"
         Height          =   225
         Left            =   3990
         TabIndex        =   13
         TabStop         =   0   'False
         Top             =   1050
         Visible         =   0   'False
         Width           =   1065
      End
      Begin Threed.SSPanel RAPanel 
         Height          =   345
         Left            =   2400
         TabIndex        =   14
         Top             =   1320
         Visible         =   0   'False
         Width           =   2670
         _Version        =   65536
         _ExtentX        =   4710
         _ExtentY        =   609
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
         Begin VB.ComboBox RouterAlias 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            Height          =   315
            Left            =   15
            Sorted          =   -1  'True
            TabIndex        =   11
            Top             =   15
            Width           =   2640
         End
         Begin VB.TextBox RouterAliasDummy 
            Height          =   375
            Left            =   0
            TabIndex        =   35
            Text            =   "Text1"
            Top             =   0
            Width           =   2655
         End
      End
      Begin Threed.SSPanel RMpanel 
         Height          =   255
         Left            =   315
         TabIndex        =   15
         Top             =   1380
         Visible         =   0   'False
         Width           =   2010
         _Version        =   65536
         _ExtentX        =   3545
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "0x"
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
         Alignment       =   1
         Begin VB.TextBox RouterMACAddress 
            Appearance      =   0  'Flat
            BackColor       =   &H00C0C0C0&
            BorderStyle     =   0  'None
            ForeColor       =   &H00000080&
            Height          =   225
            Left            =   240
            MaxLength       =   16
            TabIndex        =   10
            Text            =   "0000000000000000"
            Top             =   15
            Width           =   1725
         End
      End
      Begin Threed.SSPanel NetworkPanel 
         Height          =   255
         Left            =   1365
         TabIndex        =   26
         Top             =   735
         Visible         =   0   'False
         Width           =   1485
         _Version        =   65536
         _ExtentX        =   2619
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "Network"
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
         Alignment       =   1
         Begin VB.TextBox Network 
            Alignment       =   1  'Right Justify
            Appearance      =   0  'Flat
            BackColor       =   &H00000000&
            BorderStyle     =   0  'None
            ForeColor       =   &H0000FF00&
            Height          =   225
            Left            =   840
            MaxLength       =   5
            TabIndex        =   8
            Text            =   "00000"
            Top             =   15
            Width           =   630
         End
      End
      Begin Threed.SSPanel PortPanel 
         Height          =   345
         Left            =   1365
         TabIndex        =   23
         Top             =   315
         Width           =   3270
         _Version        =   65536
         _ExtentX        =   5768
         _ExtentY        =   609
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
         Begin VB.ComboBox Port 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            Height          =   315
            Left            =   15
            Sorted          =   -1  'True
            Style           =   2  'Dropdown List
            TabIndex        =   7
            Top             =   15
            Width           =   3240
         End
      End
      Begin Threed.SSPanel Panel3D6 
         Height          =   330
         Left            =   2415
         TabIndex        =   22
         Top             =   1785
         Width           =   2640
         _Version        =   65536
         _ExtentX        =   4657
         _ExtentY        =   582
         _StockProps     =   15
         Caption         =   "Priority:"
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
         Alignment       =   1
         Begin VB.ComboBox Priority 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            Height          =   315
            Left            =   735
            Style           =   2  'Dropdown List
            TabIndex        =   12
            Top             =   15
            Width           =   1875
         End
      End
      Begin Threed.SSCheck RouterUnknown 
         Height          =   225
         Left            =   2940
         TabIndex        =   9
         Top             =   735
         Visible         =   0   'False
         Width           =   1695
         _Version        =   65536
         _ExtentX        =   2990
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Router Unknown"
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
         Value           =   -1  'True
      End
      Begin Threed.SSOption GlobalRoute 
         Height          =   225
         Left            =   105
         TabIndex        =   6
         TabStop         =   0   'False
         Top             =   1785
         Width           =   960
         _Version        =   65536
         _ExtentX        =   1693
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Global"
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
      Begin Threed.SSOption RemoteRoute 
         Height          =   225
         Left            =   105
         TabIndex        =   5
         TabStop         =   0   'False
         Top             =   735
         Width           =   960
         _Version        =   65536
         _ExtentX        =   1693
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Remote"
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
      Begin Threed.SSOption LocalRoute 
         Height          =   225
         Left            =   105
         TabIndex        =   4
         Top             =   315
         Width           =   960
         _Version        =   65536
         _ExtentX        =   1693
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Local"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Value           =   -1  'True
      End
      Begin VB.Label rmalabel 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "Router MAC Addr"
         ForeColor       =   &H80000008&
         Height          =   195
         Left            =   345
         TabIndex        =   24
         Top             =   1155
         Visible         =   0   'False
         Width           =   1485
      End
      Begin VB.Label ralabel 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "Router Alias"
         ForeColor       =   &H80000008&
         Height          =   195
         Left            =   2415
         TabIndex        =   27
         Top             =   1155
         Visible         =   0   'False
         Width           =   1050
      End
      Begin VB.Label portlabel 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "port:"
         ForeColor       =   &H00808080&
         Height          =   195
         Left            =   1365
         TabIndex        =   25
         Top             =   105
         Width           =   405
      End
   End
   Begin Threed.SSFrame DestFrame 
      Height          =   1065
      Left            =   105
      TabIndex        =   16
      Top             =   0
      Width           =   5160
      _Version        =   65536
      _ExtentX        =   9102
      _ExtentY        =   1879
      _StockProps     =   14
      Caption         =   "Destination"
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
      Begin VB.CommandButton NewAliasButton 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         Caption         =   "new alias"
         Height          =   225
         Left            =   3990
         TabIndex        =   20
         TabStop         =   0   'False
         Top             =   315
         Width           =   1065
      End
      Begin Threed.SSPanel Panel3D1 
         Height          =   345
         Left            =   2400
         TabIndex        =   18
         Top             =   600
         Width           =   2670
         _Version        =   65536
         _ExtentX        =   4710
         _ExtentY        =   609
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
         Begin VB.ComboBox DestAlias 
            Appearance      =   0  'Flat
            BackColor       =   &H00FFFFFF&
            Height          =   315
            Left            =   15
            Sorted          =   -1  'True
            TabIndex        =   3
            Top             =   15
            Width           =   2640
         End
         Begin VB.TextBox DestAliasDummy 
            Height          =   375
            Left            =   0
            TabIndex        =   34
            Text            =   "Text1"
            Top             =   0
            Width           =   2655
         End
      End
      Begin Threed.SSPanel Panel3D3 
         Height          =   255
         Left            =   315
         TabIndex        =   17
         Top             =   645
         Width           =   2010
         _Version        =   65536
         _ExtentX        =   3545
         _ExtentY        =   450
         _StockProps     =   15
         Caption         =   "0x"
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
         Alignment       =   1
         Begin VB.TextBox DestMACAddress 
            Appearance      =   0  'Flat
            BackColor       =   &H00C0C0C0&
            BorderStyle     =   0  'None
            ForeColor       =   &H00000080&
            Height          =   225
            Left            =   240
            MaxLength       =   16
            TabIndex        =   2
            Text            =   "0000000000000000"
            Top             =   15
            Width           =   1725
         End
      End
      Begin Threed.SSOption SpecificDest 
         Height          =   225
         Left            =   105
         TabIndex        =   1
         TabStop         =   0   'False
         Top             =   420
         Width           =   1800
         _Version        =   65536
         _ExtentX        =   3175
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "MAC Address"
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
      Begin Threed.SSOption BroadcastDest 
         Height          =   225
         Left            =   105
         TabIndex        =   0
         Top             =   210
         Width           =   1380
         _Version        =   65536
         _ExtentX        =   2434
         _ExtentY        =   397
         _StockProps     =   78
         Caption         =   "Broadcast"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Value           =   -1  'True
      End
      Begin VB.Label Label1 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         BackStyle       =   0  'Transparent
         Caption         =   "Alias"
         ForeColor       =   &H80000008&
         Height          =   195
         Left            =   2415
         TabIndex        =   19
         Top             =   420
         Width           =   420
      End
   End
End
Attribute VB_Name = "TargetForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
DefInt A-Z


Private Sub DestAlias_Click()
'Note: The work has been moved to LostFocus because of a VB5    ***211 Begin
'       bug that clears the Text when ListIndex->-1!
Dim d$, l$                                  '                   ***212
  l$ = DestAlias.Text                       '                   ***212
  d$ = NxtC(l$, Chr$(9))                    '                   ***212
  DestMACAddress.Text = l$                  '                   ***212
'  DestAlias.Text = d$
  DestAliasDummy.SetFocus
'                                                               ***211 End
End Sub

Private Sub DestAlias_GotFocus()
  GoSelect DestAlias
End Sub

Private Sub DestAlias_KeyPress(KeyAscii As Integer)
  If KeyAscii = 13 Then
    KeyAscii = 0
    NewAliasButton_Click
  End If
End Sub

Private Sub DestAlias_LostFocus()
' Note: This must be done here because of a VB5 bug!        ***211 Begin
Dim d$, l$
  l$ = DestAlias.Text
  d$ = NxtC(l$, Chr$(9))
'  DestMACAddress.Text = l$         '                       ***212
  DestAlias.Text = d$
'                                                           ***211 End
End Sub

Private Sub DestAliasDummy_GotFocus()
' Note: This is required to be here because of a VB5 bug!   ***211 Begin
'
  DestAlias.SetFocus                    '                   ***210 End
End Sub


Private Sub DestMACAddress_GotFocus()
  GoSelect DestMACAddress
End Sub

Private Sub DestMACAddress_KeyPress(KeyAscii As Integer)
  If KeyAscii = 13 Then
    KeyAscii = 0
    NewAliasButton_Click
  End If
End Sub

Private Sub Form_Load()
Dim f, j, plu, l$, NL As String * 2, pl As String * 512
  NL = Chr$(13) + Chr$(10)
  RFtop = RoutingFrame.Top

  priority.Clear
  priority.AddItem "Normal"
  priority.AddItem "Urgent"
  priority.AddItem "Critical Equipment"
  priority.AddItem "Life Safety"
  priority.ListIndex = 0
  Port.Clear
  plu = NALSAPIgetportlist(pl)                  'get list of port names
  f = 1
  Do While f < plu
    j = InStr(f, pl, NL)                        'find next newline pair
    If j Then                                   'found one
      l$ = Mid$(pl, f, j - f)
    Else                                        'this is the last string
      l$ = Mid$(pl, f)
    End If
    Port.AddItem l$
    If InStr(l$, "PTP") Then                    'is this a PTP port?          ***214 Begin
    'there must be a better way to find this out...
      VTSform.PTPMenu.Enabled = True            'if there is a PTP, then enable this menu
    End If                                      '                             ***214 End
    If j = 0 Then Exit Do
    f = j + 2                                   'skip nl pair
  Loop
  If Port.ListCount = 0 Then
    Port.AddItem "No Ports Defined!"
  End If
  If Port.ListCount >= 1 Then Port.ListIndex = 0    '           ***212
  ReadAliases DestAlias, DestAliasFile
  ReadAliases RouterAlias, RouterAliasFile
End Sub

Private Sub Form_Unload(Cancel As Integer)
  VTSform.ViewTargetDevice.Checked = False
End Sub

Private Sub GlobalRoute_Click(Value As Integer)
  If Value Then
        portlabel.visible = False
        PortPanel.visible = False
        NetworkPanel.visible = False
        RouterUnknown.visible = False
        rmalabel.visible = False
        ralabel.visible = False
        NewRouterButton.visible = False
        RMpanel.visible = False
        RAPanel.visible = False
  End If
End Sub

Private Sub LocalRoute_Click(Value As Integer)
  If Value Then
        portlabel.visible = True
        PortPanel.visible = True
        NetworkPanel.visible = False
        RouterUnknown.visible = False
        rmalabel.visible = False
        ralabel.visible = False
        NewRouterButton.visible = False
        RMpanel.visible = False
        RAPanel.visible = False
  End If
End Sub

Private Sub Network_GotFocus()
  GoSelect Network
End Sub

Private Sub NewAliasButton_Click()
  If DestAlias.Text = "" Then
    Beep
  Else
    DestAlias.AddItem DestAlias.Text + Chr$(9) + DestMACAddress.Text
    SaveAliases DestAlias, DestAliasFile
  End If
End Sub

Private Sub NewRouterButton_Click()
  If RouterAlias.Text = "" Then
    Beep
  Else
    RouterAlias.AddItem RouterAlias.Text + Chr$(9) + RouterMACAddress.Text
    SaveAliases RouterAlias, RouterAliasFile
  End If
End Sub

Private Sub RemoteRoute_Click(Value As Integer)
  If Value Then
        portlabel.visible = True
        PortPanel.visible = True
        NetworkPanel.visible = True
        RouterUnknown.visible = True
        RouterUnknown.Value = True
  End If
End Sub

Private Sub RouterAlias_Click()
'Note: The work has been moved to LostFocus because of a VB5    ***211 Begin
'       bug that clears the Text when ListIndex->-1!
'Dim d$, l$
'  l$ = RouterAlias.Text
'  d$ = NxtC(l$, Chr$(9))
'  RouterMACAddress.Text = l$
'  RouterAlias.Text = d$
  RouterAliasDummy.SetFocus
'                                                               ***211 End
End Sub

Private Sub RouterAlias_GotFocus()
  GoSelect RouterAlias
End Sub

Private Sub RouterAlias_KeyPress(KeyAscii As Integer)
  If KeyAscii = 13 Then
    KeyAscii = 0
    NewRouterButton_Click
  End If
End Sub

Private Sub RouterAlias_LostFocus()
' Note: This must be done here because of a VB5 bug!        ***211 Begin
Dim d$, l$
  l$ = RouterAlias.Text
  d$ = NxtC(l$, Chr$(9))
  RouterMACAddress.Text = l$
  RouterAlias.Text = d$
'                                                           ***211 End
End Sub

Private Sub RouterAliasDummy_Change()
' Note: This is required to be here because of a VB5 bug!   ***211 Begin
'
  RouterAlias.SetFocus                  '                   ***210 End

End Sub


Private Sub RouterMACAddress_GotFocus()
  GoSelect RouterMACAddress
End Sub

Private Sub RouterMACAddress_KeyPress(KeyAscii As Integer)
  If KeyAscii = 13 Then
    KeyAscii = 0
    NewRouterButton_Click
  End If
End Sub

Private Sub RouterUnknown_Click(Value As Integer)
Dim v
  v = (Value = 0)
  rmalabel.visible = v
  ralabel.visible = v
  NewRouterButton.visible = v
  RMpanel.visible = v
  RAPanel.visible = v
  If Not Value Then RouterMACAddress.SetFocus
End Sub

Private Sub SourceMACAddress_GotFocus()
  GoSelect SourceMACAddress
End Sub

Private Sub SourceNetwork_GotFocus()
  GoSelect SourceNetwork
End Sub

Private Sub SpecificDest_Click(Value As Integer)
  If Value Then DestMACAddress.SetFocus
End Sub

Private Sub Text1_Change()

End Sub


