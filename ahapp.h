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

#ifndef __AH_APP_INCL__
#define __AH_APP_INCL__


enum
{
    AH_FRAME_MAP          =  0,
    AH_FRAME_UNITS            ,
    AH_FRAME_MSG              ,
    AH_FRAME_EDITS            ,
    AH_FRAME_UNITS_FLTR       ,

    AH_FRAME_COUNT
};

enum
{
    AH_PANE_MAP            = 0,
    AH_PANE_MAP_DESCR         ,
    AH_PANE_UNITS_HEX         ,
    AH_PANE_UNITS_FILTER      ,
    AH_PANE_UNIT_DESCR        ,
    AH_PANE_UNIT_COMMANDS     ,
    AH_PANE_UNIT_COMMENTS     ,
    AH_PANE_MSG               ,

    AH_PANE_COUNT             // whenever adding new panes, update CAhApp::ApplyFonts and CAhApp::ApplyColors!
};

enum
{
    AH_LAYOUT_2_WIN        = 0,
    AH_LAYOUT_3_WIN           ,
    AH_LAYOUT_1_WIN           ,

    AH_LAYOUT_COUNT
};

enum
{
    FONT_EDIT_DESCR        = 0,
    FONT_EDIT_ORDER           ,
    FONT_MAP_COORD            ,
    FONT_MAP_TEXT             ,
    FONT_UNIT_LIST            ,
    FONT_EDIT_HDR             ,
    FONT_VIEW_DLG             ,
    FONT_ERR_DLG              ,

    FONT_COUNT
};

enum
{
    CONFIG_FILE_CONFIG     = 0,
    CONFIG_FILE_STATE         ,

    CONFIG_FILE_COUNT
};

class CAhFrame;
class CEditPane;
class CHexFilterDlg;

enum eRepSeq  {repFirst, repPrev, repNext, repLast, repLastVisited};
enum eHexIncl {HexNew,
               HexCurrent,
               HexSelected, // inside a rectangle by mouse dragging
               HexAll,
               };

void FontToStr(const wxFont * font, CStr & s);
wxFont * NewFontFromStr(const char * p);

void StrToColor(wxColour * cr, const char * p);
void ColorToStr(char * p, wxColour * cr);
void MakePathRelative(const char * cur_dir, CStr & path);
void MakePathFull(const char * cur_dir, CStr & path);
void GetDirFromPath(const char * path, CStr & dir);
void GetFileFromPath(const char * path, CStr & file);

#define APPLY_COLOR_DELTA(x) ((unsigned char )(max(min((int)x-gpApp->m_Brightness_Delta,255),0)))



//-------------------------------------------------------------------------------

class CAtlaSortColl : public CSortedCollection
{
public:
    CAtlaSortColl()           : CSortedCollection() {};
    CAtlaSortColl(int nDelta) : CSortedCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem)
    {
        if (pItem)
            delete (CAtlaParser*)pItem;
    };
    virtual int Compare(void * pItem1, void * pItem2)
    {
        if ( ((CAtlaParser*)pItem1)->m_YearMon > ((CAtlaParser*)pItem2)->m_YearMon)
            return 1;
        else
            if ( ((CAtlaParser*)pItem1)->m_YearMon < ((CAtlaParser*)pItem2)->m_YearMon)
                return -1;
            else
                return 0;
    };
};

//-------------------------------------------------------------------------------

struct ItemWeights
{
    char * name;
    int  * weights;
};

class CWeightsColl : public CSortedCollection
{
public:
    CWeightsColl()           : CSortedCollection() {};
    CWeightsColl(int nDelta) : CSortedCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem)
    {
        if (pItem)
        {
            if (((ItemWeights*)pItem)->name)
                free(((ItemWeights*)pItem)->name);
            if (((ItemWeights*)pItem)->weights)
                free(((ItemWeights*)pItem)->weights);
            delete (ItemWeights*)pItem;
        }
    };
    virtual int Compare(void * pItem1, void * pItem2)
    {
        return SafeCmp(((ItemWeights*)pItem1)->name, ((ItemWeights*)pItem2)->name);
    };
};


//-------------------------------------------------------------------------------

class CAhApp : public wxApp
{
public:
    CAhApp();
    ~CAhApp();

    virtual bool         OnInit();
    virtual int          OnExit();


    void                 SetConfig(const char * szSection, const char * szName, const char * szNewValue);
    void                 SetConfig(const char * szSection, const char * szName, long lNewValue);
    const char         * GetConfig(const char * szSection, const char * szName);
    int                  GetSectionFirst(const char * szSection, const char *& szName, const char *& szValue);
    int                  GetSectionNext (int idx, const char * szSection, const char *& szName, const char *& szValue);
    void                 RemoveSection(const char * szSection);
    const char         * GetNextSectionName(int fileno, const char * szStart); // sorry, but fileno is needed here
    void                 MoveSectionEntries(int fileno, const char * src, const char * dest);

    void                 EditPaneChanged(CEditPane * pPane);
    void                 EditPaneDClicked(CEditPane * pPane);


    void                 FrameClosing(CAhFrame * pFrame);

    void                 ShowError (const char * msg, int msglen, BOOL ignore_disabled);
    long                 GetStudyCost   (const char * skill);
    const char         * ResolveAlias   (const char * alias);
    long                 GetStructAttr  (const char * kind, long & MaxLoad, long & MinSailingPower);
    BOOL                 GetItemWeights (const char * item, int *& weights, const char **& movenames, int & movecount );
    void                 GetMoveNames(const char **& movenames);

    BOOL                 GetOrderId     (const char * order, long & id);
    BOOL                 IsTradeItem    (const char * item);
    BOOL                 IsMan          (const char * item);
    BOOL                 IsMagicSkill   (const char * skill);
    const char *         GetWeatherLine (BOOL IsCurrent, BOOL IsGood, int Zone);
    void                 GetProdDetails (const char * item, TProdDetails & details);
    long                 GetMaxRaceSkillLevel(const char * race, const char * skill, const char * leadership, BOOL IsArcadiaSkillSystem);
    BOOL                 CanSeeAdvResources(const char * skillname, const char * terrain, CLongColl & Levels, CBufColl & Resources);
    int                  GetAttitudeForFaction(int id);
    void                 SetAttitudeForFaction(int id, int attitude);



    int                  LoadReport(BOOL Join);
    int                  LoadReport(const char * FNameIn, BOOL Join);
    int                  SaveOrders(BOOL UsingExistingName);
    void                 LoadOrders();
    CUnit              * GetSelectedUnit();
    BOOL                 CanCloseApp();
    void                 Redraw();
    void                 ApplyFonts();
    void                 ApplyColors();
    void                 ApplyIcons();

    void                 SelectUnit(CUnit * pUnit);
    BOOL                 SelectLand(const char * landcoords); //  "48,52[,somewhere]"
    void                 SelectLand(CLand * pLand);

    void                 SwitchToRep(eRepSeq whichrep);
    BOOL                 CanSwitchToRep(eRepSeq whichrep, int & RepIdx);

    void                 OnMapSelectionChange();
    void                 OnUnitHexSelectionChange(long idx);

    void                 OpenOptionsDlg();
    void                 OpenUnitFrame();
    void                 OpenMsgFrame();
    void                 OpenEditsFrame();
    void                 OpenUnitFrameFltr(BOOL PopUpSettings);
    void                 WriteMagesCSV();
    void                 ShowDescriptionList(CCollection & Items, const char * title); // Collection of CBaseObject
//    void                 ViewSkills(BOOL ViewAll);
    void                 ViewShortNamedObjects(BOOL ViewAll, const char * szSection, const char * szHeader, CBaseColl & ListNew);
    void                 ViewBattlesAll();
    void                 ViewEvents(BOOL DoEvents);
    void                 ViewSecurityEvents();
    void                 ViewNewProducts();
    void                 ViewGates();
    void                 ViewCities();
    void                 ViewProvinces();
    void                 ViewFactionInfo();
    void                 ViewFactionOverview();
    void                 ViewFactionOverview_IncrementValue(long FactionId, const char * factionname, CBaseCollById & Factions, const char * propname, long value);
    void                 CheckMonthLongOrders();
    void                 CheckProduction();
    void                 CheckSailing();
    void                 CheckTaxDetails  (CLand  * pLand, CTaxProdDetailsCollByFaction & TaxDetails);
    void                 CheckTradeDetails(CLand  * pLand, CTaxProdDetailsCollByFaction & TradeDetails);
    void                 CheckTaxTrade();
    void                 ExportHexes();
    void                 FindTradeRoutes();
    void                 EditListColumns(int command);
    const char         * GetListColSection(const char * sectprefix, const char * key);
    void                 ViewMovedUnits();
    BOOL                 GetPrevTurnReport(CAtlaParser *& pPrevTurn); 

    void                 CreateAccelerator();

    BOOL                 GetOrdersChanged(){return m_OrdersAreChanged;};
    void                 SetOrdersChanged(BOOL Changed);
    void                 StdRedirectReadMore(BOOL FromStdout, CStr & sData);
    void                 CheckRedirectedOutputFiles();
    void                 RerunOrders();
    void                 SetAllLandUnitFlags();
    void                 ShowUnitsMovingIntoHex(long CurHexId, CPlane * pCurPlane);
    void                 ShowLandFinancial(CLand * pCurLand);
    void                 AddTempHex(int X, int Y, int Plane);
    void                 DelTempHex(int X, int Y, int Plane);
    
    void                 SelectNextUnit();
    void                 SelectPrevUnit();
    void                 SelectUnitsPane();
    void                 SelectOrdersPane();

    CAtlaParser        * m_pAtlantis;

    CAhFrame           * m_Frames[AH_FRAME_COUNT];
    wxWindow           * m_Panes [AH_PANE_COUNT ];
    wxFont             * m_Fonts [FONT_COUNT];
    const char         * m_FontDescr[FONT_COUNT];
    CStrStrColl          m_UnitPropertyGroups;

//    BOOL                 m_LandFlagsChanged;
    BOOL                 m_CommentsChanged;
    BOOL                 m_DiscardChanges;
    BOOL                 m_UpgradeLandFlags;
    wxAcceleratorTable * m_pAccel;
    long                 m_Brightness_Delta;


private:
    void                 ForgetFrame(int no, BOOL frameclosed);
    int                  LoadOrders  (const char * FNameIn);
    int                  SaveOrders  (const char * FNameOut, int FactionId);
    int                  SaveHistory (const char * FNameOut);
    void                 LoadComments();
    void                 SaveComments();
    void                 PostLoadReport();
    void                 PreLoadReport();
    void                 RedrawTracks();
    void                 OpenMapFrame();
    void                 GetShortFactName(CStr & S, int FactionId);
    void                 SaveLandFlags();
    void                 LoadLandFlags();
    void                 UpdateEdgeStructs();
    void                 LoadUnitFlags();
    void                 SaveUnitFlags();

    void                 UpdateHexEditPane(CLand * pLand);
    void                 UpdateHexUnitList(CLand * pLand);
    void                 SwitchToYearMon(long YearMon);

    int                  GetConfigFileNo(const char * szSection);
    void                 UpgradeConfigFiles();
    void                 UpgradeConfigByFactionId();
    void                 ComposeConfigOrdersSection(CStr & Sect, int FactionId);
    BOOL                 GetExportHexOptions(CStr & FName, CStr & FMode, SAVE_HEX_OPTIONS & options, eHexIncl & HexIncl,
                                             bool & InclTurnNoAcl);
    void                 ExportOneHex(CFileWriter & Dest, CPlane * pPlane, CLand * pLand, SAVE_HEX_OPTIONS & options, bool InclTurnNoAcl, bool OnlyNew);
    void                 SetMapFrameTitle();
    void                 StdRedirectInit();
    void                 StdRedirectDone();
    void                 InitMoveModes();
    void                 SelectTempUnit(CUnit * pUnit);


    CAtlaSortColl        m_Reports;
    CLongSortColl        m_ReportDates;
    BOOL                 m_FirstLoad;
    CStr                 m_HexDescrSrc;
    CStr                 m_UnitDescrSrc;
    CStr                 m_MsgSrc;
    long                 m_SelUnitIdx;
    int                  m_layout;
    BOOL                 m_DisableErrs;
    CBufColl             m_MoveModes;
    CWeightsColl         m_ItemWeights;
    CConfigFile          m_Config[CONFIG_FILE_COUNT];
    CStringSortColl      m_ConfigSectionsState;
    BOOL                 m_OrdersAreChanged;
    CStr                 m_sTitle;
    CHashStrToLong       m_OrderHash;
    CHashStrToLong       m_TradeItemsHash;
    CHashStrToLong       m_MenHash;
    CHashStrToLong       m_MaxSkillHash;
    CHashStrToLong       m_MagicSkillsHash;
    int                  m_nStdoutLastPos;
    int                  m_nStderrLastPos;
    CBaseColl            m_Attitudes;
    CStringSortColl      m_WaterTerrainNames;

};

//-------------------------------------------------------------------------------

extern CAhApp * gpApp;


#endif

