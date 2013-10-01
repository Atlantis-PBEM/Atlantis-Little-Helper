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

#ifndef __LIST_PANE_H__
#define __LIST_PANE_H__


#define LIST_FLAG_ALIGN_RIGHT   0x0001

enum eSelMode {sel_by_id, sel_by_no};


//--------------------------------------------------------------------

class CListLayoutItem
{
public:
    CListLayoutItem(const char * name, const char * caption, int width, int flags)
    {
        m_Name    = strdup(name?name:"");
        m_Caption = strdup(caption?caption:"");
        m_Width   = width;
        m_Flags   = flags;
        m_No      = 0;
    };
    ~CListLayoutItem()
    {
        if (m_Name)    free((void*)m_Name);
        if (m_Caption) free((void*)m_Caption);
    };

    const char   * m_Name;
    const char   * m_Caption;
    int            m_Width;
    int            m_No;
    unsigned long  m_Flags;
};

//--------------------------------------------------------------------

class CListLayout : public CCollection
{
public:
    CListLayout() : CCollection(16) {};
    ~CListLayout()  {FreeAll();};
protected:
    virtual void FreeItem(void * pItem) { if (pItem) delete (CListLayoutItem*)pItem; };
};

//--------------------------------------------------------------------

#define NUM_SORTS 4

class CListPane : public wxListCtrl
{
public:
    CListPane(wxWindow *parent, wxWindowID id, long style=wxLC_REPORT|wxLC_SINGLE_SEL);
    ~CListPane();

    void SetLayout();
    void SetData(eSelMode selmode, long seldata, BOOL FullUpdate);
    void SetSortName(unsigned short key, const char * sortname);
    const char * GetSortName(unsigned short key);
    virtual void Sort();
    virtual void ApplyFonts();

protected:

    wxWindow            * m_pParent;
    CListLayout         * m_pLayout;    // it is child's responsibility to handle this
    TPropertyHolderColl * m_pData;      // it is child's responsibility to handle this
    const char          * m_SortKey[NUM_SORTS];
    int                   m_nColCount;
};

//--------------------------------------------------------------------


#endif

