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

#ifndef __AH_EDIT_PANE_INCL__
#define __AH_EDIT_PANE_INCL__


class CStr;

class CEditPane : public wxPanel
{
public:
    CEditPane(wxWindow* parent, const char * header, BOOL editable, int WhichFont);
    virtual     ~CEditPane();
    
    virtual void Update();
    virtual void Init();
    void         SetSource(CStr * pSource, BOOL * pChanged);
    virtual void ApplyFonts();
    BOOL         SaveModifications();
    void         OnKillFocus();
    void         OnMouseDClick();
    void         SetReadOnly(BOOL ReadOnly);
    void         GetValue(CStr & value);

    wxTextCtrl   * m_pEditor;


protected :
    void         OnSize      (wxSizeEvent & event);

    CStr         * m_pSource;
    BOOL         * m_pChanged;
    wxStaticText * m_pHeader;
    int            m_HdrHeight;
    int            m_WhichFont;     
    wxColour       m_ColorNormal;
    wxColour       m_ColorReadOnly;

    DECLARE_EVENT_TABLE()
};


#endif
