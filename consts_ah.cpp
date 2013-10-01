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

#include <stdlib.h>
#include "consts_ah.h"
#include "data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DefaultConfigRec DefaultConfig[] =
{
#if defined(_WIN32)
    {SZ_SECT_COMMON            ,     SZ_KEY_EOL            ,     SZ_EOL_MS           },
#else
    {SZ_SECT_COMMON            ,     SZ_KEY_EOL            ,     SZ_EOL_UNIX         },
#endif
    {SZ_SECT_COMMON            ,     SZ_KEY_HATCH_UNVISITED,     "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_DEFAULT_EMPTY_ONLY,  "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_REP_CACHE_COUNT,     "8"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_LOAD_ORDER     ,     "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_LOAD_REP       ,     "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_RCLICK_CENTERS ,     "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_HEX_SIZE_LIST  ,     "4,8,14,20,24,32,48"},
    {SZ_SECT_COMMON            ,     SZ_KEY_MIN_SEL_MEN    ,     "5"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_LAYOUT         ,     "2"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_SPY_DETECT_LO  ,     "100"               },
    {SZ_SECT_COMMON            ,     SZ_KEY_SPY_DETECT_HI  ,     "5000"              },
    {SZ_SECT_COMMON            ,     SZ_KEY_SPY_DETECT_AMT ,     "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_SPY_DETECT_WARNING,  "1"                },
    {SZ_SECT_COMMON            ,     SZ_KEY_BRIGHT_DELTA   ,     "8"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_DECORATE_ORDERS,     "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_MOVEMENTS      ,     SZ_DEFAULT_MOVEMENT_MODE},
    {SZ_SECT_COMMON            ,     SZ_KEY_ORD_MONTH_LONG ,     "BUILD,ENTERTAIN,PRODUCE,STUDY,TEACH,WORK,MOVE,ADVANCE,SAIL"},
    {SZ_SECT_COMMON            ,     SZ_KEY_ORD_DUPLICATABLE,    "TEACH,MOVE,ADVANCE,SAIL"},
    {SZ_SECT_COMMON            ,     SZ_KEY_CHECK_TEACH_LVL,     "1"                 },
//    {SZ_SECT_COMMON            ,     SZ_KEY_MAX_SKILL_DAYS ,     "450"               },
    {SZ_SECT_COMMON            ,     SZ_KEY_CHECK_OUTPUT_LIST,   "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_PWD_READ       ,     "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_VALID_ORDERS   ,     "address,advance,armor,assassinate,attack,autotax,avoid,behind,build,buy,cast,claim,combat,consume,declare,describe,destroy,end,endturn,enter,entertain,evict,exchange,faction,fightas,find,forget,form,give,giveif,guard,hold,leave,master,move,name,noaid,nocross,nospoils,option,password,pillage,prepare,produce,promote,quit,restart,reveal,sail,sell,show,spoils,steal,study,tax,teach,turn,weapon,withdraw,work"     },
    {SZ_SECT_COMMON            ,     SZ_KEY_LEAD_SKILL_BONUS,    "2,4"               },
    {SZ_SECT_COMMON            ,     SZ_KEY_CHECK_MOVE_MODE,     "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_TAX_PER_TAXER  ,     "50"                },
    {SZ_SECT_COMMON            ,     SZ_KEY_CHECK_NEW_UNIT_FACTION, "0"              },
    {SZ_SECT_COMMON            ,     SZ_KEY_WATER_TERRAINS ,     "Ocean,Lake"        },
    {SZ_SECT_COMMON            ,     SZ_KEY_ICONS          ,     SZ_ICONS_ADVANCED   },
    {SZ_SECT_COMMON            ,     SZ_KEY_BATTLE_STATISTICS,   "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_SHOW_STEALS    ,     "1"                 },
    {SZ_SECT_COMMON            ,     SZ_KEY_WAGONS         ,     "WAGO"              },
    {SZ_SECT_COMMON            ,     SZ_KEY_WAGON_PULLERS  ,     "HORS"              },
    {SZ_SECT_COMMON            ,     SZ_KEY_WAGON_CAPACITY ,     "250"               },



    {SZ_SECT_COLORS            ,     SZ_KEY_COLOR_RED      ,     "255,   0,   0"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_COLOR_BLACK    ,     "  0,   0,   0"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_UNKNOWN    ,     "192, 192, 192"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_GRID       ,     "128, 128, 128"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_GRID_SEL   ,     "255,   0,   0"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_TRUSTED    ,     " 80, 180, 205"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_PREFERRED  ,     "120, 212, 215"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_TOLERATED  ,     "255, 225,  80"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_ENEMY      ,     "255,  64,  64"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_TROPIC_LINE,     "255, 255, 255"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_RING       ,     "  0, 255, 255"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_COASTLINE  ,     " 50, 140, 255"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_ROAD       ,     "112, 128, 144"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_ROAD_BAD   ,     "159, 182, 205"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_DESERT     ,     "224, 164,  56"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_FOREST     ,     " 72, 200,  72"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_JUNGLE     ,     "  0, 128,   0"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_MOUNTAIN   ,     "188,  96,   0"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_OCEAN      ,     "  0,   0, 255"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_PLAINS     ,     "255, 232, 168"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_SWAMP      ,     "168, 168,  84"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_TUNDRA     ,     "184, 200, 224"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_NEXUS      ,     "  0, 147, 217"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_CAVERN     ,     "128, 128, 192"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_TUNNELS    ,     " 65, 146, 137"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_UNDERFOREST,     "116, 158,  44"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_CHASM      ,     "188, 117, 111"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_DEEPFOREST ,     "62 , 126, 47"      },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_GROTTO     ,     "119, 203, 107"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_LAKE       ,     "79 , 158, 255"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_FLAG_1     ,     "255,   0,   0"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_FLAG_2     ,     "128, 255,   0"     },
    {SZ_SECT_COLORS            ,     SZ_KEY_MAP_FLAG_3     ,     "0,     0, 128"     },

    {SZ_SECT_COLORS            ,     "MAP_Beach"       ,     "255, 255, 0"     },
    {SZ_SECT_COLORS            ,     "MAP_Harbour"     ,     "0, 255, 255"     },
    {SZ_SECT_COLORS            ,     "MAP_Ravine"      ,     "176, 0, 255"     },
    {SZ_SECT_COLORS            ,     "MAP_River"       ,     "0, 127, 255"     },
    {SZ_SECT_COLORS            ,     "MAP_ROAD"        ,     "112, 128, 144"   },
    {SZ_SECT_COLORS            ,     "MAP_Rocks"       ,     "255, 0, 0"       },



    {SZ_SECT_WEATHER           ,     SZ_KEY_WEATHER_CUR_GOOD_TROPIC ,     "The weather was clear last month"     },
    {SZ_SECT_WEATHER           ,     SZ_KEY_WEATHER_CUR_GOOD_MEDIUM ,     "The weather was clear last month"     },
    {SZ_SECT_WEATHER           ,     SZ_KEY_WEATHER_CUR_BAD_TROPIC  ,     "It was monsoon season last month"     },
    {SZ_SECT_WEATHER           ,     SZ_KEY_WEATHER_CUR_BAD_MEDIUM  ,     "It was winter last month"             },

    {SZ_SECT_WEATHER           ,     SZ_KEY_WEATHER_NEXT_GOOD_TROPIC,     "it will be clear next month"          },
    {SZ_SECT_WEATHER           ,     SZ_KEY_WEATHER_NEXT_GOOD_MEDIUM,     "it will be clear next month"          },
    {SZ_SECT_WEATHER           ,     SZ_KEY_WEATHER_NEXT_BAD_TROPIC ,     "it will be monsoon season next month" },
    {SZ_SECT_WEATHER           ,     SZ_KEY_WEATHER_NEXT_BAD_MEDIUM ,     "it will be winter next month"         },

#if defined(_WIN32)
    {SZ_SECT_MAP_PANE          ,     SZ_KEY_DASH_BAD_ROADS ,     "0"                 },
#else
    {SZ_SECT_MAP_PANE          ,     SZ_KEY_DASH_BAD_ROADS ,     "1"                 },
#endif
    {SZ_SECT_MAP_PANE          ,     SZ_KEY_STATE          ,     "2"                 },
    {SZ_SECT_MAP_PANE          ,     SZ_KEY_WALL_WIDTH     ,     "3"                 },
    {SZ_SECT_MAP_PANE          ,     SZ_KEY_FLAG_WIDTH     ,     "1"                 },
    {SZ_SECT_MAP_PANE          ,     SZ_KEY_ROAD_WIDTH     ,     "4"                 },

    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_X1             ,     "1"                 },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_Y1             ,     "1"                 },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_X2             ,     "800"               },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_Y2             ,     "600"               },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_HEIGHT_0       ,     "400"               },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_HEIGHT_1       ,     "130"               },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_HEIGHT_2       ,     "130"               },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_WIDTH_0        ,     "400"               },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_WIDTH_1        ,     "200"               },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_SORT1          ,     "faction"           },
    {SZ_SECT_WND_MAP_1_WIN     ,     SZ_KEY_SORT2          ,     "name"              },

    {SZ_SECT_WND_MAP_2_WIN     ,     SZ_KEY_X1             ,     "2"                 },
    {SZ_SECT_WND_MAP_2_WIN     ,     SZ_KEY_Y1             ,     "2"                 },
    {SZ_SECT_WND_MAP_2_WIN     ,     SZ_KEY_X2             ,     "400"               },
    {SZ_SECT_WND_MAP_2_WIN     ,     SZ_KEY_Y2             ,     "600"               },
    {SZ_SECT_WND_MAP_2_WIN     ,     SZ_KEY_HEIGHT_0       ,     "300"               },
    {SZ_SECT_WND_MAP_2_WIN     ,     SZ_KEY_HEIGHT_1       ,     "200"               },

    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_X1             ,     "402"               },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_Y1             ,     "2"                 },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_X2             ,     "800"               },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_Y2             ,     "300"               },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_HEIGHT_0       ,     "100"               },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_HEIGHT_1       ,     "100"               },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_HEIGHT_2       ,     "100"               },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_WIDTH_0        ,     "200"               },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_OPEN           ,     "1"                 },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_SORT1          ,     "faction"           },
    {SZ_SECT_WND_UNITS_2_WIN   ,     SZ_KEY_SORT2          ,     "name"              },


    {SZ_SECT_WND_MAP_3_WIN     ,     SZ_KEY_X1             ,     "2"                 },
    {SZ_SECT_WND_MAP_3_WIN     ,     SZ_KEY_Y1             ,     "2"                 },
    {SZ_SECT_WND_MAP_3_WIN     ,     SZ_KEY_X2             ,     "400"               },
    {SZ_SECT_WND_MAP_3_WIN     ,     SZ_KEY_Y2             ,     "400"               },

    {SZ_SECT_WND_EDITS_3_WIN   ,     SZ_KEY_X1             ,     "402"               },
    {SZ_SECT_WND_EDITS_3_WIN   ,     SZ_KEY_Y1             ,     "2"                 },
    {SZ_SECT_WND_EDITS_3_WIN   ,     SZ_KEY_X2             ,     "800"               },
    {SZ_SECT_WND_EDITS_3_WIN   ,     SZ_KEY_Y2             ,     "400"               },
    {SZ_SECT_WND_EDITS_3_WIN   ,     SZ_KEY_HEIGHT_0       ,     "100"               },
    {SZ_SECT_WND_EDITS_3_WIN   ,     SZ_KEY_HEIGHT_1       ,     "100"               },
    {SZ_SECT_WND_EDITS_3_WIN   ,     SZ_KEY_HEIGHT_2       ,     "100"               },
    {SZ_SECT_WND_EDITS_3_WIN   ,     SZ_KEY_WIDTH_0        ,     "200"               },
    {SZ_SECT_WND_EDITS_3_WIN   ,     SZ_KEY_OPEN           ,     "1"                 },

    {SZ_SECT_WND_UNITS_3_WIN   ,     SZ_KEY_X1             ,     "2"                 },
    {SZ_SECT_WND_UNITS_3_WIN   ,     SZ_KEY_Y1             ,     "402"               },
    {SZ_SECT_WND_UNITS_3_WIN   ,     SZ_KEY_X2             ,     "800"               },
    {SZ_SECT_WND_UNITS_3_WIN   ,     SZ_KEY_Y2             ,     "600"               },
    {SZ_SECT_WND_UNITS_3_WIN   ,     SZ_KEY_OPEN           ,     "1"                 },
    {SZ_SECT_WND_UNITS_3_WIN   ,     SZ_KEY_SORT1          ,     "faction"           },
    {SZ_SECT_WND_UNITS_3_WIN   ,     SZ_KEY_SORT2          ,     "name"              },

    {SZ_SECT_WND_UNITS_FLTR    ,     SZ_KEY_X1             ,     "200"               },
    {SZ_SECT_WND_UNITS_FLTR    ,     SZ_KEY_Y1             ,     "300"               },
    {SZ_SECT_WND_UNITS_FLTR    ,     SZ_KEY_X2             ,     "500"               },
    {SZ_SECT_WND_UNITS_FLTR    ,     SZ_KEY_Y2             ,     "500"               },
    {SZ_SECT_WND_UNITS_FLTR    ,     SZ_KEY_OPEN           ,     "0"                 },

    {SZ_SECT_WND_DESCR_LIST    ,     SZ_KEY_X1             ,     "80"                },
    {SZ_SECT_WND_DESCR_LIST    ,     SZ_KEY_Y1             ,     "150"               },
    {SZ_SECT_WND_DESCR_LIST    ,     SZ_KEY_X2             ,     "400"               },
    {SZ_SECT_WND_DESCR_LIST    ,     SZ_KEY_Y2             ,     "380"               },

    {SZ_SECT_WND_DESCR_ONE     ,     SZ_KEY_X1             ,     "80"                 },
    {SZ_SECT_WND_DESCR_ONE     ,     SZ_KEY_Y1             ,     "200"               },
    {SZ_SECT_WND_DESCR_ONE     ,     SZ_KEY_X2             ,     "450"               },
    {SZ_SECT_WND_DESCR_ONE     ,     SZ_KEY_Y2             ,     "380"               },


    {SZ_SECT_WND_MSG           ,     SZ_KEY_X1             ,     "200"               },
    {SZ_SECT_WND_MSG           ,     SZ_KEY_Y1             ,     "100"               },
    {SZ_SECT_WND_MSG           ,     SZ_KEY_X2             ,     "500"               },
    {SZ_SECT_WND_MSG           ,     SZ_KEY_Y2             ,     "500"               },

    {SZ_SECT_WND_LST_COLEDIT_DLG,     SZ_KEY_X1             ,     "250"               },
    {SZ_SECT_WND_LST_COLEDIT_DLG,     SZ_KEY_Y1             ,     "150"               },
    {SZ_SECT_WND_LST_COLEDIT_DLG,     SZ_KEY_X2             ,     "750"               },
    {SZ_SECT_WND_LST_COLEDIT_DLG,     SZ_KEY_Y2             ,     "600"               },


    {SZ_SECT_LIST_COL_CURRENT  , SZ_KEY_LIS_COL_UNITS_HEX  ,     SZ_SECT_LIST_COL_UNIT_DEF                },
    {SZ_SECT_LIST_COL_CURRENT  , SZ_KEY_LIS_COL_UNITS_FILTER,    SZ_SECT_LIST_COL_UNIT_FLTR_DEF           },

    {SZ_SECT_LIST_COL_UNIT_DEF ,     "000"                 ,     "30, 1, " PRP_FACTION_ID     ", Id F"       },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "001"                 ,     "80, 0, " PRP_FACTION        ", Fact"       },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "002"                 ,     "36, 1, " PRP_ID             ", Id"         },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "003"                 ,     "90, 0, " PRP_NAME           ", Unit"       },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "004"                 ,     "32, 1, " PRP_MEN            ", Men"        },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "005"                 ,     "32, 0, " PRP_FLAGS_STANDARD ", Flags"      },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "006"                 ,     "38, 1, " PRP_SILVER         ", Silv"       },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "007"                 ,     "31, 1, " PRP_SKILLS         ", Skill"      },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "008"                 ,     "37, 1, " PRP_STUFF          ", Items"      },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "009"                 ,     "35, 1, " PRP_HORS           ", Hors"       },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "010"                 ,     "38, 1, " PRP_WEAPONS        ", Weap"       },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "011"                 ,     "38, 1, " PRP_ARMOURS        ", Armor"      },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "012"                 ,     "65, 0, " PRP_STRUCT_NAME    ", Struct"     },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "013"                 ,     "37, 0, " PRP_STRUCT_OWNER   ", Own"        },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "014"                 ,     "42, 0, " PRP_TEACHING       ", Teach"      },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "015"                 ,     "77, 0, " PRP_ORDERS         ", Orders"     },
    {SZ_SECT_LIST_COL_UNIT_DEF ,     "016"                 ,     "99, 0, " PRP_COMMENTS       ", Comments"   },

    {SZ_SECT_LIST_COL_UNIT_FLTR_DEF  ,     "000"           ,     "30, 1, " PRP_FACTION_ID     ", Id F"       },
    {SZ_SECT_LIST_COL_UNIT_FLTR_DEF  ,     "001"           ,     "80, 0, " PRP_FACTION        ", Fact"       },
    {SZ_SECT_LIST_COL_UNIT_FLTR_DEF  ,     "002"           ,     "36, 1, " PRP_ID             ", Id"         },
    {SZ_SECT_LIST_COL_UNIT_FLTR_DEF  ,     "003"           ,     "90, 0, " PRP_NAME           ", Unit"       },
    {SZ_SECT_LIST_COL_UNIT_FLTR_DEF  ,     "004"           ,     "31, 1, " PRP_SKILLS         ", Skill"      },


    {SZ_SECT_STRUCTS           ,     STRUCT_GATE           ,     SZ_ATTR_STRUCT_HIDDEN "," SZ_ATTR_STRUCT_GATE },
    {SZ_SECT_STRUCTS           ,     "Longboat"            ,     SZ_ATTR_STRUCT_MOBILE ", MIN_POWER 5, MAX_LOAD 200"   },
    {SZ_SECT_STRUCTS           ,     "Clipper"             ,     SZ_ATTR_STRUCT_MOBILE ", MIN_POWER 10, MAX_LOAD 800"  },
    {SZ_SECT_STRUCTS           ,     "Galleon"             ,     SZ_ATTR_STRUCT_MOBILE ", MIN_POWER 15, MAX_LOAD 1800" },
    {SZ_SECT_STRUCTS           ,     "Balloon"             ,     SZ_ATTR_STRUCT_MOBILE ", MIN_POWER 10, MAX_LOAD 800"  },
    {SZ_SECT_STRUCTS           ,     "Armored Galleon"     ,     SZ_ATTR_STRUCT_MOBILE ", MIN_POWER 15, MAX_LOAD 1800" },

    {SZ_SECT_STRUCTS           ,     "Shaft"               ,     SZ_ATTR_STRUCT_SHAFT                     },
    {SZ_SECT_STRUCTS           ,     "Road N"              ,     SZ_ATTR_STRUCT_ROAD_N                    },
    {SZ_SECT_STRUCTS           ,     "Road NE"             ,     SZ_ATTR_STRUCT_ROAD_NE                   },
    {SZ_SECT_STRUCTS           ,     "Road SE"             ,     SZ_ATTR_STRUCT_ROAD_SE                   },
    {SZ_SECT_STRUCTS           ,     "Road S"              ,     SZ_ATTR_STRUCT_ROAD_S                    },
    {SZ_SECT_STRUCTS           ,     "Road SW"             ,     SZ_ATTR_STRUCT_ROAD_SW                   },
    {SZ_SECT_STRUCTS           ,     "Road NW"             ,     SZ_ATTR_STRUCT_ROAD_NW                   },

    // parallel
    {SZ_SECT_EDGE_STRUCTS      ,     "River"               ,     "Border,2"           },
    {SZ_SECT_EDGE_STRUCTS      ,     "Ravine"              ,     "Border,2"           },
    {SZ_SECT_EDGE_STRUCTS      ,     "Beach"               ,     "Ignore,2"           },
    {SZ_SECT_EDGE_STRUCTS      ,     "Rocks"               ,     "Rocks,2"            },
    // icon
    {SZ_SECT_EDGE_STRUCTS      ,     "Harbour"             ,     "Anchor,1"           },
    // perpendicular
    {SZ_SECT_EDGE_STRUCTS      ,     "Road"                ,     "Road,2"             },
    // bridge
    {SZ_SECT_EDGE_STRUCTS      ,     "Bridge"              ,     "Bridge,2"           },


    // Attitudes
    {SZ_SECT_ATTITUDES         ,     SZ_ATT_PLAYER_ID      ,     "-1"                 },
    {SZ_SECT_ATTITUDES         ,     SZ_ATT_APPLY_ON_JOIN  ,     "0"                  },
    {SZ_SECT_ATTITUDES         ,     SZ_ATT_FRIEND1        ,     "Ally,Friendly"      },
    {SZ_SECT_ATTITUDES         ,     SZ_ATT_FRIEND2        ,     "Own"      },
    {SZ_SECT_ATTITUDES         ,     SZ_ATT_NEUTRAL        ,     "Neutral"            },
    {SZ_SECT_ATTITUDES         ,     SZ_ATT_ENEMY          ,     "Unfriendly,Hostile" },

    // Arcadia III skills
    {SZ_SECT_STUDY_COST        ,     "BLIZ"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "FOG"                 ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "CONC"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "ILCR"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "ILWO"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "COUR"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "SUME"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "RESU"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "SUSP"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "INNE"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "TRAM"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "TRAF"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "MODI"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "REJU"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "SWAR"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "DIVE"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "GRYF"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "HYPN"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "BIND"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "LIGT"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "DARK"                ,     "100"                },
    {SZ_SECT_STUDY_COST        ,     "DRAL"                ,     "100"                },

    // Arcadia III items?
    {SZ_SECT_WEIGHT_MOVE       ,     "PPAR"                ,     "1,0,0,0,0"          },
    {SZ_SECT_WEIGHT_MOVE       ,     "ORCH"                ,     "1,0,0,0,0"          },
    {SZ_SECT_WEIGHT_MOVE       ,     "GRYF"                ,     "200,250,250,250,0"  },
    {SZ_SECT_WEIGHT_MOVE       ,     "CHEE"                ,     "5,0,0,0,0"          },

    {SZ_SECT_WEIGHT_MOVE       ,     PRP_STRUCT_ID         ,     "0,0,0,0,0"          },
    {SZ_SECT_WEIGHT_MOVE       ,     PRP_SEQUENCE          ,     "0,0,0,0,0"          },
    {SZ_SECT_WEIGHT_MOVE       ,     PRP_FRIEND_OR_FOE     ,     "0,0,0,0,0"          },



    // Arcadia magic skills
    /*
    WKEY => WKEY, FOG , FARS, SSTO, FSHI, FIRE, SWIN, STOR, BLIZ, CLEA, CALL
    PTTN => PTTN, CGAT, ESHI, MODI, CRPO, GATE, EART, TELE, MHEA, DIVE, RESU, SUSP
    SUMM => SUMM, DARK, SUDE, LIGT, BUND, SUBA, DEMO, NECR, SULI, BDEM, SUME, SSHI, RAIS
    MYST => MYST, ESWO, INVI, TRAM, CONC, EARM, HYPN, TRUE, BIND, DRAL
    BATT => BATT, GLAD, TOUG, FEAR, SWIF, FREN, INNE, SSIG, COUR
    CHAR => CHAR, WOLF, MERC, GRYF, TRAD, BIRD, UNIY, QUAT
    ARTL => ARTL, CRRU, CRCL, CRMA, ENGR, CRSL, CRSF, CRRI, CRTA
    */
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "AELF"                ,     "6,4,MYST,ESWO,INVI,TRAM,CONC,EARM,HYPN,TRUE,BIND,DRAL,BATT,GLAD,TOUG,FEAR,SWIF,FREN,INNE,SSIG,COUR,ARTL,CRRU,CRCL,CRMA,ENGR,CRSL,CRSF,CRRI,CRTA "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "BARB"                ,     "6,4,MYST,ESWO,INVI,TRAM,CONC,EARM,HYPN,TRUE,BIND,DRAL,WKEY,FOG ,FARS,SSTO,FSHI,FIRE,SWIN,STOR,BLIZ,CLEA,CALL,BATT,GLAD,TOUG,FEAR,SWIF,FREN,INNE,SSIG,COUR "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "DDWA"                ,     "6,4,PTTN,CGAT,ESHI,MODI,CRPO,GATE,EART,TELE,MHEA,DIVE,RESU,SUSP,BATT,GLAD,TOUG,FEAR,SWIF,FREN,INNE,SSIG,COUR,ARTL,CRRU,CRCL,CRMA,ENGR,CRSL,CRSF,CRRI,CRTA "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "ESKI"                ,     "6,4,WKEY,FOG ,FARS,SSTO,FSHI,FIRE,SWIN,STOR,BLIZ,CLEA,CALL,PTTN,CGAT,ESHI,MODI,CRPO,GATE,EART,TELE,MHEA,DIVE,RESU,SUSP,CHAR,WOLF,MERC,GRYF,TRAD,BIRD,UNIY,QUAT "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "HDWA"                ,     "6,4,BATT,GLAD,TOUG,FEAR,SWIF,FREN,INNE,SSIG,COUR,CHAR,WOLF,MERC,GRYF,TRAD,BIRD,UNIY,QUAT,SUMM,DARK,SUDE,LIGT,BUND,SUBA,DEMO,NECR,SULI,BDEM,SUME,SSHI,RAIS "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "HELF"                ,     "6,4,SUMM,DARK,SUDE,LIGT,BUND,SUBA,DEMO,NECR,SULI,BDEM,SUME,SSHI,RAIS,WKEY,FOG ,FARS,SSTO,FSHI,FIRE,SWIN,STOR,BLIZ,CLEA,CALL,CHAR,WOLF,MERC,GRYF,TRAD,BIRD,UNIY,QUAT "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "IDWA"                ,     "6,4,WKEY,FOG ,FARS,SSTO,FSHI,FIRE,SWIN,STOR,BLIZ,CLEA,CALL,PTTN,CGAT,ESHI,MODI,CRPO,GATE,EART,TELE,MHEA,DIVE,RESU,SUSP,SUMM,DARK,SUDE,LIGT,BUND,SUBA,DEMO,NECR,SULI,BDEM,SUME,SSHI,RAIS "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "MERM"                ,     "6,4,WKEY,FOG ,FARS,SSTO,FSHI,FIRE,SWIN,STOR,BLIZ,CLEA,CALL,CHAR,WOLF,MERC,GRYF,TRAD,BIRD,UNIY,QUAT,ARTL,CRRU,CRCL,CRMA,ENGR,CRSL,CRSF,CRRI,CRTA "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "NOMA"                ,     "6,4,CHAR,WOLF,MERC,GRYF,TRAD,BIRD,UNIY,QUAT,SUMM,DARK,SUDE,LIGT,BUND,SUBA,DEMO,NECR,SULI,BDEM,SUME,SSHI,RAIS,MYST,ESWO,INVI,TRAM,CONC,EARM,HYPN,TRUE,BIND,DRAL "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "ORC"                 ,     "8,0,BATT,GLAD,TOUG,FEAR,SWIF,FREN,INNE,SSIG,COUR"             },
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "PDWA"                ,     "6,4,MYST,ESWO,INVI,TRAM,CONC,EARM,HYPN,TRUE,BIND,DRAL,WKEY,FOG ,FARS,SSTO,FSHI,FIRE,SWIN,STOR,BLIZ,CLEA,CALL,BATT,GLAD,TOUG,FEAR,SWIF,FREN,INNE,SSIG,COUR "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "RAID"                ,     "6,4,SUMM,DARK,SUDE,LIGT,BUND,SUBA,DEMO,NECR,SULI,BDEM,SUME,SSHI,RAIS,ARTL,CRRU,CRCL,CRMA,ENGR,CRSL,CRSF,CRRI,CRTA,BATT,GLAD,TOUG,FEAR,SWIF,FREN,INNE,SSIG,COUR "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "SELF"                ,     "6,4,WKEY,FOG ,FARS,SSTO,FSHI,FIRE,SWIN,STOR,BLIZ,CLEA,CALL,PTTN,CGAT,ESHI,MODI,CRPO,GATE,EART,TELE,MHEA,DIVE,RESU,SUSP,ARTL,CRRU,CRCL,CRMA,ENGR,CRSL,CRSF,CRRI,CRTA "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "TELF"                ,     "6,4,MYST,ESWO,INVI,TRAM,CONC,EARM,HYPN,TRUE,BIND,DRAL,PTTN,CGAT,ESHI,MODI,CRPO,GATE,EART,TELE,MHEA,DIVE,RESU,SUSP,CHAR,WOLF,MERC,GRYF,TRAD,BIRD,UNIY,QUAT "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "TMAN"                ,     "6,4,MYST,ESWO,INVI,TRAM,CONC,EARM,HYPN,TRUE,BIND,DRAL,PTTN,CGAT,ESHI,MODI,CRPO,GATE,EART,TELE,MHEA,DIVE,RESU,SUSP,ARTL,CRRU,CRCL,CRMA,ENGR,CRSL,CRSF,CRRI,CRTA "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "UDWA"                ,     "6,4,MYST,ESWO,INVI,TRAM,CONC,EARM,HYPN,TRUE,BIND,DRAL,SUMM,DARK,SUDE,LIGT,BUND,SUBA,DEMO,NECR,SULI,BDEM,SUME,SSHI,RAIS,ARTL,CRRU,CRCL,CRMA,ENGR,CRSL,CRSF,CRRI,CRTA "},
    {SZ_SECT_MAX_MAG_SKILL_LVL ,     "WELF"                ,     "6,4,PTTN,CGAT,ESHI,MODI,CRPO,GATE,EART,TELE,MHEA,DIVE,RESU,SUSP,CHAR,WOLF,MERC,GRYF,TRAD,BIRD,UNIY,QUAT,BATT,GLAD,TOUG,FEAR,SWIF,FREN,INNE,SSIG,COUR "},

// generated from atlantis sources
#include "consts_ah.inc"

};


int DefaultConfigSize = sizeof(DefaultConfig)/sizeof(DefaultConfigRec);
