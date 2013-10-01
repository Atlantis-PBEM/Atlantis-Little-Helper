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

#include "extend.h"


#ifndef HAVE_PYTHON

#define SZ_NO_PYTHON "Python integration is disabled in this build!"

//-------------------------------------------------------------------------

eEErr  CPythonEmbedder::InitGeneric()
{
    ShowError(SZ_NO_PYTHON);
    return E_PY_DISABLED;
}

//-------------------------------------------------------------------------

void   CPythonEmbedder::DoneGeneric()
{
}

//-------------------------------------------------------------------------

void   CPythonEmbedder::CheckForPythonError()
{
}

//-------------------------------------------------------------------------

eEErr  CPythonEmbedder::InitUnitFilter(const char * userfilter, CStr & sPythonFilter)
{
    ShowError(SZ_NO_PYTHON);
    return E_PY_DISABLED;
}

//-------------------------------------------------------------------------

void   CPythonEmbedder::DoneUnitFilter()
{
}

//-------------------------------------------------------------------------

eEErr  CPythonEmbedder::RunUnitFilter(CUnit * pUnit, BOOL & success)
{
    return E_PY_DISABLED;
}

//-------------------------------------------------------------------------

void   CPythonEmbedder::GetCommonCode(CStr & code)
{
}

//-------------------------------------------------------------------------

#endif
