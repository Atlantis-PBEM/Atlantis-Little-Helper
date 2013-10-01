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
#include "files.h"
#include "cstr.h"

#include "stdhdr.h"
//#include "wx/msgdlg.h"
//#include "wx/dialog.h"




CFileReader::CFileReader() : m_Queue(256)
{
    m_f     = NULL;
    m_nPos  = 0;
    m_nSize = 0;
}

//---------------------------------------------------------------------

CFileReader::~CFileReader()
{
    Close();
}

//---------------------------------------------------------------------


BOOL  CFileReader::Open(const char * szFName)
{
    Close();

    if (szFName && *szFName)
        m_f = fopen(szFName, "rb");

    m_FileName = szFName;

    return (NULL != m_f);
}

//---------------------------------------------------------------------

void CFileReader::Close()
{
    if (m_f)
        fclose(m_f);
    m_f = NULL;
    m_nPos  = 0;
    m_nSize = 0;
}

//---------------------------------------------------------------------


BOOL CFileReader::GetNextChar(char & ch)
{
    // return first queued char
    if (m_Queue.GetLength()>0)
    {
        ch = m_Queue.GetData()[0];
        m_Queue.DelCh(0);
        return TRUE;
    }

    if (m_nPos >= m_nSize)
    {
        if ((!ReadMore()) || (m_nPos >= m_nSize))
            return FALSE;
    }


    ch = m_Buf[m_nPos];
    m_nPos++;
    return TRUE;

}


//---------------------------------------------------------------------

void CFileReader::QueueChar(char ch)
{
    m_Queue.AddCh(ch);
}

//---------------------------------------------------------------------

void CFileReader::QueueString(const char * p, int n)
{
    m_Queue.AddStr(p, n);
}

//---------------------------------------------------------------------

int CFileReader::GetNextLine(CStr & s)
{
    char ch;

    s.Empty();

    while (GetNextChar(ch))
    {
        s.AddCh(ch);
        if ('\n'==ch)
            break;
    }

    return (!s.IsEmpty());
}

//---------------------------------------------------------------------
/*
BOOL CFileReader::ReadMore()
{
    m_nPos  = 0;
    m_nSize = 0;

    if (!m_f)
        return FALSE;

    if (feof(m_f))
        return FALSE;

    m_nSize = fread(m_Buf, 1, RW_BUF_SIZE, m_f);
    if (0==m_nSize)
    {
        if (ferror(m_f))
        {
            CStr S;
            S.Format("Error reading file %s", m_FileName.GetData());
            wxMessageBox(S.GetData());
        }
        else
        {
            CStr S;
            S.Format("No error reading file %s, but still read 0 bytes", m_FileName.GetData());
            wxMessageBox(S.GetData());
        }
    }
    else
    {
        FILE * f;
        CStr   name;

        name.Format("%s_read_", m_FileName.GetData());
        if (f=fopen(name.GetData(), "ab"))
        {
            size_t n;
            n = fwrite(m_Buf, 1, m_nSize, f);
            fclose(f);
        }
        else
        {
            CStr S;
            S.Format("can not open log file %s for writing", name.GetData());
            wxMessageBox(S.GetData());
        }

    }

    return (m_nSize>0);
}
*/
//---------------------------------------------------------------------


BOOL CFileReader::ReadMore()
{
    int i;

    m_nPos  = 0;
    m_nSize = 0;

    if (!m_f)
        return FALSE;

    if (feof(m_f))
        return FALSE;

    for (i=0; i<3; i++)
    {
        m_nSize = fread(m_Buf, 1, RW_BUF_SIZE, m_f);
        if (0==m_nSize)
        {
            if (ferror(m_f))
            {
                //wxString S;
                // The stupid wxString does not compile on some configurations
                //S = wxString::Format(wxString("Error reading file %s"), m_FileName.GetData());
                //wxMessageBox(S);
                break;
            }
            else
                wxSleep(1); // maybe it will get better?
        }
        break;
    }

    return (m_nSize>0);
}


//=====================================================================


CFileWriter::CFileWriter() : m_s(1024)
{
    m_f     = NULL;
}

//---------------------------------------------------------------------

CFileWriter::~CFileWriter()
{
    Close();
}

//---------------------------------------------------------------------


BOOL CFileWriter::Open(const char * szFName, const char * szMode)
{
    Close();

    m_f = fopen(szFName, szMode);

    return (NULL != m_f);
}

//---------------------------------------------------------------------

void CFileWriter::Close()
{
    if (m_f)
    {
        Flush();
        fclose(m_f);
        m_f     = NULL;
    }
}

//---------------------------------------------------------------------


BOOL CFileWriter::WriteBuf(const char * szData, long nDataSize)
{
    if (!m_f)
        return FALSE;

    m_s.AddBuf(szData, nDataSize);
    if (m_s.GetLength() > RW_BUF_SIZE)
        return Flush();

    return TRUE;
}

//---------------------------------------------------------------------

BOOL CFileWriter::Flush()
{
    size_t n;

    n = fwrite(m_s.GetData(), 1, m_s.GetLength(), m_f);

    if (n < (size_t)m_s.GetLength())
        return FALSE;

    m_s.Empty();
    return TRUE;

}

//---------------------------------------------------------------------

