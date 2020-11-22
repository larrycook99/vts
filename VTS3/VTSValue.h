// VTSValue.h: interface for the VTSValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSVALUE_H__95864240_C787_11D6_A613_00A0C9E370F1__INCLUDED_)
#define AFX_VTSVALUE_H__95864240_C787_11D6_A613_00A0C9E370F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxmt.h>

#include "VTSDoc.h"

//
//	VTSValue
//
//	Device objects that are defined in VTS can respond to ReadProperty and WriteProperty
//	requests.  The values that they use are defined here.
//
//	On of the more complicated aspects of BACnet is that the properties of an object can 
//	be an array and the array elements can be complex datatypes.  So for each defined device,
//	the database needs to maintain a list of objects, the properties, each element of those 
//	that are an array, and then the list of application or context encoded primative 
//	data types.
//
//	This code is modeled as if this information was normalized into one table.  Each row in 
//	this virtual table contains the object identifier, property, array index, element index,
//	component, component type, context (for context encoded values), and encoded value for 
//	atomic datatypes.
//
//	This version of JDB does not support a 'table' but it does have a list where all of the 
//	list elements are the same size.  So this 'table' is split into two components, one for 
//	the fixed size 'columns' of the 'table', where the last field is a JDB objId for the 
//	variable length portion.
//
//	Dealing with a flat list like this with no ability to set a context (like a 'view') is
//	painful and tedious.  So the VTSObjectList, VTSObjectPropList, VTSValue, and VTSValueArray
//	are designed to provide some context through their Bind() function.  So the VTSObjectList
//	makes this table look like just a list of BACnet object ID's, and given a specific object,
//	the VTSObjectPropList looks like a list of properties.
//

//======================================================================

#if Mac_Platform
#pragma options align=mac68k
#endif
#if NT_Platform
#pragma pack(push,2)
#endif

//
//	VTSObjPropValue
//
//	The valueIndx is used as a flag to signal that a particular protperty value is not an
//	array (-1), a marker for the beginning of an array (0), or a particular array element (1+).
//

struct VTSObjPropValue {
	unsigned int			valueObjID;				// object identifier, i.e., (analog-input,1)
	int						valueProperty;			// property identifier
	int						valueIndx;				// element index
	int						valueComponent;			// component number
	int						valueType;				// application data type
	int						valueContext;			// context (-1 app encoded, >= 0 context encoded)
	objId					valueElemID;			// element object ID
	};

typedef VTSObjPropValue *VTSObjPropValuePtr;
const int kVTSObjPropValueSize = sizeof( VTSObjPropValue );

//
//	VTSValueElemDesc
//
//	This is the smallest piece of information, a single atomic value.  While the content is
//	defined to be a fairly large array, not all of it is actually saved in the database.  For 
//	null, opening, and closing tags there is no content to save.
//

//MAD_DB  moved to VTSPropValue.h
//const int kVTSValueElemContentLength = 512;

const int kVTSValueElemHeaderLength = kJDBObjSize + sizeof(int);

struct VTSValueElemDesc : JDBObj {
	int						elemLen;				// length of encoded content
	unsigned char			elemContent[kVTSValueElemContentLength];	// encoded value
	};

typedef VTSValueElemDesc *VTSValueElemDescPtr;
const int kVTSValueElemDescSize = sizeof( VTSValueElemDesc );
const int kVTSValueElemHeaderSize = kVTSValueElemDescSize - kVTSValueElemContentLength;

#if Mac_Platform
#pragma options align=reset
#endif
#if NT_Platform
#pragma pack(pop)
#endif

//
//	VTSObjPropValueElemDesc
//
//	This is a wrapper object that makes it easier for operations to deal with both the 
//	fixed and variable length portions together in one structure.
//

struct VTSObjPropValueElemDesc {
	VTSObjPropValue		descValue;
	VTSValueElemDesc	descElement;
	};

typedef VTSObjPropValueElemDesc *VTSObjPropValueElemDescPtr;
const int kVTSObjPropValueElemDescSize = sizeof( VTSObjPropValueElemDesc );

//
//	VTSObjPropValueProc
//
//	Most of the helper routines involve scanning through the list, looking for specific 
//	elements, and changing them.  A function with this signature is passed to ScanList()
//	and as long as it returns true the function will be called with the next element.
//

typedef bool (*VTSObjPropValueProc)( int indx, VTSObjPropValue &val );

//
//	VTSObjPropValueList
//
//	This is a manager class that deals with the details of keeping the fixed and variable 
//	portion of the list synchronized.
//

class VTSObjPropValueList {
	protected:
		int					objPropScanIndx;		// scanning index, see ScanList()

		bool				objPropBusy;			// true iff user editing contents
		CCriticalSection	objPropLock;			// locked when transaction in progress

		void Lock( void );							// wait for exclusive access to the objPropLock
		void Unlock( void );						// release the lock

	public:
		VTSDocPtr			objPropDoc;				// doc for packets
		JDBList				objPropList;			// list of VTSObjPropValue's
		objId				objPropListID;			// ID of JDBList

		VTSObjPropValueList( VTSDocPtr doc, objId listID );
		virtual ~VTSObjPropValueList( void );

		int Length( void );							// number of values

		void InsertElem( int indx, VTSObjPropValue &val );				// add a new value
		void RemoveElem( int indx );									// remove a value

		void ReadElem( int indx, VTSObjPropValue& val );				// copy out from the list
		void WriteElem( int indx, const VTSObjPropValue& val );			// save this information

		void ReadComponent( int indx, VTSObjPropValueElemDesc& desc );
		void WriteComponent( int indx, VTSObjPropValueElemDesc& desc );
		void InsertComponent( int indx, VTSObjPropValueElemDesc& desc );

		int ScanList( VTSObjPropValueProc fp, int startIndx = 0 );		// cycle through the elements

		void Sort( void );							// put the list in sorted order

		void SetBusy( void );						// dialog box going up
		void ResetBusy( void );						// user finished editing

		void Encode( unsigned int objID, int propID, int indx, BACnetAPDUEncoder& enc );	// ReadProperty
		void Decode( unsigned int objID, int propID, int indx, BACnetAPDUDecoder& dec );	// WriteProperty

		CString DescribeValue( int indx );									// build a string that describes the value
		CString DescribeComponent( const VTSObjPropValueElemDesc& desc );	// describe a specific component
	};

typedef VTSObjPropValueList *VTSObjPropValueListPtr;
const int kVTSObjPropValueListSize = sizeof( VTSObjPropValueList );



//
//	VTSObjectList
//

class VTSObjectList {
		friend class VTSObjectListContext;

	protected:
		VTSObjPropValueListPtr				objOPVL;			// bound list pointer
		CList<unsigned int,unsigned int>	objIDList;			// cached list of unique object id's

	public:
		VTSObjectList( VTSObjPropValueListPtr lp );
		virtual ~VTSObjectList( void );

		void Add( void );										// append an object
		void Remove( int indx );								// remove an object

		bool Exists( unsigned int objID );						// true iff object exists
		void Move( unsigned int oldID, unsigned int newID );	// renumber the records

		int Length( void );							// number of unique object id's
		unsigned int operator[]( int indx );		// get the i'th object id
	};

typedef VTSObjectList *VTSObjectListPtr;
const int kVTSObjectListSize = sizeof( VTSObjectList );

//
//	VTSObjectPropDesc
//
//	The VTSObjectList can be pretty simple because the shadow list is just unsigned 
//	integers.  The VTSObjectPropList is more complicated since it needs to keep 
//	track of more information about the property for the user display.
//

struct VTSObjectPropDesc {
	int			propID;			// property identifier
	bool		propIsArray;	// true iff property is an array
	CString		propValue;		// string value of property or "[array]"
	};

typedef VTSObjectPropDesc *VTSObjectPropDescPtr;

//
//	VTSObjectPropList
//

class VTSObjectPropList {
		friend class VTSObjectPropListContext;

	protected:
		VTSObjPropValueListPtr						propOPVL;	// bound list pointer
		CList<VTSObjectPropDesc,VTSObjectPropDesc>	propList;	// cached list of unique properties

	public:
		int											propObjID;	// bound object

		VTSObjectPropList( VTSObjPropValueListPtr lp );
		virtual ~VTSObjectPropList( void );

		void Bind( unsigned int objID );

		void Add( void );							// append a property
		void Remove( int indx );					// remove a property

		bool Exists( int propID );					// true iff property exists
		void Move( int oldID, int newID );			// renumber the records

		void ArrayToValue( int propID );			// convert an array to a single value
		void ValueToArray( int propID );			// convert a value to an array of one element

		void NewDescription( int propID );			// build a new description of a property value

		int Length( void );							// number of properties
		const VTSObjectPropDesc &operator[]( int indx );		// get the i'th property
	};

typedef VTSObjectPropList *VTSObjectPropListPtr;
const int kVTSObjectPropListSize = sizeof( VTSObjectPropList );

//
//	VTSValue
//

class VTSValue {
		friend class VTSValueContext;

	protected:
		VTSObjPropValueListPtr		valueOPVL;			// bound list pointer
		int							valueCompIndex;		// element number of first component
		int							valueCompLen;		// number of defined components

	public:
		unsigned int				valueObjID;			// which object
		int							valuePropID;		// which property
		int							valueArrayIndex;	// array index, -1 not an array

		VTSValue( VTSObjPropValueListPtr lp );
		~VTSValue( void );

		void Bind( unsigned int objID, int propID, int arrayIndex );

		void Add( void );								// append a component
		void Remove( int index );						// remove a component

		int Length( void );								// number of components

		void ReadComponent( int indx, VTSObjPropValueElemDesc& desc );
		void WriteComponent( int indx, VTSObjPropValueElemDesc& desc );
	};

typedef VTSValue *VTSValuePtr;
const int kVTSValueSize = sizeof( VTSValue );

//
//	VTSValueArray
//

class VTSValueArray {
		friend class VTSValueArrayContext;

	protected:
		VTSObjPropValueListPtr		arrayOPVL;		// bound list pointer
		CList<CString,CString&>		arrayText;		// string value of element or "[complex]"

		int							arrayIndex;		// index number of first array element

	public:
		unsigned int				arrayObjID;		// bound object ID
		int							arrayPropID;	// bound property ID

		VTSValueArray( VTSObjPropValueListPtr lp );
		~VTSValueArray( void );

		void Bind( unsigned int objID, int propID );

		void Add( void );							// append an element
		void Remove( int indx );					// remove an element

		void NewDescription( int indx );			// build a new description of a property value

		int Length( void );							// number of elements

		CString& GetText( int indx );				// get the description of the i'th element
		void SetText( int indx, CString& valu );	// update the description
	};

typedef VTSValueArray *VTSValueArrayPtr;
const int kVTSValueArraySize = sizeof( VTSValueArray );

#endif // !defined(AFX_VTSVALUE_H__95864240_C787_11D6_A613_00A0C9E370F1__INCLUDED_)
