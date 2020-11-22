// BakRestoreExecutor.cpp: implementation of the BakRestoreExecutor class.
// Jingbo Gao, Sep 20 2004
// John Hartman 2011: introduce the new concept of "functions" to replace
// repeating large numbers of code lines...
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSDoc.h"
#include "BACnet.hpp"
#include "VTSBackupRestoreDlg.h"
#include "ScriptExecutor.h"
#include "BakRestoreExecutor.h"
#include "PI.h"

namespace PICS {
#include "db.h"
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"
}

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SEGMENTED_APDU_HEADER_SIZE  6  //service choice + window size + sequence number + invokeID + APDU size + APDU tag
#define NPDU_HEADER_BUFF         10 //Allow this amount of space for NPDU
#define DEFAULT_BUFF_SIZE        0x400

#define BACNET_OBJECTTYPE_FILE   0x02800000

// global defines
BakRestoreExecutor      gBakRestoreExecutor;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BakRestoreExecutor::BakRestoreExecutor()
: SendReceiveExecutor("Backup and Restore Tests"),
  m_strBackupFileName(""),
  m_strPassword(""), 
  m_funToExe(ALL_BACKUP_RESTORE)
{
}

BakRestoreExecutor::~BakRestoreExecutor()
{
}

void BakRestoreExecutor::ExecuteTest()
{
   CSingleLock lock(&m_cs);
   lock.Lock();

   if (m_execState != execIdle) {
      TRACE0( "Error: invalid executor state\n" );
      return;
   }

   VTSDocPtr pVTSDoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
   VTSPorts* pPorts = pVTSDoc->GetPorts();
   VTSNames* pNames = pVTSDoc->GetNames();

   VTSBackupRestoreDlg dlg(*pNames, *pPorts);
   if ( dlg.DoModal() == IDOK )
   {
      for ( int i = 0; i < pPorts->GetSize(); i++ )
      {
         if ( ((VTSPort *) pPorts->GetAt(i))->m_strName.CompareNoCase(dlg.m_strPort) == 0 )
         {
            m_pPort = (VTSPort *) pPorts->GetAt(i);
            break;
         }
      }
      m_pName = pNames->GetAt( pNames->FindIndex(dlg.m_strDevice) );
      m_funToExe = (FunctionToExecute)dlg.m_nFunction;
      if (dlg.m_strDevObjInst.IsEmpty())
      {
         m_nDeviceObjInst = DEVICE_OBJ_INST_NUM_UNSET;
      }
      else
      {
         m_nDeviceObjInst = atoi(dlg.m_strDevObjInst);
      }
      m_strBackupFileName = dlg.m_strBackupFileName;
      m_strPassword = dlg.m_strPassword;
      m_Delay = dlg.Delay;
      m_Backup_Timeout = dlg.timeout;
   }
   else
   {
      return;
   }

   StartTest();
   lock.Unlock();
}

void BakRestoreExecutor::ProcessTask()
{
   CSingleLock lock(&m_cs);
   lock.Lock();

   BOOL bSuccess = TRUE;
   try {
      if ( !m_pPort || !m_pName || m_strBackupFileName.IsEmpty() )
      {
         throw("VTSPort, IUT or backfile not specified");
      }

      m_IUTAddr = m_pName->m_bacnetaddr;
      if (m_IUTAddr.addrType != localStationAddr &&
         m_IUTAddr.addrType != remoteStationAddr)
      {
         throw("The IUT's address must be local station or remote station");
      }

      m_execState = execRunning;

      m_pOutputDlg->BeginTestProcess();

      FindRouterAddress();

      switch(m_funToExe) {
      case ALL_BACKUP_RESTORE:
         DoBackupTest();
         DoRestoreTest();
         DoAuxiliaryTest();
         Msg("All Backup and Restore tests have been sucessfully completed!");
         break;
      case FULL_BACKUP_RESTORE:
         DoBackupTest();
         DoRestoreTest();
         Msg("Full Backup and Restore tests have been sucessfully completed!");
         break;
      case BACKUP_ONLY:
         DoBackupTest();
         Msg("Backup tests have been sucessfully completed!");
         break;
      case RESTORE_ONLY:
         DoRestoreTest();
         Msg("Restore tests have been sucessfully completed!");
         break;
      case AUXILIARY_BACKUP_RESTORE:
         DoAuxiliaryTest();
         Msg("Auxiliary backup and restore tests have been sucessfully completed!");
         break;
      default:
         break;
      }
   }
   catch (const char *errMsg) {
      bSuccess = FALSE;
      Msg(errMsg);
   }
   catch (...) {
      bSuccess = FALSE;
      Msg("Decoding error! May have received incorrect packet!");
   }

   m_execState = execIdle;
   if (bSuccess)
   {
      m_pOutputDlg->OutMessage("The entire test process has been successful completed");
   }
   else if (m_bUserCancelled)
   {
      m_bUserCancelled = FALSE;  // reset
      m_pOutputDlg->OutMessage("", TRUE);    //begin a new line
      m_pOutputDlg->OutMessage("Canceled by user");
   }
   else
   {
      m_pOutputDlg->OutMessage("Failed",TRUE);  // begin a new line
      m_pOutputDlg->OutMessage("Error occured during the test");
   }

   m_pOutputDlg->EndTestProcess();
   lock.Unlock();
}


void BakRestoreExecutor::DoBackupTest()
{
   m_pOutputDlg->OutMessage("Begin Backup process...");
   m_pOutputDlg->OutMessage("Create backup index file...", FALSE);
   CString strIndexFileName(m_strBackupFileName + ".backupindex");
   WIN32_FIND_DATA fd;
   HANDLE   hFind = ::FindFirstFile(strIndexFileName, &fd);
   if (hFind != INVALID_HANDLE_VALUE)
   {
      if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
         char msg[200];
         sprintf(msg, "The file %s already exists, do you want to overwrite it?", fd.cFileName);
         if (IDNO == AfxMessageBox(msg, MB_YESNO|MB_ICONSTOP))
         {
            throw("Backup file already exists");
         }
      }
   }
   CFile backupIndexFile(strIndexFileName, CFile::modeWrite|CFile::modeCreate);
   m_pOutputDlg->OutMessage("OK");

   BACnetEnumerated propID;
   AnyValue propValue;
   char buffer[300];

   // Throws failure
   GetDeviceMaxAPDU();
   BACnetObjectIdentifier devObjID(OBJ_DEVICE, m_nDeviceObjInst);
   BACnetUnsigned maxAPDULenAccepted(m_maxAPDULen);

   BACnetUnsigned databaseRevision;
   BACnetTimeStamp   lastRestoreTime;
   ReadDatabaseRevAndRestoreTime( devObjID, databaseRevision, lastRestoreTime );

   // write to the .backupindex file
   CString chEnc;
   devObjID.Encode(chEnc, BACnetEncodeable::FMT_PLAIN);
   int nStart = sprintf(buffer, "(%s), ", (LPCTSTR)chEnc);

   maxAPDULenAccepted.Encode(chEnc, BACnetEncodeable::FMT_PLAIN);
   nStart +=  sprintf(buffer+nStart, "%s, ", (LPCTSTR)chEnc);

   lastRestoreTime.pbacnetTypedValue->Encode(chEnc, BACnetEncodeable::FMT_PLAIN);
   nStart += sprintf(buffer+nStart, "%s\n", (LPCTSTR)chEnc);

   backupIndexFile.Write(buffer, strlen(buffer));

   m_pOutputDlg->OutMessage("Write to the Device/Backup_Failure_Timeout...", FALSE);
   // use WriteProperty to write to the Device/Backup_Failure_Timeout property
   propID.m_enumValue = PICS::BACKUP_FAILURE_TIMEOUT;
   BACnetUnsigned backupFailureTimeout(m_Backup_Timeout);
   propValue.SetObject(&backupFailureTimeout);
   if (!SendExpectWriteProperty(devObjID, propID, propValue))
   {
      throw("Write to the Backup_Failure_Timeout failed!");
   }
   m_pOutputDlg->OutMessage("OK");

   // Transmit a ReinitialzeDevice service
   m_pOutputDlg->OutMessage("Transmit a ReinitializeDevice service to start backup...", FALSE);
   if (!SendExpectReinitialize(STARTBACKUP, m_strPassword))
   {
      throw("Result(+) not received in response to the ReinitializeDevice");
   }
   m_pOutputDlg->OutMessage("OK");

   // Wait for backup preparation
   Delay( m_Delay );

   // Read array index zero of the Configuration_File property of the Device Object,
   // and store it in a variable named NUM_FILES.
   m_pOutputDlg->OutMessage("Read array size of the ConfigureFile Property...", FALSE);
   propID.m_enumValue = PICS::CONFIGURATION_FILES;
   BACnetUnsigned numFiles;
   propValue.SetObject(&numFiles);
   if (!SendExpectReadProperty(devObjID, propID, propValue, 0))
   {
      throw("Failed to read CONFIGURATION_FILES");
   }
   if (numFiles.uintValue <= 0)
   {
      throw("NUM_FILES is not greater than zero");
   }
   m_pOutputDlg->OutMessage("OK");

   unsigned long i = 0;
   for(; i < numFiles.uintValue; i++)
   {
      // Read array index I of the Configuration_Files array.
      m_pOutputDlg->OutMessage("Read array index I of the Configure_Files array...", FALSE);
      propID.m_enumValue = PICS::CONFIGURATION_FILES;
      BACnetObjectIdentifier  fileID;
      propValue.SetObject(&fileID);
      if (!SendExpectReadProperty(devObjID, propID, propValue, i+1))
      {
         throw("Failed to read CONFIGURATION_FILES");
      }
      // to check if it is a file object
      if (fileID.GetObjectType() != OBJ_FILE)
      {
         throw("File object expected here!");
      }
      UINT nFileInstance = fileID.objID & 0x003fffff;
      m_pOutputDlg->OutMessage("OK");

      m_pOutputDlg->OutMessage("Use ReadProperty to get the File_Access_Method...", FALSE);
      propID.m_enumValue = PICS::FILE_ACCESS_METHOD;
      BACnetEnumerated fileAccessMethod;
      propValue.SetObject(&fileAccessMethod);
      if (!SendExpectReadProperty(fileID, propID, propValue))
      {
         throw("Failed to read File/File_Access_Method");
      }
      m_pOutputDlg->OutMessage("OK");

      m_pOutputDlg->OutMessage("Use ReadProperty to get the Object_Name...", FALSE);
      propID.m_enumValue = PICS::OBJECT_NAME;
      BACnetCharacterString   objName;
      propValue.SetObject(&objName);
      if (!SendExpectReadProperty(fileID, propID, propValue))
      {
         throw("Failed to read File/Object_Name");
      }
      m_pOutputDlg->OutMessage("OK");

      BACnetUnsigned recordCount;
      if (fileAccessMethod.m_enumValue == PICS::RECORD_ACCESS)
      {
         m_pOutputDlg->OutMessage("Use ReadProperty to get the Record_Count...", FALSE);
         propID.m_enumValue = PICS::RECORD_COUNT;
         propValue.SetObject(&recordCount);
         if (!SendExpectReadProperty(fileID, propID, propValue))
         {
            throw("Failed to read File/Record_Count");
         }
         m_pOutputDlg->OutMessage("OK");
      }

      m_pOutputDlg->OutMessage("Use ReadProperty to get the File_Size...", FALSE);
      propID.m_enumValue = PICS::FILE_SIZE;
      BACnetUnsigned fileSize;
      propValue.SetObject(&fileSize);
      if (!SendExpectReadProperty(fileID, propID, propValue))
      {
         throw("Failed to read File/File_Size");
      }
      m_pOutputDlg->OutMessage("OK");

      // creata a new empty disk file
      m_pOutputDlg->OutMessage("Create an empty backup data file...", FALSE);
      CString strDataFileName;
      strDataFileName.Format("%s.file%d.backupdata", m_strBackupFileName, nFileInstance);
      m_pOutputDlg->OutMessage("OK");

      // write a new line to the .backupindex file
      // write to the .backupindex file

      m_pOutputDlg->OutMessage("Write to backup index file...", FALSE);
      BACnetUnsigned(nFileInstance).Encode(chEnc, BACnetEncodeable::FMT_PLAIN);

      nStart = sprintf(buffer, "%s, ", (LPCTSTR)chEnc);
      nStart +=  sprintf(buffer+nStart, "%s, ", NetworkSniffer::BAC_STRTAB_BACnetFileAccessMethod.m_pStrings[fileAccessMethod.m_enumValue]);

      fileSize.Encode(chEnc, BACnetEncodeable::FMT_PLAIN);
      nStart += sprintf(buffer+nStart, "%s, ", (LPCTSTR)chEnc);
      // add record count to index file if and only if RECORD_ACCESS type.
      if (fileAccessMethod.m_enumValue == PICS::RECORD_ACCESS)
      {
         recordCount.Encode(chEnc, BACnetEncodeable::FMT_PLAIN);
         nStart += sprintf(buffer+nStart, "%s, ", (LPCTSTR)chEnc);
      }
      objName.Encode(chEnc, BACnetEncodeable::FMT_PLAIN);
      nStart += sprintf(buffer+nStart, "%s, ", (LPCTSTR)chEnc);
      nStart += sprintf(buffer+nStart, "%s\n", (LPCSTR)strDataFileName);
      backupIndexFile.Write(buffer, strlen(buffer));

      m_pOutputDlg->OutMessage("OK");

      if (fileAccessMethod.m_enumValue == PICS::RECORD_ACCESS)
      {
         BACnetBoolean  endofFile(fileSize.uintValue == 0 ? TRUE : FALSE);
         UINT nX = 0;
         BACnetOctetString fileRecordData;

         while (!endofFile.boolValue)
         {
            // use AtomicReadFile (Record Access) to read one record from the file
            // in the IUT
            m_pOutputDlg->OutMessage("Use AtomicReadFile to read octets from the file in the IUT...", FALSE);
            BACnetInteger  fileStartRecord(nX);
            if (!SendExpectAtomicReadFile_Record(fileID, fileStartRecord, fileRecordData, endofFile))
            {
               throw("AtomicReadFile of IUT's configuration file failed");
            }
            m_pOutputDlg->OutMessage("OK");
            m_pOutputDlg->OutMessage("Write File record data to the backup data file...", FALSE);

            // Create new file for each record read
            CString strDataFileName_record;
            strDataFileName_record.Format("%s.record%d", strDataFileName, nX);
            CFile backupDataFile_record(strDataFileName_record, CFile::modeWrite|CFile::modeCreate);

            backupDataFile_record.Write(fileRecordData.strBuff, fileRecordData.strLen);
            backupDataFile_record.Close();

            m_pOutputDlg->OutMessage("OK");
            nX++;
         }
      }
      else
      {
         // now open the new file for stream access
         CFile backupDataFile(strDataFileName, CFile::modeWrite|CFile::modeCreate);

         // calculate the Maximun Requested Octet Count
         UINT nMROC;
         if (m_pPort->m_pdevice)
         {
            UINT  nM1 = m_pPort->m_pdevice->m_nMaxAPDUSize;
            UINT  nM2 = maxAPDULenAccepted.uintValue;
            nMROC = min(nM1, nM2) - 21;
         }
         else
         {  // if this port does not bind to a device
            nMROC = maxAPDULenAccepted.uintValue - 21;
         }

         UINT nX = 0;
         while (nX < fileSize.uintValue)
         {
            UINT nROC = min(nMROC, (fileSize.uintValue - nX));
            // Use AtomicReadFile to read octets from the file in the IUT
            m_pOutputDlg->OutMessage("Use AtomicReadFile to read octets from the file in the IUT...", FALSE);
            BACnetOctetString fileData;
            BACnetInteger fileStartPosition(nX);
            BACnetUnsigned ROC(nROC);
            if (!SendExpectAtomicReadFile_Stream(fileID, fileStartPosition, ROC, fileData))
            {
               throw("Failed to read data from the file in the IUT");
            }
            m_pOutputDlg->OutMessage("OK");

            // append the octets to the .backupdata disk file
            m_pOutputDlg->OutMessage("Append the octets to the backup index file...", FALSE);
            backupDataFile.Write(fileData.strBuff, fileData.strLen);
            m_pOutputDlg->OutMessage("OK");
            nX += nROC;
         }
         backupDataFile.Close();
      }
   }

   backupIndexFile.Close();
   // close the .backupindex file
   // Transmit a ReinitializeDevice service
   m_pOutputDlg->OutMessage("Transmit a ReinitalizeDevice service...", FALSE);
   if (!SendExpectReinitialize(ENDBACKUP, m_strPassword))
   {
      throw("Cannot end backup process");
   }
   m_pOutputDlg->OutMessage("OK");
}


void BakRestoreExecutor::DoRestoreTest()
{
   m_pOutputDlg->OutMessage("Begin restore process...");
   m_pOutputDlg->OutMessage("Open backup index file...", FALSE);
   CString strIndexFileName = m_strBackupFileName;
   strIndexFileName += ".backupindex";
   CStdioFile backupIndexFile;
   if (!backupIndexFile.Open(strIndexFileName, CFile::modeRead))
   {
      CString err;
       err.Format("Cannot open %s", (LPCTSTR)strIndexFileName);
      throw((LPCTSTR)err);
   }
   m_pOutputDlg->OutMessage("OK");

   BACnetEnumerated propID;
   AnyValue propValue;
   CString strText="";

   // Throws failure
   GetDeviceMaxAPDU();
   BACnetObjectIdentifier devObjID(OBJ_DEVICE, m_nDeviceObjInst);
   BACnetUnsigned maxAPDULenAccepted(m_maxAPDULen);

   // Get database revision and last restore time before the restore
   BACnetUnsigned databaseRevision;
   BACnetTimeStamp   lastRestoreTime;
   ReadDatabaseRevAndRestoreTime( devObjID, databaseRevision, lastRestoreTime );

   // Read first line of the .backupindex file, do not used during the test
   // ((DEVICE, 1170000)), 1024, {(Wednesday,December/31/1969), 18:00:00.00}
   m_pOutputDlg->OutMessage("Read the first line of the .backupindex file", FALSE);
   backupIndexFile.ReadString(strText);
   m_pOutputDlg->OutMessage("OK");

   // Transmit a ReinitialzeDevice service
   m_pOutputDlg->OutMessage("Transmit a ReinitializeDevice service to start restore...", FALSE);
   if (!SendExpectReinitialize(STARTRESTORE, m_strPassword))
   {
      throw("Result(+) was not received in response to the ReinitializeDevice");
   }
   m_pOutputDlg->OutMessage("OK");

   // Wait for restore preparation
   Delay( m_Delay );

   // Get the Device's list of objects
   // Throws failure
   BACnetArrayOf<BACnetObjectIdentifier>  objList;
   GetObjectList( objList );

   // 1, STREAM-ACCESS, 1843263, "config_file1", C:\Documents and Settings\laeol\My Documents\BACnetTesting\backup1.file1.backupdata
   while (backupIndexFile.ReadString(strText))
   {
      // parse the string
      // (Modified 19 May 2010 to allow spaces in the path and file name)
      CString  strToken;

      int nPos1 = strText.Find(',');  //After instance number
      strToken = strText.Left(nPos1++);
      strToken.TrimLeft();
      UINT nFileInstance = atoi(strToken);

      int nPos2 = strText.Find(',', nPos1);   //After Access type
      strToken = strText.Mid(nPos1, nPos2 - nPos1);
      strToken.TrimLeft();
      strToken.MakeUpper();
      BACnetEnumerated fileAccessMethod;
      if (strToken == "RECORD-ACCESS")
      {
         fileAccessMethod.m_enumValue = PICS::RECORD_ACCESS;
      }
      else if (strToken == "STREAM-ACCESS")
      {
         fileAccessMethod.m_enumValue = PICS::STREAM_ACCESS;
      }
      else
      {
         throw("Incorrect file access method in backup data file.\n"
              "Only record_access and stream_access");
      }
      nPos1 = nPos2 + 1;

      nPos2 = strText.Find(',', nPos1);       //After file size
      strToken = strText.Mid(nPos1, nPos2 - nPos1);
      strToken.TrimLeft();
      BACnetUnsigned fileSize(atoi(strToken));
        nPos1 = nPos2 + 1;

      BACnetUnsigned recordCount;
      if (fileAccessMethod.m_enumValue == PICS::RECORD_ACCESS)
      {
         nPos2 = strText.Find(',', nPos1);       //After record_count
         strToken = strText.Mid(nPos1, nPos2 - nPos1);
         strToken.TrimLeft();
         recordCount.uintValue = atoi(strToken);
         nPos1 = nPos2 + 1;
      }

        nPos2 = strText.Find(',', nPos1);       //After object name
        strToken = strText.Mid(nPos1, nPos2 - nPos1);
      strToken.TrimLeft();
        BACnetCharacterString objName;
        objName.Decode( strToken );

      strToken = strText.Right(strText.GetLength() - nPos2 - 1);  //Data file is remaining string
      strToken.TrimLeft();
      CString strDataFileName(strToken);

      int nCount = objList.GetItemCount();
      BOOL bFind = FALSE;
      
      //Find the file object of the correct instance
      for (int i = 0; i < nCount; i++)
      {
         UINT nObjectType = objList.GetElement(i)->GetObjectType();
         UINT nInstanceNum = objList.GetElement(i)->GetObjectInstance();
         
         if (nObjectType == BACNET_OBJECTTYPE_FILE && nFileInstance == nInstanceNum)
         {
            bFind = TRUE;
            break;
         }
      }

      BACnetObjectIdentifier  fileID(OBJ_FILE, nFileInstance);
      bFind = true;
      if (bFind)
      {
         if (fileAccessMethod.m_enumValue == PICS::RECORD_ACCESS)
         {
            m_pOutputDlg->OutMessage("Use ReadProperty to read the Record_Count property...", FALSE);
            propID.m_enumValue = PICS::RECORD_COUNT;
            BACnetUnsigned recordCountIUT;
            propValue.SetObject(&recordCountIUT);
            if (!SendExpectReadProperty(fileID, propID, propValue))
            {
               throw("Unable to read File/RECORD_COUNT");
            }
            m_pOutputDlg->OutMessage("OK");
            if (recordCount.uintValue != recordCountIUT.uintValue)
            {
               // use write_property to set the Record_Count to zero
               m_pOutputDlg->OutMessage("Use WriteProperty to set the Record_Count to zero...", FALSE);
               propID.m_enumValue = PICS::RECORD_COUNT;
               BACnetUnsigned temp(0);
               propValue.SetObject(&temp);
               if (!SendExpectWriteProperty(fileID, propID, propValue))
               {
                  throw("Cannot write File/Record_Count to zero");
               }
               m_pOutputDlg->OutMessage("OK");
            }
         }
         else
         {
            m_pOutputDlg->OutMessage("Use ReadProperty to read the File_Size property...", FALSE);
            propID.m_enumValue = PICS::FILE_SIZE;
            BACnetUnsigned fileSizeIUT;
            propValue.SetObject(&fileSizeIUT);
            if (!SendExpectReadProperty(fileID, propID, propValue))
            {
               throw("Unable to read File/FILE_SIZE");
            }
            m_pOutputDlg->OutMessage("OK");
            if ((fileSizeIUT.uintValue != 0) && (fileSize.uintValue != fileSizeIUT.uintValue))
            {
               // use write_property to set the File_Size to zero
               m_pOutputDlg->OutMessage("Use WriteProperty to set the File_Size to zero...", FALSE);
               propID.m_enumValue = PICS::FILE_SIZE;
               BACnetUnsigned temp(0);
               propValue.SetObject(&temp);
               if (!SendExpectWriteProperty(fileID, propID, propValue))
               {
                  throw("Cannot write File/File_Size to zero");
               }
               m_pOutputDlg->OutMessage("OK");

            }
         }
      }
      else
      {  // use the CreateObject service to create (File, FILE_INSTANCE) in the IUT
         m_pOutputDlg->OutMessage("Use the CreateObject service to create (File, File_Instance)"
            "in the IUT...", FALSE);
         BACnetSequenceOf<PropertyValue>  listOfInitialValues;
         listOfInitialValues.AddElement(
            &PropertyValue(BACnetEnumerated(PICS::OBJECT_NAME), objName)
            );
         // Note: The JCI version of fixes removed the following two elements from
         //       the create object list.  Does this make it more interoperable to do
         //       this?  I chose to leave these values in for now.  11/21/2006 LJT
         //listOfInitialValues.AddElement(
         // &PropertyValue(BACnetEnumerated(PICS::FILE_ACCESS_METHOD), fileAccessMethod)
         // );
         //listOfInitialValues.AddElement(
         // &PropertyValue(BACnetEnumerated(PICS::FILE_SIZE), fileSize)
         // );

         if (!SendExpectCreatObject(fileID, listOfInitialValues)) 
         {
            throw("Unable to create File object in the IUT");
         }
         m_pOutputDlg->OutMessage("OK");
      }

      m_pOutputDlg->OutMessage("Open the backup data file...", FALSE);

      if (fileAccessMethod.m_enumValue == PICS::RECORD_ACCESS)
      {
         CStdioFile backupDataFile_record;

         UINT nX = 0;

         CString strDataFileName_record;
         for ( UINT rcount = 0; rcount < recordCount.uintValue; rcount++)
         {
            strDataFileName_record.Format("%s.record%d", strDataFileName, nX);
            if (!backupDataFile_record.Open(strDataFileName_record, CFile::modeRead | CFile::typeBinary))
            {
               CString err;
               err.Format("Cannot open %s", (LPCTSTR)strDataFileName_record);
               throw((LPCTSTR)err);
            }
            m_pOutputDlg->OutMessage("OK");
   
            // We cannot assume that record size < maxAPDU so give a rounded buffsize greater than the file size
            UINT fSize = (UINT)backupDataFile_record.GetLength();
            UINT nMWOC = fSize + (DEFAULT_BUFF_SIZE - (fSize % DEFAULT_BUFF_SIZE) );
            
            BYTE* pBuffer = new BYTE[nMWOC];
            int len_read = backupDataFile_record.Read(pBuffer, nMWOC);

            m_pOutputDlg->OutMessage("Use AtomicWriteFile to write one record to (File, File_Instance)"
                  "in the IUT...", FALSE);
            BACnetInteger  fileStartRecord(nX);
            BACnetOctetString fileRecordData(pBuffer, len_read);
            if (!SendExpectAtomicWriteFile_Record(fileID, fileStartRecord, fileRecordData))
            {
               throw("Unable to write one record to FILE in the IUT");
            }
            m_pOutputDlg->OutMessage("OK");
//          delete pBuff;
            delete[] pBuffer;
            nX++;

            backupDataFile_record.Close();
         }
      }
      else
      {
         CFile backupDataFile;
         if (!backupDataFile.Open(strDataFileName, CFile::modeRead))
         {
            CString err;
            err.Format("Cannot open %s", (LPCTSTR)strDataFileName);
            throw((LPCTSTR)err);
         }
         m_pOutputDlg->OutMessage("OK");
         // calculate the Maximun Requested Octet Count
         UINT nMWOC;
         if (m_pPort->m_pdevice)
         {
            UINT  nM1 = m_pPort->m_pdevice->m_nMaxAPDUSize;
            UINT  nM2 = maxAPDULenAccepted.uintValue;
            nMWOC = min(nM1, nM2) - 21;
         }
         else
         {  // if this port does not bind to a device
            nMWOC = maxAPDULenAccepted.uintValue - 21;
         }

         UINT nX = 0;
         BYTE* pBuffer = new BYTE[nMWOC];
         ULONGLONG dwBytesRemaining = backupDataFile.GetLength();
         while (dwBytesRemaining)
         {
            m_pOutputDlg->OutMessage("Use AtomicWriteFile to write octets to (File, File_Instance)"
               "in the IUT...", FALSE);
            backupDataFile.Seek(nX, CFile::begin);
            UINT nWOC = backupDataFile.Read(pBuffer, nMWOC);
            BACnetInteger fileStartPosition(nX);
            BACnetOctetString fileData(pBuffer, nWOC);
            if (!SendExpectAtomicWriteFile_Stream(fileID, fileStartPosition, fileData))
            {
               throw("Unable to write File Data to IUT");
            }
            m_pOutputDlg->OutMessage("OK");
            nX += nWOC;
            dwBytesRemaining -= (ULONGLONG)nWOC;
         }
         delete[] pBuffer;
      }
   }

   // close the .backupindex file
   m_pOutputDlg->OutMessage("Transmit a ReinitializeDevice service to complete the restore...", FALSE);
   if (!SendExpectReinitialize(ENDRESTORE, m_strPassword))
   {
      throw("Cannot end restore process");
   }
   m_pOutputDlg->OutMessage("OK");

   // Delay to let IUT recover.
   Delay( m_Delay );

   // TODO: should check system-status and/or backup-restore-state

   // Get database revision and last restore time before the restore
   ReadDatabaseRevAndRestoreTime( devObjID, databaseRevision, lastRestoreTime );
}


void BakRestoreExecutor::DoAuxiliaryTest()
{
   // Throws failure
   GetDeviceMaxAPDU();
   BACnetObjectIdentifier devObjID(OBJ_DEVICE, m_nDeviceObjInst);
   BACnetUnsigned maxAPDULenAccepted(m_maxAPDULen);

// DoAuxiliaryTest_1();
   DoAuxiliaryTest_2();
// DoAuxiliaryTest_3();
   DoAuxiliaryTest_4();
   DoAuxiliaryTest_5(devObjID);
   DoAuxiliaryTest_6(devObjID);
   DoAuxiliaryTest_7();
   DoAuxiliaryTest_8();
   DoAuxiliaryTest_9(devObjID);
   DoAuxiliaryTest_10(devObjID);
   DoAuxiliaryTest_11(devObjID);
   DoAuxiliaryTest_12(devObjID);
}

/*
//Initiaing a backup procedure while already performing a backup procedure
void BakRestoreExecutor::DoAuxiliaryTest_1()
{
   m_pOutputDlg->OutMessage("Auxiliary Test 1");
   if (!SendExpectReinitialize(STARTBACKUP, m_strPassword))
   {
      throw("Cannot start Backup procedure in the IUT");
   }

   // try to find out another active port, which bind to different network number and
   // different MAC address. Use this port to simulate a different client.
   VTSDocPtr pVTSdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
   VTSPorts * pPorts = pVTSdoc == NULL ? NULL : pVTSdoc->GetPorts();
   VTSPort * pPort;
   BOOL bFind = FALSE;

   for ( int i = 0; i < pPorts->GetSize(); i++ )
   {
      pPort = ((VTSPort *) pPorts->GetAt(i));
      if (pPort != m_pPort
         && pPort->IsEnabled()
         && (pPort->m_nNet >= 0)
         && (pPort->m_nPortType == m_pPort->m_nPortType))
      {
         bFind = TRUE;
         break;
      }
   }
   if (!bFind)
   {  // can not find another active port, this test needs a different devcie
      Msg("Warning: Cannot find a different active port. This test has been skipped!");
      return;
   }

   BACnetAddress IUTAddrTemp(m_IUTAddr);
   BACnetAddress routerAddrTemp(m_routerAddr);
   VTSPortPtr  pPortTemp = m_pPort;

   m_pPort = pPort;
   SetIUTAddress(pPort, m_pName);
   FindRouterAddress();

   BACnetEnumerated  errorClass;
   BACnetEnumerated  errorCode;
   if (!SendExpectReinitializeNeg(STARTBACKUP, errorClass, errorCode))
   {
      throw("Expected BACnetErrorAPDU");
   }

   if (errorClass.m_enumValue != ErrorClass::DEVICE)
   {
      throw("Wrong Error Class");
   }

   if (errorCode.m_enumValue != ErrorCode::CONFIGURATION_IN_PROGRESS)
   {
      throw("Wrong Error Code");
   }

   // rollback
   m_pPort = pPortTemp;
   m_IUTAddr = IUTAddrTemp;
   m_routerAddr = routerAddrTemp;
}*/


// Initiating a backup procedure while already performing a restore procedure
void BakRestoreExecutor::DoAuxiliaryTest_2()
{
   m_pOutputDlg->OutMessage("Auxiliary Test 2");
   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start restore...", FALSE);
   if (!SendExpectReinitialize(STARTRESTORE, m_strPassword))
   {
      throw("Unable to initialize a restore procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   // Wait for restore prep
   Delay( m_Delay );

   m_pOutputDlg->OutMessage("Transmit Reinitialize_Request to start backup...", FALSE);
   BACnetEnumerated  errorClass;
   BACnetEnumerated  errorCode;
   if (!SendExpectReinitializeNeg(STARTBACKUP, errorClass, errorCode))
   {
      throw("Expected BACnetErrorAPDU");
   }
   if (errorClass.m_enumValue != ErrorClass::DEVICE)
   {
      throw("Wrong Error Class");
   }
   if (errorCode.m_enumValue != ErrorCode::CONFIGURATION_IN_PROGRESS)
   {
      throw("Wrong Error Code");
   }
   m_pOutputDlg->OutMessage("OK: got expected error");

   m_pOutputDlg->OutMessage("Transmit ABORTRESTORE...", FALSE);
   if (!SendExpectReinitialize(ABORTRESTORE, m_strPassword))
   {
      throw("Unable to abort a restore procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   // Delay to let IUT recover.
   // TODO: should check system-status and/or backup-restore-state
   Delay( m_Delay );
}

/*
// Initiating a restore procedure while already performing a backup procedure
void BakRestoreExecutor::DoAuxiliaryTest_3()
{
   m_pOutputDlg->OutMessage("Auxiliary Test 3");
   if (!SendExpectReinitialize(STARTBACKUP, m_strPassword))
   {
      throw("Cannot start Backup procedure in the IUT");
   }

   // try to find out another active port, which bind to different network number and
   // different MAC address. Use this port to simulate a different client.
   VTSDocPtr pVTSdoc = (VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace();
   VTSPorts * pPorts = pVTSdoc == NULL ? NULL : pVTSdoc->GetPorts();
   VTSPort * pPort;
   BOOL bFind = FALSE;

   for ( int i = 0; i < pPorts->GetSize(); i++ )
   {
      pPort = ((VTSPort *) pPorts->GetAt(i));
      if (pPort != m_pPort
         && pPort->IsEnabled()
         && (pPort->m_nNet >= 0)
         && (pPort->m_nPortType == m_pPort->m_nPortType))
      {
         bFind = TRUE;
         break;
      }
   }
   if (!bFind)
   {  // can not find another active port, this test needs a different devcie
      Msg("Warning: Cannot find a different active port. This test has been skipped!");
      return;
   }

   BACnetAddress IUTAddrTemp(m_IUTAddr);
   BACnetAddress routerAddrTemp(m_routerAddr);
   VTSPortPtr  pPortTemp = m_pPort;

   m_pPort = pPort;
   SetIUTAddress(pPort, m_pName);
   FindRouterAddress();

   BACnetEnumerated  errorClass;
   BACnetEnumerated  errorCode;
   if (!SendExpectReinitializeNeg(STARTRESTORE, errorClass, errorCode))
   {
      throw("Expected BACnetErrorAPDU");
   }

   if (errorClass.m_enumValue != ErrorClass::DEVICE)
   {
      throw("Wrong Error Class");
   }

   if (errorCode.m_enumValue != ErrorCode::CONFIGURATION_IN_PROGRESS)
   {
      throw("Wrong Error Code");
   }

   // rollback
   m_pPort = pPortTemp;
   m_IUTAddr = IUTAddrTemp;
   m_routerAddr = routerAddrTemp;
}
*/

// Initiating a restore procedure while already performing a restore procedure
void BakRestoreExecutor::DoAuxiliaryTest_4()
{
   m_pOutputDlg->OutMessage("Auxiliary Test 4");
   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start restore...", FALSE);
   if (!SendExpectReinitialize(STARTRESTORE, m_strPassword))
   {
      throw("Unable to initialize a restore procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   // Wait for restore prep
   Delay( m_Delay );

   m_pOutputDlg->OutMessage("Transmit Reinitalize-Request to start restore...", FALSE);
   BACnetEnumerated  errorClass;
   BACnetEnumerated  errorCode;

   if (!SendExpectReinitializeNeg(STARTRESTORE, errorClass, errorCode))
   {
      throw("Expected BACnetErrorAPDU");
   }
   if (errorClass.m_enumValue != ErrorClass::DEVICE)
   {
      throw("Wrong Error Class");
   }
   if (errorCode.m_enumValue != ErrorCode::CONFIGURATION_IN_PROGRESS)
   {
      throw("Wrong Error Code");
   }
   m_pOutputDlg->OutMessage("OK: got expected error");

   m_pOutputDlg->OutMessage("Transmit ABORTRESTORE...", FALSE);
   if (!SendExpectReinitialize(ABORTRESTORE, m_strPassword))
   {
      throw("Unable to abort a restore procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   // Delay to let IUT recover.
   // TODO: should check system-status and/or backup-restore-state
   Delay( m_Delay );
}

// Ending backup and restore procedure via timeout
void BakRestoreExecutor::DoAuxiliaryTest_5(BACnetObjectIdentifier& devObjID)
{
   m_pOutputDlg->OutMessage("Auxiliary Test 5");
   m_pOutputDlg->OutMessage("Transmit WriteProperty-Request to write 30 to Backup_Failure_Timeout...", FALSE);
   BACnetEnumerated propID(PICS::BACKUP_FAILURE_TIMEOUT);
   BACnetUnsigned backupFailureTimeout(30);
   AnyValue propValue;
   propValue.SetObject(&backupFailureTimeout);
   if (!SendExpectWriteProperty(devObjID, propID, propValue))
   {
      throw("Cannot write Backup_Failure_Timeout to the IUT");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Verify Backup_Failure_Timeout = 30...", FALSE);
   if (!SendExpectReadProperty(devObjID, propID, propValue))
   {
      throw("Cannot read Backup_Failure_Timeout from the IUT");
   }
   if (backupFailureTimeout.uintValue != 30)
   {
      throw("Verify Backup_Failure_Timeout in the IUT failed");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start backup...", FALSE);
   if (!SendExpectReinitialize(STARTBACKUP, m_strPassword))
   {
      throw("Unable to initialize a backup procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   // We presume that recovery from a timeout in backup mode is easy.
   // If not, add m_Delay as we do for Restore, below.
   Delay(40);

   m_pOutputDlg->OutMessage("Verify System_Status != BACKUP_IN_PROGRESS...", FALSE);
   BACnetEnumerated  systemStatus;
   propID.m_enumValue = PICS::SYSTEM_STATUS;
   propValue.SetObject(&systemStatus);
   if (!SendExpectReadProperty(devObjID, propID, propValue))
   {
      throw("Cannot read System_Status from the IUT");
   }
   if (systemStatus.m_enumValue == PICS::BACKUP_IN_PROGRESS)
   {
      throw("The System_Status in the IUT's Device Object is still BACKUP_IN_PROGRESS");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start restore...", FALSE);
   if (!SendExpectReinitialize(STARTRESTORE, m_strPassword))
   {
      throw("Unable to initialize a backup procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   // Originally waited 40 seconds here.
   // But if IUT times out after the 30 seconds set above, it may take longer than
   // 10 seconds to restore itself to operating condition.  So we add some delay.
   Delay(40 + m_Delay);

   m_pOutputDlg->OutMessage("Verify System_status != DOWNLOAD_IN_PROGRESS...", FALSE);
   if (!SendExpectReadProperty(devObjID, propID, propValue))
   {
      throw("Cannot read System_Status from the IUT");
   }
   if (systemStatus.m_enumValue == PICS::DOWNLOAD_IN_PROGRESS)
   {
      throw("The System_Status in the IUT's Device Object is still DOWNLOAD_IN_PROGRESS");
   }
   m_pOutputDlg->OutMessage("OK");
}


// Ending backup and restore procedures via abort
void BakRestoreExecutor::DoAuxiliaryTest_6(BACnetObjectIdentifier& devObjID)
{
   m_pOutputDlg->OutMessage("Auxiliary Test 6");

   // Restore backup time from dialog value m_Backup_Timeout
   m_pOutputDlg->OutMessage("Transmit WriteProperty-Request to restore Backup_Failure_Timeout...", FALSE);
   BACnetEnumerated propID(PICS::BACKUP_FAILURE_TIMEOUT);
   BACnetUnsigned backupFailureTimeout(m_Backup_Timeout);
   AnyValue propValue;
   propValue.SetObject(&backupFailureTimeout);
   if (!SendExpectWriteProperty(devObjID, propID, propValue))
   {
      throw("Cannot write Backup_Failure_Timeout to the IUT");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Verify Backup_Failure_Timeout...", FALSE);
   if (!SendExpectReadProperty(devObjID, propID, propValue))
   {
      throw("Cannot read Backup_Failure_Timeout from the IUT");
   }
   if (backupFailureTimeout.uintValue != m_Backup_Timeout)
   {
      throw("Verify Backup_Failure_Timeout in the IUT failed");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start backup...", FALSE);
   if (!SendExpectReinitialize(STARTBACKUP, m_strPassword))
   {
      throw("Unable to initialize a backup procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to end backup...", FALSE);
   if (!SendExpectReinitialize(ENDBACKUP, m_strPassword))
   {
      throw("Unable to end a backup procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Verify System_Status != BACKUP_IN_PROGRESS...", FALSE);
   BACnetEnumerated  systemStatus;
   propID = PICS::SYSTEM_STATUS;
   propValue.SetObject(&systemStatus);
   if (!SendExpectReadProperty(devObjID, propID, propValue))
   {
      throw("Cannot read System_Status from the IUT");
   }
   if (systemStatus.m_enumValue == PICS::BACKUP_IN_PROGRESS)
   {
      throw("The System_Status in the IUT's Device Object is still BACKUP_IN_PROGRESS");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start restore...", FALSE);
   if (!SendExpectReinitialize(STARTRESTORE, m_strPassword))
   {
      throw("Unable to initialiate a backup procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   // Wait for restore prep
   Delay( m_Delay );

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to abort restore...", FALSE);
   if (!SendExpectReinitialize(ABORTRESTORE, m_strPassword))
   {
      throw("Unable to abort a restore procedure");
   }
   m_pOutputDlg->OutMessage("OK");

   // Delay to let IUT recover.
   Delay( m_Delay );

   m_pOutputDlg->OutMessage("Verify System_status != DOWNLOAD_IN_PROGRESS...", FALSE);
   if (!SendExpectReadProperty(devObjID, propID, propValue))
   {
      throw("Cannot read System_Status from the IUT");
   }
   if (systemStatus.m_enumValue == PICS::DOWNLOAD_IN_PROGRESS)
   {
      throw("The System_Status in the IUT's Device Object is still DOWNLOAD_IN_PROGRESS");
   }
   m_pOutputDlg->OutMessage("OK");
}

// Initiating a backup procedure with an invalid password
void BakRestoreExecutor::DoAuxiliaryTest_7()
{
   m_pOutputDlg->OutMessage("Auxiliary Test 7");
   if (m_strPassword.IsEmpty())
   {
      Msg("no password is required, this test has been omitted");
      return;     // do not need carry on this test
   }

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start restore with invalid password...", FALSE);
   CString strPasswordTemp(m_strPassword);

   m_strPassword += "123";    // to create a invalid password
   BACnetEnumerated  errorClass;
   BACnetEnumerated  errorCode;
   if (!SendExpectReinitializeNeg(STARTBACKUP, errorClass, errorCode))
   {
      m_strPassword = strPasswordTemp;
      throw("Expected BACnetErrorAPDU");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Verify received Error PDU...", FALSE);
   if ((errorClass.m_enumValue != ErrorClass::SECURITY) ||
      (errorCode.m_enumValue != ErrorCode::PASSWORD_FAILURE))
   {
      m_strPassword = strPasswordTemp;
      throw("Wrong Error Class");
   }
   m_pOutputDlg->OutMessage("OK");

   m_strPassword = strPasswordTemp;
}

// Initiating a restore procedure with an invalid password
void BakRestoreExecutor::DoAuxiliaryTest_8()
{
   m_pOutputDlg->OutMessage("Auxiliary Test 8");
   if (m_strPassword.IsEmpty())
   {
      Msg("No password is required, this test has been omitted");
      return;     // do not need carry on this test
   }

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start restore with invalid password...", FALSE);
   CString strPasswordTemp(m_strPassword);

   m_strPassword += "123";    // to create a invalid password
   BACnetEnumerated  errorClass;
   BACnetEnumerated  errorCode;
   if (!SendExpectReinitializeNeg(STARTRESTORE, errorClass, errorCode))
   {
      m_strPassword = strPasswordTemp;
      throw("Expected BACnetErrorAPDU");
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Verify received Error PDU...", FALSE);
   if ((errorClass.m_enumValue != ErrorClass::SECURITY) ||
      (errorCode.m_enumValue != ErrorCode::PASSWORD_FAILURE))
   {
      m_strPassword = strPasswordTemp;
      throw("Wrong Error Class");
   }
   m_pOutputDlg->OutMessage("OK");

   m_strPassword = strPasswordTemp;
}

// Initiating and ending a backup procedure when a password is not required
void BakRestoreExecutor::DoAuxiliaryTest_9(BACnetObjectIdentifier& devObjID)
{
   m_pOutputDlg->OutMessage("Auxiliary Test 9");
   if (!m_strPassword.IsEmpty())
   {
      Msg("A specific password is required, this test has been omitted");
      return;     // do not need carry on this test
   }

   CString strPasswordTemp(m_strPassword);
   m_strPassword = "123";     // any non-zero length password

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start backup with any password...", FALSE);
   if (!SendExpectReinitialize(STARTBACKUP, m_strPassword))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot start Backup procedure");
      m_strPassword = strPasswordTemp;
      return;
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to end backup with any password...", FALSE);
   if (!SendExpectReinitialize(ENDBACKUP, m_strPassword))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot end Backup procedure");
      m_strPassword = strPasswordTemp;
      return;
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Verify System_Status != BACKUP_IN_PROGRESS...", FALSE);
   BACnetEnumerated systemStatus;
   AnyValue value;
   value.SetObject(&systemStatus);
   BACnetEnumerated propID(PICS::SYSTEM_STATUS);
   if (!SendExpectReadProperty(devObjID, propID, value))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot read Device/System_Status from the IUT");
      m_strPassword = strPasswordTemp;
      return;
   }

   if (systemStatus.m_enumValue == PICS::BACKUP_IN_PROGRESS)
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("After the backup procedure, the system_statue should not still be BACKUP_IN_PROGRESS");
      m_strPassword = strPasswordTemp;
      return;
   }

   m_pOutputDlg->OutMessage("OK");
   m_strPassword = strPasswordTemp;
}

// Initiating and ending a restore procedure when a password is not required
void BakRestoreExecutor::DoAuxiliaryTest_10(BACnetObjectIdentifier& devObjID)
{
   m_pOutputDlg->OutMessage("Auxiliary Test 10");
   if (!m_strPassword.IsEmpty())
   {
      Msg("A specific password is required, this test has been omitted");
      return;     // do not need carry on this test
   }

   CString strPasswordTemp(m_strPassword);
   m_strPassword = "123";

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to start restore with any password...", FALSE);
   if (!SendExpectReinitialize(STARTRESTORE, m_strPassword))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot start Backup procedure");
      m_strPassword = strPasswordTemp;
      return;
   }
   m_pOutputDlg->OutMessage("OK");

   // Wait for restore prep
   Delay( m_Delay );

   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice-Request to end restore...", FALSE);
   if (!SendExpectReinitialize(ENDRESTORE, m_strPassword))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot end Backup procedure");
      m_strPassword = strPasswordTemp;
      return;
   }
   m_pOutputDlg->OutMessage("OK");

   // Delay to let IUT recover.
   Delay( m_Delay );

   m_pOutputDlg->OutMessage("Verify system_status != DOWNLOAD_IN_PROGRESS");
   BACnetEnumerated systemStatus;
   AnyValue value;
   value.SetObject(&systemStatus);
   BACnetEnumerated propID(PICS::SYSTEM_STATUS);
   if (!SendExpectReadProperty(devObjID, propID, value))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot read Device/System_Status from the IUT");
      m_strPassword = strPasswordTemp;
      return;
   }

   if (systemStatus.m_enumValue == PICS::DOWNLOAD_IN_PROGRESS)
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("After the restore procedure, the system_statue should not still be DOWNLOAD_IN_PROGRESS");
      m_strPassword = strPasswordTemp;
      return;
   }
   m_pOutputDlg->OutMessage("OK");

   m_strPassword = strPasswordTemp;
}


// System_Status during a backup procedure
void BakRestoreExecutor::DoAuxiliaryTest_11(BACnetObjectIdentifier& devObjID)
{
   m_pOutputDlg->OutMessage("Auxiliary Test 11");
   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice request to start backup...", FALSE);
   if (!SendExpectReinitialize(STARTBACKUP, m_strPassword))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot start Backup procedure");
      return;
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Verify system_status = BACKUP_IN_PROGRESS");
   BACnetEnumerated systemStatus;
   AnyValue value;
   value.SetObject(&systemStatus);
   BACnetEnumerated propID(PICS::SYSTEM_STATUS);
   if (!SendExpectReadProperty(devObjID, propID, value))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot read Device/System_Status from the IUT");
      return;
   }

   if (systemStatus.m_enumValue != PICS::BACKUP_IN_PROGRESS)
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("The system_statue should be BACKUP_IN_PROGRESS during the backup procedure");
      return;
   }
   m_pOutputDlg->OutMessage("OK");

   m_pOutputDlg->OutMessage("Transmit a ReinitalizeDevice service to End Backup...", FALSE);
   if (!SendExpectReinitialize(ENDBACKUP, m_strPassword))
   {
      throw("Cannot end backup process");
   }
   m_pOutputDlg->OutMessage("OK");
}


// System_Status during a restore procedure
void BakRestoreExecutor::DoAuxiliaryTest_12(BACnetObjectIdentifier& devObjID)
{
   m_pOutputDlg->OutMessage("Auxiliary Test 12");
   m_pOutputDlg->OutMessage("Transmit ReinitializeDevice request to start backup...", FALSE);
   if (!SendExpectReinitialize(STARTRESTORE, m_strPassword))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot start Restore procedure");
      return;
   }
   m_pOutputDlg->OutMessage("OK");

   // Wait for restore prep
   Delay( m_Delay );

   m_pOutputDlg->OutMessage("Verify system_status = DOWNLOAD_IN_PROGRESS");
   BACnetEnumerated systemStatus;
   AnyValue value;
   value.SetObject(&systemStatus);
   BACnetEnumerated propID(PICS::SYSTEM_STATUS);
   if (!SendExpectReadProperty(devObjID, propID, value))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot read Device/System_Status from the IUT");
      return;
   }

   if (systemStatus.m_enumValue != PICS::DOWNLOAD_IN_PROGRESS)
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("The system_statue should be DOWNLOAD_IN_PROGRESS during the download procedure");
      return;
   }
   m_pOutputDlg->OutMessage("OK, Sending an Abort Restore ...", FALSE);

   if (!SendExpectReinitialize(ABORTRESTORE, m_strPassword))
   {
      m_pOutputDlg->OutMessage("Failed");
      Msg("Cannot start Backup procedure");
      return;
   }
   m_pOutputDlg->OutMessage("OK");

   // Delay to let IUT recover.
   // TODO: should check system-status and/or backup-restore-state
   Delay( m_Delay );
}

BOOL BakRestoreExecutor::SendExpectAtomicReadFile_Stream(BACnetObjectIdentifier& fileID, BACnetInteger& fileStartPosition,
                                                         BACnetUnsigned& ROC, BACnetOctetString& fileData)
{
   m_bExpectAPDU = TRUE;
   m_nExpectAPDUType = complexAckPDU;
   m_nExpectAPDUServiceChoice = atomicReadFile;
   CByteArray contents;
   BACnetAPDUEncoder enc;
   fileID.Encode(enc);
   // access method: Stream Access
   BACnetOpeningTag().Encode(enc,0);
   fileStartPosition.Encode(enc);
   ROC.Encode(enc);
   BACnetClosingTag().Encode(enc,0);
   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
   {
      contents.Add( enc.pktBuffer[i] );
   }
   
   contents.InsertAt(0, (BYTE)0x06);      // Service Choice = 06(AtomicReadFile-Request)
   contents.InsertAt(0, InvokeID());      // Invoke ID
   InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
   contents.InsertAt(0, (BYTE)0x00);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=0)

   if (!SendExpectPacket(contents))
   {
      return FALSE;
   }

   BACnetAPDUDecoder dec(m_pAPDU->pktBuffer, m_pAPDU->pktLength);
   BACnetBoolean endofFile;
   endofFile.Decode(dec);
   BACnetOpeningTag().Decode(dec);
   fileStartPosition.Decode(dec);
   fileData.Decode(dec);
   BACnetClosingTag().Decode(dec);

   if(dec.pktLength != 0)
   {
      Msg("Decoding hasn't finished!");
   }

   return TRUE;
}


// only read one record back each time
// Requested Record Count = 1;
BOOL BakRestoreExecutor::SendExpectAtomicReadFile_Record(BACnetObjectIdentifier& fileID, BACnetInteger& fileStartRecord,
                                                         BACnetOctetString& fileRecordData, BACnetBoolean& endofFile)
{
   m_bExpectAPDU = TRUE;
   m_nExpectAPDUType = complexAckPDU;
   m_nExpectAPDUServiceChoice = atomicReadFile;

   CByteArray contents;
   CByteArray segmentAck;
   BACnetAPDUEncoder enc;

   int segmentCounter = 0, totalLen;
   bool lastPacket = false;

   fileID.Encode(enc);
   // access method: Stream Access
   BACnetOpeningTag().Encode(enc,1);
   fileStartRecord.Encode(enc);
   BACnetUnsigned requestedRecordCount(1);
   requestedRecordCount.Encode(enc);
   BACnetClosingTag().Encode(enc,1);
   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
   {
      contents.Add( enc.pktBuffer[i] );
   }
   contents.InsertAt(0, (BYTE)0x06);      // Service Choice = 6(AtomicReadFile-Request)
   contents.InsertAt(0, InvokeID());      // Invoke ID
   InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
   contents.InsertAt(0, (BYTE)0x02);      // PDU Type = 0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=1) //Support Segmentation

   if (!SendExpectPacket(contents))
   {
      return FALSE;
   }

   /*  
    * Support segmented AtomicReadFile
    * This section handles the segmentAck and saves the data for each segment
    */

   //Clear out the old
   m_segmentData.RemoveAll();

   while(m_pAPDU->apduSeg) 
   {
      //Check for the last segment
      if(!m_pAPDU->apduMor)
      {
         lastPacket = true;
      }

      m_nExpectAPDUType = complexAckPDU;
      m_nExpectAPDUServiceChoice = atomicReadFile;

      //Build the segmentAck
      segmentAck.RemoveAll();
      segmentAck.InsertAt(0, 0x01);             //Window Size
      segmentAck.InsertAt(0, m_pAPDU->apduSeq);    //Sequence Number
      segmentAck.InsertAt(0, m_pAPDU->apduInvokeID);  //InvokeID
      
      //Send a NAK if the sequence is out of order
      if(m_pAPDU->apduSeq == segmentCounter++)
         segmentAck.InsertAt(0, 0x40);          //SegmentACK tag.
      else
         segmentAck.InsertAt(0, 0x42);          //SegmentNAK tag

      //Copy the data
      for(int i = 0; i < m_pAPDU->pktLength; i++)
      {
         m_segmentData.Add((BYTE)m_pAPDU->pktBuffer[i]);
      }
      
      //Send the segmentAck
      if( !SendExpectPacket(segmentAck, !lastPacket) )
      {
         return FALSE;
      }
      
      //If we just sent the last packet, consolidate the data and leave
      if(lastPacket)
      {
         delete[] m_packetData;
         
         totalLen = m_segmentData.GetSize();

         //Add the data to the global BACnetAPDU
         m_packetData = new BACnetOctet[totalLen];
         m_pAPDU->pktBuffer = m_packetData;
         m_pAPDU->pktLength = totalLen;
         memcpy(m_pAPDU->pktBuffer, &m_segmentData[0], totalLen);

         break;
      }
   }

   BACnetAPDUDecoder dec(m_pAPDU->pktBuffer, m_pAPDU->pktLength);
   endofFile.Decode(dec);
   BACnetOpeningTag().Decode(dec);
   fileStartRecord.Decode(dec);
   requestedRecordCount.Decode(dec);
   if (requestedRecordCount.uintValue != 1)
   {
      return FALSE;     // only read one record back each time
   }
   while(dec.pktLength != 0)
   {
      //if the code is closing tag then break
      BACnetAPDUTag tag;
      dec.ExamineTag(tag);
      if(tag.tagClass == closingTagClass)
         break;
      fileRecordData.Decode(dec);
   }

   return TRUE;
}


// only use BACnetObjectIdentifier as object specifier
// do not consider PropertyArrayIndex and Priority
BOOL BakRestoreExecutor::SendExpectCreatObject(BACnetObjectIdentifier& objID,
                                               BACnetSequenceOf<PropertyValue>& listOfInitialValues)
{
   m_bExpectAPDU = TRUE;
   m_nExpectAPDUType = complexAckPDU;
   m_nExpectAPDUServiceChoice = createObject;

   CByteArray contents;
   BACnetAPDUEncoder enc;

   BACnetOpeningTag().Encode(enc, 0);
   objID.Encode(enc, 1);
   BACnetClosingTag().Encode(enc, 0);

   BACnetOpeningTag().Encode(enc, 1);
   for(int index = 0; index < listOfInitialValues.GetItemCount(); index++)
   {
      PropertyValue* pValue = listOfInitialValues.GetElement(index);
      pValue->Encode(enc);
   }
   BACnetClosingTag().Encode(enc, 1);

   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
   {
      contents.Add( enc.pktBuffer[i] );
   }
   contents.InsertAt(0, (BYTE)0x0A);      // Service Choice = 13(creatObject-Request)
   contents.InsertAt(0, InvokeID());      // Invoke ID
   InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
   contents.InsertAt(0, (BYTE)0x00);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=0)

   if (!SendExpectPacket(contents))
   {
      return FALSE;
   }

   BACnetAPDUDecoder dec(m_pAPDU->pktBuffer, m_pAPDU->pktLength);
   objID.Decode(dec);

   if(dec.pktLength != 0)
   {
      Msg("Decoding hasn't finished!");
   }

   return TRUE;
}


BOOL BakRestoreExecutor::SendExpectAtomicWriteFile_Stream(BACnetObjectIdentifier& fileID, BACnetInteger& fileStartPosition,
                                                          BACnetOctetString& fileData)
{
   m_bExpectAPDU = TRUE;
   m_nExpectAPDUType = complexAckPDU;
   m_nExpectAPDUServiceChoice = atomicWriteFile;

   CByteArray contents;
   BACnetAPDUEncoder enc;

   fileID.Encode(enc);
   BACnetOpeningTag().Encode(enc, 0);
   fileStartPosition.Encode(enc);
   fileData.Encode(enc);
   BACnetClosingTag().Encode(enc, 0);
   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
   {
      contents.Add( enc.pktBuffer[i] );
   }
   contents.InsertAt(0, (BYTE)0x07);      // Service Choice = 7(atomicWriteFile-Request)
   contents.InsertAt(0, InvokeID());      // Invoke ID
   InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
   contents.InsertAt(0, (BYTE)0x00);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=0)

   if (!SendExpectPacket(contents))
   {
      return FALSE;
   }

   BACnetAPDUDecoder dec(m_pAPDU->pktBuffer, m_pAPDU->pktLength);
   fileStartPosition.Decode(dec);

   if(dec.pktLength != 0)
   {
      Msg("Decoding hasn't finished!");
   }

   return TRUE;
}

BOOL BakRestoreExecutor::SendExpectAtomicWriteFile_Record(BACnetObjectIdentifier& fileID, BACnetInteger& fileStartRecord,
                                                          BACnetOctetString& fileRecordData)
{
   CByteArray contents;
   BACnetAPDUEncoder enc;
   UINT segmentNum, segSize, dataPos;
   BYTE invokeID;
   BOOL lastSegment; 

   m_bExpectAPDU = TRUE;
   m_nExpectAPDUType = complexAckPDU;
   m_nExpectAPDUServiceChoice = atomicWriteFile;

   ASSERT(m_maxAPDULen != 0);

   //Build the Encoder
   fileID.Encode(enc);
   BACnetOpeningTag().Encode(enc, 1);
   fileStartRecord.Encode(enc);
   BACnetUnsigned returnedRecordCount(1);
   returnedRecordCount.Encode(enc);
   fileRecordData.Encode(enc);
   BACnetClosingTag().Encode(enc, 1);
   
   /*  
    * Support segmented AtomicWriteFile
    * Send the data in segments if a single record size > maxAPDU
    */
   if( ((UINT)enc.pktLength + SEGMENTED_APDU_HEADER_SIZE + NPDU_HEADER_BUFF ) > m_maxAPDULen)
   {
      lastSegment = FALSE;
      segmentNum = segSize = dataPos = 0;
      
      //Change the PDU type
      m_nExpectAPDUType = segmentAckPDU;
      
      //Use single invokeID
      invokeID = InvokeID();

      //Calculate the size of the segment
      segSize = m_maxAPDULen - SEGMENTED_APDU_HEADER_SIZE - NPDU_HEADER_BUFF;

      while(true)
      {
         //copy the encoding into the byte array   
         for (UINT i = 0; i < segSize; i++)
         {
            ASSERT(dataPos < (UINT)enc.pktLength);
            contents.Add( enc.pktBuffer[dataPos++] );
         }

         //APDU header
         contents.InsertAt(0, (BYTE)0x07);         // Service Choice = 7(atomicWriteFile-Request)
         contents.InsertAt(0, (BYTE)0x01);         // Window Size
         contents.InsertAt(0, (BYTE)segmentNum++); // Sequence number
         contents.InsertAt(0, invokeID);           // Invoke ID
         InsertMaxAPDULenAccepted(contents);       // Maximum APDU Size Accepted
         if(lastSegment)
            contents.InsertAt(0, (BYTE)0x08);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=1, MOR=0, SA=0)
         else
            contents.InsertAt(0, (BYTE)0x0C);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=1, MOR=1, SA=0)
         
         //If this is the last segment, we want a complexAck instead of a segmentAck
         if(lastSegment)
         {
            m_nExpectAPDUType = complexAckPDU;
         }

         //Send the packet
         if (!SendExpectPacket(contents))
         {
            return FALSE;
         }

         if(lastSegment)
            break;
         
         //Prep the next packet
         contents.RemoveAll();
         
         //Check for final segment
         if( (enc.pktLength - dataPos) <= segSize )
         {
            //set the flag
            lastSegment = TRUE;

            //adjust segment size to be exact
            segSize = enc.pktLength - dataPos;
         }
      }
   }
   else
   {
      // copy the encoding into the byte array
      for (int i = 0; i < enc.pktLength; i++)
      {
         contents.Add( enc.pktBuffer[i] );
      }

      contents.InsertAt(0, (BYTE)0x07);      // Service Choice = 7(atomicWriteFile-Request)
      contents.InsertAt(0, InvokeID());      // Invoke ID
      InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
      contents.InsertAt(0, (BYTE)0x00);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=0)

      if (!SendExpectPacket(contents))
      {
         return FALSE;
      }

   }

   BACnetAPDUDecoder dec(m_pAPDU->pktBuffer, m_pAPDU->pktLength);
   fileStartRecord.Decode(dec);

   if(dec.pktLength != 0)
   {
      Msg("Decoding hasn't finished!");
   }

   return TRUE;
}


BOOL BakRestoreExecutor::SendExpectReinitializeNeg(ReinitializedStateOfDevice nReinitState,
                                                   BACnetEnumerated& errorClass, BACnetEnumerated& errorCode)
{
   m_bExpectAPDU = TRUE;
   m_nExpectAPDUType = errorPDU;
   m_nExpectAPDUServiceChoice = reinitializeDevice;
   CByteArray contents;
   BACnetAPDUEncoder enc;
   BACnetEnumerated  reinitState(nReinitState);
   reinitState.Encode(enc, 0);
   if (!m_strPassword.IsEmpty())
   {
      BACnetCharacterString  password(m_strPassword);
      password.Encode(enc, 1);
   }
   // copy the encoding into the byte array
   for (int i = 0; i < enc.pktLength; i++)
   {
      contents.Add( enc.pktBuffer[i] );
   }
   contents.InsertAt(0, (BYTE)0x14);      // Service Choice = 20(ReinitializeDevice-Request)
   contents.InsertAt(0, InvokeID());      // Invoke ID
   InsertMaxAPDULenAccepted(contents);    // Maximum APDU Size Accepted
   contents.InsertAt(0, (BYTE)0x00);      // PDU Type=0 (BACnet-Confirmed-Request-PDU, SEG=0, MOR=0, SA=0)

   if (!SendExpectPacket(contents))
   {
      return FALSE;
   }
   BACnetAPDUDecoder dec(m_pAPDU->pktBuffer, m_pAPDU->pktLength);
// (dec.pktLength--, dec.pktBuffer++);
   errorClass.Decode(dec);
   errorCode.Decode(dec);

   return TRUE;
}

void BakRestoreExecutor::ReadDatabaseRevAndRestoreTime( BACnetObjectIdentifier &devObjID,
                                                        BACnetUnsigned         &databaseRevision,
                                                        BACnetTimeStamp        &lastRestoreTime )
{
   BACnetEnumerated propID;
   m_pOutputDlg->OutMessage("Use ReadProperty requests to read Device/Database_Revision"
                            "and Device/Last_Restore_Time...", FALSE);

   // Use ReadProperty request to read the Device/Database_Revision and the
   // Device/Last_Restore_Time and record these in the first line of the
   propID.m_enumValue = PICS::DATABASE_REVISION;
   AnyValue propValue;
   propValue.SetObject(&databaseRevision);
   if (!SendExpectReadProperty(devObjID, propID, propValue))
   {
      throw("Cannot read DATABASE_REVISION from IUT");
   }

   propID.m_enumValue = PICS::LAST_RESTORE_TIME;
   propValue.SetObject(&lastRestoreTime);
   if (!SendExpectReadProperty(devObjID, propID, propValue))
   {
      throw("Cannot read LAST_RESTORE_TIME from IUT");
   }
   m_pOutputDlg->OutMessage("OK");

   CString str, rev;
   databaseRevision.Encode(rev, BACnetEncodeable::FMT_PLAIN);
   str.Format( "Database_Revision = %s", (LPCSTR)rev );
   m_pOutputDlg->OutMessage( (LPCSTR)str );
}
