// VTSValue.cpp: implementation of the VTSValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSValue.h"

#include <memory.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
//	VTSObjPropValueList::VTSObjPropValueList
//

VTSObjPropValueList::VTSObjPropValueList( VTSDocPtr doc, objId listID )
: objPropDoc(doc), objPropListID(listID), objPropBusy(false)
{
	// load the list
	doc->m_pDB->pListMgr->GetList( objPropList, listID );
}

//
//	VTSObjPropValueList~VTSObjPropValueList
//

VTSObjPropValueList::~VTSObjPropValueList( void )
{
}

//
//	VTSObjPropValueList::Length
//

int VTSObjPropValueList::Length( void )
{
	return objPropList.Length();
}

//
//	VTSObjPropValueList::InsertElem
//

void VTSObjPropValueList::InsertElem( int indx, VTSObjPropValue &val )
{
	objPropList.NewElem( &indx, &val );

	// if inserting before the current scan element, it has moved
	if (objPropScanIndx >= indx)
		objPropScanIndx += 1;
}

//
//	VTSObjPropValueList::RemoveElem
//

void VTSObjPropValueList::RemoveElem( int indx )
{
	objPropList.DeleteElem( indx );

	// if deleting the current scan element, it has 'moved'
	if (objPropScanIndx >= indx)
		objPropScanIndx -= 1;
}

//
//	VTSObjPropValueList::ReadElem
//

void VTSObjPropValueList::ReadElem( int indx, VTSObjPropValue& val )
{
	objPropList.ReadElem( indx, &val );
}

//
//	VTSObjPropValueList::WriteElem
//

void VTSObjPropValueList::WriteElem( int indx, const VTSObjPropValue& val )
{
	objPropList.WriteElem( indx, (void *)&val );
}

//
//	VTSObjPropValueList::ReadComponent
//

void VTSObjPropValueList::ReadComponent( int indx, VTSObjPropValueElemDesc& desc )
{
	// make sure it's not out of bounds
	ASSERT( (indx >= 0) && (indx < objPropList.Length()) );

	// read the current contents
	ReadElem( indx, desc.descValue );

	// if there's an associated object, read it
	if (desc.descValue.valueElemID)
		objPropDoc->m_pDB->pObjMgr->ReadObject( desc.descValue.valueElemID, (JDBObjPtr)&desc.descElement );
	else
		memset( &desc.descElement, 0, sizeof(desc.descElement) );
}

//
//	VTSObjPropValueList::WriteComponent
//

void VTSObjPropValueList::WriteComponent( int indx, VTSObjPropValueElemDesc& desc )
{
	// make sure it's not out of bounds
	ASSERT( (indx >= 0) && (indx < objPropList.Length()) );

	// if there is an element ID but the new content length is zero, delete the object
	if (desc.descValue.valueElemID && (desc.descElement.elemLen == 0)) {
		// delete the object
		objPropDoc->m_pDB->pObjMgr->DeleteObject( desc.descValue.valueElemID );

		// no more object
		desc.descValue.valueElemID = 0;
	}

	// calculate the object size correctly
	int objSize = kVTSValueElemHeaderLength + desc.descElement.elemLen;		// expected size
	objSize += (objSize & 1);												// must be even

	// make sure the element object size is correct
	desc.descElement.objSize = objSize;

	// if there is no element ID but the new content length is non-zero, create a new object
	if (!desc.descValue.valueElemID && (desc.descElement.elemLen != 0)) {
		// allocate a new object
		objPropDoc->m_pDB->pObjMgr->NewObject( &desc.descValue.valueElemID, (JDBObjPtr)&desc.descElement, objSize );

		// initialize the element object header
		desc.descElement.objID = (unsigned char)(desc.descValue.valueElemID & 0xFF);
		desc.descElement.objType = 0;
	}

	// write the current contents
	WriteElem( indx, desc.descValue );

	// if there's an associated object, write it
	if (desc.descValue.valueElemID)
		objPropDoc->m_pDB->pObjMgr->WriteObject( desc.descValue.valueElemID, (JDBObjPtr)&desc.descElement );
}

//
//	VTSObjPropValueList::InsertComponent
//

void VTSObjPropValueList::InsertComponent( int indx, VTSObjPropValueElemDesc& desc )
{
	// calculate the object size correctly
	int objSize = kVTSValueElemHeaderLength + desc.descElement.elemLen;		// expected size
	objSize += (objSize & 1);												// must be even

	// if content length is non zero, create and save an object
	if (desc.descElement.elemLen != 0) {
		// allocate a new object
		objPropDoc->m_pDB->pObjMgr->NewObject( &desc.descValue.valueElemID, (JDBObjPtr)&desc.descElement, objSize );

		// save it
		objPropDoc->m_pDB->pObjMgr->WriteObject( desc.descValue.valueElemID, (JDBObjPtr)&desc.descElement );
	} else
		desc.descValue.valueElemID = 0;

	// insert the value
	InsertElem( indx, desc.descValue );
}

//
//	VTSObjPropValueList::ScanList
//
//	Most of the helper functions involve scanning the list for specific elements.  If the helper 
//	wants to modify the element and write it back, that's simple to support.  If the helper thinks 
//	the current location is the correct spot to add a new element, it should be able to call 
//	InsertElem() and not interfere with the rest of the scanning of the list (if that's desireable).
//
//	If the helper function returns true, the scanning process will continue.  If it returns false, 
//	ScanList will return the index of the element that was 'found'.  If the helper never returns 
//	false, ScanList will return -1 as an indication that all of the elements (and there may be none)
//	were scanned.
//

int VTSObjPropValueList::ScanList( VTSObjPropValueProc fp, int startIndx )
{
	VTSObjPropValue		val
	;

	objPropScanIndx = startIndx;

	// read in the elements and pass them along
	while (objPropScanIndx < objPropList.Length()) {
		objPropList.ReadElem( objPropScanIndx, &val );
		if (!(*fp)( objPropScanIndx, val ))
			return objPropScanIndx;

		// move on to the next element
		objPropScanIndx += 1;
	}

	// end of list reached
	return -1;
}

//
//	VTSObjPropValueList::Sort
//

int VTSObjPropValueListSortHelper( VTSObjPropValuePtr ap, VTSObjPropValuePtr bp );

void VTSObjPropValueList::Sort( void )
{
	objPropList.Sort( (JDBListCompFnPtr)VTSObjPropValueListSortHelper );
}

int VTSObjPropValueListSortHelper( VTSObjPropValuePtr ap, VTSObjPropValuePtr bp )
{
	if (ap->valueObjID < bp->valueObjID) return -1;
	if (ap->valueObjID > bp->valueObjID) return 1;

	if (ap->valueProperty < bp->valueProperty) return -1;
	if (ap->valueProperty > bp->valueProperty) return 1;

	if (ap->valueIndx < bp->valueIndx) return -1;
	if (ap->valueIndx > bp->valueIndx) return 1;

	if (ap->valueComponent < bp->valueComponent) return -1;
	if (ap->valueComponent > bp->valueComponent) return 1;

	// should never get here!
	return 0;
}

//
//	VTSObjPropValueList::SetBusy
//

void VTSObjPropValueList::SetBusy( void )
{
	CSingleLock lock( &objPropLock )
	;
	
	// make sure no other threads are mucking around
	lock.Lock();

	// user is editing list
	objPropBusy = true;

	// be nice and release it before returning
	lock.Unlock();
}

//
//	VTSObjPropValueList::ResetBusy
//

void VTSObjPropValueList::ResetBusy( void )
{
	CSingleLock lock( &objPropLock )
	;
	
	// make sure no other threads are mucking around
	lock.Lock();

	// user has finished editing
	objPropBusy = false;

	// be nice and release it before returning
	lock.Unlock();
}

//
//	VTSObjPropValueList::Encode
//

void VTSObjPropValueList::Encode( unsigned int objID, int propID, int indx, BACnetAPDUEncoder& enc )
{
	CSingleLock					lock( &objPropLock )
	;
	int							i, elemCount
	;
	VTSObjPropValue				val
	;
	VTSObjPropValueElemDesc		desc
	;

	// make sure no other threads are mucking around
	lock.Lock();

	// if the user is editing, toss out "device configuration in progress" error
	if (objPropBusy)
		throw (2);

	// look for the start of the obj/prop/indx
	for (i = 0; i < objPropList.Length(); i++) {
		objPropList.ReadElem( i, &val );
		if ((val.valueObjID == objID) && (val.valueProperty == propID))
			break;
	}

	// check to see if we found something
	if ((i >= objPropList.Length()) || (val.valueObjID != objID))
		throw (31); // unknown-object
	if (val.valueProperty != propID)
		throw (32); // unknown-property

	// case 1: looking for a value, got a value
	if (indx == -1) {
		// looking for the whole thing
		if (val.valueIndx == -1) {
			// got a simple value, return all of it
		} else
		if (val.valueIndx == 0) {
			// got an array, skip over the header, return the whole thing
			i += 1;
		} else
			throw (25); // operational-problem, database configuration error

		// copy the whole thing
		for (; i < objPropList.Length(); i++) {
			ReadComponent( i, desc );
			if ((val.valueObjID != objID) || (val.valueProperty != propID))
				break;

			enc.Append( desc.descElement.elemContent, desc.descElement.elemLen );
		}
	} else
	if (indx == 0) {
		// looking for the length of an array
		if (val.valueIndx == -1)
			throw (7); // inconsistent-parameters, can't index a simple value
		else
		if (val.valueIndx != 0)
			throw (25); // operational-problem, database configuration error

		// got an array, skip the marker
		i += 1;

		// count the number of elements
		elemCount = 0;
		for (; i < objPropList.Length(); i++) {
			objPropList.ReadElem( i, &val );
			if ((val.valueObjID != objID) || (val.valueProperty != propID))
				break;
			if (val.valueIndx > elemCount)
				elemCount += 1;
		}

		// encode the count
		BACnetUnsigned( elemCount ).Encode( enc );
	} else {
		// looking for the length of an array
		if (val.valueIndx == -1)
			throw (7); // inconsistent-parameters, can't index a simple value
		else
		if (val.valueIndx == 0) {
			// skip the marker
			i += 1;

			// scan for a specific index
			int found = 0;
			for (; i < objPropList.Length(); i++) {
				objPropList.ReadElem( i, &val );
				if ((val.valueObjID != objID) || (val.valueProperty != propID) || (val.valueIndx > indx))
					break;

				if (val.valueIndx == indx) {
					found = 1;
					break;
				}
			}

			// make sure we've got something
			if (!found)
				throw (42); // invalid-array-index

			// copy the whole thing
			for (; i < objPropList.Length(); i++) {
				ReadComponent( i, desc );
				if ((val.valueObjID != objID) || (val.valueProperty != propID) || (val.valueIndx != indx))
					break;

				enc.Append( desc.descElement.elemContent, desc.descElement.elemLen );
			}
		} else
			throw (25); // operational-problem, database configuration error
	}

	// be nice and release it before returning
	lock.Unlock();
}

//
//	VTSObjPropValueList::Decode
//

void VTSObjPropValueList::Decode( unsigned int objID, int propID, int indx, BACnetAPDUDecoder& dec )
{
	CSingleLock					lock( &objPropLock )
	;
	int							i, fini = 0, nest = 0, elemCount
	;
	VTSObjPropValue				val
	;
	VTSObjPropValueElemDesc		desc
	;

	// make sure no other threads are mucking around
	lock.Lock();

	// if the user is editing, toss out "device configuration in progress" error
	if (objPropBusy)
		throw (2);

	// look for the start of the obj/prop/indx
	for (i = 0; i < objPropList.Length(); i++) {
		objPropList.ReadElem( i, &val );
		if ((val.valueObjID == objID) && (val.valueProperty == propID))
			break;
	}

	// check to see if we found something
	if ((i >= objPropList.Length()) || (val.valueObjID != objID))
		throw (31); // unknown-object
	if (val.valueProperty != propID)
		throw (32); // unknown-property

	// case 1: looking for a value, got a value
	if (indx == -1) {
		// looking for the whole thing
		if (val.valueIndx != -1)
			throw (25); // operational-problem, can't write whole array

		// remove the existing content
		while (i < objPropList.Length()) {
			objPropList.ReadElem( i, &val );
			if ((val.valueObjID == objID) && (val.valueProperty == propID))
				objPropList.DeleteElem( i );
			else
				break;
		}
	} else
	if (indx == 0) {
		// looking for the length of an array
		if (val.valueIndx == -1)
			throw (7); // inconsistent-parameters, can't index a simple value
		else
		if (val.valueIndx != 0)
			throw (25); // operational-problem, database configuration error

		// got an array, change the size
		BACnetUnsigned	newLen;
		newLen.Decode( dec );

		// check for valid value
		if (newLen.uintValue < 0)
			throw (42); // invalid-array-index

		// skip over existing content that will remain
		elemCount = 0;
		while (i < objPropList.Length()) {
			objPropList.ReadElem( i, &val );
			if ((val.valueObjID != objID) || (val.valueProperty != propID) || (val.valueIndx > (int)newLen.uintValue))
				break;
			if (val.valueIndx > elemCount)
				elemCount += 1;
			i += 1;
		}

		if (elemCount > (int)newLen.uintValue)
			throw (25); // operational-problem, database configuration error

		if (elemCount == (int)newLen.uintValue) {
			// remove the rest of the content
			while (i < objPropList.Length()) {
				objPropList.ReadElem( i, &val );
				if ((val.valueObjID == objID) && (val.valueProperty == propID))
					objPropList.DeleteElem( i );
				else
					break;
			}
		} else {
			// add more elements to fill out the array
			elemCount += 1;

			// the major section will be the same
			desc.descValue.valueObjID = objID;
			desc.descValue.valueProperty = propID;	// property identifier
			desc.descValue.valueComponent = 0;		// only one component
			desc.descValue.valueType = 0;			// null
			desc.descValue.valueContext = -1;		// application encoded
			desc.descElement.elemLen = 1;			// one octet
			desc.descElement.elemContent[0] = 0x00;	// application encoded null

			// write them out
			while (elemCount <= (int)newLen.uintValue) {
				// set the index
				desc.descValue.valueIndx = elemCount++;

				// save what we just built
				InsertComponent( i++, desc );
			}

			// dont try to put in more content
			fini = 1;
		}
	} else {
		// looking for a specific element
		if (val.valueIndx == -1)
			throw (7); // inconsistent-parameters, can't index a simple value
		else
		if (val.valueIndx == 0) {
			// skip the marker
			i += 1;

			// scan for a specific index
			int found = 0;
			for (; i < objPropList.Length(); i++) {
				objPropList.ReadElem( i, &val );
				if ((val.valueObjID != objID) || (val.valueProperty != propID) || (val.valueIndx > indx))
					break;

				if (val.valueIndx == indx) {
					found = 1;
					break;
				}
			}

			// make sure we've got something
			if (!found)
				throw (42); // invalid-array-index

			// remove the existing content
			while (i < objPropList.Length()) {
				objPropList.ReadElem( i, &val );
				if ((val.valueObjID == objID) && (val.valueProperty == propID) && (val.valueIndx == indx))
					objPropList.DeleteElem( i );
				else
					break;
			}
		} else
			throw (25); // operational-problem, database configuration error
	}

	// put in the new stuff
	BACnetAPDUTag	t
	;

	// the major section will be the same
	desc.descValue.valueObjID = objID;
	desc.descValue.valueProperty = propID;	// property identifier
	desc.descValue.valueIndx = indx;		// element index

	// first piece will be component 0
	desc.descValue.valueComponent = 0;

	// scan through the rest of the data
	while (!fini && (dec.pktLength > 0)) {
		try {
			// get a peek at the tag
			dec.ExamineTag( t );

			switch (t.tagClass) {
				case contextTagClass:
					// set this as an octet string.  Because it is context tagged, the datatype connot
					// be determined, the context isn't maintained.  It could be, because we know the 
					// object type and property, and we could even validate the datatype along the 
					// way, but that will be saved for future work.
					desc.descValue.valueType = octetStringAppTag;
					desc.descValue.valueContext = (int)t.tagNumber;
					break;
					
				case openingTagClass:
					// nested
					nest++;

					desc.descValue.valueType = 13;	// opening tag
					desc.descValue.valueContext = (int)t.tagNumber;
					break;
					
				case closingTagClass:
					// not nested so deep
					if (!nest--) {
						fini = 1;
						break;
					}

					desc.descValue.valueType = 14;	// closing tag
					desc.descValue.valueContext = (int)t.tagNumber;
					break;
				
				case applicationTagClass:
					desc.descValue.valueType = t.tagNumber;	// tag number matches type
					desc.descValue.valueContext = -1;
					break;
			}
		}
		catch (...) {
			// decoding error
			break;
		}

		// done yet?
		if (fini)
			break;

		// copy the tag and data to the element
		desc.descElement.elemLen = dec.ExtractTagData( desc.descElement.elemContent );
		
		// save what we just built
		InsertComponent( i++, desc );

		// get ready for the next component
		desc.descValue.valueComponent += 1;
	}

	// be nice and release it before returning
	lock.Unlock();
}

//
//	VTSObjPropValueList::DescribeValue
//
//	This member function builds a description of a value, even when there is more than
//	one component.  To keep the strings short there are no spaces included, and only a comma ',' is 
//	used to separate elements.
//

CString VTSObjPropValueList::DescribeValue( int indx )
{
	VTSObjPropValueElemDesc		first, cur
	;
	CString						rslt
	;
	bool						needsComma
	;

	// read the first component
	ReadComponent( indx, first );

	// check for an array
	if (first.descValue.valueIndx == 0)
		return _T("[array]");

	// turn it into a description
	rslt = DescribeComponent( first );
	needsComma = (first.descValue.valueType != 13);				// openTag

	// read the rest of the components and add them
	for (;;) {
		indx += 1;
		if (indx >= objPropList.Length())
			break;

		// read the component
		ReadComponent( indx, cur );

		// make sure it's part of the same value
		if (cur.descValue.valueObjID != first.descValue.valueObjID) break;
		if (cur.descValue.valueProperty != first.descValue.valueProperty) break;
		if (cur.descValue.valueIndx != first.descValue.valueIndx) break;

		// get a description of it
		CString comp = DescribeComponent( cur );

		// figure out the length
		int rsltLen = rslt.GetLength();
		int compLen = comp.GetLength();

		// get a buffer big enough to hold the results
		LPTSTR p = rslt.GetBuffer( rsltLen + compLen + 3 );

		// comma seaparated list of values
		if (needsComma && (cur.descValue.valueType != 14))		// closeTag
			lstrcat( p, _T(",") );

		// append the description
		lstrcat( p, comp );

		// release the buffer
		rslt.ReleaseBuffer();

		needsComma = (cur.descValue.valueType != 13);			// openTag
	}

	// all done
	return rslt;
}

//
//	VTSObjPropValueList::DescribeComponent
//
//	This member function builds a description of a component.  Opening tags are encoded as '(', closing 
//	tags as ')', and all the rest of the data types are encoded as you would expect.
//

CString VTSObjPropValueList::DescribeComponent( const VTSObjPropValueElemDesc& desc )
{
	CString				rslt
	;
	BACnetAPDUDecoder	dec( desc.descElement.elemContent, desc.descElement.elemLen )
	;

	// depending on the type, decode the value
	switch (desc.descValue.valueType) {
		case 0: {
					rslt = _T( "null" );
					break;
				}
		case 1: {
					BACnetBoolean	m_Boolean
					;

					m_Boolean.Decode( dec );
					m_Boolean.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 2: {
					BACnetUnsigned	m_Unsigned
					;

					m_Unsigned.Decode( dec );
					m_Unsigned.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 3: {
					BACnetInteger	m_Integer
					;

					m_Integer.Decode( dec );
					m_Integer.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 4: {
					BACnetReal		m_Real
					;

					m_Real.Decode( dec );
					m_Real.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 5: {
					BACnetDouble	m_Double
					;

					m_Double.Decode( dec );
					m_Double.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 6: {
					BACnetOctetString	m_OctetStr
					;

					m_OctetStr.Decode( dec );
					m_OctetStr.Encode( rslt.GetBuffer(1024) );
					break;
				}
		case 7: {
					BACnetCharacterString	m_CharStr
					;

					m_CharStr.Decode( dec );
					m_CharStr.Encode( rslt.GetBuffer(1024) );
					break;
				}
		case 8: {
					BACnetBitString	m_BitStr
					;

					m_BitStr.Decode( dec );
					m_BitStr.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 9: {
					BACnetEnumerated	m_Enumerated
					;

					m_Enumerated.Decode( dec );
					m_Enumerated.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 10: {
					BACnetDate	m_Date
					;

					m_Date.Decode( dec );
					m_Date.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 11: {
					BACnetTime	m_Time
					;

					m_Time.Decode( dec );
					m_Time.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 12: {
					BACnetObjectIdentifier	m_ObjID
					;

					m_ObjID.Decode( dec );
					m_ObjID.Encode( rslt.GetBuffer(16) );
					break;
				}
		case 13: {
					rslt = _T( "(" );
					break;
				}
		case 14: {
					rslt = _T( ")" );
					break;
				}
	}

	// release the buffer
	rslt.ReleaseBuffer( );

	// all done
	return rslt;
}






//
//	VTSObjectListContext
//
//	The VTSObjectList member functions need a context for doing their work, and this is a context 
//	control class.  Note that all of the members are private, which should reduce namespace 
//	polution and keep the number of global variables to a minimum.  Since C++ is not a block 
//	structure language like Algol or Pascal, this information needs to be stored in a separate 
//	object.
//

class VTSObjectListContext {
		friend class VTSObjectList;

		VTSObjectListPtr	contextObjListPtr;
		objId				contextObjID;
		objId				contextOldID;
		objId				contextNewID;

		static bool CtorHelper( int indx, VTSObjPropValue &val );
		static bool RemoveHelper( int indx, VTSObjPropValue &val );
		static bool MoveHelper( int indx, VTSObjPropValue &val );
	};

VTSObjectListContext gVTSObjectListContext;

//
//	VTSObjectList::VTSObjectList
//
//	This object list looks like just a list of the unique BACnetObjectIdentifiers in the list.  In 
//	fact it maintains a shadow as if that was the case.
//

VTSObjectList::VTSObjectList( VTSObjPropValueListPtr lp )
	: objOPVL(lp)
{
	// set up the context
	gVTSObjectListContext.contextObjListPtr = this;
	gVTSObjectListContext.contextObjID = -1;

	// scan the list looking for unique ID's
	objOPVL->ScanList( VTSObjectListContext::CtorHelper );
}

//
//	VTSObjectListContext::CtorHelper
//
//	If a new ID is found, add it to the end of the list, set the context so the new one 
//	isn't added more than once, and continue searching.
//

bool VTSObjectListContext::CtorHelper( int indx, VTSObjPropValue &val )
{
	if (val.valueObjID != gVTSObjectListContext.contextObjID) {
		// add this to the list we are building
		gVTSObjectListContext.contextObjListPtr->objIDList.AddTail( val.valueObjID );

		// dont add this one again
		gVTSObjectListContext.contextObjID = val.valueObjID;
	}

	// keep searching
	return true;
}

//
//	VTSObjectList::~VTSObjectList
//

VTSObjectList::~VTSObjectList( void )
{
}

//
//	VTSObjectList::Add
//

void VTSObjectList::Add( void )
{
	int					len = objOPVL->Length()
	;
	VTSObjPropValue		val
	;

	// first element?
	if (len == 0)
		val.valueObjID = 0;
	else {
		objOPVL->ReadElem( len - 1, val );
		val.valueObjID += 1;
	}

	// initialize the rest of the fields
	val.valueProperty = 75;			// object-identifier
	val.valueIndx = -1;				// not an array, just a value
	val.valueComponent = 0;			// only one component
	val.valueType = 0;				// null
	val.valueContext = -1;			// application encoded
	val.valueElemID = 0;			// no more data

	// add it to the list
	objOPVL->InsertElem( len, val );

	// add it to the shadow list
	objIDList.AddTail( val.valueObjID );
}

//
//	VTSObjectList::Remove
//

void VTSObjectList::Remove( int indx )
{
	// set up the context, get the ID
	gVTSObjectListContext.contextObjListPtr = this;
	gVTSObjectListContext.contextObjID = operator []( indx );

	// remove it from the shadow list
	objIDList.RemoveAt(objIDList.FindIndex(indx));

	// scan through the list and remove them all
	objOPVL->ScanList( VTSObjectListContext::RemoveHelper );
}

//
//	VTSObjectListContext::RemoveHelper
//
//	Remove the record if the ID's match.
//

bool VTSObjectListContext::RemoveHelper( int indx, VTSObjPropValue &val )
{
	if (val.valueObjID == gVTSObjectListContext.contextObjID) {
		// remove it from the list
		gVTSObjectListContext.contextObjListPtr->objOPVL->RemoveElem( indx );
	}

	// keep searching
	return true;
}

//
//	VTSObjectList::Exists
//
//	True iff object ID already exists.  This is helpful when the object ID is changing,
//	the user is told that the object already exists.
//

bool VTSObjectList::Exists( unsigned int objID )
{
	POSITION pos = objIDList.Find( objID )
	;

	// if pos is NULL the value was not found
	return (pos != NULL);
}

//
//	VTSObjectList::Move
//

void VTSObjectList::Move( unsigned int oldID, unsigned int newID )
{
	POSITION pos = objIDList.Find( oldID )
	;

	// if the old one isn't found this should probably throw something
	if (pos == NULL)
		return;

	// change the value
	objIDList.SetAt( pos, newID );

	// set up the context
	gVTSObjectListContext.contextObjListPtr = this;
	gVTSObjectListContext.contextOldID = oldID;
	gVTSObjectListContext.contextNewID = newID;

	// scan the list looking for unique ID's
	objOPVL->ScanList( VTSObjectListContext::MoveHelper );
}

//
//	VTSObjectListContext::MoveHelper
//

bool VTSObjectListContext::MoveHelper( int indx, VTSObjPropValue &val )
{
	if (val.valueObjID == gVTSObjectListContext.contextOldID) {
		// get this record its new ID
		val.valueObjID = gVTSObjectListContext.contextNewID;

		// save this record
		gVTSObjectListContext.contextObjListPtr->objOPVL->WriteElem( indx, val );
	}

	// keep searching
	return true;
}

//
//	VTSObjectList::Length
//

int VTSObjectList::Length( void )
{
	return objIDList.GetCount();
}

//
//	VTSObjectList::operator[]
//

unsigned int VTSObjectList::operator[]( int indx )
{
	POSITION pos = objIDList.FindIndex( indx )
	;

	// make sure it's valid
	ASSERT( pos != NULL );

	// return the object ID
	return objIDList.GetAt(pos);
}

//
//	VTSObjectPropListContext
//
//	Same design pattern as VTSObjectListContext.
//

class VTSObjectPropListContext {
		friend class VTSObjectPropList;

		VTSObjectPropListPtr	contextObjListPtr;
		objId					contextObjID;
		int						contextPropID;
		int						contextOldID;
		int						contextNewID;
		int						contextIndx;

		static bool BindHelper( int indx, VTSObjPropValue &val );
		static bool AddHelper( int indx, VTSObjPropValue &val );
		static bool RemoveHelper( int indx, VTSObjPropValue &val );
		static bool MoveHelper( int indx, VTSObjPropValue &val );
		static bool ArrayToValueHelper( int indx, VTSObjPropValue &val );
		static bool ValueToArrayHelper( int indx, VTSObjPropValue &val );
		static bool NewDescriptionHelper( int indx, VTSObjPropValue &val );
	};

VTSObjectPropListContext gVTSObjectPropListContext;

//
//	VTSObjectPropList::VTSObjectPropList
//

VTSObjectPropList::VTSObjectPropList( VTSObjPropValueListPtr lp )
	: propOPVL(lp), propObjID(-1)
{
}

//
//	VTSObjectPropList::~VTSObjectPropList
//

VTSObjectPropList::~VTSObjectPropList( void )
{
}

//
//	VTSObjectPropList::Bind
//

void VTSObjectPropList::Bind( unsigned int objID )
{
	// set the bound object ID
	propObjID = objID;

	// set up the context
	gVTSObjectPropListContext.contextObjListPtr = this;
	gVTSObjectPropListContext.contextObjID = propObjID;
	gVTSObjectPropListContext.contextPropID = -1;

	// clear out all the existing properties
	propList.RemoveAll();

	// scan the list looking for unique ID's
	propOPVL->ScanList( VTSObjectPropListContext::BindHelper );
}

//
//	VTSObjectPropListContext::BindHelper
//
//	If a new property ID is found, add it to the end of the list, set the context so the new one 
//	isn't added more than once, and continue searching.
//

bool VTSObjectPropListContext::BindHelper( int indx, VTSObjPropValue &val )
{
	VTSObjectPropDesc	desc
	;

	// match the current object and a new property
	if ((val.valueObjID == gVTSObjectPropListContext.contextObjID) 
			&& (val.valueProperty != gVTSObjectPropListContext.contextPropID)) {
		// build a descriptor
		desc.propID = val.valueProperty;
		desc.propIsArray = (val.valueIndx == 0);
		desc.propValue = gVTSObjectPropListContext.contextObjListPtr->propOPVL->DescribeValue( indx );

		// add this to the list we are building
		gVTSObjectPropListContext.contextObjListPtr->propList.AddTail( desc );

		// dont add this one again
		gVTSObjectPropListContext.contextPropID = val.valueProperty;
	}

	// keep searching
	return true;
}

//
//	VTSObjectPropList::Add
//

void VTSObjectPropList::Add( void )
{
	int					indx
	;
	VTSObjectPropDesc	desc
	;
	VTSObjPropValue		val
	;
	unsigned int		propID
	;

	// set up the context
	gVTSObjectPropListContext.contextObjListPtr = this;
	gVTSObjectPropListContext.contextObjID = propObjID;
	gVTSObjectPropListContext.contextPropID = -1;

	// scan through the list and look for the right place
	indx = propOPVL->ScanList( VTSObjectPropListContext::AddHelper );

	// if no place to insert the new record was found, add it to the end
	if (indx < 0)
		indx = propOPVL->Length();

	// pick the next property
	propID = gVTSObjectPropListContext.contextPropID + 1;

	// make sure it hasn't already been used
	while (Exists(propID))
		propID += 1;

	// initialize the fields
	val.valueObjID = propObjID;
	val.valueProperty = propID;
	val.valueIndx = -1;				// not an array, just a value
	val.valueComponent = 0;			// only one component
	val.valueType = 0;				// null
	val.valueContext = -1;			// application encoded
	val.valueElemID = 0;			// no more data

	// add it to the list
	propOPVL->InsertElem( indx, val );

	// initialize a descriptor
	desc.propID = propID;
	desc.propIsArray = false;
	desc.propValue = _T("null");

	// add it to the shadow list
	propList.AddTail( desc );
}

//
//	VTSObjectPropListContext::AddHelper
//

bool VTSObjectPropListContext::AddHelper( int indx, VTSObjPropValue &val )
{
	// match the current object
	if (val.valueObjID == gVTSObjectPropListContext.contextObjID) {
		// look for the maximum property
		if (val.valueProperty > gVTSObjectPropListContext.contextPropID)
			gVTSObjectPropListContext.contextPropID = val.valueProperty;
	} else
		if (gVTSObjectPropListContext.contextPropID >= 0)
			return false;

	// keep searching
	return true;
}

//
//	VTSObjectPropList::Remove
//

void VTSObjectPropList::Remove( int indx )
{
	// set up the context, get the ID
	gVTSObjectPropListContext.contextObjListPtr = this;
	gVTSObjectPropListContext.contextObjID = propObjID;
	gVTSObjectPropListContext.contextPropID = operator []( indx ).propID;

	// remove it from the shadow list
	propList.RemoveAt(propList.FindIndex(indx));

	// scan through the list and remove them all
	propOPVL->ScanList( VTSObjectPropListContext::RemoveHelper );
}

//
//	VTSObjectPropListContext::RemoveHelper
//
//	Remove the record if the ID's match.
//

bool VTSObjectPropListContext::RemoveHelper( int indx, VTSObjPropValue &val )
{
	// match the current object and property
	if ((val.valueObjID == gVTSObjectPropListContext.contextObjID) 
			&& (val.valueProperty == gVTSObjectPropListContext.contextPropID)) {
		// remove it from the list
		gVTSObjectPropListContext.contextObjListPtr->propOPVL->RemoveElem( indx );
	}

	// keep searching
	return true;
}

//
//	VTSObjectPropList::Exists
//

bool VTSObjectPropList::Exists( int propID )
{
	bool		found
	;
	POSITION	pos = NULL
	;

	// find the position of the oldID
	found = false;
	pos = propList.GetHeadPosition();
	while (pos != NULL)
		if (propList.GetNext(pos).propID == propID) {
			found = true;
			break;
		}

	// return result
	return (found);
}

//
//	VTSObjectPropList::Move
//

void VTSObjectPropList::Move( int oldID, int newID )
{
	bool				found
	;
	POSITION			pos, foundPos = NULL
	;
	VTSObjectPropDesc	desc
	;

	// find the position of the oldID
	found = false;
	pos = propList.GetHeadPosition();
	while (pos != NULL) {
		foundPos = pos;
		desc = propList.GetNext(pos);
		if (desc.propID == oldID) {
			found = true;
			break;
		}
	}

	// if the old one isn't found this should probably throw something
	if (!found)
		return;

	// update the current contents
	desc.propID = newID;

	// change the shadow value
	propList.SetAt( foundPos, desc );

	// set up the context
	gVTSObjectPropListContext.contextObjListPtr = this;
	gVTSObjectPropListContext.contextObjID = propObjID;
	gVTSObjectPropListContext.contextOldID = oldID;
	gVTSObjectPropListContext.contextNewID = newID;

	// scan the list looking for unique ID's
	propOPVL->ScanList( VTSObjectPropListContext::MoveHelper );
}

//
//	VTSObjectPropListContext::MoveHelper
//

bool VTSObjectPropListContext::MoveHelper( int indx, VTSObjPropValue &val )
{
	// match the current object and property
	if ((val.valueObjID == gVTSObjectPropListContext.contextObjID) 
			&& (val.valueProperty == gVTSObjectPropListContext.contextOldID)) {
		// get this record its new ID
		val.valueProperty = gVTSObjectPropListContext.contextNewID;

		// save this record
		gVTSObjectPropListContext.contextObjListPtr->propOPVL->WriteElem( indx, val );
	}

	// keep searching
	return true;
}

//
//	VTSObjectPropList::ArrayToValue
//
//	Translating an array to a value consists of removing the array marker, changing the 
//	valueIndx of the first element (1) to (-1) so that it becomes a simple value, and
//	deleting all the rest of the array elements.
//

void VTSObjectPropList::ArrayToValue( int propID )
{
	// set up the context
	gVTSObjectPropListContext.contextObjListPtr = this;
	gVTSObjectPropListContext.contextObjID = propObjID;
	gVTSObjectPropListContext.contextPropID = propID;
	gVTSObjectPropListContext.contextIndx = -1;

	// scan the list looking for array elements
	propOPVL->ScanList( VTSObjectPropListContext::ArrayToValueHelper );

	// the property must have some value, but if the array was empty there wouldn't
	// be anything that marks that the property exists.  The old location of the 
	// array marker is a good spot to insert a 'null'.
	if (gVTSObjectPropListContext.contextIndx >= 0) {
		VTSObjPropValue		val
		;

		// initialize the fields
		val.valueObjID = propObjID;
		val.valueProperty = propID;
		val.valueIndx = -1;				// not an array, just a value
		val.valueComponent = 0;			// only one component
		val.valueType = 0;				// null
		val.valueContext = -1;			// application encoded
		val.valueElemID = 0;			// no more data

		// make it the property value
		propOPVL->InsertElem( gVTSObjectPropListContext.contextIndx, val );
	}
}

//
//	VTSObjectPropListContext::ArrayToValueHelper
//

bool VTSObjectPropListContext::ArrayToValueHelper( int indx, VTSObjPropValue &val )
{
	// match the current object and property
	if ((val.valueObjID != gVTSObjectPropListContext.contextObjID)
			|| (val.valueProperty != gVTSObjectPropListContext.contextPropID))
		return true;
	
	// save the position of the array marker
	if (val.valueIndx == 0)
		gVTSObjectPropListContext.contextIndx = indx;

	if (val.valueIndx == 1) {
		// this is no longer an array
		val.valueIndx = -1;

		// save this record
		gVTSObjectPropListContext.contextObjListPtr->propOPVL->WriteElem( indx, val );

		// reset the "add a value" flag
		gVTSObjectPropListContext.contextIndx = -1;
	} else {
		// remove this record
		gVTSObjectPropListContext.contextObjListPtr->propOPVL->RemoveElem( indx );
	}

	// keep searching
	return true;
}

//
//	VTSObjectPropList::ValueToArray
//

void VTSObjectPropList::ValueToArray( int propID )
{
	VTSObjPropValue		val
	;

	// set up the context
	gVTSObjectPropListContext.contextObjListPtr = this;
	gVTSObjectPropListContext.contextObjID = propObjID;
	gVTSObjectPropListContext.contextPropID = propID;
	gVTSObjectPropListContext.contextIndx = -1;

	// scan the list looking for array elements
	propOPVL->ScanList( VTSObjectPropListContext::ValueToArrayHelper );

	// make sure there is a place to insert the marker
	ASSERT( gVTSObjectPropListContext.contextIndx >= 0 );

	// initialize the fields for a marker
	val.valueObjID = propObjID;
	val.valueProperty = propID;
	val.valueIndx = 0;				// not an array, just a value
	val.valueComponent = 0;			// only one component
	val.valueType = 0;				// null
	val.valueContext = 0;			// application encoded
	val.valueElemID = 0;			// no more data

	// add it to the list
	propOPVL->InsertElem( gVTSObjectPropListContext.contextIndx, val );
}

//
//	VTSObjectPropListContext::ValueToArrayHelper
//

bool VTSObjectPropListContext::ValueToArrayHelper( int indx, VTSObjPropValue &val )
{
	// match the current object and property
	if ((val.valueObjID != gVTSObjectPropListContext.contextObjID)
			|| (val.valueProperty != gVTSObjectPropListContext.contextPropID))
		return true;
	
	// check for value components
	if (val.valueIndx == -1) {
		// first component?
		if (gVTSObjectPropListContext.contextIndx < 0)
			gVTSObjectPropListContext.contextIndx = indx;

		// make this the first array element
		val.valueIndx = 1;

		// save this record
		gVTSObjectPropListContext.contextObjListPtr->propOPVL->WriteElem( indx, val );
	}

	// keep searching
	return true;
}

//
//	VTSObjectPropList::NewDescription
//

void VTSObjectPropList::NewDescription( int propID )
{
	// set up the context
	gVTSObjectPropListContext.contextObjListPtr = this;
	gVTSObjectPropListContext.contextObjID = propObjID;
	gVTSObjectPropListContext.contextPropID = propID;

	// scan the list looking for property
	propOPVL->ScanList( VTSObjectPropListContext::NewDescriptionHelper );
}

//
//	VTSObjectPropListContext::NewDescriptionHelper
//

bool VTSObjectPropListContext::NewDescriptionHelper( int indx, VTSObjPropValue &val )
{
	POSITION			pos
	;
	VTSObjectPropDesc	desc
	;

	// match the current object and a new property
	if ((val.valueObjID == gVTSObjectPropListContext.contextObjID) 
			&& (val.valueProperty == gVTSObjectPropListContext.contextPropID)) {

		// look for the descriptor
		pos = gVTSObjectPropListContext.contextObjListPtr->propList.GetHeadPosition();
		while (pos != NULL) {
			VTSObjectPropDesc &desc = gVTSObjectPropListContext.contextObjListPtr->propList.GetNext(pos);
			if (desc.propID == gVTSObjectPropListContext.contextPropID) {
				desc.propValue = gVTSObjectPropListContext.contextObjListPtr->propOPVL->DescribeValue( indx );
				desc.propIsArray = (val.valueIndx == 0);
				break;
			}
		}

		// all done
		return false;
	}

	// keep searching
	return true;
}

//
//	VTSObjectPropList::Length
//

int VTSObjectPropList::Length( void )
{
	return propList.GetCount();
}

//
//	VTSObjectPropList::operator[]
//

const VTSObjectPropDesc &VTSObjectPropList::operator[]( int indx )
{
	POSITION pos = propList.FindIndex( indx )
	;

	// make sure it's valid
	ASSERT( pos != NULL );

	// return the object ID
	return propList.GetAt(pos);
}

//
//	VTSValueContext
//
//	Same design pattern as VTSObjectListContext.
//

class VTSValueContext {
		friend class VTSValue;

		VTSValuePtr			contextValuePtr;
		objId				contextObjID;
		int					contextPropID;
		int					contextArrayIndex;

		static bool BindHelper( int indx, VTSObjPropValue &val );
		static bool RemoveHelper( int indx, VTSObjPropValue &val );
	};

VTSValueContext gVTSValueContext;

//
//	VTSValue::VTSValue
//

VTSValue::VTSValue( VTSObjPropValueListPtr lp )
	: valueOPVL(lp), valueObjID(-1), valuePropID(-1), valueArrayIndex(-1)
{
}

//
//	VTSValue::~VTSValue
//

VTSValue::~VTSValue( void )
{
}

//
//	VTSValue::Bind
//

void VTSValue::Bind( unsigned int objID, int propID, int arrayIndex )
{
	// set the bindings
	valueObjID = objID;
	valuePropID = propID;
	valueArrayIndex = arrayIndex;

	// clear the component index and length
	valueCompIndex = -1;
	valueCompLen = 0;

	// set the context
	gVTSValueContext.contextValuePtr = this;
	gVTSValueContext.contextObjID = valueObjID;
	gVTSValueContext.contextPropID = valuePropID;
	gVTSValueContext.contextArrayIndex = valueArrayIndex;

	// scan the list looking for components
	valueOPVL->ScanList( VTSValueContext::BindHelper );
}

//
//	VTSValueContext::BindHelper
//
//	If a new property ID is found, add it to the end of the list, set the context so the new one 
//	isn't added more than once, and continue searching.
//

bool VTSValueContext::BindHelper( int indx, VTSObjPropValue &val )
{
	// match the current object, property, and index.  If we've already found the 
	// content then don't bother searching more.
	if ((val.valueObjID != gVTSValueContext.contextObjID) 
			|| (val.valueProperty != gVTSValueContext.contextPropID)
			|| (val.valueIndx != gVTSValueContext.contextArrayIndex))
		return (gVTSValueContext.contextValuePtr->valueCompIndex == -1);
	
	if (gVTSValueContext.contextValuePtr->valueCompIndex == -1) {
		// the first component must be zero
		ASSERT( val.valueComponent == 0 );

		// set the index and count
		gVTSValueContext.contextValuePtr->valueCompIndex = indx;
		gVTSValueContext.contextValuePtr->valueCompLen = 1;
	} else {
		// update the length
		gVTSValueContext.contextValuePtr->valueCompLen =
			indx - gVTSValueContext.contextValuePtr->valueCompIndex + 1;

		// make sure the components are in order
		ASSERT( val.valueComponent == gVTSValueContext.contextValuePtr->valueCompLen - 1 );
	}

	// keep searching
	return true;
}

//
//	VTSValue::Add
//
//	Adding a component is easy because the components default to application encoded nulls.
//

void VTSValue::Add( void )
{
	VTSObjPropValue		val
	;

	// initialize a value
	val.valueObjID = valueObjID;
	val.valueProperty = valuePropID;
	val.valueIndx = valueArrayIndex;
	val.valueComponent = valueCompLen;		// component added on the end
	val.valueType = 0;						// null data type
	val.valueContext = -1;					// application encoded
	val.valueElemID = 0;					// no additional element

	// stuff it into the list
	valueOPVL->InsertElem( valueCompIndex + valueCompLen, val );

	// add one to the length
	valueCompLen += 1;
}

//
//	VTSValue::Remove
//
//	Removing a component is easy because it is easy to calculate the offset into the list.
//	If there is an associated object make sure it gets deleted, delete the component, 
//	then use a helper to renumber the rest of the components.
//

void VTSValue::Remove( int indx )
{
	VTSObjPropValue		val
	;

	// make sure there is more than one component
	ASSERT( valueCompLen > 1 );

	// make sure it's not out of bounds
	ASSERT( (indx >= 0) && (indx < valueCompLen) );

	// compute the component index
	int		compIndex = valueCompIndex + indx
	;

	// read the current contents
	valueOPVL->ReadElem( compIndex, val );

	// if there's an associated object, delete it
	if (val.valueElemID)
		valueOPVL->objPropDoc->m_pDB->pObjMgr->DeleteObject( val.valueElemID );

	// delete the component
	valueOPVL->RemoveElem( compIndex );

	// set the context
	gVTSValueContext.contextValuePtr = this;
	gVTSValueContext.contextObjID = valueObjID;
	gVTSValueContext.contextPropID = valuePropID;
	gVTSValueContext.contextArrayIndex = valueArrayIndex;

	// scan the list looking for components, starting at the new component
	valueOPVL->ScanList( VTSValueContext::RemoveHelper, compIndex );

	// not so many
	valueCompLen -= 1;
}

//
//	VTSValueContext::RemoveHelper
//
//	If a new property ID is found, add it to the end of the list, set the context so the new one 
//	isn't added more than once, and continue searching.
//

bool VTSValueContext::RemoveHelper( int indx, VTSObjPropValue &val )
{
	// match the current object, property, and index, if there's no match there's no more looking
	if ((val.valueObjID != gVTSValueContext.contextObjID) 
			|| (val.valueProperty != gVTSValueContext.contextPropID)
			|| (val.valueIndx != gVTSValueContext.contextArrayIndex))
		return false;
	
	// make this a previous component
	val.valueComponent -= 1;

	// save this record
	gVTSValueContext.contextValuePtr->valueOPVL->WriteElem( indx, val );

	// keep searching
	return true;
}

//
//	VTSValue::Length
//

int VTSValue::Length( void )
{
	return valueCompLen;
}

//
//	VTSValue::ReadComponent
//

void VTSValue::ReadComponent( int indx, VTSObjPropValueElemDesc& desc )
{
	// make sure it's not out of bounds
	ASSERT( (indx >= 0) && (indx < valueCompLen) );

	// compute the component index
	int		compIndex = valueCompIndex + indx
	;

	// read the current contents
	valueOPVL->ReadComponent( compIndex, desc );
}

//
//	VTSValue::WriteComponent
//

void VTSValue::WriteComponent( int indx, VTSObjPropValueElemDesc& desc )
{
	// make sure it's not out of bounds
	ASSERT( (indx >= 0) && (indx < valueCompLen) );

	// compute the component index
	int		compIndex = valueCompIndex + indx
	;

	// write the current contents
	valueOPVL->WriteComponent( compIndex, desc );
}

//
//	VTSValueArrayContext
//
//	Same design pattern as VTSObjectListContext.
//

class VTSValueArrayContext {
		friend class VTSValueArray;

		VTSValueArrayPtr	contextArrayPtr;
		objId				contextObjID;
		int					contextPropID;
		int					contextArrayIndex;

		static bool BindHelper( int indx, VTSObjPropValue &val );
		static bool AddHelper( int indx, VTSObjPropValue &val );
		static bool RemoveHelper( int indx, VTSObjPropValue &val );
		static bool NewDescriptionHelper( int indx, VTSObjPropValue &val );
	};

VTSValueArrayContext gVTSValueArrayContext;

//
//	VTSValueArray::VTSValueArray
//

VTSValueArray::VTSValueArray( VTSObjPropValueListPtr lp )
	: arrayOPVL(lp), arrayObjID(0), arrayPropID(-1), arrayIndex(-1)
{
}

//
//	VTSValueArray::~VTSValueArray
//

VTSValueArray::~VTSValueArray( void )
{
}

//
//	VTSValueArray::Bind
//

void VTSValueArray::Bind( unsigned int objID, int propID )
{
	// set the bindings
	arrayObjID = objID;
	arrayPropID = propID;

	// clear the component index and length
	arrayIndex = -1;

	// clear any current element descriptions
	arrayText.RemoveAll();

	// set the context
	gVTSValueArrayContext.contextArrayPtr = this;
	gVTSValueArrayContext.contextObjID = arrayObjID;
	gVTSValueArrayContext.contextPropID = arrayPropID;
	gVTSValueArrayContext.contextArrayIndex = -1;

	// scan the list looking for components
	arrayOPVL->ScanList( VTSValueArrayContext::BindHelper );
}

//
//	VTSValueArrayContext::BindHelper
//

bool VTSValueArrayContext::BindHelper( int indx, VTSObjPropValue &val )
{
	VTSValueArrayPtr	vap = gVTSValueArrayContext.contextArrayPtr
	;

	// check for a match
	if ((val.valueObjID != gVTSValueArrayContext.contextObjID) 
			|| (val.valueProperty != gVTSValueArrayContext.contextPropID))
		return (vap->arrayIndex == -1);

	if (vap->arrayIndex == -1)
		// set the index so we can scan faster the next time
		vap->arrayIndex = indx;

	// skip the marker
	if (val.valueIndx == 0)
		return true;

	// if we haven't seen this array element yet, get a description of it
	if (val.valueIndx != gVTSValueArrayContext.contextArrayIndex) {
		// ask the list for a description
		vap->arrayText.AddTail( vap->arrayOPVL->DescribeValue( indx ) );

		// skip along until the next element
		gVTSValueArrayContext.contextArrayIndex = val.valueIndx;
	}

	// keep searching
	return true;
}

//
//	VTSValueArray::Add
//

void VTSValueArray::Add( void )
{
	int					indx
	;
	VTSObjPropValue		val
	;

	// set the context
	gVTSValueArrayContext.contextArrayPtr = this;
	gVTSValueArrayContext.contextObjID = arrayObjID;
	gVTSValueArrayContext.contextPropID = arrayPropID;

	// find the index of the last element
	indx = arrayOPVL->ScanList( VTSValueArrayContext::AddHelper, arrayIndex );

	// if the indx is -1 there are no more elements in the array
	if (indx < 0)
		indx = arrayOPVL->Length();

	// there must be at least something
	ASSERT( indx > 0 );

	// read the last element
	arrayOPVL->ReadElem( indx - 1, val );

	// initialize a value
	val.valueIndx += 1;						// index becomes the next value
	val.valueComponent = 0;					// first and only component
	val.valueType = 0;						// null data type
	val.valueContext = -1;					// application encoded
	val.valueElemID = 0;					// no additional element

	// stuff it into the list
	arrayOPVL->InsertElem( indx, val );

	// add its description
	arrayText.AddTail( arrayOPVL->DescribeValue(indx) );
}

//
//	VTSValueArrayContext::AddHelper
//
//	Unlike the other BindHelpers, this is only interested in finding the first record 
//	that matches the object and property.
//

bool VTSValueArrayContext::AddHelper( int indx, VTSObjPropValue &val )
{
	// check for a match
	return ((val.valueObjID == gVTSValueArrayContext.contextObjID) 
			&& (val.valueProperty == gVTSValueArrayContext.contextPropID));
}

//
//	VTSValueArray::Remove
//

void VTSValueArray::Remove( int indx )
{
	// set the context
	gVTSValueArrayContext.contextArrayPtr = this;
	gVTSValueArrayContext.contextObjID = arrayObjID;
	gVTSValueArrayContext.contextPropID = arrayPropID;
	gVTSValueArrayContext.contextArrayIndex = indx;

	// scan the list looking for components
	arrayOPVL->ScanList( VTSValueArrayContext::RemoveHelper, arrayIndex );
}

//
//	VTSValueArrayContext::RemoveHelper
//

bool VTSValueArrayContext::RemoveHelper( int indx, VTSObjPropValue &val )
{
	// check for a match
	if ((val.valueObjID == gVTSValueArrayContext.contextObjID) 
		&& (val.valueProperty == gVTSValueArrayContext.contextPropID)) {

		if (val.valueIndx == gVTSValueArrayContext.contextArrayIndex) {
			// get rid of this one
			gVTSValueArrayContext.contextArrayPtr->arrayOPVL->RemoveElem( indx );
		} else
		if (val.valueIndx > gVTSValueArrayContext.contextArrayIndex) {
			// move this down one
			val.valueIndx -= 1;

			// save it
			gVTSValueArrayContext.contextArrayPtr->arrayOPVL->WriteElem( indx, val );
		} else
			;
	}

	// keep searching
	return true;
}

//
//	VTSValueArray::NewDescription
//

void VTSValueArray::NewDescription( int indx )
{
	// set the context
	gVTSValueArrayContext.contextArrayPtr = this;
	gVTSValueArrayContext.contextObjID = arrayObjID;
	gVTSValueArrayContext.contextPropID = arrayPropID;
	gVTSValueArrayContext.contextArrayIndex = indx;

	// scan the list looking for components
	arrayOPVL->ScanList( VTSValueArrayContext::NewDescriptionHelper, arrayIndex );
}

//
//	VTSValueArrayContext::NewDescriptionHelper
//

bool VTSValueArrayContext::NewDescriptionHelper( int indx, VTSObjPropValue &val )
{
	// match the current object, property, and array index
	if ((val.valueObjID == gVTSValueArrayContext.contextObjID) 
			&& (val.valueProperty == gVTSValueArrayContext.contextPropID)
			&& (val.valueIndx == gVTSValueArrayContext.contextArrayIndex)) {

		// make SetAt() a little easier to read
		VTSValueArrayPtr vap = gVTSValueArrayContext.contextArrayPtr
		;

		// build a new description
		vap->arrayText.SetAt(	vap->arrayText.FindIndex(gVTSValueArrayContext.contextArrayIndex - 1)
			,					vap->arrayOPVL->DescribeValue( indx )
			);

		// all done
		return false;
	}

	// keep searching
	return true;
}

//
//	VTSValueArray::Length
//

int VTSValueArray::Length( void )
{
	return arrayText.GetCount();
}

//
//	VTSValueArray::GetText
//

CString& VTSValueArray::GetText( int indx )
{
	return arrayText.GetAt( arrayText.FindIndex(indx) );
}

//
//	VTSValueArray::SetText
//

void VTSValueArray::SetText( int indx, CString& valu )
{
	arrayText.SetAt( arrayText.FindIndex(indx), valu );
}
