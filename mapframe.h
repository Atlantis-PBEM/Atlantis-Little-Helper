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

#ifndef __AH_MAP_FRAME_INCL__
#define __AH_MAP_FRAME_INCL__


class CMapFrame : public CAhFrame
{
public:
    CMapFrame(wxWindow * parent, int layout);

    virtual void    Init(int layout, const char * szConfigSection);
    virtual void    Done(BOOL SetClosedFlag);

    static const char * GetConfigSection(int layout);

private:

    void MakeMenu(int layout);
    void MakeToolBar();

    void OnLoadReport          (wxCommandEvent& event);
    void OnJoinReport          (wxCommandEvent& event);
    void OnLoadOrders          (wxCommandEvent& event);
    void OnSaveOrdersAs        (wxCommandEvent& event);
    void OnQuit                (wxCommandEvent& event);
    void OnQuitNoSave          (wxCommandEvent& event);
    void OnAbout               (wxCommandEvent& event);
    void OnTest                (wxCommandEvent& event);
    void OnToolbarCmd          (wxCommandEvent& event);
    void OnOptions             (wxCommandEvent& event);
    void OnViewSkillsAll       (wxCommandEvent& event);
    void OnViewSkillsNew       (wxCommandEvent& event);
    void OnViewItemsAll        (wxCommandEvent& event);
    void OnViewItemsNew        (wxCommandEvent& event);
    void OnViewObjectsAll      (wxCommandEvent& event);
    void OnViewObjectsNew      (wxCommandEvent& event);
    void OnViewBattlesAll      (wxCommandEvent& event);
    void OnViewEvents          (wxCommandEvent& event);
    void OnViewSecurityEvents  (wxCommandEvent& event);
    void OnViewNewProducts     (wxCommandEvent& event);
    void OnViewErrors          (wxCommandEvent& event);
    void OnViewGates           (wxCommandEvent& event);
    void OnViewCities          (wxCommandEvent& event);
    void OnViewProvinces       (wxCommandEvent& event);
    void OnViewFactionInfo     (wxCommandEvent& event);
    void OnViewFactionOverview (wxCommandEvent& event);
    void OnApplyDefaultOrders  (wxCommandEvent& event);
    void OnRerunOrders         (wxCommandEvent& event);
    void OnWriteMagesCSV       (wxCommandEvent& event);
    void OnCheckMonthLongOrd   (wxCommandEvent& event);
    void OnCheckTaxTrade       (wxCommandEvent& event);
    void OnCheckProduction     (wxCommandEvent& event);
    void OnCheckSailing        (wxCommandEvent& event);
    void OnFindHexes           (wxCommandEvent& event);
    void OnWindowUnits         (wxCommandEvent& event);
    void OnWindowMessages      (wxCommandEvent& event);
    void OnWindowEditors       (wxCommandEvent& event);
    void OnWindowUnitsFltr     (wxCommandEvent& event);
    void OnExportHexes         (wxCommandEvent& event);
    void OnFindTradeRoutes     (wxCommandEvent& event);
    void OnListCol             (wxCommandEvent& event);
    void OnFlagNames           (wxCommandEvent& event);
    void OnFlagsAllSet         (wxCommandEvent& event);

    void OnViewBattlesAllUpdate(wxUpdateUIEvent& event);
    void OnViewSkillsAllUpdate (wxUpdateUIEvent& event);
    void OnViewSkillsNewUpdate (wxUpdateUIEvent& event);
    void OnViewItemsAllUpdate  (wxUpdateUIEvent& event);
    void OnViewItemsNewUpdate  (wxUpdateUIEvent& event);
    void OnViewObjectsAllUpdate(wxUpdateUIEvent& event);
    void OnViewObjectsNewUpdate(wxUpdateUIEvent& event);
    void OnViewEventsUpdate    (wxUpdateUIEvent& event);
    void OnViewSecurityEventsUpdate(wxUpdateUIEvent& event);
    void OnViewNewProductsUpdate(wxUpdateUIEvent& event);
    void OnViewErrorsUpdate    (wxUpdateUIEvent& event);
    void OnViewGatesUpdate     (wxUpdateUIEvent& event);
    void OnToolbarUpdate       (wxUpdateUIEvent& event);

    void OnCloseWindow         (wxCloseEvent& event);



    wxSplitterWindow  * m_Splitter;
    wxSplitterWindow  * m_Splitter1;
    wxSplitterWindow  * m_Splitter2;
    wxSplitterWindow  * m_Splitter3;
    wxSplitterWindow  * m_Splitter4;

    DECLARE_EVENT_TABLE()
};


#endif

