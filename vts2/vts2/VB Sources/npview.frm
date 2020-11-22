VERSION 5.00
Begin VB.Form npview 
   Caption         =   "Form1"
   ClientHeight    =   8820
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   8460
   LinkTopic       =   "Form1"
   ScaleHeight     =   8820
   ScaleWidth      =   8460
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "&Close"
      Height          =   375
      Left            =   3600
      TabIndex        =   1
      Top             =   8280
      Width           =   1215
   End
   Begin VB.ListBox npout 
      Height          =   8055
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   8415
   End
End
Attribute VB_Name = "npview"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub shownp(np As NALSpacket)

npout.Clear
npout.AddItem ("nalsNext = " + nalsNext)
npout.AddItem ("nalsExpectReply = " + nalsExpectReply)
npout.AddItem ("nalsRequestorisserver = " + nalsRequestorisserver)
npout.AddItem ("nalsBroadcast = " + nalsBroadcast)
npout.AddItem ("nalsRxType = " + nalsRxType)
npout.AddItem ("nalsInvokeID = " + nalsInvokeID)
npout.AddItem ("nalsRxFlag = " + nalsRxFlag)
npout.AddItem ("nalsPri = " + nalsPri)
npout.AddItem ("nalsDnet = " + nalsDnet)
npout.AddItem ("nalsDlen = " + nalsDlen)
npout.AddItem ("nalsDadr = " + nalsDadr)
npout.AddItem ("nalsRlen = " + nalsRlen)
npout.AddItem ("nalsRadr = " + nalsRadr)
npout.AddItem ("nalsSnet = " + nalsSnet)
npout.AddItem ("nalsSlen = " + nalsSlen)
npout.AddItem ("nalsSadr = " + nalsSadr)
npout.AddItem ("nalsLport = " + nalsLport)
npout.AddItem ("nalsAlen = " + nalsAlen)
npout.AddItem ("nalsAPDU = " + nalsAPDU)

End Sub

Private Sub Command1_Click()
    HideBiModal npview
End Sub
