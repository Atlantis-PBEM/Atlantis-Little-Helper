/*
 * This source file is part of the Atlantis Little Helper program.
 * Copyright (C) 2001 Maxim Shariy.
 *
 * Atlantis Little Helper is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Atlantis Little Helper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Atlantis Little Helper; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <stdlib.h>
#include <string.h>

#include "collection.h"


#if defined(_MSC_VER)
    #ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
    #endif
#endif


#define KEEP_MEM



//--------------------------------------------------------------------

CCollection::CCollection()
{
    m_nCount = 0;
    m_nSize  = 0;
    m_nDelta = 8;
    m_pItems = NULL;
}

//--------------------------------------------------------------------

CCollection::CCollection(int nDelta)
{
    m_nCount = 0;
    m_nSize  = 0;
    m_nDelta = nDelta;
    m_pItems = NULL;

    if (m_nDelta <= 4)
        m_nDelta  = 4;
    else
        if (m_nDelta > 0x0400)
            m_nDelta = 0x0400;
}

//--------------------------------------------------------------------

CCollection::~CCollection()
{
    // Do not call FreeAll() - it is no use! CCollection::FreeItem() will be called, anyway!
    // Application must call it!
    // FreeAll();

    if (m_pItems)
        free(m_pItems);
}

//--------------------------------------------------------------------

void * CCollection::At(int nIndex)
{
    if ( (nIndex>=0) && (nIndex<m_nCount) )
        return m_pItems[nIndex];
    else
        return NULL;
}

//--------------------------------------------------------------------

BOOL CCollection::Insert(void * pItem)
{
    AtInsert(m_nCount, pItem);
    return TRUE;
}

//--------------------------------------------------------------------

void CCollection::AtDelete(int nIndex)
{
    if ( (nIndex>=0) && (nIndex<m_nCount) )
    {
        if (nIndex<m_nCount-1)
            memcpy(&m_pItems[nIndex], &m_pItems[nIndex+1], sizeof(void*)*(m_nCount-1-nIndex));
        m_nCount--;
#ifndef KEEP_MEM
        Reallocate();
#endif
    }
}

//--------------------------------------------------------------------

void CCollection::Reallocate()
{
    int    nNewSize;
    void * p;

    if ( ( m_nSize <= m_nCount ) ||
         ( m_nSize >  m_nCount + 2*m_nDelta ) )
    {
        nNewSize = (m_nCount/m_nDelta + 1) * m_nDelta;
        p = (void**)malloc(nNewSize * sizeof(void*));

        if (m_nCount>0)
            memmove(p, m_pItems, sizeof(void*)*m_nCount);

        if (m_pItems)
            free(m_pItems);

        m_pItems = (void**)p;
        m_nSize  = nNewSize;
    }
}

//--------------------------------------------------------------------

void CCollection::AtFree(int nIndex)
{
    void * pItem;

    pItem = At(nIndex);
    if (pItem)
        FreeItem(pItem);
    AtDelete(nIndex);
}

//--------------------------------------------------------------------

void CCollection::FreeAll()
{
    void * pItem;
    int    i;

    for (i=0; i<m_nCount; i++)
    {
        pItem = At(i);
        if (pItem)
            FreeItem(pItem);
    }
    DeleteAll();
}

//--------------------------------------------------------------------

void CCollection::DeleteAll()
{
    m_nCount=0;
#ifndef KEEP_MEM
    if (m_pItems)
    {
        free(m_pItems);
        m_pItems = NULL;
    }
#endif
}

//--------------------------------------------------------------------

void CCollection::AtInsert(int nIndex, void * pItem)
{
    if (nIndex<0 || nIndex>m_nCount)
    {
        //throw "Unacceptable index!";  this does not compile in KDevelop
        int xx = 2;
        xx = 2/(xx-2);
    }
    Reallocate();

    if (nIndex<m_nCount)
        memmove(&m_pItems[nIndex+1], &m_pItems[nIndex], sizeof(void*)*(m_nCount-nIndex));
    m_pItems[nIndex] = pItem;
    m_nCount++;
}

//--------------------------------------------------------------------

void CCollection::AtSet(int nIndex, void * pItem, BOOL bFreeOld)
{
    if (nIndex<0 || nIndex>m_nCount)
    {
        //throw "Unacceptable index!";  this does not compile in KDevelop
        int xx = 2;
        xx = 2/(xx-2);
    }

    Reallocate();

    if (nIndex==m_nCount)
        m_nCount++;
    else
        if (bFreeOld)
            FreeItem(m_pItems[nIndex]);

    m_pItems[nIndex] = pItem;
}

//============================================================================

CSortedCollection::CSortedCollection() : CCollection()
{
    m_bDuplicates = FALSE;
}

CSortedCollection::CSortedCollection(int nDelta) : CCollection(nDelta)
{
    m_bDuplicates = FALSE;
}

//--------------------------------------------------------------------

BOOL CSortedCollection::Insert(void * pItem)
{
    int I;

    if ( (!Search(pItem, I)) || m_bDuplicates)
    {
        AtInsert(I, pItem);
        return TRUE;
    }
    else
        return FALSE;
}

//--------------------------------------------------------------------

BOOL CSortedCollection::Search(void * pItem, int & nIndex)
{
    int   L, H, I, C;
    BOOL  Ok;

    Ok = FALSE;
    L  = 0;
    H  = Count() - 1;
    while (L <= H)
    {
        I = (L + H) >> 1;
        C = Compare(m_pItems[I], pItem);
        if (C < 0)
            L = I + 1;
        else
        {
            H = I - 1;
            if (C == 0)
            {
                Ok = TRUE;
                if ( !m_bDuplicates)
                    L = I;
            }
        }
    }
    nIndex = L;

    return Ok;
}


//--------------------------------------------------------------------

void CResortableCollection::SetSortMode(int SortMode)
{
    if ( (SortMode != m_SortMode) && (Count()>0) )
    {
        m_SortMode = SortMode;
        DoSort(0, Count()-1);
    }
}

//--------------------------------------------------------------------

void CResortableCollection::DoSort(int l, int r)   // it is quicksort
{
    int     i, j;
    void *  x, * y;

    i = l;
    j = r;
    x = m_pItems[(l+r) >> 1];

    do
    {
        while (Compare(m_pItems[i], x          ) < 0)
            i++;
        while (Compare(x,           m_pItems[j]) < 0)
            j--;
        if (i<=j)
        {
            if (i!=j)
            {
                y           = m_pItems[i];
                m_pItems[i] = m_pItems[j];
                m_pItems[j] = y;
            }
            j--;
            i++;
        }
    }
    while (i<=j);

    if (l<j)
        DoSort(l,j);
    if (i<r)
        DoSort(i,r);
}

//--------------------------------------------------------------------

















