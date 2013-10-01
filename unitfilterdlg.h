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

#ifndef __AH_UTIL_UNIT_FILTER_DIALOG_INCL__
#define __AH_UTIL_UNIT_FILTER_DIALOG_INCL__


#define UNIT_SIMPLE_FLTR_COUNT 3

extern const char * UNIT_FILTER_OPERATION[];


class CUnitFilterDlg : public CResizableDlg
{
public:
    CUnitFilterDlg(wxWindow *parent, const char * szConfigSection);


    wxComboBox    * m_cbProperty[UNIT_SIMPLE_FLTR_COUNT];
    wxComboBox    * m_cbCompare [UNIT_SIMPLE_FLTR_COUNT];
    wxTextCtrl    * m_tcValue   [UNIT_SIMPLE_FLTR_COUNT];
    wxComboBox    * m_cbSetName;
    wxRadioButton * m_rbUseBoxes;
    wxRadioButton * m_rbUsePython;
    wxTextCtrl    * m_tcFilterText;
    wxCheckBox    * m_chUseSelectedHexes;
    wxCheckBox    * m_chDisplayOnMap;

    CStr            m_TrackingGroup;
    BOOL            m_bFindGoneUnits;

private:
    void Init();
    void Save();
    BOOL IsValid();
    void Load        (const char * szConfigSection);
    void LoadSetCombo(const char * setselect);
    void Reload      (const char * setname);
    void EnableBoxes (BOOL bOldBoxes);

    void OnButton       (wxCommandEvent& event);
    void OnRadioButton  (wxCommandEvent& event);

    void OnTextChange   (wxCommandEvent& event);
    void OnSelectChange (wxCommandEvent& event);

//    void OnSetNameChange(wxCommandEvent& event);
//    void OnSetNameSelect(wxCommandEvent& event);
    void OnBoxesChange  (wxCommandEvent& event);

    CStr         m_sControllingConfig;
    CStr         m_sCurrentSection;
    CStr         m_sSavedConfigSelected;
    BOOL         m_IsSaving;
    time_t       m_lastselect;
    wxButton   * m_btnSet   ;
    wxButton   * m_btnRemove;
    wxButton   * m_btnCancel;
    wxButton   * m_btnTracking;
    wxButton   * m_btnGone;
    wxButton   * m_btnHelp;
    wxColour     m_ColorNormal;
    wxColour     m_ColorReadOnly;

    BOOL         m_bReady;

    DECLARE_EVENT_TABLE()
};


#endif
