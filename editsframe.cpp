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
#include "wx/splitter.h"

#include "cstr.h"
#include "collection.h"
#include "cfgfile.h"
#include "files.h"
#include "atlaparser.h"
#include "consts.h"
#include "consts_ah.h"
#include "hash.h"

#include "ahapp.h"
#include "ahframe.h"
#include "editpane.h"
#include "editsframe.h"

BEGIN_EVENT_TABLE(CEditsFrame, CAhFrame)
    EVT_MENU   (menu_SaveOrders         , CEditsFrame::OnSaveOrders)  
    EVT_MENU   (accel_NextUnit          , CEditsFrame::OnNextUnit)
    EVT_MENU   (accel_PrevUnit          , CEditsFrame::OnPrevUnit)
    EVT_MENU   (accel_UnitList          , CEditsFrame::OnUnitList)
    EVT_MENU   (accel_Orders            , CEditsFrame::OnOrders  )
    EVT_CLOSE  (                          CEditsFrame::OnCloseWindow)
END_EVENT_TABLE()


//--------------------------------------------------------------------

CEditsFrame::CEditsFrame(wxWindow * parent)
            :CAhFrame (parent, "Editor panes", (wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) & ~wxMINIMIZE_BOX)
{
    m_Splitter1= NULL;
    m_Splitter2= NULL;
    m_Splitter3= NULL;
}

//--------------------------------------------------------------------

const char * CEditsFrame::GetConfigSection(int layout)
{
    switch (layout)
    {
    case AH_LAYOUT_2_WIN:        return SZ_SECT_WND_EDITS_2_WIN;
    case AH_LAYOUT_3_WIN:        return SZ_SECT_WND_EDITS_3_WIN;
    default:                     return "";
    }
}

//--------------------------------------------------------------------

void CEditsFrame::Init(int layout, const char * szConfigSection)
{
    CEditPane         *p1, *p2, *p3, *p4;
    //CFlatPanel        * panel1;
    long                y, x;

    szConfigSection = GetConfigSection(layout);

    switch (layout)
    {
    case AH_LAYOUT_3_WIN:
        CAhFrame::Init(layout, szConfigSection);

        m_Splitter1= new wxSplitterWindow(this       , -1, wxDefaultPosition, wxDefaultSize, wxSP_3D     | wxCLIP_CHILDREN);
        m_Splitter2= new wxSplitterWindow(m_Splitter1, -1, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH | wxCLIP_CHILDREN);
        m_Splitter3= new wxSplitterWindow(m_Splitter2, -1, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH | wxCLIP_CHILDREN);

        m_Splitter2->SetBorderSize(0);
        m_Splitter3->SetBorderSize(0);

        p1 = new CEditPane(m_Splitter1, "Hex description"        , FALSE, FONT_EDIT_DESCR);
        p2 = new CEditPane(m_Splitter2, "Unit description"       , FALSE, FONT_EDIT_DESCR);
        p3 = new CEditPane(m_Splitter3, "Orders"                 , FALSE, FONT_EDIT_ORDER);
        p4 = new CEditPane(m_Splitter3, "Comments/Default orders", TRUE , FONT_EDIT_ORDER);

        SetPane(AH_PANE_MAP_DESCR    , p1);
        SetPane(AH_PANE_UNIT_DESCR   , p2);
        SetPane(AH_PANE_UNIT_COMMANDS, p3);
        SetPane(AH_PANE_UNIT_COMMENTS, p4);

        p1->Init();
        p2->Init();
        p3->Init();
        p4->Init();


        y  = atol(gpApp->GetConfig(szConfigSection, SZ_KEY_HEIGHT_0));
        m_Splitter1->SetMinimumPaneSize(2);
        m_Splitter1->SplitHorizontally(p1, m_Splitter2, y);

        y  = atol(gpApp->GetConfig(szConfigSection, SZ_KEY_HEIGHT_1));
        m_Splitter2->SetMinimumPaneSize(2);
        m_Splitter2->SplitHorizontally(p2, m_Splitter3, y);

        x  = atol(gpApp->GetConfig(szConfigSection, SZ_KEY_WIDTH_0));
        m_Splitter3->SetMinimumPaneSize(2);
        m_Splitter3->SplitVertically(p3, p4, x);

        break;

    }
}

//--------------------------------------------------------------------

void CEditsFrame::Done(BOOL SetClosedFlag)
{
    switch (m_Layout)
    {
    case AH_LAYOUT_3_WIN:
        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_HEIGHT_0, m_Splitter1->GetSashPosition());
        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_HEIGHT_1, m_Splitter2->GetSashPosition());
        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_WIDTH_0 , m_Splitter3->GetSashPosition());

        break;

    }

    CAhFrame::Done(SetClosedFlag);
}


//--------------------------------------------------------------------


void CEditsFrame::OnCloseWindow(wxCloseEvent& event)
{
    gpApp->FrameClosing(this);
    Destroy();
}

//--------------------------------------------------------------------

