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
#include "cfgfile.h"
#include "files.h"
#include "compat.h"


#if defined(_MSC_VER)
    #ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
    #endif
#endif


#if defined(_WIN32)
    #define EOL_CFG "\r\n"
#else
    #define EOL_CFG "\n"
#endif
 

//====================================================================


CConfigFile::CConfigFile()
{
}

//---------------------------------------------------------------------------------

CConfigFile::~CConfigFile()
{
    FreeAll();
}

//---------------------------------------------------------------------------------

void CConfigFile::FreeItem(void * pItem)
{
    if (pItem)
    {
        if (((CONFIG_PARAM*)pItem)->szSection)
            free ( (void*)((CONFIG_PARAM*)pItem)->szSection );
        if (((CONFIG_PARAM*)pItem)->szName)
            free ( (void*)((CONFIG_PARAM*)pItem)->szName );
        if (((CONFIG_PARAM*)pItem)->szValue)
            free ( (void*)((CONFIG_PARAM*)pItem)->szValue );
        if (((CONFIG_PARAM*)pItem)->szComment)
            free ( (void*)((CONFIG_PARAM*)pItem)->szComment );
        free (pItem);
    }
}

//---------------------------------------------------------------------------------

int  CConfigFile::Compare (void * pItem1, void * pItem2)
{
    int x;

    x = SafeCmp( ((CONFIG_PARAM*)pItem1)->szSection, ((CONFIG_PARAM*)pItem2)->szSection);
    if (x!=0)
        return x;
    else
        return SafeCmp( ((CONFIG_PARAM*)pItem1)->szName, ((CONFIG_PARAM*)pItem2)->szName );
}

//---------------------------------------------------------------------------------

CONFIG_PARAM * NewParam(const char * szSection, const char * szName, const char * szValue, const char * szComment)
{
    CONFIG_PARAM * pPrm;

    if (!(szName && *szName))
        return NULL;

    // allow empty values, but not NULL
    if (!szValue)
        return NULL;

    pPrm = (CONFIG_PARAM*)malloc(sizeof(CONFIG_PARAM));
    if (szSection && *szSection)
    {
        pPrm->szSection  = strdup(szSection);
        pPrm->SectionLen = strlen(szSection);
    }
    else
    {
        pPrm->szSection  = NULL;
        pPrm->SectionLen = 0;
    }
    
    pPrm->szName      = strdup(szName);
    pPrm->szValue     = strdup(szValue);
    pPrm->NameLen     = strlen(szName);
    pPrm->ValueLen    = strlen(szValue); 
    if (szComment && *szComment)
    {
        pPrm->szComment   = strdup(szComment);
        pPrm->CommentLen  = strlen(szComment); 
    }
    else
    {
        pPrm->szComment   = NULL;
        pPrm->CommentLen  = 0; 
    }

    return pPrm;
}

//---------------------------------------------------------------------------------

BOOL CConfigFile::Load(const char * szFName)
{
    CFileReader         F;
    BOOL                Ok;
    char                ch;
    enum                EState {start, name, value, sect, cmnt} State;
    CStr                Name(128);
    CStr                Value(128);
    CStr                Section(128);
    CStr                Comment(128);
    CONFIG_PARAM      * pPrm;

    FreeAll();
    State = start;
    Ok    = F.Open(szFName);
    if (Ok)
        while (F.GetNextChar(ch))
        {
            switch (State)
            {
            case start:  switch (ch)
                         {
                         case '\r':
                         case '\n':
                         case '\t':
                         case ' ' : break;
                         case '#' :
                         case ';' : Comment.AddCh(ch);
                                    State = cmnt;
                                    break;
                         case '[' : State = sect;
                                    Section.Empty();
                                    break;
                         default  : Name.AddCh(ch);
                                    State = name;
                         }
                         break;
            
            case name:   switch (ch)
                         {
                         case '=' : State = value;
                                    break;
                         case '\n': Name.Empty();
                                    State=start;
                                    break;
                         default  : Name.AddCh(ch);
                         }
                         break;
            
            case value:  switch (ch)
                         {
                         case '\n': Section.TrimLeft (TRIM_ALL);
                                    Section.TrimRight(TRIM_ALL);
                                    Name   .TrimLeft (TRIM_ALL);
                                    Name   .TrimRight(TRIM_ALL);
                                    Value  .TrimLeft (TRIM_ALL);
                                    Value  .TrimRight(TRIM_ALL);
                                    Comment.TrimLeft (TRIM_ALL);
                                    Comment.TrimRight(TRIM_ALL);
                                    pPrm = NewParam(Section.GetData(), Name.GetData(), Value.GetData(), Comment.GetData());
                                    if (pPrm && !Insert(pPrm))
                                        FreeItem(pPrm);
                                    State = start;
                                    Name.Empty();
                                    Value.Empty();
                                    Comment.Empty();
                                    break;
                         default :  Value.AddCh(ch);
                         }
                         break;
            
            case sect:   switch (ch)
                         {
                         case ']':  State = start;
                                    break;
                         default :  Section.AddCh(ch);
                         }
                         break;

            case cmnt:   switch (ch)
                         {
                         case '\n': Comment.AddCh(ch);
                                    State = start;
                                    break;
                         default  : Comment.AddCh(ch);
                         }
                         break;


            }
        }

    if (!Name.IsEmpty())
    {
        Section.TrimLeft(TRIM_ALL);    
        Section.TrimRight(TRIM_ALL);   
        Name.TrimRight(TRIM_ALL);
        Value.TrimLeft(TRIM_ALL);
        Value.TrimRight(TRIM_ALL);
        pPrm = NewParam(Section.GetData(), Name.GetData(), Value.GetData(), Comment.GetData());
        if (pPrm && !Insert(pPrm))
            FreeItem(pPrm);
    }
    
    F.Close();

    return Ok;
}

//---------------------------------------------------------------------------------

BOOL CConfigFile::Save(const char * szFName)
{
    CFileWriter         F;
    BOOL                Ok;
    int                 i, n;
    CONFIG_PARAM      * pParam;
    int                 MaxNameLen = 0;
    int                 MinNameLen = 0x7FFFFFFF;
    char                Spaces[]   = "                                   ";
    int                 nSpaces; 
    CStr                CrntSection;

    Ok =  F.Open(szFName);
    if (Ok)
    {
        for (i=0; i<Count(); i++)
        {
            pParam = (CONFIG_PARAM*)At(i);

            // do not use empty/spaces only strings
            n = strlen(pParam->szValue);
            if ( (0==n) || (strspn(pParam->szValue, " \t\r\n")==(size_t)n) )
                continue;

            if (pParam->NameLen > MaxNameLen)
                MaxNameLen = pParam->NameLen;
            if (pParam->NameLen < MinNameLen)
                MinNameLen = pParam->NameLen;
        }

        for (i=0; i<Count(); i++)
        {
            pParam = (CONFIG_PARAM*)At(i);

            // do not store empty/spaces only strings
            n = strlen(pParam->szValue);
            if ( (0==n) || (strspn(pParam->szValue, " \t\r\n")==(size_t)n) )
                continue;

            if ( (0!=SafeCmp(pParam->szSection, CrntSection.GetData())) && (pParam->szSection) )
            {
                CrntSection = pParam->szSection;
                if (Ok)
                    Ok = F.WriteBuf(EOL_CFG, sizeof(EOL_CFG)-1) &&
                         F.WriteBuf("[", 1)  &&
                         F.WriteBuf(pParam->szSection, pParam->SectionLen) &&
                         F.WriteBuf("]", 1) &&
                         F.WriteBuf(EOL_CFG, sizeof(EOL_CFG)-1) &&
                         F.WriteBuf(EOL_CFG, sizeof(EOL_CFG)-1) ;
            }

            if (Ok && pParam->szComment)
                Ok = F.WriteBuf(EOL_CFG, sizeof(EOL_CFG)-1) &&
                     F.WriteBuf(pParam->szComment, pParam->CommentLen) &&
                     F.WriteBuf(EOL_CFG, sizeof(EOL_CFG)-1);

            nSpaces = MinNameLen+sizeof(Spaces)-1 - pParam->NameLen;
            if (nSpaces + pParam->NameLen > MaxNameLen)
                nSpaces = MaxNameLen - pParam->NameLen;
            if (Ok)
                Ok = F.WriteBuf(pParam->szName, pParam->NameLen);
            
            if (Ok && (nSpaces>0))
                Ok = F.WriteBuf(Spaces, nSpaces);

            if (Ok)
                Ok = F.WriteBuf(" = ", 3) &&
                     F.WriteBuf(pParam->szValue, pParam->ValueLen) &&
                     F.WriteBuf(EOL_CFG, sizeof(EOL_CFG)-1);

            if (!Ok)
                break;
        }
    

    }
    F.Close();

    return Ok;
}

//---------------------------------------------------------------------------------

const char * CConfigFile::GetByName(const char * szSection, const char * szName)
{
    CONFIG_PARAM * pPrm, Test;
    int            i; 

    Test.szSection = szSection;
    Test.szName    = szName;
    if (Search(&Test, i))
    {
        pPrm = (CONFIG_PARAM*)At(i);
        return pPrm->szValue;
    }
    else
        return NULL;
}

//---------------------------------------------------------------------------------

void CConfigFile::SetByName(const char * szSection, const char * szName, const char * szNewValue, const char * szComment)
{
    CONFIG_PARAM * pPrm, Test;
    int            i; 

    Test.szSection = szSection;
    Test.szName    = szName;
    if (Search(&Test, i))
    {
        if (NULL==szNewValue)
            AtFree(i);
        else
        {
            pPrm = (CONFIG_PARAM*)At(i);
            if (pPrm->szValue)
                free((void*)(pPrm->szValue));
            pPrm->szValue = strdup(szNewValue);
            pPrm->ValueLen= strlen(szNewValue); 
        }
    }
    else
    {
        pPrm = NewParam(szSection, szName,  szNewValue, szComment);
        if (pPrm && !Insert(pPrm))
            FreeItem(pPrm);
    }

}

//---------------------------------------------------------------------------------

int CConfigFile::GetFirstInSection(const char * szSection, const char *& szName, const char *& szValue)
{
    CONFIG_PARAM * pPrm, Test;
    int            i; 

    Test.szSection = szSection;
    Test.szName    = "";
    Search(&Test, i);

    pPrm = (CONFIG_PARAM*)At(i);
    if (pPrm && (0==stricmp(szSection, pPrm->szSection)) )
    {
        szName = pPrm->szName;
        szValue= pPrm->szValue;
        return i+1;
    }
    else
        return -1;
}

//---------------------------------------------------------------------------------

int CConfigFile::GetNextInSection (int idx, const char * szSection, const char *& szName, const char *& szValue)
{
    CONFIG_PARAM * pPrm;
    void         * p;

    p    = At(idx);
    pPrm = (CONFIG_PARAM*)p;
    if (pPrm && (0==stricmp(szSection, pPrm->szSection)) )
    {
        szName = pPrm->szName;
        szValue= pPrm->szValue;
        return idx+1;
    }
    else
        return -1;
}

//---------------------------------------------------------------------------------

void CConfigFile::RemoveSection(const char * szSection)
{
    CONFIG_PARAM * pPrm, Test;
    int            idx; 

    Test.szSection = szSection;
    Test.szName    = "";
    Search(&Test, idx);

    pPrm = (CONFIG_PARAM*)At(idx);
    while (pPrm && (0==stricmp(szSection, pPrm->szSection)) )
    {
        AtFree(idx);
        pPrm = (CONFIG_PARAM*)At(idx);
    }
}

//---------------------------------------------------------------------------------

BOOL CConfigFile::GetNextSection(const char * szPrevSection, const char *& szNextSection)
{
    CStr           S;
    char         * p;
    int            n;
    CONFIG_PARAM * pPrm, Test;
    int            i; 

    S = szPrevSection;
    n = S.GetLength();

    if (n>0)
    {
        p = S.AllocExtraBuf(2);
        p[0] = 1;
        p[1] = 0;
        S.UseExtraBuf(1);
    }

    Test.szSection = S.GetData();
    Test.szName    = "";
    Search(&Test, i);

    pPrm = (CONFIG_PARAM*)At(i);
    if (pPrm)
    {
        szNextSection = pPrm->szSection;
        return TRUE;
    }

    return FALSE;
}

//---------------------------------------------------------------------------------

