VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form PicsObjDBForm 
   Appearance      =   0  'Flat
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "PICS Object  Database"
   ClientHeight    =   3555
   ClientLeft      =   1590
   ClientTop       =   2085
   ClientWidth     =   5880
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
   Icon            =   "PICSDB.frx":0000
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   3555
   ScaleWidth      =   5880
   Begin VB.ListBox AnyPropList 
      Height          =   1230
      Left            =   240
      TabIndex        =   7
      Top             =   1560
      Width           =   2715
   End
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      Caption         =   "&Close"
      Height          =   372
      Left            =   4500
      TabIndex        =   8
      Top             =   3000
      Width           =   1212
   End
   Begin Threed.SSFrame Frame3D2 
      Height          =   3255
      Left            =   120
      TabIndex        =   9
      Top             =   120
      Width           =   3015
      _Version        =   65536
      _ExtentX        =   5318
      _ExtentY        =   5741
      _StockProps     =   14
      Caption         =   "BACnet Property"
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
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   10
         Top             =   600
         Width           =   2652
      End
      Begin VB.Label Label5 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Encoded Property:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   12
         Top             =   1080
         Width           =   1692
      End
      Begin VB.Label Label4 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Name:"
         ForeColor       =   &H00000000&
         Height          =   252
         Left            =   120
         TabIndex        =   11
         Top             =   360
         Width           =   2052
      End
   End
   Begin Threed.SSFrame Frame3D1 
      Height          =   2655
      Left            =   3360
      TabIndex        =   0
      Top             =   120
      Width           =   2415
      _Version        =   65536
      _ExtentX        =   4260
      _ExtentY        =   4683
      _StockProps     =   14
      Caption         =   "BACnet Objects"
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
      Begin VB.ComboBox ObjTypeCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   6
         Top             =   2160
         Width           =   2052
      End
      Begin VB.ComboBox ObjNameCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   4
         Top             =   1440
         Width           =   2052
      End
      Begin VB.ComboBox ObjIdCombo 
         Appearance      =   0  'Flat
         Height          =   288
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   2
         Top             =   720
         Width           =   2052
      End
      Begin VB.Label Label1 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Obj-Identifier:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   1
         Top             =   480
         Width           =   1092
      End
      Begin VB.Label Label2 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Obj-Name:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   3
         Top             =   1200
         Width           =   972
      End
      Begin VB.Label Label3 
         Appearance      =   0  'Flat
         BackColor       =   &H00C0C0C0&
         Caption         =   "Obj-Type:"
         ForeColor       =   &H80000008&
         Height          =   252
         Left            =   120
         TabIndex        =   5
         Top             =   1920
         Width           =   972
      End
   End
End
Attribute VB_Name = "PicsObjDBForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim fPropIdList() As Long   'Property IDs for function GetPropValue()

Private Sub AnyPropList_DblClick()
Dim i, c As Integer
On Error Resume Next
PicsAnyList!AnyList.Clear 'clear listbox
c = AnyPropList.ListCount - 1
For i = 0 To c
  PicsAnyList!AnyList.AddItem AnyPropList.List(i)
Next i
PicsAnyList.Show 0 'show modeless
PicsAnyList!AnyList.ListIndex = AnyPropList.ListIndex           '                 ***222
PicsAnyList!AnyList.TopIndex = PicsAnyList!AnyList.ListIndex
On Error GoTo 0

End Sub

Private Sub CloseButton_Click()
  Unload PicsObjDBForm
End Sub

Private Sub ComboLink(ByVal Index As Integer)
Dim i, e, ListPos As Integer
Dim PropId As Long
Dim Obj As generic_object, PropName As String * 80

If gDB.DataBase = 0 Then Exit Sub

CpyGenObj Obj, gDB.DataBase
Do Until Index <= 0
  e = GetNextGenObj(Obj)
  Index = Index - 1
Loop

PropNameCombo.Clear
ListPos = 0
ReDim fPropIdList(cPropMin To cPropMax + 1) As Long

For i = 0 To 63
  e = GetPropNameSupported(PropName, i, Obj.object_type, Obj.propFlags, PropId)
  If (PropId <> -1) And (e > 0) Then 'property supported
    PropNameCombo.AddItem PropName
    fPropIdList(ListPos) = PropId
    ListPos = ListPos + 1
  End If
Next i
fPropIdList(ListPos + 1) = -1 'mark end of list

SelectFirst PropNameCombo 'select first item in dropdown list               ***222
PropNameCombo_Click

End Sub

Private Sub Form_Load()
Dim Obj As generic_object
Dim i As Integer, TypeName As String * 80

PicsObjDBForm.Caption = PicsObjDBForm.Caption + "[" + gDefPicsTitle + "]"

CpyGenObj Obj, gDB.DataBase
Do
  ObjNameCombo.AddItem Obj.object_name
  ObjIdCombo.AddItem CStr(Obj.object_id And cObjInst)
  If GetEnumName(TypeName, Obj.object_type, cObject_Type, Obj.object_type) Then
    ObjTypeCombo.AddItem TypeName
  End If
Loop Until GetNextGenObj(Obj) = -1
  
SelectFirst ObjIdCombo 'select first item in dropdown list                    ***222
ObjIdCombo_Click

End Sub

Private Sub ObjIdCombo_Click()
  ObjNameCombo.ListIndex = ObjIdCombo.ListIndex 'select item in dropdown list    ***222
  ObjTypeCombo.ListIndex = ObjIdCombo.ListIndex 'select item in dropdown list    ***222
  ComboLink ObjIdCombo.ListIndex
End Sub

Private Sub ObjNameCombo_Click()
  ObjIdCombo.ListIndex = ObjNameCombo.ListIndex 'select item in dropdown list     ***222
  ObjTypeCombo.ListIndex = ObjNameCombo.ListIndex 'select item in dropdown list   ***222
  ComboLink ObjNameCombo.ListIndex
End Sub

Private Sub ObjTypeCombo_Click()
  ObjIdCombo.ListIndex = ObjTypeCombo.ListIndex 'select item in dropdown list     ***222
  ObjNameCombo.ListIndex = ObjTypeCombo.ListIndex 'select item in dropdown list   ***222
  ComboLink ObjTypeCombo.ListIndex
End Sub

Private Sub PropNameCombo_Click()
Dim Obj As Long, PropValue As String
Dim Msg As PVMessage
Dim StrList() As String
Dim i As Integer, u As Integer, l As Integer

If gDB.DataBase = 0 Then Exit Sub

Obj = FindGenObj(gDB.DataBase, ObjIdCombo.ListIndex)
If Obj = 0 Then Exit Sub

Msg.hw = PicsObjDBForm.hwnd
Msg.Action = cASN_1_ANY_PROP
Msg.TestNr = -1

Msg.PropId = fPropIdList(PropNameCombo.ListIndex)
Msg.Obj = Obj
Msg.TagNr = -1      'no tag
Msg.ArrayIndex = -1 'no ArrayIndex
Msg.priority = -1   'no Priority
Msg.BufferLen = cApduLen

PropValue = Space$(Msg.BufferLen)
AnyPropList.Clear
If GetPropValue(PropValue, Msg) = 0 Then 'format it
  logFormatApdu StrList(), PropValue, Msg.BufferLen
  u = UBound(StrList)
  l = LBound(StrList)
  For i = l To u
    AnyPropList.AddItem StrList(i)
  Next i
End If

End Sub

