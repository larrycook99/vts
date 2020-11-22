Attribute VB_Name = "PICSTEST"
Option Explicit

Global Const cSLen = 256         'common length of strings
Global Const cApduLen = 4096     'max length of APDU data

'basic BACnet datatypes
Global Const bacNull = &H1
Global Const bacBoolean = &H2
Global Const bacUnsigned = &H4
Global Const bacSigned = &H8
Global Const bacReal = &H10
Global Const bacDouble = &H20
Global Const bacOctet = &H40
Global Const bacChar = &H80
Global Const bacBit = &H100
Global Const bacEnum = &H200
Global Const bacDate = &H400
Global Const bacTime = &H800
Global Const bacObj = &H1000

'Parse Types
Global Const ob_id = 0                                'an object identifier
Global Const et = 1                                   'an enumeration table
Global Const flt = 2                                  'float
Global Const bits = 3                                 'octet of T or F flags
Global Const uw = 4                                   'unsigned word
Global Const ud = 5                                   'unsigned dword
Global Const sw = 6                                   'signed word
Global Const u16 = 32                                 '1..16
Global Const u127 = 7                                 '1..127
Global Const s10 = 8                                  'char [10]
Global Const s32 = 9                                  'char [32]
Global Const s64 = 10                                 'char [64]
Global Const s132 = 11                                'char [132]
Global Const propref = 12                             'obj property reference
Global Const dt = 13                                  'date/time
Global Const Calist = 14                              'list of calendarentry
Global Const act = 15                                 'action array
Global Const actext = 16                              'action_text array
Global Const pss = 17                                 'protocol services supported bitstring
Global Const pos = 18                                 'protocol objects supported bitstring
Global Const vtcl = 19                                'vt classes
Global Const evparm = 20                              'event parameter
Global Const skeys = 21                               'session keys
Global Const tsrecip = 22                             'time synch recipients
Global Const dabind = 23                              'device address bindings
Global Const recip = 24                               'recipient
Global Const paf = 25                                 'priority array flt
Global Const statext = 26                             'state text array
Global Const pab = 27                                 'priority array bpv
Global Const pau = 28                                 'priority array uw
Global Const reciplist = 29                           'list of BACnetDestination
Global Const xsched = 30                              'exception schedule: array[] of specialevent
Global Const wsched = 31                              'weekly schedule: array[7] of list of timevalue
Global Const setref = 33                              'setpoint reference
Global Const raslist = 34                             'list of readaccessspecs
Global Const stavals = 35                             'list of unsigned
Global Const dtrange = 36                             'range of dates
Global Const lopref = 37                              'list of object prop refs
Global Const ebool = 38                               'boolean enumeration
Global Const enull = 254                              'null enumeration
Global Const none = 255                               'no parsing required, runtime generated value
Global Const ptDate = 200                             'parsetype BACnetDate
Global Const ptTime = 201                             'parsetype BACnetTime
Global Const lobj = 202                               'array of objects
Global Const vtse = 203                               'list of active  vt sessions
Global Const uwarr = 204                              'array of uw

Global Const epsilon = 0.1                            'epsilon band

'active (output) status list identifiers
Global Const cTEST = 1
Global Const cCONTENT = 2
Global Const cRESULT = 3

'constants for consistency checks
Global Const consRProp = 1      'check required properties
Global Const consObjTypes = 2   'check object types supported
Global Const consGroupProp = 3  'check group properties
Global Const consConfClass = 4  'check conformance class
Global Const consFuncGroup = 5  'check functional groups
Global Const consApplServ = 6   'check application services (device object)
Global Const consObjectList = 7 'check object list (device object)


'type of test
Global Const cConsTest = 1        'Consistency test
Global Const cRdPropTest = 2      'Read property test
Global Const cWrtPropTest = 3     'Write property test
Global Const cListElementTest = 4 'List Element test
Global Const cEndTest = 99        'no more tests

Global Const cMaxResult = 50      'result list size

Type ListLink          'link to the TestList
  Entry As String      'stuffed data
End Type

Type PicsState
  TestNr As Long                 'Type of Test
  SubTestNr As Long              'Subsequent Test Number

  info1 As Long                  'string id (description1)
  info2 As Long                  'string id (description2)

  ActiveList As Integer          'active output list (status list identifiers)
  EndTest As Integer             'end of test flag
  StopAfterErr As Integer        'stop after max. errors occured
  LogFileNr As Integer           'file handle

  'entry points for cWrtPropTest, cRdPropTest:
  ActiveObject As Long           'active BACnet object to be tested (pointer)
  ActiveProperty As Long         'active Property to be tested (index)
  'entry points for cConsTest:
  consTestNr As Long             'present consistency test

End Type


'progress form/window
Type ProgressType
  Title As String       'informational text
  Description As String 'description of present value
  Min As Integer        'min range val
  max As Integer        'max range val
  pres As Integer       'present range val
End Type


'------------------------------
'Global variables
'------------------------------


Global gState As PicsState          'current state of testing
Global gTestLL() As ListLink        'link to the TestList list box

Global gPicsSend As Integer         'indicates that my apdu will be sent
                                    'true= my apdu, false= other
Global gPicsInvokeId As Integer     'InvokeId of my apdu
                                    '    -1, reply apdu has been received
                                    '    0..255, request apdu has been sent

Global gRx As String                'buffer to contain apdu received
Global gRxLen As Integer            'to contain length of apdu received
Global gPicsFilter As Integer       'filter trace window

Global gPicsTimeOut As Integer       'no apdu received within time limit
Global gPicsInterval As Integer      'timeout limit
Global gStopAfterErr As Integer      'stop after max. errors occured

' Test Values for Write Property Test
Global gU(0 To 2) As Integer
Global gF(0 To 2) As Single


'This function is used to remove all characters in a string, which are
'following the '\0' termination character.
Sub AdjustString(s As String)
Dim i As Integer
i = InStr(s, Chr$(0))
If i > 1 Then
  s = Mid$(s, 1, i - 1)
ElseIf i = 1 Then
  s = ""
End If

End Sub

'This Function is used to compare two Apdu (Read/Write property test).
'The APDUs should not be segmented.
'in:        rx          received Apdu
'           rxl         length of rx
'           tx          Apdu sent
'           txl         length of tx
'out:       errmsg      to contain error diagnosis
'returns:   eNOERROR if equal, else errorcode
Function CompareApdu(rx As String, ByVal rxl As Integer, tx As String, ByVal txl As Integer, ErrMsg As String) As Integer
Dim pval1 As String, plen1 As Integer
Dim pval2 As String, plen2 As Integer
Dim e As Integer

CompareApdu = eAPDUFAILURE

pval1 = "": plen1 = 0
pval2 = "": plen2 = 0

e = ExtractPropVal(pval1, plen1, rx, rxl)
e = ExtractPropVal(pval2, plen2, tx, txl)

If StrComp(pval1, pval2, 0) <> 0 Then
  e = LoadString(7795, ErrMsg, cSLen): AdjustString ErrMsg
  Exit Function
End If

CompareApdu = eNOERROR

End Function

'This function is used to check group properties. If a property belongs to a group, then any
'property of that group should be present, or no property of that group should be present.
'in:        object_type         specifies the BACnetObjectType
'           propFlags           bitstream of properties supported
'out        slist               array of invalid group properties
'returns:   true
Function consCheckProp(object_type As Integer, propFlags As String, slist() As String) As Integer
Dim c As Integer, i As Integer, id As Long
Dim u As Integer, l As Integer, j As Integer
Dim pname As String
Dim pflags As Integer, pgroup As Integer
Dim GroupUsed As Integer, GroupSupported As Integer, PropSupported As Integer
ReDim GroupNr(0) As Integer 'used group numbers

consCheckProp = True

pname = Space$(cSLen) 'property name
GroupNr(0) = -1
ReDim slist(0) As String

c = VTSAPIgetpropinfo(object_type, &HFFFF, pname, 0, 0, 0, 0) - 1
For i = 0 To c 'for all properties
  id = VTSAPIgetpropinfo(object_type, i, pname, 0, pgroup, pflags, 0)
  If (pflags And pfWithGroup) Then 'group membership
    GroupUsed = False
    l = LBound(GroupNr)
    u = UBound(GroupNr)
    For j = l To u
      If GroupNr(j) = pgroup Then
        GroupUsed = True
        Exit For
      End If
    Next j
    If Not GroupUsed Then
      u = u + 1
      ReDim Preserve GroupNr(u)
      GroupNr(u) = pgroup
    End If
  End If
Next i

l = LBound(GroupNr)
u = UBound(GroupNr)

For i = l To u 'for all group numbers
  GroupSupported = 1 'not true, not false
  For j = 0 To c 'for all properties
    pname = Space$(cSLen)
    id = VTSAPIgetpropinfo(object_type, j, pname, 0, pgroup, pflags, 0)
    If (pflags And pfWithGroup) And (pgroup = GroupNr(i)) Then
       PropSupported = (Asc(Mid$(propFlags, j + 1, 1)) And 1) = 1
       If GroupSupported = 1 Then
         GroupSupported = PropSupported
       ElseIf PropSupported <> GroupSupported Then 'group incomplete
         AdjustString pname
         slist(UBound(slist)) = pname & "(" & CStr(id) + ")"
         ReDim Preserve slist(UBound(slist) + 1)
       End If
    End If
  Next j
Next i

slist(UBound(slist)) = ""

End Function

'The Protocol_Services_Supported property of the Device object in the Database Section
'of the Text PICS shall indicate support for each application service for which the specified
'conformance class/functional group requires support for execution of the service. (e,j)
Sub consDoApplServ()
Dim i As Integer, e As Integer
Dim s As String, Result As String
Dim resApplServ As String
Dim Service As Integer

    resApplServ = Space(cApplServTableNr)
    e = LoadString(3030, s, cSLen): AdjustString s 'Application Services Supported Test
    scrAddItem s, PicsTestForm!ResultList
    logWriteStr s, gState.LogFileNr
    e = DevApplServCheck(gDB.BACnetStandardServices, gDB.DataBase, resApplServ)
    If (e = 0) Then
      For i = 1 To cApplServTableNr
        Service = Asc(Mid$(resApplServ, i, 1))
        If (Service <> 0) Then 'this service is missing
          If (Service And 4) = 4 Then
            'missing in device object protocol_services_supported
            e = LoadString(3077, Result, cSLen): AdjustString Result
          End If
          If (Service And 8) = 8 Then
            'missing in PICS section: application services supported
            e = LoadString(3078, Result, cSLen): AdjustString Result
          End If
          e = LoadString(4728 + i - 1, s, cSLen): AdjustString s 'application service
          ReplaceText 1, Result, s
          ReplaceText 2, Result, "" 'never Initiate
          If (Service And 2) = 2 Then
            e = LoadString(3068, s, cSLen): AdjustString s 'Execute
          Else
            s = ""
          End If
          ReplaceText 3, Result, s
          scrAddItem Result, PicsTestForm!ResultList
          logWriteStr Result, gState.LogFileNr
        End If
      Next i
    End If
    e = LoadString(3037, Result, cSLen): AdjustString Result 'Application Services supported checked.
    scrAddItem Result, PicsTestForm!ResultList
    logWriteStr Result, gState.LogFileNr

End Sub

'A minimum of one instance of each object type required by the specified conformance
'class shall be included in the Object Database section of the Text PICS. (b)
'All application services required by the specified conformance class shall be indicated
'as supported in the BACnet Standard Application Services Supported section of the Text
'PICS with Initiate and Execute indicated as required by the conformance class. (d)
Sub consDoConfClass()
Dim i As Integer, e As Integer
Dim s As String, Result As String
Dim resApplServ As String
Dim resObj As String
Dim objtype As Integer
Dim Service As Integer, eol As Integer
Dim PropId As Long
Dim cc As Integer
Dim FuncGroup As Long

    resApplServ = Space(cApplServTableNr)
    resObj = Space(64 * 3)

    For cc = 1 To gDB.BACnetConformanceClass 'all conformance classes

      e = LoadString(3045, Result, cSLen): AdjustString s 'Test of Conformance Class: ~1
      ReplaceText 1, Result, CStr(cc)
      scrAddItem Result, PicsTestForm!ResultList
      logWriteStr Result, gState.LogFileNr

      e = CheckConfClass(cc, gDB.BACnetStandardServices, resApplServ, gDB.DataBase, resObj, eol)
      If (e = 0) Then 'conformance class not supported
        For i = 1 To cApplServTableNr
          Service = Asc(Mid$(resApplServ, i, 1))
          If (Service <> 0) Then 'this service is missing
            e = LoadString(3040, Result, cSLen): AdjustString Result
            e = LoadString(4728 + i - 1, s, cSLen): AdjustString s 'application service
            ReplaceText 1, Result, s
            If (Service And 1) = 1 Then
              e = LoadString(3067, s, cSLen): AdjustString s 'Initiate
            Else
              s = ""
            End If
            ReplaceText 2, Result, s
            If (Service And 2) = 2 Then
              e = LoadString(3068, s, cSLen): AdjustString s 'Execute
            Else
              s = ""
            End If
            ReplaceText 3, Result, s
            scrAddItem Result, PicsTestForm!ResultList
            logWriteStr Result, gState.LogFileNr
          End If
        Next i
        For i = 1 To (eol + 1) * 3 Step 3
          objtype = CMyInt(Mid$(resObj, i, 2))
          PropId = Asc(Mid$(resObj, i + 2, 1))
          If objtype = -1 Then
            Exit For
          Else
            If (PropId <> &HFF) Then 'Property required
              e = LoadString(3044, Result, cSLen): AdjustString Result
              s = Space$(cSLen)
              e = GetPropName(PropId, s): AdjustString s
              ReplaceText 1, Result, s 'property name
              s = Space(cSLen)
              e = GetObjType(objtype, s): AdjustString s
              ReplaceText 2, Result, s 'object type
            Else
              e = LoadString(3041, Result, cSLen): AdjustString Result
              s = Space(cSLen)
              e = GetObjType(objtype, s): AdjustString s
              ReplaceText 1, Result, s 'object type
            End If
            scrAddItem Result, PicsTestForm!ResultList
            logWriteStr Result, gState.LogFileNr
          End If
        Next i
      End If

      If cc = 6 Then
        'fgPCWS, fgCOVEventInitiation, fgCOVEventResponse, fgClock, fgFiles
        FuncGroup = &H2 Or &H4 Or &H8 Or &H40 Or &H100
        consDoFuncGroup FuncGroup 'check required functional groups
      End If

    Next cc 'next conformance class


    e = LoadString(3035, Result, cSLen): AdjustString Result
    scrAddItem Result, PicsTestForm!ResultList
    logWriteStr " ", gState.LogFileNr
    logWriteStr Result, gState.LogFileNr

End Sub

'A minimum of one instance of each object type required by the specified functional
'group shall be included in the Object Database section of the Text PICS. (g)
'All application services required by the specified functional group shall be indicated
'as supported in the BACnet Standard Application Services Supported section of the Text
'PICS with Initiate and Execute indicated as required by the functional group. (i)
Sub consDoFuncGroup(FuncGroup As Long)

Dim i As Integer, e As Integer
Dim s As String, Result As String
Dim resApplServ As String
Dim resObj As String
Dim objtype As Integer
Dim PropId As Long
Dim FG As Integer
Dim StringTab As Integer
Dim Service As Integer, eol As Integer
Dim pObj As Long                                                    '       ***TEMP
Dim Obj As generic_object                                           '       ***TEMP

    resApplServ = Space(cApplServTableNr)
    resObj = Space(64 * 3)
    FG = 1: StringTab = cFGTableId
    Do While FG <= &H1000 'for all fg
      If (FuncGroup And FG) = FG Then
        e = LoadString(StringTab, s, cSLen): AdjustString s 'functional group
        e = LoadString(3043, Result, cSLen): AdjustString s 'Test of Functional Group: ~1
        ReplaceText 1, Result, s
        scrAddItem Result, PicsTestForm!ResultList
        logWriteStr Result, gState.LogFileNr

        pObj = gDB.DataBase                                         '       ***TEMP
        Do While pObj <> 0
          CpyGenObj Obj, pObj        'get the generic_object
          pObj = Obj.next
        Loop                                                       '       ***TEMP
        e = CheckFunctionalGroup(FG, gDB.BACnetStandardServices, resApplServ, gDB.DataBase, resObj, eol)
        If (e = 0) Then 'functional group not supported
          For i = 1 To cApplServTableNr
            Service = Asc(Mid$(resApplServ, i, 1))
            If (Service <> 0) Then 'this service is missing
              e = LoadString(3040, Result, cSLen): AdjustString Result '      Application Service: ~1 is missing.
              e = LoadString(4728 + i - 1, s, cSLen): AdjustString s 'application service
              ReplaceText 1, Result, s
              If (Service And 1) = 1 Then
                e = LoadString(3067, s, cSLen): AdjustString s 'Initiate
              Else
                s = ""
              End If
              ReplaceText 2, Result, s
              If (Service And 2) = 2 Then
                e = LoadString(3068, s, cSLen): AdjustString s 'Execute
              Else
                s = ""
              End If
              ReplaceText 3, Result, s
              scrAddItem Result, PicsTestForm!ResultList
              logWriteStr Result, gState.LogFileNr
            End If
          Next i
          For i = 1 To (eol + 1) * 3 Step 3
            objtype = CMyInt(Mid$(resObj, i, 2))
            PropId = Asc(Mid$(resObj, i + 2, 1))
            If objtype = -1 Then
              Exit For
            Else
              If (PropId <> &HFF) Then 'Property required
                e = LoadString(3044, Result, cSLen): AdjustString Result
                s = Space$(cSLen)
                e = GetPropName(PropId, s): AdjustString s
                ReplaceText 1, Result, s 'property name
                s = Space(cSLen)
                e = GetObjType(objtype, s): AdjustString s
                ReplaceText 2, Result, s 'object type
              Else
                e = LoadString(3041, Result, cSLen): AdjustString Result
                s = Space(cSLen)
                e = GetObjType(objtype, s): AdjustString s
                ReplaceText 1, Result, s 'object type
              End If
              scrAddItem Result, PicsTestForm!ResultList
              logWriteStr Result, gState.LogFileNr
            End If
          Next i
        End If
      End If 'fg not supported
      FG = FG * 2 'next fg
      StringTab = StringTab + 1
    Loop
    e = LoadString(3036, Result, cSLen): AdjustString Result
    scrAddItem Result, PicsTestForm!ResultList
    logWriteStr Result, gState.LogFileNr

End Sub

'If any of the properties supported for an object require the conditional presence of
'other properties, their presence shall be verified. (n)
Sub consDoGroupProp()
Dim pObj As Long
Dim Obj As generic_object
Dim i As Integer, e As Integer
Dim s As String, Result As String
ReDim PropList(0) As String

e = LoadString(3026, Result, cSLen): AdjustString Result
scrAddItem Result, PicsTestForm!ResultList
logWriteStr Result, gState.LogFileNr
pObj = gDB.DataBase
Do While (pObj <> 0)
  CpyGenObj Obj, pObj        'get the generic_object
  pObj = FindGenObj(pObj, 1) 'find the next object
  e = consCheckProp(Obj.object_type, Obj.propFlags, PropList()) 'find group properties
  If (e And (UBound(PropList) > 0)) Then
    For i = LBound(PropList) To UBound(PropList) - 1
      'Invalid group property: ~1, in object: ~2, inst: ~3
      e = LoadString(3027, Result, cSLen): AdjustString Result
      ReplaceText 1, Result, PropList(i)
      AdjustString Obj.object_name: s = RTrim$(Obj.object_name)
      ReplaceText 2, Result, s
      ReplaceText 3, Result, CStr(Obj.object_id And cObjInst)
      scrAddItem Result, PicsTestForm!ResultList
      logWriteStr Result, gState.LogFileNr
    Next i
  End If
Loop 'all objects
e = LoadString(3034, Result, cSLen): AdjustString Result
scrAddItem Result, PicsTestForm!ResultList
logWriteStr Result, gState.LogFileNr

End Sub

'There shall be a one-to-one correspondence between the objects listed in the Object_List
'property of the Device object and the objects included in the Object Database section
'of the Text PICS. (m)
Sub consDoObjectList()

Dim i As Integer, e As Integer
Dim s As String, Result As String
Dim objtype As Integer, ObjInst As Long
Dim ObjId As Long
Dim pObjIdRoot As Long, pid As Long, pdb As Long

    e = LoadString(3031, s, cSLen): AdjustString s 'Object List Test
    scrAddItem s, PicsTestForm!ResultList
    logWriteStr s, gState.LogFileNr
    pObjIdRoot = GetObjIdRoot(gDB.DataBase) 'root in property object-list of device object
    pid = pObjIdRoot
    Do While pid <> 0
      e = pIDinList(gDB.DataBase, pid, ObjId)
      If (e = 0) Then 'pid not in pdb list
        SplitObjectId ObjId, objtype, ObjInst
        s = Space$(cSLen)
        e = GetObjType(objtype, s): AdjustString s
        e = LoadString(3047, Result, cSLen): AdjustString Result '      Not in PICS object list: (~1,~2)
        ReplaceText 1, Result, s
        ReplaceText 2, Result, CStr(ObjInst)
        scrAddItem Result, PicsTestForm!ResultList
        logWriteStr Result, gState.LogFileNr
      End If
    Loop

    pdb = gDB.DataBase
    Do While pdb <> 0
      e = pDBinList(pObjIdRoot, pdb, ObjId)
      If (e = 0) Then 'pdb not in pid list
        SplitObjectId ObjId, objtype, ObjInst
        s = Space$(cSLen)
        e = GetObjType(objtype, s): AdjustString s
        e = LoadString(3048, Result, cSLen): AdjustString Result '      Not in device object-list: (~1,~2)
        ReplaceText 1, Result, s
        ReplaceText 2, Result, CStr(ObjInst)
        scrAddItem Result, PicsTestForm!ResultList
        logWriteStr Result, gState.LogFileNr
      End If
    Loop
    e = LoadString(3038, Result, cSLen): AdjustString Result '
    scrAddItem Result, PicsTestForm!ResultList
    logWriteStr Result, gState.LogFileNr

End Sub

'The object types listed in the Standard Object Types Supported section of the Text PICS
'shall have a one-to-one correspondence with object types listed in the
'Object_Types_Supported property of the Device object contained in the Object Database
'section of the Text PICS. (k)
'For each object type listed in the Standard Object Types Supported section of the Text PICS there
'shall be at least one object of that type in the Object Database section of the Text PICS. (l)
'The Object_Types_Supported property of the Device object in the Database Section of the
'Text PICS shall indicate support for each object type required by the specified conformance
'class. (c)
'The Object_Types_Supported property of the Device object in the Database Section of the Text
'PICS shall indicate support for each object type required by the specified functional groups. (h)
Sub consDoObjTypes()
Dim pObj As Long
Dim Obj As generic_object
Dim i As Integer, e As Integer
Dim s As String, Result As String
Dim ObjList As String, resObjDev As String, resObjPICS As String
Dim objtype As Integer

    ObjList = String$(18, "0") 'not supported
    e = LoadString(3018, Result, cSLen): AdjustString Result
    scrAddItem Result, PicsTestForm!ResultList
    logWriteStr Result, gState.LogFileNr
    pObj = gDB.DataBase
    Do While (pObj <> 0)
      CpyGenObj Obj, pObj        'get the generic_object
      pObj = FindGenObj(pObj, 1) 'find the next object
      If (Obj.object_type < 0) Or (Obj.object_type > 17) Then
        'invalid object type
      Else
        Mid$(ObjList, Obj.object_type + 1, 1) = "1" 'supported
        If ((Asc(Mid$(gDB.BACnetStandardObjects, Obj.object_type + 1, 1)) And soSupported) <> soSupported) Then
            'object type is in our object list, but not supported -> inconsistency
            s = Space$(cSLen)
            e = GetObjType(Obj.object_type, s): AdjustString s
            e = LoadString(3019, Result, cSLen): AdjustString Result 'BACnet object type: ~1 not supported.
            ReplaceText 1, Result, s
            scrAddItem Result, PicsTestForm!ResultList
            logWriteStr Result, gState.LogFileNr
        End If
      End If
    Loop 'all objects
    'check BACnetStandardObjectsArray
    For i = 1 To 18
      If ((Asc(Mid$(gDB.BACnetStandardObjects, i, 1)) And soSupported) = soSupported) And (Mid$(ObjList, i, 1) = "0") Then
        'Object is missing: -> inconsistency
        s = Space$(cSLen)
        e = GetObjType(i - 1, s): AdjustString s
        e = LoadString(3020, Result, cSLen): AdjustString Result 'Missing object: ~1
        ReplaceText 1, Result, s
        scrAddItem Result, PicsTestForm!ResultList
        logWriteStr Result, gState.LogFileNr
      End If
    Next i

    resObjDev = Space(18)
    resObjPICS = Space(18)
    e = CheckObjTypeDevPics(gDB.BACnetStandardObjects, gDB.DataBase, resObjDev, resObjPICS)
    If (e = 0) Then
      For i = 1 To 18
        If (Asc(Mid$(resObjDev, i, 1)) > 0) Then 'object type is missing in Device object
          e = LoadString(3021, Result, cSLen): AdjustString Result
          s = Space(cSLen)
          e = GetObjType(i - 1, s): AdjustString s
          ReplaceText 1, Result, s
          scrAddItem Result, PicsTestForm!ResultList
          logWriteStr Result, gState.LogFileNr
        End If
      Next i
      For i = 1 To 18
        If (Asc(Mid$(resObjPICS, i, 1)) > 0) Then 'object type is missing in PICS section
          e = LoadString(3022, Result, cSLen): AdjustString Result
          s = Space(cSLen)
          e = GetObjType(i - 1, s): AdjustString s
          ReplaceText 1, Result, s
          scrAddItem Result, PicsTestForm!ResultList
          logWriteStr Result, gState.LogFileNr
        End If
      Next i
     End If

    e = LoadString(3033, Result, cSLen): AdjustString Result
    scrAddItem Result, PicsTestForm!ResultList
    logWriteStr Result, gState.LogFileNr

End Sub

'For each object included in the Object Database section of the Text PICS, all required
'properties for that object as defined in Clause 12 of BACnet shall be present. (n)
Sub consDoRProp()
Dim e As Integer
Dim pObj As Long
Dim Obj As generic_object
Dim i As Integer, c As Integer, pflags As Integer
Dim PropName As String
Dim s As String, Result As String

    e = LoadString(3016, Result, cSLen): AdjustString Result
    scrAddItem Result, PicsTestForm!ResultList
    logWriteStr Result, gState.LogFileNr
    pObj = gDB.DataBase
    Do While (pObj <> 0)
      CpyGenObj Obj, pObj        'get the generic_object
      pObj = FindGenObj(pObj, 1) 'find the next object
      PropName = Space(cSLen)
      c = VTSAPIgetpropinfo(Obj.object_type, &HFFFF, PropName, 0, 0, 0, 0) - 1
      For i = 0 To c
        PropName = Space(cSLen)
        e = VTSAPIgetpropinfo(Obj.object_type, i, PropName, 0, 0, pflags, 0)
        If ((pflags And pfR) = pfR) Or ((pflags And pfW) = pfW) Then 'required
          If (Asc(Mid$(Obj.propFlags, i + 1, 1)) And 1) = 0 Then 'not supported!
            'save result/property name
            e = LoadString(3017, Result, cSLen)
            AdjustString PropName
            ReplaceText 1, Result, PropName
            AdjustString Obj.object_name: s = RTrim$(Obj.object_name)
            ReplaceText 2, Result, s
            ReplaceText 3, Result, CStr(Obj.object_id And cObjInst)
            Result = RTrim$(Result)
            scrAddItem Result, PicsTestForm!ResultList
            logWriteStr Result, gState.LogFileNr
          End If
        End If
      Next i 'all properties
    Loop 'all objects
    e = LoadString(3032, Result, cSLen): AdjustString Result
    scrAddItem Result, PicsTestForm!ResultList
    logWriteStr Result, gState.LogFileNr

End Sub

'This function is used to check the consistency of the PICS
'in:        Nr      consistency test number
'returns: eNOERROR if checked, else error code
Function consDoSection(ByVal nr As Long)

consDoSection = eNOERROR

Select Case nr
  
  Case consRProp: 'check required properties
    consDoRProp
  Case consGroupProp: 'check group properties
    consDoGroupProp
  Case consObjectList: 'check object list (device object)
    consDoObjectList
  Case consObjTypes: 'check object types supported
    consDoObjTypes
  Case consConfClass: 'check conformance class
    consDoConfClass
  Case consFuncGroup: 'check functional groups
    consDoFuncGroup gDB.BACnetFunctionalGroups
  'this test HAS TO BE the LAST test in PicsTestForm!TestList ( consFillStatus() )
  Case consApplServ: 'check application services supported (device object)
    consDoApplServ

    'this has to be the last statement:
    gState.consTestNr = -1 'end of test

End Select

End Function

'This function is used to check the consistency of the PICS. There should not be
'any inconsistency between data in the PICS file. Any inconsistency will be displayed
'as a warning message in the ResultList/log file.
'in:        State       current test state
'returns: eNOERROR if ok, else a Pics error code
Function consDoTest(State As PicsState) As Integer
Dim e As Integer
Dim s  As String, TestDone As Integer

consDoTest = eUNDEF
TestDone = False

Do Until TestDone Or State.EndTest
  DoEvents
  
  Select Case State.SubTestNr

    Case 0: 'initial actions ----------------------------------------------------------------------
      State.SubTestNr = State.SubTestNr + 1
      logWriteStr String$(40, "*"), gState.LogFileNr
      e = LoadString(3025, s, cSLen): AdjustString s 'PICS Consistency Test
      logWriteStr s, gState.LogFileNr
      logWriteStr String$(40, "*"), gState.LogFileNr
      logWriteStr " ", gState.LogFileNr

    Case 1: 'consistency test ---------------------------------------------------------------------
      TestDone = True
      GetEntryPoint 'continue with this test
      e = LoadString(gState.info1, s, cSLen): AdjustString s
      logWriteStr s, gState.LogFileNr
      e = LoadString(gState.info2, s, cSLen): AdjustString s
      logWriteStr s, gState.LogFileNr
      logWriteStr " ", gState.LogFileNr
      e = consDoSection(gState.consTestNr)
      logWriteStr " ", gState.LogFileNr
      On Error Resume Next
      PicsTestForm!TestList.ListIndex = PicsTestForm!TestList.ListIndex + 2 '       ***222
      PicsTestForm!TestList.TopIndex = PicsTestForm!TestList.ListIndex
      On Error GoTo 0
      If (gState.consTestNr = -1) Then 'no other consistency tests
        State.SubTestNr = State.SubTestNr + 1
      End If

    Case 2: 'exit actions (results) ---------------------------------------------------------------
      e = LoadString(3023, s, cSLen): AdjustString s
      scrAddItem s, PicsTestForm!ResultList
      logWriteStr s, gState.LogFileNr
      logWriteStr Space$(1), gState.LogFileNr
      PicsTestForm!ResultList.ListIndex = PicsTestForm!ResultList.ListCount - 1 '     ***222
      State.EndTest = True 'end of test
      State.SubTestNr = 0  'first state

  End Select

Loop 'perform at least one test

consDoTest = eNOERROR
End Function

Function consDoTestFrame(State As PicsState) As Integer
Dim e As Integer
Dim s  As String, TestDone As Integer

consDoTestFrame = eUNDEF
TestDone = False

Do Until TestDone Or State.EndTest
  DoEvents

  Select Case State.SubTestNr

    Case 0: 'initial actions ----------------------------------------------------------------------
      GetEntryPoint 'continue with this test
      logWriteStr String$(40, "*"), State.LogFileNr
      e = LoadString(3025, s, cSLen): AdjustString s 'PICS Consistency Test
      logWriteStr s, gState.LogFileNr
      logWriteStr String$(40, "*"), State.LogFileNr
      logWriteStr " ", State.LogFileNr
      State.SubTestNr = State.SubTestNr + 1

    Case 1: 'consistency test ---------------------------------------------------------------------
      TestDone = True
      e = LoadString(State.info1, s, cSLen): AdjustString s
      logWriteStr s, State.LogFileNr
      PicsTestForm!ContentList.AddItem s
      e = LoadString(State.info2, s, cSLen): AdjustString s
      logWriteStr s, State.LogFileNr
      PicsTestForm!ContentList.AddItem s
      logWriteStr " ", State.LogFileNr 'new line
      e = consDoSection(State.consTestNr)
      logWriteStr " ", State.LogFileNr
      On Error Resume Next
      PicsTestForm!ResultList.ListIndex = PicsTestForm!ResultList.ListCount - 1 '   ***222
      PicsTestForm!ResultList.TopIndex = PicsTestForm!ResultList.ListIndex
      On Error GoTo 0
      State.SubTestNr = State.SubTestNr + 1
    
    Case 2: 'exit actions (results) ---------------------------------------------------------------
      If (State.consTestNr = -1) Then 'no other consistency tests
        State.EndTest = True
      End If
      State.SubTestNr = 0  'first state
      On Error Resume Next
      PicsTestForm!TestList.ListIndex = PicsTestForm!TestList.ListIndex + 2 '       ***222
      PicsTestForm!TestList.TopIndex = PicsTestForm!TestList.ListIndex
      On Error GoTo 0

  End Select

Loop 'perform at least one test

consDoTestFrame = eNOERROR
End Function

'This function is used to fill the status listboxes with
'appropriate information for the consistency test.
Sub consFillStatus()
Dim s As String, info As String, e As Integer

If gDB.DataBase = 0 Then Exit Sub

e = LoadString(3052, info, cSLen): info = Mid$(info, 1, e) '(PICS File cross check)

'required properties test:
e = LoadString(3016, s, cSLen): AdjustString s '+++ Check PICS objects for required properties.
PicsTestForm!TestList.AddItem s
PicsTestForm!TestList.AddItem info
s = StuffLong(consRProp) & StuffLong(3053) & StuffLong(3054)
LLAddItem gTestLL(), s 'link these list items to gTestLL

'group properties test:
e = LoadString(3026, s, cSLen): AdjustString s
PicsTestForm!TestList.AddItem s
PicsTestForm!TestList.AddItem info
s = StuffLong(consGroupProp) & StuffLong(3057) & StuffLong(3058)
LLAddItem gTestLL(), s 'link these list items to gTestLL

'object list (device object) test:
e = LoadString(3031, s, cSLen): AdjustString s
PicsTestForm!TestList.AddItem s
PicsTestForm!TestList.AddItem info
s = StuffLong(consObjectList) & StuffLong(3065) & StuffLong(3066)
LLAddItem gTestLL(), s 'link these list items to gTestLL

'object types supported test:
e = LoadString(3018, s, cSLen): AdjustString s '+++ Check PICS Object Types supported
PicsTestForm!TestList.AddItem s
PicsTestForm!TestList.AddItem info
s = StuffLong(consObjTypes) & StuffLong(3055) & StuffLong(3056)
LLAddItem gTestLL(), s 'link these list items to gTestLL

'conformance class test:
e = LoadString(3028, s, cSLen): AdjustString s
PicsTestForm!TestList.AddItem s
PicsTestForm!TestList.AddItem info
s = StuffLong(consConfClass) & StuffLong(3059) & StuffLong(3060)
LLAddItem gTestLL(), s 'link these list items to gTestLL

'functional group test:
e = LoadString(3029, s, cSLen): AdjustString s
PicsTestForm!TestList.AddItem s
PicsTestForm!TestList.AddItem info
s = StuffLong(consFuncGroup) & StuffLong(3061) & StuffLong(3062)
LLAddItem gTestLL(), s 'link these list items to gTestLL

'application services test:
e = LoadString(3030, s, cSLen): AdjustString s
PicsTestForm!TestList.AddItem s
PicsTestForm!TestList.AddItem info
s = StuffLong(consApplServ) & StuffLong(3063) & StuffLong(3064)
LLAddItem gTestLL(), s 'link these list items to gTestLL

End Sub

'This function is used to find and to decode a application tagged
'Boolean value in an APDU.
'in:    apdu    encoded data
'       alen    length of apdu
'out:   pval    result: 1=true, 0=false
'returns: if no error: true, else: false
Function DecodeBool(apdu As String, ByVal alen As Integer, pval As Integer) As Integer
Dim i As Integer, e As Integer
Dim tag As Integer, tlen As Integer

DecodeBool = False
If alen > Len(apdu) Then Exit Function

i = 1
Do While i <= alen
  
  e = GetTagLen(Mid$(apdu, i, 1), tag, tlen)
  Select Case (tag)
    Case &H10: 'application tag, BOOLEAN (false)
      pval = 0
      Exit Do
    Case &H11: 'application tag, BOOLEAN (true)
      pval = 1
      Exit Do
    Case Else:
      i = i + tlen
  End Select

Loop

DecodeBool = True

End Function

'This function is used to find and to decode an application tagged
'real vaalue in an APDU.
'in:    apdu    encoded data
'       alen    length of apdu
'out:   pval    contains real value
'returns: if no error: true, else: false
Function DecodeReal(apdu As String, ByVal alen As Integer, pval As Single) As Integer
Dim i As Integer, e As Integer
Dim tag As Integer, tlen As Integer

DecodeReal = False
If alen > Len(apdu) Then Exit Function

i = 1
Do While i <= alen
  
  e = GetTagLen(Mid$(apdu, i, alen - i + 1), tag, tlen)

  Select Case (tag)
    
    Case &H44: 'application tag, REAL
      pval = vbREAL(Mid$(apdu, i + 1, 4))
      Exit Do
    Case Else: i = i + tlen

  End Select

Loop

DecodeReal = True
End Function

'This function performs testing of BACnet devices in order to their
'protocol implementation conformance statement (PICS). The function
'performs one particular test, and changes the current testing state.
'
'returns:
'   0: ok
'  <0: error
'
Function DoPicsTest(State As PicsState) As Integer
Dim e As Integer, i As Integer, c As Integer

DoPicsTest = eUNDEF 'error by default

DoEvents

If gState.StopAfterErr <= 0 Then
  e = PicsError(eTOOMANYERR, CStr(gStopAfterErr))
  If e = 0 Then 'resume next, continue testing
    State.StopAfterErr = gStopAfterErr
  Else
    DoPicsTest = eNOERROR
    State.EndTest = True
    Exit Function
  End If
End If

Select Case State.TestNr
  Case cConsTest: 'consistency test
    e = consDoTestFrame(State)
  Case cRdPropTest: 'read properties test
    e = rdDoTestFrame(State)
  Case cWrtPropTest: 'write properties test
    e = wrtDoTestFrame(State)
  Case cListElementTest: 'Add/Remove List Element Test
    e = lstDoTestFrame(State)
  Case Else
    State.EndTest = True 'end of test
End Select

DoPicsTest = e
If e <> eNOERROR Then
  e = PicsError(e, "") 'show error message

End If


End Function

'This function is used to encode the fixed part of an AddListElement
'Service-request.
'in:    apdu    variable part of apdu
'       alen    length of apdu
'out:   apdu    fixed & variable part
'       alen    new length
Sub eAddList(apdu As String, alen As Integer)
apdu = Chr$(&H0) & Chr$(&H0) & Chr$(&H0) & Chr$(8) & apdu
alen = alen + 4
End Sub

'This Function is used to remove any PDU-Header data from an APDU.
Sub eCutHeader(apdu As String, alen As Integer)
Dim pdutype As Integer, HLen As Integer

On Error GoTo eCutHeaderErr

If (alen = 0) Or Len(apdu) = 0 Then Exit Sub
pdutype = Asc(Mid$(apdu, 1, 1)) And &HF0
Select Case pdutype
  Case &H0: HLen = 4  'Confirmed-Request
  Case &H10: HLen = 2 'Unconfirmed request
  Case &H20: HLen = 3 'Simple-Ack
  Case &H30: HLen = 3 'Complex-Ack
  Case Else: HLen = 0 'Don't cut anything
End Select

If (alen - HLen) > 0 Then
  alen = alen - HLen
  apdu = Mid$(apdu, HLen + 1, alen)
End If

Exit Sub

eCutHeaderErr:
  'nada

End Sub

'This function is used to encode any property value according to its ASN.1 production
'The property value can be retrieved from the database or a generated test value.
'in:        pObj        pointer to BACnet object
'           Obj         generic_object
'           PropId      property to be tested
'out:       Apdu        encoded property
'           Alen        length of Apdu
'returns: eNOERROR, if ok
'         eTESTFAILURE, if test failed (partly or completely)
Function EncodeFromDB(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, apdu As String, alen As Integer, ByVal db As Integer) As Integer
Dim Msg As PVMessage

If (db) Then
  Msg.Action = cRND_ENCODEPROP 'encode random property value
Else
  Msg.Action = cENCODEPROP     'encode from database
End If

Msg.Obj = pObj
Msg.PropId = PropId
Msg.ArrayIndex = -1
Msg.priority = -1
Msg.TagNr = -1
Msg.BufferLen = cApduLen

If (PropId = 85) And (Not db) Then 'Present Value (Commandable Property)
  Select Case (Obj.object_type)
    Case 1, 4, 14, 2, 5: 'in objects AO, BO, MSO, AV, BV
      Msg.priority = 16
  End Select
End If

apdu = Space(cApduLen)
If GetPropValue(apdu, Msg) = 0 Then 'read property value from database
  apdu = Mid$(apdu, 1, Msg.BufferLen)
  alen = Msg.BufferLen
  EncodeFromDB = eNOERROR
Else
  alen = 0
  EncodeFromDB = eENCODEFAILURE
End If

End Function

'This Function is used to encode the fixed part of a RemoveListElement
'Service-Request.
'in:    apdu    variable part of apdu
'       alen    length of apdu
'out:   apdu    fixed & variable part
'       alen    new length
Sub eRemoveList(apdu As String, alen As Integer)
apdu = Chr$(&H0) & Chr$(&H0) & Chr$(&H0) & Chr$(9) & apdu
alen = alen + 4

End Sub

'This Function is used to encode a WriteProperty Request Header.
Sub eWriteRequest(apdu As String, alen As Integer)
apdu = Chr$(&H0) & Chr$(&H0) & Chr$(&H0) & Chr$(15) & apdu
alen = alen + 4
End Sub

'This function is used to extract the property value of an APDU. The property extracted
'will be appended to a result string.
'in:    apdu        contains property value between 3E ... 3F
'       alen        length of apdu
'out:   pval        append property extracted to pval
'       plen        present length of pval
Function ExtractPropVal(pval As String, plen As Integer, apdu As String, ByVal alen As Integer) As Integer
Dim i As Integer, e As Integer
Dim open_i As Integer, close_i As Integer, indent As Integer
Dim tag As Integer, tlen As Integer

indent = 0 'open/close tag level
i = 1
open_i = 1
close_i = alen

Do While (i <= alen)
  
  e = GetTagLen(Mid$(apdu, i, alen - i + 1), tag, tlen) 'get tag number
  
  If (tag = &H3E) Then
    indent = indent + 1
    If indent = 1 Then
      open_i = i
    End If
  ElseIf (tag = &H3F) Then
    indent = indent - 1
    If indent = 0 Then
      close_i = i
    End If
  End If

  i = i + tlen 'next tag

Loop

tlen = close_i - open_i - 1 'length of data between 3E/3F
If (tlen > 0) Then
  plen = plen + tlen
  pval = pval & Mid$(apdu, open_i + 1, tlen)
End If

ExtractPropVal = eNOERROR

End Function

'This function is used to set the test-state-machine according to
'the selection in the test-listbox and the type of test.
Sub GetEntryPoint()

Dim i, ListIndex As Integer
Dim s As String, e As Integer
Dim info1 As Integer, info2 As Integer

'index in the list link
ListIndex = PicsTestForm!TestList.ListIndex
If ListIndex < 0 Then ListIndex = 0                   '       ***221
i = ListIndex \ 2
If ListIndex Mod 2 <> 0 Then ListIndex = ListIndex - 1
If i < 0 Or i > UBound(gTestLL) Then Exit Sub
PicsTestForm!TestList.ListIndex = ListIndex

Select Case gState.TestNr
  Case cConsTest:    'consistency test
    gState.consTestNr = UnstuffLong(Mid$(gTestLL(i).Entry, 1, 4))
    gState.info1 = UnstuffLong(Mid$(gTestLL(i).Entry, 5, 4)) 'string id
    gState.info2 = UnstuffLong(Mid$(gTestLL(i).Entry, 9, 4)) 'string id
    PicsTestForm!ContentList.Clear
  Case cRdPropTest, cWrtPropTest, cListElementTest:  'various tests
    gState.ActiveObject = UnstuffLong(Mid$(gTestLL(i).Entry, 1, 4))
    PicsTestForm!ContentList.Clear
  
End Select

End Sub

'This Function is used to get the number of objects in the
'internal database.
'returns: number of objects
Function GetNumberPicsObj() As Integer
Dim i As Integer, pObj As Long
i = 0
pObj = gDB.DataBase
Do While pObj <> 0
  pObj = FindGenObj(pObj, 1) 'next object
  i = i + 1
Loop
GetNumberPicsObj = i
End Function

'This function is used to get the first tag nr and the length of tagged data.
'in:    apdu   encoded data
'out:   t      tag nr (or extended tag nr)
'       tl     number of octets: tag (+ extended tag) + length octet (+ extended)
'                                + number of data octets
Function GetTagLen(apdu As String, t As Integer, tl As Integer) As Integer
Dim tag As Integer, j As Integer, tlen As Integer

GetTagLen = True
tag = Asc(Mid$(apdu, 1, 1))

If (tag And &HF0) = &HF0 Then 'extended tag
  t = Asc(Mid$(apdu, 2, 1))
  j = 3  'position of data octets
  tl = 2 'tag + extended tag
Else
  tl = 1 '1 tag octet
  t = tag
  j = 2  'position of data octets
End If

If ((tag And &H8) = &H0) And ((tag And &HF0) = &H10) Then 'BOOLEAN application tag
 Exit Function
End If

tag = tag And &H7
Select Case (tag)  'length/value/type field

  Case 0 To 4: 'length 0..4
    tl = tl + tag
  Case 5: 'length extension
    tlen = Asc(Mid$(apdu, j, 1)) 'extended length octet
    tl = tl + 1
    If tlen = 254 Then 'length 254 .. 65535
      tl = tl + 2
      tl = tl + CMyBigInt(Mid$(apdu, j + 1, 2))
    ElseIf tlen = 255 Then 'length 65536 .. 2^32-1
      tl = tl + 4
      tl = tl + CMyBigLong(Mid$(apdu, j + 1, 4))
    ElseIf tlen < 254 Then 'length octet 5 .. 253
      tl = tl + tlen 'length 5..253
    End If
  Case 6, 7: 'opening, closing tag
      'NOP

End Select

End Function

'This function is used to convert an octetstring
'to an ASCII-Hexstring-representation.
'in:    txt     contains octet values like 1, 15, 255, 237.
'       s       start index ( 1.. len(txt) )
'       l       number of octets to convert
'returns: Hexstring, like: "010FFF..."
Function HexStr(ByVal txt As String, ByVal s%, ByVal l%) As String
Dim h As String, i As Integer, ch As String
Dim c As Integer
On Error Resume Next
h = ""
c = s + l - 1
For i = s To c
  ch = Hex$(Asc(Mid$(txt, i, 1)))
  If Len(ch) = 1 Then ch = "0" & ch
  h = h & ch
Next i

HexStr = h
On Error GoTo 0
End Function

'This function is used to check, if a property is writable or not.
'in:        Obj     generic_object
'           i       index of property (0..63)
'returns:
'  3 : present-value (ai, bi, msi) is not writable
'  2 : present-value (ai, bi, msi) is writable
'  1 : writable
'  0 : not writable
Function IsWritable(Obj As generic_object, ByVal i As Integer) As Integer
Dim PropId%, pname$, ptype%, pgroup%, pflags%, pet%
Dim pval As Integer

pname = Space$(40)
pflags = 0

PropId = VTSAPIgetpropinfo(Obj.object_type, i, pname, ptype, pgroup, pflags, pet)
pflags = (pflags And pfW) = pfW
If Not pflags Then
  pflags = (Asc(Mid$(Obj.propFlags, i + 1, 1)) And 2) = 2 'is writable?
End If

'present-value in Analog-Input, Binary-Input, Binary Value, and Multistate-Input
'required to be writable if out-of-service is set to TRUE.
If (PropId = 85) Then
  Select Case Obj.object_type
    Case 0, 3, 5, 13:
      pval = True
    Case Else
      pval = False
  End Select
End If

If (pval) Then 'present-value
  If (pflags <> 0) Then IsWritable = 2 Else IsWritable = 3
Else 'not present-value
  If (pflags <> 0) Then IsWritable = 1 Else IsWritable = 0
End If

End Function

'This function is used to check, if a property is writable "List of", or not.
'in:        Obj     generic_object
'           i       index of property 0..63
'returns:
'  1 : writable
'  0 : not writable
Function IsWritableList(Obj As generic_object, ByVal i As Integer) As Integer
Dim PropId%, pname$, ptype%, pgroup%, pflags%, pet%
Dim pval As Integer, obj_type As Integer

pname = Space$(40)
pflags = 0
PropId = VTSAPIgetpropinfo(Obj.object_type, i, pname, ptype, pgroup, pflags, pet)
pflags = (pflags And pfW) = pfW
If Not pflags Then
  pflags = (Asc(Mid$(Obj.propFlags, i + 1, 1)) And 2) = 2 'is writable?
End If

If Not pflags Then 'not writable
  IsWritableList = 0
  Exit Function
End If

Select Case PropId
  Case 23, 122, 5, 7, 55, 116, 30, 39, 53, 102, 54: 'list properties
    IsWritableList = 1
    Exit Function
  Case 85: 'Present_Value
    If (Obj.object_type = 11) Then 'group object
      IsWritableList = 1
      Exit Function
    End If
  Case Else
    IsWritableList = 0 'not a list property
End Select

End Function

'This function is used to add a item to the array of ListLinks.
'in:        LL      array of ListLink
'           Entry   entry string
Sub LLAddItem(LL() As ListLink, ByVal Entry As String)
Dim i As Integer
i = UBound(LL)
LL(i).Entry = Entry 'entry point
ReDim Preserve LL(0 To i + 1)
End Sub

'This function is used to format APDU data for the log file.
'out:        s       list of strings containing formatted APDU data
'in          Apdu    APDU data
'            Alen    length of APDU
Sub logFormatApdu(s() As String, apdu As String, ByVal alen As Integer)
ReDim s(1 To 1) As String
Dim tlen As Integer, dlen As Integer, tag As Integer
Dim n As Integer, nBlock As Integer, i As Integer
Dim TagLevel As Integer, j As Integer, ins As Integer

On Error GoTo LogFormatApduErr

n = 0           'number of free string pointers
nBlock = 0      'number of 100 blocks
i = 1           'position in string list
TagLevel = 1    'number of opening tags
j = 1           'position in Apdu

Do While j <= alen

  If n = 0 Then
    nBlock = nBlock + 1
    n = 100
    ReDim Preserve s(1 To nBlock * 100) As String
  End If

  s(i) = ""

  'read tag ---
  tlen = 1 'start of data
  dlen = 0 'length of data
  tag = Asc(Mid$(apdu, j, 1))
  If (tag And &HF0) = &HF0 Then 'extendend tag number
    tlen = tlen + 1
  End If

  ins = TagLevel
  Select Case tag And &HF
    Case &HE: 'opening tag
      TagLevel = TagLevel + 1
    Case &HF: 'closing tag
      TagLevel = TagLevel - 1
      ins = TagLevel
    Case &H5: 'extended length
      If (j + tlen) <= alen Then dlen = Asc(Mid$(apdu, j + tlen, 1))
      tlen = tlen + 1
    Case Else 'length < 5
      If (tag And &H8) = &H8 Then 'context tag
        dlen = tag And &H7
      Else 'application tag
        If (tag And &HF0) = &H10 Then 'boolean
          dlen = 0                    'no length
        Else                          'other than boolean
          dlen = tag And &H7          'length
        End If
      End If
  End Select

  If (ins > 0) Then s(i) = Space$(ins * 3)
  
  If (j + tlen - 1) <= alen Then s(i) = s(i) & HexStr(apdu, j, tlen) & " "
  j = j + tlen

  If dlen > 0 Then
    If (j + dlen - 1) <= alen Then s(i) = s(i) & HexStr(apdu, j, dlen)
    j = j + dlen
  End If
  
  i = i + 1 'next string
  n = n - 1

Loop

If (i > 1) Then ReDim Preserve s(1 To i - 1)

Exit Sub

LogFormatApduErr:
 'nop

End Sub

'This function is used to scan a logfile and to display the
'error messages in a window.
'in:    FileTitle   filename (without file path)
'       FileName    filename (complete)
Sub logScanFile(FileTitle As String, FileName As String)
Dim LogForm As New PicsAnyList
Dim f As Integer, e As Integer
Dim s1 As String, s2 As String
Dim f1 As Integer, f2 As Integer
Dim LineNr As Integer

LogForm.Caption = FileTitle
e = eNOERROR

'open log file: ---
f = FreeFile
On Error GoTo ScanLogFileErr
Open FileName For Input As f
On Error GoTo 0
If e <> eNOERROR Then Exit Sub

LogForm!AnyList.Clear

'scan log file: ---

s2 = ""
f2 = True
LineNr = 1

On Error Resume Next

Do While Not EOF(f)

  s1 = s2
  f1 = f2

  Line Input #f, s2
  s2 = LTrim$(s2)
  f2 = StrComp(Mid$(s2, 1, 3), "err", 1) = 0

  If (f2) Then
    If (Not f1) Then
      LogForm!AnyList.AddItem s1
      If Err <> 0 Then Exit Do
    End If
    LogForm!AnyList.AddItem Space$(3) & CStr(LineNr) & ": " & s2
    If Err <> 0 Then Exit Do
  End If

  LineNr = LineNr + 1

Loop

On Error GoTo 0

'close log file: ---
Close f

LogForm.Show 1 'show modal
Exit Sub

ScanLogFileErr:
  If PicsError(eFILENOTFOUND, FileTitle) = 0 Then
    Resume
  Else
    e = eFILENOTFOUND
    Resume Next
  End If

End Sub

'This Function is used to write APDU data to a file.
'in:        Title       description of APDU data
'           APDU        tagged data
'           Alen        length of Apud
'           fNr         file handle
Sub logWriteApdu(Title As String, apdu As String, ByVal alen As Integer, fNr As Integer)
Dim StrList() As String
Dim i As Integer, u As Integer, l As Integer
If fNr < 1 Then Exit Sub 'no log file open
Print #fNr, Title
logFormatApdu StrList(), apdu, alen
u = UBound(StrList)
l = LBound(StrList)
For i = l To u
  Print #fNr, StrList(i)
Next i
End Sub

'This function is used to write a string to a log file.
'in:
'           s    string
'           fNr  file handle
Sub logWriteStr(ByVal s As String, fNr As Integer)
Dim ErrStr As String
If fNr < 1 Then Exit Sub 'no log file open
Print #fNr, s
End Sub

'This function is used to perform a Add/Remove List Element test.
'in:        pObj        pointer to BACnet object
'           Obj         generic_object
'           PropId      property to be tested
'           i           index of property
'out:       s           result string
'returns: eNOERROR, if ok
'         eTESTFAILURE, if test failed (partly or completely)
Function lstDoTest(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String) As Integer

Dim f As Integer, e As Integer, invid As Integer
Dim j As Integer, c As Integer
Dim ErrStr As String, s1 As String, Result As String
Dim passed As String, errors As String, LogStr As String
Dim rxOrg As String, RxOrgLen As Integer
Dim rxTest As String, rxTestLen As Integer
Dim txTest As String, txTestLen As Integer
Dim EmptyList As String, EmptyListLen As Integer
Dim PropList As String, PropListLen As Integer
Dim TempList As String, TempListLen As Integer
Dim Msg As PVMessage, because As String, FirstElement As Integer


'1. Read original property value from device under test.
'2.1 Write empty list to "List of" property of the device under test.
'2.2 Read "List of" property
'2.3 Compare 2.2 with 2.1
'3. For every test value: 3 elements, 1 element
'    3.1 Encode test value from database
'    3.2 Add test value to "List of" property of the device under test.
'    3.3 Append test value to internal list
'    3.4 Read "List of" property from device under test.
'    3.5 Compare 3.3 with 3.4
'4. Remove all list elements from the "List of" property.
'    4.1 Remove one list element from property
'    4.2 Remove same list element from internal list
'    4.3 Read list property and compare with 4.2
'    4.4 Remove all list elements from property
'    4.5 Read list property and compare with 2.1
'5. Write back original value to device under test.


f = eNOERROR

c = LoadString(3094, because, cSLen): because = Mid$(because, 1, c)
c = LoadString(3009, passed, cSLen): passed = Mid$(passed, 1, c)
c = LoadString(3010, errors, cSLen): errors = Mid$(errors, 1, c)

logWriteStr "", gState.LogFileNr

'rd org: ~1, wrt empty list: ~2, rd empty list: ~3,  cmp empty list: ~4,
'Add List Elements: ~5,  Remove List Elements: ~6, wrt org: ~7.
c = LoadString(3092, s, cSLen): s = Mid$(s, 1, c)


'1. Read original property value from device under test. -----------------------------------------

e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxOrg, RxOrgLen, ErrStr, invid)
eCutHeader rxOrg, RxOrgLen
'Read Property Service (InvId=~1): original value (received)
c = LoadString(3085, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr
If e <> eNOERROR Then
  'Failed to read original list property value.
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 1, s, ErrStr
  ReplaceText 2, s, "-"
  ReplaceText 3, s, "-"
  ReplaceText 4, s, "-"
  ReplaceText 5, s, "-"
  ReplaceText 6, s, "-"
  ReplaceText 7, s, "-"
  lstDoTest = eTESTFAILURE
  Exit Function
Else
  ReplaceText 1, s, passed
End If

logWriteStr "", gState.LogFileNr

'2.1 Write empty list to "List of" property of the device under test. -----------------------------------------

Msg.Action = cRND_ENCODEPROP
Msg.Obj = pObj
Msg.PropId = PropId
Msg.ArrayIndex = -1
Msg.priority = -1
Msg.TagNr = -1
Msg.TestNr = 2 'empty list
Msg.BufferLen = cApduLen
txTest = Space(cApduLen)
e = GetPropValue(txTest, Msg)
txTestLen = Msg.BufferLen: txTest = Mid$(txTest, 1, txTestLen)
  
eWriteRequest txTest, txTestLen  'insert Confirmed-Request-Header
e = outWriteProp(txTest, txTestLen, ErrStr, invid)
eCutHeader txTest, txTestLen
'Write Property Service  (InvId=~1):  test value (sent)
c = LoadString(3083, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, txTest, txTestLen, gState.LogFileNr
If (e <> eNOERROR) Then
  'Failed to write empty list
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 2, s, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 3, s, passed
End If

logWriteStr "", gState.LogFileNr
EmptyList = txTest       'save the empty list (we need it to check RemoveListElement )
EmptyListLen = txTestLen

'2.2 Read "List of" property -----------------------------------------

e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxTest, rxTestLen, ErrStr, invid)
eCutHeader rxTest, rxTestLen
'Read Property Service (InvId=~1): test value (received)
c = LoadString(3084, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxTest, rxTestLen, gState.LogFileNr
If (e <> eNOERROR) Then
  'Failed to read list property.
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 3, s, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 3, s, passed
End If

'2.3 Compare 2.2 with 2.1 -----------------------------------------

e = CompareApdu(txTest, txTestLen, rxTest, rxTestLen, ErrStr)
If (e <> eNOERROR) Then
  f = eTESTFAILURE 'Property Values don't match.
  ReplaceText 4, s, ErrStr
Else
  ReplaceText 4, s, passed
End If

'3. For every test value: 3 (1 Element), 0 (3 Elements) -----------------------------------------

'Add Test List Element: ~1, Read List Property: ~2, Compare List: ~3.
c = LoadString(3095, s1, cSLen): AdjustString s1

PropList = "": PropListLen = 0
ErrStr = ""

For j = 3 To 0 Step -3

  '3.1 Encode test value from database

  Result = s1

  Msg.Action = cRND_ENCODEPROP
  Msg.Obj = pObj
  Msg.PropId = PropId
  Msg.ArrayIndex = -1
  Msg.priority = -1
  Msg.TagNr = -1
  Msg.TestNr = j '0: 3 Elements, 3: 1 Element
  Msg.BufferLen = cApduLen
  txTest = Space(cApduLen)
  e = GetPropValue(txTest, Msg)
  txTestLen = Msg.BufferLen: txTest = Mid$(txTest, 1, txTestLen)

  '3.2 Add test value to "List of" property of the device under test.

  eAddList txTest, txTestLen 'insert Service Header
  e = outWriteProp(txTest, txTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader txTest, txTestLen
  'AddListElement Service  (InvId=~1):  list elements (sent)
  c = LoadString(3098, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, txTest, txTestLen, gState.LogFileNr
  If (e <> eNOERROR) Then
    'Failed to add list elements.
    logWriteStr because & ErrStr, gState.LogFileNr
    ReplaceText 1, Result, ErrStr
    f = eTESTFAILURE
  Else
    ReplaceText 1, Result, passed
  End If
  ReplaceText 1, Result, passed


  '3.3 Append test value to internal list
  e = ExtractPropVal(PropList, PropListLen, txTest, txTestLen)
  If (j = 3) Then
    FirstElement = PropListLen
  End If

  '3.4 Read "List of" property from device under test.

  e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxTest, rxTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader rxTest, rxTestLen
  'Read Property Service (InvId=~1): test value (received)
  c = LoadString(3084, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, rxTest, rxTestLen, gState.LogFileNr
  If (e <> eNOERROR) Then
    'Failed to read list property
    logWriteStr because & ErrStr, gState.LogFileNr
    ReplaceText 2, Result, ErrStr
    f = eTESTFAILURE
  Else
    ReplaceText 2, Result, passed
  End If
  ReplaceText 2, Result, passed

  '3.5 Compare 3.3 with 3.4

  TempList = ""
  TempListLen = 0
  e = ExtractPropVal(TempList, TempListLen, rxTest, rxTestLen)

  If StrComp(PropList, TempList, 0) <> 0 Then 'case sensitive comparison failed
    f = eTESTFAILURE
    'Property Values don't match!
    c = LoadString(7795, ErrStr, cSLen): AdjustString ErrStr
    ReplaceText 3, Result, ErrStr
  Else
    ReplaceText 3, Result, passed
  End If

  'add result to screen/log file: ---
  scrAddItem Result, PicsTestForm.ResultList
  logWriteStr "", gState.LogFileNr
  logWriteStr Result, gState.LogFileNr

Next j

If Len(ErrStr) <> 0 Then
  ReplaceText 5, s, ErrStr
Else
  ReplaceText 5, s, passed
End If


'4. Remove all list elements from the "List of" property. ----------------------------------------

'Rmv 1 LstElem:   ~1, Rd LstProp: ~2, Cmp Lst: ~3,
'Rmv all LstElem: ~4, Rd LstProp: ~5, Cmp Lst: ~6
c = LoadString(3096, Result, cSLen): Result = Mid$(Result, 1, c)
ErrStr = ""

'4.1 Remove one list element from property

txTest = Mid$(EmptyList, 1, EmptyListLen - 1) 'no closing tag
txTest = txTest & Mid$(PropList, 1, FirstElement) & Chr$(&H3F)
txTestLen = EmptyListLen - 1 + FirstElement + 1

eRemoveList txTest, txTestLen
e = outWriteProp(txTest, txTestLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader txTest, txTestLen
'RemoveListElement Service  (InvId=~1):  list elements (sent)
c = LoadString(3099, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, txTest, txTestLen, gState.LogFileNr

If (e <> eNOERROR) Then
  'Failed to remove list element.
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 1, Result, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 1, Result, passed
End If

'4.2 Remove same list element from internal list
PropList = Mid$(PropList, FirstElement + 1)
PropListLen = PropListLen - FirstElement

'4.3 Read list property and compare with 4.2

e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxTest, rxTestLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxTest, rxTestLen
'Read Property Service (InvId=~1): test value (received)
c = LoadString(3084, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxTest, rxTestLen, gState.LogFileNr
If (e <> eNOERROR) Then
  'Failed to read list property
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 2, Result, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 2, Result, passed
End If

TempList = ""
TempListLen = 0
e = ExtractPropVal(TempList, TempListLen, rxTest, rxTestLen)

If StrComp(PropList, TempList, 0) <> 0 Then 'case sensitive comparison failed
  f = eTESTFAILURE
  'Property Values don't match!
  c = LoadString(7795, ErrStr, cSLen): AdjustString ErrStr
  ReplaceText 3, Result, ErrStr
Else
  ReplaceText 3, Result, passed
End If


'4.4 Remove all list elements from property

txTest = Mid$(EmptyList, 1, EmptyListLen - 1) 'no closing tag
txTest = txTest & PropList & Chr$(&H3F)
txTestLen = EmptyListLen - 1 + PropListLen + 1

eRemoveList txTest, txTestLen
e = outWriteProp(txTest, txTestLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader txTest, txTestLen
'RemoveListElement Service  (InvId=~1):  list elements (sent)
c = LoadString(3099, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, txTest, txTestLen, gState.LogFileNr

If (e <> eNOERROR) Then
  'Failed to remove list element.
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 4, Result, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 4, Result, passed
End If


'4.5 Read list property and compare with 2.1

e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxTest, rxTestLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxTest, rxTestLen
'Read Property Service (InvId=~1): test value (received)
c = LoadString(3084, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxTest, rxTestLen, gState.LogFileNr
If (e <> eNOERROR) Then
  'Failed to read list property
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 5, Result, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 5, Result, passed
End If

e = CompareApdu(rxTest, rxTestLen, EmptyList, EmptyListLen, ErrStr)
If (e <> eNOERROR) Then
  'Property Values don't match.
  ReplaceText 6, Result, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 6, Result, passed
End If

'save End-Result
If Len(ErrStr) <> 0 Then
  ReplaceText 6, s, ErrStr
Else
  ReplaceText 6, s, passed
End If

'add result to screen/log file: ---
scrAddItem Result, PicsTestForm.ResultList
logWriteStr "", gState.LogFileNr
logWriteStr Result, gState.LogFileNr

'5. Write back original value to device under test.-----------------------------------------

eWriteRequest rxOrg, RxOrgLen 'insert Confirmed-Request-Header
e = outWriteProp(rxOrg, RxOrgLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxOrg, RxOrgLen
'Write Property Service (InvId=~1): original value (sent)
c = LoadString(3086, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr
If e <> eNOERROR Then
  'Failed to write original list property value.
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 7, s, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 7, s, passed
End If

If (f <> eNOERROR) Then
  s = errors + ": " + s 'mark as failed
Else
  s = passed + ": " + s 'mark as passed
End If

lstDoTest = f
End Function

'This function is used to execute: initial actions, one ListElement test, exit actions.
'The ListElement test will be derived from the test listbox and the list of entrypoints.
'in:      State       current test state
'returns: eNOERROR if ok, else a Pics error code
Function lstDoTestFrame(State As PicsState) As Integer
Dim e As Integer, c As Integer, i As Integer, TestFailed As Integer
Dim sname$, s$, PropInfo$, PropIndex As Integer
Dim Obj As generic_object, PropId As Long
Dim PropFound As Integer, ObjName As String

lstDoTestFrame = eNOERROR
PropFound = False

Do Until PropFound Or State.EndTest
  DoEvents

  Select Case State.SubTestNr
      
    Case 0: 'initial actions -----------------------------------------------------------------------
      GetEntryPoint 'continue with this test
      State.SubTestNr = State.SubTestNr + 1
      State.ActiveProperty = 0
      PicsTestForm!ContentList.Clear
      If State.ActiveObject = 0 Then lstDoTestFrame = eNODATABASE: Exit Function
      CpyGenObj Obj, State.ActiveObject 'read the generic_object
      ObjName = Obj.object_name: AdjustString ObjName
      e = LoadString(3090, s, cSLen): AdjustString s 'Add/Remove List Element Test: ~1
      ReplaceText 1, s, ObjName
      PicsTestForm!ContentList.AddItem s
      logWriteStr "", gState.LogFileNr
      logWriteStr String$(40, "*"), gState.LogFileNr
      logWriteStr s, gState.LogFileNr
      logWriteStr String$(40, "*"), gState.LogFileNr
  
    Case 1: 'Add/Remove List Element test -------------------------------------------------------
      If State.ActiveObject = 0 Then lstDoTestFrame = eNODATABASE: Exit Function
      CpyGenObj Obj, State.ActiveObject 'read the generic_object
      PropIndex = State.ActiveProperty  'current property
      sname = Space$(cSLen)
      PropFound = 0
      Do
        e = GetPropNameSupported(sname, PropIndex, Obj.object_type, Obj.propFlags, PropId)
        If (e = -1) Then 'no property
          Exit Do
        ElseIf (e > 0) Then
          PropFound = IsWritableList(Obj, PropIndex)
        End If
        PropIndex = PropIndex + 1 'next property
      Loop Until (PropFound <> 0)

      ObjName = Obj.object_name: AdjustString ObjName
      If (State.ActiveProperty = 0) And (e = -1) Then 'this object has no writable property
        c = LoadString(3076, s, cSLen): AdjustString s
        ReplaceText 1, s, ObjName
        scrAddItem s, PicsTestForm!ResultList 'write result
        logWriteStr "", gState.LogFileNr
        logWriteStr s, gState.LogFileNr
      End If

      If (PropFound <> 0) Then 'property found

        State.ActiveProperty = PropIndex 'next property
        'Testing Property ~1 (~2) in object: ~3 (~4,~5)
        c = LoadString(3015, PropInfo, cSLen): PropInfo = Mid$(PropInfo, 1, c)
        sname = Space(cSLen)
        c = GetPropName(PropId, sname): sname = Mid$(sname, 1, c)
        ReplaceText 1, PropInfo, sname        'Property name
        ReplaceText 2, PropInfo, CStr(PropId) 'Property ID
        ReplaceText 3, PropInfo, ObjName 'Object name
        sname = Space(cSLen)
        c = GetObjType(Obj.object_type, sname): sname = Mid$(sname, 1, c)
        ReplaceText 4, PropInfo, sname
        ReplaceText 5, PropInfo, CStr(Obj.object_id And cObjInst)
        scrAddItem PropInfo, PicsTestForm!ResultList

        'run an individual test for this property/object:
        e = lstDoTest(State.ActiveObject, Obj, PropId, PropIndex - 1, s)

        If e = eTESTFAILURE Then 'this test failed
          State.StopAfterErr = State.StopAfterErr - 1 'decrement error counter
        End If
        scrAddItem s, PicsTestForm!ResultList 'write result
        logWriteStr "", gState.LogFileNr
        logWriteStr PropInfo, gState.LogFileNr
        logWriteStr s, gState.LogFileNr
        logWriteStr String$(10, "-"), gState.LogFileNr
        PicsTestForm!ResultList.ListIndex = PicsTestForm!ResultList.ListCount - 1 '   ***222
        PicsTestForm!ResultList.TopIndex = PicsTestForm!ResultList.ListIndex

      Else 'all properties tested, next state
        State.SubTestNr = State.SubTestNr + 1
      End If

    Case 2: 'exit actions (results) -----------------------------------------------------------------
      State.ActiveObject = FindGenObj(State.ActiveObject, 1) 'find next object
      If State.ActiveObject = 0 Then 'all objects tested
        State.EndTest = True 'that's all folks
      End If
      State.SubTestNr = 0 'first state
      On Error Resume Next
      PicsTestForm!TestList.ListIndex = PicsTestForm!TestList.ListIndex + 2   '       ***222
      PicsTestForm!TestList.TopIndex = PicsTestForm!TestList.ListIndex
      On Error GoTo 0
      PicsTestForm!ContentList.Clear

  End Select

Loop  'perform at least one test

lstDoTestFrame = eNOERROR

End Function

'This function is used to send a Read-Property request to a BACnet object, and to receive the
'reply (assembled, not segmented) from the BACnet object.
'in         ObjType:            object type
'           ObjectInstance:     instance number
'           PropId:             property to be read
'out        Rx:                 APDU received
'           Rxlen:              length of APDU received
'           s:                  error information
'returns:
'  eNOERROR     no error occured
'  eSENDFAIL    couldn't send  ReadPropertyService request
'  eREADPROP    couldn't encode ReadPropertyService request
Function outReadProp(ByVal objtype%, ByVal ObjectInstance&, ByVal PropId&, rx$, rxLen%, s As String, invid As Integer) As Integer
Dim e As Integer, Action As Integer
Dim apdu As String, alen As Integer
Dim oldMouse As Integer

e = eNOERROR

apdu = Space(cApduLen) 'allocate memory
'encode Read-Property request
alen = eReadPropService(apdu, vbOBJECTID(objtype, ObjectInstance), PropId, False, 0)
If alen Then
  gPicsSend = True                 'this flag is used in SendNPDU
  SendNPDU Left$(apdu, alen), True 'try to send my apdu
  invid = gPicsInvokeId            'our InvokeId
  If gPicsInvokeId = -1 Then       'send failed
    outReadProp = eSENDFAILURE
    s = "failed to send Read Property Service request!"
    Exit Function
  End If
Else
  outReadProp = eREADPROP
  s = "Function eReadPropService() failed"
  Exit Function
End If

'loop until reply received
oldMouse = Screen.MousePointer
Screen.MousePointer = HourGlass
e = PicsWaitApdu(rx, rxLen, s)
Screen.MousePointer = oldMouse

outReadProp = e
End Function

'This function is used to send a Service Request to a BACnet object.
'in         Apdu:               encoded apdu
'           Alen:               length of apdu
'out        s:                  error information
'returns:   eNOERROR            ok
'           eWRITEPROP          encoding error
'           eSENDFAIL           send failed
Function outWriteProp(apdu$, alen%, s As String, invid As Integer) As Integer
Dim e As Integer
Dim ack As String, AckLen As Integer, oldMouse As Integer

If alen > 0 Then
  gPicsSend = True                 'this flag indicates my send request
  SendNPDU Left$(apdu, alen), True 'send my apdu
  invid = gPicsInvokeId            'our InvokeId
  If gPicsInvokeId = -1 Then       'failed to send apdu
    e = eSENDFAILURE
    s = "failed to send BACnet-Confirmed-Service request!"
  Else                             'my apdu is sent
    'loop until reply received
    oldMouse = Screen.MousePointer
    Screen.MousePointer = HourGlass
    e = PicsWaitApdu(ack, AckLen, s)
    Screen.MousePointer = oldMouse
  End If
Else 'nothing to send
  e = eSENDFAILURE 'encoding error
  s = "failed to send"
End If

outWriteProp = e
End Function

'This function is used to interpret APDU sent/received.
'if APDU sent:
'  write InvokeID into gPicsInvokeId
'  reset receiver-buffer gRx, gRxLen
'if APDU received:
'  reset gPicsInvokeId (=-1)
'  copy APDU to gRx, gRxLen
'in:        AL:     nalserver packet
'           apdu:   APDU Data
Sub PicsInterpretApdu(AL As NALSpacket, apdu As String)
Dim pdutype As Integer

pdutype = dByte(1) 'read type of PDU
Select Case (pdutype And &HF0)

  'request ---

  Case &H0 'confirmed request
    gPicsInvokeId = Asc(AL.nalsInvokeID)
    gRxLen = 0
    gRx = ""

  'response ---

  Case &H20, &H30, &H40, &H50, &H60, &H70
    gPicsInvokeId = -1    'received, reset variable
    gRxLen = AL.nalsAlen  'length of APDU received
    gRx = apdu            'APDU received

  Case Else 'invalid PDUType ---
    gPicsInvokeId = -1
    gRxLen = 0
    gRx = ""

End Select

End Sub

'This function is used to wait, until a new APDU was received. The new APDU will be copied, and
'the global "receive buffer" will be cleared.
'in:        Rx      buffer to contain APDU received
'           RxLen   length of APDU received
'out:       s       contains error information
'returns:
'   eTIMEOUT:       nothing received within time limit
'   wTIMERERR:      timer could not be enabled
'   eNOERROR:       APDU was received and copied
Function PicsWaitApdu(rx As String, rxLen As Integer, s As String) As Integer
Dim received As Integer, e As Integer
Dim pdutype As Integer, c As Integer
Dim eCode As Long
Dim eClass As Long

PicsWaitApdu = eNOERROR

rxLen = 0: rx = ""
received = False

'PicsTestForm.Enabled = False               '                             ***218
e = TimerStart(gPicsInterval)               'start timer

If (e = eNOERROR) Then
  
  Do
     DoEvents                               'provide multi tasking
     If (gPicsInvokeId = -1) Then           'Apdu was received
       If gRxLen > 0 Then                   'if not consumed
         rxLen = gRxLen                     'copy length
         rx = gRx                           'copy Apdu
         rx = Mid$(rx, 1, rxLen)
         gRxLen = 0                         'reset receive length
         gRx = ""                           'reset receive buffer
         received = True
       Else                                 'invalid PDU received
         PicsWaitApdu = eAPDUEMPTY
         s = "invalid APDU received!"
         e = TimerStop()                    'stop timer
         'PicsTestForm.Enabled = True       '                             ***218
         Exit Function
       End If
     End If
  Loop Until received Or gPicsTimeOut

  e = TimerStop()  'check timer for timeout
  'PicsTestForm.Enabled = True              '                             ***218

  'interpret APDU:
  If received And rxLen > 0 Then
    pdutype = Asc(Mid$(rx, 1, 1)) And &HF0
    Select Case pdutype
      Case &H50:                            'BACnet-Error-PDU
        c = LoadString(3075, s, cSLen): AdjustString s
        eClass = Asc(Mid$(rx, 5, 1))        'error class
        eCode = Asc(Mid$(rx, 7, 1))         'error code
        ReplaceText 1, s, ErrorClass(eClass)
        ReplaceText 2, s, ErrorCode(eCode)
        e = eNAKPDU
      Case &H60: 'BACnet-Reject-PDU
        s = "BACnetReject"
        e = eNAKPDU
      Case &H70: 'BACnet-Abort-PDU
        s = "BACnetAbort"
        e = eNAKPDU
      Case Else: e = eNOERROR
    End Select
  End If

End If

If (e = eTIMEOUT) Then s = "TimeOut"
If (e = eTIMERERR) Then s = "Could not enable Timer!"

PicsWaitApdu = e
End Function


'This function is used to setup and to display the progress window
'(PicsProgressForm)
'in:        p   setup data
Sub ProgSetup(p As ProgressType)
PicsProgressForm!TitleLab.Caption = p.Title
PicsProgressForm!MinLab.Caption = CStr(p.Min)
PicsProgressForm!MaxLab.Caption = CStr(p.max)
PicsProgressForm!PresLab.Caption = CStr(p.pres)
PicsProgressForm!DescriptionLab.Caption = p.Description
PicsProgressForm!ProgScroll.Min = p.Min
PicsProgressForm!ProgScroll.max = p.max
PicsProgressForm!ProgScroll.Value = p.pres
PicsProgressForm.Show 0
End Sub

'This function is used to trigger the scrollbar in the progress window.
Sub ProgTrigger(p As ProgressType)
p.pres = p.pres + 1
If p.pres <= p.max Then
  PicsProgressForm!PresLab.Caption = CStr(p.pres)
  PicsProgressForm!ProgScroll.Value = p.pres
End If
If p.pres >= p.max Then
  Unload PicsProgressForm
End If
End Sub

'This function is used to perform a ReadProperty Test for a real property.
'in:    pObj        pointer to object in PICS Database
'       Obj         data of the generic_object
'       PropId      ID of real property
'       i           position of real property in the PICS object (zero-based)
'Out:   s           result string
'returns: eNOERROR if ok, else eTESTFAILURE
Function rdDoFloat(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String) As Integer

'1.1 Read original property value from BACnet device
'1.2 Decode ASN.1 string to real variable
'2. Get real property value from PICS Database
'3. Compare 1.2 with 2.

Dim f%, e%, c%, PropName$, eDB%
Dim passed$, because$, Result$, ErrStr$
Dim ApduRxOrg$, RxOrgLen%, errors As String
Dim PropVal$, PropLen%, invid As Integer
Dim Msg As PVMessage, LogStr As String, Value As String
Dim pval As Single, PValDB As Single

f = eNOERROR 'default

c = LoadString(3009, passed, cSLen): passed = Mid$(passed, 1, c)
c = LoadString(3094, because, cSLen): because = Mid$(because, 1, c)
c = LoadString(3010, errors, cSLen): errors = Mid$(errors, 1, c)
c = LoadString(3088, Value, cSLen): Value = Mid$(Value, 1, c)
c = LoadString(3013, s, cSLen): s = Mid$(s, 1, c) 'rd orig: ~1, compare: ~2.

logWriteStr "", gState.LogFileNr

'1.1 Read original property value from BACnet device

e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, ApduRxOrg, RxOrgLen, ErrStr, invid)
eCutHeader ApduRxOrg, RxOrgLen
c = LoadString(3085, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, ApduRxOrg, RxOrgLen, gState.LogFileNr
If e <> eNOERROR Then 'failed to read property
  logWriteStr because & ErrStr, gState.LogFileNr
  f = eTESTFAILURE
  Result = ErrStr
Else
  Result = passed
End If
ReplaceText 1, s, Result

logWriteStr "", gState.LogFileNr

'1.2 Decode ASN.1 string to real variable
e = DecodeReal(ApduRxOrg, RxOrgLen, pval)
logWriteStr Value & CStr(pval), gState.LogFileNr

'2. Get real property value from PICS Database

Msg.Action = cENCODEPROP
Msg.Obj = pObj
Msg.PropId = PropId
Msg.ArrayIndex = -1
Msg.priority = -1
Msg.TagNr = -1
Msg.BufferLen = cApduLen

PropVal = Space(cApduLen)
e = GetPropValue(PropVal, Msg) 'read property value from database
If (e = 0) Then
  PropVal = Mid$(PropVal, 1, Msg.BufferLen)
  PropLen = Msg.BufferLen
Else
  PropLen = 0
  ReplaceText 2, s, "Encoding error in GetPropValue()"
  f = eENCODEFAILURE
  Exit Function
End If

c = LoadString(3087, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, PropVal, PropLen, gState.LogFileNr

e = DecodeReal(PropVal, PropLen, PValDB)
logWriteStr Value & CStr(PValDB), gState.LogFileNr

'3. Compare 1.2 with 2.

If (pval < (PValDB - epsilon)) Or (pval > (PValDB + epsilon)) Then 'not equal
  'Property Values don't match.
  c = LoadString(7795, ErrStr, cSLen): ErrStr = Mid$(ErrStr, 1, c)
  ReplaceText 2, s, ErrStr
  f = eTESTFAILURE
Else 'equal
  ReplaceText 2, s, passed
End If


If (f <> eNOERROR) Then
  s = errors + ": " + s 'mark result as failed
Else
  s = passed + ": " + s 'mark result as passed
End If
rdDoFloat = f

End Function

'This function is used to perform a ReadProperty Test for Questionmark-Properties.
'in:    pObj        pointer to object in PICS Database
'       Obj         data of the generic_object
'       PropId      ID of real property
'       i           position of real property in the PICS object (zero-based)
'Out:   s           result string
'returns: eNOERROR if ok, else eTESTFAILURE
Function rdDoQuestion(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, ByVal pt As Integer, s As String) As Integer
Dim tag As Integer, rxTag As Integer, f As Integer
Dim e As Integer, c As Integer, invid As Integer
Dim pval As String, ApduRxOrg As String
Dim plen As Integer, RxOrgLen As Integer
Dim errors As String, passed As String, because As String
Dim ErrStr As String, LogStr As String


f = eNOERROR
c = LoadString(3094, because, cSLen): because = Mid$(because, 1, c)
c = LoadString(3009, passed, cSLen): passed = Mid$(passed, 1, c)
c = LoadString(3010, errors, cSLen): errors = Mid$(errors, 1, c)
c = LoadString(3013, s, cSLen): s = Mid$(s, 1, c) 'rd orig: ~1, compare: ~2.


'1 read the original property value ---
e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, ApduRxOrg, RxOrgLen, ErrStr, invid)
eCutHeader ApduRxOrg, RxOrgLen
c = LoadString(3085, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, ApduRxOrg, RxOrgLen, gState.LogFileNr
If e <> eNOERROR Then 'failed to read property
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 1, s, ErrStr
  ReplaceText 2, s, "-"
  rdDoQuestion = eTESTFAILURE
  Exit Function
Else
  ReplaceText 1, s, passed
End If

'write parse type:
logWriteStr "expected tag: X'" & HexStr(Chr$(tag), 1, 1) & "'", gState.LogFileNr

'2 Get tag of property value ---

pval = "": plen = 0
e = ExtractPropVal(pval, plen, ApduRxOrg, RxOrgLen)
If (plen > 0) Then
  rxTag = Asc(Mid$(pval, 1, 1)) And &HF0
Else
  rxTag = -1 'empty list
End If


'3 Compare tag from PICS with tag from device ---

Select Case pt
  Case ob_id, lobj: tag = &HC0 'BACnetObjectIdentifier
  Case et: tag = &H90 'ENUMERATED
  Case flt: tag = &H40 'REAL
  Case bits, pss, pos: tag = &H80 'BIT STRING
  Case uw, ud, u16, u127: tag = &H20 'Unsigned
  Case sw: tag = &H30 'INTEGER
  Case s10, s32, s64, s132, actext: tag = &H70 'CharacterString
  Case dt: tag = &HA0 'BACnetDateTime
  Case ptDate, dtrange: tag = &HA0 'Date
  Case ptTime: tag = &HB0 'Time
  Case ebool: tag = &H10  'BOOLEAN
  Case enull: tag = &H0  'NULL
  Case Else
    ReplaceText 2, s, "constructed datatype in ""?"" "
    rdDoQuestion = eTESTFAILURE
    Exit Function
End Select

'basic datatype:
If (rxTag <> tag) Then
  ReplaceText 2, s, "? - different datatype"
  f = eTESTFAILURE
Else
  ReplaceText 2, s, passed
End If

rdDoQuestion = f
End Function

'This function is used to perform a ReadProperty Test for standard properties.
'in:        pObj        pointer to generic_object
'           Obj         copy of generic_object
'           PropId      property identifier
'           i           position of real property in the PICS object (zero-based)
'out:       s           result string
'returns:   eNOERROR if passed, else eTESTFAILURE
Function rdDoStandard(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String)

'1. Read current property value from BACnet device
'2. Encode property value from PICS Database (ASN.1)
'3. Compare 1. with 2.

Dim f%, e%, c%, PropName$, eDB%
Dim passed$, errors$, Result$, ErrStr$
Dim ApduRxOrg$, RxOrgLen%, because As String
Dim PropVal$, PropLen%, invid As Integer
Dim Msg As PVMessage, LogStr As String

f = eNOERROR 'default

c = LoadString(3094, because, cSLen): because = Mid$(because, 1, c)
c = LoadString(3009, passed, cSLen): passed = Mid$(passed, 1, c)
c = LoadString(3010, errors, cSLen): errors = Mid$(errors, 1, c)
c = LoadString(3013, s, cSLen): s = Mid$(s, 1, c) 'rd orig: ~1, compare: ~2.

logWriteStr "", gState.LogFileNr

'1 read the original property value ---
e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, ApduRxOrg, RxOrgLen, ErrStr, invid)
eCutHeader ApduRxOrg, RxOrgLen
c = LoadString(3085, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, ApduRxOrg, RxOrgLen, gState.LogFileNr
If e <> eNOERROR Then 'failed to read property
  logWriteStr because & ErrStr, gState.LogFileNr
  f = eTESTFAILURE
  Result = ErrStr
Else
  Result = passed
End If
ReplaceText 1, s, Result

logWriteStr "", gState.LogFileNr

'encode database property value

Msg.Action = cENCODEPROP
Msg.Obj = pObj
Msg.PropId = PropId
Msg.ArrayIndex = -1
Msg.priority = -1
Msg.TagNr = -1
Msg.BufferLen = cApduLen

PropVal = Space(cApduLen)
e = GetPropValue(PropVal, Msg) 'read property value from database
If (e = 0) Then
  PropVal = Mid$(PropVal, 1, Msg.BufferLen)
  PropLen = Msg.BufferLen
Else
  PropLen = 0
  f = eENCODEFAILURE
End If

c = LoadString(3087, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, PropVal, PropLen, gState.LogFileNr

'2 compare with database ---
e = CompareApdu(ApduRxOrg, RxOrgLen, PropVal, PropLen, ErrStr)
If e <> eNOERROR Then
  Result = ErrStr
  f = eTESTFAILURE
Else
  Result = passed
End If
ReplaceText 2, s, Result

If (f <> eNOERROR) Then
  s = errors + ": " + s 'mark result as failed
Else
  s = passed + ": " + s 'mark result as passed
End If
rdDoStandard = f

End Function

'This function is used to execute the appropriate test function according to the parsetype
'of the property.
'in:        pObj        pointer to generic_object
'           Obj         copy of generic_object
'           PropId      property identifier
'           i           position of property in the PICS object (zero-based)
'out:       s           result string
'returns:   eNOERROR if passed, else eTESTFAILURE
Function rdDoTest(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String) As Integer
Dim Msg As PVMessage
Dim PropVal As String, PropLen As Integer
Dim e As Integer

e = eNOERROR

'retrieve parse type from database:
Msg.Action = cGET_PARSE_TYPE
Msg.Obj = pObj
Msg.PropId = PropId
Msg.ArrayIndex = -1
Msg.priority = -1
Msg.TagNr = -1
Msg.BufferLen = 0

PropVal = ""
If GetPropValue(PropVal, Msg) = 0 Then
  e = eNOERROR
End If

If (Asc(Mid$(Obj.propFlags, i + 1, 1)) And 4) = 4 Then
  rdDoTest = rdDoQuestion(pObj, Obj, PropId, i, Msg.pt, s)
  Exit Function
End If


'select test according parse type:
Select Case Msg.pt
  Case flt:  'decode?
    e = rdDoFloat(pObj, Obj, PropId, i, s)
  Case Else: 'standard test
    e = rdDoStandard(pObj, Obj, PropId, i, s)
End Select

rdDoTest = e

End Function

'This function is used to execute: initial actions, a ReadProperty Test, exit actions.
'The ReadProperty test will be derived from the test listbox and the list of entrypoints.
'in:      State       current test state
'returns: eNOERROR if ok, else a Pics error code
Function rdDoTestFrame(State As PicsState) As Integer
Dim PropInfo As String, PropFound As Integer
Dim e As Integer, c As Integer, i As Integer, TestFailed As Integer
Dim sname As String, s As String, PropIndex As Integer
Dim Obj As generic_object, PropId As Long, ObjName As String

rdDoTestFrame = eUNDEF
PropFound = False

Do Until PropFound Or State.EndTest

  Select Case State.SubTestNr
      
    Case 0: 'initial actions -----------------------------------------------------------------------
      GetEntryPoint 'continue with this test
      State.SubTestNr = State.SubTestNr + 1
      State.ActiveProperty = 0
      PicsTestForm!ContentList.Clear
      If State.ActiveObject = 0 Then rdDoTestFrame = eNODATABASE: Exit Function
      CpyGenObj Obj, State.ActiveObject 'read the generic_object
      ObjName = Obj.object_name: AdjustString ObjName
      e = LoadString(3003, s, cSLen): AdjustString s
      ReplaceText 1, s, ObjName
      PicsTestForm!ContentList.AddItem s
      logWriteStr "", gState.LogFileNr
      logWriteStr String$(40, "*"), gState.LogFileNr
      logWriteStr s, gState.LogFileNr
      logWriteStr String$(40, "*"), gState.LogFileNr
  
    Case 1: 'read property test ---------------------------------------------------------------
      If State.ActiveObject = 0 Then rdDoTestFrame = eNODATABASE: Exit Function
      CpyGenObj Obj, State.ActiveObject 'read the generic_object
      PropIndex = State.ActiveProperty  'current property
      sname = Space$(cSLen)
      Do
        e = GetPropNameSupported(sname, PropIndex, Obj.object_type, Obj.propFlags, PropId)
        PropIndex = PropIndex + 1
      Loop Until (e > 0) Or (e = -1) 'find next property supported
      
      If (e > 0) Then 'property found
        PropFound = True
        State.ActiveProperty = PropIndex 'next property
      
        'Testing Property ~1 (~2) in object: ~3 (~4,~5)
        c = LoadString(3015, PropInfo, cSLen): PropInfo = Mid$(PropInfo, 1, c)
        sname = Space(cSLen)
        c = GetPropName(PropId, sname): sname = Mid$(sname, 1, c)
        ReplaceText 1, PropInfo, sname        'Property name
        ReplaceText 2, PropInfo, CStr(PropId) 'Property ID
        ObjName = Obj.object_name: AdjustString ObjName
        ReplaceText 3, PropInfo, ObjName
        sname = Space(cSLen)
        c = GetObjType(Obj.object_type, sname): sname = Mid$(sname, 1, c)
        ReplaceText 4, PropInfo, sname
        ReplaceText 5, PropInfo, CStr(Obj.object_id And cObjInst)
        scrAddItem PropInfo, PicsTestForm!ResultList
      
        'do the Read/Write/Compare test ---
        If rdDoTest(gState.ActiveObject, Obj, PropId, PropIndex - 1, s) <> eNOERROR Then
          State.StopAfterErr = State.StopAfterErr - 1 'decrement error counter
        End If
      
        scrAddItem s, PicsTestForm!ResultList 'write result
        logWriteStr "", gState.LogFileNr
        logWriteStr PropInfo, gState.LogFileNr
        logWriteStr s, gState.LogFileNr
        logWriteStr String$(10, "-"), gState.LogFileNr
        On Error Resume Next
        PicsTestForm!ResultList.ListIndex = PicsTestForm!ResultList.ListCount - 1 '   ***222
        PicsTestForm!ResultList.TopIndex = PicsTestForm!ResultList.ListIndex
        On Error GoTo 0
      Else
        'all properties tested, next state
        State.SubTestNr = State.SubTestNr + 1
      End If

    Case 2: 'exit actions (results) -----------------------------------------------------------------
      State.ActiveObject = FindGenObj(State.ActiveObject, 1) 'find next object
      If State.ActiveObject = 0 Then 'all objects tested
        State.EndTest = True 'that's all folks
      End If
      State.SubTestNr = 0 'first state
      On Error Resume Next
      PicsTestForm!TestList.ListIndex = PicsTestForm!TestList.ListIndex + 2 '         ***222
      PicsTestForm!TestList.TopIndex = PicsTestForm!TestList.ListIndex
      PicsTestForm!ContentList.Clear
      On Error GoTo 0
  End Select

Loop 'perform one test cycle

rdDoTestFrame = eNOERROR
End Function

'This function is used to add a textstring to the result listbox. If the number of items
'in the listbox reached cMaxResult, then the listbox will remove the first listitem.
Sub scrAddItem(ByVal s As String, scroll As Control)

On Error Resume Next
If scroll.ListCount > cMaxResult Then
  scroll.RemoveItem 0
End If
scroll.AddItem s
scroll.TopIndex = scroll.ListCount - 1
On Error GoTo 0

End Sub

Sub StatusInfoClick()

Select Case gState.ActiveList
  Case cTEST:
    PicsTestStatusForm!StatusList.ListIndex = PicsTestForm!TestList.ListIndex '     ***222
  Case cCONTENT:
    PicsTestStatusForm!StatusList.ListIndex = PicsTestForm!ContentList.ListIndex '  ***222
  Case cRESULT:
    PicsTestStatusForm!StatusList.ListIndex = PicsTestForm!ResultList.ListIndex '   ***222
End Select

End Sub

'This Function is used to convert the octets of an integer value to a string.
'The integer value will be stored as "<low byte><high byte>"
'in:        i       int value
'returns:           string
Function StuffInt(ByVal i As Integer) As String
Dim s As String
s = ""
s = s & Chr$(&HFF And i): i = i \ CInt(256)
s = s & Chr$(&HFF And i)
StuffInt = s
End Function

'This Function is used to convert the octets of a long value to a string.
'The string will contain the lowest significant byte first.
'in:        l       long value
'returns:           string
Function StuffLong(ByVal l As Long) As String
Dim s As String
s = ""
s = s & Chr$(&HFF And l): l = l \ CLng(256)
s = s & Chr$(&HFF And l): l = l \ CLng(256)
s = s & Chr$(&HFF And l): l = l \ CLng(256)
s = s & Chr$(&HFF And l)
StuffLong = s
End Function

'This function is used to start the TimeOut-Timer.
'in: interval   length of timeout-period
'returns: eNOERROR if timer enabled, else eTIMERERR
Function TimerStart(ByVal Interval As Integer) As Integer

TimerStart = eNOERROR

On Error Resume Next
PicsGlobalForm!TimeoutTimer.Interval = Interval
PicsGlobalForm!TimeoutTimer.Enabled = True

If PicsGlobalForm!TimeoutTimer.Enabled = False Then
  TimerStart = eTIMERERR 'couldn't enable timer
End If
On Error GoTo 0

gPicsTimeOut = False    'no timeout

End Function

'This function is used to stop the TimeoutTimer.
'returns: eTIMEOUT if timeout occured
'         eNOERROR if no timeout occured
Function TimerStop() As Integer

If gPicsTimeOut Then
  TimerStop = eTIMEOUT 'timeout occured
Else
  TimerStop = eNOERROR
End If
 
'switch timer off
On Error Resume Next
PicsGlobalForm!TimeoutTimer.Enabled = False
On Error GoTo 0
End Function

'This function is used to convert a string to a int. The String contains
'the lowest significant byte first.
'in:    s   string representation of an integer value
'returns: integer value
Function UnstuffInt(ByVal s As String) As Integer
Dim i As Integer
i = 0
i = i + Asc(Mid$(s, 1, 1))
i = i + Asc(Mid$(s, 2, 1)) * CInt(256)
UnstuffInt = i
End Function

'This function is used to convert a string to a long. The String contains
'the lowest significant byte first.
'in:    s   string representation of a long value
'returns: long value
Function UnstuffLong(ByVal s As String) As Long
Dim l As Long
l = 0
l = l + Asc(Mid$(s, 1, 1))
l = l + Asc(Mid$(s, 2, 1)) * CLng(256)
l = l + Asc(Mid$(s, 3, 1)) * CLng(256 ^ 2)
l = l + Asc(Mid$(s, 4, 1)) * CLng(256 ^ 3)
UnstuffLong = l
End Function

'This function is used to perform a WriteProperty Test for enumerated properties.
'in:        pObj        pointer to generic_object
'           Obj         copy of generic_object
'           PropId      property identifier
'           i           position of real property in the PICS object (zero-based)
'out:       s           result string
'returns:   eNOERROR if passed, else eTESTFAILURE
Function wrtDoEnum(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String) As Integer
Dim e As Integer, f As Integer, invid As Integer, j As Integer
Dim c As Integer, eval As Integer, because As String
Dim ErrStr As String, passed As String, errors As String
Dim Result As String, s1 As String
Dim rxTest As String, rxTestLen As Integer
Dim txTest As String, txTestLen As Integer
Dim rxOrg As String, RxOrgLen As Integer
Dim LogStr As String, Value As String

Dim pet As Integer, pname As String, pid As Long, nEnum As Integer

'1. Read original property value from BACnet device.
'2. for every enumaration value:
'   2.1 Write enumeration value to BACnet device
'   2.2 Read back value from BACnet device
'   2.3  Compare 2.1 with 2.2
'3. Write back original property value to BACnet device.

f = eNOERROR
c = LoadString(3009, passed, cSLen): passed = Mid$(passed, 1, c)
c = LoadString(3010, errors, cSLen): errors = Mid$(errors, 1, c)
c = LoadString(3088, Value, cSLen): Value = Mid$(Value, 1, c)
c = LoadString(3094, because, cSLen): because = Mid$(because, 1, c)

pname = Space(40)
pid = VTSAPIgetpropinfo(Obj.object_type, i, pname, 0, 0, 0, pet) 'get the enumeration table

'1. Read original property value from BACnet device.

'Rd orig.: ~1, Wrt/Rd/Cmp enum values: ~2, Wrt orig.: ~3
c = LoadString(3082, s, cSLen): s = Mid$(s, 1, c)
e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxOrg, RxOrgLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxOrg, RxOrgLen
c = LoadString(3085, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr

eval = -1
If e <> eNOERROR Then 'failed to read original property value
  logWriteStr because & ErrStr, gState.LogFileNr
  s = errors + ": " + s 'mark result as failed
  ReplaceText 1, s, ErrStr
  ReplaceText 2, s, "-"
  ReplaceText 3, s, "-"
  wrtDoEnum = eTESTFAILURE
  Exit Function
Else
  ReplaceText 1, s, passed
  eval = Asc(Mid$(rxOrg, 10, 1)) 'get enumeration Value
End If

pname = Space$(40)
e = VTSAPIgetenumtable(pet, eval, 0, 0, pname) 'get enumeration text
AdjustString pname
ReplaceText 1, s, pname
logWriteStr Value & pname, gState.LogFileNr
logWriteStr "", gState.LogFileNr

'*** Range Test: writing all test values: ***
c = LoadString(3089, LogStr, cSLen): AdjustString LogStr
logWriteStr "", gState.LogFileNr
logWriteStr LogStr, gState.LogFileNr

'2. for every enumaration value:

'Wrt Enum: ~1, Rd Enum: ~2, Cmp Enum: ~3
c = LoadString(3081, s1, cSLen): s1 = Mid$(s1, 1, c)

pname = Space$(40)
nEnum = VTSAPIgetenumtable(pet, 0, 0, 0, pname)
If (nEnum = 0) Then
  wrtDoEnum = eTESTFAILURE
  s = "Code Error: No enumerations for this property! Wrong parse type!"
  Exit Function
End If

txTest = rxOrg
txTestLen = RxOrgLen

If (PropId = 85) Then 'Present Value (Commandable Property)
  Select Case (Obj.object_type)
    Case 1, 4, 14, 2, 5: 'in objects AO, BO, MSO, AV, BV
      txTest = txTest & Chr$(&H49) & Chr$(16) 'add priority
      txTestLen = txTestLen + 2
  End Select
End If

ErrStr = ""

For j = 1 To nEnum

  Result = s1 'Wrt Enum: ~1, Rd Enum: ~2, Cmp Enum: ~3
  
  '2.1 Write enumeration value to BACnet device
  
  pname = Space$(40)
  e = VTSAPIgetenumtable(pet, j - 1, 0, 0, pname) 'get enumeration text
  AdjustString pname
  
  Mid$(txTest, 10, 1) = Chr(j - 1) 'set enumeration value
  eWriteRequest txTest, txTestLen 'insert Confirmed-Request-Header
  e = outWriteProp(txTest, txTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader txTest, txTestLen
  c = LoadString(3083, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, txTest, txTestLen, gState.LogFileNr
  logWriteStr Value & pname, gState.LogFileNr
  If (e <> eNOERROR) Then
    logWriteStr because & ErrStr, gState.LogFileNr
    f = eTESTFAILURE
    ReplaceText 1, Result, ErrStr
  Else
    ReplaceText 1, Result, pname
  End If

  '2.2 Read back value from BACnet device

  e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxTest, rxTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader rxTest, rxTestLen
  c = LoadString(3084, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, rxTest, rxTestLen, gState.LogFileNr
  If (e <> eNOERROR) Then
    logWriteStr because & ErrStr, gState.LogFileNr
    f = eTESTFAILURE
    ReplaceText 2, Result, ErrStr
    eval = -1
  Else
    eval = Asc(Mid$(rxTest, 10, 1))
    pname = Space$(40)
    e = VTSAPIgetenumtable(pet, eval, 0, 0, pname) 'get enumeration text
    AdjustString pname
    ReplaceText 2, Result, pname
    logWriteStr Value & pname, gState.LogFileNr
  End If

  
  '2.3  Compare 2.1 with 2.2

  If eval <> (j - 1) Then
    f = eTESTFAILURE
    'Property Values don't match.
    c = LoadString(7795, ErrStr, cSLen): ErrStr = Mid$(ErrStr, 1, c)
    ReplaceText 3, Result, ErrStr
  Else
    ReplaceText 3, Result, passed
  End If

  scrAddItem Result, PicsTestForm.ResultList
  logWriteStr "", gState.LogFileNr
  logWriteStr Result, gState.LogFileNr

Next j

logWriteStr "", gState.LogFileNr
'*** End of Range Test ***
c = LoadString(3073, LogStr, cSLen): AdjustString LogStr
logWriteStr LogStr, gState.LogFileNr

If (ErrStr <> "") Then
  ReplaceText 2, s, ErrStr
Else
  ReplaceText 2, s, passed
End If


'3. Write back original property value to BACnet device.

pname = ""
eWriteRequest rxOrg, RxOrgLen 'insert Confirmed-Request-Header
If (PropId = 85) Then 'Present Value (Commandable Property)
  Select Case (Obj.object_type)
    Case 1, 4, 14, 2, 5: 'in objects AO, BO, MSO, AV, BV
      rxOrg = rxOrg & Chr$(&H49) & Chr$(16) 'add priority
      RxOrgLen = RxOrgLen + 2
  End Select
End If

e = outWriteProp(rxOrg, RxOrgLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxOrg, RxOrgLen
c = LoadString(3086, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr
eval = Asc(Mid$(rxOrg, 10, 1)) 'get enumeration Value
pname = Space$(40)
e = VTSAPIgetenumtable(pet, eval, 0, 0, pname) 'get enumeration text
AdjustString pname
logWriteStr Value & pname, gState.LogFileNr
If e <> eNOERROR Then
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 3, s, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 3, s, pname
End If


If (f <> eNOERROR) Then
  s = errors + ": " + s 'mark as failed
Else
  s = passed + ": " + s 'mark as passed
End If

wrtDoEnum = f
End Function

'This function is used to perform a WriteProperty Test for real properties.
'in:        pObj        pointer to generic_object
'           Obj         copy of generic_object
'           PropId      property identifier
'           i           position of real property in the PICS object (zero-based)
'out:       s           result string
'returns:   eNOERROR if passed, else eTESTFAILURE
Function wrtDoFloat(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String) As Integer

'1.1 Read original property value from BACnet device
'1.2 Decode ASN.1 string to real variable
'1.3 Arrange Test Values: first test value <> original test value
'2 for each test value:
'   2.1 Encode real test value to ASN.1 string
'   2.2 Write test value to BACnet Device
'   2.3 Read written test value from BACnet Device
'   2.4 Decode ASN.1 string to real variable
'   2.3 Compare 2.1 with 2.4 by using an epsilon-band
'3. Write back original value to BACnet Device

Dim invid As Integer, e As Integer, f As Integer
Dim c As Integer, LogStr As String, because As String
Dim Result As String, RStr As String, s1 As String
Dim OrgVal As Single, RxVal As Single
Dim ErrStr As String, passed As String, errors As String
Dim rxOrg As String, RxOrgLen As Integer
Dim txTest As String, txTestLen As Integer
Dim rxTest As String, rxTestLen As Integer
Dim Value As String
ReDim FltArr(0 To 2) As Single
Dim j As Integer, Temp As Single

c = LoadString(3009, passed, cSLen): passed = Mid$(passed, 1, c)
c = LoadString(3010, errors, cSLen): errors = Mid$(errors, 1, c)
c = LoadString(3088, Value, cSLen): Value = Mid$(Value, 1, c)
c = LoadString(3094, because, cSLen): because = Mid$(because, 1, c)
'Rd orig.: ~1, Wrt/Rd/Cmp test values: ~2, Wrt orig.: ~3
c = LoadString(3082, s, cSLen): s = Mid$(s, 1, c)

f = eNOERROR

'1.1 Read original property value from BACnet device

e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxOrg, RxOrgLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxOrg, RxOrgLen
c = LoadString(3085, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr

If e <> eNOERROR Then 'failed to read original property value
  logWriteStr because & ErrStr, gState.LogFileNr
  s = errors + ": " + s 'mark result as failed
  ReplaceText 1, s, ErrStr
  ReplaceText 2, s, "-"
  ReplaceText 3, s, "-"
  wrtDoFloat = eTESTFAILURE
  Exit Function
Else
  ReplaceText 1, s, passed
End If

'1.2 Decode ASN.1 string to real variable

e = DecodeReal(rxOrg, RxOrgLen, OrgVal)
logWriteStr Value & CStr(OrgVal), gState.LogFileNr

'1.3 Arrange Test Values: first test value <> original test value

FltArr(0) = gF(0): FltArr(1) = gF(1): FltArr(2) = gF(2)

If (OrgVal >= (FltArr(0) - epsilon)) And (OrgVal <= (FltArr(1) + epsilon)) Then 'switch
  Temp = FltArr(0)
  FltArr(0) = FltArr(1)
  FltArr(1) = Temp
End If


'*** Range Test: writing all test values: ***
c = LoadString(3089, LogStr, cSLen): AdjustString LogStr
logWriteStr "", gState.LogFileNr
logWriteStr LogStr, gState.LogFileNr
'Wrt Test Value: ~1, Rd Test Value: ~2, Cmp Test Value: ~3
c = LoadString(3081, s1, cSLen): s1 = Mid$(s1, 1, c)

'2. For each test value: ---

ErrStr = ""

For j = 0 To 2

  Result = s1 'Wrt Enum: ~1, Rd Enum: ~2, Cmp Enum: ~3
  
  '2.1 Encode real test value to ASN.1 string

  txTest = rxOrg
  txTestLen = RxOrgLen
  RStr = Space$(4)
  vbeReal RStr, FltArr(j) 'encode real
  Mid$(txTest, 10, 4) = RStr

  If (PropId = 85) Then 'Present Value (Commandable Property)
    Select Case (Obj.object_type)
      Case 1, 4, 14, 2, 5: 'in objects AO, BO, MSO, AV, BV
        txTest = txTest & Chr$(&H49) & Chr$(16) 'add priority
        txTestLen = txTestLen + 2
    End Select
  End If

  '2.2 Write test value to BACnet Device

  eWriteRequest txTest, txTestLen  'insert Confirmed-Request-Header
  e = outWriteProp(txTest, txTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader txTest, txTestLen
  c = LoadString(3083, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, txTest, txTestLen, gState.LogFileNr
  logWriteStr Value & CStr(FltArr(j)), gState.LogFileNr
  
  If (e <> eNOERROR) Then 'failed to write test value
    logWriteStr because & ErrStr, gState.LogFileNr
    f = eTESTFAILURE
    'Exit For
  End If
  ReplaceText 1, Result, CStr(FltArr(j))

  '2.3 Read written test value from BACnet Device

  e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxTest, rxTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader rxTest, rxTestLen
  c = LoadString(3084, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, rxTest, rxTestLen, gState.LogFileNr
  
  '2.4 Decode ASN.1 string to real variable
  c = DecodeReal(rxTest, rxTestLen, RxVal)
  logWriteStr Value & CStr(RxVal), gState.LogFileNr
  ReplaceText 2, Result, CStr(RxVal)
  
  If (e <> eNOERROR) Then
    logWriteStr because & ErrStr, gState.LogFileNr
    f = eTESTFAILURE
    'Exit For
  End If

  '2.5 Compare 2.1 with 2.4 by using an epsilon-band

  If (FltArr(j) < (RxVal - epsilon)) Or (FltArr(j) > (RxVal + epsilon)) Then 'not equal
    f = eTESTFAILURE
    'Property Values don't match.
    c = LoadString(7795, ErrStr, cSLen): ErrStr = Mid$(ErrStr, 1, c)
    ReplaceText 3, Result, ErrStr
  Else
    ReplaceText 3, Result, passed
  End If

  scrAddItem Result, PicsTestForm.ResultList
  logWriteStr "", gState.LogFileNr
  logWriteStr Result, gState.LogFileNr

Next j ' -----

logWriteStr "", gState.LogFileNr
'*** End of Range Test ***
c = LoadString(3073, LogStr, cSLen): AdjustString LogStr
logWriteStr LogStr, gState.LogFileNr


If (ErrStr <> "") Then
  ReplaceText 2, s, ErrStr
Else
  ReplaceText 2, s, passed
End If

'3. Write back original value to BACnet Device

eWriteRequest rxOrg, RxOrgLen 'insert Confirmed-Request-Header
If (PropId = 85) Then 'Present Value (Commandable Property)
  Select Case (Obj.object_type)
    Case 1, 4, 14, 2, 5: 'in objects AO, BO, MSO, AV, BV
      rxOrg = rxOrg & Chr$(&H49) & Chr$(16) 'add priority
      RxOrgLen = RxOrgLen + 2
  End Select
End If

e = outWriteProp(rxOrg, RxOrgLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxOrg, RxOrgLen
c = LoadString(3086, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr
logWriteStr Value & CStr(OrgVal), gState.LogFileNr
If e <> eNOERROR Then
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 3, s, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 3, s, passed
End If


If (f <> eNOERROR) Then
  s = errors + ": " + s 'mark as failed
Else
  s = passed + ": " + s 'mark as passed
End If
wrtDoFloat = f

End Function

'This function is used to perform a WriteProperty Test for certain Present_Value
'properties, which are required to be writable if Out_Of_Service is TRUE.
'in:        pObj        pointer to generic_object
'           Obj         copy of generic_object
'           PropId      property identifier
'           i           position of real property in the PICS object (zero-based)
'out:       s           result string
'returns:   eNOERROR if passed, else eTESTFAILURE
Function wrtDoPresVal(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String)

'1. Read original Out_Of_Service value
'2.1 If Out_Of_Service Is True Then GoTo 3
'2.2 If Out_Of_Service is writable
'      then 2.2.1 Write TRUE to Out_Of_Service
'      else 2.2.2 Exit
'3. Run appropriate Write Property Test according to datatype
'   of Present_Value.
'4. Write back original Out_Of_Service value


Dim e%, c%, PropName$, s1 As String
Dim passed$, errors$, Result$, ErrStr$
Dim rxApduOS$, rxLenOS%, f As Integer
Dim PropVal$, PropLen$, invid As Integer
Dim txApduOS$, txLenOS%, ApduRx$, rxLen%
Dim Msg As PVMessage, LogStr As String
Dim OSFlag As Integer, because As String

c = LoadString(3009, passed, cSLen): passed = Mid$(passed, 1, c)
c = LoadString(3010, errors, cSLen): errors = Mid$(errors, 1, c)
c = LoadString(3094, because, cSLen): because = Mid$(because, 1, c)
'rd/wrt/cmp Out_Of_Service: ~1, Present_Value Test: ~2, reset Out_Of_Service: ~3.
c = LoadString(3071, s, cSLen): AdjustString s

f = eNOERROR

'1. Read original Out_Of_Service value

e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, 81, rxApduOS, rxLenOS, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxApduOS, rxLenOS
'Read Property Service (InvId=~1): Out_Of_Service (received)
c = LoadString(3079, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxApduOS, rxLenOS, gState.LogFileNr

If e <> eNOERROR Then
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 1, s, ErrStr
  s = errors + ": " + s 'mark as failed
  wrtDoPresVal = eTESTFAILURE
  Exit Function
Else
  ReplaceText 1, s, passed
End If

'2.1 If Out_Of_Service Is True Then GoTo 3

e = DecodeBool(rxApduOS, rxLenOS, OSFlag)

If (OSFlag = 0) Then 'Out_Of_Service is false

  '2.2 If Out_Of_Service is writable
  '      then 2.2.1 Write TRUE to Out_Of_Service
  '      else 2.2.2 Exit

  txApduOS = rxApduOS
  txLenOS = rxLenOS
  Mid$(txApduOS, 9, 1) = Chr$(&H11) 'set Out_Of_Service TRUE
  eWriteRequest txApduOS, txLenOS  'insert Confirmed-Request-Header
  e = outWriteProp(txApduOS, txLenOS, ErrStr, invid) 'try to write
  logWriteStr "", gState.LogFileNr
  eCutHeader txApduOS, txLenOS 'remove apdu header
  'Write Property Service (InvId=~1): Out_Of_Service (received)
  c = LoadString(3080, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, txApduOS, txLenOS, gState.LogFileNr
  If e <> eNOERROR Then
    logWriteStr because & ErrStr, gState.LogFileNr
    ReplaceText 2, s, ErrStr
    f = eTESTFAILURE
    'wrtDoPresVal = eTESTFAILURE
    'Exit Function
  Else
    ReplaceText 2, s, passed
  End If

Else
  ReplaceText 2, s, "-"
End If 'Out_Of_Service is TRUE - now


'3. Run appropriate Write Property Test according to datatype
'   of Present_Value.

'*** Present_Value Test: writing test values ***
c = LoadString(3093, LogStr, cSLen): AdjustString LogStr
logWriteStr "", gState.LogFileNr
logWriteStr LogStr, gState.LogFileNr

e = wrtDoTest(pObj, Obj, PropId, i, s1)
If (e <> eNOERROR) Then
 ReplaceText 3, s, errors
 f = eTESTFAILURE
Else
 ReplaceText 3, s, passed
End If

logWriteStr "", gState.LogFileNr
logWriteStr s1, gState.LogFileNr
scrAddItem s1, PicsTestForm.ResultList

logWriteStr "", gState.LogFileNr
'*** End of Present_Value Test ***
c = LoadString(3072, LogStr, cSLen): AdjustString LogStr
logWriteStr LogStr, gState.LogFileNr

'4. Write back original Out_Of_Service value

If OSFlag = 0 Then 'we changed Out_Of_Service property
  eWriteRequest rxApduOS, rxLenOS 'insert Confirmed-Request-Header
  e = outWriteProp(rxApduOS, rxLenOS, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader rxApduOS, rxLenOS
  c = LoadString(3086, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, rxApduOS, rxLenOS, gState.LogFileNr
  If e <> eNOERROR Then
    logWriteStr because & ErrStr, gState.LogFileNr
    ReplaceText 4, s, ErrStr
    f = eTESTFAILURE
  Else
    ReplaceText 4, s, passed
  End If
Else
  ReplaceText 4, s, "-"
End If

If (f <> eNOERROR) Then
  s = errors + ": " + s 'mark as failed
Else
  s = passed + ": " + s 'mark as passed
End If

wrtDoPresVal = f

End Function

'This function is used to perform a WriteProperty Test for any properties
'(except: present_value, enum, float, unsigned).
'in:        pObj        pointer to generic_object
'           Obj         copy of generic_object
'           PropId      property identifier
'           i           position of real property in the PICS object (zero-based)
'out:       s           result string
'returns:   eNOERROR if passed, else eTESTFAILURE
Function wrtDoStandard(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String) As Integer

'1 Read original property value from BACnet device.
'2. for each test value:
'   2.1 Encode Test Value from database
'   2.2 Write test value to BACnet device
'   2.3 Read back test value from BACnet device
'   2.4  Compare 2.1 with 2.3
'3. Write back original property value to BACnet device.

Dim invid As Integer, e As Integer, f As Integer
Dim c As Integer, LogStr As String
Dim Result As String, s1 As String
Dim ErrStr As String, passed As String, errors As String
Dim rxOrg As String, RxOrgLen As Integer
Dim txTest As String, txTestLen As Integer
Dim rxTest As String, rxTestLen As Integer
Dim j As Integer, because As String
Dim Msg As PVMessage

c = LoadString(3094, because, cSLen): because = Mid$(because, 1, c)
c = LoadString(3009, passed, cSLen): passed = Mid$(passed, 1, c)
c = LoadString(3010, errors, cSLen): errors = Mid$(errors, 1, c)
'Rd orig.: ~1, Wrt/Rd/Cmp test values: ~2, Wrt orig.: ~3
c = LoadString(3082, s, cSLen): s = Mid$(s, 1, c)

f = eNOERROR

'1 Read original property value from BACnet device.

e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxOrg, RxOrgLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxOrg, RxOrgLen
'Read Property Service (InvId=~1): original value (received)
c = LoadString(3085, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr
If e <> eNOERROR Then 'failed to read original property value
  logWriteStr because & ErrStr, gState.LogFileNr
  s = errors + ": " + s 'mark result as failed
  ReplaceText 1, s, ErrStr
  ReplaceText 2, s, "-"
  ReplaceText 3, s, "-"
  wrtDoStandard = eTESTFAILURE
  Exit Function
Else
  ReplaceText 1, s, passed
End If


'*** Range Test: writing all test values: ***
c = LoadString(3089, LogStr, cSLen): AdjustString LogStr
logWriteStr "", gState.LogFileNr
logWriteStr LogStr, gState.LogFileNr
'Wrt Test Value: ~1, Rd Test Value: ~2, Cmp Test Value: ~3
c = LoadString(3081, s1, cSLen): s1 = Mid$(s1, 1, c)

'2. for each test value: 3 element, 1 elements, empty list

ErrStr = ""

For j = 0 To 2

  Result = s1 'Wrt Enum: ~1, Rd Enum: ~2, Cmp Enum: ~3

  '2.1 Encode Test Value from database
  
  Msg.Action = cRND_ENCODEPROP
  Msg.Obj = pObj
  Msg.PropId = PropId
  Msg.ArrayIndex = -1
  Msg.priority = -1
  Msg.TagNr = -1
  Msg.TestNr = j 'try test value nr: 0, 1, 2
  Msg.BufferLen = cApduLen
  txTest = Space(cApduLen)
  e = GetPropValue(txTest, Msg)
  txTestLen = Msg.BufferLen: txTest = Mid$(txTest, 1, txTestLen)
  
  '2.2 Write test value to BACnet device

  eWriteRequest txTest, txTestLen  'insert Confirmed-Request-Header
  e = outWriteProp(txTest, txTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader txTest, txTestLen
  'Write Property Service  (InvId=~1):  test value (sent)
  c = LoadString(3083, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, txTest, txTestLen, gState.LogFileNr
  If (e <> eNOERROR) Then 'failed to write test value
    logWriteStr because & ErrStr, gState.LogFileNr
    f = eTESTFAILURE
    'Exit For
    ReplaceText 1, Result, ErrStr
  Else
    ReplaceText 1, Result, passed
  End If

  '2.3 Read back test value from BACnet device
  
  e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxTest, rxTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader rxTest, rxTestLen
  'Read Property Service (InvId=~1): test value (received)
  c = LoadString(3084, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, rxTest, rxTestLen, gState.LogFileNr
  If (e <> eNOERROR) Then
    logWriteStr because & ErrStr, gState.LogFileNr
    f = eTESTFAILURE
    'Exit For
    ReplaceText 2, Result, ErrStr
  Else
    ReplaceText 2, Result, passed
  End If
  
  '2.4  Compare 2.1 with 2.3

  e = CompareApdu(txTest, txTestLen, rxTest, rxTestLen, ErrStr)
  If (e <> eNOERROR) Then
    f = eTESTFAILURE
    ReplaceText 3, Result, ErrStr
  Else
    ReplaceText 3, Result, passed
    'Exit For
  End If

  scrAddItem Result, PicsTestForm.ResultList
  logWriteStr "", gState.LogFileNr
  logWriteStr Result, gState.LogFileNr

Next j ' -----

logWriteStr "", gState.LogFileNr
'*** End of Range Test ***
c = LoadString(3073, LogStr, cSLen): AdjustString LogStr
logWriteStr LogStr, gState.LogFileNr

If (ErrStr <> "") Then
  ReplaceText 2, s, ErrStr
Else
  ReplaceText 2, s, passed
End If

'3. Write back original property value to BACnet device.

eWriteRequest rxOrg, RxOrgLen 'insert Confirmed-Request-Header
e = outWriteProp(rxOrg, RxOrgLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxOrg, RxOrgLen
'Write Property Service (InvId=~1): original value (sent)
c = LoadString(3086, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr
If e <> eNOERROR Then
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 3, s, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 3, s, passed
End If

If (f <> eNOERROR) Then
  s = errors + ": " + s 'mark as failed
Else
  s = passed + ": " + s 'mark as passed
End If

wrtDoStandard = f
End Function

'This function is used to execute the appropriate WriteProperty Test according to
'the property under test and the parsetype of that property.
'in:        pObj        pointer to generic_object
'           Obj         copy of generic_object
'           PropId      property identifier
'           i           position of real property in the PICS object (zero-based)
'out:       s           result string
'returns:   eNOERROR if passed, else eTESTFAILURE
Function wrtDoTest(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String) As Integer

Dim Msg As PVMessage
Dim PropVal As String, PropLen As Integer
Dim e As Integer

e = eNOERROR

Select Case Obj.object_type
  Case 11: 'group object
    If (PropId = 85) Then 'present_value
      e = wrtDoStandard(pObj, Obj, PropId, i, s)
      wrtDoTest = e
      Exit Function
    End If
End Select

'retrieve parse type from database:
Msg.Action = cGET_PARSE_TYPE
Msg.Obj = pObj
Msg.PropId = PropId
Msg.ArrayIndex = -1
Msg.priority = -1
Msg.TagNr = -1
Msg.BufferLen = 0

PropVal = ""
If GetPropValue(PropVal, Msg) = 0 Then
  e = eNOERROR
End If

'select test according to parse type:
Select Case Msg.pt
  Case flt:  'float parse type
    e = wrtDoFloat(pObj, Obj, PropId, i, s)
  Case et, ebool: 'write all enum
    e = wrtDoEnum(pObj, Obj, PropId, i, s)
  Case u16, u127, uw, ud, sw: 'unsigned parse types
    e = wrtDoUnsigned(pObj, Obj, PropId, i, s)
  Case Else: 'standard test
    e = wrtDoStandard(pObj, Obj, PropId, i, s)
End Select

wrtDoTest = e

End Function

'This function is used to execute: initial actions, a WriteProperty Test, exit actions.
'The WriteProperty test will be derived from the test listbox and the list of entrypoints.
'in:      State       current test state
'returns: eNOERROR if ok, else a Pics error code
Function wrtDoTestFrame(State As PicsState) As Integer
Dim e As Integer, c As Integer, i As Integer, TestFailed As Integer
Dim sname$, s$, PropInfo$, PropIndex As Integer
Dim Obj As generic_object, PropId As Long
Dim PropFound As Integer, ObjName As String

wrtDoTestFrame = eUNDEF
PropFound = False

Do Until PropFound Or State.EndTest

  Select Case State.SubTestNr
      
    Case 0: 'initial actions -----------------------------------------------------------------------
      GetEntryPoint 'continue with this test
      State.SubTestNr = State.SubTestNr + 1
      State.ActiveProperty = 0
      PicsTestForm!ContentList.Clear
      If State.ActiveObject = 0 Then wrtDoTestFrame = eNODATABASE: Exit Function
      CpyGenObj Obj, State.ActiveObject 'read the generic_object
      ObjName = Obj.object_name: AdjustString ObjName
      e = LoadString(3004, s, cSLen): AdjustString s
      ReplaceText 1, s, ObjName
      PicsTestForm!ContentList.AddItem s
      logWriteStr "", gState.LogFileNr
      logWriteStr String$(40, "*"), gState.LogFileNr
      logWriteStr s, gState.LogFileNr
      logWriteStr String$(40, "*"), gState.LogFileNr
  
    Case 1: 'read/write property test ---------------------------------------------------------------
      If State.ActiveObject = 0 Then wrtDoTestFrame = eNODATABASE: Exit Function
      CpyGenObj Obj, State.ActiveObject 'read the generic_object
      PropIndex = State.ActiveProperty  'current property
      sname = Space$(cSLen)
      PropFound = 0
      Do
        e = GetPropNameSupported(sname, PropIndex, Obj.object_type, Obj.propFlags, PropId)
        If (e = -1) Then 'no property
          Exit Do
        ElseIf (e > 0) Then
          PropFound = IsWritable(Obj, PropIndex)
        End If
        PropIndex = PropIndex + 1 'next property
      Loop Until (PropFound <> 0)

      ObjName = Obj.object_name: AdjustString ObjName
      If (State.ActiveProperty = 0) And (e = -1) Then 'this object has no writable property
        c = LoadString(3076, s, cSLen): AdjustString s
        ReplaceText 1, s, ObjName
        scrAddItem s, PicsTestForm!ResultList 'write result
        logWriteStr "", gState.LogFileNr
        logWriteStr s, gState.LogFileNr
      End If

      If (PropFound <> 0) Then 'property found

        State.ActiveProperty = PropIndex 'next property
        c = LoadString(3015, PropInfo, cSLen): PropInfo = Mid$(PropInfo, 1, c)
        sname = Space(cSLen)
        c = GetPropName(PropId, sname): sname = Mid$(sname, 1, c)
        ReplaceText 1, PropInfo, sname        'Property name
        ReplaceText 2, PropInfo, CStr(PropId) 'Property ID
        ReplaceText 3, PropInfo, ObjName 'Object name
        sname = Space(cSLen)
        c = GetObjType(Obj.object_type, sname): sname = Mid$(sname, 1, c)
        ReplaceText 4, PropInfo, sname
        ReplaceText 5, PropInfo, CStr(Obj.object_id And cObjInst)
        scrAddItem PropInfo, PicsTestForm!ResultList

        'run an individual test for this property/object:
        'present-value in Analog-Input, Binary-Input and Multistate-Input required to be writable
        'if out-of-service is set to TRUE.
        If (PropFound = 3) Then 'present-value is not writable
          e = wrtDoPresVal(State.ActiveObject, Obj, PropId, PropIndex - 1, s)
        Else 'property is writable
          e = wrtDoTest(State.ActiveObject, Obj, PropId, PropIndex - 1, s)
        End If

        If e = eTESTFAILURE Then 'this test failed
          State.StopAfterErr = State.StopAfterErr - 1 'decrement error counter
        End If
        scrAddItem s, PicsTestForm!ResultList 'write result
        logWriteStr "", gState.LogFileNr
        logWriteStr PropInfo, gState.LogFileNr
        logWriteStr s, gState.LogFileNr
        logWriteStr String$(10, "-"), gState.LogFileNr
        PicsTestForm!ResultList.ListIndex = PicsTestForm!ResultList.ListCount - 1 '   ***222
        PicsTestForm!ResultList.TopIndex = PicsTestForm!ResultList.ListIndex

      Else 'all properties tested, next state
        State.SubTestNr = State.SubTestNr + 1
      End If

    Case 2: 'exit actions (results) -----------------------------------------------------------------
      State.ActiveObject = FindGenObj(State.ActiveObject, 1) 'find next object
      If State.ActiveObject = 0 Then 'all objects tested
        State.EndTest = True 'that's all folks
      End If
      State.SubTestNr = 0 'first state
      On Error Resume Next                    '                                       ***222
      PicsTestForm!TestList.ListIndex = PicsTestForm!TestList.ListIndex + 2     '     ***222
      PicsTestForm!TestList.TopIndex = PicsTestForm!TestList.ListIndex
      On Error GoTo 0                         '                                       ***222
      PicsTestForm!ContentList.Clear

  End Select

Loop  'perform at least one test

wrtDoTestFrame = eNOERROR
End Function



'This function is used to perform a WriteProperty Test for unsigned properties.
'in:        pObj        pointer to generic_object
'           Obj         copy of generic_object
'           PropId      property identifier
'           i           position of real property in the PICS object (zero-based)
'out:       s           result string
'returns:   eNOERROR if passed, else eTESTFAILURE
Function wrtDoUnsigned(ByVal pObj As Long, Obj As generic_object, ByVal PropId As Long, ByVal i As Integer, s As String) As Integer

'1.1 Read original property value from BACnet device.
'1.2. arrange test values: first value <> original value
'2. for each test value:
'   2.1 Write test value to BACnet device
'   2.2 Read back value from BACnet device
'   2.3  Compare 2.1 with 2.2
'3. Write back original property value to BACnet device.

Dim e As Integer, f As Integer, invid As Integer, j As Integer
Dim c As Integer, eval As Integer
Dim ErrStr As String, passed As String, errors As String
Dim Result As String, s1 As String
Dim rxTest As String, rxTestLen As Integer
Dim txTest As String, txTestLen As Integer
Dim rxOrg As String, RxOrgLen As Integer
Dim LogStr As String, Value As String, because As String
ReDim u(0 To 2) As Integer

f = eNOERROR
c = LoadString(3009, passed, cSLen): passed = Mid$(passed, 1, c)
c = LoadString(3010, errors, cSLen): errors = Mid$(errors, 1, c)
c = LoadString(3088, Value, cSLen): Value = Mid$(Value, 1, c)
c = LoadString(3094, because, cSLen): because = Mid$(because, 1, c)

'1. Read original property value from BACnet device.

'Rd orig.: ~1, Wrt/Rd/Cmp test values: ~2, Wrt orig.: ~3
c = LoadString(3082, s, cSLen): s = Mid$(s, 1, c)
e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxOrg, RxOrgLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxOrg, RxOrgLen
c = LoadString(3085, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr

If e <> eNOERROR Then 'failed to read original property value
  logWriteStr because & ErrStr, gState.LogFileNr
  's = Errors + ": " + s 'mark result as failed
  ReplaceText 1, s, ErrStr
  ReplaceText 2, s, "-"
  ReplaceText 3, s, "-"
  wrtDoUnsigned = eTESTFAILURE
  Exit Function
Else
  eval = Asc(Mid$(rxOrg, 10, 1)) 'get unsigned Value
  logWriteStr Value & CStr(eval), gState.LogFileNr
  ReplaceText 1, s, passed
End If

'1.2. arrange test values: first value <> original value

u(0) = gU(0): u(1) = gU(1): u(2) = gU(2)
If (u(0) = eval) Then 'switch order
  u(0) = u(1): u(1) = eval
End If

txTest = rxOrg
txTestLen = RxOrgLen

If (PropId = 85) Then 'Present Value (Commandable Property)
  Select Case (Obj.object_type)
    Case 1, 4, 14, 2, 5: 'in objects AO, BO, MSO, AV, BV
      txTest = txTest & Chr$(&H49) & Chr$(16) 'add priority
      txTestLen = txTestLen + 2
  End Select
End If

'2. for every test value:

'*** Range Test: writing all test values: ***
c = LoadString(3089, LogStr, cSLen): AdjustString LogStr
logWriteStr "", gState.LogFileNr
logWriteStr LogStr, gState.LogFileNr

'Wrt Test Value: ~1, Rd Test Value: ~2, Cmp Test Value: ~3
c = LoadString(3081, s1, cSLen): s1 = Mid$(s1, 1, c)
ErrStr = ""

For j = 0 To 2

  Result = s1 'Wrt Test Value: ~1, Rd Test Value: ~2, Cmp Test Value: ~3
  
  '2.1 Write test value to BACnet device
  
  Mid$(txTest, 10, 1) = Chr(u(j)) 'set test value
  eWriteRequest txTest, txTestLen 'insert Confirmed-Request-Header
  e = outWriteProp(txTest, txTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader txTest, txTestLen
  'Write Property Service  (InvId=~1):  test value (sent)
  c = LoadString(3083, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, txTest, txTestLen, gState.LogFileNr
  logWriteStr Value & CStr(u(j)), gState.LogFileNr
  If (e <> eNOERROR) Then
    logWriteStr because & ErrStr, gState.LogFileNr
    ReplaceText 1, Result, ErrStr
    f = eTESTFAILURE
    'Exit For
  Else
    ReplaceText 1, Result, CStr(u(j))
  End If


'2.2 Read back value from BACnet device

  e = outReadProp(Obj.object_type, Obj.object_id And cObjInst, PropId, rxTest, rxTestLen, ErrStr, invid)
  logWriteStr "", gState.LogFileNr
  eCutHeader rxTest, rxTestLen
  'Read Property Service (InvId=~1): test value (received)
  c = LoadString(3084, LogStr, cSLen): AdjustString LogStr
  ReplaceText 1, LogStr, CStr(invid)
  logWriteApdu LogStr, rxTest, rxTestLen, gState.LogFileNr
  If (e <> eNOERROR) Then
    logWriteStr because & ErrStr, gState.LogFileNr
    ReplaceText 2, Result, ErrStr
    f = eTESTFAILURE
    eval = -1
  Else
    eval = Asc(Mid$(rxTest, 10, 1))
    ReplaceText 2, Result, CStr(eval)
    logWriteStr Value & CStr(eval), gState.LogFileNr
  End If

  
  '2.3  Compare 2.1 with 2.2

  If eval <> u(j) Then
    'Property Values don't match.
    c = LoadString(7795, ErrStr, cSLen): ErrStr = Mid$(ErrStr, 1, c)
    f = eTESTFAILURE
    'Exit For
    ReplaceText 3, Result, ErrStr
  Else
    ReplaceText 3, Result, passed
  End If

  scrAddItem Result, PicsTestForm.ResultList
  logWriteStr "", gState.LogFileNr
  logWriteStr Result, gState.LogFileNr

Next j

logWriteStr "", gState.LogFileNr
'*** End of Range Test ***
c = LoadString(3073, LogStr, cSLen): AdjustString LogStr
logWriteStr LogStr, gState.LogFileNr

If (ErrStr <> "") Then
  ReplaceText 2, s, ErrStr
Else
  ReplaceText 2, s, passed
End If


'3. Write back original property value to BACnet device.

eval = -1
eWriteRequest rxOrg, RxOrgLen 'insert Confirmed-Request-Header
If (PropId = 85) Then 'Present Value (Commandable Property)
  Select Case (Obj.object_type)
    Case 1, 4, 14, 2, 5: 'in objects AO, BO, MSO, AV, BV
      rxOrg = rxOrg & Chr$(&H49) & Chr$(16) 'add priority
      RxOrgLen = RxOrgLen + 2
  End Select
End If

e = outWriteProp(rxOrg, RxOrgLen, ErrStr, invid)
logWriteStr "", gState.LogFileNr
eCutHeader rxOrg, RxOrgLen
'Write Property Service (InvId=~1): original value (sent)
c = LoadString(3086, LogStr, cSLen): AdjustString LogStr
ReplaceText 1, LogStr, CStr(invid)
logWriteApdu LogStr, rxOrg, RxOrgLen, gState.LogFileNr
eval = Asc(Mid$(rxOrg, 10, 1)) 'get test Value
logWriteStr Value & CStr(eval), gState.LogFileNr
If e <> eNOERROR Then
  logWriteStr because & ErrStr, gState.LogFileNr
  ReplaceText 3, s, ErrStr
  f = eTESTFAILURE
Else
  ReplaceText 3, s, CStr(eval)
End If


If (f <> eNOERROR) Then
  s = errors + ": " + s 'mark as failed
Else
  s = passed + ": " + s 'mark as passed
End If

wrtDoUnsigned = f

End Function

'This function is used to fill the status lists for the
'ReadProperty Test, WriteProperty Test, Add/RemoveListElement Test
Sub wrtFillStatus()
Dim s As String, sname As String, e As Integer
Dim pObj As Long, Obj As generic_object
Dim Progress As ProgressType
Dim Header1 As String, Header2 As String

If gDB.DataBase = 0 Then Exit Sub

pObj = gDB.DataBase 'first object

e = LoadString(3011, Progress.Title, cSLen)
Progress.Title = Mid$(Progress.Title, 1, e)
e = LoadString(3012, Progress.Description, cSLen)
Progress.Description = Mid$(Progress.Description, 1, e)
Progress.Min = 0
Progress.max = GetNumberPicsObj()
Progress.pres = 0
ProgSetup Progress
  
e = LoadString(3002, Header1, cSLen): AdjustString Header1 '+++ Write Property Test: ~1 Object...
e = LoadString(3005, Header2, cSLen): AdjustString Header2 'Object Instance: ~1, Object Name: ~2

Do '--- for all objects in database ---
  DoEvents
  ProgTrigger Progress 'update progress form
  CpyGenObj Obj, pObj 'copy the generic_object into a VB structure
  
  sname = Space$(cSLen)
  e = GetEnumName(sname, Obj.object_type, cObject_Type, Obj.object_type)
  sname = Mid$(sname, 1, e)
  s = Header1
  ReplaceText 1, s, sname
  PicsTestForm!TestList.AddItem Space$(3) & s
  s = Header2
  ReplaceText 1, s, CStr(Obj.object_id And cObjInst)
  ReplaceText 2, s, Obj.object_name
  PicsTestForm!TestList.AddItem Space$(6) & s
  
  LLAddItem gTestLL(), StuffLong(pObj) 'link these list items to gTestLL
  pObj = FindGenObj(pObj, 1) 'next object
Loop Until pObj = 0

End Sub

