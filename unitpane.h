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

#ifndef __AH_UNIT_PANE_INCL__
#define __AH_UNIT_PANE_INCL__


class CUnitPane: public CListPane
{
public:
    CUnitPane(wxWindow *parent, wxWindowID id = list_units_hex);
    virtual void Init(CAhFrame * pParentFrame, const char * szConfigSection, const char * szConfigSectionHdr);
    virtual void Done();
    void         Update(CLand * pLand);
    virtual void ApplyFonts();
    CUnit      * GetUnit(long index);
    virtual void Sort();
    void         SelectUnit(long UnitId);
    void         SelectNextUnit();
    void         SelectPrevUnit();

    void         LoadUnitListHdr();
    void         SaveUnitListHdr();
    void         ReloadHdr(const char * szConfigSectionHdr);


    TPropertyHolderColl * m_pUnits;
    CLand               * m_pCurLand;

protected:
    void         OnSelected(wxListEvent& event);
    void         OnColClicked(wxListEvent& event);
    void         OnIdle(wxIdleEvent& event);
    void         OnRClick(wxListEvent& event);

    CAhFrame            * m_pFrame;
    //CCollection         * m_pFactions;

    CStr                  m_sConfigSection;
    CStr                  m_sConfigSectionHdr;

    int                   m_ColClicked;

public:

    void OnPopupMenuShareSilv         (wxCommandEvent& event);
    void OnPopupMenuTeach             (wxCommandEvent& event);
    void OnPopupMenuSplit             (wxCommandEvent& event);
    void OnPopupMenuDiscardJunk       (wxCommandEvent& event);
    void OnPopupMenuDetectSpies       (wxCommandEvent& event);
    void OnPopupMenuGiveEverything    (wxCommandEvent& event);
    void OnPopupMenuAddUnitToTracking (wxCommandEvent& event);
    void OnPopupMenuUnitFlags         (wxCommandEvent& event);
    void OnPopupMenuIssueOrders       (wxCommandEvent& event);


    DECLARE_EVENT_TABLE()
};

#endif
