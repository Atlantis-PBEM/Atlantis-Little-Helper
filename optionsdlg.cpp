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

#include "wx/fontdlg.h"
#include "wx/colordlg.h"

#include "cstr.h"
#include "collection.h"
#include "cfgfile.h"
#include "files.h"
#include "atlaparser.h"
#include "consts.h"
#include "consts_ah.h"
#include "objs.h"
#include "hash.h"

#include "ahapp.h"
#include "ahframe.h"
//#include "optionsdlg1.h"
#include "optionsdlg.h"

enum
{
    ID_D1_CHK_APPLY_DFLT_EMPTY  = wxID_HIGHEST + 10,
    ID_D1_CB_COLOR              ,
    ID_D1_CHK_RIGHT_CLCK_CENTER ,
    ID_D1_BTN_FONT              ,
    ID_D1_BTN_COLOR             ,
    ID_D1_CB_FACTION            ,
    ID_D1_BTN_CANCEL            ,
    ID_D1_STATIC_PWD            ,
    ID_D1_DLG_OPTIONS           ,
    ID_D1_CHK_LOAD_ORD          ,
    ID_D1_CHK_LOAD_REP          ,
    ID_D1_CHK_READ_PWD          ,
    ID_D1_TXT_PASSWORD          ,
    ID_D1_CHK_TEACH_LVL         ,
    ID_D1_STATIC_FACTION        ,
    ID_D1_CHK_UNIX_FILES        ,
    ID_D1_CHK_FIXED_PITCH       ,
    ID_D1_CHK_3WIN_LAYOUT       ,
    ID_D1_CHK_HATCH_UNVISITED   ,
    ID_D1_BTN_OK                ,
    ID_D1_CB_FONT               ,
    ID_D1_RADIO_1_WIN           ,
    ID_D1_RADIO_2_WIN           ,
    ID_D1_RADIO_3_WIN           ,
    ID_D1_CHK_CHK_PROD_REQ      ,
    ID_D1_CHK_CHK_MOVE_MODE     ,
    ID_D1_RADIO_ICONS_SIMPLE    ,
    ID_D1_RADIO_ICONS_ADVANCED  ,
};

BEGIN_EVENT_TABLE(COptionsDialog, wxDialog)
    EVT_BUTTON  (wxID_OK      , COptionsDialog::OnOk)
    EVT_BUTTON  (wxID_CANCEL  , COptionsDialog::OnCancel)
    EVT_BUTTON  (ID_D1_BTN_FONT    , COptionsDialog::OnFont)
    EVT_BUTTON  (ID_D1_BTN_COLOR   , COptionsDialog::OnColor)
    EVT_COMBOBOX(ID_D1_CB_FACTION  , COptionsDialog::OnFaction)
    EVT_TEXT    (ID_D1_TXT_PASSWORD, COptionsDialog::OnPassword)
END_EVENT_TABLE()


//--------------------------------------------------------------------------

COptionsDialog::COptionsDialog(wxWindow * parent)
               :CResizableDlg( parent, "Edit options", SZ_SECT_WND_OPTIONS_DLG)//, wxDEFAULT_DIALOG_STYLE)
{
    wxBoxSizer   * topsizer;
    wxBoxSizer   * rowsizer;
    wxBoxSizer   * colsizer;
    wxBoxSizer   * layoutsizer;
    wxButton     * btnFont  ;
    wxButton     * btnColor ;
    wxButton     * btnOk    ;
    wxButton     * btnCancel;
    wxStaticText * stPwd;
    wxStaticText * stFaction;
    wxStaticText * stLayout;
    wxStaticText * stIcons;

    m_IsValid = false;

    m_pComboFonts               = new wxComboBox(this, ID_D1_CB_FONT );
    m_pComboColors              = new wxComboBox(this, ID_D1_CB_COLOR);
    m_pComboFactions            = new wxComboBox(this, ID_D1_CB_FACTION);

    m_pChkLoadOrd               = new wxCheckBox(this, ID_D1_CHK_LOAD_ORD         , wxT("Load Orders"));
    m_pChkLoadRep               = new wxCheckBox(this, ID_D1_CHK_LOAD_REP         , wxT("Load Report"));
    m_pChkUnixStyle             = new wxCheckBox(this, ID_D1_CHK_UNIX_FILES       , wxT("Unix file format"));
    m_pChkHatchUnvisited        = new wxCheckBox(this, ID_D1_CHK_HATCH_UNVISITED  , wxT("Hatch unvisited"));
    m_pChkRClickCenters         = new wxCheckBox(this, ID_D1_CHK_RIGHT_CLCK_CENTER, wxT("Right Click Centers"));
//    m_pChk3WinLayout            = new wxCheckBox(this, ID_D1_CHK_3WIN_LAYOUT      , wxT("3 windows layout"));
    m_pChkTeach                 = new wxCheckBox(this, ID_D1_CHK_TEACH_LVL        , wxT("Check TEACH level"));
    m_pChkReadPwd               = new wxCheckBox(this, ID_D1_CHK_READ_PWD         , wxT("Import Passwords"));
    m_pChkCheckProdReq          = new wxCheckBox(this, ID_D1_CHK_CHK_PROD_REQ     , wxT("Immediate production check"));
    m_pChkMoveMode              = new wxCheckBox(this, ID_D1_CHK_CHK_MOVE_MODE    , wxT("Check movement modes"));

    m_pTxtPassword              = new wxTextCtrl(this, ID_D1_TXT_PASSWORD);

    m_pRadio1Win                = new wxRadioButton(this, ID_D1_RADIO_1_WIN, wxT("1 window"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_pRadio2Win                = new wxRadioButton(this, ID_D1_RADIO_2_WIN, wxT("2 windows"));
    m_pRadio3Win                = new wxRadioButton(this, ID_D1_RADIO_3_WIN, wxT("3 windows"));

    m_pRadioIconsSimple         = new wxRadioButton(this, ID_D1_RADIO_ICONS_SIMPLE  , wxT("Simple"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_pRadioIconsAdvanced       = new wxRadioButton(this, ID_D1_RADIO_ICONS_ADVANCED, wxT("Advanced"));


    btnFont                     = new wxButton     (this, ID_D1_BTN_FONT     , wxT("Font") );
    btnColor                    = new wxButton     (this, ID_D1_BTN_COLOR    , wxT("Color") );
    btnOk                       = new wxButton     (this, wxID_OK            , wxT("Ok")    );
    btnCancel                   = new wxButton     (this, wxID_CANCEL        , wxT("Cancel") );

    stPwd                       = new wxStaticText (this, -1                 , wxT("Password"));
    stFaction                   = new wxStaticText (this, -1                 , wxT("Faction"));
    stLayout                    = new wxStaticText (this, -1                 , wxT("Layout"));
    stIcons                     = new wxStaticText (this, -1                 , wxT("Icons"));


#define space 2

    topsizer = new wxBoxSizer( wxVERTICAL );

    rowsizer    = new wxBoxSizer( wxHORIZONTAL );
        rowsizer->Add(m_pComboFonts   , 1, wxALIGN_CENTER | wxALL , space);
        rowsizer->Add(btnFont         , 0, wxALIGN_CENTER | wxALL , space);
    topsizer->Add(rowsizer        , 0, wxALIGN_CENTER | wxALL | wxGROW, space );

    rowsizer    = new wxBoxSizer( wxHORIZONTAL );
        rowsizer->Add(m_pComboColors   , 1, wxALIGN_CENTER | wxALL , space);
        rowsizer->Add(btnColor         , 0, wxALIGN_CENTER | wxALL , space);
    topsizer->Add(rowsizer         , 0, wxALIGN_CENTER | wxALL | wxGROW, space );

    rowsizer    = new wxBoxSizer( wxHORIZONTAL );

        rowsizer->Add(stFaction        , 0, wxALIGN_CENTER | wxALL , space);
        rowsizer->Add(m_pComboFactions , 1, wxALIGN_CENTER | wxALL , space);
        rowsizer->Add(stPwd            , 0, wxALIGN_CENTER | wxALL , space);
        rowsizer->Add(m_pTxtPassword   , 1, wxALIGN_CENTER | wxALL , space);
    topsizer->Add(rowsizer         , 0, wxALIGN_CENTER | wxALL | wxGROW, space );


    rowsizer    = new wxBoxSizer( wxHORIZONTAL );

        colsizer = new wxBoxSizer( wxVERTICAL );
            colsizer->Add(m_pChkLoadOrd            , 0, wxALIGN_LEFT | wxALL, space);
            colsizer->Add(m_pChkLoadRep            , 0, wxALIGN_LEFT | wxALL, space);
            colsizer->Add(m_pChkUnixStyle          , 0, wxALIGN_LEFT | wxALL, space);
            colsizer->Add(m_pChkHatchUnvisited     , 0, wxALIGN_LEFT | wxALL, space);
        rowsizer->Add(colsizer         , 0, wxALIGN_CENTER | wxALL | wxGROW, space );

        colsizer = new wxBoxSizer( wxVERTICAL );
            colsizer->Add(m_pChkRClickCenters      , 0, wxALIGN_LEFT | wxALL, space);
            //colsizer->Add(m_pChk3WinLayout         , 0, wxALIGN_LEFT | wxALL, space);
            colsizer->Add(m_pChkTeach              , 0, wxALIGN_LEFT | wxALL, space);
            colsizer->Add(m_pChkReadPwd            , 0, wxALIGN_LEFT | wxALL, space);
            colsizer->Add(m_pChkCheckProdReq       , 0, wxALIGN_LEFT | wxALL, space);
            colsizer->Add(m_pChkMoveMode           , 0, wxALIGN_LEFT | wxALL, space);

        rowsizer->Add(colsizer         , 0, wxALIGN_CENTER | wxALL | wxGROW, space );

        colsizer = new wxBoxSizer( wxVERTICAL );
            colsizer->Add(stLayout         , 0, wxALIGN_CENTER | wxALL | wxGROW, space );
            layoutsizer = new wxBoxSizer( wxVERTICAL );
                layoutsizer->Add(m_pRadio1Win       , 0, wxALIGN_LEFT | wxALL, space);
                layoutsizer->Add(m_pRadio2Win       , 0, wxALIGN_LEFT | wxALL, space);
                layoutsizer->Add(m_pRadio3Win       , 0, wxALIGN_LEFT | wxALL, space);
            colsizer->Add(layoutsizer      , 0, wxALIGN_CENTER | wxALL | wxGROW, space );

            colsizer->Add(stIcons         , 0, wxALIGN_CENTER | wxTOP | wxGROW, space+space );
            layoutsizer = new wxBoxSizer( wxVERTICAL );
                layoutsizer->Add(m_pRadioIconsSimple       , 0, wxALIGN_LEFT | wxALL, space);
                layoutsizer->Add(m_pRadioIconsAdvanced     , 0, wxALIGN_LEFT | wxALL, space);
            colsizer->Add(layoutsizer      , 0, wxALIGN_CENTER | wxALL | wxGROW, space );

        rowsizer->Add(colsizer         , 0, wxALIGN_CENTER | wxALL | wxGROW, space );

    topsizer->Add(rowsizer         , 1, wxALIGN_CENTER | wxALL | wxGROW, space );


    rowsizer = new wxBoxSizer( wxHORIZONTAL );
        rowsizer->Add(btnOk     , 0, wxALIGN_CENTER | wxALL, space);
        rowsizer->Add(btnCancel , 0, wxALIGN_CENTER | wxALL, space);
    topsizer->Add(rowsizer  , 0, wxALIGN_CENTER | wxALL, space );




    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}



    CResizableDlg::SetSize();
}

//--------------------------------------------------------------------------

void COptionsDialog::Init()
{
    long         i;
    const char * szName;
    const char * szValue;
    CStrStr    * pSS;

    m_IsValid = true;

    m_pComboFonts   ->Clear();
    m_pComboColors  ->Clear();
    m_pComboFactions->Clear();

    for(i=0; i<FONT_COUNT; i++)
    {
        m_bFontChanged[i]      = FALSE;

        m_FontData[i].size     = gpApp->m_Fonts[i]->GetPointSize();
        m_FontData[i].family   = gpApp->m_Fonts[i]->GetFamily()   ;
        m_FontData[i].style    = gpApp->m_Fonts[i]->GetStyle()    ;
        m_FontData[i].weight   = gpApp->m_Fonts[i]->GetWeight()   ;
        m_FontData[i].encoding = gpApp->m_Fonts[i]->GetEncoding() ;
        m_FontData[i].face     = gpApp->m_Fonts[i]->GetFaceName() ;

        m_pComboFonts->Append(wxString::FromAscii(gpApp->m_FontDescr[i]), (void*)i);
//        m_pComboFonts->Append(gpApp->m_FontDescr[i]);
//        m_pComboFonts->SetClientData(m_pComboFonts->GetCount()-1, (void*)i);
    }

    i = gpApp->GetSectionFirst(SZ_SECT_COLORS, szName, szValue);
    while (i>=0)
    {
        m_pComboColors->Append(wxString::FromAscii(szName));
        pSS = new CStrStr(szName, szValue);
        m_ColorData.Insert(pSS);
        i = gpApp->GetSectionNext(i, SZ_SECT_COLORS, szName, szValue);
    }

    i = gpApp->GetSectionFirst(SZ_SECT_PASSWORDS, szName, szValue);
    while (i>=0)
    {
        m_pComboFactions->Append(wxString::FromAscii(szName));
        pSS = new CStrStr(szName, szValue);
        m_FactionData.Insert(pSS);
        i = gpApp->GetSectionNext(i, SZ_SECT_PASSWORDS, szName, szValue);
    }


    m_pChkLoadOrd          ->SetValue(0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_LOAD_ORDER)));
    m_pChkLoadRep          ->SetValue(0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_LOAD_REP)));
    m_pChkUnixStyle        ->SetValue(0!=stricmp(SZ_EOL_MS, gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_EOL)));
    m_pChkHatchUnvisited   ->SetValue(0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_HATCH_UNVISITED)));
    m_pChkRClickCenters    ->SetValue(0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_RCLICK_CENTERS)));
//    m_pChk3WinLayout       ->SetValue(0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_LAYOUT)));
    m_pChkTeach            ->SetValue(0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_CHECK_TEACH_LVL)));
    m_pChkReadPwd          ->SetValue(0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_PWD_READ)));
    m_pChkCheckProdReq     ->SetValue(0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_CHK_PROD_REQ)));
    m_pChkMoveMode         ->SetValue(0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_CHECK_MOVE_MODE)));

    switch(atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_LAYOUT)))
    {
      case AH_LAYOUT_1_WIN:   m_pRadio1Win->SetValue(TRUE);   break;
      case AH_LAYOUT_2_WIN:   m_pRadio2Win->SetValue(TRUE);   break;
      default             :   m_pRadio3Win->SetValue(TRUE);   break;
    }

    if (0==stricmp(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_ICONS), SZ_ICONS_ADVANCED))
        m_pRadioIconsAdvanced->SetValue(TRUE);
    else
        m_pRadioIconsSimple->SetValue(TRUE);


    m_pComboFonts   ->SetSelection(0);
    m_pComboColors  ->SetSelection(0);
    m_pComboFactions->SetSelection(0);

    wxCommandEvent event;
    OnFaction(event);
}




//--------------------------------------------------------------------------

void COptionsDialog::Done()
{
    m_ColorData.FreeAll();
    m_FactionData.FreeAll();
}

//--------------------------------------------------------------------------

void COptionsDialog::OnOk    (wxCommandEvent& event)
{
    BOOL DoApplyColors;
    int layout;

    if (m_IsValid && event.GetId()==wxID_OK)
    {
        m_IsValid = FALSE;
        DoApplyColors = ((m_pChkHatchUnvisited->GetValue()?1:0)!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_HATCH_UNVISITED)));


        if (m_pRadio1Win->GetValue())
            layout = AH_LAYOUT_1_WIN;
        else if (m_pRadio2Win->GetValue())
            layout = AH_LAYOUT_2_WIN;
        else
            layout = AH_LAYOUT_3_WIN;

//        if (m_pChk3WinLayout->GetValue() != (0!=atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_LAYOUT))))
        if (layout != atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_LAYOUT)))
            wxMessageBox(wxT("Please restart application for the changes to take effect"),
                         wxT("Warning"), wxOK | wxCENTRE | wxICON_EXCLAMATION);


        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_LOAD_ORDER        , m_pChkLoadOrd          ->GetValue()?"1":"0");
        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_LOAD_REP          , m_pChkLoadRep          ->GetValue()?"1":"0");
        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_HATCH_UNVISITED   , m_pChkHatchUnvisited   ->GetValue()?"1":"0");
        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_RCLICK_CENTERS    , m_pChkRClickCenters    ->GetValue()?"1":"0");
//        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_LAYOUT            , m_pChk3WinLayout       ->GetValue()?"1":"0");
        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_LAYOUT            , layout);
        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_CHECK_TEACH_LVL   , m_pChkTeach            ->GetValue()?"1":"0");
        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_PWD_READ          , m_pChkReadPwd          ->GetValue()?"1":"0");
        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_CHK_PROD_REQ      , m_pChkCheckProdReq     ->GetValue()?"1":"0");
        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_CHECK_MOVE_MODE   , m_pChkMoveMode         ->GetValue()?"1":"0");


        if (m_pChkUnixStyle->GetValue())
        {
            gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_EOL, SZ_EOL_UNIX);
            EOL_FILE = EOL_UNIX;
        }
        else
        {
            gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_EOL, SZ_EOL_MS);
            EOL_FILE = EOL_MS;
        }

        gpApp->SetConfig(SZ_SECT_COMMON, SZ_KEY_ICONS, m_pRadioIconsAdvanced->GetValue() ? SZ_ICONS_ADVANCED : SZ_ICONS_SIMPLE);
        



        if (DoApplyColors)
            gpApp->ApplyColors();
        gpApp->ApplyIcons();
        gpApp->Redraw();
    }
    StoreSize();
    EndModal(wxID_OK);
}

//--------------------------------------------------------------------------

void COptionsDialog::OnCancel(wxCommandEvent& event)
{
    int          i;
    CStrStr    * pSS;

    if (m_IsValid && event.GetId()==wxID_CANCEL)
    {
        m_IsValid = FALSE;
        for(i=0; i<FONT_COUNT; i++)
        {
            if (m_bFontChanged[i])
            {
/*              gpApp->m_Fonts[i]->SetPointSize(m_FontData[i].size    ) ;
                gpApp->m_Fonts[i]->SetFamily   (m_FontData[i].family  ) ;
                gpApp->m_Fonts[i]->SetStyle    (m_FontData[i].style   ) ;
                gpApp->m_Fonts[i]->SetWeight   (m_FontData[i].weight  ) ;
                gpApp->m_Fonts[i]->SetEncoding (m_FontData[i].encoding) ;
                gpApp->m_Fonts[i]->SetFaceName (m_FontData[i].face    ) ;*/
                delete gpApp->m_Fonts[i];
                gpApp->m_Fonts[i] = new wxFont(m_FontData[i].size,
                                            m_FontData[i].family,
                                            m_FontData[i].style,
                                            m_FontData[i].weight,
                                            FALSE,
                                            m_FontData[i].face,
                                            m_FontData[i].encoding   );
//            font = new wxFont(size, family, style, weight, FALSE, facename, (wxFontEncoding)encoding);

            }
        }

        for (i=0; i<m_ColorData.Count(); i++)
        {
            pSS = (CStrStr*)m_ColorData.At(i);
            gpApp->SetConfig(SZ_SECT_COLORS, pSS->m_key, pSS->m_value);
        }

        for (i=0; i<m_FactionData.Count(); i++)
        {
            pSS = (CStrStr*)m_FactionData.At(i);
            gpApp->SetConfig(SZ_SECT_PASSWORDS, pSS->m_key, pSS->m_value);
        }

        gpApp->ApplyFonts();
        gpApp->ApplyColors();
    }

    StoreSize();
    EndModal(wxID_CANCEL);
}

//--------------------------------------------------------------------------

void COptionsDialog::OnFont    (wxCommandEvent& event)
{
    long fontidx;

    if (!m_IsValid)
        return;

    fontidx = (long)m_pComboFonts->GetClientData(m_pComboFonts->GetSelection());
    if (fontidx<0 || fontidx>=FONT_COUNT)
        return;

    wxFontData data;
    data.SetInitialFont(*gpApp->m_Fonts[fontidx]);
    data.EnableEffects(false);

    wxFontDialog *dialog = new wxFontDialog(this, data);
    wxString      title  = wxString::FromAscii(gpApp->m_FontDescr[fontidx]);
    dialog->SetTitle(title);
    if (dialog->ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog->GetFontData();
        wxFont     retFont = retData.GetChosenFont();

        gpApp->m_Fonts[fontidx]->SetPointSize(retFont.GetPointSize());
        gpApp->m_Fonts[fontidx]->SetFamily   (retFont.GetFamily   ());
        gpApp->m_Fonts[fontidx]->SetStyle    (retFont.GetStyle    ());
        gpApp->m_Fonts[fontidx]->SetWeight   (retFont.GetWeight   ());
        gpApp->m_Fonts[fontidx]->SetEncoding (retFont.GetEncoding ());
        gpApp->m_Fonts[fontidx]->SetFaceName (retFont.GetFaceName ());

        m_bFontChanged[fontidx] = TRUE;
//        delete gpApp->m_Fonts[fontidx];
//        gpApp->m_Fonts[fontidx] = new wxFont(retFont);



        gpApp->ApplyFonts();
    }
    dialog->Destroy();
}

//--------------------------------------------------------------------------

void COptionsDialog::OnColor    (wxCommandEvent& event)
{
    wxColour     colour;
    wxColourData data;
    wxString     colname;
    char         newcolour[128];

    if (!m_IsValid)
        return;

    colname = m_pComboColors->GetStringSelection();
    StrToColor(&colour, gpApp->GetConfig(SZ_SECT_COLORS, colname.mb_str()));

    data.SetChooseFull(TRUE);
    data.SetCustomColour(0,colour);
    data.SetColour(colour);

    wxColourDialog *dialog = new wxColourDialog(this, &data);
    dialog->SetTitle(colname);
    if (dialog->ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog->GetColourData();
        wxColour col = retData.GetColour();

        ColorToStr(newcolour, &col);
        gpApp->SetConfig(SZ_SECT_COLORS, colname.mb_str(), newcolour);
        gpApp->ApplyColors();
    }
    dialog->Destroy();
}

//--------------------------------------------------------------------------

void COptionsDialog::OnFaction(wxCommandEvent& event)
{
    m_pTxtPassword->SetValue( wxString::FromAscii(gpApp->GetConfig(SZ_SECT_PASSWORDS, m_pComboFactions->GetStringSelection().mb_str())));
}

//--------------------------------------------------------------------------

void COptionsDialog::OnPassword(wxCommandEvent& event)
{
    gpApp->SetConfig(SZ_SECT_PASSWORDS, m_pComboFactions->GetStringSelection().mb_str(), m_pTxtPassword->GetValue().mb_str());
}

//--------------------------------------------------------------------------

