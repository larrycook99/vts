// ListSummaryCache.cpp: implementation of the CListSummaryCache class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSPreferences.h"				// for column count
#include "ListSummaryCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListSummaryCache::CListSummaryCache( int nCacheSize /* = 5000 */ )
{
	m_dwMRU = 0;
	m_nCacheSize = 0;
	m_aslots = NULL;

	AllocCacheSlots(nCacheSize);
}


CListSummaryCache::~CListSummaryCache()
{
	KillCache();
}


void CListSummaryCache::KillCache()
{
	if ( m_aslots != NULL )
		delete m_aslots;

	m_nCacheSize = 0;
	m_aslots = NULL;
}


void CListSummaryCache::AllocCacheSlots(int nCacheSize)
{
	KillCache();

	// Hey, let's be reasonable and not allocate a cache larger than approx 64K.  What the heck.
//	nCacheSize = nCacheSize > (65000 / sizeof(LVCacheSlot)) ? (65000 / sizeof(LVCacheSlot)) : nCacheSize;

	// OK... 64K was just silly... try 32,000 slots as a max.  That's about 6M... plenty
	// bytes.  We'll start to get into performance problems with the cache having to search so many entries
	// just to keep up the slots.  I think.
	
	nCacheSize = nCacheSize > SUMMARY_VIEW_MAXCACHESLOTS ? SUMMARY_VIEW_MAXCACHESLOTS : nCacheSize;

	if ( nCacheSize )
		m_aslots = new LVCacheSlot[nCacheSize];

	TRACE2("Allocating %d bytes for %d cache slots...\n", nCacheSize * sizeof(LVCacheSlot), nCacheSize);

	if ( m_aslots != NULL )
		m_nCacheSize = nCacheSize;

	InitCache();
}



void CListSummaryCache::InitCache()
{
	if ( m_aslots != NULL )
	{
		memset(m_aslots, 0, sizeof(LVCacheSlot) * m_nCacheSize);
		for ( int i = 0; i < m_nCacheSize; i++ )
			m_aslots[i].dwIndex = 0xffff;
	}

	m_dwMRU = 0;
	m_nLastIndex = 0;
}


// The guy returns the cached item if found...  if not it returns the structs
// to load the cached item.
// false is returned if an empty slot is given back
// true is returned with the cached data

// This allows you to call this item to see if the data is cached (and get it).
// If it wasn't, you can load up the values with the new cache

bool CListSummaryCache::GetCachedItem( DWORD dwIndex, LVCachedItem * pcacheditem )
{
	bool fAlreadyCached = true;

	int i = FindCachedIndex(dwIndex);
	if ( i == -1 )
	{
//		TRACE1("Cache item %d NOT found... ", dwIndex);
		fAlreadyCached = false;
		i = FindAvailableSlot();

//		TRACE2("Loading into to slot %d, replacing %d\n", i, m_aslots[i].dwIndex);

		m_aslots[i].dwIndex = dwIndex;
	}
//	else
//	{
//		TRACE2("Cache item %d found in slot... \n", dwIndex, i);
//	}

	m_aslots[i].dwMRUCount = ++m_dwMRU;
	pcacheditem->pszColumnData[0] = m_aslots[i].data.szColumn0;
	pcacheditem->pszColumnData[1] = m_aslots[i].data.szColumn1;
	pcacheditem->pszColumnData[2] = m_aslots[i].data.szColumn2;
	pcacheditem->pszColumnData[3] = m_aslots[i].data.szColumn3;
	pcacheditem->pszColumnData[4] = m_aslots[i].data.szColumn4;

	//Xiao Shiyuan 2004-sep-20. For more columns in summary view 
	pcacheditem->pszColumnData[5] = m_aslots[i].data.szColumn5;
	pcacheditem->pszColumnData[6] = m_aslots[i].data.szColumn6;
	pcacheditem->pszColumnData[7] = m_aslots[i].data.szColumn7;
	pcacheditem->pszColumnData[8] = m_aslots[i].data.szColumn8;
	pcacheditem->pszColumnData[9] = m_aslots[i].data.szColumn9;

	return fAlreadyCached;
}


int CListSummaryCache::FindAvailableSlot()
{
	// Search through the slots looking for the least used slot
	// don't bother with empty markers... that only happens very early

	int iLRU = 0;

	for ( int i = 0; i < m_nCacheSize; i++ )
		if ( m_aslots[i].dwMRUCount < m_aslots[iLRU].dwMRUCount )
			iLRU = i;

	return iLRU;
}

int CListSummaryCache::FindCachedIndex( DWORD dwIndex )
{
	// Tests show that "same as previous" happen more than ten times
	// as often as other conditions, so this is a significant speedup.
	if ( dwIndex == m_aslots[m_nLastIndex].dwIndex )
	{
		return m_nLastIndex;
	}

	for ( int nIndex = 0; nIndex < m_nCacheSize; nIndex++ )
	{
		if ( dwIndex == m_aslots[nIndex].dwIndex )
		{
			m_nLastIndex = nIndex;
			return m_nLastIndex;
		}
	}

	return -1;
}

void CListSummaryCache::DeleteCachedItem( DWORD dwIndex )
{
	for(int i = 0; i < m_nCacheSize; i++)
	{
		if( dwIndex == m_aslots[i].dwIndex )
			m_aslots[i].dwIndex = 0xFFFF;
		else
		if( m_aslots[i].dwIndex > dwIndex && m_aslots[i].dwIndex != 0xFFFF)
			m_aslots[i].dwIndex--;
	}
}
