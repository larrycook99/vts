Attribute VB_Name = "PICSTOOL"
Option Explicit

Global Const cDLLName = "DUDAPI32.DLL"         'name of the dll

'Definitions for the INI file.
Global Const cVtsIniFile = "vtsdud2.ini"       'name of the INI file
Global Const cPicsSection = "Pics-Test"        'Section for PICS Testing in INI file
Global Const cPicsFileEntry = "DefPicsFile"    'Entry name for the default PICS file
Global Const cPicsLogFileEntry = "DefPicsLog"  'Entry name for default PICS log file
Global Const cSBSection = "SB-Test"            'Section for script based testing
Global Const cSBFileEntry = "DefScriptFile"    'Entry name for the default script file
Global Const cRouterSection = "VTSRoute"       'Section for the router addresses
Global Const cRouterNrEntry = "Number"         'Entry name for the number of elements in this section.
Global Const cAliasSection = "VTSAlias"        'Section for the alias names
Global Const cAliasNrEntry = "Number"          'Entry name for the number of elements in this section.

Global Const cObjInst = &H3FFFFF             'mask that way: ObjID & cObjInst == InstanceNr
Global Const cPropMin = 0                    '
Global Const cPropMax = 63                   'maximum number of properties in an object

Global Const cObject_Type = 79               'Property ID of BACnet object type property

'special functionality: router flag
Global Const rfNotSupported = 0
Global Const rfSupported = 1

'standard services
Global Const ssNotSupported = 0
Global Const ssInitiate = 1
Global Const ssExecute = 2
   
'standard objects
Global Const soNotSupported = 0
Global Const soSupported = 1
Global Const soCreateable = 2
Global Const soDeletable = 4

'qualifiers of the property flag
Global Const pfO = &H0                   'optional
Global Const pfR = &H1                   'readable
Global Const pfW = &H2                   'writeable
Global Const pfWoutofservice = &H4
Global Const pfWithGroup = &H8
Global Const pfNotWithGroup1 = &H10
Global Const pfNotWithGroup2 = &H20
Global Const pfWithService = &H40
Global Const pfAtLeast1 = &H80
Global Const pfIsArray = &H100
Global Const pfIsCommandable = &H200
   
'Resource IDs for literal strings
Global Const cFGTableId = 4711 'Resource ID for BACnetFunctionalGroups
Global Const cFGTableNr = 13   'Number of BACnetFunctionalGroups

Global Const cSpecFuncTableId = 4724 'Resource ID for Special Functionality
Global Const cSpecFuncTableNr = 4    'Number Special Functionality

Global Const cApplServTableId = 4728   'Resource ID BACnetStandardServices
Global Const cApplServTableNr = 35     'Number BACnetStandardServices

Global Const cObjTypesTableId = 4763 'Resource ID BACnetStandardObjects
Global Const cObjTypesTableNr = 18   'Number BACnetStandardObjects

Global Const cDLLOptTableId = 4781 'Resource ID DataLinkLayerOptions
Global Const cDLLOptTableNr = 15   'Number DataLinkLayerOptions

Global Const cPropNameTableId = 4796 'Resource ID of Property-Names
Global Const cPropNameTableNr = 124  'Number of Property-Names

'error code constants for PicsError()
Global Const eFILENOTFOUND = 53       'file not found
Global Const eNOERROR = 0             'no error
Global Const eUNDEF = 32767           'undefined error
Global Const eLOAD_DB = 32766         'load database failed
Global Const eUNLOAD_DB = 32765       'unload database failed
Global Const eNOPICSFILE = 32764      'no PICS file name selected
Global Const eEOL = 32763             'end of list (INI)
Global Const eOPENINI = 32762         'failed to open INI file
Global Const eDLLNOTFOUND = 32761     'no DUDAPI32.DLL
Global Const eNODATABASE = 32760      'no database gDB.DataBase
Global Const eREADPROP = 32759        'read property error
Global Const eWRITEPROP = 32758       'write property error
Global Const eAPDUFAILURE = 32757     'APDU not OK
Global Const eSENDFAILURE = 32756     'failed to send apdu
Global Const eAPDUEMPTY = 32755       'APDU buffer already consumed
Global Const eTIMEOUT = 32754         'no APDU received within time limit
Global Const eTOOMANYERR = 32753      'too many errors occured
Global Const eTESTFAILURE = 32752     'PICS test failed
Global Const eENCODEFAILURE = 32751   'failed to encode apdu
Global Const eOPENLOGFILE = 32750     'could not open log file
Global Const eNAKPDU = 32749          'negative ack received
Global Const eNOLOGFILE = 32748       'no log file selected
Global Const eTIMERERR = 32747        'no timer
Global Const eLISTSERVICE = 32746     'List Element Services not supported

'MsgBox() constants ---
Global Const MB_YESNO = 4                    'yes-no-button
Global Const MB_ICONEXCLAMATION = 48         'exclamation
Global Const MB_ICONINFORMATION = 64         'i-symbol
'Icon constants ---
Global Const cIDI_ASTERISK = 32516           'i-symbol

   
' type conventions
'    word, uint  converted to Integer
'    octet, byte converted to String * 1
'    dword       converted to Long


Type PicsDB
  VendorName As String * 64                     '
  ProductName As String * 128                   '
  ProductModelNumber As String * 64             '
  ProductDescription As String * 128            '
  BACnetConformanceClass As Integer             '
  MaxAPDUSize As Integer                        '
  SegmentedRequestWindow As String * 1          '
  SegmentedResponseWindow As String * 1         '
  RouterFunctions As Integer                    '
  BACnetFunctionalGroups As Long                '
  BACnetCharSets As String * 1
  BACnetStandardServices As String * 35         '
  BACnetStandardObjects As String * 18          '
  DataLinkLayerOptions As Integer               '
  MSTPmasterBaudRates(0 To 15) As Long          '
  MSTPslaveBaudRates(0 To 15) As Long           '
  PTP232BaudRates(0 To 15) As Long              '
  PTPmodemBaudRates(0 To 15) As Long            '
  PTPAutoBaud(0 To 1) As Long                   '
  DataBase As Long                              ' Pointer: generic_object far*
End Type

Type generic_object
  next As Long                         ' Pointer: generic_object far*
  object_id As Long                    '
  object_name As String * 32           '
  object_type As Integer               ' (word) enum BACnetObjectType
  Description As String * 132          '
  propFlags As String * 64             ' up to 64 properties of parser flags
End Type


Global Const cSETPROP = 0              'modify property value (show Dialog box)
Global Const cENCODEPROP = 1           'encode ObjId, PropId, [ArrayI,] PropVal [,Prio] from database
Global Const cRND_ENCODEPROP = 2       'encode ObjId, PropId, [ArrayI,] random PropVal [,Prio]
Global Const cRND_ASN_1_ANY_PROP = 3   'encode any test value
Global Const cASN_1_ANY_PROP = 4       'encode Property Value (only) from database
Global Const cGET_PARSE_TYPE = 5       'get the parse type of any property
Global Const cSET_TRUE = 6             'encode application tagged TRUE
Global Const cSET_FALSE = 7            'encode application tagged FALSE


'Type PVMessage
'  'in ---
'  hw As Integer               'parent window
'  Action As Long              'action
'  Obj As Long                 'pointer to object
'  PropId As Long              'property id
'  ArrayIndex As Integer       'Property array index (optional)
'  priority As Integer         'Priority (for WriteProperty Service, optional)
'  TagNr As Integer            'Tag number to encode (optional)
'  TestNr As Integer           'which Test Value
'  'in/out ---
'  BufferLen As Integer        'length of encoded data
'  'out ---
'  pv As Long                  'pointer to property value
'  pt As Integer               'parse type
'  UsedBits As Integer         'number of used bits
'  ActNum As Integer           'number of action arrays
'End Type
Type PVMessage
  'in ---
  hw As Long                  'parent window
  Action As Long              'action
  Obj As Long                 'pointer to object
  PropId As Long              'property id
  ArrayIndex As Long          'Property array index (optional)
  priority As Long            'Priority (for WriteProperty Service, optional)
  TagNr As Long               'Tag number to encode (optional)
  TestNr As Long              'which Test Value
  'in/out ---
  BufferLen As Long           'length of encoded data
  'out ---
  pv As Long                  'pointer to property value
  pt As Long                  'parse type
  UsedBits As Long            'number of used bits
  ActNum As Long              'number of action arrays
End Type


'global variables ---

Global gDefPicsFile As String           'default PICS File
Global gDefPicsTitle As String          'default PICS Filename (without filepath)
Global gDefPicsLogFile As String        'default PICS Logfile
Global gDefPicsLogTitle As String       'default PICS Logname (without filepath)
Global gDBloaded As Integer             'Flag: database loaded = true
Global gDB As PicsDB                    'PICS database
Global gObjectId As Long                'most recently parsed Object (Script Tests)
Global gDBErrc As Integer               'error code
Global gPropNameTable() As String       'table of property-names







'This Function appends a string to a particular section in the INI-file.
'in:        Section         name of the section
'           Listentry       string to enter in this section
'           Filename        name of the INI-file
'returns:   True (-1) if ok, False (0) else
Function AppendListSection(Section As String, Listentry As String, FileName As String) As Integer
Dim sec As String, f As Integer, tmp As Integer, Found As Integer, ln As String
Dim FName As String, TempName As String

AppendListSection = False
Found = False
sec = "[" + Section + "]"
FName = VTSdir + FileName
TempName = VTSdir + "ini.tmp"

f = FreeFile
On Error GoTo OpenAppendError
Open FName For Input As f
On Error GoTo 0

tmp = FreeFile
Open TempName For Output As tmp

Do While ((Not EOF(f)) And (Not Found)) 'look for our section
  Line Input #f, ln
  Print #tmp, ln
  ln = LTrim$(ln) 'remove leading spaces
  If Mid$(ln, 1, 1) = "[" Then 'a section is found
    ln = RTrim$(ln) 'remove trailing spaces
    If StrComp(ln, sec, 1) = 0 Then 'our section?
      Found = True
      AppendListSection = True
      Print #tmp, Listentry 'enter Listentry
      Do While (Not EOF(f))
        Line Input #f, ln
        Print #tmp, ln
      Loop
    End If 'our section
  End If 'a section
Loop

Close #f, #tmp
If Found Then
  Kill FName 'delete old file
  Name TempName As FName 'rename temp file
End If
Exit Function

OpenAppendError:
  Dim Action, e As Integer
  If Err = eFILENOTFOUND Then e = eOPENINI Else e = Err
  Action = PicsError(e, FileName)
  If Action = 0 Then 'resume
    e = CheckListSection(Section, FileName)
    Resume 'try again
  Else
    Exit Function
  End If

End Function

'This Function is used to check, whether the INI File exists, and whether the list section exists.
'If the INI File and the list section are absent, then this function will create them.
'in:        Section     name of the section
'           FileName    name of the INI File
'returns:   True (-1) if ok, False (0) else
Function CheckListSection(Section As String, FileName As String) As Integer
Dim f As Integer, ln As String, Found As Integer, sec As String
Dim FName As String

CheckListSection = False
f = FreeFile
FName = VTSdir + FileName
On Error GoTo CheckListSectionError
Open FName For Input As f
On Error GoTo 0

sec = "[" + Section + "]"
Found = False

Do While ((Not EOF(f)) And (Not Found)) 'look for our section
  Line Input #f, ln
  ln = LTrim$(ln) 'remove leading spaces
  If Mid$(ln, 1, 1) = "[" Then 'a section is found
    ln = RTrim$(ln) 'remove trailing spaces
    If StrComp(ln, sec, 1) = 0 Then 'our section?
      Found = True
    End If 'our section
  End If 'a section
Loop

Close #f
If (Not Found) Then 'create section
f = FreeFile
Open FName For Append As #f
Print #f, sec
Close #f
End If
CheckListSection = True
Exit Function

CheckListSectionError:
  Dim g As Integer
  If Err = eFILENOTFOUND Then
    g = FreeFile
    Open FName For Output As g
    Close #g
    Resume 'try again
  Else
    Exit Function
  End If


End Function

'This function is used to remove all entries of a particular
'section in an INI-file.
'in:        Section         name of the section
'           Filename        name of the INI-File
'returns:   True (-1) if ok, False (0) else
Function ClearListSection(Section As String, FileName As String) As Integer
Dim FName As String, orig As String, sec As String, ln As String
Dim f As Integer, tmp As Integer, InSection As Integer
Dim Action, e As Integer, TempName As String

ClearListSection = False
InSection = False
sec = "[" + Section + "]"
FName = VTSdir + FileName
TempName = VTSdir + "ini.tmp"

f = FreeFile
On Error GoTo OpenClearListSectionError
Open FName For Input As f
On Error GoTo 0

tmp = FreeFile
Open TempName For Output As tmp

Do While Not EOF(f) 'look for our section
  Line Input #f, orig
  ln = LTrim$(orig) 'remove leading spaces
  If Mid$(ln, 1, 1) = "[" Then 'a section is found
    ln = RTrim$(ln) 'remove trailing spaces
    If StrComp(ln, sec, 1) = 0 Then 'our section?
      Print #tmp, orig 'section name
      InSection = True 'skip listentries for this section
    Else
      InSection = False 'not in our section
    End If 'our section
  End If 'a section
  If Not InSection Then Print #tmp, orig 'copy to temp file
Loop

Close #f, #tmp
Kill FName 'delete old file
Name TempName As FName 'rename temp file
ClearListSection = True
Exit Function

OpenClearListSectionError:
  If Err = eFILENOTFOUND Then e = eOPENINI Else e = Err
  Action = PicsError(e, FileName)
  If Action = 0 Then 'resume
    e = CheckListSection(Section, FileName)
    Resume 'try again
  Else
    Exit Function
  End If

End Function

'This function is used to remove the filepath from a
'filename.
'in:    FileName    filename
'Out:   Title       filename without path
Sub CutFilePath(Title As String, ByVal FileName As String)
Dim e As Integer
Title = FileName
Do 'get filename without filepath
  e = InStr(Title, "\")
  If e = 0 Then Exit Do
  Title = Right$(Title, Len(Title) - e)
Loop

End Sub

'This function is used to retrieve the number of elements within a list section of
'an INI-File.
'in:        Section     name of the section
'           FileName    name of the INI-File
'returns: Number of entries if OK, -1 else
Function GetNumberListSection(Section As String, FileName As String) As Integer
Dim FName As String, sec As String, ln As String
Dim f As Integer, Num As Integer, InSection As Integer
Dim Action, e As Integer

GetNumberListSection = -1
InSection = False
sec = "[" + Section + "]"
FName = VTSdir + FileName

f = FreeFile
On Error GoTo OpenGetNumberListSectionError
Open FName For Input As f
On Error GoTo 0

Num = -1
Do While Not EOF(f) 'look for our section
  Line Input #f, ln
  ln = LTrim$(RTrim$(ln)) 'remove leading/trailing spaces
  If Mid$(ln, 1, 1) = "[" Then 'a section is found
    If StrComp(ln, sec, 1) = 0 Then 'our section?
      InSection = True 'count listentries for this section
    Else
      InSection = False 'not in our section
    End If 'our section
  End If 'a section
  If InSection And Len(ln) > 0 Then Num = Num + 1
Loop

Close #f
GetNumberListSection = Num
Exit Function

OpenGetNumberListSectionError:
  If Err = eFILENOTFOUND Then e = eOPENINI Else e = Err
  Action = PicsError(e, FileName)
  If Action = 0 Then 'resume
    e = CheckListSection(Section, FileName)
    Resume 'try again
  Else
    Exit Function
  End If

End Function

'The Function loads the BACnet objects defined in the PICS file.
Function LoadObjDB(tryFile As String) As Integer
Dim e As Integer, tryDB As PicsDB, errc As Integer

LoadObjDB = eLOAD_DB 'no new database loaded

If Len(tryFile) = 0 Then 'no PICS file selected
  LoadObjDB = eNOPICSFILE
  Exit Function
End If
ReadTextPICS tryFile, tryDB, errc 'try to read database
If errc <> 0 Then
  If tryDB.DataBase <> 0 Then DeletePICSObject (tryDB.DataBase) 'delete anyway
  LoadObjDB = eLOAD_DB
  Exit Function
End If
If gDBloaded Then
  e = UnloadObjDB() 'unload old database
End If

gDefPicsFile = tryFile
gDefPicsTitle = gDefPicsFile
Do 'get filename without filepath
  e = InStr(gDefPicsTitle, "\")
  If e = 0 Then Exit Do
  gDefPicsTitle = Right$(gDefPicsTitle, Len(gDefPicsTitle) - e)
Loop

gDB = tryDB
gDBloaded = True
LoadObjDB = eNOERROR
End Function

'This function is used to load a textstring from the resource file.
'in:    ID      id of textstring
'       Size    size of receive buffer
'out:   s       receive buffer
Function LoadString(ByVal id As Integer, s As String, ByVal Size As Integer) As Integer
s = Space$(Size)
LoadString = LoadStringRes(id, s, Size)
End Function

Sub PicsDeInit()
Dim e As Integer
WriteINIFile
e = UnloadObjDB() 'unload database if loaded
CloseDudapi
End Sub

' Return Value
'   0: Resume
'   1: Resume Next
'   2: Unrecoverable error
'   3: Unrecognized  error
Function PicsError(ByVal eCode As Integer, Param As String) As Integer
Dim Msg As String, c As Integer
Dim MsgType As Integer, Response As Integer

MsgType = MB_ICONEXCLAMATION
Select Case eCode
  'unrecoverable errors ---
  Case eUNDEF
    c = LoadString(7777, Msg, cSLen) 'An unknown error occured!
  Case eUNLOAD_DB
    c = LoadString(7778, Msg, cSLen) 'Error while unloading object database!
  Case eLOAD_DB
    c = LoadString(7779, Msg, cSLen) 'Error while loading object database!
  Case eNOPICSFILE
    c = LoadString(7780, Msg, cSLen) 'No appropriate PICS File selected!
  Case eNODATABASE
    c = LoadString(7798, Msg, cSLen) 'No appropriate PICS File selected!
  Case eNOERROR
    c = LoadString(7781, Msg, cSLen) 'No error occured!
  Case eNOLOGFILE
    c = LoadString(7800, Msg, cSLen): AdjustString Msg
  Case eLISTSERVICE
    c = LoadString(3091, Msg, cSLen): AdjustString Msg
  'other errors ---
  Case eDLLNOTFOUND
   'you can not avoid these literal strings!
    Msg = "DLL not found: " & Param & " Try again?"
    MsgType = MB_YESNO + MB_ICONEXCLAMATION
  Case eOPENINI
    c = LoadString(7783, Msg, cSLen) 'File not found: ~1 Create?
    Msg = RTrim$(Msg)
    ReplaceText 1, Msg, Param
    MsgType = MB_YESNO + MB_ICONEXCLAMATION
  Case eFILENOTFOUND
    c = LoadString(7782, Msg, cSLen) 'File not found: ~1 Try again?
    Msg = RTrim$(Msg)
    ReplaceText 1, Msg, Param
    MsgType = MB_YESNO + MB_ICONEXCLAMATION
  Case eTOOMANYERR
    c = LoadString(7796, Msg, cSLen)
    ReplaceText 1, Msg, Param
    MsgType = MB_YESNO + MB_ICONEXCLAMATION
  Case eOPENLOGFILE
    c = LoadString(7797, Msg, cSLen)
    ReplaceText 1, Msg, Param
    MsgType = MB_YESNO + MB_ICONEXCLAMATION
  Case Else
    PicsError = 3
    Error eCode
    Exit Function
End Select

Msg = Trim$(Msg)
Response = MsgBox(Msg, MsgType, "PICS Test Error")
Select Case Response
  Case 1, 4, 6 'OK, Yes, Retry Buttons
    PicsError = 0 'Resume
  Case 5 ' Ignore Button
    PicsError = 1 'Resume Next
  Case 2, 3, 7 'Cancel, No, Abort Button
    PicsError = 2
  Case Else
    PicsError = 3
  End Select
End Function

Sub PicsInit()
Dim i As Integer, s As String, c As Integer
  

'call a function from the DLL
On Error GoTo DLLNotFound
  InitDudapi
On Error GoTo 0

'load property name table
ReDim gPropNameTable(0 To 123) As String
For i = 0 To 123
  c = LoadString(4796 + i, gPropNameTable(i), cSLen)
  gPropNameTable(i) = Mid$(gPropNameTable(i), 1, c)
Next i
  
gPicsInterval = 2000           'timeout for receiving PICS-Test APDUs (ms)
gStopAfterErr = 80             'stop testing after max. errors occured
gPicsFilter = 0                'don't filter PICS apdu
gDefPicsLogFile = ""
gDefPicsLogTitle = ""
gDefPicsTitle = ""
gDBloaded = False              'database not loaded

'Test Values for Write Property Test

gU(0) = 1
gU(1) = 2
gU(2) = 3

gF(0) = 0.1
gF(1) = 49.9
gF(2) = 99.9

ReadINIFile


Exit Sub

DLLNotFound:
  Dim Action As Integer
  Action = PicsError(eDLLNOTFOUND, cDLLName)
  If Action = 0 Then
    Resume 'Try again
  Else
    ExitVTS 'abort program
  End If

End Sub














Sub ReadINIFile()
Dim c As Integer
c = CheckListSection(cRouterSection, cVtsIniFile)
c = CheckListSection(cAliasSection, cVtsIniFile)
gDefPicsFile = Space$(cSLen)
c = GetPrivateProfileString(cPicsSection, cPicsFileEntry, "Def.Tpi", gDefPicsFile, cSLen, cVtsIniFile)
DefScriptFile = Space$(cSLen)
c = GetPrivateProfileString(cSBSection, cSBFileEntry, "Def.Vts", DefScriptFile, cSLen, cVtsIniFile)
gDefPicsLogFile = Space$(cSLen)
c = GetPrivateProfileString(cPicsSection, cPicsLogFileEntry, "Def.Log", gDefPicsLogFile, cSLen, cVtsIniFile)
CutFilePath gDefPicsLogTitle, gDefPicsLogFile
AdjustString gDefPicsLogFile
AdjustString gDefPicsLogTitle
End Sub


'This function is used to read a particular listentry of an INI-File. If the INI-File
'is not found, then the INI-File will be created.
'in:        Section         name of the section
'           index           position of the listentry in this section
'           Filename        name of the INI-File
'out:       Listentry       string variable to contain the listentry
'returns:   True (-1) if ok, False (0) else
Function ReadListSection(Section As String, ByVal Index As Integer, Listentry As String, FileName As String) As Integer
Dim f As Integer, ln As String, Finish As Integer, sec As String

ReadListSection = False
f = FreeFile
On Error GoTo OpenReadError
Open VTSdir + FileName For Input As f
On Error GoTo 0

Finish = False
sec = "[" + Section + "]"
Do While ((Not EOF(f)) And (Not Finish)) 'look for our section
  Line Input #f, ln
  ln = LTrim$(ln) 'remove leading spaces
  If Mid$(ln, 1, 1) = "[" Then 'a section is found
    ln = RTrim$(ln) 'remove trailing spaces
    If StrComp(ln, sec, 1) = 0 Then 'our section?

      Do 'read entries in our section
        If EOF(f) Then Finish = True: Exit Do 'not found
        Line Input #f, ln 'read next line
        ln = LTrim$(RTrim$(ln)) 'remove leading/trailing spaces
        If Len(ln) <> 0 Then    'non empty line in section
          If Mid$(ln, 1, 1) = "[" Then Finish = True: Exit Do 'start of new section
          Index = Index - 1 'decrement position in list
          If Index < 0 Then
            ReadListSection = 1 'entry found
            Listentry = ln
            Finish = True
            Exit Do 'listentry is found
          End If
        End If 'non empty line
      Loop

    End If 'our section
  End If 'a section
Loop

Close #f
Exit Function

OpenReadError:
  Dim Action, e As Integer
  If Err = eFILENOTFOUND Then e = eOPENINI Else e = Err
  Action = PicsError(e, FileName)
  If Action = 0 Then 'resume
    e = CheckListSection(Section, FileName)
    Resume 'try again
  Else
    Exit Function
  End If

End Function

'This function is used to replace a marker in a string by a replacement string.
'in:        i           replacement number
'           stxt        this string contains marker, e.g. "...~1..~2.."
'           replace     replacement string
Sub ReplaceText(ByVal i As Integer, stxt As String, replace As String)
i = InStr(stxt, "~" & CStr(i))
If i = 0 Then Exit Sub
stxt = Mid$(stxt, 1, i - 1) & replace & Mid$(stxt, i + 2)
End Sub

Function UnloadObjDB() As Integer
If gDBloaded Then 'unload only if database already loaded
  If gDB.DataBase <> 0 Then MyDeletePICSObject gDB.DataBase
  gDBloaded = False
  gDB.DataBase = 0
End If
UnloadObjDB = eNOERROR
End Function

Sub WriteINIFile()
Dim e As Integer
e = WritePrivateProfileString(cPicsSection, cPicsFileEntry, gDefPicsFile, cVtsIniFile)
e = WritePrivateProfileString(cSBSection, cSBFileEntry, DefScriptFile, cVtsIniFile)
e = WritePrivateProfileString(cPicsSection, cPicsLogFileEntry, gDefPicsLogFile, cVtsIniFile)
End Sub

