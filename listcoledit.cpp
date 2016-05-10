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
#include "listcoledit.h"
#include "listpane.h"

//==========================================================================


enum
{
  ID_LIST_SRC   =    wxID_HIGHEST + 10,
  ID_LIST_DEST   ,
  ID_BTN_ADD     ,
  ID_BTN_DEL     ,
  ID_BTN_DEL_ALL ,
  ID_BTN_HELP    ,
  ID_CB_SET_NAME ,
  ID_TXT_CAPTION
};



BEGIN_EVENT_TABLE(CListHeaderEditDlg, wxDialog)
    EVT_BUTTON             (-1            , CListHeaderEditDlg::OnButton)
    EVT_COMBOBOX           (ID_CB_SET_NAME, CListHeaderEditDlg::OnSetNameSelect)
    EVT_TEXT               (ID_CB_SET_NAME, CListHeaderEditDlg::OnSetNameChange)
    EVT_LIST_ITEM_SELECTED (-1            , CListHeaderEditDlg::OnListSelect)
    EVT_LIST_ITEM_ACTIVATED(-1            , CListHeaderEditDlg::OnListActivate)
END_EVENT_TABLE()


//--------------------------------------------------------------------------

CListHeaderEditDlg::CListHeaderEditDlg(wxWindow *parent, const char * szWorkKey)
              :CResizableDlg( parent, "Edit list column set", SZ_SECT_WND_LST_COLEDIT_DLG)// , wxDEFAULT_DIALOG_STYLE)
{
    wxBoxSizer   * topsizer;
    wxBoxSizer   * rowsizer;
    wxBoxSizer   * colsizer;
    wxBoxSizer   * colsizer_d;
    CStr           ConfigKey;
    CStr           S;
    wxButton     * btnAdd;
    wxButton     * btnDel;
    wxButton     * btnDelAll;
    wxButton     * btnHelp;
    wxStaticText * stCaption;
    wxStaticText * stSetName;

    m_WorkKey         = szWorkKey;
    m_SetIsValid      = FALSE;
    m_SourceIdx       = -1;
    m_DestIdx         = -1;
    m_IsSaving        = FALSE;
    m_lastselect      = 0;

    m_btnOk           = new wxButton     (this, wxID_OK        , wxT("Ok")    );
    m_btnCancel       = new wxButton     (this, wxID_CANCEL    , wxT("Cancel") );
    btnAdd            = new wxButton     (this, ID_BTN_ADD     , wxT(">") );
    btnDel            = new wxButton     (this, ID_BTN_DEL     , wxT("<") );
    btnDelAll         = new wxButton     (this, ID_BTN_DEL_ALL , wxT("<<") );
    btnHelp           = new wxButton     (this, ID_BTN_HELP    , wxT("Help") );
    stCaption         = new wxStaticText (this, -1             , wxT("Caption:"));
    stSetName         = new wxStaticText (this, -1             , wxT("Current set:"));
    m_lstSource = new wxListCtrl(this, ID_LIST_SRC , wxDefaultPosition, wxDefaultSize, wxLC_LIST | wxSUNKEN_BORDER | wxLC_SINGLE_SEL);
    m_lstDest   = new wxListCtrl(this, ID_LIST_DEST, wxDefaultPosition, wxDefaultSize, wxLC_LIST | wxSUNKEN_BORDER | wxLC_SINGLE_SEL);
    m_cbSetName = new wxComboBox(this, ID_CB_SET_NAME, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN);
    m_txtCaption= new wxTextCtrl(this, ID_TXT_CAPTION);

    topsizer = new wxBoxSizer( wxVERTICAL );

    rowsizer    = new wxBoxSizer( wxHORIZONTAL );
    rowsizer->Add(stSetName       , 1, wxALIGN_CENTER | wxALL , 5);
    rowsizer->Add(m_cbSetName     , 3, wxALIGN_CENTER | wxALL , 5);
    topsizer->Add(rowsizer        , 0, wxALIGN_CENTER | wxALL | wxGROW, 5 );

    colsizer = new wxBoxSizer( wxVERTICAL );
    colsizer->Add(btnAdd     , 0, wxALIGN_CENTER | wxALL, 5);
    colsizer->Add(btnDel     , 0, wxALIGN_CENTER | wxALL, 5);
    colsizer->Add(btnDelAll  , 0, wxALIGN_CENTER | wxALL, 5);

    colsizer_d  = new wxBoxSizer( wxVERTICAL );
    colsizer_d->Add(m_lstDest       , 1, wxALL | wxGROW, 5);
    colsizer_d->Add(stCaption       , 0, wxALL | wxGROW, 5);
    colsizer_d->Add(m_txtCaption    , 0, wxALL | wxGROW, 5);

    rowsizer    = new wxBoxSizer( wxHORIZONTAL );
    rowsizer->Add(m_lstSource     , 1, wxALL | wxGROW, 5);
    rowsizer->Add(colsizer        , 0, wxALIGN_CENTER | wxALL, 5 );
    rowsizer->Add(colsizer_d      , 1, wxALL | wxGROW, 5);
    topsizer->Add(rowsizer        , 1, wxALIGN_CENTER | wxALL | wxGROW, 5 );


    rowsizer = new wxBoxSizer( wxHORIZONTAL );
    rowsizer->Add(m_btnOk     , 0, wxALIGN_CENTER | wxALL, 5);
    rowsizer->Add(m_btnCancel , 0, wxALIGN_CENTER | wxALL, 5);
    rowsizer->Add(btnHelp     , 0, wxALIGN_RIGHT  | wxALL, 5);
    topsizer->Add(rowsizer    , 0, wxALIGN_CENTER | wxALL, 5 );


    LoadSetCombo();
    LoadListSrc();
    LoadListDest(m_cbSetName->GetValue().mb_str());


    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    m_lstSource->SetFocus();
    m_btnOk->SetDefault();


    CResizableDlg::SetSize();
}

//--------------------------------------------------------------------------

CListHeaderEditDlg::~CListHeaderEditDlg()
{
    m_Fields.FreeAll();
}

//--------------------------------------------------------------------------

void CListHeaderEditDlg::LoadSetCombo()
{
    const char * setsection;
    int          setnameoffs;
    const char * setselect;
    int          x=0, i=0;

    setselect   = gpApp->GetConfig(SZ_SECT_LIST_COL_CURRENT, m_WorkKey.GetData());
    setnameoffs = strlen(SZ_SECT_LIST_COL_UNIT);
    setsection  = gpApp->GetNextSectionName(CONFIG_FILE_CONFIG, SZ_SECT_LIST_COL_UNIT);
    while (setsection)
    {
        if (0!=strnicmp(setsection, SZ_SECT_LIST_COL_UNIT, setnameoffs))
            break;
        m_cbSetName->Append(wxString::FromAscii(&setsection[setnameoffs]));
        if (0==stricmp(setsection, setselect))
            x = i;
        i++;
        setsection = gpApp->GetNextSectionName(CONFIG_FILE_CONFIG, setsection);
    }
    m_cbSetName->SetSelection(x);
}

//--------------------------------------------------------------------------

void CListHeaderEditDlg::LoadListSrc()
{
    int          i;
    const char * item;

    m_SourceIdx       = -1;
    m_lstSource->ClearAll();
    for (i=0; i<gpApp->m_pAtlantis->m_UnitPropertyNames.Count(); i++)
    {
        item = (const char *) gpApp->m_pAtlantis->m_UnitPropertyNames.At(i);
        m_lstSource->InsertItem(m_lstSource->GetItemCount() , wxString::FromAscii(item));
    }
}

//--------------------------------------------------------------------------

void CListHeaderEditDlg::LoadListDest(const char * szNewName)
{
    CStr           S;
    CStr           Section;
    int            sectidx;
    const char   * szName;
    const char   * szValue;
    TUnitColData * pField;

    if (m_IsSaving)
        return;

    SaveListDest();
    m_txtCaption->SetValue(wxT(""));


    m_DestIdx         = -1;
    m_lstDest->ClearAll();
    m_Fields.FreeAll();
    m_SetIsValid = FALSE;

    m_SetName =szNewName;
    if (m_SetName.IsEmpty())
        return;

    Section << SZ_SECT_LIST_COL_UNIT << m_SetName;

    sectidx = gpApp->GetSectionFirst(Section.GetData(), szName, szValue);
    while (sectidx >= 0)
    {
// 001 = 87, 0, faction, FName
        m_SetIsValid  = TRUE;
        pField        = new TUnitColData;

        szValue       = SkipSpaces(S.GetToken(szValue, ','));
        pField->width = atol(S.GetData());
        szValue       = SkipSpaces(S.GetToken(szValue, ','));
        pField->flags = atol(S.GetData());
        szValue       = SkipSpaces(pField->PropName.GetToken(szValue, ','));
        szValue       = SkipSpaces(pField->Caption.GetToken(szValue, ','));

        m_lstDest->InsertItem(m_lstDest->GetItemCount() , wxString::FromAscii(pField->PropName.GetData()));
        if (!m_Fields.Insert(pField))
            delete pField;

        sectidx = gpApp->GetSectionNext(sectidx, Section.GetData(), szName, szValue);
    }
}

//--------------------------------------------------------------------------

void  CListHeaderEditDlg::SaveListDest()
{
    CStr           S;
    CStr           Value;
    CStr           Section;
    int            x;
    TUnitColData   Dummy;
    TUnitColData * pField;
    int            idx;
    BOOL           found=FALSE;

    if (!m_SetIsValid || m_IsSaving)
        return;
    m_IsSaving = TRUE;

    ProcessCaption(m_DestIdx, m_DestIdx);

    for (x=0; x<(int)m_cbSetName->GetCount(); x++)
        if (0==stricmp(m_cbSetName->GetString(x).mb_str(), m_SetName.GetData()))
        {
            found = TRUE;
            break;
        }
    if (!found)
        m_cbSetName->Append(wxString::FromAscii(m_SetName.GetData()));

    Section << SZ_SECT_LIST_COL_UNIT << m_SetName;
    gpApp->RemoveSection(Section.GetData());

    for (x=0; x<m_lstDest->GetItemCount(); x++)
    {
        Dummy.PropName = m_lstDest->GetItemText(x).mb_str();
        if (m_Fields.Search(&Dummy, idx))
        {
            pField = (TUnitColData *)m_Fields.At(idx);
            S.Format("%03d", x);
            Value.Format("%d, %lu, %s, %s", pField->width, pField->flags, pField->PropName.GetData(), pField->Caption.GetData());
            gpApp->SetConfig(Section.GetData(), S.GetData(), Value.GetData());
        }
    }

    m_IsSaving = FALSE;
}

//--------------------------------------------------------------------------

void  CListHeaderEditDlg::AddItem()
{
    TUnitColData   Dummy;
    TUnitColData * pField;
    int            idx;
    int            flags = 0;

    if (m_SourceIdx >= 0 && m_SourceIdx < m_lstSource->GetItemCount())
        Dummy.PropName = m_lstSource->GetItemText(m_SourceIdx).mb_str();
    if (Dummy.PropName.IsEmpty())
        return;

    if (m_Fields.Search(&Dummy, idx))
        pField = (TUnitColData *)m_Fields.At(idx);
    else
    {
        CStrInt      * pSI, SI(Dummy.PropName.GetData(), 0);

        if (gpApp->m_pAtlantis->m_UnitPropertyTypes.Search(&SI, idx))
        {
            pSI = (CStrInt*)gpApp->m_pAtlantis->m_UnitPropertyTypes.At(idx);
            if (eLong == (EValueType)pSI->m_value)
                flags = LIST_FLAG_ALIGN_RIGHT;
        }

        pField           = new TUnitColData;
        pField->width    = 40;
        pField->flags    = flags;
        pField->PropName = Dummy.PropName;
        pField->Caption  = Dummy.PropName;

        if (!m_Fields.Insert(pField))
            delete pField;
    }
    m_lstDest->InsertItem(m_DestIdx+1, wxString::FromAscii(pField->PropName.GetData()));
    m_lstDest->SetItemState(m_DestIdx+1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
//    m_txtCaption->SetValue(wxString::FromAscii(pField->Caption.GetData()));
    m_SetIsValid = TRUE;
}

//--------------------------------------------------------------------------

void  CListHeaderEditDlg::ProcessCaption(int oldidx, int newidx)
{
    TUnitColData   Dummy;
    TUnitColData * pField;
    int            idx;
    CStr           Caption;

    // Save caption
    if (oldidx >= 0 && oldidx < m_lstDest->GetItemCount())
    {
        Dummy.PropName = m_lstDest->GetItemText(oldidx).mb_str();
        if (m_Fields.Search(&Dummy, idx))
        {
            pField          = (TUnitColData *)m_Fields.At(idx);
            pField->Caption = m_txtCaption->GetValue().mb_str();
        }
    }

    // Load caption
    if (newidx >= 0 && newidx < m_lstDest->GetItemCount())
    {
        Dummy.PropName = m_lstDest->GetItemText(newidx).mb_str();
        if (m_Fields.Search(&Dummy, idx))
        {
            pField          = (TUnitColData *)m_Fields.At(idx);
            Caption         = pField->Caption.GetData();
        }
    }
    m_txtCaption->SetValue(wxString::FromAscii(Caption.GetData()));
}

//--------------------------------------------------------------------------

void  CListHeaderEditDlg::DeleteItem()
{
    m_lstDest->DeleteItem(m_DestIdx);

    if (m_DestIdx > m_lstDest->GetItemCount()-1)
        m_DestIdx = m_lstDest->GetItemCount()-1;

    ProcessCaption(-1, m_DestIdx);
    m_lstDest->SetItemState(m_DestIdx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

//--------------------------------------------------------------------------


void CListHeaderEditDlg::OnButton(wxCommandEvent& event)
{
    CStr       S;

    switch (event.GetId())
    {
    case wxID_OK:
        SaveListDest();
        S << SZ_SECT_LIST_COL_UNIT << m_SetName;
        gpApp->SetConfig(SZ_SECT_LIST_COL_CURRENT, m_WorkKey.GetData(), S.GetData());
        StoreSize();
        EndModal(wxID_OK);
        break;

    case wxID_CANCEL:
        StoreSize();
        EndModal(wxID_CANCEL);
        break;


    case ID_BTN_ADD:
        AddItem();
        break;

    case ID_BTN_DEL:
        DeleteItem();
        break;

    case ID_BTN_DEL_ALL:
        m_lstDest->DeleteAllItems();
        break;

    case ID_BTN_HELP:
        wxMessageBox(wxT("- Column set names can not be modified.\n"
                     "- To add a column set type in the name for the new set and add items.\n"
                     "- To delete a column set remove all items from it."));
    }

}

//--------------------------------------------------------------------------

void  CListHeaderEditDlg::OnSetNameChange(wxCommandEvent& event)
{
    if (m_IsSaving)
        return;
    if (m_lastselect < time(NULL))
        LoadListDest(m_cbSetName->GetValue().mb_str()); // returns the old value on windoze when value is selected
}

//--------------------------------------------------------------------------

void  CListHeaderEditDlg::OnSetNameSelect(wxCommandEvent& event)
{
    if (m_IsSaving)
        return;
    LoadListDest(m_cbSetName->GetString(m_cbSetName->GetSelection()).mb_str());
    m_lastselect = time(NULL);
}

//--------------------------------------------------------------------------

void  CListHeaderEditDlg::OnListSelect(wxListEvent& event)
{
    switch (event.GetId())
    {
    case ID_LIST_SRC:
        m_SourceIdx = event.GetIndex();
        break;
    case ID_LIST_DEST:
        ProcessCaption(m_DestIdx, event.GetIndex());
        m_DestIdx = event.GetIndex();
        break;
    default:
        return;
    }
}

//--------------------------------------------------------------------------

void  CListHeaderEditDlg::OnListActivate(wxListEvent& event)
{
    switch (event.GetId())
    {
    case ID_LIST_SRC:
        m_SourceIdx = event.GetIndex();
        AddItem();
        break;
    case ID_LIST_DEST:
        m_DestIdx = event.GetIndex();
        DeleteItem();
        break;
    default:
        return;
    }
}

//--------------------------------------------------------------------------
