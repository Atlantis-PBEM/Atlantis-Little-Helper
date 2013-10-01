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

#include "stdhdr.h"

#include "wx/listctrl.h"

#include "objs.h"
#include "data.h"

#include "listpane.h"


//------------------------------------------------------------------------

CListPane::CListPane(wxWindow *parent, wxWindowID id, long style)
          :wxListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style )
{
    m_pParent        = parent;
    m_pLayout        = NULL;
    m_pData          = NULL;

    for (int i=0; i<NUM_SORTS; i++)
        m_SortKey[i] = NULL;
    m_SortKey[NUM_SORTS-1] = PRP_ID;

}

//-----------------------------------------------------------------------

CListPane::~CListPane()
{
    for (int i=0; i<NUM_SORTS-1; i++)
    {
        if (m_SortKey[i])
            free((void*)m_SortKey[i]);
        m_SortKey[i] = NULL;
    }
}

//-----------------------------------------------------------------------

void CListPane::ApplyFonts()
{
}

//-----------------------------------------------------------------------

void CListPane::SetSortName(unsigned short key, const char * sortname)
{
    int               i;
    CListLayoutItem * p;
    wxListItem        item;
    CStr              S;
    int               width;

    if (!sortname)
        return; // can not set it
    if (key >= NUM_SORTS-1 )
        return;


    if (m_SortKey[key] && 0==stricmp(m_SortKey[key], sortname))
        return; // no need to set

    if (m_SortKey[key])
    {
        // reset old sort caption
        if (m_pLayout)
            for (i=0; i<m_pLayout->Count(); i++)
            {
                p = (CListLayoutItem*)m_pLayout->At(i);
                if (0==stricmp(p->m_Name, m_SortKey[key]))
                {
                    width  = GetColumnWidth(i);
                    item.m_mask = wxLIST_MASK_TEXT;
                    item.SetText(p->m_Caption);
                    SetColumn(i, item);
                    SetColumnWidth(i, width);
                }
            }

        free((void*)m_SortKey[key]);
    }

    m_SortKey[key] = sortname?strdup(sortname):NULL;

    // set new sort caption
    for (key=0; key<NUM_SORTS-1; key++)
        if (m_pLayout)
            for (i=0; i<m_pLayout->Count(); i++)
            {
                p = (CListLayoutItem*)m_pLayout->At(i);
                if (m_SortKey[key] && (0==stricmp(p->m_Name, m_SortKey[key])))
                {
                    S.Format("%d ", key+1);
                    S << p->m_Caption;
                    width  = GetColumnWidth(i);
                    item.m_mask   = wxLIST_MASK_TEXT;
                    item.SetText(S.GetData());
                    SetColumn(i, item);
                    SetColumnWidth(i, width);
                }
            }

}

//-----------------------------------------------------------------------

const char * CListPane::GetSortName(unsigned short key)
{
    if (key >= NUM_SORTS-1 )
        return NULL;
    return m_SortKey[key];
}

//------------------------------------------------------------------------

void CListPane::SetLayout()
{
    int               i;
    CListLayoutItem * p;

    DeleteAllColumns();

    for (i=0; i < NUM_SORTS-1; i++)
        if (m_SortKey[i])
        {
            free((void*)m_SortKey[i]);
            m_SortKey[i] = NULL;
        }


    if (m_pLayout)
        for (i=0; i<m_pLayout->Count(); i++)
        {
            p = (CListLayoutItem*)m_pLayout->At(i);

            InsertColumn(i, p->m_Caption, (p->m_Flags&LIST_FLAG_ALIGN_RIGHT)?wxLIST_FORMAT_RIGHT:wxLIST_FORMAT_LEFT, p->m_Width);
        }
    Refresh();
}

//------------------------------------------------------------------------

void CListPane::SetData(eSelMode selmode, long seldata, BOOL FullUpdate)
{
    int                row, col;
    TPropertyHolder  * dataitem;
    CListLayoutItem  * layoutitem;
    const void       * value;
    EValueType         valuetype;
    wxListItem         info;
    int                selno = -1;

    if (FullUpdate)
        DeleteAllItems();

    info.m_mask   = wxLIST_MASK_TEXT;
    if (m_pData && m_pLayout)
        for (row=0; row<m_pData->Count(); row++)
        {
            dataitem = (TPropertyHolder*)m_pData->At(row);
            info.m_itemId = row;
            for (col=0; col<m_pLayout->Count(); col++)
            {
                layoutitem = (CListLayoutItem*)m_pLayout->At(col);

                info.m_text.Empty();
                info.m_col= col;

                if (dataitem->GetProperty(layoutitem->m_Name, valuetype, value ))
                {
                    switch (valuetype)
                    {
                    case eLong:
                        if (((long)value<0) && (0==stricmp(PRP_ID, layoutitem->m_Name)))
                            value = 0; // mask negative ids of new units
                        info.m_text << (long)value;
                        break;
                    case eCharPtr:
                        info.m_text = (const char*)value;
                        break;
                    default:
                        break;
                    }
                }

                info.m_format = (layoutitem->m_Flags&LIST_FLAG_ALIGN_RIGHT)?wxLIST_FORMAT_RIGHT:wxLIST_FORMAT_LEFT;
                info.m_mask  |= wxLIST_MASK_FORMAT;

                if ( (0==col) && (row>=GetItemCount()) )
                {
                    if (dataitem->GetProperty( PRP_ID, valuetype, value ) && (eLong==valuetype))
                    {
                        info.m_data  = (long)value;
                        info.m_mask |= wxLIST_MASK_DATA;
                        if ((sel_by_id==selmode) && (selno<0) && ((long)value==seldata))
                            selno = row;
                    }
                    if ((sel_by_no==selmode) && (selno<0) && (row==seldata))
                        selno = row;

                    InsertItem(info);

                    info.m_mask   = wxLIST_MASK_TEXT;
                }
                else
                    SetItem(info);
            }
        }

    if (FullUpdate && (selno>=0))
    {
        SetItemState(selno, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        EnsureVisible(selno);
    }
}

//------------------------------------------------------------------------

void CListPane::Sort()
{
    long         idx;
    long         data=0;
    wxListItem   info;

    idx = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idx>=0)
    {
        info.SetId(idx);
        info.m_mask  = wxLIST_MASK_DATA;
        GetItem(info);
        data = info.m_data;
    }

    if (m_pData )
    {
        m_pData->SetSortMode(m_SortKey, NUM_SORTS);
        SetData(sel_by_id, data, TRUE);
    }
}

//------------------------------------------------------------------------

