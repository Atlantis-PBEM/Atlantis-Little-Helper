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


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#include <malloc.h>
#include <assert.h>
//#include <wchar.h>
#include "cstr.h"
#include "compat.h"

#define MIN_DELTA   8

#if !defined(_MSC_VER)


char * _strupr(char * p)
{
    char * ret = p;

    while (*p)
    {
        *p = toupper(*p);
        p++;
    }
    return ret;
}

char * _strlwr(char * p)
{
    char * ret = p;

    while (*p)
    {
        *p = tolower(*p);
        p++;
    }
    return ret;
}

#endif


const char * stristr( const char * string, const char * image )
{
    char   first[3];
    int    imagelen;

    if ((NULL==image) || (0==*image))
        return string;

    imagelen = strlen(image);
    first[0] = tolower(image[0]);
    first[1] = toupper(image[0]);
    first[2] = 0;

    while (string)
    {
        string = strpbrk(string, first);
        if (string)
            if (0==strnicmp(string, image, imagelen))
                return string;
            else
                string++;
    }

    return NULL;
}

//---------------------------------------------------------------------------------

int SafeCmp(const char * s1, const char * s2)
{
    if (NULL==s1)
        if (NULL==s2)
            return 0;
        else
            return -1;
    else
        if (NULL==s2)
            return 1;
        else
            return stricmp(s1, s2);
}

//----------------------------------------------------------------------

int SafeCmpNoSpaces(const char * s1, const char * s2)
{
    if (NULL==s1)
        if (NULL==s2)
            return 0;
        else
            return -1;
    else
        if (NULL==s2)
            return 1;
        else
        {
            while (*s1 && *s2)
            {
                while (*s1 && *s1<=' ')
                    s1++;
                while (*s2 && *s2<=' ')
                    s2++;
                if (*s1 < *s2)
                    return -1;
                else if (*s1 > *s2)
                    return 1;
                s1++;
                s2++;
            }
            return 0;
        }
}

//----------------------------------------------------------------------

const char * SkipSpaces(const char * p)
{
    while (p && *p && (*p<=' '))
        p++;
    return (char *)p;
}

//==========================================================================

CStr::CStr()
{
    m_pData    = NULL;
    m_nDelta   = MIN_DELTA*2;
    m_nStrLen  = 0;
    m_nDataSize= 0;
}

//--------------------------------------------------------------------------

CStr::CStr(const char * src)
{
    m_pData    = NULL;
    m_nDelta   = MIN_DELTA*2;
    m_nStrLen  = 0;
    m_nDataSize= 0;
    AddStr(src);
}


//--------------------------------------------------------------------------

CStr::CStr(short int nDelta)
{
    m_pData    = NULL;
    m_nDelta   = nDelta;
    m_nStrLen  = 0;
    m_nDataSize= 0;

    if (m_nDelta<MIN_DELTA)
        m_nDelta = MIN_DELTA;
}

//--------------------------------------------------------------------------

CStr::CStr(const CStr & src)
{
    m_pData    = NULL;
    m_nDelta   = src.m_nDelta;
    m_nStrLen  = 0;
    m_nDataSize= 0;
    AddStr(src.m_pData, src.m_nStrLen);
}

//--------------------------------------------------------------------------

CStr::~CStr()
{
    if (m_pData)
       free(m_pData);

}


//--------------------------------------------------------------------------

void CStr::ReAllocate(int nNewSize)
{
     int nTheSize;

     if (nNewSize>0)
         nTheSize = ((nNewSize-1)/m_nDelta +1 )*m_nDelta;
     else
         nTheSize = 0;

     if (nTheSize!=m_nDataSize)
     {
         m_pData     = (char*)realloc(m_pData, nTheSize);
         m_nDataSize = nTheSize;
         if (m_nStrLen>m_nDataSize)
             m_nStrLen=m_nDataSize;
     }
}


//--------------------------------------------------------------------------

void CStr::AddCh(char ch)
{
    if (m_nStrLen+1 > m_nDataSize)
        ReAllocate(m_nStrLen+1);

    m_pData[m_nStrLen] = ch;
    m_nStrLen++;

}

//--------------------------------------------------------------------------

void CStr::DelCh(int nPos)
{
    if ((nPos<0) || (nPos>=m_nStrLen))
        return;

    memmove(&m_pData[nPos], &m_pData[nPos+1], m_nStrLen-(nPos+1) );
    m_nStrLen--;
}

//--------------------------------------------------------------------------

void CStr::SetCh(int nPos, char ch)
{
    if ((nPos<0) || (nPos>=m_nStrLen))
        return;
    m_pData[nPos] = ch;
}

//--------------------------------------------------------------------------

void CStr::AddLong(long lNum)
{
    char * p;
    ReAllocate(m_nStrLen + 32); // it must be enough for long...
    p = &(m_pData[m_nStrLen]);
    sprintf(p, "%ld", lNum);
    m_nStrLen += strlen(p);
}

//--------------------------------------------------------------------------

void CStr::AddULong(unsigned long ulNum)
{
    char * p;
    ReAllocate(m_nStrLen + 32); // it must be enough for long...
    p = &(m_pData[m_nStrLen]);
    sprintf(p, "%lu", ulNum);
    m_nStrLen += strlen(p);
}

//--------------------------------------------------------------------------

void CStr::AddDouble(double dNum, int width, int precision)
{
    char mask[64];
    sprintf(mask, "%s%d.%df", "%", width, precision);

    if (width < 312+precision)
        width = 312+precision;  // this must be enough if something like 0 specified...
    char * p;
    ReAllocate(m_nStrLen + width); // 16 chars must be enough for long...
    p = &(m_pData[m_nStrLen]);
    sprintf(p, mask, dNum);
    m_nStrLen += strlen(p);
}



//--------------------------------------------------------------------------

void CStr::AddBuf(const void * szData, int iDataLen)
{
    if ((NULL==szData) || (0==iDataLen))
        return;

    if (m_nStrLen+iDataLen > m_nDataSize)
        ReAllocate(m_nStrLen+iDataLen);

    memmove(&m_pData[m_nStrLen], szData, iDataLen);
    m_nStrLen += iDataLen;
}


//--------------------------------------------------------------------------

void CStr::InsBuf(const void * szData, int nPos, int iDataLen)
{
    if (nPos>m_nStrLen)
        nPos=m_nStrLen;

    if (m_nStrLen+iDataLen > m_nDataSize)
        ReAllocate(m_nStrLen+iDataLen);

    if (nPos < m_nStrLen)
        memmove(&m_pData[nPos+iDataLen], &m_pData[nPos], m_nStrLen-nPos);
    memmove(&m_pData[nPos], szData, iDataLen);
    m_nStrLen += iDataLen;
}

//--------------------------------------------------------------------------

void CStr::AddStr(const char * szS, int iSLen)
{
    if ( (NULL==szS) || (0==szS[0]) )
        return;

    if (0==iSLen)
        iSLen = strlen(szS);

    AddBuf(szS, iSLen);
}

//--------------------------------------------------------------------------

void CStr::SetStr(const char * szS, int iSLen)
{
    Empty();
    AddStr(szS,  iSLen);
}


//--------------------------------------------------------------------------

void CStr::InsStr(const char * szS, int nPos, int iSLen)
{
    if ( (NULL==szS) || (0==szS[0]) )
        return;

    if (0==iSLen)
        iSLen = strlen(szS);

    InsBuf(szS, nPos, iSLen);
}

//--------------------------------------------------------------------------

const char * CStr::GetData()
{
    AddCh(0);
    m_nStrLen--;

    return m_pData;
}


//--------------------------------------------------------------------------

void CStr::Empty()
{
    if (m_nDataSize>m_nDelta*2)
        ReAllocate(0);
    else
    {
        if (m_pData)
            m_pData[0] = 0;
        m_nStrLen  = 0;
    }
}


//--------------------------------------------------------------------------

char * CStr::AllocExtraBuf(int size)  // allocate buffer for external writer at the end of line
{
    if (m_nStrLen+size > m_nDataSize)
        ReAllocate(m_nStrLen+size);

    return &m_pData[m_nStrLen];
}


//--------------------------------------------------------------------------

void CStr::UseExtraBuf(int size)      // set extra string len
{
    if (m_nStrLen+size > m_nDataSize)
        ReAllocate(m_nStrLen+size);

    m_nStrLen += size;
}


//--------------------------------------------------------------------------

const char * CStr::ToUpper()
{
    GetData();
    return _strupr(m_pData);
}

//--------------------------------------------------------------------------

const char * CStr::ToLower()
{
    GetData();
    return _strlwr(m_pData);
}


//--------------------------------------------------------------------------

char * CStr::GetInteger(const char * Src, BOOL & Valid)
{
    Empty();

    while (Src)
    {
        if ( (*Src>='0') && (*Src<='9') )
            AddCh(*Src);
        else
            if ( ('-'==*Src) && (0==GetLength()) )
                AddCh(*Src);
            else
                break;
        Src++;
    }

    Valid = m_nStrLen > 1 || m_nStrLen == 1 && *m_pData != '-';

    return (char*)Src;
}


//--------------------------------------------------------------------------

// Only in 123.45 format!
char * CStr::GetDouble(const char * Src, BOOL & Valid)
{
    Empty();

    while (Src)
    {
        if ( (*Src>='0') && (*Src<='9') || (*Src=='.') )
            AddCh(*Src);
        else
            if ( ('-'==*Src) && (0==GetLength()) )
                AddCh(*Src);
            else
                break;
        Src++;
    }

    Valid = m_nStrLen > 1 || m_nStrLen == 1 && *m_pData != '-' && *m_pData != '.';

    return (char*)Src;
}


//--------------------------------------------------------------------------

BOOL CStr::IsInteger()
{
    BOOL   Ok = FALSE;
    int    pos = 0;
    const char * p = GetData();

    while (*p)
    {
        if ( *p>='0' && *p<='9' || 0==pos && '-'==*p)
            Ok = TRUE;
        else
            return FALSE;
        p++;
    }
    return Ok;
}

//--------------------------------------------------------------------------

char * CStr::GetToken(const char * Src, char Limit, TrimMode Mode, BOOL StripQuotes)
{
    char * p;

    Empty();

    if (NULL==Src)
        return NULL;

    // SH-EXCPT Quotes
    if (StripQuotes && '\"' == *Src)
    {
        p = (char*)strchr(Src + 1, '\"');
        Src++;
    }
    else
        p = (char*)strchr(Src,Limit);
    // SH-EXCPT

    if (NULL==p)
        AddStr(Src);
    else
    {
        if (p!=Src)
            AddStr(Src, p-Src);
        p++;
    }

    TrimLeft(Mode);
    TrimRight(Mode);

    return p;
}

//--------------------------------------------------------------------------

char * CStr::GetToken(const char * Src, const char * Limit, char & LimitUsed, TrimMode Mode, BOOL StripQuotes)
{
    char * p;

    Empty();
    LimitUsed = 0;

    if (NULL==Src)
        return NULL;

    // SH-EXCPT Quotes
    if (StripQuotes && '\"' == *Src)
    {
        p = (char*)strchr(Src + 1, '\"');
        Src++;
    }
    else
        p = (char*)strpbrk(Src,Limit);
    // SH-EXCPT Quotes

    if (NULL==p)
        AddStr(Src);
    else
    {
        if (p!=Src)
            AddStr(Src, p-Src);
        LimitUsed = *p;
        p++;
    }

    TrimLeft(Mode);
    TrimRight(Mode);

    return p;

}

//--------------------------------------------------------------------------

void CStr::TrimLeft(TrimMode Mode)
{
    if (TRIM_NONE==Mode)
        return;

    char * p = m_pData;
    int    i = 0;

    if (NULL==p)
        return;


    if (TRIM_SPACES==Mode)
        while ( (i<m_nStrLen) && ((' '==*p) || ('\t'==*p)) )
        {
            p++;
            i++;
        }
    else
        while ( (i<m_nStrLen) && (*p<=' ') )
        {
            p++;
            i++;
        }
    if (i>0)
    {
        m_nStrLen-=i;
        memmove(m_pData, p, m_nStrLen);
    }

}

//--------------------------------------------------------------------------

void CStr::TrimRight(TrimMode Mode)
{
    if (TRIM_NONE==Mode)
        return;
    if (TRIM_SPACES==Mode)
        while ( (m_nStrLen>0) && ((' '==m_pData[m_nStrLen-1]) || ('\t'==m_pData[m_nStrLen-1]))  )
            m_nStrLen--;
    else
        while ( (m_nStrLen>0) && (m_pData[m_nStrLen-1]<=' ') )
            m_nStrLen--;
}

//--------------------------------------------------------------------------

void CStr::Format(const char * lpszFormat, va_list argList)
{
    int     nMaxLen     = 0x0100;
    va_list argListSave; // = argList;
    int     err;

#if defined(_MSC_VER)
    argListSave = argList;
#else
    va_copy(argListSave, argList);
#endif
    while (nMaxLen<0x80000)  // if we cannot fit into half a meg, forget it
    {
        ReAllocate(nMaxLen);
        err = _vsnprintf(m_pData, nMaxLen, lpszFormat, argListSave);
        m_nStrLen = strlen(m_pData);
        if (err<nMaxLen)
            break;
        // FIXME: if err is not less than nMaxLen (no break), then
        // how can it be less than 0? dead code!
        if (err<0)
            nMaxLen <<= 1;    // just double it, man ;)))
        else
            nMaxLen = err+1;  // later versions of glibc return  the
                              // number  of  characters (excluding the trailing '\0') which
                              // would have been written to  the  final  string  if  enough
                              // space had been available.
    }
    va_end(argListSave);
}

//--------------------------------------------------------------------------

void CStr::Format(const char * lpszFormat, ...)
{
    va_list argList;

    va_start(argList, lpszFormat);
    Format(lpszFormat, argList);
    va_end(argList);
}

//--------------------------------------------------------------------------

int CStr::FindSubStr(const char * szS)
{
    const char * pFirst;

    if (!szS)
        return -1;

    pFirst = stristr(GetData(), szS);

    if (pFirst)
        return (pFirst - m_pData);

    return -1;
}

//--------------------------------------------------------------------------

int CStr::FindSubStrR(const char * szS)
{
    const char * p1 = NULL;
    const char * p2 = GetData();
    int    n;

    if (!szS)
        return -1;

    n  = strlen(szS);
    p2 = stristr(p2, szS);
    while (p2)
    {
        p1 = p2;
        p2 +=n;
        if (p2-m_pData >= GetLength())
            break;
        p2 = stristr(p2, szS);
    }

    if (p1)
        return (p1 - m_pData);

    return -1;
}

//--------------------------------------------------------------------------

void CStr::DelSubStr(int nPos, int nCount)
{
    if ((nPos < 0) || (nCount <= 0) || ((nPos + 1) > GetLength()))
        return;

    if ((nPos + nCount) > GetLength())
        nCount = GetLength() - nPos;

    memmove(&m_pData[nPos], &m_pData[nPos+nCount], m_nStrLen-(nPos+nCount));
    m_nStrLen = m_nStrLen - nCount;
    return;
}

//--------------------------------------------------------------------------

// remove all extra spaces, so two strings can be compared
void CStr::Normalize()
{
    int i;

    TrimRight(TRIM_ALL);
    TrimLeft(TRIM_ALL);

    for (i=m_nStrLen-1; i>0; i--)
        if (m_pData[i] <= ' ')
            if (m_pData[i-1] <= ' ')
                DelCh(i);
            else
                m_pData[i] = ' ';
}

//--------------------------------------------------------------------------

void CStr::RemoveLineBreaks()
{
    int i;

    for (i=m_nStrLen-1; i>0; i--)
        if ('\r' == m_pData[i] || '\n' == m_pData[i])
            DelCh(i);
}

//--------------------------------------------------------------------------

void CStr::Replace(char search, char replace_with)
{
    int i;

    for (i=m_nStrLen-1; i>0; i--)
        if (search == m_pData[i])
            m_pData[i] = replace_with;
}

//--------------------------------------------------------------------------

