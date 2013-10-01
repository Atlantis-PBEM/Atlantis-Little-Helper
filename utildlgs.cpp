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

#include "wx/listctrl.h"
#include "wx/dialog.h"


#include "cstr.h"
#include "collection.h"
#include "cfgfile.h"
#include "files.h"
#include "atlaparser.h"
#include "consts.h"
#include "consts_ah.h"
#include "hash.h"
#include "ahapp.h"
#include "ahframe.h"
#include "utildlgs.h"


//==========================================================================

BEGIN_EVENT_TABLE(CShowOneDescriptionDlg, CResizableDlg)
    EVT_BUTTON        (-1, CShowOneDescriptionDlg::OnButton)
END_EVENT_TABLE()

CShowOneDescriptionDlg::CShowOneDescriptionDlg(wxWindow * parent, const char * title, const char * description)
                       :CResizableDlg( parent, title, SZ_SECT_WND_DESCR_ONE)
{
    wxBoxSizer * topsizer = new wxBoxSizer( wxVERTICAL );
    wxButton   * pBtnDone = new wxButton  (this, wxID_CANCEL, wxT("Done") );
    wxButton   * pBtnSave = new wxButton  (this, wxID_OK    , wxT("Save as") );
    //wxTextCtrl * pText    = new wxTextCtrl(this, -1, description, wxDefaultPosition, wxDefaultSize,
    //                                       wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL );
    wxTextCtrl * pText    = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize,
                                           wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL );
    const char * p;
    int          count = 0;
    CStr         S;

    topsizer->Add(  pText  ,
                    1,            // make vertically  stretchable
                    wxEXPAND );//| wxALL,    // make horizontally stretchable  //   and make border all around


    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );

    button_sizer->Add(  pBtnSave, 0, wxALL, 10 );
    button_sizer->Add(  pBtnDone, 0, wxALL, 10 );

    topsizer->Add( button_sizer, 0, wxALIGN_CENTER );

    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    pText->SetFocus();
    pBtnDone ->SetDefault();

    CResizableDlg::SetSize();

    pText->SetValue(wxString::FromAscii(description));
    pText->SetFont(*gpApp->m_Fonts[FONT_VIEW_DLG]);
    m_descr = description;

    p = description;
    while (p && *p && count++ < 3)
    {
        p = S.GetToken(p, '(', TRIM_ALL);
        p = S.GetToken(p, ')', TRIM_ALL);
        if (gpApp->SelectLand(S.GetData()))
            break;
    }
}


//--------------------------------------------------------------------------

void CShowOneDescriptionDlg::OnButton(wxCommandEvent& event)
{
    if (event.GetId()==wxID_OK)
    {
        int         err;
        wxString CurrentDir = wxGetCwd();
        if (!m_descr)
            return;
        wxFileDialog dialog(GetParent(),
                            wxT("Save current text"),
                            wxT(""),
                            wxT(""),
                            wxT(SZ_ALL_FILES),
                            wxSAVE |  wxOVERWRITE_PROMPT );
        err = dialog.ShowModal();
        wxSetWorkingDirectory(CurrentDir);

        if (wxID_OK == err)
        {
            CFileWriter F;
            if (F.Open(dialog.GetPath().mb_str()))
            {
                F.WriteBuf(m_descr, strlen(m_descr));

                F.Close();
            }
            else
                wxMessageBox(wxT("Can not open file"));

        }

    }
    else if (event.GetId()==wxID_CANCEL)
    {
        StoreSize();
        EndModal(wxID_CANCEL);
    }
}

//==========================================================================

BEGIN_EVENT_TABLE(CShowDescriptionListDlg, CResizableDlg)
    EVT_BUTTON        (-1, CShowDescriptionListDlg::OnButton)
    EVT_LISTBOX       (-1, CShowDescriptionListDlg::OnLBSelect)
    EVT_LISTBOX_DCLICK(-1, CShowDescriptionListDlg::OnLBDClick)
END_EVENT_TABLE()



CShowDescriptionListDlg::CShowDescriptionListDlg(wxWindow * parent, const char * title, CCollection * items)
                        :CResizableDlg( parent, title, SZ_SECT_WND_DESCR_LIST)
{
    long i;
    CBaseObject * pObj;
    wxButton    * pBtnView;
    wxButton    * pBtnStoreSize;
    wxButton    * pBtnSave;


    m_pItems = items;
    m_pList = new wxListBox(this, -1, wxDefaultPosition, wxDefaultSize,
                               0, NULL, wxLB_SINGLE | wxLB_NEEDED_SB );

    for (i=0; i<items->Count(); i++)
    {
        pObj = (CBaseObject * )items->At(i);
        m_pList->Append(wxString::FromAscii(pObj->Name.GetData()), (void *)i);
    }


    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    pBtnView         = new wxButton  (this, wxID_OK    , wxT("View") );
    pBtnStoreSize    = new wxButton  (this, wxID_CANCEL, wxT("Done") );
    pBtnSave         = new wxButton  (this, wxID_SAVE  , wxT("Save as") );

    topsizer->Add(  m_pList  ,
                    1,                  // make vertically stretchable
                    wxEXPAND | wxALL,   // make horizontally stretchable //   and make border all around
                    4 );                // set border width to 10

    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );

    button_sizer->Add(  pBtnView,
                        0,           // make horizontally unstretchable
                        wxALL,       // make border all around (implicit top alignment)
                        10 );        // set border width to 10
    button_sizer->Add(  pBtnSave,
                        0,           // make horizontally unstretchable
                        wxALL,       // make border all around (implicit top alignment)
                        10 );        // set border width to 10
    button_sizer->Add(  pBtnStoreSize,
                        0,           // make horizontally unstretchable
                        wxALL,       // make border all around (implicit top alignment)
                        10 );        // set border width to 10

    topsizer->Add( button_sizer,
                   0,                // make vertically unstretchable
                   wxALIGN_CENTER ); // no border and centre horizontally

    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    m_pList->SetSelection(0);
    m_pList->SetFocus();
    pBtnView->SetDefault();

    CResizableDlg::SetSize();
}

//--------------------------------------------------------------------------

void CShowDescriptionListDlg::OnButton(wxCommandEvent& event)
{
    if (event.GetId()==wxID_OK)
        OnLBDClick(event);
    else if (event.GetId()==wxID_CANCEL)
    {
        StoreSize();
        EndModal(wxID_CANCEL);
    }
    else if (event.GetId()==wxID_SAVE)
        SaveAs();
}

//--------------------------------------------------------------------------

void CShowDescriptionListDlg::SaveAs()
{
    int            err, i;
    CBaseObject  * pObj;
    CStr           S(128);

    wxString CurrentDir = wxGetCwd();
    wxFileDialog dialog(GetParent(),
                        wxT("Save current text"),
                        wxT(""),
                        wxT(""),
                        wxT(SZ_ALL_FILES),
                        wxSAVE |  wxOVERWRITE_PROMPT );
    err = dialog.ShowModal();
    wxSetWorkingDirectory(CurrentDir);

    if (wxID_OK == err)
    {
        CFileWriter F;
        if (F.Open(dialog.GetPath().mb_str()))
        {
            for (i=0; i<m_pItems->Count(); i++)
            {
                pObj = (CBaseObject*)m_pItems->At(i);
                S    = pObj->Description;
                S.TrimRight(TRIM_ALL);
                S << EOL_FILE;
                F.WriteBuf(S.GetData(), S.GetLength());
            }
            F.Close();
        }
        else
            wxMessageBox(wxT("Can not open file"));

    }

}

//--------------------------------------------------------------------------

void CShowDescriptionListDlg::OnLBDClick(wxCommandEvent & event)
{
    CBaseObject  * pObj;

    pObj = (CBaseObject*)m_pItems->At((long)m_pList->GetClientData(m_pList->GetSelection()));
    if (pObj)
    {
        CShowOneDescriptionDlg dlg(GetParent(), pObj->Name.GetData(), pObj->Description.GetData());
        dlg.ShowModal();
        m_pList->SetFocus();
    }
}

//--------------------------------------------------------------------------

void CShowDescriptionListDlg::OnLBSelect(wxCommandEvent & event)
{
    CBaseObject  * pObj;
    CStr           S;
    const char   * p;

    pObj = (CBaseObject*)m_pItems->At((long)m_pList->GetClientData(m_pList->GetSelection()));
    if (pObj)
    {
        p = pObj->Name.GetData();
        while (p && *p)
        {
            p = S.GetToken(p, '(', TRIM_ALL);
            p = S.GetToken(p, ')', TRIM_ALL);
            if (gpApp->SelectLand(S.GetData()))
                break;
        }
    }
}

//==========================================================================

enum
{
    MCSV_RADIO_1 = wxID_HIGHEST + 10,
    MCSV_RADIO_2,
    MCSV_RADIO_3
};

BEGIN_EVENT_TABLE(CExportMagesCSVDlg, CResizableDlg)
    EVT_BUTTON        (-1, CExportMagesCSVDlg::OnButton)
END_EVENT_TABLE()

int CExportMagesCSVDlg::borderwidth = 4;

CExportMagesCSVDlg::CExportMagesCSVDlg(wxWindow * parent, const char * fname)
                   :CResizableDlg( parent, "Export mages", SZ_SECT_WND_EXP_MAGES_CSV)
{
    wxButton    * pBtn;
    wxStaticText* pStatic;
    const char  * p;
    wxString      FName = wxString::FromAscii(fname);

    wxBoxSizer  * topsizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer  * horsizer;
    wxBoxSizer  * colsizer;
    wxBoxSizer  * layoutsizer;

    m_pSeparator = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN);
    m_pSeparator->Append(wxT(","));
    m_pSeparator->Append(wxT(";"));
    p = SkipSpaces(gpApp->GetConfig(m_sConfigSection.GetData(), SZ_KEY_SEPARATOR));
    if (p && *p)
        m_pSeparator->SetValue(wxString::FromAscii(p));
    else
        m_pSeparator->SetSelection(0);

    m_pOrientation = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN );
    m_pOrientation->Append(wxT(SZ_VERTICAL));
    m_pOrientation->Append(wxT(SZ_HORIZONTAL));
    p = SkipSpaces(gpApp->GetConfig(m_sConfigSection.GetData(), SZ_KEY_ORIENTATION));
    if (p && *p)
        m_pOrientation->SetValue(wxString::FromAscii(p));
    else
        m_pOrientation->SetSelection(0);

    m_pRadio1 = new wxRadioButton(this, MCSV_RADIO_1, wxT("Decorated"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_pRadio2 = new wxRadioButton(this, MCSV_RADIO_2, wxT("days"));
    m_pRadio3 = new wxRadioButton(this, MCSV_RADIO_3, wxT("level(days)"));

    m_nFormat = atol(gpApp->GetConfig(m_sConfigSection.GetData(), SZ_KEY_FORMAT));
    switch (m_nFormat)
    {
        case 0:   m_pRadio1->SetValue(TRUE); break;
        case 1:   m_pRadio2->SetValue(TRUE); break;
        case 2:   m_pRadio3->SetValue(TRUE); break;
    }

    m_pFileName = new wxTextCtrl(this, -1, FName);

    // -------------

    horsizer = new wxBoxSizer( wxHORIZONTAL );


    colsizer = new wxBoxSizer( wxVERTICAL );
        pStatic  = new wxStaticText(this, -1, wxT("Format :"), wxDefaultPosition, wxDefaultSize);
        colsizer->Add(pStatic, 0, wxALL, borderwidth );
        colsizer->Add(m_pRadio1, 0, wxALIGN_LEFT | wxALL, 1);
        colsizer->Add(m_pRadio2, 0, wxALIGN_LEFT | wxALL, 1);
        colsizer->Add(m_pRadio3, 0, wxALIGN_LEFT | wxALL, 1);
    horsizer->Add(colsizer, 0, wxALIGN_CENTER | wxALL | wxGROW, borderwidth*4 );


    colsizer = new wxBoxSizer( wxVERTICAL );
        layoutsizer = new wxBoxSizer( wxHORIZONTAL );
            pStatic  = new wxStaticText(this, -1, wxT("Separator :"), wxDefaultPosition, wxDefaultSize);
            layoutsizer->Add(pStatic     , 1, wxALIGN_RIGHT | wxALL, borderwidth );
            layoutsizer->Add(m_pSeparator, 2, wxALIGN_RIGHT | wxALL, borderwidth );
        colsizer->Add(layoutsizer, 0, wxALL, borderwidth );

        layoutsizer = new wxBoxSizer( wxHORIZONTAL );
            pStatic  = new wxStaticText(this, -1, wxT("Orientation :"), wxDefaultPosition, wxDefaultSize);
            layoutsizer->Add(pStatic       , 1, wxALIGN_RIGHT | wxALL, borderwidth );
            layoutsizer->Add(m_pOrientation, 2, wxALIGN_RIGHT | wxALL, borderwidth );
        colsizer->Add(layoutsizer, 0, wxALL, borderwidth );
    horsizer->Add(colsizer, 0, wxALIGN_CENTER | wxALL | wxGROW, borderwidth );

    topsizer->Add( horsizer, 0, wxALIGN_CENTER | wxALL | wxGROW );

    // -------------


    horsizer = new wxBoxSizer( wxHORIZONTAL );

    pStatic  = new wxStaticText(this, -1, wxT("Export to file :"), wxDefaultPosition, wxDefaultSize);
    horsizer->Add(pStatic, 0, wxALL, borderwidth );

    horsizer->Add(m_pFileName, 1, wxALL, borderwidth );

    pBtn = new wxButton  (this, wxID_SETUP , wxT("Browse") );
    horsizer->Add(pBtn, 0, wxALL, borderwidth );


    topsizer->Add( horsizer, 0, wxALIGN_CENTER | wxALL | wxGROW  );

    // -------------


    horsizer = new wxBoxSizer( wxHORIZONTAL );

    pBtn = new wxButton  (this, wxID_OK    , wxT("Ok") );
    horsizer->Add(pBtn, 0, wxALL, borderwidth );

    pBtn = new wxButton  (this, wxID_CANCEL    , wxT("Cancel") );
    horsizer->Add(pBtn, 0, wxALL, borderwidth );

    topsizer->Add( horsizer, 0, wxALIGN_CENTER | wxALL | wxGROW  );

    // -------------

    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    CResizableDlg::SetSize();
}


void CExportMagesCSVDlg::OnButton(wxCommandEvent& event)
{
    wxString S;

    switch (event.GetId())
    {
    case wxID_OK:
        S = m_pSeparator->GetValue();
        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_SEPARATOR, S.mb_str());
        S = m_pOrientation->GetValue();
        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_ORIENTATION, S.mb_str());
        m_nFormat = 0;
        if (m_pRadio1->GetValue())
            m_nFormat = 0;
        else if (m_pRadio2->GetValue())
            m_nFormat = 1;
        else if (m_pRadio3->GetValue())
            m_nFormat = 2;
        S.Empty();
        S << m_nFormat;
        gpApp->SetConfig(m_sConfigSection.GetData(), SZ_KEY_FORMAT, S.mb_str());

        StoreSize();
        EndModal(wxID_OK);
        break;

    case wxID_CANCEL:
        StoreSize();
        EndModal(wxID_CANCEL);
        break;

    case wxID_SETUP:  // browse button
        {
            int         err;
            wxString CurrentDir = wxGetCwd();
            wxFileDialog dialog(GetParent(),
                                wxT("Save mages info"),
                                wxT(""),
                                m_pFileName->GetValue(),
                                wxT(SZ_CSV_FILES),
                                wxSAVE |  wxOVERWRITE_PROMPT );
            err = dialog.ShowModal();
            wxSetWorkingDirectory(CurrentDir);

            if (wxID_OK == err)
                m_pFileName->SetValue( dialog.GetPath() );
        }

        break;
    }
}

//==========================================================================

BEGIN_EVENT_TABLE(CHexExportDlg, wxDialog)
    EVT_BUTTON(-1, CHexExportDlg::OnButton)
END_EVENT_TABLE()


//--------------------------------------------------------------------------


CHexExportDlg::CHexExportDlg(wxWindow *parent)
              :CResizableDlg( parent, "Hex export options", SZ_SECT_WND_EXP_HEXES, wxDEFAULT_DIALOG_STYLE)
{
    wxBoxSizer * topsizer;
    wxBoxSizer * sizer   ;
    wxBoxSizer * colsizer;
    wxStaticBox* box;
    CStr         ConfigKey;
    CStr         S;

    m_btnOk           = new wxButton     (this, wxID_OK    , wxT("Ok")    );
    m_btnCancel       = new wxButton     (this, wxID_CANCEL, wxT("Cancel") );
    m_btnBrowse       = new wxButton     (this, -1,          wxT("Browse") );
    m_tcFName         = new wxTextCtrl   (this, -1, wxT(""), wxDefaultPosition, wxSize(150, -1));



    m_rbHexNew        = new wxRadioButton(this, -1, wxT("New")     , wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_rbHexCurrent    = new wxRadioButton(this, -1, wxT("Current") , wxDefaultPosition, wxDefaultSize, 0);
    m_rbHexSelected   = new wxRadioButton(this, -1, wxT("Selected"), wxDefaultPosition, wxDefaultSize, 0);
    m_rbHexAll        = new wxRadioButton(this, -1, wxT("All")     , wxDefaultPosition, wxDefaultSize, 0);

    m_rbFileOverwrite = new wxRadioButton(this, -1, wxT("Overwrite it") ,wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_rbFileAppend    = new wxRadioButton(this, -1, wxT("Append to it") , wxDefaultPosition, wxDefaultSize, 0);

    m_chbInclStructs  = new wxCheckBox   (this, -1, wxT("Structures"));
    m_chbInclUnits    = new wxCheckBox   (this, -1, wxT("Units"));
    m_chbInclResources= new wxCheckBox   (this, -1, wxT("Resources"));
    m_chbInclTurnNoAcl= new wxCheckBox   (this, -1, wxT("Turn No a la Atlaclient"));


    topsizer = new wxBoxSizer( wxVERTICAL );

    sizer     = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add(new wxStaticText(this, -1, wxT("Export to file")), 0, wxALIGN_LEFT | wxALL, 5);
    sizer->Add(m_tcFName  , 0, wxALIGN_CENTER | wxALL, 5);
    sizer->Add(m_btnBrowse, 0, wxALIGN_CENTER | wxALL, 5);
    topsizer->Add(sizer, 0, wxALIGN_CENTER | wxALL, 10);



    sizer    = new wxBoxSizer( wxHORIZONTAL );

    box      = new wxStaticBox(this, -1, wxT("If file exists:"));
    colsizer = new wxStaticBoxSizer(box, wxVERTICAL );
    colsizer->Add(m_rbFileOverwrite , 0, wxALIGN_LEFT | wxALL, 2);
    colsizer->Add(m_rbFileAppend    , 0, wxALIGN_LEFT | wxALL, 2);
    sizer->Add(colsizer, 0, wxALIGN_LEFT | wxALL, 5 );

    box      = new wxStaticBox(this, -1, wxT("Hex(es):"));
    colsizer = new wxStaticBoxSizer(box, wxVERTICAL );
    colsizer->Add(m_rbHexNew     , 0, wxALIGN_LEFT | wxALL, 2);
    colsizer->Add(m_rbHexCurrent , 0, wxALIGN_LEFT | wxALL, 2);
    colsizer->Add(m_rbHexSelected, 0, wxALIGN_LEFT | wxALL, 2);
    colsizer->Add(m_rbHexAll     , 0, wxALIGN_LEFT | wxALL, 2);
    sizer->Add(colsizer, 0, wxALIGN_LEFT | wxALL, 5 );

    box      = new wxStaticBox(this, -1, wxT("Include:"));
    colsizer = new wxStaticBoxSizer(box, wxVERTICAL );
    colsizer->Add(m_chbInclStructs  , 0, wxALIGN_LEFT | wxALL, 2);
    colsizer->Add(m_chbInclUnits    , 0, wxALIGN_LEFT | wxALL, 2);
    colsizer->Add(m_chbInclResources, 0, wxALIGN_LEFT | wxALL, 2);
    colsizer->Add(m_chbInclTurnNoAcl, 0, wxALIGN_LEFT | wxALL, 2);
    sizer->Add(colsizer, 0, wxALIGN_LEFT | wxALL, 5 );

    topsizer->Add(sizer, 0, wxALIGN_CENTER );



    sizer    = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add(m_btnOk     , 0, wxALIGN_CENTER | wxALL, 5);
    sizer->Add(m_btnCancel , 0, wxALIGN_CENTER | wxALL, 5);
    topsizer->Add(sizer, 0, wxALIGN_CENTER | wxALL, 10 );




    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    m_tcFName->SetFocus();
    m_btnOk->SetDefault();


    CResizableDlg::SetSize();
}


//--------------------------------------------------------------------------

void CHexExportDlg::OnButton(wxCommandEvent& event)
{
    wxObject * object = event.GetEventObject();

    if (object == m_btnBrowse)
    {
        int          err;
        wxString     CurrentDir = wxGetCwd();
        wxFileDialog dialog(GetParent(),
                            wxT("Export hexes to"),
                            wxT(""),
                            m_tcFName->GetValue(),
                            wxT(SZ_ALL_FILES),
                            wxSAVE  );
        err = dialog.ShowModal();
        wxSetWorkingDirectory(CurrentDir);

        if (wxID_OK == err)
            m_tcFName->SetValue( dialog.GetPath() );
    }
    else if (object==m_btnOk)
    {
        StoreSize();
        EndModal(wxID_OK);
    }
    else if (object==m_btnCancel)
    {
        StoreSize();
        EndModal(wxID_CANCEL);
    }
}

//==========================================================================



BEGIN_EVENT_TABLE(CComboboxDlg, wxDialog)
    EVT_BUTTON(-1, CComboboxDlg::OnButton)
END_EVENT_TABLE()

//--------------------------------------------------------------------------

CComboboxDlg::CComboboxDlg(wxWindow *parent, const char * szTitle, const char * szMessage, const char * szChoices)
             :wxDialog(parent, -1, wxString::FromAscii(szTitle), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
    wxBoxSizer * topsizer;
    wxBoxSizer * sizer   ;
    CStr         S;
    wxButton   * btnOk;
    wxButton   * btnCancel;

    btnOk       = new wxButton     (this, wxID_OK    , wxT("Ok")    );
    btnCancel   = new wxButton     (this, wxID_CANCEL, wxT("Cancel") );
    m_cbChoices = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN);


    topsizer = new wxBoxSizer( wxVERTICAL );

    topsizer->Add(new wxStaticText(this, -1, wxString::FromAscii(szMessage)), 0, wxALIGN_LEFT | wxALL, 5);
    topsizer->Add(m_cbChoices                          , 0, wxALIGN_CENTER | wxALL | wxGROW, 5);

    sizer     = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add(btnOk    , 0, wxALIGN_CENTER | wxALL, 5);
    sizer->Add(btnCancel, 0, wxALIGN_CENTER | wxALL, 5);
    topsizer->Add(sizer,   0, wxALIGN_CENTER | wxALL, 10);

    while (szChoices && *szChoices)
    {
        szChoices = S.GetToken(szChoices, ',');
        m_cbChoices->Append(wxString::FromAscii(S.GetData()));
    }

    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    m_cbChoices->SetFocus();
    btnOk->SetDefault();

    Centre( wxBOTH | wxCENTER_FRAME);
}


//--------------------------------------------------------------------------

void CComboboxDlg::OnButton(wxCommandEvent& event)
{
    m_Choice = m_cbChoices->GetValue().mb_str();
    event.Skip();
}

//==========================================================================


BEGIN_EVENT_TABLE(CGetTextDlg, CResizableDlg)
    EVT_BUTTON(-1, CGetTextDlg::OnButton)
END_EVENT_TABLE()

//--------------------------------------------------------------------------

CGetTextDlg::CGetTextDlg(wxWindow *parent, const char * szTitle, const char * szMessage)
            :CResizableDlg( parent, szTitle, SZ_SECT_WND_GET_TEXT_DLG)
{
    wxBoxSizer * topsizer;
    wxBoxSizer * sizer   ;
    CStr         S;
    wxButton   * btnOk;
    wxButton   * btnCancel;

    btnOk       = new wxButton     (this, wxID_OK    , wxT("Ok")    );
    btnCancel   = new wxButton     (this, wxID_CANCEL, wxT("Cancel") );
    m_tcText    = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize(60,60), wxTE_MULTILINE | wxHSCROLL );


    topsizer = new wxBoxSizer( wxVERTICAL );

    topsizer->Add(new wxStaticText(this, -1, wxString::FromAscii(szMessage)), 0, wxALIGN_LEFT | wxALL, 5);
    topsizer->Add(m_tcText                             , 1, wxALIGN_CENTER | wxALL | wxGROW, 5);

    sizer     = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add(btnOk    , 0, wxALIGN_CENTER | wxALL, 5);
    sizer->Add(btnCancel, 0, wxALIGN_CENTER | wxALL, 5);
    topsizer->Add(sizer,   0, wxALIGN_CENTER | wxALL, 10);


    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

//    btnOk->SetDefault();

    CResizableDlg::SetSize();
    m_tcText->SetFocus();
}


//--------------------------------------------------------------------------

void CGetTextDlg::OnButton(wxCommandEvent& event)
{
    m_Text = m_tcText->GetValue().mb_str();
    StoreSize();
    event.Skip();
}

//==========================================================================

class CMessageBoxSwitchableDlg : public wxDialog
{
public:
    CMessageBoxSwitchableDlg(wxWindow *parent, const char * szTitle, const char * szMessage);

    wxCheckBox    * m_chbSwitchOff;
};


CMessageBoxSwitchableDlg::CMessageBoxSwitchableDlg(wxWindow *parent, const char * szTitle, const char * szMessage)
                         :wxDialog(parent, -1, wxString::FromAscii(szTitle), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
    wxBoxSizer * topsizer;
    CStr         S;
    wxButton   * btnOk;

    btnOk          = new wxButton     (this, wxID_OK    , wxT("Ok")    );
    m_chbSwitchOff = new wxCheckBox   (this, -1, wxT("Do not show this again"));

    topsizer = new wxBoxSizer( wxVERTICAL );

    topsizer->Add(new wxStaticText(this, -1, wxString::FromAscii(szMessage)), 0, wxALIGN_LEFT | wxALL, 5);
    topsizer->Add(m_chbSwitchOff                       , 0, wxALIGN_LEFT | wxALL | wxGROW, 5);
    topsizer->Add(btnOk, 0, wxALIGN_CENTER | wxALL, 10);

    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    btnOk->SetDefault();

    Centre( wxBOTH | wxCENTER_FRAME);
}

//--------------------------------------------------------------------------

void ShowMessageBoxSwitchable(const char * szTitle, const char * szMessage, const char * szConfigKey)
{
    CStr S;
    S = gpApp->GetConfig(SZ_SECT_DO_NOT_SHOW_THESE, szConfigKey);
    if (atol(S.GetData()) > 0)
        return;

    CMessageBoxSwitchableDlg dlg(NULL, szTitle, szMessage);
    dlg.ShowModal();
    if (dlg.m_chbSwitchOff->IsChecked())
        gpApp->SetConfig(SZ_SECT_DO_NOT_SHOW_THESE, szConfigKey, "1");
}

//==========================================================================

