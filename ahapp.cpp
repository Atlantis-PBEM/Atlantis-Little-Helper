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

#include "wx/splitter.h"
#include "wx/listctrl.h"
//#include "wx/resource.h"

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
#include "mapframe.h"
#include "unitframe.h"
#include "unitframefltr.h"
#include "msgframe.h"
#include "editsframe.h"
#include "editpane.h"
#include "mappane.h"
#include "listpane.h"
#include "unitpane.h"
#include "utildlgs.h"
#include "unitfilterdlg.h"
#include "unitpanefltr.h"
#include "listcoledit.h"
#include "optionsdlg.h"
#include "flagsdlg.h"

#ifdef __WXMAC_OSX__
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
#endif

CAhApp * gpApp = NULL; // Our own the one and only pointer

IMPLEMENT_APP(CAhApp);

static CGameDataHelper ThisGameDataHelper;

//=========================================================================

CAhApp::CAhApp() : m_HexDescrSrc    (128),
                   m_UnitDescrSrc   (128),
                   m_ItemWeights    ( 32),
                   m_OrderHash      (  3),
                   m_TradeItemsHash (  2),
                   m_MenHash        (  2),
                   m_MaxSkillHash   (  6),
                   m_MagicSkillsHash(  6)
{
    m_FirstLoad         = TRUE;
    m_OrdersAreChanged  = FALSE;
    m_CommentsChanged   = FALSE;
    m_UpgradeLandFlags  = FALSE;
    m_DiscardChanges    = FALSE;
    m_SelUnitIdx        = -1;
    m_layout            = 0;
    m_DisableErrs       = FALSE;
    m_pAccel            = NULL;

    memset(m_Frames, 0, sizeof(m_Frames));
    memset(m_Panes , 0, sizeof(m_Panes ));
    memset(m_Fonts , 0, sizeof(m_Fonts ));

    m_FontDescr[FONT_EDIT_DESCR]  = "Descriptions";
    m_FontDescr[FONT_EDIT_ORDER]  = "Orders & comments";
    m_FontDescr[FONT_MAP_COORD ]  = "Map coordinates";
    m_FontDescr[FONT_MAP_TEXT  ]  = "Map text";
    m_FontDescr[FONT_UNIT_LIST ]  = "Unit list";
    m_FontDescr[FONT_EDIT_HDR  ]  = "Edit pane header";
    m_FontDescr[FONT_VIEW_DLG  ]  = "View dialogs";
    m_FontDescr[FONT_ERR_DLG   ]  = "Messages and Errors";




}

CAhApp::~CAhApp()
{
}

//-------------------------------------------------------------------------

bool CAhApp::OnInit()
{
    int               i;
    const char      * p;
    const char      * szName;
    const char      * szValue;
    CStrStr         * pSS;
    CStr              S(32), S2;
    int               sectidx;
    CStrStrColl2      Coll;


    gpApp = this;

    m_ConfigSectionsState.Insert(strdup(SZ_SECT_DEF_ORDERS       ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_ORDERS           ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_REPORTS          ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_LAND_FLAGS       ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_LAND_VISITED     ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_SKILLS           ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_ITEMS            ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_OBJECTS          ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_PASSWORDS        ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_UNIT_TRACKING    ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_FOLDERS          ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_DO_NOT_SHOW_THESE));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_TROPIC_ZONE      ));
    m_ConfigSectionsState.Insert(strdup(SZ_SECT_UNIT_FLAGS       ));


    m_Config[CONFIG_FILE_CONFIG].Load(SZ_CONFIG_FILE);
    m_Config[CONFIG_FILE_STATE ].Load(SZ_CONFIG_STATE_FILE);

    UpgradeConfigFiles();

    m_layout = atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_LAYOUT));
    if (m_layout<0)
        m_layout = 0;
    if (m_layout>=AH_LAYOUT_COUNT)
        m_layout = AH_LAYOUT_COUNT-1;

    m_Brightness_Delta = atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_BRIGHT_DELTA));


    for (i=0; i<FONT_COUNT; i++)
    {
        S.Empty();
        S << (long)i;
        szValue = GetConfig(SZ_SECT_FONTS_2, S.GetData());
        m_Fonts[i] = NewFontFromStr(szValue);
    }

    if (0==stricmp(SZ_EOL_MS, GetConfig(SZ_SECT_COMMON, SZ_KEY_EOL)))
        EOL_FILE = EOL_MS;
    else
        EOL_FILE = EOL_UNIX;


    // Load unit property groups
    m_UnitPropertyGroups.m_bDuplicates=TRUE;
    sectidx = GetSectionFirst(SZ_SECT_UNITPROP_GROUPS, szName, szValue);
    while (sectidx >= 0)
    {
        while (szValue && *szValue)
        {
            szValue = S.GetToken(szValue, ',');
            pSS     = new CStrStr(szName, S.GetData());
            if (Coll.Insert(pSS))
                m_UnitPropertyGroups.Insert(pSS);
            else
                delete pSS;
        }
        sectidx = GetSectionNext(sectidx, SZ_SECT_UNITPROP_GROUPS, szName, szValue);
    }
    CUnit::m_PropertyGroupsColl = &m_UnitPropertyGroups;


    // Property group name must not be an alias!
    szName = "";
    for (i=0; i<Coll.Count(); i++)
    {
        pSS = (CStrStr*)Coll.At(i);
        if (0!=stricmp(szName, pSS->m_key))
        {
            szName = pSS->m_key;
            p = ResolveAlias(szName);
            if (0!=stricmp(szName, p))
            {
                S = "Group name \"";
                S << szName << "\" can be resolved as alias for \"" << p << "\"!\r\n";
                ShowError(S.GetData(), S.GetLength(), TRUE);
            }
        }
    }
    Coll.DeleteAll();


    InitMoveModes();

    //m_Attitudes.FreeAll();
    SetAttitudeForFaction(0, ATT_NEUTRAL);

    // Water terrain types
    //m_WaterTerrainNames = CStringSortColl(); no need for that
    p = SkipSpaces(GetConfig(SZ_SECT_COMMON, SZ_KEY_WATER_TERRAINS));
    int idx;
    while (p && *p)
    {
        p = SkipSpaces(S.GetToken(p, ','));
        if (!S.IsEmpty() && !m_WaterTerrainNames.Search((void *)S.ToLower(), idx))
            m_WaterTerrainNames.Insert(strdup(S.ToLower()));
    }


    // Load order hash
    m_OrderHash.Insert("advance"    ,     (void*)O_ADVANCE    );
    m_OrderHash.Insert("assassinate",     (void*)O_ASSASSINATE);
    m_OrderHash.Insert("attack"     ,     (void*)O_ATTACK     );
    m_OrderHash.Insert("autotax"    ,     (void*)O_AUTOTAX    );
    m_OrderHash.Insert("build"      ,     (void*)O_BUILD      );
    m_OrderHash.Insert("buy"        ,     (void*)O_BUY        );
    m_OrderHash.Insert("claim"      ,     (void*)O_CLAIM      );
    m_OrderHash.Insert("end"        ,     (void*)O_ENDFORM    );
    m_OrderHash.Insert("endturn"    ,     (void*)O_ENDTURN    );
    m_OrderHash.Insert("enter"      ,     (void*)O_ENTER      );
    m_OrderHash.Insert("form"       ,     (void*)O_FORM       );
    m_OrderHash.Insert("give"       ,     (void*)O_GIVE       );
    m_OrderHash.Insert("giveif"     ,     (void*)O_GIVEIF     );
    m_OrderHash.Insert("take"       ,     (void*)O_TAKE       );
    m_OrderHash.Insert("send"       ,     (void*)O_SEND       );
    m_OrderHash.Insert("withdraw"   ,     (void*)O_WITHDRAW   );
    m_OrderHash.Insert("leave"      ,     (void*)O_LEAVE      );
    m_OrderHash.Insert("move"       ,     (void*)O_MOVE       );
    m_OrderHash.Insert("produce"    ,     (void*)O_PRODUCE    );
    m_OrderHash.Insert("promote"    ,     (void*)O_PROMOTE    );
    m_OrderHash.Insert("sail"       ,     (void*)O_SAIL       );
    m_OrderHash.Insert("sell"       ,     (void*)O_SELL       );
    m_OrderHash.Insert("steal"      ,     (void*)O_STEAL      );
    m_OrderHash.Insert("study"      ,     (void*)O_STUDY      );
    m_OrderHash.Insert("teach"      ,     (void*)O_TEACH      );
    m_OrderHash.Insert("turn"       ,     (void*)O_TURN       );

    m_OrderHash.Insert("tax"        ,     (void*)O_TAX        );
    m_OrderHash.Insert("work"       ,     (void*)O_WORK       );

    m_OrderHash.Insert("guard"      ,     (void*)O_GUARD      );
    m_OrderHash.Insert("avoid"      ,     (void*)O_AVOID      );
    m_OrderHash.Insert("behind"     ,     (void*)O_BEHIND     );
    m_OrderHash.Insert("reveal"     ,     (void*)O_REVEAL     );
    m_OrderHash.Insert("hold"       ,     (void*)O_HOLD       );
    m_OrderHash.Insert("noaid"      ,     (void*)O_NOAID      );
    m_OrderHash.Insert("consume"    ,     (void*)O_CONSUME    );
    m_OrderHash.Insert("nocross"    ,     (void*)O_NOCROSS    );
    m_OrderHash.Insert("spoils"     ,     (void*)O_SPOILS     );

    m_OrderHash.Insert("recruit"    ,     (void*)O_RECRUIT    );
    m_OrderHash.Insert("share"      ,     (void*)O_SHARE      );

    m_OrderHash.Insert("template"   ,     (void*)O_TEMPLATE   );
    m_OrderHash.Insert("endtemplate",     (void*)O_ENDTEMPLATE);
    m_OrderHash.Insert("all"        ,     (void*)O_ALL        );
    m_OrderHash.Insert("endall"     ,     (void*)O_ENDALL     );

    m_OrderHash.Insert("type"       ,     (void*)O_TYPE       );
    m_OrderHash.Insert("label"      ,     (void*)O_LABEL      );
    m_OrderHash.Insert("name"       ,     (void*)O_NAME       );






    p = SkipSpaces(GetConfig(SZ_SECT_COMMON, SZ_KEY_VALID_ORDERS));
    while (p && *p)
    {
        const void * data;
        p = SkipSpaces(S.GetToken(p, ','));
        if (!S.IsEmpty() && !m_OrderHash.Locate(S.GetData(), data))
            m_OrderHash.Insert(S.GetData(), (void*)-1);
    }
//    m_OrderHash.Dbg_Print();

    // Load trade items hash
    p = SkipSpaces(GetConfig(SZ_SECT_UNITPROP_GROUPS,  PRP_TRADE_ITEMS));
    while (p && *p)
    {
        const void * data;
        p = SkipSpaces(S.GetToken(p, ','));
        if (!S.IsEmpty() && !m_TradeItemsHash.Locate(S.GetData(), data))
            m_TradeItemsHash.Insert(S.GetData(), (void*)-1);
    }

    // All the men hash
    p = SkipSpaces(GetConfig(SZ_SECT_UNITPROP_GROUPS,  PRP_MEN));
    while (p && *p)
    {
        const void * data;
        p = SkipSpaces(S.GetToken(p, ','));
        if (!S.IsEmpty() && !m_MenHash.Locate(S.GetData(), data))
            m_MenHash.Insert(S.GetData(), (void*)-1);
    }

    // Magic skills hash
    p = SkipSpaces(GetConfig(SZ_SECT_UNITPROP_GROUPS,  PRP_MAG_SKILLS));
    while (p && *p)
    {
        const void * data;
        int x;
        p = SkipSpaces(S.GetToken(p, ','));
        x = S.FindSubStrR(PRP_SKILL_POSTFIX);
        if (x>=0)
            S.DelSubStr(x, S.GetLength()-x+1);

        if (!S.IsEmpty() && !m_MagicSkillsHash.Locate(S.GetData(), data))
            m_MagicSkillsHash.Insert(S.GetData(), (void*)-1);
    }

    m_pAtlantis = new CAtlaParser(&ThisGameDataHelper);
    m_Reports.Insert(m_pAtlantis);

    // Read list of year/month for report
    i = GetSectionFirst(SZ_SECT_REPORTS, szName, szValue);
    while (i>=0)
    {
        m_ReportDates.Insert((void*)atol(szName));
        i = GetSectionNext (i, SZ_SECT_REPORTS, szName, szValue);
    }





    StdRedirectInit();

    CreateAccelerator();

    OpenMapFrame();

    if ((AH_LAYOUT_3_WIN==m_layout || AH_LAYOUT_2_WIN==m_layout) &&
        atol(GetConfig(CUnitFrame::GetConfigSection(m_layout), SZ_KEY_OPEN)) )
        OpenUnitFrame();

    if ((AH_LAYOUT_3_WIN==m_layout) &&
        (atol(GetConfig(CEditsFrame::GetConfigSection(m_layout), SZ_KEY_OPEN))) )
        OpenEditsFrame();

    SetTopWindow(m_Frames[AH_FRAME_MAP]);
    m_Frames[AH_FRAME_MAP]->SetFocus();


    if (argc>1)
        for (i=1; i<argc; i++)
            LoadReport(wxString(argv[i]).mb_str(), i>1);
    else
        if (atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_LOAD_REP)) && (m_ReportDates.Count() > 0) )
        {
            S.Empty();
            S << (long)m_ReportDates.At(m_ReportDates.Count()-1);
            S2 = GetConfig(SZ_SECT_REPORTS, S.GetData());
            const char * p = S2.GetData();
            BOOL         join = FALSE;
            while (p && *p)
            {
                p = S.GetToken(p, ',');
                LoadReport(S.GetData(), join);
                join = TRUE;
            }
        }

    if (atol(GetConfig(CUnitFrameFltr::GetConfigSection(m_layout), SZ_KEY_OPEN)) )
        OpenUnitFrameFltr(FALSE);

    return TRUE;
}

//-------------------------------------------------------------------------

int CAhApp::OnExit()
{
    int  i;
    CStr S;
    CStr Name;

    CUnit::ResetCustomFlagNames();

    for (i=0; i<FONT_COUNT; i++)
    {
        FontToStr(m_Fonts[i], S);
        Name.Empty();
        Name << (long)i;
        SetConfig(SZ_SECT_FONTS_2, Name.GetData(), S.GetData());
    }

    if (!m_DiscardChanges)
    {
        m_Config[CONFIG_FILE_CONFIG].Save(SZ_CONFIG_FILE);
        m_Config[CONFIG_FILE_STATE ].Save(SZ_CONFIG_STATE_FILE);

        if (ERR_OK==m_pAtlantis->m_ParseErr)
            SaveHistory(SZ_HISTORY_FILE);
    }

    m_TradeItemsHash.FreeAll();
    m_MenHash.FreeAll();
    m_MaxSkillHash.FreeAll();
    m_MagicSkillsHash.FreeAll();

    m_Reports.FreeAll();

    for (i=0; i<FONT_COUNT; i++)
        delete m_Fonts[i];


    if (m_pAccel)
        delete m_pAccel;

    m_MoveModes.FreeAll();
    m_ItemWeights.FreeAll();
    m_ConfigSectionsState.FreeAll();
    m_OrderHash.FreeAll();
    m_Attitudes.FreeAll();
    m_WaterTerrainNames.FreeAll();

    StdRedirectDone();

    gpApp = NULL;
    return 0;
}

//-------------------------------------------------------------------------

void CAhApp::CreateAccelerator()
{
    static wxAcceleratorEntry entries[5];
    entries[0].Set(wxACCEL_CTRL,  (int)'S',     menu_SaveOrders);
    entries[1].Set(wxACCEL_CTRL,  (int)'N',     accel_NextUnit );
    entries[2].Set(wxACCEL_CTRL,  (int)'P',     accel_PrevUnit );
    entries[3].Set(wxACCEL_CTRL,  (int)'U',     accel_UnitList );
    entries[4].Set(wxACCEL_CTRL,  (int)'O',     accel_Orders   );
    
    m_pAccel = new wxAcceleratorTable(5, entries);
}

//-------------------------------------------------------------------------

void CAhApp::Redraw()
{
    int i;

    for (i=0; i<AH_PANE_COUNT; i++)
        if (m_Panes[i])
            m_Panes[i]->Refresh(FALSE);
}


//-------------------------------------------------------------------------

void CAhApp::ApplyFonts()
{

    if (m_Panes[AH_PANE_MAP          ]) ((CMapPane *)m_Panes[AH_PANE_MAP          ])->ApplyFonts();
    if (m_Panes[AH_PANE_MAP_DESCR    ]) ((CEditPane*)m_Panes[AH_PANE_MAP_DESCR    ])->ApplyFonts();
    if (m_Panes[AH_PANE_UNITS_HEX    ]) ((CUnitPane*)m_Panes[AH_PANE_UNITS_HEX    ])->ApplyFonts();
    if (m_Panes[AH_PANE_UNITS_FILTER ]) ((CUnitPane*)m_Panes[AH_PANE_UNITS_FILTER ])->ApplyFonts();
    if (m_Panes[AH_PANE_UNIT_DESCR   ]) ((CEditPane*)m_Panes[AH_PANE_UNIT_DESCR   ])->ApplyFonts();
    if (m_Panes[AH_PANE_UNIT_COMMANDS]) ((CEditPane*)m_Panes[AH_PANE_UNIT_COMMANDS])->ApplyFonts();
    if (m_Panes[AH_PANE_UNIT_COMMENTS]) ((CEditPane*)m_Panes[AH_PANE_UNIT_COMMENTS])->ApplyFonts();
    if (m_Panes[AH_PANE_MSG          ]) ((CEditPane*)m_Panes[AH_PANE_MSG          ])->ApplyFonts();

}

//-------------------------------------------------------------------------

void CAhApp::ApplyColors()
{
    if (m_Panes[AH_PANE_MAP          ]) ((CMapPane *)m_Panes[AH_PANE_MAP          ])->ApplyColors();
}

//-------------------------------------------------------------------------

void CAhApp::ApplyIcons()
{
    if (m_Panes[AH_PANE_MAP          ]) ((CMapPane *)m_Panes[AH_PANE_MAP          ])->ApplyIcons();
}
        
//-------------------------------------------------------------------------

void CAhApp::OpenOptionsDlg()
{
    int rc;

    COptionsDialog *dialog = new COptionsDialog(m_Frames[AH_FRAME_MAP]);
    {
        dialog->Init();
        rc = dialog->ShowModal();
        if (wxID_OK==rc)
        {
        }
        dialog->Done();
    }
    //dialog->Close(TRUE);
}

//-------------------------------------------------------------------------

void CAhApp::OpenMapFrame()
{
    if (!m_Frames[AH_FRAME_MAP])
    {
        m_Frames[AH_FRAME_MAP] = new CMapFrame(NULL, m_layout);
        m_Frames[AH_FRAME_MAP]->Init(m_layout, NULL);
        m_Frames[AH_FRAME_MAP]->Show(TRUE);
    }
    else
        m_Frames[AH_FRAME_MAP]->Raise();
}

//-------------------------------------------------------------------------

void CAhApp::OpenUnitFrame()
{
    if (!m_Frames[AH_FRAME_UNITS])
    {
        m_Frames[AH_FRAME_UNITS] = new CUnitFrame(m_Frames[AH_FRAME_MAP]);
        m_Frames[AH_FRAME_UNITS]->Init(m_layout, NULL);
        m_Frames[AH_FRAME_UNITS]->Show(TRUE);
    }
    else
        m_Frames[AH_FRAME_UNITS]->Raise();
}

//-------------------------------------------------------------------------

void CAhApp::OpenUnitFrameFltr(BOOL PopUpSettings)
{
    if (!m_Frames[AH_FRAME_UNITS_FLTR])
    {
        m_Frames[AH_FRAME_UNITS_FLTR] = new CUnitFrameFltr(m_Frames[AH_FRAME_MAP]);
        m_Frames[AH_FRAME_UNITS_FLTR]->Init(m_layout, NULL);
        m_Frames[AH_FRAME_UNITS_FLTR]->Show(TRUE);

        CUnitPaneFltr   * pUnitPaneF = (CUnitPaneFltr*)m_Panes [AH_PANE_UNITS_FILTER];
        wxCommandEvent    event;

        if (pUnitPaneF)
            if  (PopUpSettings)
                pUnitPaneF->OnPopupMenuFilter(event);
            else
                pUnitPaneF->Update(NULL);
    }
    else
        m_Frames[AH_FRAME_UNITS_FLTR]->Raise();


}

//-------------------------------------------------------------------------

void CAhApp::OpenMsgFrame()
{
    if (!m_Frames[AH_FRAME_MSG])
    {
        m_Frames[AH_FRAME_MSG] = new CMsgFrame(m_Frames[AH_FRAME_MAP]);
        m_Frames[AH_FRAME_MSG]->Init(m_layout, NULL);
        m_MsgSrc.Empty();
        m_Frames[AH_FRAME_MSG]->Show(TRUE);
    }
    else
        m_Frames[AH_FRAME_MSG]->Raise();
}

//-------------------------------------------------------------------------

void CAhApp::OpenEditsFrame()
{
    if (!m_Frames[AH_FRAME_EDITS])
    {
        m_Frames[AH_FRAME_EDITS] = new CEditsFrame(m_Frames[AH_FRAME_MAP]);
        m_Frames[AH_FRAME_EDITS]->Init(m_layout, NULL);
        m_Frames[AH_FRAME_EDITS]->Show(TRUE);
    }
    else
        m_Frames[AH_FRAME_EDITS]->Raise();
}

//-------------------------------------------------------------------------

void CAhApp::UpgradeConfigFiles()
{
    CStr         Section;
    const char * szNextSection;
    const char * szName;
    const char * szValue;
    BOOL         Ok = TRUE;
    int          fileno, idx, i;
    CStr         ConfigKey;

    // move the sections
    Ok = m_Config[CONFIG_FILE_CONFIG].GetNextSection("", szNextSection);
    while (Ok)
    {
        Section = szNextSection;
        fileno    = GetConfigFileNo(Section.GetData());

        if (CONFIG_FILE_CONFIG != fileno)
        {
            // move to the appropriate file
            idx = m_Config[CONFIG_FILE_CONFIG].GetFirstInSection(Section.GetData(), szName, szValue);
            while (idx>=0)
            {
                m_Config[fileno].SetByName(Section.GetData(), szName, szValue);
                idx = m_Config[CONFIG_FILE_CONFIG].GetNextInSection(idx, Section.GetData(), szName, szValue);
            }
            m_Config[CONFIG_FILE_CONFIG].RemoveSection(Section.GetData());

            // and it means land flags has to be moved, too
            m_UpgradeLandFlags = TRUE;
        }

        Ok = m_Config[CONFIG_FILE_CONFIG].GetNextSection(Section.GetData(), szNextSection);
    }

    // unit lists columns
    szValue = m_Config[CONFIG_FILE_CONFIG].GetByName(SZ_SECT_LIST_COL_CURRENT, SZ_KEY_LIS_COL_UNITS_HEX);
    if (!szValue || !*szValue)
    {
        MoveSectionEntries(CONFIG_FILE_CONFIG, SZ_SECT_UNITLIST_HDR     , SZ_SECT_LIST_COL_UNIT_DEF     );
        MoveSectionEntries(CONFIG_FILE_CONFIG, SZ_SECT_UNITLIST_HDR_FLTR, SZ_SECT_LIST_COL_UNIT_FLTR_DEF);

        SetConfig(SZ_SECT_LIST_COL_CURRENT  , SZ_KEY_LIS_COL_UNITS_HEX  ,     SZ_SECT_LIST_COL_UNIT_DEF);
        SetConfig(SZ_SECT_LIST_COL_CURRENT  , SZ_KEY_LIS_COL_UNITS_FILTER,    SZ_SECT_LIST_COL_UNIT_FLTR_DEF);
    }

    // unit filter
    Section.Empty();
    Section  << SZ_SECT_UNIT_FILTER << "Default";
    Ok = FALSE;
    for (i=0; i<UNIT_SIMPLE_FLTR_COUNT; i++)
    {
        ConfigKey.Format("%s%d", SZ_KEY_UNIT_FLTR_PROPERTY, i);
        szValue = SkipSpaces(gpApp->GetConfig(SZ_SECT_WND_UNITS_FLTR, ConfigKey.GetData()));
        if (szValue && *szValue)
        {
            gpApp->SetConfig(Section.GetData(),      ConfigKey.GetData(), szValue);
            gpApp->SetConfig(SZ_SECT_WND_UNITS_FLTR, ConfigKey.GetData(), "");
            Ok = TRUE;
        }

        ConfigKey.Format("%s%d", SZ_KEY_UNIT_FLTR_COMPARE , i);
        szValue = gpApp->GetConfig(SZ_SECT_WND_UNITS_FLTR, ConfigKey.GetData());
        if (szValue && *szValue)
        {
            gpApp->SetConfig(Section.GetData(),      ConfigKey.GetData(), szValue);
            gpApp->SetConfig(SZ_SECT_WND_UNITS_FLTR, ConfigKey.GetData(), "");
            Ok = TRUE;
        }

        ConfigKey.Format("%s%d", SZ_KEY_UNIT_FLTR_VALUE   , i);
        szValue = gpApp->GetConfig(SZ_SECT_WND_UNITS_FLTR, ConfigKey.GetData());
        if (szValue && *szValue)
        {
            gpApp->SetConfig(Section.GetData(),      ConfigKey.GetData(), szValue);
            gpApp->SetConfig(SZ_SECT_WND_UNITS_FLTR, ConfigKey.GetData(), "");
            Ok = TRUE;
        }
    }
    if (Ok)
        gpApp->SetConfig(SZ_SECT_WND_UNITS_FLTR, SZ_KEY_FLTR_SET, Section.GetData());

    // Arcadia III roads
    szValue = m_Config[CONFIG_FILE_CONFIG].GetByName(SZ_SECT_COLORS,  SZ_KEY_MAP_ROAD_OLD);
    if (szValue && *szValue)
    {
        m_Config[CONFIG_FILE_CONFIG].SetByName(SZ_SECT_COLORS, SZ_KEY_MAP_ROAD    , szValue);
        m_Config[CONFIG_FILE_CONFIG].SetByName(SZ_SECT_COLORS, SZ_KEY_MAP_ROAD_OLD, "");
    }
    szValue = m_Config[CONFIG_FILE_CONFIG].GetByName(SZ_SECT_COLORS,  SZ_KEY_MAP_ROAD_BAD_OLD);
    if (szValue && *szValue)
    {
        m_Config[CONFIG_FILE_CONFIG].SetByName(SZ_SECT_COLORS, SZ_KEY_MAP_ROAD_BAD    , szValue);
        m_Config[CONFIG_FILE_CONFIG].SetByName(SZ_SECT_COLORS, SZ_KEY_MAP_ROAD_BAD_OLD, "");
    }
}

//-------------------------------------------------------------------------

void CAhApp::MoveSectionEntries(int fileno, const char * src, const char * dest)
{
    const char * szName;
    const char * szValue;
    CBufColl     Names, Values;
    int          idx;

    idx = m_Config[fileno].GetFirstInSection(src, szName, szValue);
    while (idx>=0)
    {
        Names.Insert(strdup(szName));
        Values.Insert(strdup(szValue));
        idx = m_Config[fileno].GetNextInSection(idx, src, szName, szValue);
    }
    m_Config[fileno].RemoveSection(src);

    for (idx=0; idx<Values.Count(); idx++)
    {
        szName = (const char *)Names.At(idx);
        szValue= (const char *)Values.At(idx);
        m_Config[fileno].SetByName(dest, szName?szName:"", szValue?szValue:"");
    }

    Names.FreeAll();
    Values.FreeAll();
}

//-------------------------------------------------------------------------

void CAhApp::UpgradeConfigByFactionId()
{
    int          fileno, idx;
    CStr         S, Section, Key;
    const char * szName;
    const char * szValue;

    if (m_pAtlantis->m_CrntFactionId > 0)
    {
        // Upgrade order files
        ComposeConfigOrdersSection(Section, m_pAtlantis->m_CrntFactionId);
        fileno  = GetConfigFileNo(SZ_SECT_ORDERS);
        idx     = m_Config[fileno].GetFirstInSection(SZ_SECT_ORDERS, szName, szValue);
        while (idx>=0)
        {
            m_Config[fileno].SetByName(Section.GetData(), szName, szValue);
            idx = m_Config[fileno].GetNextInSection(idx, SZ_SECT_ORDERS, szName, szValue);
        }
        m_Config[fileno].RemoveSection(SZ_SECT_ORDERS);

        // Upgrade passwords
        S = GetConfig(SZ_SECT_COMMON, SZ_KEY_PWD_OLD);
        S.TrimRight(TRIM_ALL);
        if (!S.IsEmpty())
        {
            Key.Empty();
            Key << (long)m_pAtlantis->m_CrntFactionId;
            SetConfig(SZ_SECT_PASSWORDS, Key.GetData() , S.GetData() );
            SetConfig(SZ_SECT_COMMON   , SZ_KEY_PWD_OLD, (const char *)NULL);
        }
    }
}

//-------------------------------------------------------------------------

void CAhApp::ComposeConfigOrdersSection(CStr & Sect, int FactionId)
{
    Sect = SZ_SECT_ORDERS;
    Sect << "_" << (long)FactionId;
}

//-------------------------------------------------------------------------

int CAhApp::GetConfigFileNo(const char * szSection)
{
    int x;

    if (m_ConfigSectionsState.Search((void*)szSection, x) ||
        0==strnicmp(SZ_SECT_ORDERS, szSection, sizeof(SZ_SECT_ORDERS)-1) ) // orders section is composite starting from 2.1.6
        return CONFIG_FILE_STATE;
    else
        return CONFIG_FILE_CONFIG;
}

//-------------------------------------------------------------------------

const char * CAhApp::GetConfig(const char * szSection, const char * szName)
{
    const char * p;
    int          i;
    int          fileno = GetConfigFileNo(szSection);

    p = m_Config[fileno].GetByName(szSection, szName);
    if (NULL==p)
    {
        for (i=0; i<DefaultConfigSize; i++)
            if ( (0==stricmp(szSection, DefaultConfig[i].szSection)) &&
                 (0==stricmp(szName,    DefaultConfig[i].szName))  )
            {
                p = DefaultConfig[i].szValue;
                break;
            }
        m_Config[fileno].SetByName(szSection, szName, p?p:" ");
    }
    if (NULL==p)
        p = "";
    return p;
}

//-------------------------------------------------------------------------

void CAhApp::SetConfig(const char * szSection, const char * szName, const char * szNewValue)
{
    int  fileno = GetConfigFileNo(szSection);
    m_Config[fileno].SetByName(szSection, szName, szNewValue);
}

//-------------------------------------------------------------------------

void CAhApp::SetConfig(const char * szSection, const char * szName, long lNewValue)
{
    char   buf[64];
    int    fileno = GetConfigFileNo(szSection);

    sprintf(buf, "%ld", lNewValue);
    m_Config[fileno].SetByName(szSection, szName, buf);
}


//-------------------------------------------------------------------------

int  CAhApp::GetSectionFirst(const char * szSection, const char *& szName, const char *& szValue)
{
    int idx;
    int i;
    int fileno = GetConfigFileNo(szSection);

    idx = m_Config[fileno].GetFirstInSection(szSection, szName, szValue);
    if (idx < 0)
    {
        for (i=0; i<DefaultConfigSize; i++)
            if (0==stricmp(szSection, DefaultConfig[i].szSection))
                m_Config[fileno].SetByName(szSection, DefaultConfig[i].szName, DefaultConfig[i].szValue);

        idx = m_Config[fileno].GetFirstInSection(szSection, szName, szValue);
    }

    return idx;
}

//-------------------------------------------------------------------------

int  CAhApp::GetSectionNext (int idx, const char * szSection, const char *& szName, const char *& szValue)
{
    int   fileno = GetConfigFileNo(szSection);
    return m_Config[fileno].GetNextInSection (idx, szSection, szName, szValue);
}

//-------------------------------------------------------------------------

void  CAhApp::RemoveSection(const char * szSection)
{
    int fileno = GetConfigFileNo(szSection);
    m_Config[fileno].RemoveSection(szSection);
}

//-------------------------------------------------------------------------

const char * CAhApp::GetNextSectionName(int fileno, const char * szStart)
{
    const char * szNextSection = NULL;

    if (fileno!=CONFIG_FILE_STATE && fileno!=CONFIG_FILE_CONFIG)
        return NULL;

    m_Config[fileno].GetNextSection(szStart, szNextSection);

    return szNextSection;
}

//-------------------------------------------------------------------------

void CAhApp::ForgetFrame(int no, BOOL frameclosed)
{
    int i;

    if (m_Frames[no])
    {
        m_Frames[no]->Done(frameclosed);

        for (i=0; i<AH_PANE_COUNT; i++)
            if (m_Frames[no]->m_Panes[i])
                m_Panes[i] = NULL;

        m_Frames[no] = NULL;
    }
}


//-------------------------------------------------------------------------

void CAhApp::FrameClosing(CAhFrame * pFrame)
{
    int  no;

    if (pFrame)
        for (no=0; no<AH_FRAME_COUNT; no++)
            if (m_Frames[no] == pFrame)
            {
                if (AH_FRAME_MAP==no)
                {
                    // shutdown in progress
                    for (no=0; no<AH_FRAME_COUNT; no++)
                        ForgetFrame(no, FALSE);
                }
                else
                    ForgetFrame(no, TRUE);
                break;
            }
}

//-------------------------------------------------------------------------

void CAhApp::ShowError(const char * msg, int msglen, BOOL ignore_disabled)
{
    CEditPane * p;

    if (m_DisableErrs && !ignore_disabled)
       return;

    OpenMsgFrame();
    m_MsgSrc.AddStr(msg, msglen);

    p = (CEditPane*)m_Panes[AH_PANE_MSG];
    if (p)
        p->SetSource(&m_MsgSrc, NULL);
}

//-------------------------------------------------------------------------

long CAhApp::GetStructAttr(const char * kind, long & MaxLoad, long & MinSailingPower)
{
    const char * attrlist;
    const char * p;
    CStr         S, Name;
    long         attr = 0;

    MaxLoad         = 0;
    MinSailingPower = 0;

    attrlist = GetConfig(SZ_SECT_STRUCTS, ResolveAlias(kind));
    while (attrlist && *attrlist)
    {
        attrlist = S.GetToken(attrlist, ',', TRIM_ALL);
        if (S.IsEmpty())
            break;

        if      (0==stricmp(SZ_ATTR_STRUCT_MOBILE , S.GetData()))      attr |= SA_MOBILE ;
        else if (0==stricmp(SZ_ATTR_STRUCT_HIDDEN , S.GetData()))      attr |= SA_HIDDEN ;
        else if (0==stricmp(SZ_ATTR_STRUCT_SHAFT  , S.GetData()))      attr |= SA_SHAFT  ;
        else if (0==stricmp(SZ_ATTR_STRUCT_GATE   , S.GetData()))      attr |= SA_GATE   ;
        else if (0==stricmp(SZ_ATTR_STRUCT_ROAD_N , S.GetData()))      attr |= SA_ROAD_N ;
        else if (0==stricmp(SZ_ATTR_STRUCT_ROAD_NE, S.GetData()))      attr |= SA_ROAD_NE;
        else if (0==stricmp(SZ_ATTR_STRUCT_ROAD_SE, S.GetData()))      attr |= SA_ROAD_SE;
        else if (0==stricmp(SZ_ATTR_STRUCT_ROAD_S , S.GetData()))      attr |= SA_ROAD_S ;
        else if (0==stricmp(SZ_ATTR_STRUCT_ROAD_SW, S.GetData()))      attr |= SA_ROAD_SW;
        else if (0==stricmp(SZ_ATTR_STRUCT_ROAD_NW, S.GetData()))      attr |= SA_ROAD_NW;
        else
        {
            // Two-token attributes, MaxLoad & MinSailingPower.
            p = SkipSpaces(Name.GetToken(S.GetData(), ' ', TRIM_ALL));
            if      (0==stricmp(SZ_ATTR_STRUCT_MAX_LOAD, Name.GetData()))   MaxLoad         = atol(p);
            else if (0==stricmp(SZ_ATTR_STRUCT_MIN_SAIL, Name.GetData()))   MinSailingPower = atol(p);
        }

    }
    if (0 == stricmp(kind, STRUCT_GATE))
        attr |= SA_GATE; // to compensate for legacy missing gate flag in the config
    return attr;
}

//-------------------------------------------------------------------------

const char * CAhApp::ResolveAlias(const char * alias)
{
    const char * p;
    const char * p1;
    int          cnt = 0;

    p1 = alias;
    do
    {
        p = SkipSpaces(GetConfig(SZ_SECT_ALIAS, p1));
        if (p && *p)
            p1 = p;
        if (cnt++ > 20)  // don't play with recursy, man!
        {
            p1 = alias;
            break;
        }

    } while (p && *p);

    return p1;
}

//-------------------------------------------------------------------------

long CAhApp::GetStudyCost(const char * skill)
{
    long        n;
    const char *p;

    p = ResolveAlias(skill);
    n = atol(GetConfig(SZ_SECT_STUDY_COST, p));

    return n;
}

//-------------------------------------------------------------------------

BOOL CAhApp::GetItemWeights(const char * item, int *& weights, const char **& movenames, int & movecount )
{
    ItemWeights   Dummy;
    ItemWeights * pWeights;
    int           i;
    const char  * p;
    BOOL          Ok = TRUE;
    BOOL          Update = FALSE;


    Dummy.name = (char *)item;

    if (m_ItemWeights.Search(&Dummy, i))
        pWeights = (ItemWeights *)m_ItemWeights.At(i);
    else
    {
        CStr S;

        p = SkipSpaces(GetConfig(SZ_SECT_WEIGHT_MOVE, item));


        Ok = (p && *p);
        pWeights          = new ItemWeights;
        pWeights->name    = strdup(item);
        pWeights->weights = (int*)malloc(m_MoveModes.Count()*sizeof(int));


        for (i=0; i<m_MoveModes.Count(); i++)
        {
            p = SkipSpaces(S.GetToken(p, ','));
            if (i==4 && S.IsEmpty())
            {
                // Update swimming for 2.3.2
                int x;
                for (x=0; x<DefaultConfigSize; x++)
                    if ( (0==stricmp(SZ_SECT_WEIGHT_MOVE, DefaultConfig[x].szSection)) &&
                         (0==stricmp(item               , DefaultConfig[x].szName))  )
                    {
                        const char * q = DefaultConfig[x].szValue;
                        int          m;
                        for (m=0; m<=i; m++)
                            q = SkipSpaces(S.GetToken(q, ','));
                        Update = TRUE;
                        break;
                    }
            }
            pWeights->weights[i] = atoi(S.GetData());
        }
        if (Update && !IsASkillRelatedProperty(item))
        {
            // Update swimming for 2.3.2
            S.Empty();
            for (i=0; i<m_MoveModes.Count(); i++)
            {
                if (i>0)
                    S << ',';
                S << (long)pWeights->weights[i];
            }
            SetConfig(SZ_SECT_WEIGHT_MOVE, item, S.GetData());
        }
        m_ItemWeights.Insert(pWeights);
    }

    weights   = pWeights->weights;
    movenames = (const char **)m_MoveModes.GetItems();
    movecount = m_MoveModes.Count();

    if (!Ok)
    {
        CStr S;

        if (!IsASkillRelatedProperty(item))
        {
            S.Empty();
            S << "Warning! Weight and capacities for " << item <<
                 " are unknown and assumed to be zero. Movement modes can not be calculated correct. Update your " <<
                 SZ_CONFIG_FILE << " file!" <<EOL_SCR;
            ShowError(S.GetData(), S.GetLength(), TRUE);
        }
    }

    return Ok;
}


//-------------------------------------------------------------------------

void CAhApp::GetMoveNames(const char **& movenames)
{
    movenames = (const char **)m_MoveModes.GetItems();
}

//-------------------------------------------------------------------------

BOOL CAhApp::GetOrderId(const char * order, long & id)
{
    const void * data = NULL;
    BOOL  Ok;

    Ok = m_OrderHash.Locate(order, data);
    id = (long)data;

    return Ok;
}

//-------------------------------------------------------------------------

BOOL CAhApp::IsTradeItem(const char * item)
{
    const void * data = NULL;

    return m_TradeItemsHash.Locate(item, data);
}

//-------------------------------------------------------------------------

BOOL CAhApp::IsMan(const char * item)
{
    const void * data = NULL;

    return m_MenHash.Locate(item, data);
}

//-------------------------------------------------------------------------

BOOL CAhApp::IsMagicSkill(const char * skill)
{
    const void * data = NULL;

    return m_MagicSkillsHash.Locate(skill, data);
}

//-------------------------------------------------------------------------

const char * CAhApp::GetWeatherLine(BOOL IsCurrent, BOOL IsGood, int Zone)
{
    const char * szKey = NULL;

    if (IsCurrent)
        if (IsGood)
            if (0==Zone) //Tropic
                szKey = SZ_KEY_WEATHER_CUR_GOOD_TROPIC;
            else
                szKey = SZ_KEY_WEATHER_CUR_GOOD_MEDIUM;
        else
            if (0==Zone) //Tropic
                szKey = SZ_KEY_WEATHER_CUR_BAD_TROPIC;
            else
                szKey = SZ_KEY_WEATHER_CUR_BAD_MEDIUM;
    else
        if (IsGood)
            if (0==Zone) //Tropic
                szKey = SZ_KEY_WEATHER_NEXT_GOOD_TROPIC;
            else
                szKey = SZ_KEY_WEATHER_NEXT_GOOD_MEDIUM;
        else
            if (0==Zone) //Tropic
                szKey = SZ_KEY_WEATHER_NEXT_BAD_TROPIC;
            else
                szKey = SZ_KEY_WEATHER_NEXT_BAD_MEDIUM;

    return GetConfig(SZ_SECT_WEATHER, szKey);
}

//-------------------------------------------------------------------------

long CAhApp::GetMaxRaceSkillLevel(const char * race, const char * skill, const char * leadership, BOOL IsArcadiaSkillSystem)
{
    // we will cache it a bit...
    long  level    = 0;
    long  maxlevel = 0;
    CStr  sKey;
    CStr  sVal, S;
    const char * p;

    if (!leadership)
        leadership = "";
    sKey << race << ":" << leadership << ":" << skill;

    if (!m_MaxSkillHash.Locate(sKey.GetData(), (const void *&)level))
    {
        sVal = GetConfig(SZ_SECT_MAX_SKILL_LVL, race);
        p = sVal.GetData();

        p = S.GetToken(p, ',', TRIM_ALL);
        maxlevel = atol(S.GetData());

        p = S.GetToken(p, ',', TRIM_ALL);
        level = atol(S.GetData());

        while (p && *p)
        {
            p = S.GetToken(p, ',', TRIM_ALL);
            if (0==stricmp(skill, S.GetData()))
            {
                level = maxlevel;
                break;
            }
        }

        if (IsArcadiaSkillSystem && *leadership)
        {
            if ( IsMagicSkill(skill))
            {
                if ( 0==stricmp(leadership, SZ_HERO))
                {
                    // reread magic skill
                    sVal = GetConfig(SZ_SECT_MAX_MAG_SKILL_LVL, race);
                    p = sVal.GetData();

                    p = S.GetToken(p, ',', TRIM_ALL);
                    maxlevel = atol(S.GetData());

                    p = S.GetToken(p, ',', TRIM_ALL);
                    level = atol(S.GetData());

                    while (p && *p)
                    {
                        p = S.GetToken(p, ',', TRIM_ALL);
                        if (0==stricmp(skill, S.GetData()))
                        {
                            level = maxlevel;
                            break;
                        }
                    }
                }
                else
                    level = 0;
            }
            else
            {
                // adjust for leadership
                int leader_bonus, hero_bonus, bonus=0;

                sVal = GetConfig(SZ_SECT_COMMON, SZ_KEY_LEAD_SKILL_BONUS);
                p = sVal.GetData();

                p = S.GetToken(p, ',', TRIM_ALL);
                leader_bonus = atol(S.GetData());

                p = S.GetToken(p, ',', TRIM_ALL);
                hero_bonus = atol(S.GetData());

                if (0==stricmp(leadership, SZ_LEADER))
                    bonus = leader_bonus;
                else
                    if (0==stricmp(leadership, SZ_HERO))
                        bonus = hero_bonus;
                level += bonus;
            }
        }

        m_MaxSkillHash.Insert(sKey.GetData(), (void*)level);
    }

    return level;
}

//-------------------------------------------------------------------------

void CAhApp::GetProdDetails (const char * item, TProdDetails & details)
{
    CStr sVal, S;
    const char * p;
    int x;

    details.Empty();
    sVal = GetConfig(SZ_SECT_PROD_SKILL, item);
    if (!sVal.IsEmpty())
    {
        S = details.skillname.GetToken(sVal.GetData(), ' ', TRIM_ALL);
        details.skilllevel = atol(S.GetData());
    }

    sVal = GetConfig(SZ_SECT_PROD_RESOURCE, item);
    x = 0;
    p = sVal.GetData();
    while (p && *p && x<MAX_RES_NUM)
    {
        p = details.resname[x].GetToken(SkipSpaces(p), ' ', TRIM_ALL);
        p = S.GetToken(p, ',', TRIM_ALL);
        details.resamt[x] = atol(S.GetData());
        x++;
    }

    sVal = GetConfig(SZ_SECT_PROD_MONTHS, item);
    if (!sVal.IsEmpty())
        details.months = atol(sVal.GetData());

    sVal = GetConfig(SZ_SECT_PROD_TOOL, item);
    if (!sVal.IsEmpty())
    {
        S = details.toolname.GetToken(sVal.GetData(), ' ', TRIM_ALL);
        details.toolhelp = atol(S.GetData());
    }

}

//-------------------------------------------------------------------------

BOOL CAhApp::CanSeeAdvResources(const char * skillname, const char * terrain, CLongColl & Levels, CBufColl & Resources)
{
    CStr         ProdSkillLine;
    CStr         ProdLandLine;
    BOOL         Ok = FALSE;
    const char * p1, * p2, *p;
    CStr         Prod1, Prod2, S1;
    long         level;

    Levels.FreeAll();
    Resources.FreeAll();

    ProdSkillLine = GetConfig(SZ_SECT_RESOURCE_SKILL,  skillname);
    ProdSkillLine.TrimRight(TRIM_ALL);

    ProdLandLine = GetConfig(SZ_SECT_RESOURCE_LAND,  terrain);
    ProdLandLine.TrimRight(TRIM_ALL);

    if (!ProdSkillLine.IsEmpty() && !ProdLandLine.IsEmpty())
    {
        p1 = SkipSpaces(S1.GetToken(ProdSkillLine.GetData(), ',', TRIM_ALL));
        while (!S1.IsEmpty())
        {
            p  = SkipSpaces(Prod1.GetToken(S1.GetData(), ' ', TRIM_ALL));
            level = p ? atol(p) : 0;

            p2 = SkipSpaces(Prod2.GetToken(ProdLandLine.GetData(), ',', TRIM_ALL));
            while (!Prod2.IsEmpty())
            {
                if (0==stricmp(Prod1.GetData(), Prod2.GetData()))
                {
                    Ok = TRUE;
                    Levels.Insert((void*)level);
                    Resources.Insert(strdup(Prod1.GetData()));
                    break;
                }
                p2 = SkipSpaces(Prod2.GetToken(p2, ',', TRIM_ALL));
            }

            p1 = SkipSpaces(S1.GetToken(p1, ',', TRIM_ALL));
        }
    }

    return Ok;
}


//-------------------------------------------------------------------------

int CAhApp::GetAttitudeForFaction(int id)
{
    int player_id = atol( GetConfig(SZ_SECT_ATTITUDES, SZ_ATT_PLAYER_ID));
    if(id == player_id) return ATT_FRIEND2;
    int attitude = ATT_UNDECLARED;
    CAttitude * policy;
    for(int i=m_Attitudes.Count(); i>=0; i--)
    {
        policy = (CAttitude *) m_Attitudes.At(i);
        if(policy && (policy->FactionId == id)) attitude=policy->Stance;
    }
    if(attitude == ATT_UNDECLARED)
    {
        // check for default attitude
        for(int i=m_Attitudes.Count(); i>=0; i--)
        {
            policy = (CAttitude *) m_Attitudes.At(i);
            if(policy && (policy->FactionId == 0)) attitude=policy->Stance;
        }
    }
    return attitude;
}

//-------------------------------------------------------------------------
void CAhApp::SetAttitudeForFaction(int id, int attitude)
{
    int att_idx = -1;
    CAttitude * policy;
    if((attitude < ATT_FRIEND1) || (attitude >= ATT_UNDECLARED)) return;
    for(int i=m_Attitudes.Count(); i>=0; i--)
    {
        policy = (CAttitude *) m_Attitudes.At(i);
        if(policy && (policy->FactionId == id)) att_idx=i;
    }
    if(att_idx < 0)
    {   // new attitude declaration
        policy = new CAttitude;
        policy->FactionId = id;
        policy->SetStance(attitude);
        m_Attitudes.Insert(policy);
    }
    else
    {   // change existing declaration
        policy = (CAttitude *) m_Attitudes.At(att_idx);
        policy->SetStance(attitude);
    }
}

//-------------------------------------------------------------------------

void CAhApp::GetShortFactName(CStr & S, int FactionId)
{
#define MAX_F_NAME 8
    int           i;
    char          ch;
    CFaction    * pFaction;
//    CBaseObject   Dummy;
//    int           idx;

    S.Empty();
//    Dummy.Id = FactionId;
//    if (m_pAtlantis->m_Factions.Search(&Dummy, idx))
//    {
//        pFaction = (CFaction*)m_pAtlantis->m_Factions.At(idx);
//        S = pFaction->Name;
//    }
//    else
//        S << (long)FactionId;
    pFaction = m_pAtlantis->GetFaction(FactionId);
    if (pFaction)
        S = pFaction->Name;
    else
        S << (long)FactionId;

    if (0==stricmp(S.GetData(), "faction"))
    {
        S.Empty();
        S << "F_" << (long)FactionId << "_";
    }

    S.ToLower();
    for (i=S.GetLength()-1; i>=0; i--)
    {
        ch = S.GetData()[i];
        if ( (ch < 'a' || ch > 'z') && (ch < '0' || ch > '9') )
            S.DelCh(i);
    }
    if (S.GetLength() > MAX_F_NAME)
        S.DelSubStr(MAX_F_NAME, S.GetLength()-MAX_F_NAME);
    S.TrimRight(TRIM_ALL);

}

//-------------------------------------------------------------------------

void CAhApp::SetMapFrameTitle()
{
    CMapFrame   * pMapFrame  = (CMapFrame *)m_Frames[AH_FRAME_MAP];
    CMapPane    * pMapPane   = (CMapPane  * )m_Panes[AH_PANE_MAP];
    CPlane      * pPlane     = NULL;

    CStr          S;

    S = m_sTitle;

    if (pMapPane)
    {
        pPlane = (CPlane*)m_pAtlantis->m_Planes.At(pMapPane->m_SelPlane);

        S << " (" << pMapPane->m_SelHexX << "," << pMapPane->m_SelHexY;
        if (pPlane && 0!=stricmp(DEFAULT_PLANE, pPlane->Name.GetData()))
        {
            S << "," << pPlane->Name;
        }
        S << ")";
    }

    if (m_OrdersAreChanged)
        S << " [modified]";
    if (pMapFrame)
        pMapFrame->SetTitle(wxString::FromAscii(S.GetData()));
}

//-------------------------------------------------------------------------

void CAhApp::SetOrdersChanged(BOOL Changed)
{
    m_OrdersAreChanged = Changed;

    SetMapFrameTitle();
}


//-------------------------------------------------------------------------

int CAhApp::SaveOrders(BOOL UsingExistingName)
{
    CStr S, FName, Section;
    int  i, id, err=ERR_OK;

    for (i=0; i<m_pAtlantis->m_OurFactions.Count(); i++)
    {
        id = (long)m_pAtlantis->m_OurFactions.At(i);
        if (UsingExistingName)
        {
            ComposeConfigOrdersSection(Section, id);
            S.Empty();
            S << (long)m_pAtlantis->m_YearMon;
            FName = GetConfig(Section.GetData(), S.GetData());
            FName.TrimRight(TRIM_ALL);
        }
        err = SaveOrders(FName.GetData(), id);
        if (ERR_OK!=err)
            break;
    }

    if (ERR_OK==err)
        SetOrdersChanged(FALSE);

    return err;
}

//-------------------------------------------------------------------------

int  CAhApp::SaveOrders(const char * FNameOut, int FactionId)
{

    int         err;
    char        buf[64];
    CStr        FName;
    CStr        Dir;
    CStr        S, Section, Prompt, Key;
    CFaction  * pFaction;

    FName = FNameOut;
    FName.TrimRight(TRIM_ALL);

    ComposeConfigOrdersSection(Section, FactionId);
    if (FName.IsEmpty())
    {
        S.Format("%d", m_pAtlantis->m_YearMon);
        FName = GetConfig(Section.GetData(), S.GetData());
        FName.TrimRight(TRIM_ALL);

        if (FName.IsEmpty())
        {
            GetShortFactName(S, FactionId);
            if (S.IsEmpty())
                S << (long)FactionId;
            FName.Format("%s%04d.ord", S.GetData(), m_pAtlantis->m_YearMon);
        }
        pFaction = m_pAtlantis->GetFaction(FactionId);

        Prompt = "Save orders for ";
        if (pFaction)
            Prompt << pFaction->Name.GetData() << " ";
        else
            Prompt << "Faction ";
        Prompt << (long)FactionId;

        Dir = GetConfig(SZ_SECT_FOLDERS, SZ_KEY_FOLDER_ORDERS);
        Dir.TrimRight(TRIM_ALL);
        if (Dir.IsEmpty())
            Dir = ".";

        CStr File;
        wxString CurrentDir = wxGetCwd();
        //MakePathFull(CurrentDir.mb_str(), FName);
        GetFileFromPath(FName.GetData(), File);

        MakePathFull(CurrentDir.mb_str(), Dir);
        wxFileDialog dialog((CMapFrame*)m_Frames[AH_FRAME_MAP],
                            wxString::FromAscii(Prompt.GetData()),
                            wxString::FromAscii(Dir.GetData()),
                            wxString::FromAscii(File.GetData()),
                            wxT(SZ_ORD_FILES),
                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
        err = dialog.ShowModal();
        wxSetWorkingDirectory(CurrentDir);

        if (wxID_OK == err)
        {
            FName = dialog.GetPath().mb_str();
            MakePathRelative(CurrentDir.mb_str(), FName);
            GetDirFromPath(FName.GetData(), Dir);
            SetConfig(SZ_SECT_FOLDERS, SZ_KEY_FOLDER_ORDERS, Dir.GetData() );
        }
        else
            return ERR_CANCEL;

        FName.TrimRight(TRIM_ALL);
    }
    if (FName.IsEmpty())
        return ERR_FNAME;

    Key.Empty();
    Key << (long)FactionId;

    err = m_pAtlantis->SaveOrders(FName.GetData(),
                                  GetConfig(SZ_SECT_PASSWORDS, Key.GetData()),
                                  (BOOL)atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_DECORATE_ORDERS)),
                                  FactionId
                                 );
    if (ERR_OK==err)
    {
        sprintf(buf, "%ld", m_pAtlantis->m_YearMon);
        SetConfig(Section.GetData(), buf, FName.GetData());
    }

    // Save config, too
    m_Config[CONFIG_FILE_CONFIG].Save(SZ_CONFIG_FILE);
    m_Config[CONFIG_FILE_STATE ].Save(SZ_CONFIG_STATE_FILE);

    if (ERR_OK==m_pAtlantis->m_ParseErr)
        SaveHistory(SZ_HISTORY_FILE);

    return err;
}

//-------------------------------------------------------------------------

void CAhApp::RedrawTracks()
{
    CUnit       * pUnit = GetSelectedUnit();
    CPlane      * pPlane;
    CMapPane    * pMapPane  = (CMapPane* )m_Panes[AH_PANE_MAP];

    if (!pMapPane)
        return;

    pPlane   = (CPlane*)m_pAtlantis->m_Planes.At(pMapPane->m_SelPlane);
    pMapPane->RedrawTracksForUnit(pPlane, pUnit, NULL, TRUE);
}

//-------------------------------------------------------------------------

CUnit * CAhApp::GetSelectedUnit()
{
    CUnit       * pUnit = NULL;
    CUnitPane   * pUnitPane = (CUnitPane*)m_Panes[AH_PANE_UNITS_HEX];

    if (pUnitPane)
        pUnit = (CUnit*)pUnitPane->m_pUnits->At(m_SelUnitIdx);

    return pUnit;
}

//-------------------------------------------------------------------------

int  CAhApp::LoadOrders  (const char * FNameIn)
{
    int           err;
    CStr          S(32), FName, Sect;
    int           factid;
//    CMapPane    * pMapPane = (CMapPane* )m_Panes[AH_PANE_MAP];


    FName = FNameIn;  // FNameIn can be coming from config, so do not use it directly!
    err = m_pAtlantis->LoadOrders(FName.GetData(), factid);
    if (ERR_OK==err)
    {
        S.Empty();
        S << (long)m_pAtlantis->m_YearMon;
        ComposeConfigOrdersSection(Sect, factid);
        SetConfig(Sect.GetData(), S.GetData(), FName.GetData());

//        if (pMapPane)
//            pMapPane->Refresh(FALSE, NULL);
//            pMapPane->CleanCities(); //pMapPane->Refresh(FALSE, NULL); // to remove pointers to land wich could be replaced by joining orders

        OnMapSelectionChange();
        RedrawTracks();
    }
    else
       if (m_Frames[AH_FRAME_MSG])
           ((CAhFrame*)m_Frames[AH_FRAME_MSG])->Raise();

    return err;
}

//-------------------------------------------------------------------------

void EncodeConfigLine(CStr & dest, const char * src)
{
    dest.Empty();
    while (src && *src)
    {
        switch (*src)
        {
        case '\r':  break;
        case '\n':  dest.AddStr("\\n", 2);
                    break;
        default  :  dest.AddCh(*src);
        }
        src++;
    }
}

//-------------------------------------------------------------------------

void DecodeConfigLine(CStr & dest, const char * src)
{
    BOOL          Esc;

    dest.Empty();
    Esc = FALSE;
    while (src && *src)
    {
        if ('\\' == *src)


            Esc = TRUE;
        else
        {
            if (Esc)
            {
                switch(*src)
                {
                case 'n':
                    dest << EOL_SCR;
                    break;
                default:
                    dest.AddCh('\\');
                    dest.AddCh(*src);
                }
            }
            else
                dest.AddCh(*src);
            Esc = FALSE;
        }

        src++;
    }
}

//-------------------------------------------------------------------------

void CAhApp::LoadComments()
{
    int           i;
    CUnit       * pUnit;
    char          buf[32];
    CStr          S;

    for (i=0; i<m_pAtlantis->m_Units.Count(); i++)
    {
        pUnit = (CUnit*)m_pAtlantis->m_Units.At(i);
        sprintf(buf, "%ld", pUnit->Id);

        DecodeConfigLine(pUnit->DefOrders, GetConfig(SZ_SECT_DEF_ORDERS, buf));

        pUnit->DefOrders.TrimRight(TRIM_ALL);
        pUnit->ExtractCommentsFromDefOrders();
    }
    m_CommentsChanged = FALSE;
}

//-------------------------------------------------------------------------

void CAhApp::SaveComments()
{
    int           i;
    CUnit       * pUnit;
    char          buf[32];
    CStr          S;
    const char  * p;

    for (i=0; i<m_pAtlantis->m_Units.Count(); i++)
    {
        S.Empty();
        pUnit = (CUnit*)m_pAtlantis->m_Units.At(i);
        pUnit->DefOrders.TrimRight(TRIM_ALL);
        if (pUnit->DefOrders.GetLength() > 0)
        {
            EncodeConfigLine(S, pUnit->DefOrders.GetData());
            p = S.GetData();
        }
        else
            p = NULL;
        sprintf(buf, "%ld", pUnit->Id);
        SetConfig(SZ_SECT_DEF_ORDERS, buf, p);
    }
    m_CommentsChanged = FALSE;
}


//-------------------------------------------------------------------------

void CAhApp::LoadUnitFlags()
{
    int           i, x;
    CUnit       * pUnit;
    char          buf[32];
    CStr          S;

    for (i=0; i<m_pAtlantis->m_Units.Count(); i++)
    {
        pUnit = (CUnit*)m_pAtlantis->m_Units.At(i);
        sprintf(buf, "%ld", pUnit->Id);

        x = atol(GetConfig(SZ_SECT_UNIT_FLAGS, buf));
        if (x & UNIT_CUSTOM_FLAG_MASK)
        {
            pUnit->Flags    |= (x & UNIT_CUSTOM_FLAG_MASK);
            pUnit->FlagsOrg |= (x & UNIT_CUSTOM_FLAG_MASK);
            pUnit->FlagsLast = ~pUnit->Flags;
        }
    }
}

//-------------------------------------------------------------------------

void CAhApp::SaveUnitFlags()
{
    int           i;
    CUnit       * pUnit;
    char          buf[32];
    CStr          S;

    for (i=0; i<m_pAtlantis->m_Units.Count(); i++)
    {
        pUnit = (CUnit*)m_pAtlantis->m_Units.At(i);
        sprintf(buf, "%ld", pUnit->Id);

        S.Empty();
        if (pUnit->Flags & UNIT_CUSTOM_FLAG_MASK)
            S << (long)(pUnit->Flags & UNIT_CUSTOM_FLAG_MASK);
        SetConfig(SZ_SECT_UNIT_FLAGS, buf, S.GetData());
    }
}

//-------------------------------------------------------------------------

void CAhApp::SetAllLandUnitFlags()
{
    CUnitPane  * pUnitPane = (CUnitPane*)m_Panes[AH_PANE_UNITS_HEX];
    CPlane     * pPlane;
    CLand      * pLand;
    CUnit      * pUnit;
    int          i, n, f, x;
    int          rc;

    CUnitFlagsDlg dlg(m_Frames[AH_FRAME_MAP], eAll, 0);

    rc = dlg.ShowModal();

    if ((ID_BTN_SET_ALL_LAND==rc || ID_BTN_RMV_ALL_LAND==rc) && dlg.m_LandFlags>0)
    {
        for (n=0; n<m_pAtlantis->m_Planes.Count(); n++)
        {
            pPlane = (CPlane*)m_pAtlantis->m_Planes.At(n);
            for (i=0; i<pPlane->Lands.Count(); i++)
            {
                pLand = (CLand*)pPlane->Lands.At(i);
                x     = 1;
                for (f=0; f<LAND_FLAG_COUNT; f++)
                {
                    if (dlg.m_LandFlags & x)
                    {
                        if (ID_BTN_RMV_ALL_LAND==rc)
                        {
                            // clear flag
                            pLand->FlagText[f].Empty();
                        }
                        else
                        {
                            // set flag
                            if (pLand->FlagText[f].IsEmpty())
                                pLand->FlagText[f] = LandFlagLabel[f];
                        }
                        pLand->Flags |= LAND_HAS_FLAGS;
                    }
                    x <<= 1;
                }
            }
        }

        if (m_Panes[AH_PANE_MAP])
            (m_Panes[AH_PANE_MAP])->Refresh(FALSE);
    }

    if ( (ID_BTN_SET_ALL_UNIT==rc || ID_BTN_RMV_ALL_UNIT==rc) && dlg.m_UnitFlags>0 )
    {
        for (i=0; i<m_pAtlantis->m_Units.Count(); i++)
        {
            pUnit = (CUnit*)m_pAtlantis->m_Units.At(i);

            if (ID_BTN_SET_ALL_UNIT==rc)
            {
                pUnit->Flags    |= (dlg.m_UnitFlags & UNIT_CUSTOM_FLAG_MASK);
                pUnit->FlagsOrg |= (dlg.m_UnitFlags & UNIT_CUSTOM_FLAG_MASK);
            }
            else
            {
                pUnit->Flags    &= ~(dlg.m_UnitFlags & UNIT_CUSTOM_FLAG_MASK);
                pUnit->FlagsOrg &= ~(dlg.m_UnitFlags & UNIT_CUSTOM_FLAG_MASK);
            }

            pUnit->FlagsLast = ~pUnit->Flags;
        }
        if (pUnitPane)
            pUnitPane->Update(pUnitPane->m_pCurLand);
    }
}

//-------------------------------------------------------------------------

void CAhApp::SaveLandFlags()
{
    int          i, n, f;
    CPlane     * pPlane;
    CLand      * pLand;
    CStr         sName;
    CStr         sData;
    long         ym_last;
    long         ym_first;
    const char * p;

    for (n=0; n<m_pAtlantis->m_Planes.Count(); n++)
    {
        pPlane = (CPlane*)m_pAtlantis->m_Planes.At(n);
        for (i=0; i<pPlane->Lands.Count(); i++)
        {
            pLand = (CLand*)pPlane->Lands.At(i);
            sData.Empty();
            for (f=0; f<LAND_FLAG_COUNT; f++)
            {
                pLand->FlagText[f].TrimRight(TRIM_ALL);
                if (!pLand->FlagText[f].IsEmpty())
                {
                    if (!sData.IsEmpty())
                        sData << "\\n";
                    sData << (long)f << ":" << pLand->FlagText[f];
                }
            }
            m_pAtlantis->ComposeLandStrCoord(pLand, sName);


            if (!sData.IsEmpty() || (pLand->Flags & LAND_HAS_FLAGS)) // allow to remove flags
                SetConfig(SZ_SECT_LAND_FLAGS, sName.GetData(), sData.GetData());

            if (pLand->Flags&LAND_IS_CURRENT) //LAND_UNITS)
            {
                //ym = atol(GetConfig(SZ_SECT_LAND_VISITED, sName.GetData()));
                p        = sData.GetToken(GetConfig(SZ_SECT_LAND_VISITED, sName.GetData()), ',');
                ym_last  = atol(sData.GetData());
                if (sData.IsEmpty())
                    ym_first = m_pAtlantis->m_YearMon;
                else
                {
                    p        = sData.GetToken(SkipSpaces(p), ',');
                    ym_first = atol(sData.GetData());
                }
                if (ym_last < m_pAtlantis->m_YearMon)
                {
                    sData.Empty();
                    sData << m_pAtlantis->m_YearMon << "," << ym_first;
                    SetConfig(SZ_SECT_LAND_VISITED, sName.GetData(), sData.GetData());
                }
            }
        }
    }

//    m_LandFlagsChanged = FALSE;
}

//-------------------------------------------------------------------------

void CAhApp::LoadLandFlags()
{
    int               sectidx, n;
    const char      * szName;
    const char      * szValue;
    const char      * p;
    const char      * line;
    CStr              sData, sLine, sN;
    CLand           * pLand;

    sectidx = GetSectionFirst(SZ_SECT_LAND_FLAGS, szName, szValue);
    while (sectidx >= 0)
    {
        pLand   = m_pAtlantis->GetLand(szName);
        if (pLand)
        {
            DecodeConfigLine(sData, szValue);

            line = sData.GetData();
            while (line && *line)
            {
                line = sLine.GetToken(line, '\n');
                p    = sLine.GetData();
                p    = sN.GetToken(p, ':');
                if (p)
                    n = atoi(sN.GetData());
                else
                {
                    p = sN.GetData();
                    n = 0;
                }
                if (n<0 || n>=LAND_FLAG_COUNT)
                    n = 0;
                pLand->FlagText[n] = p;
                pLand->Flags |= LAND_HAS_FLAGS;
            }

        }
        sectidx = GetSectionNext(sectidx, SZ_SECT_LAND_FLAGS, szName, szValue);
    }


}


//-------------------------------------------------------------------------

void CAhApp::UpdateEdgeStructs()
{
    int          i, n, k;
    int          d, adj_dir;
    int          x, y, z;
    int          adj_index;
    CPlane     * pPlane;
    CLand      * pLand, * adj_land;
    CStruct    * pEdge;

    for (n=0; n<m_pAtlantis->m_Planes.Count(); n++)
    {
        pPlane = (CPlane*)m_pAtlantis->m_Planes.At(n);
        for (i=0; i<pPlane->Lands.Count(); i++)
        {
            pLand = (CLand*)pPlane->Lands.At(i);
            if(!pLand) continue;
            // set the Water-Type flag
            if(m_WaterTerrainNames.Search((void *) pLand->TerrainType.ToLower(), k))
            {
                pLand->Flags |= LAND_IS_WATER;
            }
            for(d=0; d<6; d++)
            {
                adj_dir = (d%6)-3;
                if(adj_dir < 0) adj_dir += 6;
                LandIdToCoord(pLand->Id,x,y,z);
                switch (d%6)
                {
                    case North     : y -= 2;     break;
                    case Northeast : y--; x++;   break;
                    case Southeast : y++; x++;   break;
                    case South     : y += 2;     break;
                    case Southwest : y++; x--;   break;
                    case Northwest : y--; x--;   break;
                }
                CBaseObject Dummy;
                Dummy.Id = LandCoordToId(x, y, z);
                if (pPlane->Lands.Search(&Dummy, adj_index))
                {
                    adj_land = (CLand *) pPlane->Lands.At(adj_index);
                    if(adj_land)
                    {
                        if((pLand->Flags&LAND_IS_CURRENT) && !(adj_land->Flags&LAND_IS_CURRENT))
                        {  // set the corresponding Edge Structure in adjacent region
                            adj_land->RemoveEdgeStructs(adj_dir);
                            for(k=pLand->EdgeStructs.Count(); k>=0; k--)
                            {
                                pEdge = (CStruct*) pLand->EdgeStructs.At(k);
                                if((pEdge != NULL) && (pEdge->Location == d))                          adj_land->AddNewEdgeStruct(pEdge->Kind.GetData(), adj_dir);
                            }
                        }
                        // set CoastBits
                        if(m_WaterTerrainNames.Search((void *) adj_land->TerrainType.ToLower(), k))
                        {
                            if(!(pLand->Flags & LAND_IS_WATER))
                                adj_land->CoastBits |= ExitFlags[adj_dir];
                        }
                        else if(pLand->Flags&LAND_IS_WATER)
                        {
                            pLand->CoastBits |= ExitFlags[d];
                        }
                    }
                }
            }
        }
    }
}


//-------------------------------------------------------------------------

void CAhApp::WriteMagesCSV()
{
    CStr FName;
    CStr S;


//    GetShortFactName(S);
//    FName.Format("%s_%s%04d.csv", S.GetData(), "mages", m_pAtlantis->m_YearMon);
    FName.Format("%s%04d.csv", "mages", m_pAtlantis->m_YearMon);

    CExportMagesCSVDlg Dlg(m_Frames[AH_FRAME_MAP], FName.GetData());
    if (wxID_OK == Dlg.ShowModal())
        m_pAtlantis->WriteMagesCSV(Dlg.m_pFileName->GetValue().mb_str(),
                                   0==SafeCmp(Dlg.m_pOrientation->GetValue().mb_str(), SZ_VERTICAL),
                                   Dlg.m_pSeparator->GetValue().mb_str(),
                                   Dlg.m_nFormat
                                  );
}

//-------------------------------------------------------------------------

void CAhApp::CheckTaxDetails  (CLand  * pLand, CTaxProdDetailsCollByFaction & TaxDetails)
{
    int               x;
    CUnit           * pUnit;
//    long              tax = pLand->Taxable;
    EValueType        type;
    long              men;
    CStr              sCoord;
    CTaxProdDetails * pDetail;
    CTaxProdDetails   Dummy;
    int               idx;
    CTaxProdDetailsCollByFaction Factions;
    CStr              OneLine(32);

    for (x=0; x<pLand->Units.Count(); x++)
    {
        pUnit = (CUnit*)pLand->Units.At(x);
        if (pUnit->Flags & UNIT_FLAG_TAXING)
        {
            Dummy.FactionId = pUnit->FactionId;
            if (TaxDetails.Search(&Dummy, idx))
                pDetail = (CTaxProdDetails*)TaxDetails.At(idx);
            else
            {
                pDetail = new CTaxProdDetails;
                pDetail->FactionId = pUnit->FactionId;
                TaxDetails.Insert(pDetail);
            }
            if (Factions.Insert(pDetail))
            {
                pDetail->amount = pLand->Taxable;
                pDetail->HexCount++;
            }
            if (pUnit->GetProperty(PRP_MEN, type, (const void *&)men, eNormal) && eLong==type)
                pDetail->amount -= men*atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_TAX_PER_TAXER));
        }
    }


    // Output
    for (x=0; x<Factions.Count(); x++)
    {
        pDetail = (CTaxProdDetails*)Factions.At(x);
        OneLine.Empty();

        m_pAtlantis->ComposeLandStrCoord(pLand, sCoord);
        OneLine << pLand->TerrainType << " (" << sCoord << ") ";
        while (OneLine.GetLength() < 24)
            OneLine.AddCh(' ');

        if (pDetail->amount > 0)
            OneLine << "is undertaxed by " << pDetail->amount << " silv" << EOL_SCR;
        else if (pDetail->amount<0)
            OneLine << "is overtaxed  by " << (-pDetail->amount) << " silv" << EOL_SCR;
        else
            OneLine << EOL_SCR;

        pDetail->Details << OneLine;
    }

    Factions.DeleteAll();
}

//-------------------------------------------------------------------------

void CAhApp::CheckTradeDetails(CLand  * pLand, CTaxProdDetailsCollByFaction & TradeDetails)
{
    int             x, k;
    CUnit         * pUnit;
    EValueType      type;
    long            men, lvl, tool, canproduce;
    CStr            sCoord, Skill;
    CProduct      * pProd;
//    long            amount;
    TProdDetails    details;
//    BOOL            working;
    CTaxProdDetails * pFactionInfo;
    CTaxProdDetails   Dummy;
    int               idx;
    CTaxProdDetailsCollByFaction Factions;
    CTaxProdDetailsCollByFaction AllFactions;
    CStr              OneLine(32);

//    m_pAtlantis->ComposeLandStrCoord(pLand, sCoord);
//    Details << pLand->TerrainType << " (" << sCoord << "). ";

    for (k=0; k<pLand->Products.Count(); k++)
    {
        pProd = (CProduct*)pLand->Products.At(k);
        if (0==pProd->Amount)
            continue;
  //      amount = pProd->Amount;
        GetProdDetails(pProd->ShortName.GetData(), details);
  //      working = FALSE;
        Skill.Empty();
        Skill << details.skillname << PRP_SKILL_POSTFIX;

        for (x=0; x<pLand->Units.Count(); x++)
        {
            pUnit = (CUnit*)pLand->Units.At(x);
            if (pUnit->Flags & UNIT_FLAG_PRODUCING) 
            {
                Dummy.FactionId = pUnit->FactionId;
                if (TradeDetails.Search(&Dummy, idx))
                    pFactionInfo = (CTaxProdDetails*)TradeDetails.At(idx);
                else
                {
                    pFactionInfo = new CTaxProdDetails;
                    pFactionInfo->FactionId = pUnit->FactionId;
                    TradeDetails.Insert(pFactionInfo);
                }
                if (Factions.Insert(pFactionInfo))
                    pFactionInfo->amount = pProd->Amount;
                if (AllFactions.Insert(pFactionInfo) )
                    pFactionInfo->HexCount++;

                if ( 0==stricmp(pUnit->ProducingItem.GetData(), pProd->ShortName.GetData()))
                {
                    if (!pUnit->GetProperty(PRP_MEN, type, (const void *&)men, eNormal) || eLong!=type)
                        continue;

                    // check skill level
                    if (!pUnit->GetProperty(Skill.GetData(), type, (const void *&)lvl, eNormal) || (eLong!=type) )
                        continue;

                    if (!details.toolname.IsEmpty())
                        if (!pUnit->GetProperty(details.toolname.GetData(), type, (const void *&)tool, eNormal) || eLong!=type )
                            tool = 0;
                    if (tool > men)
                        tool = men;

                    canproduce = (long)((((double)men)*lvl + tool*details.toolhelp) / details.months);
                    pFactionInfo->amount -= canproduce;
    //                working = TRUE;
                }
            }
        }



/*        if (working)
            if (amount>0)
                Details << pProd->ShortName << " is underproduced by " << amount << ". ";
            else if (amount<0)
                Details << pProd->ShortName << " is overproduced by " << (-amount) << ". ";*/

        for (x=0; x<Factions.Count(); x++)
        {
            pFactionInfo = (CTaxProdDetails*)Factions.At(x);

            m_pAtlantis->ComposeLandStrCoord(pLand, sCoord);
            OneLine.Empty();
            OneLine << pLand->TerrainType << " (" << sCoord << ") ";
            while (OneLine.GetLength() < 24)
                OneLine.AddCh(' ');
            OneLine << pProd->ShortName;

            while (OneLine.GetLength() < 28)
                OneLine.AddCh(' ');
            if (pFactionInfo->amount > 0)
                OneLine << " is underproduced by " << pFactionInfo->amount << ". " << EOL_SCR;
            else if (pFactionInfo->amount<0)
                OneLine << " is overproduced  by " << (-pFactionInfo->amount) << ". " << EOL_SCR;
            else
                OneLine << " exact amount produced " << EOL_SCR;

            pFactionInfo->Details << OneLine;
        }

        Factions.DeleteAll();
    }


    AllFactions.DeleteAll();
}

//-------------------------------------------------------------------------

void CAhApp::CheckTaxTrade()
{
    CStr                sTax(64);
    CStr                sTrade(64);
    CStr                Report(64), S(64);
    CStr                Details(256);
//    long                tax = 0;
//    long                trade = 0;
    int                 n, i;
    CLand             * pLand;
    CPlane            * pPlane;
    CTaxProdDetailsCollByFaction  Taxes;
    CTaxProdDetailsCollByFaction  Trades;
    CTaxProdDetails              *pDetails;

    for (n=0; n<m_pAtlantis->m_Planes.Count(); n++)
    {
        pPlane = (CPlane*)m_pAtlantis->m_Planes.At(n);
        for (i=0; i<pPlane->Lands.Count(); i++)
        {
            pLand = (CLand*)pPlane->Lands.At(i);

            if (pLand->Flags & LAND_TAX_NEXT)
                CheckTaxDetails(pLand, Taxes);

            if (pLand->Flags & LAND_TRADE_NEXT)
                CheckTradeDetails(pLand, Trades);
        }
    }
    Report.Empty();
    for (i=0; i<Taxes.Count(); i++)
    {
        pDetails = (CTaxProdDetails*)Taxes.At(i);
        Report << "Faction " << pDetails->FactionId << " : " << pDetails->HexCount << " TAX regions"   << EOL_SCR
               << pDetails->Details << EOL_SCR  << EOL_SCR;
    }
    for (i=0; i<Trades.Count(); i++)
    {
        pDetails = (CTaxProdDetails*)Trades.At(i);
        Report << "Faction " << pDetails->FactionId << " : " << pDetails->HexCount << " TRADE regions"   << EOL_SCR
               << pDetails->Details << EOL_SCR  << EOL_SCR;
    }

    Taxes.FreeAll();
    Trades.FreeAll();

    ShowError(Report.GetData()      , Report.GetLength()      , TRUE);
}

//-------------------------------------------------------------------------

void CAhApp::CheckProduction()
{
    int    n, i, x;
    CLand  * pLand;
    CPlane * pPlane;
    CUnit  * pUnit;
    CStr     Error(64), S(32);

    for (n=0; n<m_pAtlantis->m_Planes.Count(); n++)
    {
        pPlane = (CPlane*)m_pAtlantis->m_Planes.At(n);
        for (i=0; i<pPlane->Lands.Count(); i++)
        {
            pLand = (CLand*)pPlane->Lands.At(i);
            for (x=0; x<pLand->Units.Count(); x++)
            {
                pUnit = (CUnit*)pLand->Units.At(x);
                if (!m_pAtlantis->CheckResourcesForProduction(pUnit, pLand, S))
                    Error << "Unit " << pUnit->Id << " " << S << EOL_SCR;
            }
        }
    }

    S.Empty();
    if (Error.IsEmpty())
        wxMessageBox(wxT("No problem with resources for production detected"));
    else
    {
        S << "The following problems were detected:" << EOL_SCR << EOL_SCR << Error;
        ShowError(S.GetData(), S.GetLength(), TRUE);
    }
}

//--------------------------------------------------------------------------

void CAhApp::CheckSailing()
{
    int    n, i, x;
    CLand  * pLand;
    CPlane * pPlane;
    CStruct* pStruct;
    CStr     Error(64), S(32), sCoord(32);

    for (n=0; n<m_pAtlantis->m_Planes.Count(); n++)
    {
        pPlane = (CPlane*)m_pAtlantis->m_Planes.At(n);
        for (i=0; i<pPlane->Lands.Count(); i++)
        {
            pLand = (CLand*)pPlane->Lands.At(i);
            m_pAtlantis->ComposeLandStrCoord(pLand, sCoord);
            for (x=0; x<pLand->Structs.Count(); x++)
            {
                pStruct = (CStruct*)pLand->Structs.At(x);
                if ((pStruct->Attr & SA_MOBILE) && pStruct->SailingPower > 0)
                {
                    if (pStruct->Load > pStruct->MaxLoad)
                        Error << pLand->TerrainType << " (" << sCoord << ") - Ship " << pStruct->Id << " is overloaded by " << (pStruct->Load - pStruct->MaxLoad) << "." << EOL_SCR;
                    if (pStruct->SailingPower < pStruct->MinSailingPower)
                        Error << pLand->TerrainType << " (" << sCoord << ") - Ship " << pStruct->Id << " is underpowered by " << (pStruct->MinSailingPower - pStruct->SailingPower) << "." << EOL_SCR;
                }
            }
        }
    }

    S.Empty();
    if (Error.IsEmpty())
        wxMessageBox(wxT("No problems with sailing detected"));
    else
    {
        S << "The following problems were detected:" << EOL_SCR << EOL_SCR << Error;
        ShowError(S.GetData(), S.GetLength(), TRUE);
    }
}

//--------------------------------------------------------------------------

#define SET_UNIT_PROP_NAME(_name, _type)                                 \
{                                                                        \
    CStrInt         * pSI, SI;                                           \
    int               k;                                                 \
    if (!m_pAtlantis->m_UnitPropertyNames.Search((void*)_name, k))       \
        m_pAtlantis->m_UnitPropertyNames.Insert(strdup(_name));          \
    SI.m_key = _name;                                                    \
    if (!m_pAtlantis->m_UnitPropertyTypes.Search(&SI, k))                \
    {                                                                    \
        pSI = new CStrInt(_name, _type);                                 \
        m_pAtlantis->m_UnitPropertyTypes.Insert(pSI);                    \
    }                                                                    \
    SI.m_key = NULL;                                                     \
}

void CAhApp::PreLoadReport()
{
    CStr S, FName;

    SaveLandFlags();
    SaveUnitFlags();
    if (m_CommentsChanged)
        SaveComments();
    if (GetOrdersChanged())
        SaveOrders(TRUE);

    if (ERR_OK==m_pAtlantis->m_ParseErr)
        SaveHistory(SZ_HISTORY_FILE);


}

//-------------------------------------------------------------------------

void CAhApp::PostLoadReport()
{
    CStr              S;
    CMapFrame       * pMapFrame  = (CMapFrame    *)m_Frames[AH_FRAME_MAP];
    CMapPane        * pMapPane   = (CMapPane     *)m_Panes [AH_PANE_MAP];
    CUnitPaneFltr   * pUnitPaneF = (CUnitPaneFltr*)m_Panes [AH_PANE_UNITS_FILTER];
    CUnitPane       * pUnitPane  = (CUnitPane    *)m_Panes [AH_PANE_UNITS_HEX];
    long              year, mon;
    const char      * szName;
    const char      * szValue;
    CUnit           * pUnit;
    CPlane          * pPlane;
    CShortNamedObj  * pItem;
    CFaction          DummyFaction;
    CFaction        * pFaction;
    int               i, n;


    // update edge structures
    UpdateEdgeStructs();

    SaveLandFlags();

    // count number of our men in every hex

    m_pAtlantis->CountMenForTheFaction(m_pAtlantis->m_CrntFactionId);

    if (pMapFrame)
    {
        m_sTitle.Empty();

        for (i=0; i<m_pAtlantis->m_OurFactions.Count(); i++)
        {
            pFaction = m_pAtlantis->GetFaction((long)m_pAtlantis->m_OurFactions.At(i));
            if (pFaction)
            {
                if (!m_sTitle.IsEmpty())
                    m_sTitle << ", ";
                if (m_pAtlantis->m_OurFactions.Count()<3)
                    m_sTitle << pFaction->Name << " ";
                m_sTitle << (long)pFaction->Id;
            }
        }
        year = (long)(m_pAtlantis->m_YearMon/100);
        mon  = m_pAtlantis->m_YearMon % 100 - 1;
        if ( (mon >= 0) && (mon < 12) )
            m_sTitle << ". " << Monthes[mon] << " year " << year;
        SetMapFrameTitle();
    }

    // if loaded for the very first time, center it
    if (GetSectionFirst(SZ_SECT_REPORTS, szName, szValue) < 0)
    {
        wxCommandEvent event(wxEVT_COMMAND_TOOL_CLICKED, tool_centerout);

        if (m_Panes[AH_PANE_MAP])
            ((CMapPane*)m_Panes[AH_PANE_MAP])->OnToolbarCmd(event);
    }



    // stnadard unit and base properties - that is likely to be forgotten when
    // new properties are introduced :((
    SET_UNIT_PROP_NAME(PRP_COMMENTS          , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_ORDERS            , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_FACTION_ID        , eLong   )
    SET_UNIT_PROP_NAME(PRP_FACTION           , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_LAND_ID           , eLong   )
    SET_UNIT_PROP_NAME(PRP_ID                , eLong   )
    SET_UNIT_PROP_NAME(PRP_NAME              , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_FULL_TEXT         , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_TEACHING          , eLong   )
    SET_UNIT_PROP_NAME(PRP_SEQUENCE          , eLong   )
    SET_UNIT_PROP_NAME(PRP_FRIEND_OR_FOE     , eLong   )
    SET_UNIT_PROP_NAME(PRP_WEIGHT            , eLong   )
    SET_UNIT_PROP_NAME(PRP_DESCRIPTION       , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_COMBAT            , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_MOVEMENT          , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_FLAGS_STANDARD    , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_FLAGS_CUSTOM      , eCharPtr)
    SET_UNIT_PROP_NAME(PRP_FLAGS_CUSTOM_ABBR , eCharPtr)



    // If no orders loaded, no movement will be calculated. Force it.
    if (!m_pAtlantis->m_OrdersLoaded)
    {
        for (i=0; i<m_pAtlantis->m_Units.Count(); i++)
        {
            pUnit = (CUnit*)m_pAtlantis->m_Units.At(i);
            pUnit->ResetNormalProperties();
        }

        for (n=0; n<m_pAtlantis->m_Planes.Count(); n++)
        {
            pPlane = (CPlane*)m_pAtlantis->m_Planes.At(n);
            for (i=0; i<pPlane->Lands.Count(); i++)
            {
                ((CLand*)pPlane->Lands.At(i))->CalcStructsLoad();
                ((CLand*)pPlane->Lands.At(i))->SetFlagsFromUnits(); // maybe not needed here...
            }
        }
    }

    // skills
    for (i=0; i<m_pAtlantis->m_Skills.Count(); i++)
    {
        pItem = (CShortNamedObj*)m_pAtlantis->m_Skills.At(i);


        EncodeConfigLine(S, pItem->Description.GetData());
        SetConfig(SZ_SECT_SKILLS, pItem->Name.GetData(), S.GetData());
    }

    // Items
    for (i=0; i<m_pAtlantis->m_Items.Count(); i++)
    {
        pItem = (CShortNamedObj*)m_pAtlantis->m_Items.At(i);

        EncodeConfigLine(S, pItem->Description.GetData());
        SetConfig(SZ_SECT_ITEMS, pItem->Name.GetData(), S.GetData());
    }

    // Objects
    for (i=0; i<m_pAtlantis->m_Objects.Count(); i++)
    {
        pItem = (CShortNamedObj*)m_pAtlantis->m_Objects.At(i);

        EncodeConfigLine(S, pItem->Description.GetData());
        SetConfig(SZ_SECT_OBJECTS, pItem->Name.GetData(), S.GetData());
    }

    if (pMapPane)
        pMapPane->Refresh(FALSE, NULL);


    if (pUnitPane)
        pUnitPane->m_pCurLand = NULL; // force the unit pane to do full update
    OnMapSelectionChange();

    // if there were Hex Events, show them
    if (!m_pAtlantis->m_HexEvents.Description.IsEmpty())
    {
        CBaseColl   Coll;
        Coll.Insert(&m_pAtlantis->m_HexEvents);
        ShowDescriptionList(Coll, "Hex Events");
    }

    // show newly discovered products (advanced resources), if any
    if (m_pAtlantis->m_NewProducts.Count() > 0)
        ShowDescriptionList(m_pAtlantis->m_NewProducts, "New products");

    if (pUnitPaneF)
        pUnitPaneF->Update(NULL);

    CheckRedirectedOutputFiles();
    
    if (!m_pAtlantis->m_SecurityEvents.Description.IsEmpty())
        m_pAtlantis->m_SecurityEvents.Description << EOL_SCR << EOL_SCR;
}

//-------------------------------------------------------------------------

int  CAhApp::LoadReport  (const char * FNameIn, BOOL Join)
{
    CStr S, Sect, S2;
    CStr FName;
    int  LoadOrd;
    int  i;
    long n;
    int  err = ERR_FOPEN;

    wxBeginBusyCursor();

    m_DisableErrs = TRUE;

    if (FNameIn && *FNameIn)
    {
        FName = FNameIn;
        FName.TrimRight(TRIM_ALL);

        PreLoadReport();

        if (!m_FirstLoad && !Join)
            m_pAtlantis = new CAtlaParser(&ThisGameDataHelper);

        if (!Join)
        {
            m_pAtlantis->Clear();
            m_pAtlantis->ParseRep(SZ_HISTORY_FILE, FALSE, TRUE);
        }

        // Append unit group property names here so they are available while parsing
        for (i=0; i<m_UnitPropertyGroups.Count(); i++ )
        {
            CStrStr * pSS = (CStrStr*)m_UnitPropertyGroups.At(i);
            SET_UNIT_PROP_NAME(pSS->m_key, eLong)
        }


        err = m_pAtlantis->ParseRep(FName.GetData(), Join, FALSE);
        switch (err)
        {
            case ERR_INV_TURN:
                wxMessageBox(wxT("Wrong turn in the report"), wxT("Error"));
                break;
        }
        SetOrdersChanged(FALSE);
        m_CommentsChanged = FALSE;
        if ( ERR_OK==err && m_pAtlantis->m_YearMon != 0 && m_pAtlantis->m_CrntFactionId != 0 )
        {
            m_ReportDates.Insert((void*)m_pAtlantis->m_YearMon);
            UpgradeConfigByFactionId();

            if (atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_PWD_READ)) && !m_pAtlantis->m_CrntFactionPwd.IsEmpty())
            {
                S.Empty();
                S << (long)m_pAtlantis->m_CrntFactionId;
                SetConfig(SZ_SECT_PASSWORDS, S.GetData(), m_pAtlantis->m_CrntFactionPwd.GetData() );
            }

            LoadOrd = atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_LOAD_ORDER));
            if (LoadOrd)
            {
                S.Empty();
                S << (long)m_pAtlantis->m_YearMon;
                ComposeConfigOrdersSection(Sect, m_pAtlantis->m_CrntFactionId);
                LoadOrders(GetConfig(Sect.GetData(), S.GetData()));
            }
        }

        LoadComments();
        LoadLandFlags();
        LoadUnitFlags();
        PostLoadReport();

        if ( (ERR_OK==err) && (m_pAtlantis->m_YearMon != 0) )
        {
            // doing it after PostLoadReport() since it will check the section
            S.Empty();
            S << (long)m_pAtlantis->m_YearMon;
            if (!Join)
                SetConfig(SZ_SECT_REPORTS, S.GetData(), FName.GetData());
            else
            {
                S2 = GetConfig(SZ_SECT_REPORTS, S.GetData());
                if (!S2.IsEmpty())
                    S2 << ", ";
                S2 << FName;
                SetConfig(SZ_SECT_REPORTS, S.GetData(), S2.GetData());
            }
        }

        if (!m_FirstLoad && !Join)
        {
            if (m_Reports.Search(m_pAtlantis, i))
                m_Reports.AtFree(i);
            m_Reports.Insert(m_pAtlantis);

            n = atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_REP_CACHE_COUNT));
            if (n<=0)
                n = 1;
            if (m_Reports.Count()>n)
            {
                if (i > n/2)
                    n = 0;
                else
                    n = m_Reports.Count()-1;
                if (m_pAtlantis != m_Reports.At(n))
                    m_Reports.AtFree(n);
            }
        }
        m_FirstLoad = FALSE;
    }

    m_DisableErrs = FALSE;

    wxEndBusyCursor();

    return err;
}

//-------------------------------------------------------------------------

int  CAhApp::LoadReport(BOOL Join)
{
    int rc;
    CStr Dir;
    const char * key;

    key = Join ? SZ_KEY_FOLDER_REP_JOIN : SZ_KEY_FOLDER_REP_LOAD;
    Dir = GetConfig(SZ_SECT_FOLDERS, key);
    if (Dir.IsEmpty())
        Dir = ".";

    wxString CurrentDir = wxGetCwd();
    wxFileDialog dialog(m_Frames[AH_FRAME_MAP],
                        wxT("Load Report"),
                        wxString::FromAscii(Dir.GetData()),
                        wxT(""),
                        wxT(SZ_REP_FILES),
                        wxFD_OPEN);
    rc = dialog.ShowModal();
    wxSetWorkingDirectory(CurrentDir);

    if (wxID_OK == rc)
    {
        CStr S;
        S = dialog.GetPath().mb_str();
        MakePathRelative(CurrentDir.mb_str(), S);

        GetDirFromPath(S.GetData(), Dir);
        SetConfig(SZ_SECT_FOLDERS, key, Dir.GetData() );

        return LoadReport(S.GetData(), Join);
    }
    else
        return ERR_CANCEL;

}

//-------------------------------------------------------------------------

void CAhApp::EditPaneChanged(CEditPane * pPane)
{
    CMapPane  * pMapPane  = (CMapPane* )m_Panes[AH_PANE_MAP];
    CLand     * pLand;
    CUnit     * pUnit;

    if (pPane && pMapPane)
    {
        pLand = m_pAtlantis->GetLand(pMapPane->m_SelHexX, pMapPane->m_SelHexY, pMapPane->m_SelPlane, TRUE);

        if (pPane == m_Panes[AH_PANE_UNIT_COMMANDS])
        {
            // selected unit's orders have been changed

            // TBD: is it needed? m_pCurLand->guiUnit = m_pUnitListPane->GetCurrentUnitId();
            m_pAtlantis->RunOrders(pLand);
            UpdateHexUnitList(pLand);
            UpdateHexEditPane(pLand);
            SetOrdersChanged(m_OrdersAreChanged); // this hack is needed since EditPanes are modifying the vars directly...
        }
        else if (pPane == m_Panes[AH_PANE_UNIT_COMMENTS])
        {
            // selected unit's comments / default orders have been changed
            pUnit = GetSelectedUnit(); // depends on m_SelUnitIdx
            if (pUnit)
            {
                pUnit->ExtractCommentsFromDefOrders();
                UpdateHexUnitList(pLand);
            }
        }
    }
}

//-------------------------------------------------------------------------

void CAhApp::SelectTempUnit(CUnit * pUnit)
{
    CEditPane   * pDescription = (CEditPane*)m_Panes[AH_PANE_UNIT_DESCR   ];
    CEditPane   * pOrders      = (CEditPane*)m_Panes[AH_PANE_UNIT_COMMANDS];
    CEditPane   * pComments    = (CEditPane*)m_Panes[AH_PANE_UNIT_COMMENTS];

    OnUnitHexSelectionChange(-1); // unselect   
    m_UnitDescrSrc.Empty();

    if (pUnit)
        m_UnitDescrSrc = pUnit->Description;

    if (pDescription)
        pDescription->SetSource(&m_UnitDescrSrc, NULL);
    if (pOrders)
    {
        pOrders->SetSource(NULL, NULL);
        pOrders->SetReadOnly ( TRUE );
        pOrders->ApplyFonts();
    }
    if (pComments)
    {
        pComments->SetSource(NULL, NULL);
//        pComments->SetReadOnly ( TRUE );
    }
}

//-------------------------------------------------------------------------

void CAhApp::SelectUnit(CUnit * pUnit)
{
    CMapPane    * pMapPane  = (CMapPane* )gpApp->m_Panes[AH_PANE_MAP];
    CUnitPane   * pUnitPane = (CUnitPane*)gpApp->m_Panes[AH_PANE_UNITS_HEX];
    CLand       * pLand;
    CPlane      * pPlane;
    int           nx, ny, nz;
    BOOL          refresh;
    BOOL          NeedSetUnit;

    if (!pUnit || !pMapPane)
        return;
    pLand = m_pAtlantis->GetLand(pUnit->LandId);
    if (!pLand)
        return;

    pLand->guiUnit = pUnit->Id;

    LandIdToCoord(pLand->Id, nx, ny, nz);
    pPlane   = (CPlane*)m_pAtlantis->m_Planes.At(nz);

    refresh = pMapPane->EnsureLandVisible(nx, ny, nz, FALSE);
    if (refresh)
        pMapPane->Refresh(FALSE);

    NeedSetUnit = (pUnitPane && (pLand==pUnitPane->m_pCurLand));

    pMapPane->SetSelection(nx, ny, pUnit, pPlane, TRUE);

    if (pUnit->Flags & UNIT_FLAG_TEMP)
    {
        pUnitPane->SelectUnit(-1);
        SelectTempUnit(pUnit);  // just redraw description
    }
    else
        if (NeedSetUnit)
            pUnitPane->SelectUnit(pUnit->Id); // otherwise will be already selected
}

//-------------------------------------------------------------------------

void CAhApp::SelectLand(CLand * pLand)
{
    CMapPane    * pMapPane  = (CMapPane* )gpApp->m_Panes[AH_PANE_MAP];
    CUnitPane   * pUnitPane = (CUnitPane*)gpApp->m_Panes[AH_PANE_UNITS_HEX];
    CPlane      * pPlane;
    int           nx, ny, nz;
    BOOL          refresh;

    if (pLand)
    {
        LandIdToCoord(pLand->Id, nx, ny, nz);
        pPlane   = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(nz);

        refresh = pMapPane->EnsureLandVisible(nx, ny, nz, TRUE);
        if (refresh)
            pMapPane->Refresh(FALSE);


        if (!pUnitPane || pLand != pUnitPane->m_pCurLand)
            pMapPane->SetSelection(nx, ny, NULL, pPlane, TRUE);
    }
}

//-------------------------------------------------------------------------

BOOL CAhApp::SelectLand(const char * landcoords) //  "48,52[,somewhere]"
{
    CLand       * pLand     = m_pAtlantis->GetLand(landcoords);

    if (pLand)
    {
        SelectLand(pLand);
        return TRUE;
    }
    else
        return FALSE;
}

//-------------------------------------------------------------------------

void CAhApp::EditPaneDClicked(CEditPane * pPane)
{
    const char  * p;
    CStr          src, S;
    char          ch;
    CUnit       * pUnit;
    CBaseObject   Dummy;
    int           idx;
    long          position;


    if (pPane == m_Panes[AH_PANE_MSG])
    {
        pPane->GetValue(src);
        position = pPane->m_pEditor->GetInsertionPoint();

// There is a bug in win32 GetInsertionPoint() - returned value corresponds to "\r\n" end of lines,
// while actual returned string has "\n" end of lines
#ifdef __WXMSW__
        long x = 0;
        p = src.GetData();
        while (x<position)
        {
            if ('\n' == p[x])
                position--;
            x++;
        }
#endif
        if (position > src.GetLength())
            position = src.GetLength();

        p = src.GetData();
        while (position > 0)

        {
            if (p[position-1]=='\n')
                break;
            position--;
        }

        p = &src.GetData()[position];
        p = SkipSpaces(S.GetToken(p, " \t", ch, TRIM_ALL));
        if (0==stricmp("UNIT", S.GetData()))  // that is an order problem report
        {
            S.GetToken(p, " \t", ch, TRIM_ALL);
            Dummy.Id = atol(S.GetData());
            if (m_pAtlantis->m_Units.Search(&Dummy, idx))
            {
                pUnit = (CUnit*)m_pAtlantis->m_Units.At(idx);
                SelectUnit(pUnit);
                return;
            }
        }


        p = &src.GetData()[position];
        p = SkipSpaces(S.GetToken(p, "(\n", ch, TRIM_ALL)); // must be an error from the report file
        if ('('==ch)
        {
            S.GetToken(p, ",)\n", ch, TRIM_ALL);
            Dummy.Id = atol(S.GetData());
            if (')'==ch && m_pAtlantis->m_Units.Search(&Dummy, idx))
            {
                pUnit = (CUnit*)m_pAtlantis->m_Units.At(idx);
                SelectUnit(pUnit);
                return;
            }
        }

        // land
        p = &src.GetData()[position];
        p = SkipSpaces(S.GetToken(p, "(\n", ch, TRIM_ALL));
        if ('('==ch)
        {
            p = SkipSpaces(S.GetToken(p, ")\n", ch, TRIM_ALL));
            if (')' == ch && SelectLand(S.GetData()))
                return;
        }
    }
}

//-------------------------------------------------------------------------

void CAhApp::SwitchToYearMon(long YearMon)
{
    char          Dummy[sizeof(CAtlaParser)];
    CAtlaParser * pDummy  = (CAtlaParser *)Dummy;
    int           i;
    CStr          S, S2;

    PreLoadReport();
    if (GetOrdersChanged())
        return;
    pDummy->m_YearMon = YearMon;
    if (m_Reports.Search(pDummy, i))
    {
        m_pAtlantis = (CAtlaParser *)m_Reports.At(i);
        PostLoadReport();
    }
    else
    {
        S.Empty();
        S << YearMon;

        S2 = GetConfig(SZ_SECT_REPORTS, S.GetData());
        const char * p = S2.GetData();
        BOOL         join = FALSE;
        while (p && *p)
        {
            p = S.GetToken(p, ',');
            LoadReport(S.GetData(), join);
            join = TRUE;
        }
    }
}

//-------------------------------------------------------------------------

void CAhApp::SwitchToRep(eRepSeq whichrep)
{
    int  i;

    m_DisableErrs = TRUE;

    if (CanSwitchToRep(whichrep, i))
        SwitchToYearMon((long)m_ReportDates.At(i));

    m_DisableErrs = FALSE;
}

//-------------------------------------------------------------------------

BOOL CAhApp::CanSwitchToRep(eRepSeq whichrep, int & RepIdx)
{
    long       ym;
    CStr       sName, sData;
    CLand    * pLand;
    CMapPane * pMapPane;

    RepIdx=-1;

    switch(whichrep)
    {
    case repFirst:
        RepIdx = 0;
        break;

    case repLast:
        if (m_pAtlantis->m_YearMon == (long)m_ReportDates.At(gpApp->m_ReportDates.Count()-1) )
            RepIdx = -1;
        else
            RepIdx = m_ReportDates.Count()-1;
        break;

    case repPrev:
        if (m_ReportDates.Search((void*)m_pAtlantis->m_YearMon, RepIdx) )
            RepIdx--;
        break;

    case repNext:
        if (m_ReportDates.Search((void*)m_pAtlantis->m_YearMon, RepIdx) )
            RepIdx++;
        break;

    case repLastVisited:
        pMapPane = (CMapPane* )m_Panes[AH_PANE_MAP];
        pLand    = m_pAtlantis->GetLand(pMapPane->m_SelHexX, pMapPane->m_SelHexY, pMapPane->m_SelPlane, TRUE);
        m_pAtlantis->ComposeLandStrCoord(pLand, sName);
//        ym       = atol(GetConfig(SZ_SECT_LAND_VISITED, sName.GetData()));
        sData.GetToken(GetConfig(SZ_SECT_LAND_VISITED, sName.GetData()), ',');
        ym = atol(sData.GetData());


        if (ym==m_pAtlantis->m_YearMon || !m_ReportDates.Search((void*)ym, RepIdx))
            RepIdx = -1;
        break;
    }

    return (RepIdx>=0 && RepIdx<m_ReportDates.Count());
}

//-------------------------------------------------------------------------

BOOL CAhApp::GetPrevTurnReport(CAtlaParser *& pPrevTurn)
{
    int idx;
    
    pPrevTurn = NULL;
        
    if (CanSwitchToRep(repPrev, idx))
    {
        char          Dummy[sizeof(CAtlaParser)];
        CAtlaParser * pDummy  = (CAtlaParser *)Dummy;
        int           i;
        CStr          S, S2;
    
        long YearMon = (long)m_ReportDates.At(idx);
    
        pDummy->m_YearMon = YearMon;
        if (m_Reports.Search(pDummy, i))
        {
            pPrevTurn = (CAtlaParser *)m_Reports.At(i);
        }
        else
        {
            S.Empty();
            S << YearMon;
    
            S2 = GetConfig(SZ_SECT_REPORTS, S.GetData());
            const char * p = S2.GetData();
            BOOL         join = FALSE;
            m_DisableErrs = TRUE;
            wxBeginBusyCursor();
            pPrevTurn = new CAtlaParser(&ThisGameDataHelper);
            pPrevTurn->ParseRep(SZ_HISTORY_FILE, FALSE, TRUE);
            while (p && *p)
            {
                p = S.GetToken(p, ',');
                //LoadReport(S.GetData(), join);
                pPrevTurn->ParseRep(S.GetData(), join, FALSE);
                join = TRUE;
            }
            wxEndBusyCursor();
            m_DisableErrs = FALSE;
            if (pPrevTurn->m_YearMon == YearMon)
                m_Reports.Insert(pPrevTurn);
            else
            {
                delete pPrevTurn;
                pPrevTurn = NULL;
            }
        }
    }

    return (pPrevTurn != NULL);
}

/*
    CStr S, Sect, S2;
    CStr FName;
    int  LoadOrd;
    int  i;
    long n;
    int  err = ERR_FOPEN;

    wxBeginBusyCursor();

    m_DisableErrs = TRUE;

    if (FNameIn && *FNameIn)
    {
        FName = FNameIn;
        FName.TrimRight(TRIM_ALL);

        PreLoadReport();

        if (!m_FirstLoad && !Join)
            m_pAtlantis = new CAtlaParser(&ThisGameDataHelper);

        if (!Join)
        {
            m_pAtlantis->Clear();
            m_pAtlantis->ParseRep(SZ_HISTORY_FILE, FALSE, TRUE);
        }

        // Append unit group property names here so they are available while parsing
        for (i=0; i<m_UnitPropertyGroups.Count(); i++ )
        {
            CStrStr * pSS = (CStrStr*)m_UnitPropertyGroups.At(i);
            SET_UNIT_PROP_NAME(pSS->m_key, eLong)
        }


        err = m_pAtlantis->ParseRep(FName.GetData(), Join, FALSE);
        switch (err)
        {
            case ERR_INV_TURN:
                wxMessageBox("Wrong turn in the report", "Error");
                break;
        }
        SetOrdersChanged(FALSE);
        m_CommentsChanged = FALSE;
        if ( ERR_OK==err && m_pAtlantis->m_YearMon != 0 && m_pAtlantis->m_CrntFactionId != 0 )
        {
            m_ReportDates.Insert((void*)m_pAtlantis->m_YearMon);
            UpgradeConfigByFactionId();

            if (atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_PWD_READ)) && !m_pAtlantis->m_CrntFactionPwd.IsEmpty())
            {
                S.Empty();
                S << (long)m_pAtlantis->m_CrntFactionId;
                SetConfig(SZ_SECT_PASSWORDS, S.GetData(), m_pAtlantis->m_CrntFactionPwd.GetData() );
            }

            LoadOrd = atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_LOAD_ORDER));
            if (LoadOrd)
            {
                S.Empty();
                S << (long)m_pAtlantis->m_YearMon;
                ComposeConfigOrdersSection(Sect, m_pAtlantis->m_CrntFactionId);
                LoadOrders(GetConfig(Sect.GetData(), S.GetData()));
            }
        }

        LoadComments();
        LoadLandFlags();
        LoadUnitFlags();
        PostLoadReport();

        if ( (ERR_OK==err) && (m_pAtlantis->m_YearMon != 0) )
        {
            // doing it after PostLoadReport() since it will check the section
            S.Empty();
            S << (long)m_pAtlantis->m_YearMon;
            if (!Join)
                SetConfig(SZ_SECT_REPORTS, S.GetData(), FName.GetData());
            else
            {
                S2 = GetConfig(SZ_SECT_REPORTS, S.GetData());
                if (!S2.IsEmpty())
                    S2 << ", ";
                S2 << FName;
                SetConfig(SZ_SECT_REPORTS, S.GetData(), S2.GetData());
            }
        }

        if (!m_FirstLoad && !Join)
        {
            if (m_Reports.Search(m_pAtlantis, i))
                m_Reports.AtFree(i);
            m_Reports.Insert(m_pAtlantis);

            n = atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_REP_CACHE_COUNT));
            if (n<=0)
                n = 1;
            if (m_Reports.Count()>n)
            {
                if (i > n/2)
                    n = 0;
                else
                    n = m_Reports.Count()-1;
                if (m_pAtlantis != m_Reports.At(n))
                    m_Reports.AtFree(n);
            }
        }
        m_FirstLoad = FALSE;
    }

    m_DisableErrs = FALSE;

    wxEndBusyCursor();
*/


//-------------------------------------------------------------------------

void CAhApp::UpdateHexEditPane(CLand * pLand)
{
    CStruct     * pStruct;
    CEditPane   * pEditPane;
    int           i;
    BOOL          FlagsEmpty = TRUE;

    m_HexDescrSrc.Empty();

    pEditPane = (CEditPane*)m_Panes[AH_PANE_MAP_DESCR];
    if (pEditPane)
    {
        if (pLand)
        {
            m_HexDescrSrc << pLand->Description;

            m_HexDescrSrc << EOL_SCR;
            m_pAtlantis->ComposeProductsLine(pLand, EOL_SCR, m_HexDescrSrc);

            if (pLand->Structs.Count()>0)
            {
                m_HexDescrSrc.TrimRight(TRIM_ALL);
                m_HexDescrSrc << EOL_SCR << "-----------" << EOL_SCR;
                for (i=0; i<pLand->Structs.Count(); i++)
                {
                    pStruct = (CStruct*)pLand->Structs.At(i);
                    m_HexDescrSrc << pStruct->Description;
                    m_HexDescrSrc.TrimRight(TRIM_ALL);
                    if (pStruct->Attr & SA_MOBILE)
                        m_HexDescrSrc << " Load: " << pStruct->Load << ", Power: " << pStruct->SailingPower << ".";
                    m_HexDescrSrc << EOL_SCR;
                }
            }

            for (i=0; i<LAND_FLAG_COUNT; i++)
                if (!pLand->FlagText[i].IsEmpty())
                {
                    FlagsEmpty = FALSE;
                    break;
                }


            if (!FlagsEmpty)
            {
                m_HexDescrSrc.TrimRight(TRIM_ALL);
                m_HexDescrSrc << EOL_SCR << "-----------";

                for (i=0; i<LAND_FLAG_COUNT; i++)
                    if (!pLand->FlagText[i].IsEmpty())
                        m_HexDescrSrc << EOL_SCR << pLand->FlagText[i];
            }

            if (!pLand->Events.IsEmpty() &&
                 0 != stricmp(SkipSpaces(pLand->Events.GetData()), "none")
               )
                m_HexDescrSrc << EOL_SCR << "Events:" << EOL_SCR << pLand->Events << EOL_SCR;
            m_HexDescrSrc << EOL_SCR << "Exits:"  << EOL_SCR << pLand->Exits;
        }
        pEditPane->SetSource(&m_HexDescrSrc, NULL);
    }
}

//-------------------------------------------------------------------------

void CAhApp::UpdateHexUnitList(CLand * pLand)
{
    CUnitPane   * pUnitPane = (CUnitPane*)m_Panes[AH_PANE_UNITS_HEX];

    if (pUnitPane)
        pUnitPane->Update(pLand);
}

//-------------------------------------------------------------------------

void CAhApp::OnMapSelectionChange()
{
    CLand       * pLand    = NULL;
    CMapPane    * pMapPane = (CMapPane* )m_Panes[AH_PANE_MAP];

    if (pMapPane)
        pLand   = m_pAtlantis->GetLand(pMapPane->m_SelHexX, pMapPane->m_SelHexY, pMapPane->m_SelPlane, TRUE);

    UpdateHexEditPane(pLand);  // NULL is Ok!
    UpdateHexUnitList(pLand);
    SetMapFrameTitle();
}

//-------------------------------------------------------------------------


void CAhApp::OnUnitHexSelectionChange(long idx)
{
    // It can be called as a result of selecting a hex on the map!

    // It will be unit in the current hex!

    BOOL          ReadOnly = TRUE;
    CEditPane   * pDescription;
    CEditPane   * pOrders;
    CEditPane   * pComments;
    CFaction    * pFaction;
    CUnit       * pUnit;


    m_SelUnitIdx = idx;
    pUnit        = GetSelectedUnit(); // depends on m_SelUnitIdx
    pFaction     = pUnit?pUnit->pFaction:NULL;

    pDescription = (CEditPane*)m_Panes[AH_PANE_UNIT_DESCR   ];
    pOrders      = (CEditPane*)m_Panes[AH_PANE_UNIT_COMMANDS];
    pComments    = (CEditPane*)m_Panes[AH_PANE_UNIT_COMMENTS];

    m_UnitDescrSrc.Empty();

    if (pUnit)
    {
        m_UnitDescrSrc = pUnit->Description;
        if (!pUnit->Errors.IsEmpty())
            m_UnitDescrSrc << " ***** Errors:\r\n" << pUnit->Errors;
        if (!pUnit->Events.IsEmpty())
            m_UnitDescrSrc << " ----- Events:\r\n" << pUnit->Events;

        ReadOnly = (!pUnit->IsOurs || pUnit->Id<=0) ;
    }

    if (!ReadOnly)
        ReadOnly = (m_pAtlantis->m_YearMon != (long)m_ReportDates.At(gpApp->m_ReportDates.Count()-1) );

    if (pDescription)
        pDescription->SetSource(&m_UnitDescrSrc, NULL);
    if (pOrders)
    {
        if (pOrders->m_pEditor->IsModified())
        {
            long    Id = 0;
            CLand * pLand = NULL;

            if (pUnit)
            {
                Id = pUnit->Id;
                pLand = m_pAtlantis->GetLand(pUnit->LandId);

            }

            // OnKillFocus event for the editor did not fire up
            pOrders->OnKillFocus();

            // OnKillFocus kills all new units!
            pUnit = NULL;
            if (Id != 0 && pLand)
            {
                CBaseObject         Dummy;
                int                 idx;

                Dummy.Id = Id;
                if (pLand->Units.Search(&Dummy, idx))
                {
                    pUnit = (CUnit*)pLand->Units.At(idx);
                    SelectUnit(pUnit);
                }

            }
        }

        pOrders->SetSource(pUnit?&pUnit->Orders:NULL,      &m_OrdersAreChanged);
        pOrders->SetReadOnly ( ReadOnly );
        pOrders->ApplyFonts();
    }
    if (pComments)
    {
        if (pComments->m_pEditor->IsModified())
        {
            // OnKillFocus event for the editor did not fire up
            pComments->OnKillFocus();
        }
        pComments->SetSource(pUnit?&pUnit->DefOrders:NULL, &m_CommentsChanged);
    }

    RedrawTracks();
}

//-------------------------------------------------------------------------

void CAhApp::LoadOrders()
{
    int rc;
    CStr Dir;

    Dir = GetConfig(SZ_SECT_FOLDERS, SZ_KEY_FOLDER_ORDERS);
    if (Dir.IsEmpty())
        Dir = ".";

    wxString CurrentDir = wxGetCwd();
    wxFileDialog dialog(m_Frames[AH_FRAME_MAP],
                        wxT("Load orders"),
                        wxString::FromAscii(Dir.GetData()),
                        wxT(""),
                        wxT(SZ_ORD_FILES),
                        wxFD_OPEN );
    rc = dialog.ShowModal();
    wxSetWorkingDirectory(CurrentDir);

    if (wxID_OK==rc)
    {
        CStr S;
        S = dialog.GetPath().mb_str();
        MakePathRelative(CurrentDir.mb_str(), S);
        GetDirFromPath(S.GetData(), Dir);
        SetConfig(SZ_SECT_FOLDERS, SZ_KEY_FOLDER_ORDERS, Dir.GetData() );

        LoadOrders(S.GetData());
        SetOrdersChanged(FALSE);
    }
}

//-------------------------------------------------------------------------

BOOL CAhApp::CanCloseApp()
{
    SaveLandFlags();
    SaveUnitFlags();
    if (m_CommentsChanged)
        SaveComments();

    return ( m_DiscardChanges || !GetOrdersChanged() || ERR_OK==SaveOrders(TRUE));
}

//--------------------------------------------------------------------------

void CAhApp::ShowDescriptionList(CCollection & Items, const char * title) // Collection of CBaseObject
{
    CBaseObject  * pObj;

    if (Items.Count() > 0)
    {
        if (1 == Items.Count())
        {
            pObj = (CBaseObject*)Items.At(0);
            CShowOneDescriptionDlg dlg(gpApp->m_Frames[AH_FRAME_MAP], pObj->Name.GetData(), pObj->Description.GetData());
            dlg.ShowModal();
        }
        else
        {
            CShowDescriptionListDlg dlg(gpApp->m_Frames[AH_FRAME_MAP], title, &Items);
            dlg.ShowModal();
        }
    }

}

//--------------------------------------------------------------------------
/*
void CAhApp::ViewSkills(BOOL ViewAll)
{
    CBaseColl     Skills;
    CBaseObject * pSkill;
    const char  * szName;
    const char  * szValue;
    int           sectidx;

    if (ViewAll)
    {
        sectidx = GetSectionFirst(SZ_SECT_SKILLS, szName, szValue);
        while (sectidx >= 0)
        {
            pSkill              = new CBaseObject;
            pSkill->Name        = szName;
            DecodeConfigLine(pSkill->Description, szValue);
            Skills.Insert(pSkill);

            sectidx = GetSectionNext(sectidx, SZ_SECT_SKILLS, szName, szValue);
        }

        ShowDescriptionList(Skills, "Skills");
        Skills.FreeAll();
    }
    else
        ShowDescriptionList(m_pAtlantis->m_Skills, "Skills");
}
*/
//--------------------------------------------------------------------------

void CAhApp::ViewShortNamedObjects(BOOL ViewAll, const char * szSection, const char * szHeader, CBaseColl & ListNew)
{
    CBaseColl     Items;
    CBaseObject * pItem;
    const char  * szName;
    const char  * szValue;
    int           sectidx;

    if (ViewAll)
    {
        sectidx = GetSectionFirst(szSection, szName, szValue);
        while (sectidx >= 0)
        {
            pItem              = new CBaseObject;
            pItem->Name        = szName;
            DecodeConfigLine(pItem->Description, szValue);
            Items.Insert(pItem);

            sectidx = GetSectionNext(sectidx, szSection, szName, szValue);
        }

        ShowDescriptionList(Items, szHeader);
        Items.FreeAll();
    }
    else
        ShowDescriptionList(ListNew, szHeader);
}

//--------------------------------------------------------------------------

void CAhApp::ViewEvents(BOOL DoEvents)
{
    CBaseColl   Coll;

    if (DoEvents)
    {
        Coll.Insert(&m_pAtlantis->m_Events);
        ShowDescriptionList(Coll, "Events");
    }
    else
    {
//        Coll.Insert(&m_pAtlantis->m_Errors);
//        ShowDescriptionList(Coll, "Errors");
        m_MsgSrc.Empty();
        ShowError(m_pAtlantis->m_Errors.Description.GetData(), m_pAtlantis->m_Errors.Description.GetLength(), TRUE);

    }
    Coll.DeleteAll();
}

//--------------------------------------------------------------------------

void CAhApp::ViewSecurityEvents()
{
/*    CBaseColl   Coll;

    Coll.Insert(&m_pAtlantis->m_SecurityEvents);
    ShowDescriptionList(Coll, "Security Events");

    Coll.DeleteAll();*/
    
        m_MsgSrc.Empty();
        ShowError(m_pAtlantis->m_SecurityEvents.Description.GetData(), m_pAtlantis->m_SecurityEvents.Description.GetLength(), TRUE);
}

//--------------------------------------------------------------------------

void CAhApp::ViewNewProducts()
{
    ShowDescriptionList(m_pAtlantis->m_NewProducts, "New products");
}

//--------------------------------------------------------------------------

void CAhApp::ViewBattlesAll()
{
    ShowDescriptionList(m_pAtlantis->m_Battles, "Battles");
}

//--------------------------------------------------------------------------

void CAhApp::ViewGates()
{
    ShowDescriptionList(m_pAtlantis->m_Gates, "Gates");
}

//--------------------------------------------------------------------------

void CAhApp::ViewCities()
{
    CBaseObject      * pObj;
    CBaseCollByName    coll;
    int                np,nl;
    CPlane           * pPlane;
    CLand            * pLand;
    //int                x,y,z;
    CStr               sCoord;

    for (np=0; np<m_pAtlantis->m_Planes.Count(); np++)
    {
        pPlane = (CPlane*)m_pAtlantis->m_Planes.At(np);
        for (nl=0; nl<pPlane->Lands.Count(); nl++)
        {
            pLand    = (CLand*)pPlane->Lands.At(nl);
            if (!pLand->CityName.IsEmpty())
            {
                pObj       = new CBaseObject;
                pObj->Name = pLand->CityName;

                //LandIdToCoord(pLand->Id, x, y, z);
                m_pAtlantis->ComposeLandStrCoord(pLand, sCoord);
                pObj->Description << pLand->TerrainType << " (" << sCoord << ") in " << pLand->Name;
                pObj->Description << ", contains " << pLand->CityName << " [" << pLand->CityType << "]";

                if (!coll.Insert(pObj))
                    delete pObj;
            }
        }
    }

    ShowDescriptionList(coll, "Cities");
    coll.FreeAll();
}

//--------------------------------------------------------------------------

void CAhApp::ViewProvinces()
{
    CBaseObject      * pObj;
    CBaseCollByName    coll;
    int                np,nl;
    CPlane           * pPlane;
    CLand            * pLand;
    CStr               sCoord;
    int                loop;

    for (loop=0; loop<2; loop++)
    {
        for (np=0; np<m_pAtlantis->m_Planes.Count(); np++)
        {
            pPlane = (CPlane*)m_pAtlantis->m_Planes.At(np);
            for (nl=0; nl<pPlane->Lands.Count(); nl++)
            {
                pLand      = (CLand*)pPlane->Lands.At(nl);
                if ((pLand->Flags&LAND_VISITED) || 1==loop) // we run it twice, so we pick visited hexes if we can
                {
                    pObj       = new CBaseObject;
                    pObj->Name = pLand->Name;

                    m_pAtlantis->ComposeLandStrCoord(pLand, sCoord);
                    pObj->Description << pLand->TerrainType << " (" << sCoord << ") in " << pLand->Name;

                    if (!coll.Insert(pObj))
                        delete pObj;
                }
            }
        }
    }

    ShowDescriptionList(coll, "Provinces");
    coll.FreeAll();
}

//--------------------------------------------------------------------------

void CAhApp::ViewFactionInfo()
{
    CStr sMoreInfo(32), sInfo(32);
    int                np,nl;
    CPlane           * pPlane;
    CLand            * pLand;
    long               nLandsTotal = 0, nLandsVisited=0;

    sMoreInfo << EOL_SCR << "-------------------------" << EOL_SCR;
    for (np=0; np<m_pAtlantis->m_Planes.Count(); np++)
    {
        pPlane = (CPlane*)m_pAtlantis->m_Planes.At(np);
        for (nl=0; nl<pPlane->Lands.Count(); nl++)
        {
            pLand    = (CLand*)pPlane->Lands.At(nl);
            nLandsTotal++;
            if (pLand->Flags&LAND_VISITED)
                nLandsVisited++;
        }
    }
    sMoreInfo << "Total hexes  : " << nLandsTotal   << EOL_SCR
              << "Visited hexes: " << nLandsVisited << EOL_SCR ;


    sInfo << m_pAtlantis->m_FactionInfo << sMoreInfo;
    CShowOneDescriptionDlg dlg(gpApp->m_Frames[AH_FRAME_MAP],
                               "Faction Info",
                               sInfo.GetData());
    dlg.ShowModal();
}

//--------------------------------------------------------------------------

void CAhApp::ViewFactionOverview_IncrementValue(long FactionId, const char * factionname, CBaseCollById & Factions, const char * propname, long value)
{
    CBaseObject   * pFaction;
    CBaseObject     Dummy;
    int             idx;
    EValueType      type;
    const void    * valuetot;
    
    Dummy.Id = FactionId;
    if (Factions.Search(&Dummy, idx))
        pFaction = (CBaseObject*)Factions.At(idx);
    else
    {
        pFaction       = new CBaseObject;
        pFaction->Id   = FactionId;
        if (factionname)
            pFaction->Name = factionname;
        Factions.Insert(pFaction);
    }

    if (!pFaction->GetProperty(propname, type, valuetot, eNormal))
        valuetot = (void*)0;

    if (-1==(long)valuetot || 0x7fffffff - (long)value < (long)valuetot )
        valuetot = (void*)(long)-1; // overflow protection
    else
        valuetot = (void*)((long)valuetot + (long)value);
    pFaction->SetProperty(propname, eLong, valuetot, eNormal);
}                    

//--------------------------------------------------------------------------

void CAhApp::ViewFactionOverview()
{
//m_UnitPropertyNames

    int             unitidx, propidx, nl;
    CUnit         * pUnit;
    CStr            propname;
    CStr            Skill;
    int             skilllen;
    int             maxproplen = 0;
    CStr            Report(128);
    CMapPane      * pMapPane  = (CMapPane* )m_Panes[AH_PANE_MAP];
    BOOL            Selected  = FALSE;
    EValueType      type;
    const void    * value;
    int             idx;
    CBaseObject   * pFaction;
    long            men;

    CBaseColl       Hexes(64);
    CBaseCollById   Factions(16);
    CLand         * pLand;

    if (!pMapPane->HaveSelection())
        ShowMessageBoxSwitchable("Hint", "Faction overview can be generated using only selected area on the map", "FACTION_OVERVIEW");

    if (pMapPane->HaveSelection() &&
        wxYES == wxMessageBox(wxT("Use only selected hexes?"), wxT("Confirm"), wxYES_NO, NULL))
        Selected = TRUE;

    skilllen    = strlen(PRP_SKILL_POSTFIX);

    // collect data
    pMapPane->GetSelectedOrAllHexes(Hexes, Selected);
    for (nl=0; nl<Hexes.Count(); nl++)
    {
        pLand = (CLand*)Hexes.At(nl);
        for (unitidx=0; unitidx<pLand->Units.Count(); unitidx++)
        {
            pUnit    = (CUnit*)pLand->Units.At(unitidx);
            men      = 0;
            if (pUnit->GetProperty(PRP_MEN, type, value, eOriginal) && (eLong==type) )
                men = (long)value;

            for (propidx=0; propidx<m_pAtlantis->m_UnitPropertyNames.Count(); propidx++)
            {
                propname = (const char *) gpApp->m_pAtlantis->m_UnitPropertyNames.At(propidx);

                // skip 'skill days' property
                if (IsASkillRelatedProperty(propname.GetData()) &&
                     propname.FindSubStrR(PRP_SKILL_POSTFIX) != propname.GetLength()-skilllen)
                    continue;

                // skip some properties which can not be aggegated
                if (0==stricmp(propname.GetData(), PRP_ID        ) ||
                    0==stricmp(propname.GetData(), PRP_FACTION_ID) ||
                    0==stricmp(propname.GetData(), PRP_LAND_ID   ) ||
                    0==stricmp(propname.GetData(), PRP_STRUCT_ID ) ||
                    0==stricmp(propname.GetData(), PRP_TEACHING  ) ||
                    0==stricmp(propname.GetData(), PRP_SKILLS    ) ||
                    0==stricmp(propname.GetData(), PRP_MAG_SKILLS) ||
                    0==stricmp(propname.GetData(), PRP_SEQUENCE  ) ||
                    0==stricmp(propname.GetData(), PRP_FRIEND_OR_FOE  )


                   )
                    continue;

                if (pUnit->GetProperty(propname.GetData(), type, value, eOriginal) &&
                    (eLong==type) )
                    do
                    {
                        if (propname.FindSubStrR(PRP_SKILL_POSTFIX) == propname.GetLength()-skilllen)
                        {
                                // it is a skill
    
                            propname << (long)value;
                            value    = (void*)men;
                        }
                        else 
                            if (IsASkillRelatedProperty(propname.GetData()))
                                break;
    
                        if (propname.GetLength() > maxproplen)
                            maxproplen = propname.GetLength();
    
                        ViewFactionOverview_IncrementValue(pUnit->FactionId, pUnit->pFaction ? pUnit->pFaction->Name.GetData() : NULL, Factions, propname.GetData(), (long)value);
                        
                    } while (FALSE);

            }

            if (pUnit->Flags & UNIT_FLAG_AVOIDING)
                ViewFactionOverview_IncrementValue(pUnit->FactionId, pUnit->pFaction ? pUnit->pFaction->Name.GetData() : NULL, Factions, "Avoiding", men);
            else
            {
                if (pUnit->Flags & UNIT_FLAG_BEHIND)
                    ViewFactionOverview_IncrementValue(pUnit->FactionId, pUnit->pFaction ? pUnit->pFaction->Name.GetData() : NULL, Factions, "Back Line", men);
                else
                    ViewFactionOverview_IncrementValue(pUnit->FactionId, pUnit->pFaction ? pUnit->pFaction->Name.GetData() : NULL, Factions, "Front Line", men);
            }
            

            /*
            propidx  = 0;
            propname = pUnit->GetPropertyName(propidx);
            while (!propname.IsEmpty())
            {
                if (pUnit->GetProperty(propname.GetData(), type, value, eOriginal) &&
                    (eLong==type) )
                    do
                    {
                        if (propname.FindSubStrR(PRP_SKILL_POSTFIX) == propname.GetLength()-skilllen)
                        {
                            // it is a skill

                            propname << (long)value;
                            if (!pUnit->GetProperty(PRP_MEN, type, value, eOriginal) &&
                                (eLong==type) )
                                break;
                        }
                        else if (IsASkillRelatedProperty(propname.GetData()) ||
                                 0==stricmp(PRP_SEQUENCE, propname.GetData()) ||
                                 0==stricmp(PRP_STRUCT_ID, propname.GetData()) )
                            break;

                        if (propname.GetLength() > maxproplen)
                            maxproplen = propname.GetLength();

                        Dummy.Id = pUnit->FactionId;
                        if (Factions.Search(&Dummy, idx))
                            pFaction = (CBaseObject*)Factions.At(idx);
                        else
                        {
                            pFaction       = new CBaseObject;
                            pFaction->Id   = pUnit->FactionId;
                            if (pUnit->pFaction)
                                pFaction->Name = pUnit->pFaction->Name;
                            Factions.Insert(pFaction);
                        }

                        if (!pFaction->GetProperty(propname.GetData(), type, valuetot, eNormal))
                            valuetot = (void*)0;

                        valuetot = (void*)((long)valuetot + (long)value);
                        pFaction->SetProperty(propname.GetData(), eLong, valuetot, eNormal);
                    } while (FALSE);

                propname = pUnit->GetPropertyName(++propidx);
            }
            */
        }
    }
    Hexes.DeleteAll();

    // prepare display

    for (idx=0; idx<Factions.Count(); idx++)
    {
        pFaction = (CBaseObject*)Factions.At(idx);
        Report << "Faction " << pFaction->Id << " " << pFaction->Name << EOL_SCR << EOL_SCR;


        propidx  = 0;
        propname = pFaction->GetPropertyName(propidx);
        while (!propname.IsEmpty())
        {
            if (pFaction->GetProperty(propname.GetData(), type, value, eNormal) &&
                (eLong==type) )
            {
                while (propname.GetLength() < maxproplen)
                    propname.AddCh(' ');
                Report << propname << "  " << (long)value << EOL_SCR;
            }

            propname = pFaction->GetPropertyName(++propidx);
        }
        Report << EOL_SCR << "-------------------------------------------"  << EOL_SCR << EOL_SCR;
    }

    //display data

    CShowOneDescriptionDlg dlg(gpApp->m_Frames[AH_FRAME_MAP],
                               "Factions Overview",
                               Report.GetData());
    dlg.ShowModal();
    Factions.FreeAll();
}

//--------------------------------------------------------------------------

void CAhApp::CheckMonthLongOrders()
{
    static const char dup_ord_msg[] = ";--- Duplicate month long orders";
    static const char no_ord_msg[]  = ";--- No month long orders";
    int                  x;
    CUnit              * pUnit;
    const char         * src;
    const char         * dupord;
    const char         * p;
    char                 ch;
    CStr                 Line;
    CStr                 Ord;
    const char         * order;
    BOOL                 IsNew;
    BOOL                 Found;
    CStr                 Errors(128);
    CStr                 S(64);
    CStr                 FoundOrder;
    CStringSortColl      MonthLongOrders;
    CStringSortColl      MonthLongDup;
    long                 men;
    EValueType           type;
    CUnitPaneFltr      * pUnitPaneF = NULL;
    int                  errcount = 0;
    int                  turnlvl;
    CBaseColl            Hexes(64);
    int                  nl, unitidx;
    CLand              * pLand;
    CMapPane           * pMapPane  = (CMapPane* )m_Panes[AH_PANE_MAP];


    p = SkipSpaces(GetConfig(SZ_SECT_COMMON, SZ_KEY_ORD_MONTH_LONG));
    while (p && *p)
    {
        p = SkipSpaces(S.GetToken(p, ','));
        if (!S.IsEmpty())
            MonthLongOrders.Insert(strdup(S.GetData()));
    }

    p = SkipSpaces(GetConfig(SZ_SECT_COMMON, SZ_KEY_ORD_DUPLICATABLE));
    while (p && *p)
    {
        p = SkipSpaces(S.GetToken(p, ','));
        if (!S.IsEmpty())
            MonthLongDup.Insert(strdup(S.GetData()));
    }

    if (1==atol(SkipSpaces(GetConfig(SZ_SECT_COMMON, SZ_KEY_CHECK_OUTPUT_LIST))))
    {
        // Output will go into the unit filter window
        OpenUnitFrameFltr(FALSE);
        pUnitPaneF = (CUnitPaneFltr*)m_Panes [AH_PANE_UNITS_FILTER];
    }

    if (pUnitPaneF)
        pUnitPaneF->InsertUnitInit();

    pMapPane->GetSelectedOrAllHexes(Hexes, FALSE);
    for (nl=0; nl<Hexes.Count(); nl++)
    {
        pLand = (CLand*)Hexes.At(nl);
        for (unitidx=0; unitidx<pLand->Units.Count(); unitidx++)
        {
            pUnit    = (CUnit*)pLand->Units.At(unitidx);

            if (!pUnit->IsOurs)
                continue;
            src   = pUnit->Orders.GetData();
            IsNew = FALSE;
            Found = FALSE;
            turnlvl = 0;
            while (src && *src)
            {
                dupord = src;
                src    = Line.GetToken(src, '\n', TRIM_ALL);
                Ord.GetToken(SkipSpaces(Line.GetData()), " \t", ch, TRIM_ALL);
                order = Ord.GetData();
                if ('@'==*order)
                    order++;
                if (0==SafeCmp("FORM", order))
                    IsNew = TRUE;
                else if (0==SafeCmp("END", order))
                    IsNew = FALSE;
                else if (0==SafeCmp("TURN", order))
                    turnlvl++;
                else if (0==SafeCmp("ENDTURN", order))
                    turnlvl--;
                else if (!IsNew && 0==turnlvl && MonthLongOrders.Search((void*)order, x) )
                {
                    if (Found)
                    {
                        if (0==stricmp(order, FoundOrder.GetData()) &&
                            MonthLongDup.Search((void*)order, x))
                            continue; // it is an order which can be duplicated

                        errcount++;
                        if (pUnitPaneF)
                        {
                            int newpos;

                            pUnitPaneF->InsertUnit(pUnit);
                            S = dup_ord_msg;
                            S << EOL_SCR;
                            newpos = dupord - pUnit->Orders.GetData() + S.GetLength();
                            pUnit->Orders.InsBuf(S.GetData(), dupord - pUnit->Orders.GetData(), S.GetLength());
                            src = &pUnit->Orders.GetData()[newpos];
                        }
                        else
                        {
                            S.Format("Unit % 5d Error : Duplicate month long orders - %s", pUnit->Id, Line.GetData());
                            Errors << S << EOL_SCR;
                        }
                        break;
                    }
                    Found      = TRUE;
                    FoundOrder = order;
                }
            }
            if (!Found)
            {
                if (!pUnit->GetProperty(PRP_MEN, type, (const void *&)men, eNormal) ||
                    (eLong==type && 0==men))
                    continue; // no men - no orders is ok

                errcount++;
                if (pUnitPaneF)
                {
                    pUnitPaneF->InsertUnit(pUnit);
                    Line.GetToken(pUnit->Orders.GetData(), '\n', TRIM_ALL);
                    if (NULL==strstr(Line.GetData(), no_ord_msg))
                    {
                        S = no_ord_msg;
                        S << EOL_SCR;
                        pUnit->Orders.InsBuf(S.GetData(), 0, S.GetLength());
                    }
                }
                else
                {
                    S.Format("Unit % 5d Warning : No month long orders", pUnit->Id);
                    Errors << S << EOL_SCR;
                }
            }
        }
    }

    Hexes.DeleteAll();


    if (pUnitPaneF)
        pUnitPaneF->InsertUnitDone();

    if (!pUnitPaneF && errcount>0)
        ShowError(Errors.GetData(), Errors.GetLength(), TRUE);

    if (0==errcount)
        wxMessageBox(wxT("No problems found."), wxT("Order checking"), wxOK | wxCENTRE, m_Frames[AH_FRAME_MAP]);


//int wxMessageBox(const wxString& message, const wxString& caption = "Message", int style = wxOK | wxCENTRE,
// wxWindow *parent = NULL, int x = -1, int y = -1)

    MonthLongOrders.FreeAll();
    MonthLongDup.FreeAll();
}

//--------------------------------------------------------------------------

void CAhApp::ShowUnitsMovingIntoHex(long CurHexId, CPlane * pCurPlane)
{
    CLand          * pLand;
    CUnit          * pUnit;
    int              nl, nu, i;
    long             HexId;
    CUnitPaneFltr  * pUnitPaneF = NULL;
    CStr             UnitText(128), S(16);
    CBaseColl        FoundUnits;

    for (nl=0; nl<pCurPlane->Lands.Count(); nl++)
    {
        pLand = (CLand*)pCurPlane->Lands.At(nl);
        for (nu=0; nu<pLand->Units.Count(); nu++)
        {
            pUnit = (CUnit*)pLand->Units.At(nu);
            if (pUnit->pMovement && pUnit->pMovement->Count()>0)
            {
                HexId = (long)pUnit->pMovement->At(pUnit->pMovement->Count()-1);
                if (HexId==CurHexId)
                    FoundUnits.Insert(pUnit);
            }
        }
    }

    // now display our findings
    if (FoundUnits.Count() > 0)
    {
        if (1==atol(SkipSpaces(GetConfig(SZ_SECT_COMMON, SZ_KEY_CHECK_OUTPUT_LIST))))
        {
            // Output will go into the unit filter window
            OpenUnitFrameFltr(FALSE);
            pUnitPaneF = (CUnitPaneFltr*)m_Panes [AH_PANE_UNITS_FILTER];
            pUnitPaneF->InsertUnitInit();
        }


        for (i=0; i<FoundUnits.Count(); i++)
        {
            pUnit = (CUnit*)FoundUnits.At(i);
            if (pUnitPaneF)
                pUnitPaneF->InsertUnit(pUnit);
            else
            {
                S.Format("Unit % 5d", pUnit->Id);
                UnitText << S << EOL_SCR;
            }
        }

        if (pUnitPaneF)
            pUnitPaneF->InsertUnitDone();
        else
            ShowError(UnitText.GetData(), UnitText.GetLength(), TRUE);
    }
    else
        wxMessageBox(wxT("Found no units moving into the current hex."), wxT("Units moving"), wxOK | wxCENTRE, m_Frames[AH_FRAME_MAP]);


    FoundUnits.DeleteAll();
}

//--------------------------------------------------------------------------

void CAhApp::ShowLandFinancial(CLand * pCurLand)
{
    CUnit            * pUnit;
    int                idx, factidx;
    long               CurFaction;
    long               SilvOrg = 0;
    long               SilvRes = 0;
    long               TaxOur  = 0;
    long               TaxTheir= 0;
    long               WorkOur  = 0;
    long               WorkTheir= 0;
    long               Maintain = 0;
    long               men;
    long               MovedOut = 0;
    long               Workers  = 0;
    EValueType         type;
    const void       * value;
    CBaseObject        Report;
    CBaseCollByName    coll;
    CStr               sCoord;
    CLongSortColl      Factions;
    long               TaxPerTaxer;
    const char       * leadership;

    if (!pCurLand)
        return;

    TaxPerTaxer = atol(GetConfig(SZ_SECT_COMMON, SZ_KEY_TAX_PER_TAXER));

    // get faction list
    for (idx=0; idx<pCurLand->Units.Count(); idx++)
    {
        pUnit    = (CUnit*)pCurLand->Units.At(idx);
        if (pUnit->FactionId != 0)
            Factions.Insert((void*)pUnit->FactionId);
    }

    // check each faction
    for (factidx=0; factidx<Factions.Count(); factidx++)
    {
        CurFaction = (long)Factions.At(factidx);
        SilvOrg  = 0;
        SilvRes  = 0;
        TaxOur   = 0;
        TaxTheir = 0;
        WorkOur  = 0;
        WorkTheir= 0;
        Maintain = 0;
        MovedOut = 0;
        Workers  = 0;

        for (idx=0; idx<pCurLand->Units.Count(); idx++)
        {
            pUnit    = (CUnit*)pCurLand->Units.At(idx);
            if (!pUnit->IsOurs)
                continue;
            if (pUnit->FactionId == CurFaction)
            {
                if (pUnit->GetProperty(PRP_SILVER, type, value, eOriginal) && eLong==type)
                    SilvOrg += (long)value;
                if (pUnit->GetProperty(PRP_SILVER, type, value, eNormal) && eLong==type)
                {
                    SilvRes += (long)value;

                    if (pUnit->pMovement && pUnit->pMovement->Count()>0)
                        MovedOut += (long)value;
                }
            }

            if (pUnit->GetProperty(PRP_MEN, type, (const void *&)men, eNormal) && eLong==type)
            {
                if (pUnit->Flags & UNIT_FLAG_TAXING)
                    if (pUnit->FactionId == CurFaction)
                        TaxOur += men*TaxPerTaxer;
                    else
                        TaxTheir += men*TaxPerTaxer;

                if (pUnit->IsWorking)
                    if (pUnit->FactionId == CurFaction)
                    {
                        Workers += men;
                        WorkOur +=  (long)(men*pCurLand->Wages);
                    }
                    else
                        WorkTheir +=  (long)(men*pCurLand->Wages);

                if (pUnit->FactionId == CurFaction && (!pUnit->pMovement || pUnit->pMovement->Count()==0))
                    if (pUnit->GetProperty(PRP_LEADER, type, (const void *&)leadership, eNormal) && eCharPtr==type &&
                        (0==strcmp(leadership, SZ_LEADER) || 0==strcmp(leadership, SZ_HERO)))
                        Maintain += men*20;
                    else
                        Maintain += men*10;
            }

        }

        if (TaxOur + TaxTheir > pCurLand->Taxable)
            TaxOur =  (long)(((double)pCurLand->Taxable)/(TaxTheir + TaxOur)*TaxOur);

        if (WorkOur + WorkTheir > pCurLand->MaxWages)
            WorkOur =  (long)(((double)pCurLand->MaxWages)/(WorkTheir + WorkOur)*WorkOur);

        if (Maintain>0)
        {
            Report.Description << EOL_SCR << "Faction " << (long)CurFaction << EOL_SCR;
            Report.Description << "==========" << EOL_SCR;
            Report.Description << "SILV in the beginning       "   << SilvOrg << EOL_SCR;
            Report.Description << "SILV after executing orders "   << SilvRes << EOL_SCR;
            Report.Description << "Expected Tax Income         "   << TaxOur << EOL_SCR;
            Report.Description << "Expected Work Income        "   << WorkOur << EOL_SCR;
            Report.Description << "Expected Maintenance       -"   << Maintain << EOL_SCR;
            Report.Description << "Moved out                  -"   << MovedOut << EOL_SCR;
            Report.Description << "                            -------"    << EOL_SCR;
            Report.Description << "Expected Balance            "   << (SilvRes + TaxOur + WorkOur - Maintain - MovedOut) << EOL_SCR;
            Report.Description << ""    << EOL_SCR;
            Report.Description << "Workers                     "   << Workers << EOL_SCR;
            Report.Description << "Max workers                 "   << (long)(((double)pCurLand->MaxWages)/pCurLand->Wages) << EOL_SCR;
        }
    }

    m_pAtlantis->ComposeLandStrCoord(pCurLand, sCoord);
    Report.Name << "Financial report for " << sCoord;
    coll.Insert(&Report);

    ShowDescriptionList(coll, "Financial report");
    coll.DeleteAll();
}

//--------------------------------------------------------------------------

void CAhApp::AddTempHex(int X, int Y, int Plane)
{
    CLand  * pCurLand = m_pAtlantis->GetLand(X, Y, Plane, TRUE);
    if (pCurLand)
        return;
        
    CPlane * pPlane = (CPlane*)m_pAtlantis->m_Planes.At(Plane);
    if (!pPlane)
        return;
        
    assert(Plane == pPlane->Id);
    
    CStr     sTerrain;
    wxString strTerrain = wxGetTextFromUser(wxT("Terrain"), wxT("Please specify terrain type"));
    sTerrain = strTerrain.mb_str();
        
    if (sTerrain.IsEmpty())
        return;    
        
    CLand * pLand       = new CLand;
    pLand->ExitBits     = 0xFF;
    pLand->Id           = LandCoordToId ( X,Y, pPlane->Id );
    pLand->pPlane       = pPlane;
    pLand->Name         = SZ_MANUAL_HEX_PROVINCE;
    pLand->TerrainType  = sTerrain;
    pLand->Taxable      = 0;
    pLand->Description  << sTerrain << " (" << (long)X << "," << (long)Y << ") in " SZ_MANUAL_HEX_PROVINCE; // ", 0 peasants (unknown), $0.";
    pPlane->Lands.Insert ( pLand );
}

//--------------------------------------------------------------------------

void CAhApp::DelTempHex(int X, int Y, int Plane)
{
    int      idx;
    CLand  * pCurLand = m_pAtlantis->GetLand(X, Y, Plane, TRUE);
    if (!pCurLand)
        return;
        
    CPlane * pPlane = (CPlane*)m_pAtlantis->m_Planes.At(Plane);
    if (!pPlane)
        return;
        
    assert(Plane == pPlane->Id);
    
    if (pPlane->Lands.Search(pCurLand, idx))
        pPlane->Lands.AtFree(idx);
}

//--------------------------------------------------------------------------

void CAhApp::RerunOrders()
{
    m_pAtlantis->RunOrders(NULL);
    CUnitPane * pUnitPane = (CUnitPane*)gpApp->m_Panes[AH_PANE_UNITS_HEX];
    if (pUnitPane)
        pUnitPane->Update(pUnitPane->m_pCurLand);
}

//--------------------------------------------------------------------------

int CAhApp::SaveHistory(const char * FNameOut)
{
    CLand            * pLand;
    CFileWriter        Dest;
    int                nl, np;
    CPlane           * pPlane;
    SAVE_HEX_OPTIONS   options;

    memset(&options, 0, sizeof(options));
    options.AlwaysSaveImmobStructs = TRUE;
    options.SaveResources          = TRUE;

    if ( (m_pAtlantis->m_Planes.Count()>0) &&
         (0==m_pAtlantis->m_ParseErr)      && // don't destroy if not loaded!
         Dest.Open(FNameOut)
       )
    {
        for (np=0; np<m_pAtlantis->m_Planes.Count(); np++)
        {
            pPlane = (CPlane*)m_pAtlantis->m_Planes.At(np);
            for (nl=0; nl<pPlane->Lands.Count(); nl++)
            {
                pLand    = (CLand*)pPlane->Lands.At(nl);
                m_pAtlantis->SaveOneHex(Dest, pLand, pPlane, &options);
            }
        }
        Dest.Close();
    }
    return ERR_OK;
}

//--------------------------------------------------------------------------




BOOL CAhApp::GetExportHexOptions(CStr & FName, CStr & FMode, SAVE_HEX_OPTIONS & options, eHexIncl & HexIncl,
                                 bool & InclTurnNoAcl )
{

    static CStr     stFName;
    static bool     stOverwrite     = FALSE;
    static eHexIncl stHexIncl       = HexNew;
    static bool     stInclStructs   = TRUE;
    static bool     stInclUnits     = TRUE;
    static bool     stInclTurnNoAcl = FALSE;
    static bool     stInclResources = TRUE;

    CHexExportDlg   dlg(m_Frames[AH_FRAME_MAP]);

    memset(&options, 0, sizeof(options));
    options.SaveUnits = TRUE;

    if (stFName.IsEmpty())
        stFName.Format("map.%04d", m_pAtlantis->m_YearMon);

    dlg.m_tcFName         ->SetValue(wxString::FromAscii(stFName.GetData()));

    dlg.m_rbHexNew        ->SetValue(HexNew      == stHexIncl);
    dlg.m_rbHexCurrent    ->SetValue(HexCurrent  == stHexIncl);
    dlg.m_rbHexSelected   ->SetValue(HexSelected == stHexIncl);
    dlg.m_rbHexAll        ->SetValue(HexAll      == stHexIncl);

    dlg.m_rbFileOverwrite ->SetValue(false); //stOverwrite);
    dlg.m_rbFileAppend    ->SetValue(true);  //!stOverwrite);

    dlg.m_chbInclStructs  ->SetValue(stInclStructs  );
    dlg.m_chbInclUnits    ->SetValue(stInclUnits    );
    dlg.m_chbInclTurnNoAcl->SetValue(stInclTurnNoAcl);
    dlg.m_chbInclResources->SetValue(stInclResources);



    if (wxID_OK == dlg.ShowModal())
    {
        stFName.SetStr(dlg.m_tcFName->GetValue().mb_str());

        if (dlg.m_rbHexNew->GetValue())
            stHexIncl = HexNew;
        else if (dlg.m_rbHexCurrent->GetValue())
            stHexIncl = HexCurrent;
        else if (dlg.m_rbHexSelected->GetValue())
            stHexIncl = HexSelected;
        else if (dlg.m_rbHexAll->GetValue())
            stHexIncl = HexAll;

        stOverwrite = dlg.m_rbFileOverwrite->GetValue();

        stInclStructs   = dlg.m_chbInclStructs  ->GetValue();
        stInclUnits     = dlg.m_chbInclUnits    ->GetValue();
        stInclTurnNoAcl = dlg.m_chbInclTurnNoAcl->GetValue();
        stInclResources = dlg.m_chbInclResources->GetValue();

        FName = stFName;
#if defined(_MSC_VER)
        FMode = stOverwrite?"wb":"ab";
#else
        FMode = stOverwrite?"w":"a";
#endif
        options.SaveStructs  = stInclStructs;
        options.SaveUnits    = stInclUnits;
        options.SaveResources= stInclResources;
        HexIncl = stHexIncl;
        InclTurnNoAcl = stInclTurnNoAcl;

        return TRUE;
    }


    return FALSE;
}

//--------------------------------------------------------------------------

// will discriminate by new hex

void CAhApp::ExportOneHex(CFileWriter & Dest, CPlane * pPlane, CLand * pLand, SAVE_HEX_OPTIONS & options, bool InclTurnNoAcl, bool OnlyNew)
{
    CStr               sData, sName;
    const char       * p;
    int                ym_first = 0;
    int                ym_last  = 0;

    m_pAtlantis->ComposeLandStrCoord(pLand, sName);

    p  = sData.GetToken(GetConfig(SZ_SECT_LAND_VISITED, sName.GetData()), ',');
    if (sData.IsEmpty())
    {
/*        ym_first = m_pAtlantis->m_YearMon;
        ym_last  = m_pAtlantis->m_YearMon;*/
    }
    else
    {
        ym_last = atol(sData.GetData());
        sData.GetToken(SkipSpaces(p), ',');
        ym_first = atol(sData.GetData());
    }

    if (InclTurnNoAcl)
        options.WriteTurnNo = (ym_last/100 - 1)*12 + ym_last%100;
    else
        options.WriteTurnNo = 0;

    if (ym_first==m_pAtlantis->m_YearMon || !OnlyNew)
    {
        m_pAtlantis->SaveOneHex(Dest, pLand, pPlane, &options);
    }
}

//--------------------------------------------------------------------------

void CAhApp::ExportHexes()
{
    CStr               sData, sName;
    CMapPane         * pMapPane  = (CMapPane* )m_Panes[AH_PANE_MAP];

    CLand            * pLand;
    CFileWriter        Dest;
    int                nl;
    CPlane           * pPlane;
    SAVE_HEX_OPTIONS   options;
    eHexIncl           HexIncl;
    bool               InclTurnNoAcl ;

    if ( GetExportHexOptions(sName, sData, options, HexIncl, InclTurnNoAcl) &&
         Dest.Open(sName.GetData(), sData.GetData()) )
        if (HexCurrent==HexIncl)
        {
            pPlane   = (CPlane*)m_pAtlantis->m_Planes.At(pMapPane->m_SelPlane);
            pLand    = m_pAtlantis->GetLand(pMapPane->m_SelHexX, pMapPane->m_SelHexY, pMapPane->m_SelPlane, TRUE);
            ExportOneHex(Dest, pPlane, pLand, options, InclTurnNoAcl, FALSE);
        }
        else
        {
            CBaseColl  Hexes(64);
            pMapPane->GetSelectedOrAllHexes(Hexes, HexSelected==HexIncl);
            for (nl=0; nl<Hexes.Count(); nl++)
            {
                pLand = (CLand*)Hexes.At(nl);
                ExportOneHex(Dest, pLand->pPlane, pLand, options, InclTurnNoAcl, HexNew==HexIncl);
            }

            Hexes.DeleteAll();
        }
        Dest.Close();
}

//--------------------------------------------------------------------------

void CAhApp::FindTradeRoutes()
{
    CMapPane    * pMapPane  = (CMapPane* )m_Panes[AH_PANE_MAP];
    CBaseColl     Hexes(64);
    CLand       * pSellLand, * pBuyLand;
    int           i, j;
    CStr          Report(64);
    int           idx;
    const char  * propnameprice;
    EValueType    type;
    const void  * value;
    CStr          GoodsName(32), PropName(32), sCoord(32);
    long          nSaleAmount, nSalePrice, nBuyAmount, nBuyPrice;
    
    if (!pMapPane)
        return;
    wxBeginBusyCursor();
    
    pMapPane->GetSelectedOrAllHexes(Hexes, TRUE);
    if (0==Hexes.Count())
        wxMessageBox(wxT("Please select area on the map first."));
    for (i=0; i<Hexes.Count(); i++)
    {
        pSellLand = (CLand*)Hexes.At(i);
        
        idx      = 0;
        propnameprice = pSellLand->GetPropertyName(idx);
        while (propnameprice)
        {
            if (pSellLand->GetProperty(propnameprice, type, value, eOriginal) && 
                eLong==type && 
                0==strncmp(propnameprice, PRP_SALE_PRICE_PREFIX, sizeof(PRP_SALE_PRICE_PREFIX)-1))
            {
                nSalePrice = (long)value;
                GoodsName = &(propnameprice[sizeof(PRP_SALE_PRICE_PREFIX)-1]);
                
                PropName.Empty(); 
                PropName << PRP_SALE_AMOUNT_PREFIX << GoodsName;
                if (!pSellLand->GetProperty(PropName.GetData(), type, value, eOriginal) || eLong!=type)
                    continue;
                nSaleAmount = (long)value;
                
                for (j=0; j<Hexes.Count(); j++)
                {
                    pBuyLand = (CLand*)Hexes.At(j);
                    
                    PropName.Empty(); 
                    PropName << PRP_WANTED_PRICE_PREFIX << GoodsName;
                    if (!pBuyLand->GetProperty(PropName.GetData(), type, value, eOriginal) || eLong!=type)
                        continue;
                    nBuyPrice = (long)value;
                    
                    PropName.Empty(); 
                    PropName << PRP_WANTED_AMOUNT_PREFIX << GoodsName;
                    if (!pBuyLand->GetProperty(PropName.GetData(), type, value, eOriginal) || eLong!=type)
                        continue;
                    nBuyAmount = (long)value;
                    
                    if (nBuyPrice > nSalePrice)
                    {
                        m_pAtlantis->ComposeLandStrCoord(pSellLand, sCoord);
                        Report << pSellLand->TerrainType << " (" << sCoord << ") " << EOL_SCR;
                        m_pAtlantis->ComposeLandStrCoord(pBuyLand, sCoord);
                        Report << "         to " << pBuyLand->TerrainType << " (" << sCoord << ")   ("
                               << nBuyPrice << "-" << nSalePrice << ")*" << min(nSaleAmount,nBuyAmount)
                               << " " << GoodsName
                               << " = " << (nBuyPrice - nSalePrice) * min(nSaleAmount,nBuyAmount) << EOL_SCR;
                    }
                }
            }
            propnameprice = pSellLand->GetPropertyName(++idx);
        }
    }
    
    if (Report.IsEmpty())
        wxMessageBox(wxT("No trade routes found."));
    else
        ShowError(Report.GetData()      , Report.GetLength()      , TRUE);

    Hexes.DeleteAll();
    wxEndBusyCursor();
}

//--------------------------------------------------------------------------

void CAhApp::EditListColumns(int command)
{
    CMapFrame   * pMapFrame  = (CMapFrame *)m_Frames[AH_FRAME_MAP];
    CUnitPane   * pUnitPane  = NULL;
    const char  * szConfigSectionHdr;


    const char * szKey = NULL;
    switch (command)
    {
    case menu_ListColUnits:
        szKey = SZ_KEY_LIS_COL_UNITS_HEX;
        pUnitPane = (CUnitPane*)m_Panes[AH_PANE_UNITS_HEX];
        break;

    case menu_ListColUnitsFltr:
        szKey = SZ_KEY_LIS_COL_UNITS_FILTER;
        pUnitPane = (CUnitPane*)m_Panes[AH_PANE_UNITS_FILTER];
        break;

    default:
        return;
    }
    if (pUnitPane)
        pUnitPane->SaveUnitListHdr();

    CListHeaderEditDlg dlg(pMapFrame, szKey);

    if (wxID_OK == dlg.ShowModal())
    {
        szConfigSectionHdr = GetListColSection(SZ_SECT_LIST_COL_UNIT, szKey);
        if (pUnitPane)
            pUnitPane->ReloadHdr(szConfigSectionHdr);
    }
}

//--------------------------------------------------------------------------

const char * CAhApp::GetListColSection(const char * sectprefix, const char * key)
{
    const char * sect;

    sect = GetConfig(SZ_SECT_LIST_COL_CURRENT, key);
    if (!sect || !*sect)
        sect  = GetNextSectionName(CONFIG_FILE_CONFIG, sectprefix);

    return sect;
}

//--------------------------------------------------------------------------

void CAhApp::StdRedirectInit()
{
#ifdef __WXMAC_OSX__
	char cwd[MAXPATHLEN];
	// Setup new working directory in case we got started from /Applications
	if((getcwd(cwd, MAXPATHLEN)) != NULL){
		if((strncmp(cwd, "/Applications", strlen("/Applications"))) == 0){
			const char *home = getenv("HOME");
			if(home != NULL){
				if(0 == chdir(home)){
					mkdir(".alh", 0750);
					if(0 != chdir(".alh"))
						chdir("/Applications");
				}
			}
		}
	}
#endif
    freopen("ah.stdout", "w", stdout);
    freopen("ah.stderr", "w", stderr);
    m_nStdoutLastPos = 0;
    m_nStderrLastPos = 0;
}

//--------------------------------------------------------------------------

void CAhApp::StdRedirectReadMore(BOOL FromStdout, CStr & sData)
{
    FILE       * f;
    int        * pCurPos;
    char         buf[1024];
    int          n;

    sData.Empty();
    if (FromStdout)
    {
        fflush(stdout);
        pCurPos  =  &m_nStdoutLastPos;
        f        = fopen("ah.stdout", "rb");
    }
    else
    {
        fflush(stderr);
        pCurPos  =  &m_nStderrLastPos;
        f        = fopen("ah.stderr", "rb");
    }

    if (f)
    {
        fseek(f, *pCurPos, SEEK_SET);
        do
        {
            n = fread(buf, 1, sizeof(buf), f);
            if (n>0)
                sData.AddBuf(buf, n);
        } while (n>0);
        *pCurPos = ftell(f);
        fclose(f);
    }
}

//--------------------------------------------------------------------------

void CAhApp::CheckRedirectedOutputFiles()
{
    CStr S;

    gpApp->StdRedirectReadMore(FALSE, S);
    if (!S.IsEmpty())
        ShowError(S.GetData(), S.GetLength(), TRUE);
    gpApp->StdRedirectReadMore(TRUE, S);
    if (!S.IsEmpty())
        ShowError(S.GetData(), S.GetLength(), TRUE);
}

//--------------------------------------------------------------------------

void CAhApp::StdRedirectDone()
{
}

//--------------------------------------------------------------------------

void CAhApp::InitMoveModes()
{
    const char * p;
    CStr         S;
    int          n;
    BOOL         Update = FALSE;

    p     = SkipSpaces(GetConfig(SZ_SECT_COMMON, SZ_KEY_MOVEMENTS));
    while (p && *p)
    {
        p = SkipSpaces(S.GetToken(p, ','));
        m_MoveModes.Insert(strdup(S.GetData()));
    }

    // do update here for 2.3.2
    p = SZ_DEFAULT_MOVEMENT_MODE;
    n = 0;
    while (p && *p)
    {
        p = SkipSpaces(S.GetToken(p, ','));
        n++;

        if (n > m_MoveModes.Count())
        {
            m_MoveModes.Insert(strdup(S.GetData()));
            Update = TRUE;
        }
    }
    if (Update)
    {
        S.Empty();
        for (n=0; n<m_MoveModes.Count(); n++)
        {
            if (n>0)
                S << ',';
            S << (const char *)m_MoveModes.At(n);
        }
        SetConfig(SZ_SECT_COMMON, SZ_KEY_MOVEMENTS, S.GetData());
    }
}

//--------------------------------------------------------------------------

void CAhApp::SelectNextUnit()
{
    if (m_Panes[AH_PANE_UNITS_HEX]) 
        ((CUnitPane*)m_Panes[AH_PANE_UNITS_HEX])->SelectNextUnit();
}

//--------------------------------------------------------------------------

void CAhApp::SelectPrevUnit()
{
    if (m_Panes[AH_PANE_UNITS_HEX]) 
        ((CUnitPane*)m_Panes[AH_PANE_UNITS_HEX])->SelectPrevUnit();
}

//--------------------------------------------------------------------------

void CAhApp::SelectUnitsPane()
{
    if (m_Panes[AH_PANE_UNITS_HEX]) 
        ((CUnitPane*)m_Panes[AH_PANE_UNITS_HEX])->SetFocus();
}

//--------------------------------------------------------------------------

void CAhApp::SelectOrdersPane()
{
    if (m_Panes[AH_PANE_UNIT_COMMANDS]) 
        ((CUnitPane*)m_Panes[AH_PANE_UNIT_COMMANDS])->SetFocus();
}

//--------------------------------------------------------------------------

void CAhApp::ViewMovedUnits()
{
}

//=========================================================================

void  CGameDataHelper::ReportError(const char * msg, int msglen, BOOL orderrelated)
{
    gpApp->ShowError(msg, msglen, !orderrelated);
};

long  CGameDataHelper::GetStudyCost(const char * skill)
{
    return gpApp->GetStudyCost(skill);
};

long  CGameDataHelper::GetStructAttr(const char * kind, long & MaxLoad, long & MinSailingPower)
{
    return gpApp->GetStructAttr(kind, MaxLoad, MinSailingPower);
}

const char *  CGameDataHelper::ResolveAlias(const char * alias)
{
    return gpApp->ResolveAlias(alias);
}

BOOL CGameDataHelper::GetItemWeights(const char * item, int *& weights, const char **& movenames, int & movecount )
{
    return gpApp->GetItemWeights(ResolveAlias(item), weights, movenames, movecount );
}

void CGameDataHelper::GetMoveNames(const char **& movenames)
{
    gpApp->GetMoveNames(movenames);
}

const char * CGameDataHelper::GetConfString(const char * section, const char * param)
{
    if (!section)
        section = SZ_SECT_COMMON;
    return gpApp->GetConfig(section, param);
}

BOOL CGameDataHelper::GetOrderId(const char * order, long & id)
{
    return gpApp->GetOrderId(order, id);
}

BOOL CGameDataHelper::IsTradeItem(const char * item)
{
    return gpApp->IsTradeItem(item);
}

BOOL CGameDataHelper::IsMan(const char * item)
{
    return gpApp->IsMan(item);
}

const char * CGameDataHelper::GetWeatherLine(BOOL IsCurrent, BOOL IsGood, int Zone)
{
    return gpApp->GetWeatherLine(IsCurrent, IsGood, Zone);
}

BOOL CGameDataHelper::GetTropicZone  (const char * plane, long & y_min, long & y_max)
{
    const char * value;
    CStr         S;

    value = SkipSpaces(gpApp->GetConfig(SZ_SECT_TROPIC_ZONE, plane));
    if (!value || !*value)
        return FALSE;

    value = S.GetToken(value, ',');
    y_min = atol(S.GetData());

    value = S.GetToken(value, ',');
    y_max = atol(S.GetData());

    return TRUE;
}

void CGameDataHelper::SetTropicZone  (const char * plane, long y_min, long y_max)
{
    CStr S;
    S << y_min << ',' << y_max;
    gpApp->SetConfig(SZ_SECT_TROPIC_ZONE, plane, S.GetData());
}

void CGameDataHelper::GetProdDetails (const char * item, TProdDetails & details)
{
    gpApp->GetProdDetails (item, details);
}

long CGameDataHelper::MaxSkillLevel  (const char * race, const char * skill, const char * leadership, BOOL IsArcadiaSkillSystem)
{
    return gpApp->GetMaxRaceSkillLevel(race, skill, leadership, IsArcadiaSkillSystem);
}

BOOL CGameDataHelper::ImmediateProdCheck()
{
    return atol(gpApp->GetConfig(SZ_SECT_COMMON,  SZ_KEY_CHK_PROD_REQ));
}

BOOL CGameDataHelper::CanSeeAdvResources(const char * skillname, const char * terrain, CLongColl & Levels, CBufColl & Resources)
{
    return gpApp->CanSeeAdvResources(skillname, terrain, Levels, Resources);
}

int CGameDataHelper::GetAttitudeForFaction(int id)
{
    return gpApp->GetAttitudeForFaction(id);
}

void CGameDataHelper::SetAttitudeForFaction(int id, int attitude)
{
    gpApp->SetAttitudeForFaction(id, attitude);
}

void CGameDataHelper::SetPlayingFaction(long id)
{
    // set playing faction to ATT_FRIEND2
    gpApp->SetAttitudeForFaction(id, ATT_FRIEND2);
    gpApp->SetConfig(SZ_SECT_ATTITUDES, SZ_ATT_PLAYER_ID, id);
}

BOOL CGameDataHelper::ShowMoveWarnings()
{
    return atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_CHECK_MOVE_MODE));
}

BOOL CGameDataHelper::IsRawMagicSkill(const char * skillname)
{
    static int     postlen = strlen(PRP_SKILL_POSTFIX);
    CStr           S;

    S = skillname;
    if (S.FindSubStrR(PRP_SKILL_POSTFIX) == S.GetLength()-postlen)
    {
        S.DelSubStr(S.GetLength()-postlen, postlen);
        return gpApp->IsMagicSkill(S.GetData());
    }

    return FALSE;
}

BOOL CGameDataHelper::IsWagon(const char * item)
{
    if (!item)
        return FALSE;
    CStr S = gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_WAGONS);
    CStr T;
    const char * p = S.GetData();
    while (p && *p)
    {
        p = T.GetToken(p, ',', TRIM_ALL);
        if (0==stricmp(item, T.GetData()))
            return TRUE;
    }
    return FALSE;
}

BOOL CGameDataHelper::IsWagonPuller(const char * item)
{
    if (!item)
        return FALSE;
    CStr S = gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_WAGON_PULLERS);
    CStr T;
    const char * p = S.GetData();
    while (p && *p)
    {
        p = T.GetToken(p, ',', TRIM_ALL);
        if (0==stricmp(item, T.GetData()))
            return TRUE;
    }
    return FALSE;
}

int CGameDataHelper::WagonCapacity()
{
    return atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_WAGON_CAPACITY));
}

//==========================================================================

void FontToStr(const wxFont * font, CStr & s)
{
    s.Empty();
    s << (long)font->GetPointSize()  << ","
      << (long)font->GetFamily   ()  << ","
      << (long)font->GetStyle    ()  << ","
      << (long)font->GetWeight   ()  << ","
      << (long)font->GetEncoding ()  << ","
      <<       font->GetFaceName ().mb_str() ;
}

//--------------------------------------------------------------------------

#if defined(_WIN32)
   #define AH_DEFAULT_FONT_SIZE 10
#else
   #define AH_DEFAULT_FONT_SIZE 12
#endif


wxFont * NewFontFromStr(const char * p)
{
    int            size;
    int            family;
    int            style;
    int            weight;
    int            encoding;
    wxString       facename;
    wxFont     *   font;


    CStr           S;

    if (p && *p)
    {
        p = S.GetToken(SkipSpaces(p), ',');  size     = atol(S.GetData());
        p = S.GetToken(SkipSpaces(p), ',');  family   = atol(S.GetData());
        p = S.GetToken(SkipSpaces(p), ',');  style    = atol(S.GetData());
        p = S.GetToken(SkipSpaces(p), ',');  weight   = atol(S.GetData());
        p = S.GetToken(SkipSpaces(p), ',');  encoding = atol(S.GetData());
                                             facename = wxString::FromAscii(SkipSpaces(p));
    }
    else
    {
        size     = AH_DEFAULT_FONT_SIZE;
        family   = wxDEFAULT;
        style    = wxNORMAL;
        weight   = wxNORMAL;
        encoding = wxFONTENCODING_SYSTEM;
        facename = wxT("");
    }

    font = new wxFont(size, family, style, weight, FALSE, facename, (wxFontEncoding)encoding);

    return font;
}

//--------------------------------------------------------------------------

void StrToColor(wxColour * cr, const char * p)
{
    CStr          S;
    int           r, g, b;

    p = S.GetToken(p, ',');
    r = atol(S.GetData());

    p = S.GetToken(p, ',');
    g = atol(S.GetData());

    p = S.GetToken(p, ',');
    b = atol(S.GetData());

    cr->Set(r,g,b);
}

//--------------------------------------------------------------------------

void ColorToStr(char * p, wxColour * cr)
{
    sprintf(p, "%d, %d, %d",
            (int)(cr->Red()  ),
            (int)(cr->Green()),
            (int)(cr->Blue() )
        );
}

//--------------------------------------------------------------------------

#if defined(__WXMSW__)
  #define EQUAL_PATH_CHARS(a, b) (tolower(a) == tolower(b))
#else
  #define EQUAL_PATH_CHARS(a, b) (a == b)
#endif

#if defined(__WXMSW__)
#define SEP '\\'
#else
#define SEP '/'
#endif


void MakePathRelative(const char * cur_dir, CStr & path)
{
    const char * p = path.GetData();
    CStr         rel_path;

    while (*p && EQUAL_PATH_CHARS(*p, *cur_dir) )
    {
        p++;
        cur_dir++;
    }

    if (*p==SEP)
        p++;
    else
        rel_path << ".." << SEP;

    while (*cur_dir)
    {
        if (*cur_dir == SEP)
            rel_path << ".." << SEP;

        cur_dir++;
    }

    rel_path << p;

    if (path.GetLength() > rel_path.GetLength())
        path = rel_path;
}

//-------------------------------------------------------------------------

void MakePathFull(const char * cur_dir, CStr & path)
{
    CStr full_path;
    CStr rel_path;
    
    full_path = cur_dir;
    rel_path = path;

    if (!full_path.IsEmpty() && full_path.GetData()[full_path.GetLength()-1] != SEP)
        full_path.AddCh( SEP);

    if (!rel_path.IsEmpty())
    {
        if (rel_path.GetData()[0]=='.' && rel_path.GetData()[1]==SEP)
            rel_path.DelSubStr(0,2);
    }
    
    path = full_path;
    path << rel_path;
}

//-------------------------------------------------------------------------

void GetDirFromPath(const char * path, CStr & dir)
{
    int n = 0;
    const char * p;

    if (!path || !*path)
        return;

    dir = path;
    p   = dir.GetData() + (dir.GetLength()-1);
    while (*p!='\\' && *p!='/' && n<dir.GetLength())
    {
        p--;
        n++;
    }
    if (*p=='\\' || *p=='/')
        n++;

    if (n>0)
        dir.DelSubStr(dir.GetLength()-n, n);
    if (dir.IsEmpty())
        dir = ".";
}

//-------------------------------------------------------------------------

void GetFileFromPath(const char * path, CStr & file)
{
    const char * p = strrchr(path, SEP);
    
    file.Empty();
    if (p && *p)
    {
        p++;
        file = p;
    }
    else
        file = path;
}

//-------------------------------------------------------------------------

