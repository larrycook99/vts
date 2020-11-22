VERSION 5.00
Begin VB.Form sackForm 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Simple Ack/Error"
   ClientHeight    =   2040
   ClientLeft      =   1950
   ClientTop       =   2400
   ClientWidth     =   4410
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2040
   ScaleWidth      =   4410
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton CloseButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   375
      Left            =   3375
      TabIndex        =   11
      Top             =   480
      Width           =   960
   End
   Begin VB.CommandButton SendButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "Send"
      Default         =   -1  'True
      Height          =   375
      Left            =   3360
      TabIndex        =   10
      Top             =   45
      Width           =   960
   End
   Begin VB.ComboBox ErrorClass 
      Appearance      =   0  'Flat
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   315
      Left            =   1320
      TabIndex        =   6
      Top             =   1320
      Visible         =   0   'False
      Width           =   3015
   End
   Begin VB.ComboBox ErrorCode 
      Appearance      =   0  'Flat
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   315
      Left            =   1320
      TabIndex        =   5
      Top             =   1680
      Visible         =   0   'False
      Width           =   3030
   End
   Begin VB.ComboBox ServiceChoice 
      Appearance      =   0  'Flat
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   315
      Left            =   1320
      TabIndex        =   4
      Top             =   960
      Width           =   3030
   End
   Begin VB.TextBox invokeID 
      Appearance      =   0  'Flat
      BackColor       =   &H00000000&
      BorderStyle     =   0  'None
      ForeColor       =   &H0000FF00&
      Height          =   270
      Left            =   1320
      MaxLength       =   10
      TabIndex        =   2
      Text            =   "0"
      Top             =   570
      Width           =   600
   End
   Begin VB.OptionButton pdu 
      Caption         =   "Error PDU"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   1
      Left            =   75
      TabIndex        =   1
      Top             =   300
      Width           =   1335
   End
   Begin VB.OptionButton pdu 
      Caption         =   "Simple Ack PDU"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   0
      Left            =   60
      TabIndex        =   0
      Top             =   75
      Value           =   -1  'True
      Width           =   1815
   End
   Begin VB.Label classl 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Error Class"
      ForeColor       =   &H80000008&
      Height          =   195
      Left            =   120
      TabIndex        =   9
      Top             =   1320
      Visible         =   0   'False
      Width           =   1005
   End
   Begin VB.Label codel 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Error Code"
      ForeColor       =   &H80000008&
      Height          =   195
      Left            =   120
      TabIndex        =   8
      Top             =   1680
      Visible         =   0   'False
      Width           =   1005
   End
   Begin VB.Label sackl 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "Service Choice"
      ForeColor       =   &H80000008&
      Height          =   195
      Left            =   120
      TabIndex        =   7
      Top             =   960
      Width           =   1140
   End
   Begin VB.Label Label3 
      Alignment       =   1  'Right Justify
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BackStyle       =   0  'Transparent
      Caption         =   "invokeID"
      ForeColor       =   &H80000008&
      Height          =   195
      Left            =   480
      TabIndex        =   3
      Top             =   630
      Width           =   720
   End
End
Attribute VB_Name = "sackForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub CloseButton_Click()
  HideBiModal sackForm
End Sub

Private Sub Form_Activate()
'                                   moved entire sub from Form_Load
  invokeID.Text = "0"
'  classl.Move sackl.Left, sackl.Top                            ***209
'  ErrorClass.Move ServiceChoice.Left, ServiceChoice.Top        ***209
  ServiceChoice.Clear
  ServiceChoice.AddItem "acknowledgeAlarm*"
  ServiceChoice.AddItem "confirmedCOVNotification*"
  ServiceChoice.AddItem "confirmedEventNotification*"
  ServiceChoice.AddItem "getAlarmSummary"
  ServiceChoice.AddItem "getEnrollmentSummary"
  ServiceChoice.AddItem "subscribeCOV*"
  ServiceChoice.AddItem "atomicReadFile"
  ServiceChoice.AddItem "atomicWriteFile"
  ServiceChoice.AddItem "addListElement*"
  ServiceChoice.AddItem "removeListElement*"
  ServiceChoice.AddItem "createObject*"
  ServiceChoice.AddItem "deleteObject*"
  ServiceChoice.AddItem "readProperty"
  ServiceChoice.AddItem "readPropertyConditional"
  ServiceChoice.AddItem "readPropertyMultiple"
  ServiceChoice.AddItem "writeProperty*"
  ServiceChoice.AddItem "writePropertyMultiple*"
  ServiceChoice.AddItem "deviceCommunicationControl*"
  ServiceChoice.AddItem "confirmedPrivateTransfer"
  ServiceChoice.AddItem "confirmedTextMessage*"
  ServiceChoice.AddItem "reinitializeDevice*"
  ServiceChoice.AddItem "vtOpen"
  ServiceChoice.AddItem "vtClose*"
  ServiceChoice.AddItem "vtData"
  ServiceChoice.AddItem "authenticate"
  ServiceChoice.AddItem "requestKey"
  ServiceChoice.ListIndex = 0
  ErrorClass.Clear
  ErrorClass.AddItem "device"
  ErrorClass.AddItem "object"
  ErrorClass.AddItem "property"
  ErrorClass.AddItem "resources"
  ErrorClass.AddItem "security"
  ErrorClass.AddItem "services"
  ErrorClass.AddItem "vt"
  ErrorClass.ListIndex = 0
  ErrorCode.Clear
  ErrorCode.AddItem "other"
  ErrorCode.AddItem "authentication-failed"
  ErrorCode.AddItem "configuration-in-progress"
  ErrorCode.AddItem "device-busy"
  ErrorCode.AddItem "dynamic-creation-not-supported"
  ErrorCode.AddItem "file-access-denied"
  ErrorCode.AddItem "incompatible-security-levels"
  ErrorCode.AddItem "inconsistant-parameters"
  ErrorCode.AddItem "inconsistant-selection-criterion"
  ErrorCode.AddItem "invalid-data-type"
  ErrorCode.AddItem "invalid-file-access-method"
  ErrorCode.AddItem "invalid-file-start-position"
  ErrorCode.AddItem "invalid-operator-name"
  ErrorCode.AddItem "invalid-parameter-data-type"
  ErrorCode.AddItem "invalid-time-stamp"
  ErrorCode.AddItem "key-generation-error"
  ErrorCode.AddItem "missing-required-parameter"
  ErrorCode.AddItem "no-objects-of-specified-type"
  ErrorCode.AddItem "no-space-for-object"
  ErrorCode.AddItem "no-space-to-add-list-element"
  ErrorCode.AddItem "no-space-to-write-property"
  ErrorCode.AddItem "no-vt-sessions-available"
  ErrorCode.AddItem "object-deletion-not-permitted"
  ErrorCode.AddItem "object-identified-already-exists"
  ErrorCode.AddItem "operational-problem"
  ErrorCode.AddItem "password-failure"
  ErrorCode.AddItem "property-is-not-a-list"
  ErrorCode.AddItem "read-access-denied"
  ErrorCode.AddItem "security-not-supported"
  ErrorCode.AddItem "service-request-denied"
  ErrorCode.AddItem "timeout"
  ErrorCode.AddItem "unknown-object"
  ErrorCode.AddItem "unknown-property"
  ErrorCode.AddItem "spare"
  ErrorCode.AddItem "unknown-vt-class"
  ErrorCode.AddItem "unknown-vt-session"
  ErrorCode.AddItem "unsupported-object-type"
  ErrorCode.AddItem "value-out-of-range"
  ErrorCode.AddItem "vt-session-already-closed"
  ErrorCode.AddItem "vt-session-termination-failure"
  ErrorCode.AddItem "write-access-denied"
  ErrorCode.AddItem "character-set-not-supported"
  ErrorCode.AddItem "invalid-array-index"
  ErrorCode.ListIndex = 0
End Sub

Private Sub PDU_Click(Index As Integer)
Dim ack As Boolean
  ack = (Index = 0)
'  ServiceChoice.visible = ack                          '***209
'  sackl.visible = ack                                  '***209
  ErrorClass.visible = Not ack
  ErrorCode.visible = Not ack
  classl.visible = Not ack
  codel.visible = Not ack
End Sub


Private Sub SendButton_Click()
Dim alen, apdu As String * 128, i, id As Byte
  If invokeID.Text = "" Then
    Beep
    MsgBox "You must specify an invoke ID!", 48, "No Invoke ID Specified"
    invokeID.SetFocus
    Exit Sub
  End If
  i = Val(invokeID.Text)
  If i < 0 Or i > 255 Then
    Beep
    MsgBox "You must specify an invoke ID between 0 and 255!", 48, "Invalid Invoke ID"
    invokeID.SetFocus
    Exit Sub
  End If
  id = i
  If pdu(0).Value Then                          'simple ack
     alen = eSimpleAckPDU(apdu, id, ServiceChoice.ListIndex)
  Else                                          'error code
     alen = eErrorPDU(apdu, id, ErrorClass.ListIndex, ErrorCode.ListIndex, ServiceChoice.ListIndex) '***209
  End If
  SendNPDU Left$(apdu, alen), False
End Sub


