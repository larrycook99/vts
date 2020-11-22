VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form PicsPropInfoForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "PICS PropInfo"
   ClientHeight    =   4530
   ClientLeft      =   2835
   ClientTop       =   2175
   ClientWidth     =   4785
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
   Icon            =   "PICSPROP.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4530
   ScaleWidth      =   4785
   Begin Threed.SSFrame Frame3D2 
      Height          =   855
      Left            =   120
      TabIndex        =   1
      Top             =   120
      Width           =   3375
      _Version        =   65536
      _ExtentX        =   5953
      _ExtentY        =   1508
      _StockProps     =   14
      Caption         =   "ObjectTypes supported"
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
      Begin VB.ComboBox PropObjCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   240
         Style           =   2  'Dropdown List
         TabIndex        =   2
         Top             =   360
         Width           =   2892
      End
   End
   Begin Threed.SSFrame Frame3D1 
      Height          =   3255
      Left            =   120
      TabIndex        =   0
      Top             =   1080
      Width           =   4575
      _Version        =   65536
      _ExtentX        =   8070
      _ExtentY        =   5741
      _StockProps     =   14
      Caption         =   "Properties"
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
      Begin VB.ComboBox PropNameCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   1800
         Style           =   2  'Dropdown List
         TabIndex        =   3
         Top             =   360
         Width           =   2532
      End
      Begin VB.ComboBox PropEnumCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   1800
         Style           =   2  'Dropdown List
         TabIndex        =   4
         Top             =   2640
         Width           =   2652
      End
      Begin VB.Label PropIdLab 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1800
         TabIndex        =   16
         Top             =   840
         Width           =   1092
      End
      Begin VB.Label Label7 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "PropID:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   840
         TabIndex        =   15
         Top             =   840
         Width           =   732
      End
      Begin VB.Label PetLab 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1800
         TabIndex        =   5
         Top             =   2280
         Width           =   1092
      End
      Begin VB.Label PFlagsLab 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1800
         TabIndex        =   14
         Top             =   1920
         Width           =   1092
      End
      Begin VB.Label PGroupLab 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1800
         TabIndex        =   13
         Top             =   1560
         Width           =   1092
      End
      Begin VB.Label PTypeLab 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1800
         TabIndex        =   12
         Top             =   1200
         Width           =   1092
      End
      Begin VB.Label Label6 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Enumeration:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   360
         TabIndex        =   11
         Top             =   2640
         Width           =   1212
      End
      Begin VB.Label Label5 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Property name:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   240
         TabIndex        =   10
         Top             =   480
         Width           =   1332
      End
      Begin VB.Label Label4 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "ptype:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   960
         TabIndex        =   9
         Top             =   1200
         Width           =   612
      End
      Begin VB.Label Label3 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "pet:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   1200
         TabIndex        =   8
         Top             =   2280
         Width           =   372
      End
      Begin VB.Label Label2 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "pflags"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   960
         TabIndex        =   7
         Top             =   1920
         Width           =   612
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "pgroup:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   840
         TabIndex        =   6
         Top             =   1560
         Width           =   732
      End
   End
End
Attribute VB_Name = "PicsPropInfoForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'This combobox displays the BACnet object types supported. If you select
'a object type, then the enumerated value of the object type can be
'retrieved as follows: fPropObjList(PropObjCombo.ListIndex)
Dim fPropObjList(0 To 17) As Integer

Private Sub Form_Load()
Dim i, e, ListPos As Integer
Dim l As Long                                           '***220
Dim ObjTypeName As String
ObjTypeName = String(40, " ")

PicsPropInfoForm.Caption = PicsPropInfoForm.Caption + "[" + gDefPicsTitle + "]"

'read all BACnetStandardObjects supported
ListPos = 0
For i = 0 To cObjTypesTableNr - 1
  If Asc(Mid$(gDB.BACnetStandardObjects, i + 1, 1)) And soSupported Then
    e = GetObjType(i, ObjTypeName)
    PropObjCombo.AddItem ObjTypeName
    fPropObjList(ListPos) = i
    ListPos = ListPos + 1
  End If
Next i
'select first item in list
SelectFirst PropObjCombo                              '***222
PropObjCombo_Click
End Sub

Private Function GetStrIndex(ByVal s As String) As Integer
  GetStrIndex = CInt(Val(s))
End Function

Private Sub PropEnumCombo_Click()
  SelectFirst PropEnumCombo         '                                     ***222
End Sub

Private Sub PropNameCombo_Click()
Dim i, j, enumc As Integer, sflag As String
Dim l As Long                                                  '***220
Dim PropId, PropCount As Long, pname As String * 40
Dim ptype As Integer, pgroup As Integer, pflags As Integer, pet As Integer

If PropNameCombo.ListCount = 0 Then Exit Sub
PropEnumCombo.Clear

j = fPropObjList(PropObjCombo.ListIndex)
i = PropNameCombo.ListIndex
PropId = VTSAPIgetpropinfo(j, i, pname, ptype, pgroup, pflags, pet)
PropIdLab.Caption = CStr(PropId)
PTypeLab.Caption = CStr(ptype)
PGroupLab.Caption = CStr(pgroup)
sflag = "O"
If pflags And pfR Then sflag = sflag + "|R"
If pflags And pfW Then sflag = sflag + "|W"
PFlagsLab.Caption = Hex$(pflags) + ": " + sflag
PetLab.Caption = CStr(pet)

enumc = VTSAPIgetenumtable(pet, 0, 0, 0, pname)
If enumc = 0 Then Exit Sub
PropEnumCombo.Clear
For j = 0 To enumc - 1
  i = VTSAPIgetenumtable(pet, j, 0, 0, pname)
  PropEnumCombo.AddItem pname
Next j
'select first item in list
SelectFirst PropEnumCombo                               '***222
End Sub

Private Sub PropObjCombo_Click()
Dim i, j, e As Integer
Dim PropId, PropCount As Long, pname As String * 40
Dim l As Long                                             '***220
Dim ptype As Integer, pgroup As Integer, pflags As Integer, pet As Integer

If PropObjCombo.ListCount = 0 Then Exit Sub
PropNameCombo.Clear

'read all BACnetProperties supported
j = fPropObjList(PropObjCombo.ListIndex)
PropCount = VTSAPIgetpropinfo(j, &HFFFF, "", 0, 0, 0, 0)
For i = 0 To PropCount - 1 'all properties of selected object
  PropId = VTSAPIgetpropinfo(j, i, pname, ptype, pgroup, pflags, pet)
  If (PropId = &HFFFF) Then e = PicsError(0, " ")
  PropNameCombo.AddItem pname
Next i
'select first item in list
SelectFirst PropNameCombo                               '***222
PropNameCombo_Click

End Sub

