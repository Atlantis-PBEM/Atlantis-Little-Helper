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

#ifndef __AH_FRAME_INCL__
#define __AH_FRAME_INCL__


//-------------------------------------------------------------------

class CAhFrame : public wxFrame
{
public:
    CAhFrame(wxWindow* parent, const char * title, long style);

    virtual void    Init(int layout, const char * szConfigSection);
    virtual void    Update();
    virtual void    Done(BOOL SetClosedFlag);

    CStr            m_sConfigSection;

    wxWindow      * m_Panes [AH_PANE_COUNT ];

protected:
    void            SetPane(int no, wxWindow * pane);
    void            OnSaveOrders(wxCommandEvent& event);
    void            OnNextUnit(wxCommandEvent& event);
    void            OnPrevUnit(wxCommandEvent& event);
    void            OnUnitList(wxCommandEvent& event);
    void            OnOrders(wxCommandEvent& event);

    int             m_Layout;
};

//-------------------------------------------------------------------

class CFlatPanel : public wxPanel
{
public:
    CFlatPanel(wxWindow* parent);
    void SetChild(wxWindow* child) {m_pChild = child;};


private:
    void OnSize(wxSizeEvent& event);

    wxWindow * m_pChild;

    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------------

class CResizableDlg : public wxDialog
{
public:
    CResizableDlg(wxWindow * parent, const char * title, const char * szConfigSection, long style = wxRESIZE_BORDER | wxSYSTEM_MENU );

protected:
    void SetSize();
    void StoreSize();
    void SetPos();
    void OnClose (wxCloseEvent& event);

    CStr m_sConfigSection;

    DECLARE_EVENT_TABLE()
};


//-------------------------------------------------------------------

#endif

