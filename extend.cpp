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


#define CHECK_NULL_PTR(ptr, err, msg) \
if (!ptr)                             \
{                                     \
    ShowError(msg);                   \
    result = err;                     \
    goto quit;                        \
}

#define SZ_UNIT_FILTER_MODULE              "unit_filter_module"
#define SZ_UNIT_FILTER_FUNC                "unit_filter_function"
#define SZ_ALH_PROGRAM_NAME                "alh_extension"
#define SZ_ALH_UNIT_FILTER_MODULE          "alh_unit_filter"
#define SZ_ALH_UNIT_FILTER_FN_GET_PROPERTY "get_property"

//-------------------------------------------------------------------------

CPythonEmbedder::CPythonEmbedder()
{
    m_pAtlantis = NULL;
    ShowError("Wrong CPythonEmbedder constructor called!");
}

//-------------------------------------------------------------------------

CPythonEmbedder::CPythonEmbedder(CAtlaParser * pAtlantis)
{
    m_pAtlantis       = pAtlantis;
    m_bInitUnitFilter = FALSE;
    m_bInitUnitFilter = FALSE;
    m_bInitGeneric    = FALSE;

    m_pCode   = NULL;
    m_pModule = NULL;
    m_pDict   = NULL;
    m_pFunc   = NULL;
}

//-------------------------------------------------------------------------

CPythonEmbedder::~CPythonEmbedder()
{
    if (m_bInitUnitFilter)
        DoneUnitFilter();
    if (m_bInitGeneric)
        DoneGeneric();
}

//-------------------------------------------------------------------------

void   CPythonEmbedder::ShowError(const char * msg, int msglen)
{
    if (msglen<=0)
        msglen = strlen(msg);

    gpApp->ShowError (msg, msglen, TRUE);
}


//=========================================================================
//
//  Now all the python-specific functions
//

#ifdef HAVE_PYTHON
//ALH_PYTHON_EXTEND

#include "Python.h"

//-------------------------------------------------------------------------


eEErr  CPythonEmbedder::InitGeneric()
{
    eEErr rc = E_OK;

    if (!m_pAtlantis)
        return E_NULL_POINTERS;

    if (m_bInitGeneric)
        return E_ALREADY_INIT;

    Py_SetProgramName((char*)SZ_ALH_PROGRAM_NAME);
    Py_Initialize();
    m_bInitGeneric = TRUE;

    return rc;
}

//-------------------------------------------------------------------------

void   CPythonEmbedder::DoneGeneric()
{
    Py_Finalize();
    m_bInitGeneric = FALSE;
}

//-------------------------------------------------------------------------

void   CPythonEmbedder::CheckForPythonError()
{
    if (PyErr_Occurred() )
        PyErr_Print();
}

//-------------------------------------------------------------------------

static CUnit * gpUnit = NULL;  // to be used by unitfltr_getproperty()

extern "C" PyObject * unitfltr_getproperty(PyObject *self, PyObject* args)
{
    char          * propname;
    EValueType      type;
    const void    * value;

    if (!PyArg_ParseTuple(args, "s", &propname) || !gpUnit)
        return Py_BuildValue("s", NULL);

    if (!gpUnit->GetProperty(propname, type, value, eNormal) )
    {
        // make default empty value
        CStrInt * pSI, SI(propname, 0);
        int       idx;

        if (gpApp->m_pAtlantis->m_UnitPropertyTypes.Search(&SI, idx))
        {
            pSI = (CStrInt*)gpApp->m_pAtlantis->m_UnitPropertyTypes.At(idx);
            type = (EValueType)pSI->m_value;
            if (eLong == type)
                value = 0;
            else
                value = "";
        }
        else
            return Py_BuildValue("s", NULL);
    }

    if (eLong==type)
        return Py_BuildValue("i", (long)value);
    else
    {
        // python is case sensitive, so lowercase all string values
        CStr S;
        S.SetStr((const char *)value);
        S.ToLower();
        return Py_BuildValue("s", S.GetData());
    }
}


PyMethodDef unitfltr_methods[] =
{
    {SZ_ALH_UNIT_FILTER_FN_GET_PROPERTY,  unitfltr_getproperty, METH_VARARGS,  "Get unit property."},
    {NULL, NULL}   // sentinel
};


void initunitfltr(void)
{
    PyImport_AddModule(SZ_ALH_UNIT_FILTER_MODULE);
    Py_InitModule     (SZ_ALH_UNIT_FILTER_MODULE, unitfltr_methods);
}




eEErr  CPythonEmbedder::InitUnitFilter(const char * userfilter, CStr & sPythonFilter)
{
    eEErr        result = E_OK;
    CStr         sToken;
    const char * p = userfilter;
    char         ch;
    int          idx;
    CStr         sCommand;

    sPythonFilter.Empty();
    result = InitGeneric();
    if (E_OK != result)
        return result;

    if (m_bInitUnitFilter)
        return E_ALREADY_INIT;
    m_bInitUnitFilter = TRUE;

    GetCommonCode(sCommand);

    sCommand << "\n"
             << "import " << SZ_ALH_UNIT_FILTER_MODULE << "\n"
             << "def " << SZ_UNIT_FILTER_FUNC << "():\n"
             << "    res = " ;
    while (p && *p)
    {
        p = sToken.GetToken(p, "+-*/<>=!()., \t\r\n", ch, TRIM_ALL, FALSE);

        // python is case sensitive, so lowercase all quoted strings
        if (!sToken.IsEmpty() && '\"' == sToken.GetData()[0] && '\"' == sToken.GetData()[sToken.GetLength()-1])
            sToken.ToLower();

        if (gpApp->m_pAtlantis->m_UnitPropertyNames.Search((void*)sToken.GetData(), idx))
            sCommand << SZ_ALH_UNIT_FILTER_MODULE << "." << SZ_ALH_UNIT_FILTER_FN_GET_PROPERTY << "(\"" << sToken << "\")";
        else
            sCommand << sToken;

        if ('\n' == ch)
            sCommand << ' ';
        else if (ch && '\r' != ch)
            sCommand << ch;
    }
    sCommand << "\n"
             << "    return res\n";
    sPythonFilter = sCommand;


    initunitfltr();

    m_pCode   = Py_CompileString((char*)sCommand.GetData(), SZ_UNIT_FILTER_MODULE,  Py_file_input);
                CHECK_NULL_PTR(m_pCode, E_PYTHON, "Py_CompileString()")
    m_pModule = PyImport_ExecCodeModule((char*)SZ_UNIT_FILTER_MODULE, m_pCode);
                CHECK_NULL_PTR(m_pModule, E_PYTHON, "PyImport_ExecCodeModule()")
    m_pDict   = PyModule_GetDict(m_pModule);
                CHECK_NULL_PTR(m_pDict, E_PYTHON, "PyModule_GetDict()")
    m_pFunc   = PyDict_GetItemString(m_pDict, SZ_UNIT_FILTER_FUNC);
                CHECK_NULL_PTR(m_pFunc, E_PYTHON, "PyDict_GetItemString()")


quit:

    if (E_OK != result)
        CheckForPythonError();

    return result;
}

//-------------------------------------------------------------------------

eEErr  CPythonEmbedder::RunUnitFilter(CUnit * pUnit, BOOL & success)
{
    eEErr result = E_PYTHON;

    success = FALSE;

    CHECK_NULL_PTR(m_pFunc, E_NULL_POINTERS, "m_pFunc not initialised")

    if (PyCallable_Check(m_pFunc))
    {
        PyObject * pValue;

        gpUnit = pUnit; // will be used by the extension function
        pValue = PyObject_CallObject(m_pFunc, NULL);
        if (pValue)
        {
            success = PyInt_AsLong(pValue);
            Py_DECREF(pValue);
            result = E_OK;
        }
        gpUnit = NULL;
    }

quit:

    if (E_OK != result)
        CheckForPythonError();

    return result;
}

//-------------------------------------------------------------------------

void   CPythonEmbedder::DoneUnitFilter()
{
    if (m_pCode)    Py_DECREF(m_pCode);
    if (m_pModule)  Py_DECREF(m_pModule);
    /// m_pDict is a borrowed reference
    /// m_pFunc: Borrowed reference
    m_pCode   = NULL;
    m_pModule = NULL;
    m_pDict   = NULL;
    m_pFunc   = NULL;
    m_bInitUnitFilter = FALSE;
}

//-------------------------------------------------------------------------

void   CPythonEmbedder::GetCommonCode(CStr & code)
{
    CFileReader  F;
    CFileWriter  W;
    CStr         S;
    int          x;

    code.Empty();
    if (F.Open(SZ_COMMON_PY_FILE))
    {
        while (F.GetNextLine(S))
            code << S;
        F.Close();
    }
    else
    {
        if (W.Open(SZ_COMMON_PY_FILE))
        {
            code << "import string";
            W.WriteBuf(code.GetData(), code.GetLength());
            W.Close();
        }
    }

    // 0D 0A sequence kills the dumb python parser on windows

    x = code.FindSubStr("\r");
    while (x>=0)
    {
        code.DelCh(x);
        x = code.FindSubStr("\r");
    }
}

//-------------------------------------------------------------------------

#endif
