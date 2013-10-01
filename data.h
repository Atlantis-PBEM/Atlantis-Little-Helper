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

#ifndef __DATA_H_INCL__
#define __DATA_H_INCL__


#include "cstr.h"
#include "collection.h"
#include "objs.h"
#include <string.h>
#include "compat.h"

typedef enum {GT=0,GE,   EQ,   LE,   LT,  NE, NOP} eCompareOp;

// common properties
#define PRP_ID                          "id"
#define PRP_NAME                        "name"
#define PRP_FULL_TEXT                   "fulltext"

// special properties
#define PRP_ORG_NAME                    "org$name"
#define PRP_ORG_DESCR                   "org$descr"

// unit properties
#define PRP_FACTION_ID                  "factionid"
#define PRP_FACTION                     "faction"
#define PRP_LAND_ID                     "landid"
#define PRP_STRUCT_ID                   "structid"
#define PRP_COMMENTS                    "comments"
#define PRP_ORDERS                      "orders"
#define PRP_STRUCT_OWNER                "structowner"
#define PRP_STRUCT_NAME                 "structname"
#define PRP_TEACHING                    "teaching"
#define PRP_WEIGHT                      "weight"
#define PRP_MOVEMENT                    "movement"
#define PRP_SILVER                      "silv"
#define PRP_LEADER                      "leadership"

#define PRP_FLAGS_STANDARD              "flags_standard"
#define PRP_FLAGS_CUSTOM                "flags_custom"
#define PRP_FLAGS_CUSTOM_ABBR           "flags_custom_abbr"


#define PRP_MEN                         "men"
#define PRP_SKILLS                      "skills"
#define PRP_MAG_SKILLS                  "mag_skills"
#define PRP_STUFF                       "stuff"
#define PRP_FOOD                        "foods"
#define PRP_HORS                        "mounts"
#define PRP_ARMOURS                     "armours"
#define PRP_WEAPONS                     "weapons"
#define PRP_MAG_ITEMS                   "mag_items"
#define PRP_JUNK_ITEMS                  "junk_items"
#define PRP_TRADE_ITEMS                 "trade_items"
#define PRP_SEL_FACT_MEN                "sel_fact_men"
#define PRP_SEQUENCE                    "sequence"
#define PRP_DESCRIPTION                 "description"
#define PRP_COMBAT                      "combat_spell"
#define PRP_FRIEND_OR_FOE               "attitude"

#define PRP_SKILL_POSTFIX                  "_"
#define PRP_SKILL_STUDY_POSTFIX            "_s"
#define PRP_SKILL_EXPERIENCE_POSTFIX       "_x"
#define PRP_SKILL_DAYS_POSTFIX             "_d"
#define PRP_SKILL_DAYS_EXPERIENCE_POSTFIX  "_dx"
#define PRP_GIVE_ALL_POSTFIX               "_ga"

// land properties
//#define PRP_SALE_AMOUNT_POSTFIX         "_s"
//#define PRP_SALE_PRICE_POSTFIX          "_s$"
//#define PRP_WANTED_AMOUNT_POSTFIX       "_w"
//#define PRP_WANTED_PRICE_POSTFIX        "_w$"
#define PRP_SALE_AMOUNT_PREFIX         "Sell.Amount."
#define PRP_SALE_PRICE_PREFIX          "Sell.Price."
#define PRP_WANTED_AMOUNT_PREFIX       "Want.Amount."
#define PRP_WANTED_PRICE_PREFIX        "Want.Price."
#define PRP_RESOURCE_PREFIX            "Resource."
#define PRP_LAND_LINK                  "*landlink"

#define MOVE_MODE_MAX                   5

#define LAND_FLAG_COUNT  3

// land flags
#define LAND_UNITS          0x00000001
#define LAND_VISITED        0x00000002
#define LAND_TAX            0x00000004
#define LAND_TRADE          0x00000008
#define LAND_BATTLE         0x00000010
#define LAND_SET_EXITS      0x00000020
#define LAND_HAS_FLAGS      0x00000040
#define LAND_IS_CURRENT     0x00000080

#define LAND_STR_HIDDEN     0x00000100
#define LAND_STR_MOBILE     0x00000200
#define LAND_STR_SHAFT      0x00000400
#define LAND_STR_GATE       0x00000800
#define LAND_STR_ROAD_N     0x00001000
#define LAND_STR_ROAD_NE    0x00002000
#define LAND_STR_ROAD_SE    0x00004000
#define LAND_STR_ROAD_S     0x00008000
#define LAND_STR_ROAD_SW    0x00010000
#define LAND_STR_ROAD_NW    0x00020000
#define LAND_STR_GENERIC    0x00040000
#define LAND_STR_SPECIAL   (LAND_STR_HIDDEN  | LAND_STR_MOBILE  | LAND_STR_SHAFT   | LAND_STR_GATE   | \
                            LAND_STR_ROAD_N  | LAND_STR_ROAD_NE | LAND_STR_ROAD_SE | LAND_STR_ROAD_S | \
                            LAND_STR_ROAD_SW | LAND_STR_ROAD_NW)

#define LAND_TAX_NEXT       0x00080000
#define LAND_TRADE_NEXT     0x00100000
#define LAND_LOCATED_UNITS  0x00200000
#define LAND_LOCATED_LAND   0x00400000
#define LAND_TOWN           0x00800000
#define LAND_CITY           0x01000000
#define LAND_IS_WATER       0x02000000

// alarm flags
#define PRESENCE_OWN        0x0001
#define PRESENCE_FRIEND     0x0002
#define PRESENCE_NEUTRAL    0x0004
#define PRESENCE_ENEMY      0x0008
#define GUARDED_BY_OWN      0x0010
#define GUARDED_BY_FRIEND   0x0020
#define GUARDED_BY_NEUTRAL  0x0040
#define GUARDED_BY_ENEMY    0x0080
#define GUARDED             0x0100
#define CLAIMED_BY_OWN      0x0200
#define CLAIMED_BY_FRIEND   0x0400
#define CLAIMED_BY_NEUTRAL  0x0800
#define CLAIMED_BY_ENEMY    0x1000
#define ALARM               0x2000


// structure attributes
#define SA_HIDDEN   0x0001
#define SA_MOBILE   0x0002
#define SA_SHAFT    0x0004
#define SA_GATE     0x0008
#define SA_ROAD_N   0x0010
#define SA_ROAD_NE  0x0020
#define SA_ROAD_SE  0x0040
#define SA_ROAD_S   0x0080
#define SA_ROAD_SW  0x0100
#define SA_ROAD_NW  0x0200
#define SA_ROAD_BAD 0x0400

// unit flags - standard flags from the top, custom from the bottom
#define UNIT_FLAG_GIVEN             0x80000000
#define UNIT_FLAG_TAXING            0x40000000
#define UNIT_FLAG_PRODUCING         0x20000000
#define UNIT_FLAG_GUARDING          0x10000000
#define UNIT_FLAG_AVOIDING          0x08000000
#define UNIT_FLAG_BEHIND            0x04000000
#define UNIT_FLAG_REVEALING_UNIT    0x02000000
#define UNIT_FLAG_REVEALING_FACTION 0x01000000
#define UNIT_FLAG_HOLDING           0x00800000
#define UNIT_FLAG_RECEIVING_NO_AID  0x00400000
#define UNIT_FLAG_CONSUMING_UNIT    0x00200000
#define UNIT_FLAG_CONSUMING_FACTION 0x00100000
#define UNIT_FLAG_NO_CROSS_WATER    0x00080000
#define UNIT_FLAG_SPOILS            0x00040000
#define UNIT_FLAG_SHARING           0x00020000
#define UNIT_FLAG_TEMP              0x00010000


#define UNIT_CUSTOM_FLAG_COUNT   8
#define UNIT_CUSTOM_FLAG_MASK    0xFF

#define NO_LOCATION         (-1)

extern const char * STD_UNIT_PROPS[];
extern const int    STD_UNIT_PROPS_COUNT;


/*extern const char * GRP_MEN;
extern const char * GRP_SKILLS;
extern const char * GRP_MAG_SKILLS;
extern const char * GRP_STUFF;
extern const char * GRP_HORS;
extern const char * GRP_MAG_ITEMS;
extern const char * GRP_JUNK_ITEMS;
extern const char * GRP_TRADE_ITEMS;
extern const char * GRP_ARMOURS;
extern const char * GRP_WEAPONS;*/
extern const char * STRUCT_GATE;

enum {
    ATT_FRIEND1     = 0,
    ATT_FRIEND2,
    ATT_NEUTRAL,
    ATT_ENEMY,
    ATT_UNDECLARED
};


/* it looks like there is no need for the new unit id to be related to land id
#define NEW_UNIT_ID(pLand,n) (-pLand->Id*100 - n)
*/
#define NEW_UNIT_ID(_n, _FactId) ((_FactId << 16) | _n)
#define REVERSE_NEW_UNIT_ID(_n) (_n & 0xFFFF)
#define IS_NEW_UNIT_ID(_Id)   ((_Id & 0xFFFF0000) != 0)
#define IS_NEW_UNIT(_pUnit)   IS_NEW_UNIT_ID(_pUnit->Id)




class CPlane;

//-----------------------------------------------------------------

class CBaseObject : public TPropertyHolder
{
public:
    CBaseObject();
    virtual ~CBaseObject(){};
    virtual void Done();


    virtual const char  * ResolveAlias(const char * alias);
    virtual BOOL GetProperty(const char  *  name,
                             EValueType   & type,
                             const void  *& value, // returns pointer to inner location
                             EPropertyType  proptype = eNormal
                            );
    long Id;
    CStr Name;
    CStr Description;

    void SetName(const char * newname);
    void SetDescription(const char * newdescr);
    void ResetName();
    void ResetDescription();
    virtual void ResetNormalProperties();

    virtual void DebugPrint(CStr & sDest);
    virtual void Clear() {Id=0; Name.Empty(); Description.Empty();};

};

//-----------------------------------------------------------------

class CBaseColl : public CCollection
{
public:
    CBaseColl();
    CBaseColl(int nDelta);
protected:
    virtual void FreeItem(void * pItem);
};

//-----------------------------------------------------------------

class CBaseCollById : public CSortedCollection
{
public:
    CBaseCollById();
    CBaseCollById(int nDelta);
protected:
    virtual void FreeItem(void * pItem);
    virtual int Compare(void * pItem1, void * pItem2) ;
};

//-----------------------------------------------------------------

class CProduct
{
public:
    long  Amount;
    CStr  ShortName;
    CStr  LongName;
};

//-----------------------------------------------------------------

class CProductColl : public CSortedCollection
{
public:
    CProductColl()            : CSortedCollection()      {};
    CProductColl(int nDelta)  : CSortedCollection(nDelta){};
protected:
    virtual void FreeItem(void * pItem) {delete (CProduct*)pItem;};
    virtual int Compare(void * pItem1, void * pItem2)
    {return(SafeCmp( ((CProduct*)pItem1)->ShortName.GetData(),
                     ((CProduct*)pItem2)->ShortName.GetData() ));};
};

//-----------------------------------------------------------------

class CFaction : public CBaseObject
{
public:
    CFaction() : CBaseObject() {UnclaimedSilver=0;};
    long UnclaimedSilver;

    virtual void DebugPrint(CStr & sDest);
};

//-----------------------------------------------------------------

class CAttitude : public CBaseObject
{
    public:
        int FactionId;
        int Stance;
        void    SetStance(int new_stance);
        BOOL    IsDeclaredAs(int attitude);
        BOOL    IsEqual(CAttitude *attitude);
};

//-----------------------------------------------------------------

class CUnit : public CBaseObject
{
public:
    CUnit();
    virtual ~CUnit();
    virtual BOOL GetProperty(const char  *  name,
                             EValueType   & type,
                             const void  *& value, // returns pointer to inner location
                             EPropertyType  proptype = eNormal
                            );
    virtual CStrStrColl * GetPropertyGroups();
    void    ExtractCommentsFromDefOrders();
    virtual void ResetNormalProperties();
    void    CheckWeight(CStr & sErr);
    void    CalcWeightsAndMovement();

    CUnit * AllocSimpleCopy();

    static void         LoadCustomFlagNames();
    static void         ResetCustomFlagNames();
    static const char * GetCustomFlagName(int no);

    bool            IsOurs;
    long            FactionId;
    CFaction      * pFaction;
    long            LandId;
    long            Weight[MOVE_MODE_MAX];
    long            SilvRcvd;
    double          Teaching; // number of students per teacher / number of days per student
    CStr            Comments;
    CStr            DefOrders;
    CStr            Orders;
    CStr            OrdersDecorated;
    CStr            Errors;
    CStr            Events;
    CStr            StudyingSkill;
    CStr            ProducingItem;
    CLongColl     * pMovement; // Collection of ids of hexes to move through
    CLongColl     * pMoveA3Points; // Collection of Arcadia III locations for movement
    CBaseCollById * pStudents;
    unsigned long   Flags;
    unsigned long   FlagsOrg;
    unsigned long   FlagsLast;

    // potentially can be moved to some bitflags field
    bool            IsWorking;

    static CStrStrColl * m_PropertyGroupsColl;

    virtual void DebugPrint(CStr & sDest);
protected:
    void    AddWeight(int nitems, int * weights, const char ** movenames, int nweights);
    
    static CStr     m_CustomFlagNames[UNIT_CUSTOM_FLAG_COUNT];
    static BOOL     m_CustomFlagNamesLoaded;

};

//-----------------------------------------------------------------

class CStruct : public CBaseObject
{
public:
    CStruct() : CBaseObject() {LandId=0; OwnerUnitId=0; Attr=0; Location=NO_LOCATION;
                               Load=0; SailingPower=0; MaxLoad=0; MinSailingPower=0;};
    virtual void ResetNormalProperties();
    long LandId;
    long OwnerUnitId;
    long Attr;
    CStr Kind;
    int  Location;
    long Load;
    long SailingPower;
    long MaxLoad;
    long MinSailingPower;
};

//-----------------------------------------------------------------

/*
class CEdgeStruct : public CStruct
{
public:
    CEdgeStruct() : CStruct() {};
    int   Direction;
};
*/

//-----------------------------------------------------------------

class CLand : public CBaseObject
{
public:
    CLand();
    virtual ~CLand();

    BOOL      AddUnit(CUnit * pUnit);
    void      RemoveUnit(CUnit * pUnit);
    void      ResetUnitsAndStructs();
    CStruct * GetStructById(long id);
    void      CalcStructsLoad();
    void      SetFlagsFromUnits();
    CStruct * AddNewStruct(CStruct * pNewStruct);
    void      RemoveEdgeStructs(int direction);
    void      AddNewEdgeStruct(const char * name, int direction);
    int       GetNextNewUnitNo();


    virtual void DebugPrint(CStr & sDest);

    long          Taxable;
    long          Peasants;
    CStr          PeasantRace;
    CStr          TerrainType;
    CStr          CityName;
    CStr          CityType;
    CStr          FlagText[LAND_FLAG_COUNT];
    CStr          Exits;
    CStr          Events;
    CBaseCollById Structs;
    CBaseCollById Units;
    CBaseColl     UnitsSeq; // this will keep units in the sequence they were met in the report
    CBaseColl     EdgeStructs;
    CProductColl  Products;
    unsigned long Flags;
    unsigned long AlarmFlags;
    unsigned long EventFlags;
    int           ExitBits;
    int           CoastBits;
    CPlane      * pPlane;
    int           AtlaclientsLastTurnNo;
    BOOL          WeatherWillBeGood;
    double        Wages;
    long          MaxWages;
    long          Troops[ATT_UNDECLARED+1];

    long          guiUnit;  // will be used by GUI only
    int           guiColor; // will be used by GUI only
};

//-----------------------------------------------------------------

#define TROPIC_ZONE_MAX   0x00001000    // should be no less than XY_DELTA in data.cpp!

class CPlane : public CBaseObject
{
public:
    CPlane() : Lands(32) {EastEdge=0;  WestEdge=0;   Width=0;
                          EdgeSrcId=0; EdgeExitId=0; EdgeDir=0; ExitsCount=0;
                          TropicZoneMin  = TROPIC_ZONE_MAX; TropicZoneMax  = -(TROPIC_ZONE_MAX);
                         };
    virtual ~CPlane();

    CBaseCollById Lands;
    int           EastEdge;
    int           WestEdge;
    int           Width;

    long          EdgeSrcId ;
    long          EdgeExitId;
    int           EdgeDir   ;
    long          ExitsCount; // is used to determine 1.0.0 history file

    long          TropicZoneMin;
    long          TropicZoneMax;

};

//-----------------------------------------------------------------

class CShortNamedObj : public CBaseObject
{
public:
    CShortNamedObj() : CBaseObject() {Level = 0;};
    CStr ShortName;
    long Level;

};

class CBattle : public CBaseObject
{
public:
    CStr LandStrId;
};

//-----------------------------------------------------------------

class CBaseCollByName : public CBaseCollById
{
public:
    CBaseCollByName()           : CBaseCollById()       {};
    CBaseCollByName(int nDelta) : CBaseCollById(nDelta) {};
protected:
    virtual int Compare(void * pItem1, void * pItem2) ;
};

//-----------------------------------------------------------------

class CUnitsByHex : public CSortedCollection
{
public:
    CUnitsByHex() : CSortedCollection() {};
    CUnitsByHex(int nDelta) : CSortedCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem) {};
    virtual int Compare(void * pItem1, void * pItem2)
    {
        CUnit * p1 = (CUnit*)pItem1;
        CUnit * p2 = (CUnit*)pItem2;

        if (p1->LandId > p2->LandId)
            return 1;
        else
            if (p1->LandId < p2->LandId)
                return -1;
            else
                if (p1->Id > p2->Id)
                    return 1;
                else
                    if (p1->Id < p2->Id)
                        return -1;
                    else
                        return 0;
    };
};

//-----------------------------------------------------------------

#define MAX_RES_NUM 8

class TProdDetails
{
public:
    CStr skillname;
    long skilllevel;

    long months;
    CStr resname[MAX_RES_NUM];
    long resamt[MAX_RES_NUM];

    CStr toolname;
    long toolhelp;

    void Empty();
};

//-----------------------------------------------------------------

class CGameDataHelper
{
public:
    void         ReportError       (const char * msg, int msglen, BOOL orderrelated);
    long         GetStudyCost      (const char * skill);
    long         GetStructAttr     (const char * kind, long & MaxLoad, long & MinSailingPower);
    const char * GetConfString     (const char * section, const char * param);
    BOOL         GetOrderId        (const char * order, long & id);
    BOOL         IsTradeItem       (const char * item);
    BOOL         IsMan             (const char * item);
    const char * GetWeatherLine    (BOOL IsCurrent, BOOL IsGood, int Zone);
    const char * ResolveAlias      (const char * alias);
    BOOL         GetItemWeights    (const char * item, int *& weights, const char **& movenames, int & movecount );
    void         GetMoveNames      (const char **& movenames);
    BOOL         GetTropicZone     (const char * plane, long & y_min, long & y_max);
    void         SetTropicZone     (const char * plane, long y_min, long y_max);
    void         GetProdDetails    (const char * item, TProdDetails & details);
    long         MaxSkillLevel     (const char * race, const char * skill, const char * leadership, BOOL IsArcadiaSkillSystem);
    BOOL         ImmediateProdCheck();
    BOOL         CanSeeAdvResources(const char * skillname, const char * terrain, CLongColl & Levels, CBufColl & Resources);
    BOOL         ShowMoveWarnings  ();
    BOOL         IsRawMagicSkill   (const char * skillname);
    int          GetAttitudeForFaction(int id);
    void         SetAttitudeForFaction(int id, int attitude);
    void         SetPlayingFaction (long id);
    BOOL         IsWagon           (const char * item);
    BOOL         IsWagonPuller     (const char * item);
    int          WagonCapacity     ();
};

extern CGameDataHelper * gpDataHelper;

//-----------------------------------------------------------------

class CTaxProdDetails
{
public:
    CTaxProdDetails() {HexCount=0; FactionId=0; amount=0;}

    long  FactionId;
    long  HexCount;
    long  amount;
    CStr  Details;
};

class CTaxProdDetailsCollByFaction : public CSortedCollection
{
    public:
        CTaxProdDetailsCollByFaction() : CSortedCollection() {};
        CTaxProdDetailsCollByFaction(int nDelta) : CSortedCollection(nDelta) {};
    protected:
        virtual void FreeItem(void * pItem)
        {
            CTaxProdDetails * p = (CTaxProdDetails*)pItem;
            delete p;
        };
        virtual int Compare(void * pItem1, void * pItem2)
        {
            CTaxProdDetails * p1 = (CTaxProdDetails*)pItem1;
            CTaxProdDetails * p2 = (CTaxProdDetails*)pItem2;

            if (p1->FactionId > p2->FactionId)
                return 1;
            else
                if (p1->FactionId < p2->FactionId)
                    return -1;
            else
                return 0;
        };
};

//-----------------------------------------------------------------



long LandCoordToId(int x, int y, int z);
void LandIdToCoord(long id, int & x, int & y, int & z);
void TestLandId();
BOOL IsASkillRelatedProperty(const char * propname);
void MakeQualifiedPropertyName(const char * prefix, const char * shortname, CStr & FullName);
void SplitQualifiedPropertyName(const char * fullname, CStr & Prefix, CStr & ShortName);
BOOL EvaluateBaseObjectByBoxes(CBaseObject * pObj, CStr * Property, eCompareOp * CompareOp, CStr * Value, long * lValue, int count);

#endif
