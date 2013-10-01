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

#ifndef __AH_REP_PARSER_H__
#define __AH_REP_PARSER_H__

#include "data.h"
#include "hash.h"

extern const char * Monthes[];

extern const char * EOL_MS   ;
extern const char * EOL_UNIX ;
extern const char * EOL_SCR  ;
extern const char * EOL_FILE ;

#define DEFAULT_PLANE "Overworld"

typedef enum { North=0, Northeast,   Southeast,   South,   Southwest,   Northwest, Center }   eDirection;
extern int   ExitFlags [];
extern int   EntryFlags[];

extern int Flags_NW_N_NE;
extern int Flags_N      ;
extern int Flags_SW_S_SE;
extern int Flags_S      ;

// MZ - O_SHARE added for Arcadia
enum {
    O_ADDRESS = 1,
    O_ADVANCE,
    O_ARMOR,
    O_ASSASSINATE,
    O_ATTACK,
    O_AUTOTAX,
    O_AVOID,
    O_BEHIND,
    O_BUILD,
    O_BUY,
    O_CAST,
    O_CLAIM,
    O_COMBAT,
    O_CONSUME,
    O_DECLARE,
    O_DESCRIBE,
    O_DESTROY,
    O_ENDFORM,
    O_ENTER,
    O_ENTERTAIN,
    O_EVICT,
    O_EXCHANGE,
    O_FACTION,
    O_FIND,
    O_FORGET,
    O_FORM,
    O_GIVE,
    O_GIVEIF,
    O_TAKE,
    O_SEND,
    O_GUARD,
    O_HOLD,
    O_LEAVE,
    O_MOVE,
    O_NAME,
    O_NOAID,
    O_NOCROSS,
    O_NOSPOILS,
    O_OPTION,
    O_PASSWORD,
    O_PILLAGE,
    O_PREPARE,
    O_PRODUCE,
    O_PROMOTE,
    O_QUIT,
    O_RESTART,
    O_REVEAL,
    O_SAIL,
    O_SELL,
    O_SHARE,
    O_SHOW,
    O_SPOILS,
    O_STEAL,
    O_STUDY,
    O_TAX,
    O_TEACH,
    O_WEAPON,
    O_WITHDRAW,
    O_WORK,
    O_RECRUIT,


    O_TYPE,
    O_LABEL,

    // must be in this sequence! O_ENDXXX == O_XXX+1
    O_TURN,
    O_ENDTURN,
    O_TEMPLATE,
    O_ENDTEMPLATE,
    O_ALL,
    O_ENDALL,


    NORDERS
};


typedef struct SAVE_HEX_OPTIONS_STRUCT
{
    BOOL   SaveStructs;
    BOOL   AlwaysSaveImmobStructs;
    BOOL   SaveUnits;
    BOOL   SaveResources;
    long   WriteTurnNo; // Add turn number atlaclient style
} SAVE_HEX_OPTIONS;



//======================================================================

class CAtlaParser
{
public:
    CAtlaParser();
    CAtlaParser(CGameDataHelper * pHelper);
    ~CAtlaParser();
    void       Clear();
    int        ParseRep(const char * FNameIn, BOOL Join, BOOL IsHistory);     // History is a rep!
    int        SaveOrders  (const char * FNameOut, const char * password, BOOL decorate, int factid);
    int        LoadOrders  (const char * FNameIn, int & FactionId);  // return an id of the order's faction
    void       RunOrders(CLand * pLand, const char * sCheckTeach = NULL);
    BOOL       ShareSilver(CUnit * pMainUnit);
    BOOL       GenOrdersTeach(CUnit * pMainUnit);
    BOOL       GenGiveEverything(CUnit * pFrom, const char * To);
    BOOL       DiscardJunkItems(CUnit * pUnit, const char * junk);
    BOOL       DetectSpies(CUnit * pUnit, long lonum, long hinum, long amount);
    BOOL       ApplyDefaultOrders(BOOL EmptyOnly);
    int        ParseCBDataFile(const char * FNameIn);
    void       WriteMagesCSV(const char * FName, BOOL vertical, const char * separator, int format);

    CLand    * GetLand(int x, int y, int nPlane, BOOL AdjustForEdge=FALSE);
    CLand    * GetLand(long LandId);
    CLand    * GetLand(const char * landcoords); //  "48,52[,somewhere]"
    void       GetUnitList(CCollection * pResultColl, int x, int y, int z);
    void       CountMenForTheFaction(int FactionId);
    void       ComposeProductsLine(CLand * pLand, const char * eol, CStr & S);
    BOOL       LandStrCoordToId(const char * landcoords, long & id);
    void       ComposeLandStrCoord(CLand * pLand, CStr & LandStr);
    CFaction * GetFaction(int id);
    BOOL       SaveOneHex(CFileWriter & Dest, CLand * pLand, CPlane * pPlane, SAVE_HEX_OPTIONS * pOptions);
    long       SkillDaysToLevel(long days);
    BOOL       CheckResourcesForProduction(CUnit * pUnit, CLand * pLand, CStr & Error);


    int               m_CrntFactionId;
    CStr              m_CrntFactionPwd;
    CLongColl         m_OurFactions;
    CBaseObject       m_Events;
    CBaseObject       m_SecurityEvents;
    CBaseObject       m_HexEvents;
    CBaseObject       m_Errors;
    CBaseColl         m_NewProducts;

    CBaseCollById     m_Factions;
    CBaseCollById     m_Units;
    CBaseColl         m_Planes;
    long              m_YearMon;    // Current year/month accumulated for all loaded files
    CStringSortColl   m_UnitPropertyNames;
    CStrIntColl       m_UnitPropertyTypes;
    CStringSortColl   m_LandPropertyNames;
    //CStringSortColl   m_LandPropertyTypes;
    CBaseColl         m_Skills;
    CBaseColl         m_Items;
    CBaseColl         m_Objects;
//    CBaseCollByName   m_Battles;
    CBaseColl         m_Battles;
    CBaseCollById     m_Gates;

    long              m_nCurLine;
    long              m_GatesCount;
    int               m_ParseErr;
    BOOL              m_OrdersLoaded;
    CStr              m_FactionInfo;
    BOOL              m_ArcadiaSkills;

protected:
    int          ParseFactionInfo(BOOL GetNo, BOOL Join);
    int          ParseEvents(BOOL IsEvents=TRUE);
    int          ParseUnclSilver(CStr & Line);
    int          ParseAttitudes(CStr & Line, BOOL Join);
    int          ParseTerrain (CLand * pMotherLand, int ExitDir, CStr & FirstLine, BOOL FullMode, CLand ** ppParsedLand);
    int          AnalyzeTerrain(CLand * pMotherLand, CLand * pLand, BOOL IsExit, int ExitDir, CStr & Description);
    void         ComposeHexDescriptionForArnoGame(const char * olddescr, const char * newdescr, CStr & CompositeDescr);

    void         ParseWages(CLand * pLand, const char * str1, const char * str2);
    void         CheckExit(CPlane * pPlane, int Direction, CLand * pLandSrc, CLand * pLandExit);
    int          ParseUnit(CStr & FirstLine, BOOL Join);
    int          ParseStructure (CStr & FirstLine);
    int          ParseErrors();
    int          ParseLines(BOOL Join);
    BOOL         ParseOneUnitEvent(CStr & EventLine, BOOL IsEvent, int UnitId);
    BOOL         ParseOneLandEvent(CStr & EventLine, BOOL IsEvent);
    void         ParseOneMovementEvent(const char * params, const char * structid, const char * fullevent);
    int          ParseOneEvent(CStr & EventLine, BOOL IsEvent);
    void         ParseWeather(const char * src, CLand * pLand);
    int          ApplyLandFlags();
    int          SetLandFlag(const char * p, long flag);
    int          SetLandFlag(long LandId, long flag);
    int          ParseBattles();
    int          ParseSkills();
    int          ParseItems();
    int          ParseObjects();
    void         SetExitFlagsAndTropicZone();
    int          SetUnitProperty(CUnit * pUnit, const char * name, EValueType type, const void * value, EPropertyType proptype);
    int          SetLandProperty(CLand * pLand, const char * name, EValueType type, const void * value, EPropertyType proptype);
    int          LoadOrders  (CFileReader & F, int FactionId, BOOL GetComments);
    const char * ReadPropertyName(const char * src, CStr & Name);
    void         StoreBattle(CStr & Source);
    void         AnalyzeBattle(const char * src, CStr & Details);
    void         AnalyzeBattle_OneSide(const char * src, CStr & Details);
    const char * AnalyzeBattle_ParseUnit(const char * src, CUnit *& pUnit, BOOL & InFrontLine);
    void         AnalyzeBattle_SummarizeUnits(CBaseColl & Units, CStr & Details);
    void         SetShaftLinks();
    void         ApplySailingEvents();
    BOOL         GetTargetUnitId(const char *& p, long FactionId, long & nId);
    int          ParseOneImportantEvent(CStr & EventLine);
    int          ParseImportantEvents();

    CUnit      * MakeUnit(long Id);
    CPlane     * MakePlane(const char * planename);

    BOOL         ReadNextLine(CStr & s);
    void         PutLineBack (CStr & s);

    void         GenericErr(int Severity, const char * Msg);
    void         OrderErr(int Severity, int UnitId, const char * Msg);
    void         OrderErrFinalize();
    void         RunLandOrders(CLand * pLand, const char * sCheckTeach = NULL);
    void         OrderProcess_Teach(BOOL skiperror, CUnit * pUnit);

    // Order handlers and helpers
    void         RunOrder_Teach            (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params, BOOL TeachCheckGlb);
    void         RunOrder_Move             (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params, int & X, int & Y, int & LocA3, long order);
    void         RunOrder_Promote          (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params);
    void         RunOrder_Sell             (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params);
    void         RunOrder_Buy              (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params);
    void         RunOrder_Give             (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params, BOOL IgnoreMissingTarget);
    void         RunOrder_Take             (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params, BOOL IgnoreMissingTarget);
    void         RunOrder_Send             (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params);
    void         RunOrder_Produce          (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params);
    void         RunOrder_Study            (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params);
    void         RunOrder_Name             (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params);
    void         RunOrder_SailAIII         (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params, int & X, int & Y, int & LocA3);
    BOOL         FindTargetsForSend        (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char *& params, CUnit *& pUnit2, CLand *& pLand2);
    BOOL         GetItemAndAmountForGive   (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params, CStr & Item, int & amount, const char * command, CUnit * pUnit2);
    void         RunOrder_Withdraw         (CStr & Line, CStr & ErrorLine, BOOL skiperror, CUnit * pUnit, CLand * pLand, const char * params);

    void         AdjustSkillsAfterGivingMen(CUnit * pUnitGive, CUnit * pUnitTake, CStr & item, long AmountGiven);
    void         LookupAdvancedResourceVisibility(CUnit * pUnit, CLand * pLand);
    void         RunPseudoComment(int sequence, CLand * pLand, CUnit * pUnit, const char * src);

    int          ParseCBHex   (const char * FirstLine);
    int          ParseCBStruct(const char * FirstLine);

    CFileReader    * m_pSource;

    CStringSortColl  m_TaxLandStrs;
    CStringSortColl  m_TradeLandStrs;
    CStringSortColl  m_BattleLandStrs;
    CLongSortColl    m_TradeUnitIds;
    CBaseCollByName  m_PlanesNamed;
    CBaseColl        m_LandsToBeLinked;
    CHashStrToLong   m_UnitFlagsHash;
    CBaseColl        m_TempSailingEvents;

    CLand          * m_pCurLand   ;
    CStruct        * m_pCurStruct ;
    int              m_NextStructId;
    CStr             m_sOrderErrors;
    BOOL             m_JoiningRep;  // joining an allies' report
    BOOL             m_IsHistory;   // parsing history file
    long             m_CurYearMon; // Year/month for the file being loaded
    CStr             m_WeatherLine[8];
};



#endif

