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

#ifndef __AH_HASH_H_INCL__
#define __AH_HASH_H_INCL__

typedef struct _HASH_LIST_ITEM
{
    void * key;
    void * data;
    struct _HASH_LIST_ITEM * next;
} HASH_LIST_ITEM;


//---------------------------------------------------------------

// duplicates are allowed and expected

class CHash
{
public:
    CHash(unsigned int sizeidx);
    virtual ~CHash();

    void             FreeAll();
    void             Insert    (const void * key, const void * data);
    void             Free      (const void * key);
    HASH_LIST_ITEM * LocateItem(const void * key);
    bool             Locate    (const void * key, const void *& data);
    void             ForEach   (void (*action)(void*, void*, void*), void * user_data);
    void             ForEachCheckRemove(int (*action)(void*, void*, void*), void * user_data);

protected:
    virtual void             FreeItem(HASH_LIST_ITEM * pItem) = 0;
    virtual HASH_LIST_ITEM * NewItem  (const void * key,  const void * data)  = 0;
    virtual unsigned long    HashValue(const void * key)                      = 0;
    virtual bool             Equal    (const void * key1, const void * key2)  = 0;


    int               m_nHashSize;
    HASH_LIST_ITEM ** m_Buckets;
};

//---------------------------------------------------------------

class CHashStrToLong : public CHash
{
public:
    CHashStrToLong(unsigned int sizeidx) : CHash(sizeidx) {};

    void   Dbg_Print();

protected:
    virtual void             FreeItem(HASH_LIST_ITEM * pItem);
    virtual HASH_LIST_ITEM * NewItem  (const void * key,  const void * data);
    virtual unsigned long    HashValue(const void * key);
    virtual bool             Equal    (const void * key1, const void * key2);
};

//---------------------------------------------------------------

// stores duplicates of strings, destroys them

class CHashLongToStr : public CHash
{
    public:
        CHashLongToStr(unsigned int sizeidx) : CHash(sizeidx) {};
    protected:
        virtual void             FreeItem(HASH_LIST_ITEM * pItem);
        virtual HASH_LIST_ITEM * NewItem  (const void * key,  const void * data);
        virtual unsigned long    HashValue(const void * key);
        virtual bool             Equal    (const void * key1, const void * key2);
};

//---------------------------------------------------------------

// Stores generic pointers, user is responsible for deleting the memory pointed to

class CHashLongToPointer : public CHash
{
    public:
        CHashLongToPointer(unsigned int sizeidx) : CHash(sizeidx) {};
    protected:
        virtual void             FreeItem(HASH_LIST_ITEM * pItem);
        virtual HASH_LIST_ITEM * NewItem  (const void * key,  const void * data);
        virtual unsigned long    HashValue(const void * key);
        virtual bool             Equal    (const void * key1, const void * key2);
};

//---------------------------------------------------------------

#endif

