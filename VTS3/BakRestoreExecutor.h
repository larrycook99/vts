// BakRestoreExecutor.h: interface for the BakRestoreExecutor class.
// Jingbo Gao, Sep 20 2004
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAKRESTOREEXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_)
#define AFX_BAKRESTOREEXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include "bacnet.hpp"
#include "vtsdoc.h"
#include "SendReceiveExecutor.h"

class VTSDoc;
typedef VTSDoc * VTSDocPtr;
struct BACnetNPDU;

class BakRestoreExecutor : public SendReceiveExecutor
{
public:
   enum FunctionToExecute
   {
      ALL_BACKUP_RESTORE = 0,
      FULL_BACKUP_RESTORE,
      BACKUP_ONLY,
      RESTORE_ONLY,
      AUXILIARY_BACKUP_RESTORE
   };

public:
   BakRestoreExecutor();
   virtual ~BakRestoreExecutor();
   virtual void ProcessTask();

   void ExecuteTest();
   void ReadDatabaseRevAndRestoreTime( BACnetObjectIdentifier &devObjID,
                                       BACnetUnsigned         &databaseRevision,
                                       BACnetTimeStamp        &lastRestoreTime );
private:
   CString              m_strBackupFileName;  // the base name of the files to create
   CString              m_strPassword;
   FunctionToExecute    m_funToExe;

   CByteArray           m_segmentData; //Segment data

   UINT                 m_Delay;
   UINT                 m_Backup_Timeout;

   void DoBackupTest();
   void DoRestoreTest();
   void DoAuxiliaryTest();

// void DoAuxiliaryTest_1();
   void DoAuxiliaryTest_2();
// void DoAuxiliaryTest_3();
   void DoAuxiliaryTest_4();
   void DoAuxiliaryTest_5(BACnetObjectIdentifier& devObjID);
   void DoAuxiliaryTest_6(BACnetObjectIdentifier& devObjID);
   void DoAuxiliaryTest_7();
   void DoAuxiliaryTest_8();
   void DoAuxiliaryTest_9(BACnetObjectIdentifier& devObjID);
   void DoAuxiliaryTest_10(BACnetObjectIdentifier& devObjID);
   void DoAuxiliaryTest_11(BACnetObjectIdentifier& devObjID);
   void DoAuxiliaryTest_12(BACnetObjectIdentifier& devObjID);
   
   BOOL SendExpectAtomicReadFile_Stream(BACnetObjectIdentifier& fileID, BACnetInteger& fileStartPosition, 
                               BACnetUnsigned& ROC, BACnetOctetString& fileData);
   BOOL SendExpectAtomicReadFile_Record(BACnetObjectIdentifier& fileID, BACnetInteger& fileStartRecord, 
                               BACnetOctetString& fileRecordData, 
                               BACnetBoolean& endofFile);
   BOOL SendExpectCreatObject(BACnetObjectIdentifier& objID, 
                              BACnetSequenceOf<PropertyValue>& listOfInitialValues);
   BOOL SendExpectAtomicWriteFile_Stream(BACnetObjectIdentifier& fileID, BACnetInteger& fileStartPosition, 
                                         BACnetOctetString& fileData);
   BOOL SendExpectAtomicWriteFile_Record(BACnetObjectIdentifier& fileID, BACnetInteger& fileStartRecord, 
                                         BACnetOctetString& fileRecordData);

   BOOL SendExpectReinitializeNeg(ReinitializedStateOfDevice nReinitState,
                                  BACnetEnumerated& errorClass, BACnetEnumerated& errorCode);
};

#endif // !defined(AFX_BAKRESTOREEXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_)
