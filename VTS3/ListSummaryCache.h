// ListSummaryCache.h: interface for the CListSummaryCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTSUMMARYCACHE_H__7E68EECE_35C2_4D2D_91C8_9EFCD4ABCDC8__INCLUDED_)
#define AFX_LISTSUMMARYCACHE_H__7E68EECE_35C2_4D2D_91C8_9EFCD4ABCDC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//=====================================================================
//  Cache for list summary view
//  View is owner draw so let's make this as efficient as possible

typedef struct
{
	// define simple array of column data pointers, size from VTSPreferences
	char	* pszColumnData[SUMMARY_VIEW_COLUMN_COUNT];
} LVCachedItem;


typedef struct
{
	char szColumn0[10];			// data for number
	char szColumn1[20];			// data for timestamp
	char szColumn2[30];			// data for port name
	//Xiao Shiyuan 2004-sep-20
	char szColumn3[100];		// data for source (may include device name AND router info)
	char szColumn4[100];		// data for destination (may include device name AND router info)
	char szColumn5[20];			// data for snet 
	char szColumn6[100];		// data for saddr
	char szColumn7[20];			// data for dnet 
	char szColumn8[100];		// data for daddr 	
	char szColumn9[200];		// data for BACnet message summary
	//Xiao Shiyuan 2004-sep-20
} LVCacheData;


typedef struct
{
	DWORD	dwIndex;				// holds item number this data caches
	DWORD	dwMRUCount;				// holds count when last accessed
	LVCacheData	data;
} LVCacheSlot;



class CListSummaryCache : public CObject  
{
	private:
		int	m_dwMRU;					// constantly incrementing value with slot access
		int m_nCacheSize;			// current size of cache;
		int m_nLastIndex;

		LVCacheSlot	 *	m_aslots;	// pointer to slots, treated as array

		void KillCache(void);
		int FindAvailableSlot(void);

	public:

		CListSummaryCache( int nCacheSize = 100 );
		virtual ~CListSummaryCache();

		void InitCache(void);
		int GetCacheSize(void) { return m_nCacheSize; }
		void AllocCacheSlots(int nCacheSize);
		bool GetCachedItem( DWORD dwIndex, LVCachedItem * pcacheditem );
		int FindCachedIndex( DWORD dwIndex );
		void DeleteCachedItem( DWORD dwIndex );
};

#endif // !defined(AFX_LISTSUMMARYCACHE_H__7E68EECE_35C2_4D2D_91C8_9EFCD4ABCDC8__INCLUDED_)
