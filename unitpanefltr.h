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

#ifndef __AH_UNIT_PANE_FLTR_INCL__
#define __AH_UNIT_PANE_FLTR_INCL__


//----------------------------------------------------------------


class CUnitPaneFltr: public CUnitPane
{
public:
    CUnitPaneFltr(wxWindow *parent, wxWindowID id = list_units_hex_fltr);
    void         Update(CUnitFilterDlg * pFilter);
    virtual void Done();


    void InsertUnitInit();
    void InsertUnit(CUnit * pUnit);
    void InsertUnitDone();

    void OnPopupMenuFilter   (wxCommandEvent& event) ;

private:
    void ClearLandFlags();


    void OnSelected(wxListEvent& event);
    void OnColClicked(wxListEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnRClick(wxListEvent& event);

    void OnPopupMenuSetSort    (wxCommandEvent& event) ;
    void OnPopupMenuIssueOrders(wxCommandEvent& event);

    int  m_ColClickedFltr;
    BOOL m_IsUpdating;
    CBaseColl m_NewUnits;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------



#endif
