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


#include "stdlib.h"
#include "string.h"
#include "collection.h"
#include <stdarg.h>
#include "cstr.h"
#include "objs.h"
#include "compat.h"


//===================================================================

TProperty::TProperty()
{
    m_name      = NULL;
    m_type      = eLong;
    m_value     = (void*)0;
    m_valueorg  = (void*)0;
}

//-------------------------------------------------------------------

TProperty::TProperty(const char * name, EValueType type, const void * value)
{
    m_name      = name?strdup(name):NULL;
    m_type      = type;
    if (eCharPtr==type)
    {
        m_value     = value?strdup((const char *)value):NULL;
        m_valueorg  = value?strdup((const char *)value):NULL;
    }
    else
    {
        m_value     = (void*)value;
        m_valueorg  = (void*)value;
    }
}

//-------------------------------------------------------------------

TProperty::~TProperty()
{
    if (m_name)
        free((void*)m_name);
    if (eCharPtr==m_type) 
    {
        if (m_value)
            free(m_value);
        if (m_valueorg)
            free(m_valueorg);
    }
}

//-------------------------------------------------------------------

int TProperty::SetValue(EValueType     type,
                        const void  *  value, 
                        EPropertyType  proptype
                        )
{
    void     ** pvalue;

    if (type!=m_type)
        return PE_INV_VALUE_TYPE;

    switch (proptype)
    {
    case eNormal:
        pvalue = &m_value;
        break;
    case eOriginal:
        pvalue = &m_valueorg;
        break;
    default:
        return PE_INV_PROP_TYPE;
    }

    if (eCharPtr==type)
    {
        if (*pvalue)
            free(*pvalue);
        *pvalue     = value?strdup((const char *)value):NULL;
    }
    else
        *pvalue     = (void*)value;

    return PE_OK;
}


//===================================================================

int TPropertyColl::Compare(void * pItem1, void * pItem2) 
{
    return stricmp( ((TProperty*)pItem1)->m_name,  ((TProperty*)pItem2)->m_name );
}

//===================================================================

TPropertyHolder::TPropertyHolder()
{

}

//-------------------------------------------------------------------

TPropertyHolder::~TPropertyHolder()
{
    m_Properties.FreeAll();
}

//-------------------------------------------------------------------

BOOL TPropertyHolder::GetJustProperty(const char    *  name,
                                      EValueType     & valuetype,
                                      const void    *& value, // returns pointer to inner location
                                      EPropertyType    proptype 
                                     )       
{
    TProperty   Dummy;
    TProperty * pProp;
    int         idx;
    BOOL        Ok = FALSE;

    Dummy.m_name = name; //ResolveAlias(name);
    if (m_Properties.Search(&Dummy, idx))
    {
        pProp     = (TProperty*)m_Properties.At(idx);
        Ok        = TRUE;
        valuetype = pProp->m_type;
        switch (proptype)
        {
        case eNormal:
            value = pProp->m_value;
            break;
        case eOriginal:
            value = pProp->m_valueorg;
            break;
        default:
            value = NULL;
            Ok    = FALSE;
        }
    }
    Dummy.m_name = NULL;
    return Ok;
}

//-------------------------------------------------------------------

const char * TPropertyHolder::GetPropertyName(int no)
{
    TProperty * pProp;

    pProp = (TProperty*)m_Properties.At(no);

    return pProp?pProp->m_name:NULL;
}

//-------------------------------------------------------------------

BOOL TPropertyHolder::GetProperty(const char    *  name,
                                  EValueType     & valuetype,
                                  const void    *& value, // returns pointer to inner location
                                  EPropertyType    proptype 
                                  )       
{
    BOOL        Ok = FALSE;

    name = ResolveAlias(name);

    if (GetJustProperty(name, valuetype, value, proptype))
        Ok = TRUE;
    else  // check groups. value type must be long!
    {
        CStrStrColl * pSSC;
        CStrStr     * pSS;
        CStrStr       SSDummy;
        int           i;
        const void  * x;
        long          sum = 0;
        EValueType    vt;

        SSDummy.m_key = name;
        pSSC          = GetPropertyGroups();
        if (pSSC && pSSC->Search(&SSDummy, i))
        {
            pSS = (CStrStr*)pSSC->At(i);
            while (pSS && (0==stricmp(SSDummy.m_key, pSS->m_key)))
            {
                // do not use recursy so circular list will not destroy stack!
                if (GetJustProperty(pSS->m_value, vt, x, proptype) && (eLong==vt) )
                {
                    sum       += (long)x;
                    valuetype  = eLong;
                    Ok         = TRUE;
                }
                pSS = (CStrStr*)pSSC->At(++i);
            }
            if (Ok)
                value = (const void *)sum;
        }
        SSDummy.m_key = NULL;
    }
    
    return Ok;
}

//-------------------------------------------------------------------

int  TPropertyHolder::SetProperty(const char  *  name,
                                  EValueType     type,
                                  const void  *  value, 
                                  EPropertyType  proptype 
                                 )
{
    TProperty   Dummy;
    TProperty * pProp;
    int         idx;
    int         err = PE_OK;

    name = ResolveAlias(name);

    Dummy.m_name = name;
    if (m_Properties.Search(&Dummy, idx))
    {
        pProp = (TProperty*)m_Properties.At(idx);
        if (eBoth == proptype)
        {
            err = pProp->SetValue(type, value, eNormal);
            if (PE_OK==err)
                err = pProp->SetValue(type, value, eOriginal);
        }
        else
            err = pProp->SetValue(type, value, proptype);
    }
    else
    {
        pProp = new TProperty(name, type, value);
        m_Properties.Insert(pProp);
    }
    
    Dummy.m_name = NULL;
    return err;
}

//-------------------------------------------------------------------

void TPropertyHolder::DelProperty(const char  *  name)
{
    TProperty   Dummy;
    int         idx;

    name = ResolveAlias(name);

    Dummy.m_name = name;
    if (m_Properties.Search(&Dummy, idx))
        m_Properties.AtFree(idx);
    Dummy.m_name = NULL;
}

//-------------------------------------------------------------------

void TPropertyHolder::ResetNormalProperties()
{
    int         i;
    TProperty * pProp;

    for (i=0; i<m_Properties.Count(); i++)
    {
        pProp = (TProperty *)m_Properties.At(i);
        pProp->SetValue(pProp->m_type, pProp->m_valueorg, eNormal);
    }

}

//===================================================================

TPropertyHolderColl::TPropertyHolderColl() : CResortableCollection() 
{
    m_bDuplicates = TRUE;
    m_KeyCount    = 0;
    memset(m_Key, 0, sizeof(m_Key));
}

//-------------------------------------------------------------------

TPropertyHolderColl::TPropertyHolderColl(int nDelta) : CResortableCollection(nDelta)
{
    m_bDuplicates = TRUE;
    m_KeyCount    = 0;
    memset(m_Key, 0, sizeof(m_Key));
}
                                   
//-------------------------------------------------------------------

TPropertyHolderColl::~TPropertyHolderColl()
{
    ClearKeys();
}

//-------------------------------------------------------------------

void TPropertyHolderColl::ClearKeys()
{
    int i;

    for (i=0; i<min(m_KeyCount,MAX_PROP_COLL_KEYS); i++)
        if (m_Key[i])
        {
            free(m_Key[i]);
            m_Key[i] = NULL;
        }
    m_KeyCount = 0;
}

//-------------------------------------------------------------------

void TPropertyHolderColl::SetSortMode(const char ** keys, int keycount)
{
    int i;

    ClearKeys();
    for (i=0; i<min(keycount,MAX_PROP_COLL_KEYS); i++)
        if (keys[i] && *keys[i])
            m_Key[m_KeyCount++] = strdup(keys[i]);

    CResortableCollection::SetSortMode(m_SortMode^1);
}

//-------------------------------------------------------------------

void TPropertyHolderColl::FreeItem(void * pItem)
{
    if (pItem)
        delete (TPropertyHolder*) pItem;
}

//-------------------------------------------------------------------

int TPropertyHolderColl::Compare(void * pItem1, void * pItem2)
{
    int               n, x;
    const void      * p1,  * p2;
    BOOL              Ok1,   Ok2;
    EValueType        t1,    t2; 

    // are both pointers ok?
    if (!pItem1)
        if (!pItem2)
            return 0;
        else
            return -1;
    else
        if (!pItem2)
            return 1;

    for (n=0; n<min(m_KeyCount, MAX_PROP_COLL_KEYS); n++)
    {
        Ok1 = ((TPropertyHolder*)pItem1)->GetProperty(m_Key[n], t1, p1);
        Ok2 = ((TPropertyHolder*)pItem2)->GetProperty(m_Key[n], t2, p2);
    
        // do we have both properties?
        if (!Ok1)
            if (!Ok2)
                continue;
            else
                return -1;
        else
            if (!Ok2)
                return 1;
        
        // are the properties of the same type?
        if (t1!=t2)
            continue;

        switch (t1)
        {
        case eLong:  
            if ( (long)p1 > (long)p2 )
                return 1;
            else
                if ( (long)p1 < (long)p2 )
                    return -1;
            break;
        case eCharPtr:
            if (!p1)
                if (!p2)
                    continue;
                else
                    return -1;
            else
                if (!p2)
                    return 1;
            x = stricmp((const char*)p1, (const char*)p2);
            if (0!=x)
                return x;

            break;
        default:
            return 0; // can not compare...
        
        }
    }
    return 0;
}

//===================================================================

