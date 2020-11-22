//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSPROPVALUE_H__95864240_C787_11D6_A613_00A0C9E370F1__INCLUDED_)
#define AFX_VTSPROPVALUE_H__95864240_C787_11D6_A613_00A0C9E370F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxmt.h>

#if Mac_Platform
#pragma options align=mac68k
#endif
#if NT_Platform
#pragma pack(push,2)
#endif


// ===========================================================
// Create macros useful for typed pointer arrays
// Usage:
//			DECLARE_VTSPTRARRY(VTSDevObjects, VTSDevObject)
// This defines the VTSDevObjects class, a CPtrArray of VTSDevObject pointers

#define DECLARE_VTSPTRARRAY_GUTS(class_name2, element_name2) \
		public: \
			class_name2();	virtual ~class_name2( void ); \
			void DeepCopy( const class_name2 * psrc ); \
			void KillContents( void ); \
			element_name2 * Find( LPCSTR lpszName ); \
			void Serialize( CArchive& archive, UINT version ); \
			DECLARE_SERIAL(class_name2);

#define DECLARE_VTSPTRARRAY(class_name, element_name) \
	class class_name : public CTypedPtrArray<CPtrArray, element_name *> { \
			DECLARE_VTSPTRARRAY_GUTS(class_name, element_name); };


// ===========================================================
// Implement macro for typed pointer arrays
// Usage:
//			IMPLEMENT_VTSPTRARRY(VTSDevObjects, VTSDevObject)
// This creates all of the serialization, kill and copy methods

#define IMPLEMENT_VTSPTRARRAY(class_name, element_name) \
	class_name::class_name(void) {} \
	class_name::~class_name(void) { KillContents(); } \
	void class_name::KillContents(void) { \
		for ( int i = 0; i < GetSize(); i++ ) if ( GetAt(i) != NULL ) delete GetAt(i); \
		RemoveAll(); } \
	void class_name::DeepCopy( const class_name * psrc ) { \
		KillContents(); \
		for ( int i = 0; i < psrc->GetSize(); i++ )	{ \
			element_name * pelement = new element_name(); \
			*pelement = *(*psrc)[i]; \
			Add(pelement); } } \
	element_name * class_name::Find( LPCSTR lpszName ) { \
		for ( int i = 0; i < GetSize(); i++ ) \
			if ( CString(GetAt(i)->GetName()).CompareNoCase(lpszName) == 0 ) \
				return GetAt(i); \
		return NULL; } \
	void class_name::Serialize(CArchive& ar, UINT version) { \
		if (ar.IsStoring())	{ ar << GetSize(); for ( int i = 0; i < GetSize(); i++ ) GetAt(i)->Serialize(ar,version); } \
		else {	int iSize; KillContents(); \
			for ( ar >> iSize; iSize > 0; iSize-- ) { \
			element_name * p = new element_name(); p->Serialize(ar,version); Add(p); } } } \
	IMPLEMENT_SERIAL(class_name, CPtrArray, 1);



const int kVTSValueElemContentLength = 512;

class VTSDevValues;
class BACnetEncodeable;

class VTSDevValue : public CObject
{
	protected:
		void KillValueList( void );

	public:
		// Schema version 1
		// begin persistent members
		int				m_nType;				// application data type
		int				m_nContext;				// context (-1 app encoded, >= 0 context encoded)
		int				m_nLength;									// length of encoded content
		unsigned char	m_abContent[kVTSValueElemContentLength];	// encoded value
		VTSDevValues *  m_pdevvalues;			// pointer to sister components for this value...
												// has potential for infinity... see to it we don't. :)
		// end persistent storage

	public:
		VTSDevValue(void);
		virtual ~VTSDevValue(void);

		CString GetDescription(void);
		VTSDevValues * AllocateNewValueList( void );
		BACnetEncodeable * CreateBACnetObject( void );

		LPCSTR GetName(void) { return NULL; }

		const VTSDevValue& operator=(const VTSDevValue& rdevvalueSrc);
		void Serialize( CArchive& archive, UINT version );
		VTSDevValues * GetValueList(void) { return m_pdevvalues; }

		DECLARE_SERIAL(VTSDevValue);
};

DECLARE_VTSPTRARRAY(VTSDevValues, VTSDevValue);
                     

class VTSDevProperty : public CObject
{
    private:
		// Schema version 1
		// Begin persistent storage
		VTSDevValues	m_devvalues;
		unsigned int	m_nPropID;			// property identifier
		bool			m_fIsArray;			// true if property is an array
		// End persistent storage

	public:
		VTSDevProperty(void);
		virtual ~VTSDevProperty(void);

		int GetID() { return m_nPropID; }
		void SetID( int nID ) { m_nPropID = nID; }
		bool IsArray() { return m_fIsArray; }
		void SetIsArray( bool fIsArray ) { m_fIsArray = fIsArray; }
		VTSDevValues * GetValues() { return &m_devvalues; }
		CString GetDescription( void );

		LPCSTR GetName(void) { return NULL; }
		static LPCSTR GetDescription( unsigned int nPropID );

		const VTSDevProperty& operator=(const VTSDevProperty& rdevpropertySrc);
		void Serialize( CArchive& archive, UINT version );

		DECLARE_SERIAL(VTSDevProperty);
};

DECLARE_VTSPTRARRAY(VTSDevProperties, VTSDevProperty);


class VTSDevObject : public CObject
{
	private:
		// Schema version 1
		// Begin persistent storage
		VTSDevProperties	m_devproperties;
		unsigned int		m_nbacnetObjID;
		// End persistent storage

	public:
		VTSDevObject(void);
		virtual ~VTSDevObject(void);

		unsigned int GetID() { return m_nbacnetObjID; }
		void SetID( unsigned int nID ) { m_nbacnetObjID = nID; }
		void SetID( int nObjType, int nInstance ) { SetID((nObjType << 22) + nInstance); }
		VTSDevProperties * GetProperties() { return &m_devproperties; }
		CString GetDescription(void);

		LPCSTR GetName(void) { return NULL; }

		int GetType(void) { return (int) (GetID() >> 22); }
		int GetInstance(void) { return (int) (GetID() & ((1 << 22) - 1)); }
		const VTSDevObject& operator=(const VTSDevObject& rdevobjectSrc);
		void Serialize( CArchive& archive, UINT version );

		DECLARE_SERIAL(VTSDevObject);
};

DECLARE_VTSPTRARRAY(VTSDevObjects, VTSDevObject);



#endif // !defined(AFX_VTSPROPVALUE_H__95864240_C787_11D6_A613_00A0C9E370F1__INCLUDED_)
