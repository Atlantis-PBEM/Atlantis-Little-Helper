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

#ifndef __OPTIONS_DLG_INCL_H__
#define __OPTIONS_DLG_INCL_H__



typedef struct _AH_FONT_DATA
{
    int            size;
    int            family;
    int            style;
    int            weight;
    wxFontEncoding encoding;
    wxString       face;
}  AH_FONT_DATA;

//---------------------------------------------------------------------------

class COptionsDialog : public CResizableDlg
{
public:
    COptionsDialog(wxWindow * parent);
    void Init();
    void Done();

private:
    void OnOk      (wxCommandEvent& event);
    void OnCancel  (wxCommandEvent& event);
    void OnFont    (wxCommandEvent& event);
    void OnColor   (wxCommandEvent& event);
    void OnFaction (wxCommandEvent& event);
    void OnPassword(wxCommandEvent& event);

    bool            m_IsValid;
    bool            m_bFontChanged[FONT_COUNT];
    AH_FONT_DATA    m_FontData[FONT_COUNT]; // original data to restore on cancel
    CStrStrColl     m_ColorData;            // original data to restore on cancel
    CStrStrColl     m_FactionData;          // original data to restore on cancel

    wxComboBox    * m_pComboFonts;
    wxComboBox    * m_pComboColors;
    wxComboBox    * m_pComboFactions;

    wxCheckBox    * m_pChkLoadOrd;
    wxCheckBox    * m_pChkLoadRep;
    wxCheckBox    * m_pChkUnixStyle;
    wxCheckBox    * m_pChkHatchUnvisited;
    wxCheckBox    * m_pChkRClickCenters;
    //wxCheckBox    * m_pChkApplyDefaultEmpty;
    //wxCheckBox    * m_pChk3WinLayout;
    wxCheckBox    * m_pChkTeach;
    wxCheckBox    * m_pChkReadPwd;
    wxCheckBox    * m_pChkCheckProdReq;
    wxCheckBox    * m_pChkMoveMode;

    wxTextCtrl    * m_pTxtPassword;

    wxRadioButton * m_pRadio1Win;
    wxRadioButton * m_pRadio2Win;
    wxRadioButton * m_pRadio3Win;

    wxRadioButton * m_pRadioIconsSimple;
    wxRadioButton * m_pRadioIconsAdvanced;


    DECLARE_EVENT_TABLE()
};

#endif

