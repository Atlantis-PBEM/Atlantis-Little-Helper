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

#ifndef __AH_FLAGS_DIALOG_INCL__
#define __AH_FLAGS_DIALOG_INCL__


#define ID_BTN_SET_ALL_LAND  100
#define ID_BTN_SET_ALL_UNIT  101
#define ID_BTN_RMV_ALL_LAND  102
#define ID_BTN_RMV_ALL_UNIT  103


enum E_FLAG_EDIT_MODE {eThisUnit, eManyUnits, eAll, eNames};
extern const char * LandFlagLabel[LAND_FLAG_COUNT];

//---------------------------------------------------------------------

class CUnitFlagsDlg : public CResizableDlg
{
public:
    CUnitFlagsDlg(wxWindow *parent, E_FLAG_EDIT_MODE Mode, unsigned int UnitFlags);
    ~CUnitFlagsDlg();

    unsigned int m_UnitFlags;
    unsigned int m_LandFlags;

private:

    void OnCancel       (wxCommandEvent& event);
    void OnOk           (wxCommandEvent& event);
    void OnSetClear     (wxCommandEvent& event);

    E_FLAG_EDIT_MODE     m_EditMode      ;

    wxButton           * m_btnOk         ;
    wxButton           * m_btnCancel     ;
                     
    wxButton           * m_btnSetAllLand ;
    wxButton           * m_btnSetAllUnit ;
    wxButton           * m_btnRmvAllLand ;
    wxButton           * m_btnRmvAllUnit ;
                     
    wxCheckBox         * m_chbUnitFlags[UNIT_CUSTOM_FLAG_COUNT];
    wxCheckBox         * m_chbLandFlags[LAND_FLAG_COUNT];
                     
    wxTextCtrl         * m_txtUnitFlagText[UNIT_CUSTOM_FLAG_COUNT];

    DECLARE_EVENT_TABLE()
};


//---------------------------------------------------------------------

class CMapFlagDlg : public wxDialog
{
public:
    CMapFlagDlg(wxWindow *parent, CLand * pLand, wxPoint & position);
    void OnButton(wxCommandEvent& event);

    wxTextCtrl * m_FlagText[LAND_FLAG_COUNT];

private:
    wxButton   * m_btnSet;
    wxButton   * m_btnRemove;
    wxButton   * m_btnCancel;
    CLand      * m_pLand;

    DECLARE_EVENT_TABLE()
};


//---------------------------------------------------------------------

#endif
