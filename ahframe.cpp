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

#ifndef __WXMSW__
    #include "bitmaps/icon_logo.xpm"
#endif


//==========================================================================

BEGIN_EVENT_TABLE(CFlatPanel, wxPanel)
    EVT_SIZE  (CFlatPanel::OnSize)
END_EVENT_TABLE()

//--------------------------------------------------------------------------

CFlatPanel::CFlatPanel(wxWindow* parent)
          :wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize)
{
    m_pChild = NULL;
}

//--------------------------------------------------------------------------

void CFlatPanel::OnSize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();

    if (m_pChild)
        m_pChild->SetSize(0, 0, size.x, size.y, wxSIZE_ALLOW_MINUS_ONE);
}

//==========================================================================

CAhFrame::CAhFrame(wxWindow* parent, const char * title, long style)
           :wxFrame(parent, wxID_ANY, wxString::FromAscii(title), wxDefaultPosition, wxSize(10,10), style)
{
    memset(m_Panes , 0, sizeof(m_Panes ));
}

//--------------------------------------------------------------------------

void CAhFrame::Init(int layout, const char * szConfigSection)
{
    int x, y, w, h, saved_pos;

    m_sConfigSection  = szConfigSection;
    m_Layout          = layout;

    SetIcon(wxICON(icon_logo));
    x = atol(gpApp->GetConfig(szConfigSection, SZ_KEY_X1));
    y = atol(gpApp->GetConfig(szConfigSection, SZ_KEY_Y1));
    w = atol(gpApp->GetConfig(szConfigSection, SZ_KEY_X2)) - x;
    h = atol(gpApp->GetConfig(szConfigSection, SZ_KEY_Y2)) - y;
    saved_pos = atol(gpApp->GetConfig(szConfigSection, SZ_KEY_USE_SAVED_POS));

    // check to get the titlebar visible if config contains too small value for Y.
    // can be needed if the system menu bar is on top of screen
    if (0==saved_pos)
    {
        const char      * szName;
        const char      * szValue;
        if (gpApp->GetSectionFirst(SZ_SECT_REPORTS, szName, szValue) < 0)
        {
            int  x_old, y_old;
            GetPosition(&x_old, &y_old); 
            y = y_old;
        }
        gpApp->SetConfig(szConfigSection, SZ_KEY_USE_SAVED_POS, "1");
    }

    SetSize(x, y, w, h, wxSIZE_ALLOW_MINUS_ONE);

    if (gpApp->m_pAccel)
        SetAcceleratorTable(*gpApp->m_pAccel);
}

//--------------------------------------------------------------------------

void CAhFrame::Update()
{
}

//--------------------------------------------------------------------------

void CAhFrame::Done(BOOL SetClosedFlag)
{
    int x, y, w, h, i;
    if (gpApp)
    {
        GetPosition(&x, &y);
        GetSize    (&w, &h);

        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_X1, x);
        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_Y1, y);
        
        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_X2, x+w);
        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_Y2, y+h);

        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_OPEN, SetClosedFlag?"0":"1");

        for (i=0; i<AH_PANE_COUNT; i++)
            if (m_Panes[i])
                SetPane(i, NULL);
    }
}

//--------------------------------------------------------------------------

void CAhFrame::SetPane(int no, wxWindow * pane)
{
    m_Panes[no] = pane;
    gpApp->m_Panes[no] = pane;
}

//--------------------------------------------------------------------------

void CAhFrame::OnSaveOrders(wxCommandEvent& WXUNUSED(event))
{
    gpApp->SaveOrders(TRUE);
}


//--------------------------------------------------------------------------

void CAhFrame::OnNextUnit(wxCommandEvent& event)
{
    gpApp->SelectNextUnit();
}

//--------------------------------------------------------------------------

void CAhFrame::OnPrevUnit(wxCommandEvent& event)
{
    gpApp->SelectPrevUnit();
}

//--------------------------------------------------------------------------

void CAhFrame::OnUnitList(wxCommandEvent& event)
{
    gpApp->SelectUnitsPane();
}

//--------------------------------------------------------------------------


void CAhFrame::OnOrders(wxCommandEvent& event)
{
    gpApp->SelectOrdersPane();
}

//--------------------------------------------------------------------------



//==========================================================================

BEGIN_EVENT_TABLE(CResizableDlg, wxDialog)
    EVT_CLOSE  (    CResizableDlg::OnClose )
END_EVENT_TABLE()

CResizableDlg::CResizableDlg(wxWindow * parent, const char * title, const char * szConfigSection, long style)
              :wxDialog( parent, -1, wxString::FromAscii(title), wxDefaultPosition, wxDefaultSize, style | wxCAPTION  )
{
    // FIXME: modal dialogs?
    // style | wxDIALOG_MODAL used to be the default here
    m_sConfigSection = szConfigSection;
}

void CResizableDlg::SetSize()
{
    int x, y, w=-1, h=-1;

    x = atol(gpApp->GetConfig(m_sConfigSection.GetData(), SZ_KEY_X1));
    y = atol(gpApp->GetConfig(m_sConfigSection.GetData(), SZ_KEY_Y1));
    if (GetWindowStyle() & wxRESIZE_BORDER)
    {
        w = atol(gpApp->GetConfig(m_sConfigSection.GetData(), SZ_KEY_X2)) - x;
        h = atol(gpApp->GetConfig(m_sConfigSection.GetData(), SZ_KEY_Y2)) - y;
    }

    wxDialog::SetSize(x, y, w, h, wxSIZE_ALLOW_MINUS_ONE);
}

void CResizableDlg::SetPos()
{
    int x, y;

    x = atol(gpApp->GetConfig(m_sConfigSection.GetData(), SZ_KEY_X1));
    y = atol(gpApp->GetConfig(m_sConfigSection.GetData(), SZ_KEY_Y1));

    wxDialog::Move(x, y);
}

void CResizableDlg::StoreSize()
{
    int x, y, w, h;

    GetPosition(&x, &y);
    GetSize    (&w, &h);

    gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_X1, x);
    gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_Y1, y);
    gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_X2, x+w);
    gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_Y2, y+h);
}

void CResizableDlg::OnClose(wxCloseEvent& event)
{
    StoreSize();
    event.Skip();
}

//==========================================================================






















