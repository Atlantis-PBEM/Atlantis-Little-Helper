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
#include "listpane.h"
#include "unitpane.h"
#include "unitfilterdlg.h"
#include "unitpanefltr.h"
#include "unitframefltr.h"

BEGIN_EVENT_TABLE(CUnitFrameFltr, CAhFrame)
    EVT_MENU   (menu_SaveOrders         , CUnitFrameFltr::OnSaveOrders)
    EVT_CLOSE  (                          CUnitFrameFltr::OnCloseWindow)
END_EVENT_TABLE()



//--------------------------------------------------------------------

CUnitFrameFltr::CUnitFrameFltr(wxWindow * parent)
               :CAhFrame (parent, "Unit Locator", (wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) & ~wxMINIMIZE_BOX)
{
}

//--------------------------------------------------------------------

const char * CUnitFrameFltr::GetConfigSection(int layout)
{
    // we do not depend on layout
    return SZ_SECT_WND_UNITS_FLTR;
}

//--------------------------------------------------------------------

void CUnitFrameFltr::Init(int layout, const char * szConfigSection)
{
    CUnitPaneFltr * p1;
    const char    * szConfigSectionHdr;

    szConfigSection    = GetConfigSection(layout);
    szConfigSectionHdr = gpApp->GetListColSection(SZ_SECT_LIST_COL_UNIT, SZ_KEY_LIS_COL_UNITS_FILTER);
    CAhFrame::Init(layout, szConfigSection);

    p1 = new CUnitPaneFltr(this);
    SetPane(AH_PANE_UNITS_FILTER, p1);


    p1->Init(this, szConfigSection, szConfigSectionHdr); //SZ_SECT_UNITLIST_HDR_FLTR);

}

//--------------------------------------------------------------------

void CUnitFrameFltr::Done(BOOL SetClosedFlag)
{
    CUnitPaneFltr * pUnitPane;

    pUnitPane  = (CUnitPaneFltr*)gpApp->m_Panes[AH_PANE_UNITS_FILTER];
    if (pUnitPane)
        pUnitPane->Done();

    CAhFrame::Done(SetClosedFlag);
}


//--------------------------------------------------------------------


void CUnitFrameFltr::OnCloseWindow(wxCloseEvent& event)
{
    gpApp->FrameClosing(this);
    Destroy();
}

