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
#include "data.h"
#include "hash.h"

#include "objs.h"
#include "ahapp.h"
#include "editpane.h"

//--------------------------------------------------------------------


class CEditorForPane : public wxTextCtrl
{
public:
    CEditorForPane(CEditPane * parent);

protected :
    void         OnKillFocus(wxFocusEvent& event);
    void         OnMouseEvent(wxMouseEvent& event);

    CEditPane * m_pParent;

    DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(CEditorForPane, wxTextCtrl)
    EVT_KILL_FOCUS       (    CEditorForPane::OnKillFocus      )
    EVT_LEFT_DCLICK      (    CEditorForPane::OnMouseEvent     )
END_EVENT_TABLE()


//--------------------------------------------------------------------

CEditorForPane::CEditorForPane(CEditPane * parent)
               :wxTextCtrl(parent, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_AUTO_SCROLL )
{
    m_pParent = parent;
}

//--------------------------------------------------------------------

void CEditorForPane::OnKillFocus(wxFocusEvent& event) 
{
    m_pParent->OnKillFocus();
    event.Skip();
}

void CEditorForPane::OnMouseEvent(wxMouseEvent& event)
{
    // maybe it can be handled in the parent control, but just to be sure
    m_pParent->OnMouseDClick();
}

//====================================================================


BEGIN_EVENT_TABLE(CEditPane, wxPanel)
    EVT_SIZE             (                      CEditPane::OnSize           )
END_EVENT_TABLE()




//--------------------------------------------------------------------

CEditPane::CEditPane(wxWindow* parent, const char * header, BOOL editable, int WhichFont)
          :wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize)
{
    m_pSource       = NULL;   
    m_pChanged      = NULL; 
                   
    m_pHeader       = (header && *header)?(new wxStaticText(this, -1, wxString::FromAscii(header), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE )):NULL;
    m_HdrHeight     = 0;
    m_WhichFont     = WhichFont;
    m_pEditor       = new CEditorForPane(this);
    m_ColorNormal   = m_pEditor->GetBackgroundColour() ;

    m_ColorReadOnly.Set(APPLY_COLOR_DELTA(m_ColorNormal.Red()), 
                        APPLY_COLOR_DELTA(m_ColorNormal.Green()), 
                        APPLY_COLOR_DELTA(m_ColorNormal.Blue()));

    SetReadOnly(!editable);
}

CEditPane::~CEditPane()
{
}


//--------------------------------------------------------------------

void CEditPane::Init()
{
    ApplyFonts();
}

//--------------------------------------------------------------------

void CEditPane::SetSource(CStr * pSource, BOOL * pChanged)
{
    m_pSource   = pSource; 
    m_pChanged  = pChanged;
    m_pEditor->SetValue(pSource?wxString::FromAscii(pSource->GetData()):wxT(""));
}

//--------------------------------------------------------------------

void CEditPane::Update()
{
}

//--------------------------------------------------------------------

BOOL CEditPane::SaveModifications()
{
    if (m_pEditor->IsModified())
    {
        if (m_pSource)
            m_pSource->SetStr(m_pEditor->GetValue().mb_str());
        if (m_pChanged)
            *m_pChanged = TRUE;
        m_pEditor->DiscardEdits();

		if (!m_pSource && !m_pChanged)
			return FALSE;

        return TRUE;
    }
    else
        return FALSE;
}

//--------------------------------------------------------------------

void CEditPane::GetValue(CStr & value)
{
    value.SetStr(m_pEditor->GetValue().mb_str());
}

//--------------------------------------------------------------------

void CEditPane::ApplyFonts()
{
    m_pEditor->SetFont(*gpApp->m_Fonts[m_WhichFont]);

    if (m_pHeader)
    {
        wxCoord           w, h, descent, ext;

        m_pHeader->SetFont(*gpApp->m_Fonts[FONT_EDIT_HDR]);
        m_pHeader->GetTextExtent(wxT("A"), &w, &h, &descent, &ext);

        m_HdrHeight = h+2;
    }
}

//--------------------------------------------------------------------

void CEditPane::SetReadOnly(BOOL ReadOnly)
{
    if (m_pEditor)
    {
        m_pEditor->SetEditable(!ReadOnly);
        m_pEditor->SetBackgroundColour(ReadOnly?m_ColorReadOnly:m_ColorNormal);
    }
}

//--------------------------------------------------------------------

void CEditPane::OnKillFocus() 
{
    if (SaveModifications())
        gpApp->EditPaneChanged(this);
}

//--------------------------------------------------------------------

void CEditPane::OnSize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();

    if (m_pHeader && (m_HdrHeight>0))
        m_pHeader->SetSize(0, 0, size.x, m_HdrHeight, wxSIZE_ALLOW_MINUS_ONE);

    m_pEditor->SetSize(0, m_HdrHeight, size.x, size.y-m_HdrHeight, wxSIZE_ALLOW_MINUS_ONE);

}

//--------------------------------------------------------------------

void CEditPane::OnMouseDClick()
{
    gpApp->EditPaneDClicked(this);
}
