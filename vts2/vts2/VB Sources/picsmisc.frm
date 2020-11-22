VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form PicsMiscForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "PICS Miscellaneous"
   ClientHeight    =   4905
   ClientLeft      =   1545
   ClientTop       =   2550
   ClientWidth     =   7605
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
   Icon            =   "PICSMISC.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4905
   ScaleWidth      =   7605
   Begin VB.TextBox BaudText 
      Appearance      =   0  'Flat
      Height          =   528
      Left            =   2280
      MultiLine       =   -1  'True
      ScrollBars      =   1  'Horizontal
      TabIndex        =   21
      Top             =   3720
      Width           =   1572
   End
   Begin Threed.SSFrame Frame3D5 
      Height          =   1575
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   7095
      _Version        =   65536
      _ExtentX        =   12515
      _ExtentY        =   2778
      _StockProps     =   14
      Caption         =   "Product Information"
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
      Begin VB.Label Label1 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Vendor Name:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   1
         Top             =   360
         Width           =   1572
      End
      Begin VB.Label ProdDescLab 
         Appearance      =   0  'Flat
         AutoSize        =   -1  'True
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   192
         Left            =   1800
         TabIndex        =   8
         Top             =   1080
         Width           =   5172
         WordWrap        =   -1  'True
      End
      Begin VB.Label ProdNrLab 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1800
         TabIndex        =   6
         Top             =   840
         Width           =   5172
      End
      Begin VB.Label ProdNameLab 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1800
         TabIndex        =   4
         Top             =   600
         Width           =   5172
      End
      Begin VB.Label VendNameLab 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1800
         TabIndex        =   2
         Top             =   360
         Width           =   5172
      End
      Begin VB.Label Label4 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Prod. Description:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   7
         Top             =   1080
         Width           =   1572
      End
      Begin VB.Label Label3 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Prod.Model Nr:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   5
         Top             =   840
         Width           =   1572
      End
      Begin VB.Label Label2 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Product Name:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   3
         Top             =   600
         Width           =   1572
      End
   End
   Begin Threed.SSFrame Frame3D4 
      Height          =   1575
      Left            =   120
      TabIndex        =   9
      Top             =   1800
      Width           =   2895
      _Version        =   65536
      _ExtentX        =   5106
      _ExtentY        =   2778
      _StockProps     =   14
      Caption         =   "BACnet Class/Groups"
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
      Begin VB.ComboBox FuncGroupsCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   13
         Top             =   1080
         Width           =   2652
      End
      Begin VB.Label Label9 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Conformance Class:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   10
         Top             =   480
         Width           =   1692
      End
      Begin VB.Label Label11 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Functional Groups:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   12
         Top             =   840
         Width           =   1692
      End
      Begin VB.Label ConfClassLab 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1920
         TabIndex        =   11
         Top             =   480
         Width           =   372
      End
   End
   Begin Threed.SSFrame Frame3D3 
      Height          =   1335
      Left            =   120
      TabIndex        =   19
      Top             =   3480
      Width           =   3855
      _Version        =   65536
      _ExtentX        =   6800
      _ExtentY        =   2355
      _StockProps     =   14
      Caption         =   "Data Link Layer Option"
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
      Begin VB.ComboBox DLLOptCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   22
         Top             =   960
         Width           =   3612
      End
      Begin VB.Label Label5 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Baud rate(s) supported:"
         ForeColor       =   &H80000008&
         Height          =   192
         Left            =   120
         TabIndex        =   20
         Top             =   360
         Width           =   1968
      End
   End
   Begin Threed.SSFrame Frame3D2 
      Height          =   855
      Left            =   4080
      TabIndex        =   23
      Top             =   3480
      Width           =   3375
      _Version        =   65536
      _ExtentX        =   5953
      _ExtentY        =   1508
      _StockProps     =   14
      Caption         =   "Special Functionality"
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
      Begin VB.ComboBox SpecFuncCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   24
         Top             =   360
         Width           =   3132
      End
   End
   Begin Threed.SSFrame Frame3D1 
      Height          =   1575
      Left            =   3120
      TabIndex        =   14
      Top             =   1800
      Width           =   4095
      _Version        =   65536
      _ExtentX        =   7223
      _ExtentY        =   2778
      _StockProps     =   14
      Caption         =   "Standard Services/Objects"
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
      Begin VB.ComboBox ApplServCombo 
         Appearance      =   0  'Flat
         BackColor       =   &H00FFFFFF&
         Height          =   288
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   16
         Top             =   480
         Width           =   3852
      End
      Begin VB.ComboBox ObjTypesCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   18
         Top             =   1080
         Width           =   3852
      End
      Begin VB.Label Label12 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Application Services:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   15
         Top             =   240
         Width           =   1812
      End
      Begin VB.Label Label13 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Object-Types:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   17
         Top             =   840
         Width           =   1812
      End
   End
End
Attribute VB_Name = "PicsMiscForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'form local variables ---

'link to selected Data Link Layer Options ComboBox. These variables contain
'the index of the linked Listbox-Position of the Listitem.
'eg. fPTT232Sel contains the position of the Point-to-Point-232 DLL option in
'the DLL combo box.
Dim fMSTPMasterSel, fMSTPSlaveSel, fPTP232Sel, fPTPModemSel, fPTPAutoSel As Integer

Private Sub ApplServCombo_Click()
  SelectFirst ApplServCombo     '                                 ***222
End Sub

Private Sub DLLOptCombo_Click()
Dim s As String, i As Integer
s = " "
Select Case DLLOptCombo.ListIndex
  Case fMSTPMasterSel
    For i = LBound(gDB.MSTPmasterBaudRates) To UBound(gDB.MSTPmasterBaudRates)
      If gDB.MSTPmasterBaudRates(i) = 0 Then Exit For
      s = s + Str$(gDB.MSTPmasterBaudRates(i)) + ", "
    Next i
  Case fMSTPSlaveSel
    For i = LBound(gDB.MSTPslaveBaudRates) To UBound(gDB.MSTPslaveBaudRates)
      If gDB.MSTPslaveBaudRates(i) = 0 Then Exit For
      s = s + Str$(gDB.MSTPslaveBaudRates(i)) + ", "
    Next i
  Case fPTP232Sel
     For i = LBound(gDB.PTP232BaudRates) To UBound(gDB.PTP232BaudRates)
       If gDB.PTP232BaudRates(i) = 0 Then Exit For
       s = s + Str$(gDB.PTP232BaudRates(i)) + ", "
    Next i
  Case fPTPModemSel
    For i = LBound(gDB.PTPmodemBaudRates) To UBound(gDB.PTPmodemBaudRates)
      If gDB.PTPmodemBaudRates(i) = 0 Then Exit For
      s = s + Str$(gDB.PTPmodemBaudRates(i)) + ", "
    Next i
  Case fPTPAutoSel
    s = Str$(gDB.PTPAutoBaud(1)) + " to " + Str$(gDB.PTPAutoBaud(2))
End Select
BaudText.Text = s

End Sub

Private Sub Form_Load()
Dim i, e As Integer, mask As Long, SelPos As Integer
Dim sname As String

sname = String(cSLen, " ")
PicsMiscForm.Caption = PicsMiscForm.Caption + "[" + gDefPicsTitle + "]"

'set form control values ---
VendNameLab.Caption = gDB.VendorName
ProdNameLab.Caption = gDB.ProductName
ProdNrLab.Caption = gDB.ProductModelNumber
ProdDescLab.Caption = gDB.ProductDescription
ConfClassLab.Caption = Str$(gDB.BACnetConformanceClass)

'fill FuncGroupsCombo ---
mask = 1
For i = 0 To cFGTableNr - 1
  If (gDB.BACnetFunctionalGroups And mask) Then
    e = LoadString(cFGTableId + i, sname, cSLen)
    FuncGroupsCombo.AddItem sname
  End If
  mask = mask * 2
Next i
'select first item in dropdown list
SelectFirst FuncGroupsCombo     '                                   ***222

'fill SpecFuncCombo ---
e = LoadString(cSpecFuncTableId + 0, sname, cSLen)
SpecFuncCombo.AddItem sname + Str$(gDB.MaxAPDUSize)
e = LoadString(cSpecFuncTableId + 1, sname, cSLen)
SpecFuncCombo.AddItem sname + Str$(Asc(gDB.SegmentedRequestWindow))
e = LoadString(cSpecFuncTableId + 2, sname, cSLen)
SpecFuncCombo.AddItem sname + Str$(Asc(gDB.SegmentedResponseWindow))
If gDB.RouterFunctions = rfSupported Then
  e = LoadString(cSpecFuncTableId + 3, sname, cSLen)
  SpecFuncCombo.AddItem sname
End If
'select the first item in dropdown list
SelectFirst SpecFuncCombo       '                                   ***222

'fill ApplServCombo ---
For i = 0 To cApplServTableNr - 1
  Select Case Asc(Mid$(gDB.BACnetStandardServices, i + 1, 1))
    Case ssNotSupported
    Case ssInitiate
      e = LoadString(cApplServTableId + i, sname, cSLen)
      ApplServCombo.AddItem sname + " Initiate"
    Case ssExecute
      e = LoadString(cApplServTableId + i, sname, cSLen)
      ApplServCombo.AddItem sname + " Execute"
    Case ssInitiate + ssExecute
      e = LoadString(cApplServTableId + i, sname, cSLen)
      ApplServCombo.AddItem sname + " Initiate Execute"
 End Select
Next i
'select the first item in dropdown list
SelectFirst ApplServCombo       '                                   ***222

'fill ObjTypesCombo
For i = 0 To cObjTypesTableNr - 1
  Select Case Asc(Mid$(gDB.BACnetStandardObjects, i + 1, 1))
    Case soNotSupported
    Case soSupported
      e = GetObjType(i, sname)
      ObjTypesCombo.AddItem sname
    Case soCreateable + soSupported
      e = GetObjType(i, sname)
      ObjTypesCombo.AddItem sname + " Createable"
    Case soDeletable + soSupported
      e = GetObjType(i, sname)
      ObjTypesCombo.AddItem sname + " Deleteable"
    Case soCreateable + soDeletable + soSupported
      e = GetObjType(i, sname)
      ObjTypesCombo.AddItem sname + " Createable Deleteable"
  End Select
Next i
'select the first item in dropdown list
SelectFirst ObjTypesCombo       '                                   ***222

'fill DLLOptCombo
mask = 1
SelPos = 0
For i = 0 To cDLLOptTableNr - 1
  If (gDB.DataLinkLayerOptions And mask) Then
    e = LoadString(cDLLOptTableId + i, sname, cSLen)
    DLLOptCombo.AddItem sname
    Select Case mask
      Case &H200
        fMSTPMasterSel = SelPos
      Case &H400
        fMSTPSlaveSel = SelPos
      Case &H800
        fPTP232Sel = SelPos
      Case &H1000
        fPTPModemSel = SelPos
      Case &H2000
        fPTPAutoSel = SelPos
    End Select
    SelPos = SelPos + 1
  End If
  mask = mask * 2
Next i
'select the first item in dropdown list
SelectFirst DLLOptCombo         '                                     ***222

End Sub

Private Sub FuncGroupsCombo_Click()
  SelectFirst FuncGroupsCombo         '                                     ***222
End Sub

Private Sub ObjTypesCombo_Click()
  SelectFirst ObjTypesCombo         '                                     ***222
End Sub

Private Sub SpecFuncCombo_Click()
  SelectFirst SpecFuncCombo         '                                     ***222
End Sub

