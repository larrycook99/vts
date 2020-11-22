// InconsistentParsExecutor.h  - executes the 13.4.3, 13.4.4, and 13.4.5 tests
// Lori Tribble - 10/17/2009
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INCONSISTENTPARSEXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_)
#define AFX_INCONSISTENTPARSEXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_

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
class VTSInconsistentParsProgressDlg;

class InconsistentParsExecutor : public SendReceiveExecutor
{
public:
   enum RejectReason
   {
      OTHER = 0,
      BUFFER_OVERFLOW,
      INCONSISTENT_PARAMETERS,
      INVALID_PARAMETER_DATA_TYPE,
      INVALID_TAG,
      MISSING_REQUIRED_PARAMETER,
      PARAMETER_OUT_OF_RANGE,
      TOO_MANY_ARGUMENTS,
      UNDEFINED_ARGUMENTS,
      UNRECOGNIZED_SERVICE
   };
   enum FunctionToExecute
   {
      INVALID_TAG_TEST = 0,
      MISSING_REQUIRED_TEST,
      TOO_MANY_ARGS_TEST
   };

public:
   InconsistentParsExecutor();
   virtual ~InconsistentParsExecutor();
   virtual void ProcessTask();

   void ExecuteTest();

private:
   BACnetObjectIdentifier  m_ObjID;
   BACnetEnumerated        m_propID;
   FunctionToExecute       m_funToExe;

   void DoInvalidTagTest();
   void DoMissingRequiredTest();
   void DoTooManyArgsTest();

   BOOL SendReadPropertyExpectReject(BACnetAPDUEncoder *enc, RejectReason expectedErrors[], int len);
};

#endif // !defined(AFX_INCONSISTENTPARSEEXECUTOR_H__B11EB238_C49B_4046_84A9_0CF30BFD844B__INCLUDED_)
