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

#ifndef __AH_EXTEND_INCL__
#define __AH_EXTEND_INCL__

#ifdef HAVE_CONFIG_H
   #include "config.h"
#endif


#ifdef HAVE_PYTHON
    #include "Python.h"
#else
    typedef struct __PyObject {} PyObject;
#endif


typedef enum
{
    E_OK           = 0,
    E_PY_DISABLED     ,
    E_NULL_POINTERS   ,
    E_ALREADY_INIT    ,
    E_PYTHON          ,  // could not initialize Python properly

} eEErr;




class CPythonEmbedder
{
public:
    CPythonEmbedder();
    CPythonEmbedder(CAtlaParser * pAtlantis);
    ~CPythonEmbedder();



    eEErr  InitUnitFilter(const char * userfilter, CStr & sPythonFilter);
    void   DoneUnitFilter();
    eEErr  RunUnitFilter(CUnit * pUnit, BOOL & success);

protected:
    eEErr  InitGeneric();
    void   DoneGeneric();
    void   ShowError(const char * msg, int msglen=0);
    void   CheckForPythonError();
    void   GetCommonCode(CStr & code);

    CAtlaParser * m_pAtlantis;
    BOOL          m_bInitUnitFilter;
    BOOL          m_bInitGeneric;


    PyObject * m_pCode  ;
    PyObject * m_pModule;
    PyObject * m_pDict  ;
    PyObject * m_pFunc  ;

};

#endif
