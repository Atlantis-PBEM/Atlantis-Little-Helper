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
#include "editpane.h"
#include "msgframe.h"

BEGIN_EVENT_TABLE(CMsgFrame, CAhFrame)
    EVT_CLOSE  (              CMsgFrame::OnCloseWindow)
END_EVENT_TABLE()


//--------------------------------------------------------------------

CMsgFrame::CMsgFrame(wxWindow * parent)
          :CAhFrame (parent, "Messages and Errors", (wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) & ~wxMAXIMIZE_BOX & ~wxMINIMIZE_BOX)
{
}

//--------------------------------------------------------------------

void CMsgFrame::Init(int layout, const char * szConfigSection)
{
    CEditPane         * p;

    szConfigSection = SZ_SECT_WND_MSG;
    CAhFrame::Init(layout, szConfigSection);

    p = new CEditPane(this, NULL  , FALSE, FONT_ERR_DLG);       
    SetPane(AH_PANE_MSG    , p);
    p->Init();
}

//--------------------------------------------------------------------

void CMsgFrame::OnCloseWindow(wxCloseEvent& event)
{
    gpApp->FrameClosing(this);
    Destroy();
}

//--------------------------------------------------------------------

