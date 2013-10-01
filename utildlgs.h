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

#ifndef __AH_UTIL_DIALOGS_INCL__
#define __AH_UTIL_DIALOGS_INCL__



//--------------------------------------------------------------------------

class CShowOneDescriptionDlg : public CResizableDlg
{
public:
    CShowOneDescriptionDlg(wxWindow * parent, const char * title, const char * description);
    void OnButton(wxCommandEvent& event);
private:
    const char * m_descr;
    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------


class CShowDescriptionListDlg : public CResizableDlg
{
public:
    CShowDescriptionListDlg(wxWindow * parent, const char * title, CCollection * items);

private:
    void OnButton(wxCommandEvent& event);
    void OnLBDClick(wxCommandEvent & event);
    void OnLBSelect(wxCommandEvent & event);
    void SaveAs();
    wxListBox   * m_pList;
    CCollection * m_pItems;

    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------


class CExportMagesCSVDlg : public CResizableDlg
{
public:
    CExportMagesCSVDlg(wxWindow * parent, const char * fname);
    wxComboBox  * m_pSeparator;
    wxComboBox  * m_pOrientation;
    wxTextCtrl  * m_pFileName;
    int           m_nFormat;

private:
    void OnButton(wxCommandEvent& event);
    static int borderwidth;

    wxRadioButton * m_pRadio1;
    wxRadioButton * m_pRadio2;
    wxRadioButton * m_pRadio3;


    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------

class CHexExportDlg : public CResizableDlg
{
public:
    CHexExportDlg(wxWindow *parent);

    void OnButton(wxCommandEvent& event);

    wxTextCtrl * m_tcFName;

    wxRadioButton * m_rbHexNew;
    wxRadioButton * m_rbHexCurrent;
    wxRadioButton * m_rbHexSelected;
    wxRadioButton * m_rbHexAll;

    wxRadioButton * m_rbFileOverwrite;
    wxRadioButton * m_rbFileAppend;

    wxCheckBox    * m_chbInclStructs;
    wxCheckBox    * m_chbInclUnits;
    wxCheckBox    * m_chbInclTurnNoAcl;
    wxCheckBox    * m_chbInclResources;

private:
    wxButton   * m_btnOk    ;
    wxButton   * m_btnCancel;
    wxButton   * m_btnBrowse;

    DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------------

class CComboboxDlg : public wxDialog
{
public:
    CComboboxDlg(wxWindow *parent, const char * szTitle, const char * szMessage, const char * szChoices);

    CStr m_Choice;

private:
    void OnButton(wxCommandEvent& event);

    wxComboBox    * m_cbChoices;

    DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------------

class CGetTextDlg : public CResizableDlg
{
public:
    CGetTextDlg(wxWindow *parent, const char * szTitle, const char * szMessage);

    CStr m_Text;

private:
    void OnButton(wxCommandEvent& event);

    wxTextCtrl * m_tcText;
    DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------------

void ShowMessageBoxSwitchable(const char * szTitle, const char * szMessage, const char * szConfigKey);

#endif


