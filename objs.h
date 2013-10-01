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

#ifndef __OBJS_IS_OBJECTS_FOUNDATION_H_INCL__
#define __OBJS_IS_OBJECTS_FOUNDATION_H_INCL__

#include "cstr.h"
#include "collection.h"

enum EValueType
{
    eLong,
    eCharPtr,
    eObjPtr
};

enum EPropertyType
{
    eNormal,
    eOriginal,
    eBoth
};

#ifndef min
#define min(x,y) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef max
#define max(x,y) ( ((x) > (y)) ? (x) : (y) )
#endif


#define MAX_PROP_COLL_KEYS  16

// property errors

#define PE_OK              ( 0)
#define PE_NOT_IMPLEMENTED (-1)
#define PE_INV_NAME        (-2)
#define PE_INV_VALUE_TYPE  (-3)      
#define PE_INV_PROP_TYPE   (-4)      

//=============================================================

class CStrInt
{
public:
    CStrInt(){m_key=NULL; m_value=0;};
    CStrInt(const char * key, int value)
    {
        m_key       = key?strdup(key  ):NULL;
        m_value     = value;
    };
    ~CStrInt()
    {
        if (m_key)
            free((void*)m_key);
    }

    const char * m_key;
    int          m_value;
};


//-----------------------------------------------------------------

class CStrIntColl : public CSortedCollection
{
public:
    CStrIntColl()           : CSortedCollection()       {};
    CStrIntColl(int nDelta) : CSortedCollection(nDelta) {};
    virtual ~CStrIntColl() {FreeAll();};
protected:
    virtual void FreeItem(void * pItem) {if (pItem) delete (CStrInt*)pItem;};
    virtual int Compare(void * pItem1, void * pItem2) 
    {
        return SafeCmp( ((CStrInt*)pItem1)->m_key,  ((CStrInt*)pItem2)->m_key );
    };
};


//-------------------------------------------------------------------

class CStrStr
{
public:
    CStrStr(){m_key=NULL; m_value=NULL;};
    CStrStr(const char * key, const char * value)
    {
        m_key       = key?strdup(key  ):NULL;
        m_value     = value?strdup(value):NULL;
    };
    ~CStrStr()
    {
        if (m_key)
            free((void*)m_key);
        if (m_value)
            free((void*)m_value);
    }

    const char * m_key;
    const char * m_value;
};


//-----------------------------------------------------------------

class CStrStrColl : public CSortedCollection
{
public:
    CStrStrColl()           : CSortedCollection()       {};
    CStrStrColl(int nDelta) : CSortedCollection(nDelta) {};
    virtual ~CStrStrColl() {FreeAll();};
protected:
    virtual void FreeItem(void * pItem) {if (pItem) delete (CStrStr*)pItem;};
    virtual int Compare(void * pItem1, void * pItem2) 
    {
        return SafeCmp( ((CStrStr*)pItem1)->m_key,  ((CStrStr*)pItem2)->m_key );
    };
};


//-------------------------------------------------------------------

class CStrStrColl2 : public CSortedCollection
{
public:
    CStrStrColl2()           : CSortedCollection()       {};
    CStrStrColl2(int nDelta) : CSortedCollection(nDelta) {};
    virtual ~CStrStrColl2() {FreeAll();};
protected:
    virtual void FreeItem(void * pItem) {if (pItem) delete (CStrStr*)pItem;};
    virtual int Compare(void * pItem1, void * pItem2) 
    {
        int x = SafeCmp( ((CStrStr*)pItem1)->m_key,  ((CStrStr*)pItem2)->m_key );
        if (0==x)
            x = SafeCmp( ((CStrStr*)pItem1)->m_value,  ((CStrStr*)pItem2)->m_value );

        return x;
    };
};


//-------------------------------------------------------------------

struct TEvent
{
    unsigned long   id;
    void          * data;
};

//-------------------------------------------------------------------

class TObject
{
public:
    TObject() {};
    virtual ~TObject() {};

    virtual BOOL HandleEvent(TEvent * event) {return FALSE;};
};

//-------------------------------------------------------------------

class TProperty
{
public:
    TProperty();
    TProperty(const char * name, EValueType type, const void * value);
    ~TProperty();
    
    int     SetValue(EValueType     type,
                     const void  *  value, 
                     EPropertyType  proptype = eNormal
                    );

    const char * m_name;
    EValueType   m_type;
    void       * m_value;
    void       * m_valueorg;
};

//-------------------------------------------------------------------

class TPropertyColl : public CSortedCollection
{
public:
    TPropertyColl()           : CSortedCollection()       {};
    TPropertyColl(int nDelta) : CSortedCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem) {if (pItem) delete (TProperty*)pItem;};
    virtual int  Compare(void * pItem1, void * pItem2);
};

//-------------------------------------------------------------------

class TPropertyHolder : public TObject
{
public: 
    TPropertyHolder();
    virtual ~TPropertyHolder();

    virtual BOOL GetProperty(const char    *  name,
                             EValueType     & valuetype,
                             const void    *& value, // returns pointer to inner location
                             EPropertyType    proptype = eNormal
                            );
    virtual int  SetProperty(const char  *  name,
                             EValueType     type,
                             const void  *  value, 
                             EPropertyType  proptype = eNormal
                            );
    virtual void DelProperty(const char  *  name);
    virtual void ResetNormalProperties();
    virtual const char  * ResolveAlias(const char * alias) {return alias;};
    virtual CStrStrColl * GetPropertyGroups() {return NULL;};
    virtual const char  * GetPropertyName(int no);

protected:
    BOOL GetJustProperty    (const char    *  name,
                             EValueType     & valuetype,
                             const void    *& value, // returns pointer to inner location
                             EPropertyType    proptype = eNormal
                            );
    TPropertyColl  m_Properties;

};

//-------------------------------------------------------------------

class TPropertyHolderColl : public CResortableCollection
{
public:
    TPropertyHolderColl();
    TPropertyHolderColl(int nDelta);
    virtual ~TPropertyHolderColl();

    void SetSortMode(const char ** keys, int keycount);


protected:
    virtual void FreeItem(void * pItem);
    virtual int  Compare(void * pItem1, void * pItem2);

    void         ClearKeys();

    int    m_KeyCount;
    char * m_Key[MAX_PROP_COLL_KEYS];
};

//-------------------------------------------------------------------

#endif

