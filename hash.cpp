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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"
#include "compat.h"

static const int g_primes[] =
{
    11,                     //   0
    19,                     //   1
    37,                     //   2
    73,                     //   3
    109,                    //   4
    163,                    //   5
    251,                    //   6
    367,                    //   7
    557,                    //   8
    823,                    //   9
    1237,                   //   10
    1861,                   //   11
    2777,                   //   12
    4177,                   //   13
    6247,                   //   14
    9371,                   //   15
    14057,                  //   16
    21089,                  //   17
    31627,                  //   18
    47431,                  //   19
    71143,                  //   20
    106721,                 //   21
    160073,                 //   22
    240101,                 //   23
    360163,                 //   24
    540217,                 //   25
    810343,                 //   26
    1215497,                //   27
    1823231,                //   28
    2734867,                //   29
    4102283,                //
    6153409,                //
    9230113,                //
    13845163,               //
};

//---------------------------------------------------------------

CHash::CHash(unsigned int sizeidx)
{
    if (sizeidx >= sizeof(g_primes)/sizeof(*g_primes))
        sizeidx =  sizeof(g_primes)/sizeof(*g_primes) - 1;
    m_nHashSize = g_primes[sizeidx];

    m_Buckets = (HASH_LIST_ITEM **) malloc(m_nHashSize * sizeof(HASH_LIST_ITEM *));
    memset(m_Buckets, 0, m_nHashSize * sizeof(HASH_LIST_ITEM *));
}

//---------------------------------------------------------------

CHash::~CHash()
{
    //FreeAll(); can not call it here - does not work for descendants. Suxx!
    free(m_Buckets);
}

//---------------------------------------------------------------

void CHash::FreeAll()
{
    int x;
    HASH_LIST_ITEM * pItem, *p;

    for (x=0; x<m_nHashSize; x++)
    {
        pItem        = m_Buckets[x];
        m_Buckets[x] = NULL;
        while (pItem)
        {
            p = pItem;
            pItem = p->next;
            FreeItem(p);
        }
    }
}

//---------------------------------------------------------------

void CHash::ForEach(void (*action)(void*, void*, void*), void * user_data)
{
    int x;
    HASH_LIST_ITEM * pItem;

    for (x=0; x<m_nHashSize; x++)
    {
        pItem        = m_Buckets[x];
        while (pItem)
        {
            action(pItem->key, pItem->data, user_data);
            pItem = pItem->next;
        }
    }
}

//---------------------------------------------------------------

void CHash::ForEachCheckRemove(int (*action)(void*, void*, void*), void * user_data)
{
    int               x;
    HASH_LIST_ITEM ** ppItem;
    HASH_LIST_ITEM  * p;

    for (x=0; x<m_nHashSize; x++)
    {
        ppItem       = &m_Buckets[x];
        while (*ppItem)
        {
            if (action((*ppItem)->key, (*ppItem)->data, user_data))
            {
                p = *ppItem;
                *ppItem = (*ppItem)->next;
            }
            else
                ppItem = &(*ppItem)->next;
        }
    }
}

//---------------------------------------------------------------

void CHash::Insert(const void * key, const void * data)
{
    unsigned long    x = HashValue(key) % m_nHashSize;
    HASH_LIST_ITEM * p = NewItem(key, data);

    p->next      = m_Buckets[x];
    m_Buckets[x] = p;
}

//---------------------------------------------------------------

HASH_LIST_ITEM * CHash::LocateItem(const void * key)
{
    unsigned long    x = HashValue(key) % m_nHashSize;
    HASH_LIST_ITEM * p = m_Buckets[x];

    while (p)
    {
        if (Equal(key, p->key))
            return p;
        p = p->next;
    }

    return NULL;
}

//---------------------------------------------------------------

bool CHash::Locate(const void * key, const void *& data)
{
    HASH_LIST_ITEM * p = LocateItem(key);

    if (p)
    {

        data = p->data;
        return true;
    }
    else
    {
        data = NULL;
        return false;
    }
}

//---------------------------------------------------------------

void CHash::Free(const void * key)
{
    unsigned long        x = HashValue(key) % m_nHashSize;
    HASH_LIST_ITEM  * p;
    HASH_LIST_ITEM ** root = &m_Buckets[x];

    while (*root)
    {
        if (Equal(key, (*root)->key))
        {
            p = *root;
            *root = p->next;
            FreeItem(p);
            break;
        }
        root = &(*root)->next;
    }
}

//---------------------------------------------------------------


// ==============================================================

HASH_LIST_ITEM * CHashStrToLong::NewItem(const void * key, const void * data)
{
    HASH_LIST_ITEM * pItem;

    pItem = (HASH_LIST_ITEM*)malloc(sizeof(HASH_LIST_ITEM));
    pItem->key  = strdup(key?(char*)key:"");
    pItem->data = (void*)data;
    pItem->next = NULL;

    return pItem;
}

//---------------------------------------------------------------

void CHashStrToLong::FreeItem(HASH_LIST_ITEM * pItem)
{
    if (pItem)
    {
        if (pItem->key)
            free(pItem->key);
        free(pItem);
    }
}

//---------------------------------------------------------------

unsigned long CHashStrToLong::HashValue(const void * key)
{
  const char   *p = (const char *)key;
  unsigned long h = tolower(*p);

  if (h)
    for (p += 1; *p != '\0'; p++)
      h = (h << 5) - h + tolower(*p);

  return h;
}

//---------------------------------------------------------------

bool CHashStrToLong::Equal(const void * key1, const void * key2)
{
    return (0==stricmp((const char*)key1, (const char*)key2));
}

//---------------------------------------------------------------

void CHashStrToLong::Dbg_Print()
{
    int x;
    HASH_LIST_ITEM * pItem;

    for (x=0; x<m_nHashSize; x++)
    {
        pItem        = m_Buckets[x];
        while (pItem)
        {
            printf("%s %ld\t", (char*)pItem->key, (long)pItem->data);
            pItem = pItem->next;
        }
        printf("\n");
    }
}



// ==============================================================

HASH_LIST_ITEM * CHashLongToStr::NewItem(const void * key, const void * data)
{
    HASH_LIST_ITEM * pItem;

    pItem = (HASH_LIST_ITEM*)malloc(sizeof(HASH_LIST_ITEM));
    pItem->key  = (void*)key;
    pItem->data = strdup(data?(char*)data:"");
    pItem->next = NULL;

    return pItem;
}

//---------------------------------------------------------------

void CHashLongToStr::FreeItem(HASH_LIST_ITEM * pItem)
{
    if (pItem)
    {
        if (pItem->data)
            free(pItem->data);
        free(pItem);
    }
}

//---------------------------------------------------------------

unsigned long CHashLongToStr::HashValue(const void * key)
{
    return (long)key;
}

//---------------------------------------------------------------

bool CHashLongToStr::Equal(const void * key1, const void * key2)
{
    return ((long)key1 == (long)key2);
}

//---------------------------------------------------------------

// ==============================================================

HASH_LIST_ITEM * CHashLongToPointer::NewItem(const void * key, const void * data)
{
    HASH_LIST_ITEM * pItem;

    pItem = (HASH_LIST_ITEM*)malloc(sizeof(HASH_LIST_ITEM));
    pItem->key  = (void*)key;
    pItem->data = (void*)data;
    pItem->next = NULL;

    return pItem;
}

//---------------------------------------------------------------

void CHashLongToPointer::FreeItem(HASH_LIST_ITEM * pItem)
{
    if (pItem)
    {
        free(pItem);
    }
}

//---------------------------------------------------------------

unsigned long CHashLongToPointer::HashValue(const void * key)
{
    return (long)key;
}

//---------------------------------------------------------------

bool CHashLongToPointer::Equal(const void * key1, const void * key2)
{
    return ((long)key1 == (long)key2);
}

//---------------------------------------------------------------

