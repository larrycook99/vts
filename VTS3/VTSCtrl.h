#ifndef _VTSCtrl
#define _VTSCtrl

#include "BACnet.hpp"
#include "BACnetIP.hpp"
#include "StringTables.h"

//
// VTSCtrl
//

class VTSCtrl {
   public:
      BOOL        ctrlNull;            // no value specified
      bool        ctrlEnabled;         // control enabled on page

      int         ctrlID;              // dialog control ID
      const CWnd* ctrlWindow;          // owner window

      VTSCtrl( const CWnd* wp, int id );  // bind to window and control
      virtual ~VTSCtrl( void );

      virtual void Enable( void );     // enable object and control
      virtual void Disable( void );    // disable both

      virtual void CtrlToObj( void ) = 0;     // interpret control contents, save in object
      virtual void ObjToCtrl( void ) = 0;     // object value reflected in control

      virtual void SaveCtrl( BACnetAPDUEncoder& enc ) = 0;      // save the contents
      virtual void RestoreCtrl( BACnetAPDUDecoder& dec ) = 0;      // restore the contents

      void UpdateData( BOOL bCtrlToObj = true );   // call one of above functions.
};

//
// VTSAddrCtrl
//

class VTSAddrCtrl : public VTSCtrl, public BACnetOctetString {
   public:
      bool  emptyIsNull;                  // empty control value is null

      VTSAddrCtrl( const CWnd* wp, int id );    // bind to window and control

      void CtrlToObj( void );                // interpret control contents, save in object
      void ObjToCtrl( void );                // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSEnetAddrCtrl
//

//MAD_DB  class VTSNameList;
class VTSNames;
class VTSName;
class VTSPort;

class VTSAddrComboCtrl : public VTSCtrl, public BACnetAddress
{
   public:
      int        m_ctrlComboID;
      VTSNames * m_pnamesCtrl;

      VTSAddrComboCtrl( const CWnd* wp, int cid, int tid ); // bind to window and control

      void LoadCombo( VTSNames * pnames, VTSPort * pport, bool okBroadcast = false );
      virtual bool IsNameMatch( VTSName * pname, VTSPort * pport, bool okBroadcast ) const;

      void Selchange( void );                   // combo selection changed
      virtual void AssignAddress(VTSName * pname) = 0;
      void FindName( LPCSTR lpszName );            // initialize with a specific name (TD or IUT)

      virtual void Enable( void );              // enable object and control
      virtual void Disable( void );             // disable both

      void CtrlToObj( void );             // interpret control contents, save in object
      virtual void CtrlToAddress( LPCTSTR s );
      virtual bool IsAddressMatch( BACnetAddress * pbacnetaddr );

      void ObjToCtrl( void );             // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};


class VTSEnetAddrCtrl : public VTSAddrComboCtrl
{
   public:
      VTSEnetAddrCtrl( const CWnd* wp, int cid, int tid );  // bind to window and control

      virtual void AssignAddress(VTSName * pname);
};


class VTSMSTPAddrCtrl : public VTSAddrComboCtrl
{
   public:
      VTSMSTPAddrCtrl( const CWnd* wp, int cid, int tid );  // bind to window and control

      virtual void AssignAddress(VTSName * pname);
      virtual void CtrlToAddress( LPCTSTR s );
      void ObjToCtrl( void );
};

//
// VTSIPAddrCtrl
//

//MAD_DB class VTSIPAddrCtrl : public VTSCtrl, public BACnetIPAddr {
// Warning... have to munge BACnetIPAddr methods because polymorphism removes this ability

class VTSIPAddrCtrl : public VTSAddrComboCtrl
{
   public:
      VTSIPAddrCtrl( const CWnd* wp, int cid, int tid ); // bind to window and controls

      virtual void AssignAddress(VTSName * pname);
      virtual void CtrlToAddress( LPCTSTR s );

      void ObjToCtrl( void );             // object value reflected in control
};

//
// VTSRemoteAddrCtrl
//

class VTSIntegerCtrl;

class VTSRemoteAddrCtrl : public VTSAddrComboCtrl
{
   public:
      VTSIntegerCtrl *ctrlNet;

      VTSRemoteAddrCtrl( const CWnd* wp, VTSIntegerCtrl *icp, int cid, int tid );   // bind to window and controls

      virtual bool IsNameMatch( VTSName * pname, VTSPort * pport, bool okBroadcast ) const;
      virtual void AssignAddress(VTSName * pname);
      virtual bool IsAddressMatch( BACnetAddress * pbacnetaddr );
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSBooleanCtrl
//

class VTSBooleanCtrl : public VTSCtrl, public BACnetBoolean {
   public:
      bool  m_bCheckBox;               // true iff ctrl is a check box

      VTSBooleanCtrl( const CWnd* wp, int id, bool isCheckBox = false );   // bind to window and control

      void CtrlToObj( void );             // interpret control contents, save in object
      void ObjToCtrl( void );             // object value reflected in control
      void SetCheck( bool fChecked );     // madanner 9/4/02
                                          // simplifies setting control from external 

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSEnumeratedCtrl
//

class VTSEnumeratedCtrl : public VTSCtrl, public BACnetEnumerated
{
public:
   const char* const *m_Table;         // ptr to list of char*
   int      m_TableSize;               // number of enumeration values
   bool     m_bCombo;                  // true iff ctrl is a combo
   bool     m_sortStrings;             // true to sort strings in the como

   // Data for property selection dialog
   int      m_nObjType;                // the Object Type when propCombo           Added by Zhu Zhenhua 2003-22
   int      m_prevObjType;             // the previous Object Type when propCombo
   int      m_SelectValue;             // the Selecct Index of PropList            Added by Zhu Zhenhua 2003-22
   int      m_VendorPropID;            // the ID of Vendor Property when propCombo      Added by Zhu Zhenhua 2003-22

   VTSEnumeratedCtrl( const CWnd* wp, int id, NetworkSniffer::BACnetStringTable &table, 
                      bool isCombo = false,
                      bool sortStrings = false );// bind to window and control

   // Old-style constructor, deprecated in favor of the constructor taking NetworkSniffer::BACnetStringTable
   VTSEnumeratedCtrl( const CWnd* wp, int id, char **table, int tableSize, 
                      bool isCombo = false,
                      bool sortStrings = false ); // bind to window and control

   void LoadCombo( void );             // initialize combo list
   void SetDropDownSize( UINT lines ); // change the number of lines to display

   void CtrlToObj( void );             // interpret control contents, save in object
   void ObjToCtrl( void );             // object value reflected in control

   void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
   void RestoreCtrl( BACnetAPDUDecoder& dec );  // restore the contents

   // Select an enumerated value, populating and showing the control if necessary
   void SetEnumValue( int theValue );

private:
   void EnumToSelect();
   void SelectToEnum();
};

void SetDropDownSize( CComboBox& box, UINT lines );

//
// VTSUnsignedCtrl
//

class VTSUnsignedCtrl : public VTSCtrl, public BACnetUnsigned {
   public:
      VTSUnsignedCtrl( const CWnd* wp, int id );      // bind to window and control

      void CtrlToObj( void );                   // interpret control contents, save in object
      void ObjToCtrl( void );                   // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSIntegerCtrl
//

class VTSIntegerCtrl : public VTSCtrl, public BACnetInteger {
   public:
      VTSIntegerCtrl( const CWnd* wp, int id );    // bind to window and control

      void CtrlToObj( void );             // interpret control contents, save in object
      void ObjToCtrl( void );             // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSRealCtrl
//

class VTSRealCtrl : public VTSCtrl, public BACnetReal {
   public:
      VTSRealCtrl( const CWnd* wp, int id );    // bind to window and control

      void CtrlToObj( void );             // interpret control contents, save in object
      void ObjToCtrl( void );             // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSDoubleCtrl
//

class VTSDoubleCtrl : public VTSCtrl, public BACnetDouble {
   public:
      VTSDoubleCtrl( const CWnd* wp, int id );     // bind to window and control

      void CtrlToObj( void );             // interpret control contents, save in object
      void ObjToCtrl( void );             // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSCharacterStringCtrl
//

class VTSCharacterStringCtrl : public VTSCtrl, public BACnetCharacterString {
   public:
      bool  emptyIsNull;                  // empty control value is null

      VTSCharacterStringCtrl( const CWnd* wp, int id );  // bind to window and control
      virtual ~VTSCharacterStringCtrl();

      void CtrlToObj( void );                // interpret control contents, save in object
      void ObjToCtrl( void );                // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSOctetStringCtrl
//

class VTSOctetStringCtrl : public VTSCtrl, public BACnetOctetString {
   public:
      bool  emptyIsNull;                  // empty control value is null

      VTSOctetStringCtrl( const CWnd* wp, int id );      // bind to window and control
      virtual ~VTSOctetStringCtrl();

      void CtrlToObj( void );                // interpret control contents, save in object
      void ObjToCtrl( void );                // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSBitStringCtrl
//

class VTSBitStringCtrl : public VTSCtrl, public BACnetBitString {
   public:
      bool  emptyIsNull;                  // empty control value is null

      VTSBitStringCtrl( const CWnd* wp, int id );     // bind to window and control
      ~VTSBitStringCtrl();

      void CtrlToObj( void );                // interpret control contents, save in object
      void ObjToCtrl( void );                // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSDateCtrl
//

class VTSDateCtrl : public VTSCtrl, public BACnetDate {
   public:
      VTSDateCtrl( const CWnd* wp, int id );    // bind to window and control

      void CtrlToObj( void );             // interpret control contents, save in object
      void ObjToCtrl( void );             // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSTimeCtrl
//

class VTSTimeCtrl : public VTSCtrl, public BACnetTime {
   public:
      VTSTimeCtrl( const CWnd* wp, int id );    // bind to window and control

      void CtrlToObj( void );             // interpret control contents, save in object
      void ObjToCtrl( void );             // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSObjectIdentifierCtrl
//

class VTSObjectIdentifierCtrl : public VTSCtrl, public BACnetObjectIdentifier {
   public:
      VTSObjectIdentifierCtrl( const CWnd* wp, int id ); // bind to window and control

      void CtrlToObj( void );             // interpret control contents, save in object
      void ObjToCtrl( void );             // object value reflected in control

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

//
// VTSListCtrl
//

struct VTSListColDesc {
   char* colName;             // column name
   int      colFormat;              // LVCFMT_LEFT, LVCFMT_RIGHT, or LVCFMT_CENTER
   int      colWidth;               // width
   int      colCtrlID;              // ID of matching control
};

class CSendPage;

class VTSListCtrl {
   private:
      bool        listAddInProgress;   // iff new item being added

   public:
      VTSListCtrl( void );
      virtual ~VTSListCtrl( void );

      int            listSelectedRow;  // selected row, -1 iff nothing selected

      CSendPage      *listWindow;      // window containing controls
      CListCtrl      *listCtrl;        // control to oversee
      VTSListColDesc *listItems;       // columns

      void Init( CSendPage* wp, CListCtrl *cp, VTSListColDesc *dp  );   // start empty

      int   GetItemCount( void );                  // from the list
      const char *GetItemText( int row, int col ); //

      virtual const char *GetCtrlText( int ctrlID );
      virtual void SetCtrlText( int ctrlID, const char *text );
      virtual void EnableCtrl( int ctrlID, int enable );

      void AddButtonClick( void );     // add button clicked
      void RemoveButtonClick( void );     // remove button clicked

      void OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult );

      void OnChangeItem( int ctrlID ); // text in control changed
};

//
// VTSStatusFlags
//

class VTSStatusFlags : public BACnetBitString {
   public:
      VTSStatusFlags( const CWnd* wp );         // bind to window and control

      bool     ctrlEnabled;               // controls enabled on page

      const CWnd* ctrlWindow;                // owner window

      void Enable( void );                // enable object and controls
      void Disable( void );                  // disable both

      void InAlarmClick( void );             // In-alarm button clicked
      void FaultClick( void );               // Fault button clicked
      void OverriddenClick( void );          // Overridden button clicked
      void OutOfServiceClick( void );           // Out-of-service button clicked

      void CtrlToObj( void );                // interpret control contents, save in object
      void ObjToCtrl( void );                // object value reflected in control
      void UpdateData( BOOL bCtrlToObj );       // call one of the above

      void SaveCtrl( BACnetAPDUEncoder& enc );     // save the contents
      void RestoreCtrl( BACnetAPDUDecoder& dec );     // restore the contents
};

#endif
