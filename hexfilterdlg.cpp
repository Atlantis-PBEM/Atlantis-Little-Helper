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
#include "hexfilterdlg.h"
#include "mappane.h"



#define SPACER_GENERIC 5

#define ID_BTN_TRACK     wxID_HIGHEST + 10
#define ID_CB_SET_NAME   wxID_HIGHEST + 11
#define ID_BTN_HELP      wxID_HIGHEST + 12
#define ID_TC_TEXT       wxID_HIGHEST + 13

const char * HEX_FILTER_OPERATION[] = {">", ">=", "=", "<=", "<", "<>"};


BEGIN_EVENT_TABLE(CHexFilterDlg, wxDialog)
    EVT_BUTTON      (-1,             CHexFilterDlg::OnButton)
    EVT_RADIOBUTTON (-1,             CHexFilterDlg::OnRadioButton)
    EVT_COMBOBOX    (-1,             CHexFilterDlg::OnSelectChange)
    EVT_TEXT        (-1,             CHexFilterDlg::OnTextChange)
END_EVENT_TABLE()

//--------------------------------------------------------------------------

CHexFilterDlg::CHexFilterDlg(wxWindow *parent, const char * szConfigSection)
               :CResizableDlg( parent, "Find Hexes", SZ_SECT_WND_HEX_FLTR_DLG),
                m_bReady(0)
{
//    CMapPane   * pMapPane   = (CMapPane  * )gpApp->m_Panes[AH_PANE_MAP];

    wxBoxSizer * topsizer;
    wxBoxSizer * sizer   ;
    wxBoxSizer * rowsizer;
    wxFlexGridSizer * gridsizer;
    int          count;
    CStr         sConfSet;
    wxStaticText * stSetName;

    m_IsSaving           = FALSE;
    m_lastselect         = 0;
    m_sControllingConfig = szConfigSection;


    topsizer = new wxBoxSizer( wxVERTICAL );

    m_btnSet        = new wxButton     (this, wxID_OK     , "Set"    );
    m_btnRemove     = new wxButton     (this, wxID_NO     , "Clear"  );
    m_btnCancel     = new wxButton     (this, wxID_CANCEL , "Cancel" );
//    m_btnTracking   = new wxButton     (this, ID_BTN_TRACK, "Tracking" );
    m_btnHelp       = new wxButton     (this, ID_BTN_HELP , "Help" );
    m_cbSetName     = new wxComboBox   (this, ID_CB_SET_NAME);
    m_rbUseBoxes    = new wxRadioButton(this, -1, "Boxes");
    m_rbUsePython   = new wxRadioButton(this, -1, "Python" );
    m_rbUsePython->Enable(false);
    m_tcFilterText  = new wxTextCtrl   (this, ID_TC_TEXT, "", wxDefaultPosition, wxSize(100,50), wxTE_MULTILINE | wxTE_AUTO_SCROLL );
//    m_chDisplayOnMap= new wxCheckBox(this, -1, "Mark results on the map");
//    m_chUseSelectedHexes = new wxCheckBox(this, -1, "In the selected hexes only");
    stSetName       = new wxStaticText (this, -1, "Filter name:");

    //if (!pMapPane->HaveSelection())
    //    m_chUseSelectedHexes->Enable(FALSE);

    sizer    = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add(stSetName       , 0, wxALIGN_CENTER | wxALL , SPACER_GENERIC);
    sizer->Add(m_cbSetName     , 1, wxALIGN_CENTER | wxALL , SPACER_GENERIC);
    topsizer->Add(sizer        , 0, wxALIGN_CENTER | wxALL | wxGROW, SPACER_GENERIC );


    gridsizer = new wxFlexGridSizer(2,2,3,3) ;
    gridsizer->AddGrowableCol(1);
    gridsizer->AddGrowableRow(1);

    rowsizer = new wxBoxSizer( wxVERTICAL );
    gridsizer->Add(m_rbUseBoxes, 0, wxALIGN_LEFT | wxALL, SPACER_GENERIC);

    for (count=0; count < HEX_SIMPLE_FLTR_COUNT; count++)
    {
        //wxSize WiderSize(wxDefaultSize.GetWidth()*2, wxDefaultSize.GetHeight());
        m_cbProperty[count] = new wxComboBox(this, -1); //, "", wxDefaultPosition,  WiderSize);
        m_cbCompare [count] = new wxComboBox(this, -1); //, "", wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN | wxCB_READONLY );
        m_tcValue   [count] = new wxTextCtrl(this, -1);

        if (count>0)
            rowsizer->Add(new wxStaticText(this, -1, "AND"), 0, wxALIGN_LEFT | wxLEFT, SPACER_GENERIC );

        sizer    = new wxBoxSizer( wxHORIZONTAL );
        sizer->Add(m_cbProperty[count], 2, wxALIGN_CENTER | wxALL | wxGROW, SPACER_GENERIC);
        sizer->Add(m_cbCompare [count], 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC);
        sizer->Add(m_tcValue   [count], 3, wxALIGN_CENTER | wxALL | wxGROW, SPACER_GENERIC);
        rowsizer->Add(sizer, 1, wxALIGN_CENTER | wxGROW );
    }
    gridsizer->Add(rowsizer, 1, wxALIGN_LEFT | wxGROW );


    gridsizer->Add(m_rbUsePython   , 0, wxALIGN_LEFT | wxALL, SPACER_GENERIC);
    gridsizer->Add(m_tcFilterText, 1, wxALIGN_CENTER | wxGROW | wxALL, SPACER_GENERIC);
    topsizer->Add(gridsizer, 1, wxALIGN_CENTER | wxGROW);

    //sizer    = new wxBoxSizer( wxHORIZONTAL );
    //sizer->Add(m_chDisplayOnMap, 0, wxALIGN_LEFT | wxALL, SPACER_GENERIC);
    //sizer->Add(m_chUseSelectedHexes , 0, wxALIGN_LEFT | wxALL, SPACER_GENERIC);
    //topsizer->Add(sizer, 0, wxALIGN_LEFT );

    sizer    = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add(m_btnSet     , 0, wxALIGN_CENTER);
    sizer->Add(m_btnRemove  , 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC);
//    sizer->Add(m_btnTracking, 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC);
    sizer->Add(m_btnCancel  , 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC);
    sizer->Add(m_btnHelp    , 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC);
    topsizer->Add(sizer, 0, wxALIGN_CENTER );

    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    m_cbProperty[0]->SetFocus();
    m_btnSet->SetDefault();


    m_ColorNormal   = m_tcFilterText->GetBackgroundColour() ;
    m_ColorReadOnly.Set(APPLY_COLOR_DELTA(m_ColorNormal.Red()),
                        APPLY_COLOR_DELTA(m_ColorNormal.Green()),
                        APPLY_COLOR_DELTA(m_ColorNormal.Blue()));




    sConfSet = gpApp->GetConfig(szConfigSection, SZ_KEY_FLTR_SET);
    sConfSet.TrimRight(TRIM_ALL);
    if (sConfSet.IsEmpty())
        sConfSet << SZ_SECT_HEX_FILTER << "Default";
    LoadSetCombo(sConfSet.GetData());
    Init();
    Load(sConfSet.GetData());

    CResizableDlg::SetSize();

    m_bReady = TRUE;
}

//--------------------------------------------------------------------------

void CHexFilterDlg::LoadSetCombo(const char * setselect)
{
    const char * setsection;
    int          setnameoffs;
    int          x=0, i=0;

    m_IsSaving = TRUE;

    setnameoffs = strlen(SZ_SECT_HEX_FILTER);
    setsection  = gpApp->GetNextSectionName(CONFIG_FILE_CONFIG, SZ_SECT_HEX_FILTER);
    while (setsection)
    {
        if (0!=strnicmp(setsection, SZ_SECT_HEX_FILTER, setnameoffs))
            break;
        m_cbSetName->Append(&setsection[setnameoffs]);
        if (0==stricmp(setsection, setselect))
            x = i;
        i++;
        setsection = gpApp->GetNextSectionName(CONFIG_FILE_CONFIG, setsection);
    }
    if (0==i)
        m_cbSetName->Append(&setselect[setnameoffs]);
    m_cbSetName->SetSelection(x);

    m_IsSaving = FALSE;
}

//--------------------------------------------------------------------------

void CHexFilterDlg::Init()
{
    int          count;
    int          i;
    const char * item;
    int          skilllen;
    CStr         S;

    skilllen    = strlen(PRP_SKILL_POSTFIX);
    for (count=0; count < HEX_SIMPLE_FLTR_COUNT; count++)
    {
        m_cbProperty[count]->Append("");
        m_cbCompare [count]->Append("");

        for (i=0; i<gpApp->m_pAtlantis->m_LandPropertyNames.Count(); i++)
        {
            item = (const char *) gpApp->m_pAtlantis->m_LandPropertyNames.At(i);
            m_cbProperty[count]->Append(item);
        }

        for (i=0; (unsigned)i<sizeof(HEX_FILTER_OPERATION)/sizeof(*HEX_FILTER_OPERATION); i++)
        {
            item = HEX_FILTER_OPERATION[i];
            m_cbCompare[count]->Append(item);
        }
    }
}

//--------------------------------------------------------------------------

void CHexFilterDlg::Load(const char * szConfigSection)
{
    int          count;
    int          i;
    CStr         ConfigKey;
    const char * selvalue;
    int          selidx;
    CStr         S;

    m_sCurrentSection = szConfigSection;

    for (count=0; count < HEX_SIMPLE_FLTR_COUNT; count++)
    {
        ConfigKey.Format("%s%d", SZ_KEY_HEX_FLTR_PROPERTY, count);
        selvalue = gpApp->GetConfig(szConfigSection, ConfigKey.GetData());
        selidx   = 0;
        for (i=0; i<(int)m_cbProperty[count]->GetCount(); i++)
            if (0==stricmp(m_cbProperty[count]->GetString(i), selvalue))
            {
                selidx = i;
                break;
            }
        m_cbProperty[count]->SetSelection(selidx);

        ConfigKey.Format("%s%d", SZ_KEY_HEX_FLTR_COMPARE , count);
        m_cbCompare [count]->SetValue(gpApp->GetConfig(szConfigSection, ConfigKey.GetData()));
        selvalue = gpApp->GetConfig(szConfigSection, ConfigKey.GetData());
        selidx   = 0;
        for (i=0; i<(int)m_cbCompare[count]->GetCount(); i++)
            if (0==stricmp(m_cbCompare[count]->GetString(i), selvalue))
            {
                selidx = i;
                break;
            }
        m_cbCompare[count]->SetSelection(selidx);

        ConfigKey.Format("%s%d", SZ_KEY_HEX_FLTR_VALUE   , count);
        m_tcValue   [count]->SetValue(SkipSpaces(gpApp->GetConfig(szConfigSection, ConfigKey.GetData())) );
    }

    m_tcFilterText->SetValue(gpApp->GetConfig(szConfigSection, SZ_KEY_HEX_FLTR_PYTHON_CODE));

    S = gpApp->GetConfig(szConfigSection, SZ_KEY_HEX_FLTR_SOURCE);
    if (0==stricmp(S.GetData(), SZ_KEY_HEX_FLTR_SOURCE_PYTHON))
    {
        m_rbUsePython->SetValue(TRUE);
        EnableBoxes(FALSE);
    }
    else
    {
        m_rbUseBoxes->SetValue(TRUE);
        EnableBoxes(TRUE);
    }

    //S = gpApp->GetConfig(szConfigSection, SZ_KEY_HEX_FLTR_SELECTED_HEXES);
    //m_sSavedConfigSelected = S;
    //if ( m_chUseSelectedHexes->IsEnabled() )
    //    m_chUseSelectedHexes->SetValue(atol(S.GetData()) != 0);

    //S = gpApp->GetConfig(szConfigSection, SZ_KEY_HEX_FLTR_SHOW_ON_MAP);
    //m_chDisplayOnMap->SetValue(atol(S.GetData()) != 0);
}

//--------------------------------------------------------------------------

BOOL CHexFilterDlg::IsValid()
{
    CStr         S1, S2;
    int          count;
    BOOL         isvalid = FALSE;

    if (m_rbUseBoxes->GetValue())
        for (count=0; count < HEX_SIMPLE_FLTR_COUNT; count++)
        {
            S1 = m_cbProperty[count]->GetValue();   S1.TrimRight(TRIM_ALL);
            S2 = m_cbCompare[count]->GetValue();    S2.TrimRight(TRIM_ALL);
            if (!S1.IsEmpty() && !S2.IsEmpty())
                isvalid = TRUE;
        }
    else
    {
        S1 = m_tcFilterText->GetValue();
        if (!S1.IsEmpty() )
            isvalid = TRUE;
    }
    return isvalid;
}

//--------------------------------------------------------------------------

void CHexFilterDlg::Save()
{
    int          count;
    CStr         ConfigKey;
    CStr         SetName;
    CStr         S;
    BOOL         found = FALSE;

    m_IsSaving = TRUE;

    gpApp->RemoveSection(m_sCurrentSection.GetData());

    if (IsValid())
    {
        SetName = m_sCurrentSection;
        SetName.DelSubStr(0, strlen(SZ_SECT_HEX_FILTER));

        for (count=0; count<(int)m_cbSetName->GetCount(); count++)
            if (0==stricmp(m_cbSetName->GetString(count), SetName.GetData()))
            {
                found = TRUE;
                break;
            }
        if (!found)
            m_cbSetName->Append(SetName.GetData());


        for (count=0; count < HEX_SIMPLE_FLTR_COUNT; count++)
        {
            ConfigKey.Format("%s%d", SZ_KEY_HEX_FLTR_PROPERTY, count);
            gpApp->SetConfig(m_sCurrentSection.GetData(), ConfigKey.GetData(), m_cbProperty[count]->GetValue());

            ConfigKey.Format("%s%d", SZ_KEY_HEX_FLTR_COMPARE , count);
            gpApp->SetConfig(m_sCurrentSection.GetData(), ConfigKey.GetData(), m_cbCompare[count]->GetValue());

            ConfigKey.Format("%s%d", SZ_KEY_HEX_FLTR_VALUE   , count);
            gpApp->SetConfig(m_sCurrentSection.GetData(), ConfigKey.GetData(), m_tcValue[count]->GetValue());
        }

        gpApp->SetConfig(m_sCurrentSection.GetData(), SZ_KEY_HEX_FLTR_PYTHON_CODE, m_tcFilterText->GetValue());
        gpApp->SetConfig(m_sCurrentSection.GetData(), SZ_KEY_HEX_FLTR_SOURCE, m_rbUsePython->GetValue() ? SZ_KEY_HEX_FLTR_SOURCE_PYTHON : "");

        //S = m_sSavedConfigSelected;
        //if ( m_chUseSelectedHexes->IsEnabled() )
        //    S = m_chUseSelectedHexes->GetValue()?"1":"0";
        //gpApp->SetConfig(m_sCurrentSection.GetData(), SZ_KEY_HEX_FLTR_SELECTED_HEXES, S.GetData());
        //gpApp->SetConfig(m_sCurrentSection.GetData(), SZ_KEY_HEX_FLTR_SHOW_ON_MAP, m_chDisplayOnMap->GetValue()?"1":"0");

    }

    m_IsSaving = FALSE;
}

//--------------------------------------------------------------------------

void CHexFilterDlg::Reload(const char * setname)
{
    CStr  Sect;

    if (m_IsSaving)
        return;

    Save();
    Sect << SZ_SECT_HEX_FILTER << setname;
    Load(Sect.GetData());
}

//--------------------------------------------------------------------------

void CHexFilterDlg::EnableBoxes (BOOL bOldBoxes)
{
    int i;

    m_tcFilterText->Enable(!bOldBoxes);

    for (i=0; i < HEX_SIMPLE_FLTR_COUNT; i++)
    {
        m_cbProperty[i]->Enable(bOldBoxes);
        m_cbCompare [i]->Enable(bOldBoxes);
        m_tcValue   [i]->Enable(bOldBoxes);
    }

}

//--------------------------------------------------------------------------


void CHexFilterDlg::OnTextChange   (wxCommandEvent& event)
{
    wxObject * object = event.GetEventObject();

    if (object == m_cbSetName)
    {
        if (m_lastselect < time(NULL))
            Reload(m_cbSetName->GetValue()); // returns the old value on windoze when value is selected
    }
    else
        OnBoxesChange(event);
}

//--------------------------------------------------------------------------

void CHexFilterDlg::OnSelectChange (wxCommandEvent& event)
{
    wxObject * object = event.GetEventObject();

    if (object == m_cbSetName)
    {
        Reload(m_cbSetName->GetString(m_cbSetName->GetSelection()));
        m_lastselect = time(NULL);
    }
    else
        OnBoxesChange(event);
}

//--------------------------------------------------------------------------

void CHexFilterDlg::OnBoxesChange  (wxCommandEvent& event)
{
    wxObject * object = event.GetEventObject();

    if (!m_bReady || object == m_tcFilterText)
        return;

    if (m_rbUseBoxes->GetValue())
    {
        int i;
        CStr s, s1, s2, s3;

        for (i=0; i < HEX_SIMPLE_FLTR_COUNT; i++)
        {
            s1 = m_cbProperty[i]->GetValue();
            s2 = m_cbCompare [i]->GetValue();
            s3 = m_tcValue   [i]->GetValue();
            if (!s1.IsEmpty() || !s2.IsEmpty() || !s3.IsEmpty())
            {
                // adjust 'equal to'
                s2.TrimLeft();
                s2.TrimRight();
                if (0 == strcmp(s2.GetData(), "="))
                    s2 = "==";

                // quote strings
                s1.TrimLeft();
                s1.TrimRight();

                CStrInt  * pSI, SI(s1.GetData(), 0);
                int        idx;
                EValueType type;

                if (gpApp->m_pAtlantis->m_UnitPropertyTypes.Search(&SI, idx))
                {
                    pSI = (CStrInt*)gpApp->m_pAtlantis->m_UnitPropertyTypes.At(idx);
                    type = (EValueType)pSI->m_value;
                    if (eCharPtr == type)
                    {
                        s3.InsStr("\"", 0, 1);
                        s3 << '\"';
                    }
                }

                if (!s.IsEmpty())
                    s << " and ";
                s << s1 << s2 << s3;
            }
        }
        m_tcFilterText->SetValue(s.GetData());
    }
}

//--------------------------------------------------------------------------

void CHexFilterDlg::OnRadioButton  (wxCommandEvent& event)
{
    wxObject * object = event.GetEventObject();

    EnableBoxes(object == m_rbUseBoxes);
}

//--------------------------------------------------------------------------

void CHexFilterDlg::OnButton(wxCommandEvent& event)
{
    wxObject * object = event.GetEventObject();
    int        i;

    m_TrackingGroup.Empty();
    if (object == m_btnRemove)
    {
        for (i=0; i < HEX_SIMPLE_FLTR_COUNT; i++)
        {
            m_cbProperty[i]->SetValue("");
            m_cbCompare [i]->SetValue("");
            m_tcValue   [i]->SetValue("");
        }
        m_tcFilterText->SetValue("");
    }
    //else if (object == m_btnTracking)
    //{
    //    int          sectidx;
    //    CStr         S;
    //    const char * szName;
    //    const char * szValue;

    //    sectidx = gpApp->GetSectionFirst(SZ_SECT_UNIT_TRACKING, szName, szValue);
    //    while (sectidx >= 0)
    //    {
    //        if (!S.IsEmpty())
    //            S << ",";
    //        S << szName;
    //        sectidx = gpApp->GetSectionNext(sectidx, SZ_SECT_UNIT_TRACKING, szName, szValue);
    //    }
    //    if (S.IsEmpty())
    //        S = "Default";

    //    CComboboxDlg dlg(this, "Show a tracking group", "Select a group to load units from.", S.GetData());
    //    if (wxID_OK == dlg.ShowModal())
    //    {
    //        m_TrackingGroup = dlg.m_Choice;

    //        StoreSize();
    //        EndModal(wxID_OK);
    //    }
    //}
    else if (object == m_btnSet)
    {
        Save();
        gpApp->SetConfig(m_sControllingConfig.GetData(), SZ_KEY_FLTR_SET, m_sCurrentSection.GetData());
        StoreSize();
        EndModal(wxID_OK);
    }
    else if (object == m_btnCancel)
    {
        StoreSize();
        EndModal(wxID_CANCEL);
    }
    else if (object == m_btnHelp)
    {
        wxMessageBox("\n"
                     "- Filter names can not be modified.\n"
                     "- To add a filter type in the name for it and set conditions.\n"
                     "- To delete a filter remove conditions from it."
                     );
    }
}

