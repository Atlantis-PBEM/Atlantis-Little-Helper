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

#ifndef __CONSTS_H_INCL__
#define __CONSTS_H_INCL__

// ------------ error codes

#define ERR_OK                 0
#define ERR_FOPEN            101
#define ERR_FEMPTY           102
#define ERR_INV_LAND         103
#define ERR_LOGIC            104  // some internal logic problem
#define ERR_DUPLICATE_UNIT   105
#define ERR_FNAME            106
#define ERR_CANCEL           107 
#define ERR_NOTHING          108  // nothing was parsed
#define ERR_INV_TURN         109  // joining report for the wrong turn
#define ERR_INV_UNIT         110

// 

#define HDR_FACTION          "Atlantis Report For:"
#define HDR_FACTION_STATUS   "Faction Status:"
#define HDR_ERRORS           "Errors during turn:"
#define HDR_EVENTS           "Events during turn:"
#define HDR_EVENTS_2         "Important Messages:"
#define HDR_SILVER           "Unclaimed silver:"
#define HDR_SKILLS           "Skill reports:"
#define HDR_ITEMS            "Item reports:"
#define HDR_OBJECTS          "Object reports:"
#define HDR_BATTLES          "Battles during turn:"
#define HDR_ATTACKERS        "Attackers:"
#define HDR_DEFENDERS        "Defenders:"
#define HDR_CASUALTIES       "Total Casualties:"
#define HDR_SPOILS           "Spoils:" 
#define HDR_NOSPOILS         "No spoils." 
#define HDR_ATTITUDES        "Declared Attitudes ("
#define HDR_ORDER_TEMPLATE   "Orders Template ("
#define HDR_UNIT_OWN         "* " 
#define HDR_UNIT_ALIEN       "- " 
#define HDR_STRUCTURE        "+ "






#define STUDENTS_PER_TEACHER 10

#endif

