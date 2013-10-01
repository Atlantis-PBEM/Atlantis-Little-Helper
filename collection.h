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

#ifndef _INCL_COLL_H_
#define _INCL_COLL_H_

#include "bool.h"
#include "compat.h"


class CCollection
{
public:
    CCollection();
    CCollection(int nDelta);
    virtual ~CCollection();

    void * At(int nIndex);
    virtual BOOL Insert(void * pItem);
    //virtual void Delete(void * pItem);  Delete and Free are removed because they are slow
    //virtual void Free(void * pItem);
    void AtInsert(int nIndex, void * pItem);
    void AtSet(int nIndex, void * pItem, BOOL bFreeOld);
    void AtDelete(int nIndex);
    void AtFree(int nIndex);
    void DeleteAll();
    void FreeAll();
    inline int  Count() {return m_nCount;};
    inline void ** GetItems() {return m_pItems;};
    

protected:
    virtual void FreeItem(void * pItem)=0;
    void Reallocate();

    void ** m_pItems;
    int     m_nCount;
    int     m_nSize;
    int     m_nDelta;
};

//====================================================

// it is just collection of longs. Cast void* to long and vice versa
class CLongColl : public CCollection
{
public:
    CLongColl() : CCollection() {};
    CLongColl(int nDelta) : CCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem) {};
};

//====================================================

// it is just collection of buffers allocated using malloc() .
class CBufColl : public CCollection
{
public:
    CBufColl() : CCollection() {};
    CBufColl(int nDelta) : CCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem) { if (pItem) free(pItem); };
};

//====================================================

class CSortedCollection : public CCollection
{
public:
    CSortedCollection();
    CSortedCollection(int nDelta);
    virtual BOOL Insert(void * pItem);
    //virtual void Delete(void * pItem);  Delete and Free are removed because they are slow
    //virtual void Free(void * pItem);



    BOOL Search(void * pItem, int & nIndex);
    //BOOL FindFirst(void * pItem, int & nIndex); Removed since Search always finds first matching item (if m_bDuplicates)
    
    BOOL m_bDuplicates; 

protected:
    virtual int Compare(void * pItem1, void * pItem2) = 0;

};

//====================================================

// it is just collection of sorted longs. Cast void* to long and vice versa
class CLongSortColl : public CSortedCollection
{
public:
    CLongSortColl() : CSortedCollection() {};
    CLongSortColl(int nDelta) : CSortedCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem) {};
    virtual int Compare(void * pItem1, void * pItem2) 
    {
        if ((long)pItem1 > (long)pItem2)
            return 1;
        else
            if ((long)pItem1 < (long)pItem2)
                return -1;
            else
                return 0;
    };
};

//====================================================

class CStringSortColl : public CSortedCollection
{
public:
    CStringSortColl()           : CSortedCollection() {};
    CStringSortColl(int nDelta) : CSortedCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem) { if (pItem) free(pItem); };
    virtual int Compare(void * pItem1, void * pItem2) {return stricmp((const char*)pItem1, (const char*)pItem2);};
};


//====================================================

// It is possible to change order of items in this collection.
// In order to do this implementation of Compare() must check m_SortMode.
// And duplicates is another problem, SetSortMode() will do nothing, think for yourself

class CResortableCollection : public CSortedCollection
{
public:
    CResortableCollection()           : CSortedCollection()       {m_SortMode=0;};
    CResortableCollection(int nDelta) : CSortedCollection(nDelta) {m_SortMode=0;};

    void SetSortMode(int SortMode);

protected:
    void DoSort(int l, int r);

    int m_SortMode;
};

//====================================================


#endif
















