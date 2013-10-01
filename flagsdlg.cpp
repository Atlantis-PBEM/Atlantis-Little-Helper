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

#include "wx/spinctrl.h"
#include "wx/dialog.h"


#include "cstr.h"
#include "collection.h"
#include "cfgfile.h"
#include "files.h"
#include "atlaparser.h"
#include "consts_ah.h"
#include "hash.h"
#include "ahapp.h"
#include "ahframe.h"

#include "flagsdlg.h"


const char * LandFlagLabel[LAND_FLAG_COUNT] = {SZ_KEY_MAP_FLAG_1, SZ_KEY_MAP_FLAG_2, SZ_KEY_MAP_FLAG_3 };


#define SPACER_GENERIC 5
#define SPACER_NARROW  1

BEGIN_EVENT_TABLE(CUnitFlagsDlg, wxDialog)
    EVT_BUTTON  (wxID_CANCEL  ,  CUnitFlagsDlg::OnCancel)
    EVT_BUTTON  (wxID_OK      ,  CUnitFlagsDlg::OnOk)

    EVT_BUTTON  (ID_BTN_SET_ALL_LAND      ,  CUnitFlagsDlg::OnSetClear)
    EVT_BUTTON  (ID_BTN_SET_ALL_UNIT      ,  CUnitFlagsDlg::OnSetClear)
    EVT_BUTTON  (ID_BTN_RMV_ALL_LAND      ,  CUnitFlagsDlg::OnSetClear)
    EVT_BUTTON  (ID_BTN_RMV_ALL_UNIT      ,  CUnitFlagsDlg::OnSetClear)

END_EVENT_TABLE()



//--------------------------------------------------------------------------

CUnitFlagsDlg::CUnitFlagsDlg(wxWindow *parent, E_FLAG_EDIT_MODE Mode, unsigned int UnitFlags)
     :CResizableDlg( parent, "Flags", SZ_SECT_WND_LAND_UNIT_FLAGS_DLG )
{
    wxBoxSizer      * topsizer;
    wxBoxSizer      * sizer   ;
    wxStaticText    * st;
    int               i, x;
    CStr              sKey;
    const char      * p;

    // Zero all pointers first
    m_btnOk         = NULL;
    m_btnCancel     = NULL;
    m_btnSetAllLand = NULL;
    m_btnSetAllUnit = NULL;
    m_btnRmvAllLand = NULL;
    m_btnRmvAllUnit = NULL;
    memset(m_chbUnitFlags   , 0, sizeof(m_chbUnitFlags));
    memset(m_chbLandFlags   , 0, sizeof(m_chbLandFlags));
    memset(m_txtUnitFlagText, 0, sizeof(m_txtUnitFlagText));
    
    
    m_UnitFlags = UnitFlags;
    m_LandFlags = 0;
    m_EditMode  = Mode;


    topsizer = new wxBoxSizer( wxVERTICAL );


    // Unit flags

    st = new wxStaticText(this, -1, "Unit Flags");
    topsizer->Add(st         , 0, wxALIGN_CENTER | wxALL , SPACER_GENERIC);

    x = 1;
    for (i=0; i<UNIT_CUSTOM_FLAG_COUNT; i++)
    {
        sKey.Empty();
        sKey << (long)i;
        p = gpApp->GetConfig(SZ_SECT_UNIT_FLAG_NAMES, sKey.GetData());

        sizer    = new wxBoxSizer( wxHORIZONTAL );
        if (eThisUnit == m_EditMode || eAll == m_EditMode || eManyUnits == m_EditMode)
        {
            m_chbUnitFlags[i]     = new wxCheckBox   (this, -1, "");
            if (m_UnitFlags & x)
                m_chbUnitFlags[i]->SetValue(TRUE);
            sizer->Add(m_chbUnitFlags[i] , 0, wxALIGN_CENTER | wxLEFT | wxRIGHT , SPACER_GENERIC);
        }
        sKey.Empty();
        sKey << (long)(i+1) << ". ";
        st = new wxStaticText(this, -1, sKey.GetData());
        m_txtUnitFlagText[i]  = new wxTextCtrl(this, -1, p ); //, wxDefaultPosition, wxDefaultSize); //wxSize(100,30));
        m_txtUnitFlagText[i]->SetEditable(eNames == m_EditMode);

        sizer->Add(st                   , 0, wxALIGN_CENTER | wxLEFT | wxRIGHT , SPACER_GENERIC);
        sizer->Add(m_txtUnitFlagText[i] , 1, wxALIGN_CENTER | wxLEFT | wxRIGHT , SPACER_GENERIC);

        topsizer->Add(sizer        , 1, wxALIGN_CENTER | wxALL | wxGROW, 2 );

        x <<= 1; 
    }

    if (eAll == m_EditMode || eManyUnits == m_EditMode)
    {
        m_btnSetAllUnit = new wxButton     (this, ID_BTN_SET_ALL_UNIT , "Set for all units"    ); 
        m_btnRmvAllUnit = new wxButton     (this, ID_BTN_RMV_ALL_UNIT , "Clear for all units"    );
        sizer    = new wxBoxSizer( wxHORIZONTAL );
            sizer->Add(m_btnSetAllUnit , 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC );
            sizer->Add(m_btnRmvAllUnit , 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC);
        topsizer->Add(sizer, 0, wxALIGN_CENTER );
    }

    // Land Flags

    if (eAll == m_EditMode)
    {
        st = new wxStaticText(this, -1, "Land Flags");
        topsizer->Add(st         , 0, wxALIGN_CENTER | wxALL , SPACER_GENERIC);

        for (i=0; i<LAND_FLAG_COUNT; i++)
        {
            m_chbLandFlags[i] = new wxCheckBox   (this, -1, LandFlagLabel[i]);

            sizer    = new wxBoxSizer( wxHORIZONTAL );
                sizer->Add(m_chbLandFlags[i]   , 0, wxALIGN_LEFT  | wxLEFT | wxRIGHT , SPACER_GENERIC );
            topsizer->Add(sizer        , 1, wxALIGN_CENTER | wxALL | wxGROW, 2 );
        }

        m_btnSetAllLand = new wxButton     (this, ID_BTN_SET_ALL_LAND , "Set for all hexes"    ); 
        m_btnRmvAllLand = new wxButton     (this, ID_BTN_RMV_ALL_LAND , "Clear for all hexes"    );
        sizer    = new wxBoxSizer( wxHORIZONTAL );
            sizer->Add(m_btnSetAllLand , 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC );
            sizer->Add(m_btnRmvAllLand , 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC);
        topsizer->Add(sizer, 0, wxALIGN_CENTER );
    }


    m_btnCancel     = new wxButton     (this, wxID_CANCEL , "Cancel" );
    if (eNames == m_EditMode || eThisUnit == m_EditMode)
        m_btnOk     = new wxButton     (this, wxID_OK     , "Set"    );

    sizer    = new wxBoxSizer( wxHORIZONTAL );
        if (m_btnOk)
            sizer->Add(m_btnOk       , 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC );
        sizer->Add(    m_btnCancel   , 0, wxALIGN_CENTER | wxALL, SPACER_GENERIC );
    topsizer->Add(sizer, 0, wxALIGN_CENTER );



    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    if (m_btnOk)
        m_btnOk->SetDefault();

    CResizableDlg::SetPos();
}


//--------------------------------------------------------------------------

CUnitFlagsDlg::~CUnitFlagsDlg()
{
}

//--------------------------------------------------------------------------

void CUnitFlagsDlg::OnOk(wxCommandEvent& event)
{
    int i, x;
    CStr sKey;

    if (eNames == m_EditMode)
    {
        for (i=0; i<UNIT_CUSTOM_FLAG_COUNT; i++)
        {
            sKey.Empty();
            sKey << (long)i;
            gpApp->SetConfig(SZ_SECT_UNIT_FLAG_NAMES, sKey.GetData(), m_txtUnitFlagText[i]->GetValue());
        }
        CUnit::ResetCustomFlagNames();
    }
    
    if (eThisUnit == m_EditMode)
    {
        x           = 1;
        m_UnitFlags = 0;
        for (i=0; i<UNIT_CUSTOM_FLAG_COUNT; i++)
        {
            if (m_chbUnitFlags[i]->IsChecked())
                m_UnitFlags |= x;
            x <<= 1; 
        }
    }
    
    StoreSize();
    EndModal(wxID_OK);
}

//--------------------------------------------------------------------------

void CUnitFlagsDlg::OnCancel(wxCommandEvent& event)
{
    StoreSize();
    EndModal(wxID_CANCEL);
}

//--------------------------------------------------------------------------

void CUnitFlagsDlg::OnSetClear(wxCommandEvent& event)
{
    int x, i;

    x           = 1;
    m_UnitFlags = 0;
    for (i=0; i<UNIT_CUSTOM_FLAG_COUNT; i++)
    {
        if (m_chbUnitFlags[i] && m_chbUnitFlags[i]->IsChecked())
            m_UnitFlags |= x;
        x <<= 1; 
    }

    x           = 1;
    m_LandFlags = 0;
    for (i=0; i<LAND_FLAG_COUNT; i++)
    {
        if (m_chbLandFlags[i] && m_chbLandFlags[i]->IsChecked())
            m_LandFlags |= x;
        x <<= 1;
    }
    
    StoreSize();
    EndModal(event.GetId());
}


//==========================================================================

BEGIN_EVENT_TABLE(CMapFlagDlg, wxDialog)
    EVT_BUTTON(-1, CMapFlagDlg::OnButton)
END_EVENT_TABLE()



CMapFlagDlg::CMapFlagDlg(wxWindow *parent, CLand * pLand, wxPoint & position) 
            :wxDialog( parent, -1, "Hex flags", position) //, wxDefaultSize, wxDIALOG_MODAL | wxRESIZE_BORDER )
{  
    int i;

    m_pLand = pLand;

    wxBoxSizer      * topsizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer      * sizer   ;
    wxStaticText    * st;


    m_btnSet    = new wxButton  (this, wxID_OK    , "Set" );
    m_btnRemove = new wxButton  (this, wxID_NO    , "Remove" );
    m_btnCancel = new wxButton  (this, wxID_CANCEL, "Cancel" );

    for (i=0; i<LAND_FLAG_COUNT; i++)
    {
        m_FlagText[i]  = new wxTextCtrl(this, -1, pLand->FlagText[i].GetData(), wxDefaultPosition, wxDefaultSize); //wxSize(100,30));
        sizer          = new wxBoxSizer( wxHORIZONTAL );
        st             = new wxStaticText(this, -1, LandFlagLabel[i]);

        sizer->Add(st           , 0, wxALIGN_LEFT  | wxALL           , SPACER_NARROW );
        sizer->Add(m_FlagText[i], 1, wxALIGN_RIGHT | wxALL | wxEXPAND, SPACER_NARROW );
    
        topsizer->Add(sizer, 1, wxALIGN_LEFT | wxGROW);
    }

    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );  
    
    button_sizer->Add(  m_btnSet   , 0, wxALL, 10 );
    button_sizer->Add(  m_btnRemove, 0, wxALL, 10 );
    button_sizer->Add(  m_btnCancel, 0, wxALL, 10 );
    
    
    topsizer->Add( button_sizer, 0, wxALIGN_CENTER );

    SetAutoLayout( TRUE );     // tell dialog to use sizer
    SetSizer( topsizer );      // actually set the sizer
    topsizer->Fit( this );            // set size to minimum size as calculated by the sizer
    topsizer->SetSizeHints( this );   // set size hints to honour mininum size}

    m_FlagText[0]->SetFocus();
    m_btnSet  ->SetDefault();


    int x,y, w,h, w1,h1;
    BOOL change = FALSE;

    GetPosition(&x,&y);
    GetSize(&w,&h);
    wxDisplaySize(&w1,&h1);

    if (x<0)
    {
        x      = 0;
        change = TRUE;
    }
    if (y<0)
    {
        y      = 0;
        change = TRUE;
    }
    if (x+w > w1)
    {
        x = w1-w;
        change = TRUE;
    }
    if (y+h > h1)
    {
        y = h1-h;
        change = TRUE;
    }
    if (change)
        SetSize(x,y,w,h);


}

//--------------------------------------------------------------------------

void CMapFlagDlg::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == m_btnRemove )
        EndModal(wxID_NO);
    else
        event.Skip();
}

//--------------------------------------------------------------------------

