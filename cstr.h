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

#ifndef __CSTR_IS_A_SIMPLE_STRING__
#define __CSTR_IS_A_SIMPLE_STRING__

#include <stdarg.h>
#include <string.h>
#include "bool.h"

enum TrimMode {TRIM_NONE=0, TRIM_SPACES, TRIM_ALL};

class CStr
{
public:
    CStr();
    CStr(short int nDelta);
    CStr(const char * src);
    CStr(const CStr & src);
    ~CStr();

    void         AddCh(char ch);
    void         DelCh(int nPos);
    void         SetCh(int nPos, char ch);
    void         AddStr(const char * szS, int iSLen=0);
    void         SetStr(const char * szS, int iSLen=0);
    void         InsStr(const char * szS, int nPos, int iSLen=0);
    void         AddLong(long lNum);
    void         AddULong(unsigned long ulNum);
    void         AddDouble(double dNum, int width, int precision);
    void         AddBuf(const void * szData, int iDataLen);
    void         InsBuf(const void * szData, int nPos, int iDataLen);
    const char * GetData();
    void         Empty();
    const char * ToUpper();
    const char * ToLower();
    char       * GetToken(const char * Src, char Limit, TrimMode Mode=TRIM_SPACES, BOOL StripQuotes=TRUE );
    char       * GetToken(const char * Src, const char * Limit, char & LimitUsed, TrimMode Mode=TRIM_SPACES, BOOL StripQuotes=TRUE);
    char       * GetInteger(const char * Src, BOOL & Valid);
    char       * GetDouble(const char * Src, BOOL & Valid);  // Only in 123.45 format!
    void         TrimLeft(TrimMode Mode=TRIM_SPACES);
    void         TrimRight(TrimMode Mode=TRIM_SPACES);
    void         Format(const char * lpszFormat, ...);
    void         Format(const char * lpszFormat, va_list argList);
    int          FindSubStr(const char * szS);
    int          FindSubStrR(const char * szS);
    void         DelSubStr(int nPos, int nCount);
    char *       AllocExtraBuf(int size);  // Allocate buffer for external writer at the end of line
    void         UseExtraBuf(int size);    // Call it after AllocExtraBuf to change string length
    void         Normalize();
    void         RemoveLineBreaks();
    void         Replace(char search, char replace_with);
    BOOL         IsInteger();

    inline BOOL  IsEmpty()                           { return 0==m_nStrLen; }
    inline int   GetLength()                         { return m_nStrLen;  }

    inline CStr& operator<<(const char *  psz)       { AddStr(psz);  return *this;  }
    inline CStr& operator<<(char ch)                 { AddCh(ch);  return *this;  }
    inline CStr& operator<<(long lNum)               { AddLong(lNum);  return *this;  }
    inline CStr& operator<<(unsigned long ulNum)     { AddULong(ulNum);  return *this;  }
    inline CStr& operator<<(double dNum)             { AddDouble(dNum, 0, 2);  return *this;  }
    inline CStr& operator=(const char *  psz)        { Empty(); AddStr(psz); return *this; }
    inline CStr& operator=(CStr & S)                 { Empty(); AddStr(S.GetData(), S.GetLength()); return *this; }
    inline CStr& operator<<(CStr & S)                { AddStr(S.GetData(), S.GetLength()); return *this; }

private:
    char    * m_pData;
    short int m_nDelta;
    int       m_nStrLen;
    int       m_nDataSize;


    void ReAllocate(int nNewSize);

};




int SafeCmp(const char * s1, const char * s2);
int SafeCmpNoSpaces(const char * s1, const char * s2);
const char * SkipSpaces(const char * p);

#endif

