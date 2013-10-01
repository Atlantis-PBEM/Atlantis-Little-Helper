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

#ifndef __CONSTANTS_FOR_US__
#define __CONSTANTS_FOR_US__

typedef struct
{
    const char * szSection;
    const char * szName;
    const char * szValue;
} DefaultConfigRec;

extern DefaultConfigRec DefaultConfig[];
extern int              DefaultConfigSize;

static const char AH_VERSION[] =
#include "version_no"
;

enum
{
    // main menu
    menu_LoadReport    = 1  ,
    menu_JoinReport         ,
    menu_LoadOrders         ,
    menu_SaveOrders         ,
    menu_SaveOrdersAs       ,
    menu_Quit               ,
    menu_QuitNoSave         ,
    menu_Options            ,
    menu_ViewSkillsAll      ,
    menu_ViewSkillsNew      ,
    menu_ViewItemsAll       ,
    menu_ViewItemsNew       ,
    menu_ViewObjectsAll     ,
    menu_ViewObjectsNew     ,
    menu_ViewBattlesAll     ,
    menu_ViewEvents         ,
    menu_ViewErrors         ,
    menu_ViewGates          ,
    menu_ViewCities         ,
    menu_ViewProvinces      ,
    menu_ViewNewProducts    ,
    menu_ViewFactionInfo    ,
    menu_ViewFactionOverview,
    menu_ViewSecurityEvents ,
    menu_WindowUnits        ,
    menu_WindowMessages     ,
    menu_WindowEditors      ,
    menu_WindowUnitsFltr    ,
    menu_ListColUnits       ,
    menu_ListColUnitsFltr   ,
    menu_ListColumns        ,
    menu_ApplyDefaultOrders ,
    menu_RerunOrders        ,
    menu_WriteMagesCSV      ,
    menu_CheckMonthLongOrd  ,
    menu_CheckTaxTrade      ,
    menu_CheckProduction    ,
    menu_CheckSailing       ,
    menu_FindHexes          ,
    menu_ExportHexes        ,
    menu_FlagNames          ,
    menu_FlagsAllSet        ,
    menu_FindTradeRoutes    ,
    menu_About              ,

    // map popup menu
    menu_Popup_Flag         ,
    menu_Popup_Center       ,
    menu_Popup_ShareSilv    ,
    menu_Popup_Teach        ,
    menu_Popup_Split        ,
    menu_Popup_DiscardJunk  ,
    menu_Popup_DetectSpies  ,
    menu_Popup_Filter       ,
    menu_Popup_SetSort      ,
    menu_Popup_Battles      ,
    menu_Popup_GiveEverything,
    menu_Popup_AddToTracking,
    menu_Popup_UnitFlags    ,
    menu_Popup_IssueOrders  ,
    menu_Popup_WhoMovesHere ,
    menu_Popup_Financial    ,
    menu_Popup_New_Hex      ,
    menu_Popup_Del_Hex      ,
    menu_Popup_DistanceRing ,

    // accelerator key
    accel_NextUnit          ,
    accel_PrevUnit          ,
    accel_UnitList          ,
    accel_Orders            ,

    // toolbar. order corresponds to one used in CMapFrame::MakeToolBar()

    tool_zoomin             ,
    tool_zoomout            ,
    tool_centerout          ,
    tool_prevzoom           ,
    tool_showcoord          ,
    tool_shownames          ,
    tool_planeup            ,
    tool_planedwn           ,
    tool_lastvisitturn      ,
    tool_prevturn           ,
    tool_nextturn           ,
    tool_lastturn           ,
    tool_findhex            ,


    // other
    list_units_hex          ,
    list_units_hex_fltr
};

#if defined(_WIN32)
  #define SZ_REP_FILES                  "Report (*.rep)|*.rep|Report (*.atl)|*.atl|Text (*.txt)|*.txt|All|*.*||"
  #define SZ_ORD_FILES                  "Order (*.ord)|*.ord|Text (*.txt)|*.txt|All|*.*||"
  #define SZ_CSV_FILES                  "Comma Separated (*.csv)|*.csv|All|*.*||"
  #define SZ_ALL_FILES                  "All|*.*||"
#else
  #define SZ_REP_FILES                  "Report (*.rep)|*.rep|Report (*.atl)|*.atl|Text (*.txt)|*.txt|All|*||"
  #define SZ_ORD_FILES                  "Order (*.ord)|*.ord|Text (*.txt)|*.txt|All|*||"
  #define SZ_CSV_FILES                  "Comma Separated (*.csv)|*.csv|All|*||"
  #define SZ_ALL_FILES                  "All|*||"
#endif

#define SZ_CB_DATA_FILES                "Text (*.txt)|*.txt|All|*.*||"
#define SZ_CONFIG_FILE                  "ah.cfg"
#define SZ_CONFIG_STATE_FILE            "ah.st.cfg"
#define SZ_HISTORY_FILE                 "ah.his"
#define SZ_COMMON_PY_FILE               "ah.common.py"

#define SZ_EOL_MS                       "MS"
#define SZ_EOL_UNIX                     "UNIX"


// config sections
#define SZ_SECT_COLORS                  "COLORS"
#define SZ_SECT_COMMON                  "COMMON"
#define SZ_SECT_FONTS                   "FONTS"
#define SZ_SECT_FONTS_2                 "FONTS_2"
#define SZ_SECT_UNITLIST_HDR            "UNIT_LIST_HDR"
#define SZ_SECT_UNITLIST_HDR_FLTR       "UNIT_LIST_HDR_FLTR"
#define SZ_SECT_UNITPROP_GROUPS         "UNIT_PROPERTY_GROUPS"
#define SZ_SECT_STUDY_COST              "STUDYING_COST"
#define SZ_SECT_ALIAS                   "ALIASES"
#define SZ_SECT_STRUCTS                 "STRUCTURES"
#define SZ_SECT_PROD_SKILL              "PRODUCTION_SKILL"
#define SZ_SECT_PROD_RESOURCE           "PRODUCTION_RESOURCE"
#define SZ_SECT_PROD_TOOL               "PRODUCTION_TOOL"
#define SZ_SECT_PROD_MONTHS             "PRODUCTION_MONTHS"
#define SZ_SECT_MAX_SKILL_LVL           "MAX_SKILL_LEVELS"
#define SZ_SECT_MAX_MAG_SKILL_LVL       "MAX_SKILL_LEVELS_MAGIC"

#define SZ_SECT_MAP_PANE                "PANE_MAP"
#define SZ_SECT_WEIGHT_MOVE             "WEIGHTS_CAPACITIES"
#define SZ_SECT_UNIT_FLAG_NAMES         "UNIT_FLAG_NAMES"
#define SZ_SECT_WND_MAP_2_WIN           "WINDOW_MAP"
#define SZ_SECT_WND_MAP_3_WIN           "WINDOW_MAP_1"
#define SZ_SECT_WND_MAP_1_WIN           "WINDOW_MAP_2"
#define SZ_SECT_WND_UNITS_2_WIN         "WINDOW_UNITS"
#define SZ_SECT_WND_UNITS_3_WIN         "WINDOW_UNITS_1"
#define SZ_SECT_WND_UNITS_FLTR          "WINDOW_UNITS_FLTR"
#define SZ_SECT_WND_HEX_FLTR            "WINDOW_HEX_FLTR"
#define SZ_SECT_WND_EDITS_2_WIN         "WINDOW_EDITORS"
#define SZ_SECT_WND_EDITS_3_WIN         "WINDOW_EDITORS_1"
#define SZ_SECT_WND_MSG                 "WINDOW_MESSAGES"
#define SZ_SECT_WND_DESCR_ONE           "WINDOW_DESCRIPTION_ONE"
#define SZ_SECT_WND_DESCR_LIST          "WINDOW_DESCRIPTION_LIST"
#define SZ_SECT_WND_EXP_MAGES_CSV       "WINDOW_EXPORT_MAGES_CSV"
#define SZ_SECT_WND_EXP_HEXES           "WINDOW_EXPORT_HEXES_DLG"
#define SZ_SECT_WND_UNITS_FLTR_DLG      "WINDOW_UNITS_FLTR_DLG"
#define SZ_SECT_WND_HEX_FLTR_DLG        "WINDOW_HEX_FLTR_DLG"
#define SZ_SECT_WND_LST_COLEDIT_DLG     "WINDOW_LIST_COL_EDIT_DLG"
#define SZ_SECT_WND_OPTIONS_DLG         "WINDOW_OPTIONS"
#define SZ_SECT_WND_SPLIT_UNIT_DLG      "WINDOW_SPLIT_UNIT_DLG"
#define SZ_SECT_WND_LAND_UNIT_FLAGS_DLG "WINDOW_LAND_UNIT_FLAGS_DLG"
#define SZ_SECT_WND_HEX_FLAGS_DLG       "WINDOW_HEX_FLAGS_DLG"
#define SZ_SECT_WND_GET_TEXT_DLG        "WINDOW_GET_TEXT_DLG"

#define SZ_SECT_LIST_COL_CURRENT        "LIST_COL_CUR_SET"
#define SZ_SECT_LIST_COL_UNIT           "LIST_COL_UNIT_"
#define SZ_SECT_LIST_COL_UNIT_DEF       "LIST_COL_UNIT_DEFAULT"
#define SZ_SECT_LIST_COL_UNIT_FLTR_DEF  "LIST_COL_UNIT_DEFAULT_FLTR"

#define SZ_SECT_UNIT_FILTER             "UNIT_FLTR_"
#define SZ_SECT_HEX_FILTER              "HEX_FLTR_"
#define SZ_SECT_EDGE_STRUCTS            "EDGE_STRUCTURES"
#define SZ_SECT_WEATHER                 "WEATHER"
#define SZ_SECT_RESOURCE_SKILL          "RESOURCE_SKILL"
#define SZ_SECT_RESOURCE_LAND           "RESOURCE_LAND"
#define SZ_SECT_ATTITUDES               "ATTITUDES"

// config state sections
#define SZ_SECT_DEF_ORDERS              "DEFAULT_ORDERS"
#define SZ_SECT_ORDERS                  "ORDER_FILES"
#define SZ_SECT_REPORTS                 "REPORT_FILES"
#define SZ_SECT_LAND_FLAGS              "LAND_FLAGS"
#define SZ_SECT_LAND_VISITED            "LAND_LAST_VISITED"
#define SZ_SECT_SKILLS                  "SKILL_DESCRIPTIONS"
#define SZ_SECT_ITEMS                   "ITEM_DESCRIPTIONS"
#define SZ_SECT_OBJECTS                 "OBJECT_DESCRIPTIONS"
#define SZ_SECT_PASSWORDS               "PASSWORDS"
#define SZ_SECT_UNIT_TRACKING           "UNIT_TRACKING_GROUPS"
#define SZ_SECT_FOLDERS                 "FOLDERS"
#define SZ_SECT_DO_NOT_SHOW_THESE       "DO_NOT_SHOW_THESE"
#define SZ_SECT_TROPIC_ZONE             "TROPIC_ZONE"
#define SZ_SECT_UNIT_FLAGS              "UNIT_FLAGS"

// for 1.0 compatibility
#define SZ_SECT_WND_MAP                  SZ_SECT_WND_MAP_2_WIN
#define SZ_SECT_WND_UNITS                SZ_SECT_WND_UNITS_2_WIN

#define SZ_KEY_WEATHER_CUR_GOOD_TROPIC   "CUR_GOOD_TROPIC"
#define SZ_KEY_WEATHER_CUR_GOOD_MEDIUM   "CUR_GOOD_MEDIUM"
#define SZ_KEY_WEATHER_CUR_BAD_TROPIC    "CUR_BAD_TROPIC"
#define SZ_KEY_WEATHER_CUR_BAD_MEDIUM    "CUR_BAD_MEDIUM"
#define SZ_KEY_WEATHER_NEXT_GOOD_TROPIC  "NEXT_GOOD_TROPIC"
#define SZ_KEY_WEATHER_NEXT_GOOD_MEDIUM  "NEXT_GOOD_MEDIUM"
#define SZ_KEY_WEATHER_NEXT_BAD_TROPIC   "NEXT_BAD_TROPIC"
#define SZ_KEY_WEATHER_NEXT_BAD_MEDIUM   "NEXT_BAD_MEDIUM"


#define SZ_KEY_FOLDER_ORDERS             "ORDERS"
#define SZ_KEY_FOLDER_REP_LOAD           "REPORT_LOAD"
#define SZ_KEY_FOLDER_REP_JOIN           "REPORT_JOIN"

#define SZ_KEY_Y1                        "WIN_TOP"
#define SZ_KEY_Y2                        "WIN_BOTTOM"
#define SZ_KEY_X1                        "WIN_LEFT"
#define SZ_KEY_X2                        "WIN_RIGHT"
#define SZ_KEY_USE_SAVED_POS             "USE_SAVED_POS"
#define SZ_KEY_HEIGHT_0                  "HEIGHT_0"
#define SZ_KEY_HEIGHT_1                  "HEIGHT_1"
#define SZ_KEY_HEIGHT_2                  "HEIGHT_2"
#define SZ_KEY_HEIGHT_3                  "HEIGHT_3"
#define SZ_KEY_WIDTH_0                   "WIDTH_0"
#define SZ_KEY_WIDTH_1                   "WIDTH_1"
#define SZ_KEY_WIDTH_2                   "WIDTH_2"
#define SZ_KEY_WIDTH_3                   "WIDTH_3"
#define SZ_KEY_HEX_SIZE                  "HEX_SIZE"
#define SZ_KEY_HEX_SIZE_OLD              "HEX_SIZE_OLD"
#define SZ_KEY_ATLA_X0                   "ATLA_LEFT"
#define SZ_KEY_ATLA_Y0                   "ATLA_TOP"
#define SZ_KEY_HEX_SEL_X                 "HEX_SELECTED_X"
#define SZ_KEY_HEX_SEL_Y                 "HEX_SELECTED_Y"
#define SZ_KEY_STATE                     "SHOW_STATE"
#define SZ_KEY_PLANE_SEL                 "PLANE_SELECTED"
#define SZ_KEY_OPEN                      "IS_OPEN"
#define SZ_KEY_LOAD_ORDER                "LOAD_ORDERS"
#define SZ_KEY_LOAD_REP                  "LOAD_REPORT"
#define SZ_KEY_EOL                       "EOL"
#define SZ_KEY_PWD_OLD                   "PASSWORD"
#define SZ_KEY_HATCH_UNVISITED           "HATCH_UNVISITED"
#define SZ_KEY_SORT1                     "SORT_PRIMARY"
#define SZ_KEY_SORT2                     "SORT_SECONDARY"
#define SZ_KEY_SORT3                     "SORT_TERTIARY"
#define SZ_KEY_RCLICK_CENTERS            "RCLICK_CENTERS"
#define SZ_KEY_DEFAULT_EMPTY_ONLY        "DEFAULT_EMPTY_ONLY"
#define SZ_KEY_REP_CACHE_COUNT           "REPORT_CACHE_MAX"
#define SZ_KEY_MIN_SEL_MEN               "MEN_THRESHOLD"
#define SZ_KEY_BRIGHT_DELTA              "BRIGHTNESS_DELTA"
#define SZ_KEY_DECORATE_ORDERS           "DECORATE_ORDERS"
#define SZ_KEY_LAYOUT                    "LAYOUT"
#define SZ_KEY_CHECK_TEACH_LVL           "CHECK_TEACH_LEVEL"
//#define SZ_KEY_MAX_SKILL_DAYS            "MAX_SKILL_DAYS"
#define SZ_KEY_WATER_TERRAINS            "WATER_TERRAINS"
#define SZ_KEY_CHECK_OUTPUT_LIST         "CHECK_OUTPUT_LIST"
#define SZ_KEY_PWD_READ                  "READ_PWD_FROM_REP"
#define SZ_KEY_CHK_PROD_REQ              "IMMEDIATE_PROD_CHECK"
#define SZ_KEY_VALID_ORDERS              "VALID_ORDERS"
#define SZ_KEY_DASH_BAD_ROADS            "DASH_BAD_ROADS"
#define SZ_KEY_CHECK_MOVE_MODE           "CHECK_MOVE_MODE"
#define SZ_KEY_CHECK_NEW_UNIT_FACTION    "CHECK_FACTION_X_NEW_Y"

#define SZ_KEY_COLOR_TEST                "Test colour"
#define SZ_KEY_COLOR_RED                 "COLOR_RED"
#define SZ_KEY_COLOR_BLACK               "COLOR_BLACK"
#define SZ_KEY_MAP_GRID                  "GRID_NORMAL"
#define SZ_KEY_MAP_GRID_SEL              "GRID_SELECTED"
#define SZ_KEY_MAP_TROPIC_LINE           "TROPIC_LINE"
#define SZ_KEY_MAP_RING                  "DISTANCE_RING"
#define SZ_KEY_MAP_COASTLINE             "COASTLINE"

#define SZ_KEY_MAP_ROAD_OLD              "MAP_ROADS"
#define SZ_KEY_MAP_ROAD_BAD_OLD          "MAP_ROADS_BAD"

#define SZ_KEY_MAP_ROAD                  "MAP_ROAD"
#define SZ_KEY_MAP_ROAD_BAD              "MAP_ROAD_BAD"

#define SZ_KEY_MAP_TRUSTED               "TRUSTED_UNITS"
#define SZ_KEY_MAP_PREFERRED             "PREFERRED_UNITS"
#define SZ_KEY_MAP_TOLERATED             "TOLERATED_UNITS"
#define SZ_KEY_MAP_ENEMY                 "ENEMY_UNITS"
#define SZ_KEY_MAP_PREFIX                "MAP_"
#define SZ_KEY_MAP_UNKNOWN               "UNKNOWN"
#define SZ_KEY_MAP_DESERT                "DESERT"
#define SZ_KEY_MAP_FOREST                "FOREST"
#define SZ_KEY_MAP_JUNGLE                "JUNGLE"
#define SZ_KEY_MAP_MOUNTAIN              "MOUNTAIN"
#define SZ_KEY_MAP_OCEAN                 "OCEAN"
#define SZ_KEY_MAP_PLAINS                "PLAIN"
#define SZ_KEY_MAP_SWAMP                 "SWAMP"
#define SZ_KEY_MAP_TUNDRA                "TUNDRA"
#define SZ_KEY_MAP_NEXUS                 "NEXUS"
#define SZ_KEY_MAP_CAVERN                "CAVERN"
#define SZ_KEY_MAP_TUNNELS               "TUNNELS"
#define SZ_KEY_MAP_UNDERFOREST           "UNDERFOREST"

#define SZ_KEY_MAP_CHASM                 "chasm"
#define SZ_KEY_MAP_DEEPFOREST            "deepforest"
#define SZ_KEY_MAP_GROTTO                "grotto"
#define SZ_KEY_MAP_LAKE                  "lake"

#define SZ_KEY_MAP_FLAG_1                "FLAG_1"
#define SZ_KEY_MAP_FLAG_2                "FLAG_2"
#define SZ_KEY_MAP_FLAG_3                "FLAG_3"

#define SZ_KEY_SPY_DETECT_LO             "SPY_DETECT_LOW"
#define SZ_KEY_SPY_DETECT_HI             "SPY_DETECT_HIGH"
#define SZ_KEY_SPY_DETECT_AMT            "SPY_DETECT_AMOUNT"
#define SZ_KEY_SPY_DETECT_WARNING        "SPY_DETECT_WARNING"

#define SZ_KEY_HEX_SIZE_LIST             "HEX_SIZE_LIST"
#define SZ_KEY_SHOW_TRACKS               "SHOW_TRACKS"
#define SZ_KEY_WALL_WIDTH                "WALL_WIDTH"
#define SZ_KEY_FLAG_WIDTH                "FLAG_WIDTH"
#define SZ_KEY_ROAD_WIDTH                "ROAD_WIDTH"

#define SZ_KEY_UNIT_FLTR_PROPERTY        "UNIT_FLTR_PROPERTY_"
#define SZ_KEY_UNIT_FLTR_COMPARE         "UNIT_FLTR_COMPARE_"
#define SZ_KEY_UNIT_FLTR_VALUE           "UNIT_FLTR_VALUE_"
#define SZ_KEY_UNIT_FLTR_TRACKING        "UNIT_FLTR_TRACKING"
#define SZ_KEY_UNIT_FLTR_PYTHON_CODE     "PYTHON_CODE"
#define SZ_KEY_UNIT_FLTR_SOURCE          "SOURCE"
#define SZ_KEY_UNIT_FLTR_SOURCE_PYTHON   "Python"
#define SZ_KEY_UNIT_FLTR_SELECTED_HEXES  "SELECTED_HEXES"
#define SZ_KEY_UNIT_FLTR_SHOW_ON_MAP     "SHOW_ON_MAP"

#define SZ_KEY_HEX_FLTR_PROPERTY        "HEX_FLTR_PROPERTY_"
#define SZ_KEY_HEX_FLTR_COMPARE         "HEX_FLTR_COMPARE_"
#define SZ_KEY_HEX_FLTR_VALUE           "HEX_FLTR_VALUE_"
#define SZ_KEY_HEX_FLTR_PYTHON_CODE     "PYTHON_CODE"
#define SZ_KEY_HEX_FLTR_SOURCE          "SOURCE"
#define SZ_KEY_HEX_FLTR_SOURCE_PYTHON   "Python"



#define SZ_KEY_MOVEMENTS                 "MOVEMENT_MODES"
#define SZ_KEY_ORD_MONTH_LONG            "MONTH_LONG_ORDERS"
#define SZ_KEY_ORD_DUPLICATABLE          "MONTH_LONG_DUP_ORD"

#define SZ_KEY_SEPARATOR                 "SEPARATOR"
#define SZ_KEY_ORIENTATION               "ORIENTATION"
#define SZ_KEY_FORMAT                    "FORMAT"

#define SZ_KEY_LIS_COL_UNITS_HEX         "UNITS_HEX"
#define SZ_KEY_LIS_COL_UNITS_FILTER      "UNITS_FILTER"

#define SZ_KEY_FLTR_SET                  "FILTER_SET"
#define SZ_KEY_LEAD_SKILL_BONUS          "LEAD_SKILL_BONUS"
#define SZ_KEY_TAX_PER_TAXER             "TAX_PER_TAXER"

#define SZ_KEY_ICONS                     "ICONS"
#define SZ_KEY_BATTLE_STATISTICS         "BATTLE_STATISTICS"
#define SZ_KEY_SHOW_STEALS               "SHOW_STEALS_EVENT"
#define SZ_KEY_WAGONS                    "WAGONS"
#define SZ_KEY_WAGON_PULLERS             "WAGON_PULLERS"
#define SZ_KEY_WAGON_CAPACITY            "WAGON_CAPACITY"

#define SZ_VERTICAL                      "Vertical"
#define SZ_HORIZONTAL                    "Horizontal"

#define SZ_ATTR_STRUCT_HIDDEN            "HIDDEN"
#define SZ_ATTR_STRUCT_MOBILE            "MOBILE"
#define SZ_ATTR_STRUCT_SHAFT             "SHAFT"
#define SZ_ATTR_STRUCT_GATE              "GATE"
#define SZ_ATTR_STRUCT_ROAD_N            "ROAD_N"
#define SZ_ATTR_STRUCT_ROAD_NE           "ROAD_NE"
#define SZ_ATTR_STRUCT_ROAD_SE           "ROAD_SE"
#define SZ_ATTR_STRUCT_ROAD_S            "ROAD_S"
#define SZ_ATTR_STRUCT_ROAD_SW           "ROAD_SW"
#define SZ_ATTR_STRUCT_ROAD_NW           "ROAD_NW"
#define SZ_ATTR_STRUCT_MAX_LOAD          "MAX_LOAD"
#define SZ_ATTR_STRUCT_MIN_SAIL          "MIN_POWER"

#define SZ_LEADER                        "leader"
#define SZ_HERO                          "hero"

#define SZ_ATT_PLAYER_ID                 "PLAYER_FACTION_ID"
#define SZ_ATT_APPLY_ON_JOIN             "APPLY_ATTITUDES_ON_JOIN"
#define SZ_ATT_FRIEND1                   "TRUSTED_ATTITUDES"
#define SZ_ATT_FRIEND2                   "PREFERRED_ATTITUDES"
#define SZ_ATT_NEUTRAL                   "TOLERATED_ATTITUDES"
#define SZ_ATT_ENEMY                     "ENEMY_ATTITUDES"


#define SZ_DEFAULT_FONT_PROP             "9,77,90,90,1,arial"
#define SZ_DEFAULT_FONT_MONO             "10,77,90,90,1,courier new"
#define SZ_DEFAULT_MOVEMENT_MODE         "None,Walk,Ride,Fly,Swim"

#define SZ_ICONS_ADVANCED                "ADVANCED"
#define SZ_ICONS_SIMPLE                  "SIMPLE"

#define SZ_MANUAL_HEX_PROVINCE           "_Unknown_Temp_"            
#endif
