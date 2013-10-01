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

#include "wx/listctrl.h"
#include "wx/spinctrl.h"
#include "wx/dialog.h"
#include "wx/textdlg.h"
#if wxCHECK_VERSION(2, 6, 0)
    #include "wx/numdlg.h"
#endif

#include "cstr.h"
#include "collection.h"
#include "cfgfile.h"
#include "files.h"
#include "atlaparser.h"
#include "consts.h"
#include "consts_ah.h"
#include "hash.h"

#include "ahapp.h"
#include "ahframe.h"
#include "data.h"
#include "mappane.h"
#include "editpane.h"
#include "listpane.h"
#include "unitpane.h"
#include "atlaparser.h"
#include "flagsdlg.h"
#include "hexfilterdlg.h"

#include <math.h>



const double     cos30               = sqrt(3.0)/2;
const double     tan30               = 1/sqrt(3.0);

static wxPoint   glb_PopupPoint;
CLand          * glb_pPopupLand = NULL;

#define          MAX_DP_BORDER      1

#define          COORD_PANE_WIDTH  24
#define          COORD_PANE_HEIGHT 24

enum { EDGE_SHAPE_BORDER, EDGE_SHAPE_ROAD, EDGE_SHAPE_BRIDGE, EDGE_SHAPE_ANCHOR, EDGE_SHAPE_ROCKS,
       EDGE_SHAPE_IGNORE };
       
// Detail levels govern what is shown at what scale
enum { MIN_DETAIL, UNIT_DETAIL, BATTLE_DETAIL, 
       TOWN_DETAIL, ROAD_DETAIL, NAME_DETAIL, 
       FLAG_DETAIL, WEATHER_DETAIL, EDGE_DETAIL, 
       MILITARY_DETAIL, SPECIAL_DETAIL, ECONOMIC_DETAIL, 
       //BUILDING_DETAIL, 
       MAX_DETAIL };
       
const int        SizeForDetail[] =
                                    { 0, // MIN_DETAIL
                                      8, // UNIT_DETAIL
                                      8, // BATTLE_DETAIL
                                      9, // TOWN_DETAIL
                                     10, // ROAD_DETAIL
                                     11, // NAME_DETAIL
                                     12, // FLAG_DETAIL
                                     14, // WEATHER_DETAIL
                                     16, // EDGE_DETAIL
                                     20, // MILITARY_DETAIL
                                     21, // SPECIAL_DETAIL
                                     22, // ECONOMIC_DETAIL
                                   //  28, // BUILDING_DETAIL
                                     56};// MAX_DETAIL

#define ICON_SIZE       8
#define ICON_SIZE_SMALL 6
                                     

BEGIN_EVENT_TABLE(CMapPane, wxWindow)
    EVT_PAINT            (                          CMapPane::OnPaint             )
    EVT_ERASE_BACKGROUND (                          CMapPane::OnEraseBackground   )
    EVT_LEFT_DOWN        (                          CMapPane::OnMouseEvent        )
    EVT_LEFT_UP          (                          CMapPane::OnMouseEvent        )
    EVT_MOTION           (                          CMapPane::OnMouseEvent        )
    EVT_LEAVE_WINDOW     (                          CMapPane::OnMouseEvent        )
    EVT_ENTER_WINDOW     (                          CMapPane::OnMouseEvent        )
    EVT_RIGHT_DOWN       (                          CMapPane::OnMouseEvent        )
    EVT_LEFT_DCLICK      (                          CMapPane::OnMouseEvent        )
                                                                                 
    EVT_MENU             (menu_Popup_Flag         , CMapPane::OnPopupMenuFlag     )
    EVT_MENU             (menu_Popup_Center       , CMapPane::OnPopupMenuCenter   )
    EVT_MENU             (menu_Popup_Battles      , CMapPane::OnPopupMenuBattles  )
    EVT_MENU             (menu_Popup_WhoMovesHere , CMapPane::OnPopupWhoMovesHere )
    EVT_MENU             (menu_Popup_Financial    , CMapPane::OnPopupFinancial    )
    EVT_MENU             (menu_Popup_New_Hex      , CMapPane::OnPopupNewHex       )
    EVT_MENU             (menu_Popup_Del_Hex      , CMapPane::OnPopupDeleteHex    )
    EVT_MENU             (menu_Popup_DistanceRing , CMapPane::OnPopupDistanceRing )


    


END_EVENT_TABLE()



//--------------------------------------------------------------------------

CMapPane::CMapPane(wxWindow * parent, wxWindowID id, int layout)
         :wxWindow(parent, id, wxDefaultPosition, wxDefaultSize,
                   (AH_LAYOUT_3_WIN==layout)?wxSUNKEN_BORDER:0 )
{
    int i;

    m_pPen             = new wxPen;
    m_pPenWall         = new wxPen;
    m_pPenSel          = new wxPen;
    m_pPenTropic       = new wxPen;
    m_pPenRing         = new wxPen;
    m_pPenRoad         = new wxPen;
    m_pPenRoadBad      = new wxPen;
    m_pPenBlack        = new wxPen(*wxBLACK, 1, wxSOLID);
    m_pPenRed2         = new wxPen(*wxRED  , 2, wxSOLID);
    m_pPenRed          = new wxPen(*wxRED  , 1, wxSOLID);
    wxColor * grey     = new wxColor(98,109,112);
    m_pPenGrey         = new wxPen(*grey , 1, wxSOLID);
    m_pPenCoastline    = new wxPen;

    for (i=0; i<LAND_FLAG_COUNT; i++)
        m_pPenFlag[i]  = new wxPen;

    m_pBrushBlack      = new wxBrush(*wxBLACK, wxSOLID);
    m_pBrushWhite      = new wxBrush(*wxWHITE, wxSOLID);
    m_pBrushRed        = new wxBrush(*wxRED  , wxSOLID);

    for(i=0; i<ATT_UNDECLARED; i++)
    {
        m_pUnitColor[i] = new wxColour;
        m_pDarkColor[i] = new wxColour;
    }

    m_HexSizeIdx       = -1;
    m_HexSizeIdxOld    = -1;
    m_ShowState        = 0;
    m_MinSelMen        = 0;
    m_pCities          = new CBaseCollById(32);
    m_pTrackHexes      = new CLongColl(8);
    m_pPopupLand       = NULL;
    m_UnknownColorIdx  = -1;
    m_Hatch            = FALSE;

    m_Rect_IsPaused    = false;
    m_Rect_IsDragging  = false;
    m_Rect_x1          = 0;
    m_Rect_y1          = 0;
    m_Rect_x2          = 0;
    m_Rect_y2          = 0;

    m_RingRadius       = 0;

    m_bAdvancedIcons   = FALSE;
}

//--------------------------------------------------------------------------

CMapPane::~CMapPane()
{
    int i;

    delete m_pPen;
    delete m_pPenWall;
    delete m_pPenSel;
    delete m_pPenTropic;
    delete m_pPenRing;
    delete m_pPenRoad;
    delete m_pPenRoadBad;
    delete m_pPenBlack;
    delete m_pPenRed2;
    delete m_pPenRed;
    delete m_pPenGrey;
    delete m_pPenCoastline;

    for(i=0; i<ATT_UNDECLARED; i++)
    {
        delete m_pUnitColor[i];
        delete m_pDarkColor[i];
    }

    for (i=0; i<LAND_FLAG_COUNT; i++)
        delete m_pPenFlag[i];

    delete m_pBrushBlack;
    delete m_pBrushWhite;
    delete m_pBrushRed  ;

    m_pCities->DeleteAll();
    delete m_pCities;
    delete m_pTrackHexes;

    m_TerrainBrushes.FreeAll();
    m_TerrainNames  .FreeAll();
    m_EdgeProps     .FreeAll();
}

//--------------------------------------------------------------------------


void CMapPane::Init(CAhFrame * pParentFrame)
{
    const char * p;
    long         x;
    CStr         S;

    m_pFrame        = pParentFrame;

    m_ShowState = atol(gpApp->GetConfig(SZ_SECT_MAP_PANE, SZ_KEY_STATE));
    m_MinSelMen = atol(gpApp->GetConfig(SZ_SECT_COMMON  , SZ_KEY_MIN_SEL_MEN));

    p = gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_HEX_SIZE_LIST);
    while (p && *p)
    {
        p = SkipSpaces(S.GetToken(p, ','));
        x = atol(S.GetData());
        x = (x/2)*2;  // make it even!
        if (x>0)
            m_HexSizes.Insert((void*)x);
    }
    if (0==m_HexSizes.Count())
        m_HexSizes.Insert((void*)24);

    m_SelPlane             = atol(gpApp->GetConfig(SZ_SECT_MAP_PANE  , SZ_KEY_PLANE_SEL       ));

    LoadPlaneConfig();
    ApplyIcons();
    ApplyColors();
    //UpdateEditPane();
    gpApp->OnMapSelectionChange();
}

//--------------------------------------------------------------------------

void CMapPane::Done()
{
    gpApp->SetConfig(SZ_SECT_MAP_PANE ,   SZ_KEY_STATE          , m_ShowState);
    gpApp->SetConfig(SZ_SECT_MAP_PANE ,   SZ_KEY_PLANE_SEL      , m_SelPlane );

    SavePlaneConfig();
}

//--------------------------------------------------------------------------

void CMapPane::SavePlaneConfig()
{
    CStr   sSection;

    sSection << "PLANE_" << m_SelPlane;

    gpApp->SetConfig(sSection.GetData(), SZ_KEY_HEX_SIZE       , m_HexSizeIdx               );
    gpApp->SetConfig(sSection.GetData(), SZ_KEY_HEX_SIZE_OLD   , m_HexSizeIdxOld            );
    gpApp->SetConfig(sSection.GetData(), SZ_KEY_ATLA_X0        , m_AtlaX0                   );
    gpApp->SetConfig(sSection.GetData(), SZ_KEY_ATLA_Y0        , m_AtlaY0                   );
    gpApp->SetConfig(sSection.GetData(), SZ_KEY_HEX_SEL_X      , m_SelHexX                  );
    gpApp->SetConfig(sSection.GetData(), SZ_KEY_HEX_SEL_Y      , m_SelHexY                  );

}

//-------------------------------------------------------------------------

void CMapPane::LoadPlaneConfig()
{
    CStr   sSection;
    long   x;

    sSection << "PLANE_" << m_SelPlane;

    x                      = atol(gpApp->GetConfig(sSection.GetData(), SZ_KEY_HEX_SIZE        ));
    SetHexSize(x);
    m_HexSizeIdxOld        = atol(gpApp->GetConfig(sSection.GetData(), SZ_KEY_HEX_SIZE_OLD    ));
    m_AtlaX0               = atol(gpApp->GetConfig(sSection.GetData(), SZ_KEY_ATLA_X0         ));
    m_AtlaY0               = atol(gpApp->GetConfig(sSection.GetData(), SZ_KEY_ATLA_Y0         ));
    m_SelHexX              = atol(gpApp->GetConfig(sSection.GetData(), SZ_KEY_HEX_SEL_X       ));
    m_SelHexY              = atol(gpApp->GetConfig(sSection.GetData(), SZ_KEY_HEX_SEL_Y       ));
}


//-------------------------------------------------------------------------

void CMapPane::ApplyFonts()
{
    Refresh(FALSE);
}

//-------------------------------------------------------------------------

//void CMapPane::ApplyOneEdgeColor(const char * name, const char * value)
//{
//    wxColour     cr;
//    const char * p;
//    CStr         S;
//    int          n;
//    CEdgeStructProperties * pEdgeProp = new CEdgeStructProperties;
//
//    // Name
//    pEdgeProp->name = name;
//
//    // Orientation
//    p = value;
//    p = SkipSpaces(S.GetToken(p, ','));
//    pEdgeProp->orientation = atol(S.GetData());
//
//    // Thickness
//    p = SkipSpaces(S.GetToken(p, ','));
//    n = atol(S.GetData());
//    if (n<=0)
//        n = 1;
//
//    // Color
//    S = SZ_KEY_MAP_PREFIX;
//    S << name;
//    StrToColor(&cr, gpApp->GetConfig(SZ_SECT_COLORS, S.GetData()));
//    pEdgeProp->pen = new wxPen(cr, n, wxSOLID);
//
//    if (!m_EdgeProps.Insert(pEdgeProp))
//        delete pEdgeProp;
//}

//-------------------------------------------------------------------------

void CMapPane::ApplyOneColor(wxColour & cr, const char * name)
{
    wxBrush    * pBrush = NULL;
    wxBitmap     bmp;
    CStr         S;
    CFileReader  F;
    wxColour     cr2;

    m_TerrainNames.Insert(strdup(name));

    if (0==stricmp(SZ_KEY_MAP_UNKNOWN, name))
        m_UnknownColorIdx = m_TerrainBrushes.Count();

#ifndef __WXMAC_OSX__
    S << name << ".bmp";
    if (F.Open(S.GetData()))
    {
        F.Close();
        if (bmp.LoadFile(S.GetData(), wxBITMAP_TYPE_BMP ))
        {
            pBrush = new wxBrush(bmp);
        }
    }
#endif

    if (!pBrush)
        pBrush = new wxBrush(cr, wxSOLID);
    m_TerrainBrushes.Insert(pBrush);

    cr2.Set(cr.Red() >> 1, cr.Green()  >> 1, cr.Blue() >> 1);
    pBrush = new wxBrush(cr2, wxCROSSDIAG_HATCH);
    m_TerrainBrushes.Insert(pBrush);

}

//-------------------------------------------------------------------------

void CMapPane::ApplyIcons()
{
    m_bAdvancedIcons = (0==stricmp(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_ICONS), SZ_ICONS_ADVANCED));
}

//-------------------------------------------------------------------------

void CMapPane::ApplyColors()
{
    static const char * FlagColorName[LAND_FLAG_COUNT] = {SZ_KEY_MAP_FLAG_1, SZ_KEY_MAP_FLAG_2, SZ_KEY_MAP_FLAG_3};
    static wxDash dash[2];

    wxColour     cr;
    long         x;
//    int          idx;
//    const char * name;
//    const char * value;
    int          i,n;
    int          flagwidth;
    CLand      * pLand;
    CPlane     * pPlane;


    delete m_pPen;
    StrToColor(&cr,   gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_GRID)    );
    m_pPen = new wxPen(cr, 1, wxSOLID);

    x = atol(gpApp->GetConfig(SZ_SECT_MAP_PANE, SZ_KEY_WALL_WIDTH));
    if (x<1) x=1;
    if (x>5) x=5;
    delete m_pPenWall;
    m_pPenWall = new wxPen(*wxBLACK, x, wxSOLID);

    delete m_pPenSel;
    StrToColor(&cr,   gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_GRID_SEL)    );
    m_pPenSel = new wxPen(cr, 2, wxSOLID);


    delete m_pPenTropic;
    StrToColor(&cr,   gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_TROPIC_LINE)    );
    m_pPenTropic = new wxPen(cr, 1, wxSOLID);

    delete m_pPenRing;
    StrToColor(&cr,   gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_RING)    );
    m_pPenRing = new wxPen(cr, 1, wxSOLID);

    x = atol(gpApp->GetConfig(SZ_SECT_MAP_PANE, SZ_KEY_ROAD_WIDTH));
    if (x<1) x=1;
    if (x>6) x=6;
    delete m_pPenRoad;
    StrToColor(&cr,   gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_ROAD)    );
    m_pPenRoad = new wxPen(cr, x, wxSOLID);

    delete m_pPenRoadBad;
    i = atol(gpApp->GetConfig(SZ_SECT_MAP_PANE, SZ_KEY_DASH_BAD_ROADS));
    if (i)
    {
        m_pPenRoadBad = new wxPen(cr, x, wxUSER_DASH); //wxSOLID);
        dash[0] = 1; dash[1] = 2;
        m_pPenRoadBad->SetDashes(2, dash);
    }
    else
    {
        StrToColor(&cr,   gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_ROAD_BAD)    );
        m_pPenRoadBad = new wxPen(cr, x, wxSOLID);
    }

    flagwidth = atoi(gpApp->GetConfig(SZ_SECT_MAP_PANE, SZ_KEY_FLAG_WIDTH));
    for (i=0; i<LAND_FLAG_COUNT; i++)
    {
        delete m_pPenFlag[i];
        StrToColor(&cr,   gpApp->GetConfig(SZ_SECT_COLORS, FlagColorName[i])    );
        m_pPenFlag[i] = new wxPen(cr, flagwidth, wxSOLID);
    }

    delete m_pPenCoastline;
    StrToColor(&cr,   gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_COASTLINE)    );
    m_pPenCoastline = new wxPen(cr, 6, wxSOLID);

    for (i=0; i<ATT_UNDECLARED; i++)
    {
        delete m_pUnitColor[i];
        delete m_pDarkColor[i];
        switch(i)
        {
            case ATT_FRIEND1:
                StrToColor(&cr, gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_TRUSTED));
                break;
            case ATT_FRIEND2:
                StrToColor(&cr, gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_PREFERRED));
                break;
            case ATT_NEUTRAL:
                StrToColor(&cr, gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_TOLERATED));
                break;
            case ATT_ENEMY:
                StrToColor(&cr, gpApp->GetConfig(SZ_SECT_COLORS, SZ_KEY_MAP_ENEMY));
                break;
            default: // shouldn't be used
                StrToColor(&cr, "255,255,255");
        }
        m_pUnitColor[i] = new wxColour(cr);
        m_pDarkColor[i] = new wxColor((char) (cr.Red()*.66),(char) (cr.Green()*.66),(char) (cr.Blue()*.66));
    }

    m_TerrainBrushes.FreeAll();
    m_TerrainNames  .FreeAll();
    m_UnknownColorIdx  = -1;

    for (n=0; n<gpApp->m_pAtlantis->m_Planes.Count(); n++)
    {
        pPlane = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(n);
        for (i=0; i<pPlane->Lands.Count(); i++)
        {
            pLand = (CLand*)pPlane->Lands.At(i);
            pLand->guiColor = -1;
        }
    }

    m_Hatch = atol(gpApp->GetConfig(SZ_SECT_COMMON,  SZ_KEY_HATCH_UNVISITED));

//    idx = gpApp->GetSectionFirst(SZ_SECT_COLORS, name, value);
//    while (idx>=0)
//    {
//        StrToColor(&cr, value);
//        ApplyOneColor(cr, name);
//
//        idx = gpApp->GetSectionNext (idx, SZ_SECT_COLORS, name, value);
//    }

    // load edge structures colors
    m_EdgeProps.FreeAll();
//    idx = gpApp->GetSectionFirst(SZ_SECT_EDGE_STRUCTS, name, value);
//    while (idx>=0)
//    {
//        // There is a bug here - function calls GetConfig, which creates a new config entry,T
//        // making idx point to a wrong location.
//        // Though it seems not dangerous under current conditions.... maybe fix later
//        ApplyOneEdgeColor(name, value);
//
//        idx = gpApp->GetSectionNext (idx, SZ_SECT_EDGE_STRUCTS, name, value);
//    }


    Refresh(FALSE);
}

//--------------------------------------------------------------------------

wxBrush * CMapPane::GetLandBrush(CLand * pLand, BOOL GetHatched)
{
    int          idx, i;
    const char * name;
    wxColour     cr;
//    int          Hatch;

    idx = pLand?pLand->guiColor:m_UnknownColorIdx;
    if (idx<0 || idx>=m_TerrainBrushes.Count())
    {
        idx = -1;
        name = pLand?pLand->TerrainType.GetData():SZ_KEY_MAP_UNKNOWN;

        for (i=0; i<m_TerrainNames.Count(); i++)
            if (0==stricmp(name, (const char*)m_TerrainNames.At(i)))
            {
                idx = i*2;
                break;
            }

        if (idx<0)
        {
            // totally new color, but try to read it, so it gets into config file
            StrToColor(&cr,   gpApp->GetConfig(SZ_SECT_COLORS, name) );
            //Hatch = atol(gpApp->GetConfig(SZ_SECT_COMMON,  SZ_KEY_HATCH_UNVISITED));
            idx   = m_TerrainBrushes.Count();
            ApplyOneColor(cr, name);
        }

        if (pLand)
            pLand->guiColor   = idx;
        else
            m_UnknownColorIdx = idx; // should be set by this moment, but just in case.

    }
    if (GetHatched)
        idx++;

    return (wxBrush*)m_TerrainBrushes.At(idx);
}

//--------------------------------------------------------------------------

CEdgeStructProperties * CMapPane::GetEdgeProps(const char * name)
{
    wxColour     cr;
    const char * p;
    CStr         S;
    int          n;
    CEdgeStructProperties * pEdgeProp; //
    CEdgeStructProperties   Dummy;
    int                  idx;

    Dummy.name = name;
    if (m_EdgeProps.Search(&Dummy, idx))
        pEdgeProp = (CEdgeStructProperties*)m_EdgeProps.At(idx);
    else
    {
        pEdgeProp = new CEdgeStructProperties;

        // Name
        pEdgeProp->name = name;

        // Shape
        p = gpApp->GetConfig(SZ_SECT_EDGE_STRUCTS, name);
        if (!p || !*p)
        {
            p = "border,1";
            gpApp->SetConfig(SZ_SECT_EDGE_STRUCTS, name, p);
        }
        p = SkipSpaces(S.GetToken(p, ','));
        if (0==stricmp(S.GetData(), "border"))
            pEdgeProp->shape = EDGE_SHAPE_BORDER;
        else if (0==stricmp(S.GetData(), "road"))
            pEdgeProp->shape = EDGE_SHAPE_ROAD;
        else if (0==stricmp(S.GetData(), "bridge"))
            pEdgeProp->shape = EDGE_SHAPE_BRIDGE;
        else if (0==stricmp(S.GetData(), "anchor"))
            pEdgeProp->shape = EDGE_SHAPE_ANCHOR;
        else if (0==stricmp(S.GetData(), "rocks"))
            pEdgeProp->shape = EDGE_SHAPE_ROCKS;
        else
            pEdgeProp->shape = EDGE_SHAPE_IGNORE;

        // Thickness
        p = SkipSpaces(S.GetToken(p, ','));
        n = atol(S.GetData());
        if (n<=0)
            n = 1;

        // Color
        S = SZ_KEY_MAP_PREFIX;
        S << name;
        StrToColor(&cr, gpApp->GetConfig(SZ_SECT_COLORS, S.GetData()));
        pEdgeProp->pen = new wxPen(cr, n, wxSOLID);

        m_EdgeProps.Insert(pEdgeProp);
    }

    return pEdgeProp;
}

//--------------------------------------------------------------------------

inline BOOL CMapPane::ValidHexNo(int NoX, int NoY)
{
    // sum of coordinates must be even

    int x = abs(NoX) + abs(NoY);

    return x == ((x>>1)<<1);
}


//--------------------------------------------------------------------------

int  CMapPane::NormalizeHexX(int NoX, CPlane * pPlane)
{
    if (pPlane && pPlane->Width>0)
    {
        while (NoX < pPlane->WestEdge)
            NoX += pPlane->Width;
        while (NoX > pPlane->EastEdge)
            NoX -= pPlane->Width;
    }
    return NoX;
}

//--------------------------------------------------------------------------

void CMapPane::GetHexNo(int & NoX, int & NoY, int WinX, int WinY)
{
    // center of (0,0) hex has 0,0 Atla coordinates

    long AtlaX, AtlaY;
    int  ApprX, ApprY; //approximite hex no
    int  x, y;
    int  x0, y0, x1, y1, x2, y2;

    WinToAtla(WinX, WinY, AtlaX, AtlaY);

    ApprX = AtlaX / (m_HexSize * 3 / 2);       // m_HexSize must be even!
    ApprY = AtlaY / m_HexHalfHeight;

    for (x = ApprX-1; x <= ApprX+1; x++)
        for (y = ApprY-2; y <= ApprY+2; y++)
            if ( ValidHexNo(x,y) &&
                (AtlaY >= y*m_HexHalfHeight - m_HexHalfHeight) &&
                (AtlaY <= y*m_HexHalfHeight + m_HexHalfHeight) )
            {
                // hex center
                x0 = x * m_HexSize * 3 / 2;
                y0 = y * m_HexHalfHeight;

                x1 = x0 - m_HexSize;
                y1 = y0;
                x2 = x0 - m_HexHalfSize;
                y2 = y0 - m_HexHalfHeight;
                if (AtlaY < y1 + (double)(y2-y1)/(x2-x1)*(AtlaX-x1))
                    continue;

                x1 = x0 + m_HexSize;
                y1 = y0;
                x2 = x0 + m_HexHalfSize;
                y2 = y0 - m_HexHalfHeight;
                if (AtlaY < y1 + (double)(y2-y1)/(x2-x1)*(AtlaX-x1))
                    continue;


                x1 = x0 - m_HexSize;
                y1 = y0;
                x2 = x0 - m_HexHalfSize;
                y2 = y0 + m_HexHalfHeight;
                if (AtlaY > y1 + (double)(y2-y1)/(x2-x1)*(AtlaX-x1))
                    continue;

                x1 = x0 + m_HexSize;
                y1 = y0;
                x2 = x0 + m_HexHalfSize;
                y2 = y0 + m_HexHalfHeight;
                if (AtlaY > y1 + (double)(y2-y1)/(x2-x1)*(AtlaX-x1))
                    continue;

                NoX = x;
                NoY = y;
                return;
            }
}

//--------------------------------------------------------------------------

void CMapPane::GetHexCenter(int NoX, int NoY, int & WinX, int & WinY)
{
    // center of (0,0) hex has 0,0 Atla coordinates

    long AtlaX, AtlaY;

    AtlaX = NoX * m_HexSize * 3 / 2;
    AtlaY = NoY * m_HexHalfHeight;

    AtlaToWin(WinX, WinY, AtlaX, AtlaY);
}

//--------------------------------------------------------------------------

inline void CMapPane::WinToAtla(int   WinX, int   WinY, long & AtlaX, long & AtlaY)
{
    AtlaX = WinX + m_AtlaX0;
    AtlaY = WinY + m_AtlaY0;
}

//--------------------------------------------------------------------------

inline void CMapPane::AtlaToWin(int & WinX, int & WinY, long   AtlaX, long   AtlaY)
{
    WinX = AtlaX - m_AtlaX0;
    WinY = AtlaY - m_AtlaY0;
}

//--------------------------------------------------------------------------

BOOL CMapPane::SetHexSize(int HexSizeIdx)
{
  //  wxRect rect;
    long CX, CY;
    long CXOld, CYOld;
    BOOL Changed;
    int  width, height;
    int  OldIdx   = m_HexSizeIdx;
    int  OldSize       = m_HexSize; //(long)m_HexSizes.At(m_HexSizeIdx);
    int  OldHalfHeight = m_HexHalfHeight;

    m_HexSizeIdx    = HexSizeIdx;
    if (m_HexSizeIdx < 0)
        m_HexSizeIdx = 0;
    if (m_HexSizeIdx >= m_HexSizes.Count())
        m_HexSizeIdx = m_HexSizes.Count()-1;

    Changed = (OldIdx != m_HexSizeIdx);
    if (Changed)
    {
        m_HexSizeIdxOld = OldIdx;
        if (m_HexSizeIdxOld < 0)
            m_HexSizeIdxOld = m_HexSizeIdx;

        m_HexSize       = (long)m_HexSizes.At(m_HexSizeIdx);
        m_HexHalfSize   = m_HexSize / 2;
        m_HexHalfHeight = (int)(m_HexSize * cos30 + 0.5);
        m_HexHeight     = m_HexHalfHeight * 2;

        if (OldIdx>=0)
        {
            GetClientSize(&width, &height);

            WinToAtla(width/2, height/2, CXOld, CYOld);
            CX = (long)( (double)CXOld * (double)m_HexSize       / (double)OldSize       + 0.5);
            CY = (long)( (double)CYOld * (double)m_HexHalfHeight / (double)OldHalfHeight + 0.5);

            m_AtlaX0 = CX - width/2;
            m_AtlaY0 = CY - height/2;

            //m_AtlaX0 += CXOld*((double)m_HexSize/(double)OldSize - 1);
            //m_AtlaY0 += CYOld*((double)m_HexSize/(double)OldSize - 1);;
        }
        
        for (int sidx=0; sidx<MAX_DETAIL; sidx++)
        {
            if(m_HexSize >= SizeForDetail[sidx]) m_Detail = sidx;
        }
    }

    return Changed;
}

//--------------------------------------------------------------------------

void CMapPane::DrawHexTest(int NoX, int NoY, wxDC * pDC, CLand * pLand)
{
    /*
    if (!ValidHexNo(NoX, NoY))
        return;
    int           x0, y0;  // hex center

    GetHexCenter(NoX, NoY, x0, y0);
    pDC->Ellipse(x0-6, y0-6, x0+6, y0+6);
    */
}

//--------------------------------------------------------------------------

void CMapPane::DrawEdge(wxDC * pDC, CPlane * pPlane)
{

}

void CMapPane::DrawCoastalLine(wxDC * pDC, wxPoint * point, int direction)
{
    int x1=0, y1=0, x2=0, y2=0;
    int scale = m_HexHalfHeight / 4;
    if(scale%2) scale +=1;
    if(scale < 6) scale = 6;
    if(m_Detail<EDGE_DETAIL) scale -= 2;
    if(m_Detail<UNIT_DETAIL) scale = 2;
    switch (direction)
    {
        case North:
            x1 = point[1].x+1;
            y1 = point[1].y+(scale/2);
            x2 = point[2].x-1;
            y2 = point[2].y+(scale/2);
            break;
        case Northeast:
            x1 = point[2].x-(scale/2)+1;
            y1 = point[2].y+3;
            x2 = point[3].x-(scale/2);
            y2 = point[3].y;
            break;
        case Southeast:
            x1 = point[3].x-(scale/2);
            y1 = point[3].y;
            x2 = point[4].x-(scale/2)+1;
            y2 = point[4].y-2;
            break;
        case South:
            x1 = point[4].x-1;
            y1 = point[4].y-(scale/2);
            x2 = point[5].x+1;
            y2 = point[5].y-(scale/2);
            break;
        case Southwest:
            x1 = point[5].x+(scale/2)-1;
            y1 = point[5].y-2;
            x2 = point[0].x+(scale/2);
            y2 = point[0].y;
            break;
        case Northwest:
            x1 = point[0].x+(scale/2);
            y1 = point[0].y;
            x2 = point[1].x+(scale/2)-1;
            y2 = point[1].y+3;
            break;
     }
     m_pPenCoastline->SetWidth(scale);
     pDC->SetPen(*m_pPenCoastline);
     pDC->DrawLine(x1, y1, x2, y2);
}


void CMapPane::DrawEdgeStructures(wxDC * pDC, CLand * pLand, wxPoint * point, int x0, int y0)
{
    int i;
    int scale = m_HexHalfHeight / 4;
    if(scale%2) scale +=1;
    if(scale < 6) scale = 6;
    if (m_Detail >= UNIT_DETAIL && pLand)
    {
        for (i=0; i<pLand->EdgeStructs.Count(); i++)
        {
            int x1=0, y1=0, x2=0, y2=0;
            int xa1, ya1, xb1, yb1;
            int xa2, ya2, xb2, yb2;

            CStruct               * pEdge = (CStruct*)pLand->EdgeStructs.At(i);
            CEdgeStructProperties * pProps = GetEdgeProps(pEdge->Kind.GetData());
            
            wxPen * pPen = pProps->pen;
            int offset = 1;
            if (m_Detail < EDGE_DETAIL)
            {
                if ((pProps->shape != EDGE_SHAPE_BORDER)
                    && (pProps->shape != EDGE_SHAPE_ROAD)) continue;
                if ((m_Detail<ROAD_DETAIL) && (pProps->shape == EDGE_SHAPE_ROAD))
                    continue;
                pPen = new wxPen(pPen->GetColour(), 1, pPen->GetStyle());
                offset = 0;
            }

            pDC->SetPen(*pPen);

            switch (pProps->shape)
            {
            case EDGE_SHAPE_BORDER:
                // parallel
                if(pLand->Flags&LAND_IS_WATER) break;
                switch (pEdge->Location)
                {
                case North:
                    x1 = point[1].x + offset;    y1 = point[1].y + offset;
                    x2 = point[2].x - offset;    y2 = point[2].y + offset;
                    break;
                case Northeast:
                    x1 = point[2].x - offset;    y1 = point[2].y + offset + 1;
                    x2 = point[3].x - offset;    y2 = point[3].y;
                    break;
                case Southeast:
                    x1 = point[3].x - offset;    y1 = point[3].y;
                    x2 = point[4].x - offset;    y2 = point[4].y - offset - 1;
                    break;
                case South:
                    x1 = point[4].x - offset;    y1 = point[4].y - offset;
                    x2 = point[5].x + offset;    y2 = point[5].y - offset;
                    break;
                case Southwest:
                    x1 = point[5].x + offset;    y1 = point[5].y - offset - 1;
                    x2 = point[0].x + offset;    y2 = point[0].y;
                    break;
                case Northwest:
                    x1 = point[0].x + offset;    y1 = point[0].y;
                    x2 = point[1].x + offset;    y2 = point[1].y + offset + 1;
                    break;
                }
                pDC->DrawLine(x1, y1, x2, y2);
                break;

            case EDGE_SHAPE_ROAD:
                // perpendiculars (roads)
                switch (pEdge->Location)
                {
                case North:
                    x1 = x0;
                    y1 = y0-m_HexHalfHeight;
                    break;
                case Northeast:
                    x1 = x0+m_HexHalfSize*3/2;
                    y1 = y0-m_HexHalfHeight/2;
                    break;
                case Southeast:
                    x1 = x0+m_HexHalfSize*3/2;
                    y1 = y0+m_HexHalfHeight/2;
                    break;
                case South:
                    x1 = x0;
                    y1 = y0+m_HexHalfHeight;
                    break;
                case Southwest:
                    x1 = x0-m_HexHalfSize*3/2;
                    y1 = y0+m_HexHalfHeight/2;
                    break;
                case Northwest:
                    x1 = x0-m_HexHalfSize*3/2;
                    y1 = y0-m_HexHalfHeight/2;
                    break;
                }
                pDC->DrawLine(x0, y0, x1, y1);
                break;

            case EDGE_SHAPE_BRIDGE:
                // Bridge
                switch (pEdge->Location)
                {
                case North:
                    xa1 = (point[1].x + point[2].x)/2 - 2;   xb1 = xa1+4;
                    ya1 = yb1 = point[1].y;
                    xa2 = xa1;   xb2 = xb1;
                    ya2 = yb2 = point[1].y + m_HexHalfHeight/2;
                    break;
                case Northeast:
                    x1 = (point[2].x + point[3].x)/2;
                    y1 = (point[2].y + point[3].y)/2;
                    xa1 = x1-1; xb1 = x1+1;
                    ya1 = y1-2; yb1 = y1+2;
                    x1 = (x1+x0)/2;
                    y1 = (y1+y0)/2;
                    xa2 = x1-1; xb2 = x1+1;
                    ya2 = y1-2; yb2 = y1+2;
                    break;
                case Southeast:
                    x1 = (point[3].x + point[4].x)/2;
                    y1 = (point[3].y + point[4].y)/2;
                    xa1 = x1+1; xb1 = x1-1;
                    ya1 = y1-2; yb1 = y1+2;
                    x1 = (x1+x0)/2;
                    y1 = (y1+y0)/2;
                    xa2 = x1+1; xb2 = x1-1;
                    ya2 = y1-2; yb2 = y1+2;
                    break;
                case South:
                    xa1 = (point[4].x + point[5].x)/2 + 2;   xb1 = xa1-4;
                    ya1 = yb1 = point[4].y;
                    xa2 = xa1;   xb2 = xb1;
                    ya2 = yb2 = point[4].y - m_HexHalfHeight/2;
                    break;
                case Southwest:
                    x1 = (point[5].x + point[0].x)/2;
                    y1 = (point[5].y + point[0].y)/2;
                    xa1 = x1-1; xb1 = x1+1;
                    ya1 = y1-2; yb1 = y1+2;
                    x1 = (x1+x0)/2;
                    y1 = (y1+y0)/2;
                    xa2 = x1-1; xb2 = x1+1;
                    ya2 = y1-2; yb2 = y1+2;
                    break;
                case Northwest:
                    x1 = (point[0].x + point[1].x)/2;
                    y1 = (point[0].y + point[1].y)/2;
                    xa1 = x1+1; xb1 = x1-1;
                    ya1 = y1-2; yb1 = y1+2;
                    x1 = (x1+x0)/2;
                    y1 = (y1+y0)/2;
                    xa2 = x1+1; xb2 = x1-1;
                    ya2 = y1-2; yb2 = y1+2;
                    break;
                }
                pDC->DrawLine(xa1, ya1, xa2, ya2);
                pDC->DrawLine(xb1, yb1, xb2, yb2);
                break;
            case EDGE_SHAPE_ANCHOR:
                // Anchor - only in water-type regions
                if(0==(pLand->Flags&LAND_IS_WATER)) break;
                switch (pEdge->Location)
                {
                case North:
                    x1 = x0;
                    y1 = y0-m_HexHalfHeight+5;
                    break;
                case Northeast:
                    x1 = x0+m_HexHalfSize*3/2-5;
                    y1 = y0-m_HexHalfHeight/2+2;
                    break;
                case Southeast:
                    x1 = x0+m_HexHalfSize*3/2-5;
                    y1 = y0+m_HexHalfHeight/2-2;
                    break;
                case South:
                    x1 = x0;
                    y1 = y0+m_HexHalfHeight-5;
                    break;
                case Southwest:
                    x1 = x0-m_HexHalfSize*3/2+5;
                    y1 = y0+m_HexHalfHeight/2-2;
                    break;
                case Northwest:
                    x1 = x0-m_HexHalfSize*3/2+5;
                    y1 = y0-m_HexHalfHeight/2+2;
                    break;
                }
                // draw anchor at x1,y1
                pDC->SetPen(*pProps->pen);
                pDC->DrawLine(x1-2, y1+2, x1+4, y1-4);
                pDC->DrawLine(x1-1, y1+2, x1+5, y1-4);
                pDC->DrawLine(x1, y1-4, x1+1, y1-3);
                pDC->DrawLine(x1+1, y1-4, x1+1, y1-3);
                pDC->DrawLine(x1+4, y1-1, x1+5, y1);
                pDC->DrawLine(x1+5, y1-1, x1+6, y1);
                pDC->DrawLine(x1-3, y1-2, x1-3, y1+2);
                pDC->DrawLine(x1-4, y1-1, x1-4, y1+1);
                pDC->DrawLine(x1-1, y1+3, x1+3, y1+3);
                pDC->DrawLine(x1, y1+4, x1+2, y1+4);
                pDC->SetPen(*m_pPenBlack);
                pDC->DrawLine(x1-2, y1+1, x1+3, y1-4);
                pDC->DrawLine(x1, y1+2, x1+4, y1-2);
                pDC->DrawLine(x1-1, y1-4, x1, y1-3);
                pDC->DrawLine(x1+4, y1, x1+5, y1+1);
                pDC->DrawLine(x1-4, y1-3, x1-4, y1-1);
                pDC->DrawLine(x1-5, y1-1, x1-5, y1+1);
                pDC->DrawLine(x1-4, y1+2, x1-2, y1+4);
                pDC->DrawLine(x1-2, y1+3, x1, y1+5);
                pDC->DrawLine(x1, y1+5, x1+2, y1+5);
                pDC->DrawLine(x1+2, y1+4, x1+4, y1+4);
                break;
            case EDGE_SHAPE_ROCKS:
                // Rocks - only in water-type regions
                if(0==(pLand->Flags&LAND_IS_WATER)) break;
                switch (pEdge->Location)
                {
                    case North:
                        x1 = point[1].x+1;
                        y1 = point[1].y+(scale/2);
                        x2 = point[2].x-1;
                        y2 = point[2].y+(scale/2);
                        break;
                    case Northeast:
                        x1 = point[2].x-(scale/2)+1;
                        y1 = point[2].y+2;
                        x2 = point[3].x-(scale/2);
                        y2 = point[3].y;
                        break;
                    case Southeast:
                        x1 = point[3].x-(scale/2);
                        y1 = point[3].y;
                        x2 = point[4].x-(scale/2)+1;
                        y2 = point[4].y-2;
                        break;
                    case South:
                        x1 = point[4].x-1;
                        y1 = point[4].y-(scale/2);
                        x2 = point[5].x+1;
                        y2 = point[5].y-(scale/2);
                        break;
                    case Southwest:
                        x1 = point[5].x+(scale/2)-1;
                        y1 = point[5].y-2;
                        x2 = point[0].x+(scale/2);
                        y2 = point[0].y;
                        break;
                    case Northwest:
                        x1 = point[0].x+(scale/2);
                        y1 = point[0].y;
                        x2 = point[1].x+(scale/2)-1;
                        y2 = point[1].y+2;
                        break;
                }
                wxPen *pPen = new wxPen(*pProps->pen);
                pPen->SetStyle(wxSHORT_DASH);
                pDC->SetPen(*pPen);
                pDC->DrawLine(x1,y1,x2,y2);
            }
        }
    }
}

//--------------------------------------------------------------------------

void CMapPane::DrawUnitColumn(wxDC * pDC, int x0, int y0, int height)
{
    pDC->DrawLine(x0, y0, x0, y0-height);
    pDC->DrawLine(x0+1, y0, x0+1, y0-height);
    pDC->SetPen(*m_pPenBlack);
    pDC->DrawLine(x0+2, y0, x0+2, y0-height);
}

//--------------------------------------------------------------------------

#define SET_ROAD_PEN(attr)                                              \
    BadRoad = FALSE;                                                    \
    for (i=0; i<pLand->Structs.Count(); i++)                            \
    {                                                                   \
        pStruct = (CStruct*)pLand->Structs.At(i);                       \
        if (pStruct->Attr & attr)                                       \
        {                                                               \
            BadRoad = ((pStruct->Attr & SA_ROAD_BAD) > 0);              \
            break;                                                      \
        }                                                               \
    }                                                                   \
    if (BadRoad)                                                        \
        pDC->SetPen(*m_pPenRoadBad);                                    \
    else                                                                \
        pDC->SetPen(*m_pPenRoad);                                       \

void CMapPane::DrawRoads(wxDC * pDC, CLand * pLand, int x0, int y0)
{
    int           x2, y2;
    int           i;
    CStruct     * pStruct;
    BOOL          BadRoad;

    if (pLand && (pLand->Flags&LAND_STR_ROAD_N))
    {
        SET_ROAD_PEN(SA_ROAD_N)
        pDC->DrawLine(x0, y0, x0, y0-m_HexHalfHeight);
    }
    if (pLand && (pLand->Flags&LAND_STR_ROAD_S))
    {
        SET_ROAD_PEN(SA_ROAD_S)
        pDC->DrawLine(x0, y0, x0, y0+m_HexHalfHeight);
    }
    if (pLand && (pLand->Flags&LAND_STR_ROAD_SE))
    {
        SET_ROAD_PEN(SA_ROAD_SE)
        x2 = x0+m_HexHalfSize*3/2;
        y2 = y0+m_HexHalfHeight/2;
        pDC->DrawLine(x0, y0, x2, y2);
    }
    if (pLand && (pLand->Flags&LAND_STR_ROAD_SW))
    {
        SET_ROAD_PEN(SA_ROAD_SW)
        x2 = x0-m_HexHalfSize*3/2;
        y2 = y0+m_HexHalfHeight/2;
        pDC->DrawLine(x0, y0, x2, y2);
    }
    if (pLand && (pLand->Flags&LAND_STR_ROAD_NE))
    {
        SET_ROAD_PEN(SA_ROAD_NE)
        x2 = x0+m_HexHalfSize*3/2;
        y2 = y0-m_HexHalfHeight/2;
        pDC->DrawLine(x0, y0, x2, y2);
    }
    if (pLand && (pLand->Flags&LAND_STR_ROAD_NW))
    {
        SET_ROAD_PEN(SA_ROAD_NW)
        x2 = x0-m_HexHalfSize*3/2;
        y2 = y0-m_HexHalfHeight/2;
        pDC->DrawLine(x0, y0, x2, y2);
    }
}

//--------------------------------------------------------------------------

void CMapPane::DrawTaxTrade(wxDC * pDC, CLand * pLand, wxPoint * point)
{

    if (m_bAdvancedIcons)
    {
        //i = 0;
        if (pLand && (pLand->Flags&LAND_TAX))
        {
            GetNextIconPos(point, TOP_LEFT_CORNER);
            wxColour * ink = new wxColor(247,249,249); // top/left rim
            wxPen * pen = new wxPen(*ink,1,wxSOLID);
            pDC->SetPen(*pen);
            pDC->DrawLine(SymLeft+2, SymBottom-8, SymLeft+6, SymBottom-8);
            pDC->DrawLine(SymLeft+1, SymBottom-7, SymLeft  , SymBottom-6);
            pDC->DrawLine(SymLeft  , SymBottom-6, SymLeft  , SymBottom-1);
            pDC->DrawLine(SymLeft+1, SymBottom-1, SymLeft+2, SymBottom);
            pDC->DrawLine(SymLeft+2, SymBottom-4, SymLeft+3, SymBottom-4);
            pDC->DrawLine(SymLeft+6, SymBottom-6, SymLeft+6, SymBottom-1);
            pDC->DrawLine(SymLeft+3, SymBottom-5, SymLeft+6, SymBottom-5);
            delete ink;
            delete pen;
            
            wxColour *ink1 = new wxColor(196,200,201); // interior
            wxPen * pen1 = new wxPen(*ink1,1,wxSOLID);
            pDC->SetPen(*pen1);
            pDC->DrawLine(SymLeft+1, SymBottom-6, SymLeft+1, SymBottom-1);
            pDC->DrawLine(SymLeft+2, SymBottom-7, SymLeft+2, SymBottom);
            pDC->DrawLine(SymLeft+3, SymBottom-7, SymLeft+3, SymBottom);
            pDC->DrawLine(SymLeft+5, SymBottom-7, SymLeft+5, SymBottom);
            pDC->DrawLine(SymLeft+6, SymBottom-7, SymLeft+6, SymBottom);
            pDC->DrawLine(SymLeft+7, SymBottom-6, SymLeft+7, SymBottom-1);
            delete ink1;
            delete pen1;
            
            wxColour * ink2 = new wxColor(117,151,154); // lightly embossed
            wxPen * pen2 = new wxPen(*ink2,1,wxSOLID);
            pDC->SetPen(*pen2);
            pDC->DrawLine(SymLeft+2, SymBottom-6, SymLeft+3, SymBottom-6);
            pDC->DrawLine(SymLeft+2, SymBottom-2, SymLeft+3, SymBottom-2);
            pDC->DrawLine(SymLeft+4, SymBottom-7, SymLeft+4, SymBottom);
            pDC->DrawLine(SymLeft+5, SymBottom-8, SymLeft+7, SymBottom-8);
            pDC->DrawLine(SymLeft  , SymBottom-2, SymLeft+2, SymBottom);
            delete ink2;
            delete pen2;
            
            wxColour * ink3 = new wxColor(39,103,107); // embossed colour
            wxPen * pen3 = new wxPen(*ink3,1,wxSOLID);
            pDC->SetPen(*pen3);
            pDC->DrawLine(SymLeft+3, SymBottom-6, SymLeft+6, SymBottom-6);
            pDC->DrawLine(SymLeft+2, SymBottom-5, SymLeft+3, SymBottom-4);
            pDC->DrawLine(SymLeft+3, SymBottom-4, SymLeft+6, SymBottom-4);
            pDC->DrawLine(SymLeft+6, SymBottom-3, SymLeft+5, SymBottom-2);
            pDC->DrawLine(SymLeft+3, SymBottom-2, SymLeft+6, SymBottom-2);
            delete ink3;
            delete pen3;
            
            wxColour * ink4 = new wxColor(137,145,147); // right/bottom shadow rim
            wxPen * pen4 = new wxPen(*ink4,1,wxSOLID);
            pDC->SetPen(*pen4);
            pDC->DrawLine(SymLeft+2, SymBottom, SymLeft+7, SymBottom);
            pDC->DrawLine(SymLeft+7, SymBottom-1, SymLeft+8, SymBottom-2);
            pDC->DrawLine(SymLeft+8, SymBottom-2, SymLeft+8, SymBottom-7);
            pDC->DrawLine(SymLeft+7, SymBottom-7, SymLeft+8, SymBottom-6);
            delete ink4;
            delete pen4;
        }
        if (pLand && (pLand->Flags&LAND_TRADE))
        {
            GetNextIconPos(point, TOP_LEFT_CORNER);
            wxColour * ink = new wxColor(102,33,17); // shaft
            wxPen * pen = new wxPen(*ink,1,wxSOLID);
            pDC->SetPen(*pen);
            pDC->DrawLine(SymLeft+1, SymBottom-1, SymLeft+7, SymBottom-7);
            pDC->DrawLine(SymLeft+3, SymBottom-5, SymLeft+8, SymBottom);
            delete ink;
            delete pen;
    
            wxColour * ink1 = new wxColor(178,144,136); // hilight shaft
            wxPen * pen1 = new wxPen(*ink1,1,wxSOLID);
            pDC->SetPen(*pen1);
            pDC->DrawLine(SymLeft+2, SymBottom-5, SymLeft+3, SymBottom-4);
            pDC->DrawLine(SymLeft+1, SymBottom-2, SymLeft+6, SymBottom-7);
            delete ink1;
            delete pen1;
    
            wxColour * ink2 = new wxColor(106,89,85); // shadow shaft
            wxPen * pen2 = new wxPen(*ink2,1,wxSOLID);
            pDC->SetPen(*pen2);
            pDC->DrawLine(SymLeft+5, SymBottom-4, SymLeft+8, SymBottom-1);
            delete ink2;
            delete pen2;
    
            wxColour * ink3 = new wxColor(118,112,147); // metal rimpixels
            wxPen * pen3 = new wxPen(*ink3,1,wxSOLID);
            pDC->SetPen(*pen3);
            //pDC->DrawLine(SymLeft  , SymBottom-5, SymLeft+1, SymBottom-5);
            pDC->DrawLine(SymLeft+2, SymBottom-6, SymLeft+3, SymBottom-6);
            pDC->DrawLine(SymLeft+7, SymBottom-8, SymLeft+8, SymBottom-8);
            pDC->DrawLine(SymLeft+8, SymBottom-7, SymLeft+7, SymBottom-7);
            delete ink3;
            delete pen3;
    
            wxColour * ink4 = new wxColor(39,75,189); // light metal
            wxPen * pen4 = new wxPen(*ink4,1,wxSOLID);
            pDC->SetPen(*pen4);
            pDC->DrawLine(SymLeft  , SymBottom-8, SymLeft  , SymBottom-5);
            pDC->DrawLine(SymLeft+1, SymBottom-5, SymLeft+2, SymBottom-5);
            pDC->DrawLine(SymLeft+1, SymBottom-8, SymLeft+3, SymBottom-8);
            pDC->DrawLine(SymLeft+4, SymBottom-8, SymLeft+7, SymBottom-8);
            pDC->DrawLine(SymLeft+7, SymBottom-7, SymLeft+8, SymBottom-6);
            delete ink4;
            delete pen4;
    
            wxColour * ink5 = new wxColor(34,21,112); // dark metal
            wxPen * pen5 = new wxPen(*ink5,1,wxSOLID);
            pDC->SetPen(*pen5);
            pDC->DrawLine(SymLeft+1, SymBottom-7, SymLeft+4, SymBottom-7);
            pDC->DrawLine(SymLeft+1, SymBottom-6, SymLeft+4, SymBottom-6);
            pDC->DrawLine(SymLeft+6, SymBottom-7, SymLeft+8, SymBottom-5);
            pDC->DrawLine(SymLeft+8, SymBottom-6, SymLeft+8, SymBottom-3);
            delete ink5;
            delete pen5;
        }
    }
    else // simple icons
    {
        int           i = 0;
        char          buf[128]  = {0};
        
        if (pLand && (pLand->Flags&LAND_TAX))
        {
            buf[i]   = '$';
            buf[++i] = 0;
        }
        if (pLand && (pLand->Flags&LAND_TRADE))
        {
            buf[i]   = 'P';
            buf[++i] = 0;
        }

        if (*buf)
            pDC->DrawText(buf,  point[1].x, point[1].y+1 );
    }
}

//--------------------------------------------------------------------------

void CMapPane::DrawCity(wxDC * pDC, CLand * pLand, int x0, int y0)
{
    if (pLand && pLand->Flags&LAND_TOWN)
    {
        pDC->SetPen  (*m_pPenBlack);
        pDC->SetBrush(*m_pBrushWhite);
        pDC->DrawCircle(x0, y0, 4);
        pDC->SetPen  (*m_pPenBlack);
        pDC->SetBrush(*m_pBrushBlack);
        pDC->DrawCircle(x0, y0, 2);
    }
    else if (pLand && pLand->Flags&LAND_CITY)
    {
        pDC->SetPen  (*m_pPenBlack);
        pDC->SetBrush(*m_pBrushWhite);
        pDC->DrawCircle(x0, y0, 5);
        pDC->SetBrush(*wxLIGHT_GREY_BRUSH);
        pDC->DrawCircle(x0, y0, 4);
        pDC->SetPen  (*m_pPenBlack);
        pDC->SetBrush(*m_pBrushBlack);
        pDC->DrawCircle(x0, y0, 1);
    }
    else
    {
        // draw a smaller circle
        pDC->SetPen  (*m_pPenBlack);
        pDC->SetBrush(*wxLIGHT_GREY_BRUSH);
        pDC->DrawCircle(x0, y0, 3);
        pDC->SetPen  (*m_pPenBlack);
        pDC->SetBrush(*m_pBrushBlack);
        pDC->DrawCircle(x0, y0, 1);
    }
}

//--------------------------------------------------------------------------

void CMapPane::DrawUnits(wxDC * pDC, CLand * pLand, wxPoint * point)
{
    long          x;
    EValueType    type;

    if (m_bAdvancedIcons)
    {
        // set unit color
        int color = ATT_FRIEND1;
        if(pLand->AlarmFlags&PRESENCE_ENEMY)
        {
            color = ATT_ENEMY;
        }
        else if(pLand->Flags&PRESENCE_NEUTRAL)
        {
            color = ATT_NEUTRAL;
        }
        else if(pLand->Flags&PRESENCE_OWN)
        {
            color = ATT_FRIEND2;
        }
    
        if (!pLand->GetProperty(PRP_SEL_FACT_MEN, type, (const void *&)x, eNormal) || (eLong!=type))
            x=0;
        GetNextIconPos(point, BOTTOM_LEFT_CORNER);
    
        // show flag icon for guards' stance
        BOOL mark = pLand && pLand->AlarmFlags&PRESENCE_OWN;
        if(pLand&&pLand->AlarmFlags&GUARDED)
        {
            if(pLand&&pLand->AlarmFlags&GUARDED_BY_OWN)
            {
                color = ATT_FRIEND2;
                mark=FALSE;
            }
            else if (pLand&&pLand->AlarmFlags&GUARDED_BY_FRIEND) color = ATT_FRIEND1;
            else if (pLand&&pLand->AlarmFlags&GUARDED_BY_ENEMY)  color = ATT_ENEMY;
            else color = ATT_NEUTRAL;
            DrawFlagIcon(pDC, SymLeft, SymBottom, *m_pUnitColor[color], *m_pDarkColor[color], (x>m_MinSelMen), mark);
        } else // draw normal shield
        {
            if(pLand&&pLand->AlarmFlags&CLAIMED_BY_OWN)
            {
                color = ATT_FRIEND2;
                mark=FALSE;
            }
            else if (pLand&&pLand->AlarmFlags&CLAIMED_BY_FRIEND) color = ATT_FRIEND1;
            else if (pLand&&pLand->AlarmFlags&CLAIMED_BY_ENEMY)  color = ATT_ENEMY;
            else color = ATT_NEUTRAL;
            DrawShieldIcon(pDC, SymLeft, SymBottom, *m_pUnitColor[color], *m_pDarkColor[color], (x>m_MinSelMen), mark);
        }
    
        // draw troop strength columns
        int xc = SymLeft+7;
        if(m_Detail >= MILITARY_DETAIL)
        {
            for(int side=ATT_FRIEND1; side < ATT_UNDECLARED; side++)
            {
            // juggle the order of FRIEND1 / FRIEND2 !
            if(side==ATT_FRIEND1) side = ATT_FRIEND2;
            else if(side==ATT_FRIEND2) side = ATT_FRIEND1;
            if(pLand && (pLand->Troops[side] > 0))
            {
                xc += 2;
                wxPen * pen = new wxPen(*m_pUnitColor[side],1,wxSOLID);
                pDC->SetPen(*pen);
                DrawUnitColumn(pDC, xc, SymBottom, (int) pLand->Troops[side]);
                delete pen;
            }
            }
        }
        SymLeft = -1;
    
        GetNextIconPos(point, BOTTOM_RIGHT_CORNER);
        // draw alarm icon
        if((m_Detail>=TOWN_DETAIL)&&pLand&&(pLand->AlarmFlags&ALARM))
        {
            xc=SymLeft; //+18;
            if(pLand->AlarmFlags&PRESENCE_ENEMY) color = ATT_ENEMY;
            else color = ATT_NEUTRAL;
            if((pLand->AlarmFlags&PRESENCE_ENEMY)||(pLand->AlarmFlags&PRESENCE_NEUTRAL))
            {
                wxPen * pen = new wxPen(*m_pUnitColor[color],1,wxSOLID);
                pDC->SetPen(*pen);
                pDC->DrawLine(xc  , SymBottom  , xc  , SymBottom-1);
                pDC->DrawLine(xc  , SymBottom-3, xc  , SymBottom-8);
                delete pen;
                pDC->SetPen(*m_pPenBlack);
                pDC->DrawLine(xc  , SymBottom-1, xc+2, SymBottom-1);
                pDC->DrawLine(xc  , SymBottom-8, xc+2, SymBottom-8);
                pDC->DrawLine(xc+1, SymBottom  , xc+1, SymBottom-2);
                pDC->DrawLine(xc+1, SymBottom-3, xc+1, SymBottom-8);
            }
        }
    }
    else  // simple icons
    {
        int x;
        
        GetNextIconPos(point, BOTTOM_LEFT_CORNER);
        
        if (!pLand->GetProperty(PRP_SEL_FACT_MEN, type, (const void *&)x, eNormal) || (eLong!=type))
            x=0;

        if (x>m_MinSelMen)
            x = ICON_SIZE;
        else
            x = ICON_SIZE_SMALL;


        pDC->SetPen  (*m_pPenBlack);
        pDC->SetBrush(*m_pBrushRed);
        pDC->DrawCircle(SymLeft+x/2, SymBottom-x/2, x/2);

        //GetNextIconPos(x0, y0, SymLeft, SymBottom);
    }
}

//--------------------------------------------------------------------------

#define ICON_SIZE       8
#define ICON_SIZE_SMALL 6

void CMapPane::DrawStructures(wxDC * pDC, CLand * pLand, wxPoint * point, int x0, int y0)
{
    if (m_bAdvancedIcons)
    {
        // Gate
        if (pLand && (pLand->Flags&LAND_STR_GATE) && (m_Detail>=SPECIAL_DETAIL))
        {
            GetNextIconPos(point, RIGHT_CORNER);
            pDC->SetPen  (*m_pPenBlack);
            pDC->SetBrush(*m_pBrushBlack);
        #ifdef __WXMSW__
            pDC->DrawRectangle(SymLeft, SymBottom-ICON_SIZE, ICON_SIZE, ICON_SIZE);
        #else
            pDC->DrawRectangle(SymLeft, SymBottom-ICON_SIZE, ICON_SIZE+1, ICON_SIZE+1);
        #endif
    
            pDC->SetBrush(*m_pBrushWhite);
            pDC->DrawCircle(SymLeft+ICON_SIZE/2, SymBottom-ICON_SIZE/2, ICON_SIZE/2);
        }
    
        // Shaft
        if (pLand && (pLand->Flags&LAND_STR_SHAFT) && (m_Detail>=SPECIAL_DETAIL))
        {
            GetNextIconPos(point, RIGHT_CORNER);
            pDC->SetPen  (*m_pPenBlack);
            pDC->DrawLine(SymLeft          , SymBottom-ICON_SIZE, SymLeft          , SymBottom);
            pDC->DrawLine(SymLeft+ICON_SIZE, SymBottom-ICON_SIZE, SymLeft+ICON_SIZE, SymBottom);
    
            pDC->DrawLine(SymLeft, SymBottom-3          , SymLeft+ICON_SIZE, SymBottom-3          );
            pDC->DrawLine(SymLeft, SymBottom-ICON_SIZE+2, SymLeft+ICON_SIZE, SymBottom-ICON_SIZE+2);
        }
    
        // Ship
        if (pLand && pLand->Flags & LAND_STR_MOBILE && (m_Detail>=MILITARY_DETAIL))
        {
            GetNextIconPos(point, LEFT_CORNER);
            // Hull and Mast
            wxColour * ink = new wxColor(83,36,27);
            wxPen * pen = new wxPen(*ink,1,wxSOLID);
            pDC->SetPen(*pen);
            pDC->DrawLine(SymLeft+4, SymBottom-8, SymLeft+4, SymBottom);
            pDC->DrawLine(SymLeft+6, SymBottom-4, SymLeft+6, SymBottom-3);
            pDC->DrawLine(SymLeft, SymBottom-2, SymLeft+2, SymBottom);
            pDC->DrawLine(SymLeft+2, SymBottom-1, SymLeft+9, SymBottom-1);
            pDC->DrawLine(SymLeft+2, SymBottom, SymLeft+8, SymBottom);
            pDC->DrawLine(SymLeft+7, SymBottom-2, SymLeft+9, SymBottom-2);
            delete ink;
            delete pen;
    
            // Hull hilight
            wxColour * ink1 = new wxColor(134,101,95);
            wxPen * pen1 = new wxPen(*ink1,1,wxSOLID);
            pDC->SetPen(*pen1);
            pDC->DrawLine(SymLeft+3, SymBottom-8, SymLeft+3, SymBottom);
            pDC->DrawLine(SymLeft  , SymBottom-2, SymLeft  , SymBottom);
            pDC->DrawLine(SymLeft+1, SymBottom  , SymLeft+2, SymBottom);
            pDC->DrawLine(SymLeft+6, SymBottom-2, SymLeft+5, SymBottom-2);
            pDC->DrawLine(SymLeft+8, SymBottom-1, SymLeft+8, SymBottom);
            delete ink1;
            delete pen1;
            // Sail
            wxColour * ink2 = new wxColor(255,245,201);
            wxPen * pen2 = new wxPen(*ink2,1,wxSOLID);
            pDC->SetPen(*pen2);
            pDC->DrawLine(SymLeft+2, SymBottom-7, SymLeft+8, SymBottom-7);
            pDC->DrawLine(SymLeft+1, SymBottom-6, SymLeft+8, SymBottom-6);
            pDC->DrawLine(SymLeft+1, SymBottom-5, SymLeft+7, SymBottom-5);
            pDC->DrawLine(SymLeft+1, SymBottom-4, SymLeft+6, SymBottom-4);
            pDC->DrawLine(SymLeft+2, SymBottom-3, SymLeft+6, SymBottom-3);
            delete ink2;
            delete pen2;
            // Sail darker shades
            wxColour * ink3 = new wxColor(220,212,171);
            wxPen * pen3 = new wxPen(*ink3,1,wxSOLID);
            pDC->SetPen(*pen3);
            pDC->DrawLine(SymLeft+2, SymBottom-7, SymLeft  , SymBottom-5);
            pDC->DrawLine(SymLeft+2, SymBottom-4, SymLeft+4, SymBottom-2);
            pDC->DrawLine(SymLeft+6, SymBottom-6, SymLeft+4, SymBottom-4);
            pDC->DrawLine(SymLeft+7, SymBottom-6, SymLeft+4, SymBottom-3);
            delete ink3;
            delete pen3;
            // Sail shadow
            wxColour * ink4 = new wxColor(124,118,89);
            wxPen * pen4 = new wxPen(*ink4,1,wxSOLID);
            pDC->SetPen(*pen4);
            pDC->DrawLine(SymLeft+8, SymBottom-7, SymLeft+8, SymBottom-6);
            pDC->DrawLine(SymLeft+8, SymBottom-6, SymLeft+6, SymBottom-4);
            delete ink4;
            delete pen4;
        }
    
        // Generic struct
        if (pLand && pLand->Flags & LAND_STR_GENERIC && (m_Detail>=SPECIAL_DETAIL))
        {
            GetNextIconPos(point, TOP_RIGHT_CORNER);
            wxPoint tri[3] =
            {
                wxPoint(SymLeft,             SymBottom-1),
                wxPoint(SymLeft+ICON_SIZE/2, SymBottom-ICON_SIZE),
                wxPoint(SymLeft+ICON_SIZE,   SymBottom-1)
            };
    
            pDC->SetPen  (*m_pPenBlack);
            pDC->SetBrush(*m_pBrushBlack);
            pDC->DrawPolygon(3, tri, 0, 0, wxWINDING_RULE);
        }
    
        // Battle
        if (pLand && (pLand->Flags&LAND_BATTLE))
        {
            pDC->SetPen  (*m_pPenRed);
            pDC->DrawLine(x0-6, y0+1, x0-3, y0+4);
            pDC->DrawLine(x0+6, y0, x0+2, y0+4);
            //pDC->SetPen  (*m_pPenBlack);
            pDC->DrawLine(x0-6, y0, x0-2, y0+4);
            pDC->DrawLine(x0+6, y0+1, x0+3, y0+4);
            pDC->SetPen  (*m_pPenRed);
            pDC->DrawLine(x0-5, y0-7, x0+7, y0+5);
            pDC->DrawLine(x0+4, y0-7, x0-8, y0+5);
            //pDC->SetPen  (*m_pPenBlack);
            pDC->DrawLine(x0-4, y0-7, x0+8, y0+5);
            pDC->DrawLine(x0+5, y0-7, x0-7, y0+5);
        }
    
        // Located units
        if (pLand && pLand->Flags & LAND_LOCATED_UNITS)
        {
            pDC->SetPen  (*m_pPenSel);
            pDC->SetBrush( *wxTRANSPARENT_BRUSH);
    
            pDC->DrawCircle(x0, y0, m_HexHalfSize);
        }
    
        // the hex itself was located
        if (pLand && pLand->Flags & LAND_LOCATED_LAND)
        {
            pDC->SetPen  (*m_pPenRed );
            pDC->SetBrush(*m_pBrushRed);
    
            wxPoint points[] = { wxPoint(x0, y0-m_HexHalfSize),
                                wxPoint( (long)(x0+m_HexHalfSize*cos30), y0+m_HexHalfSize/2),
                                wxPoint( (long)(x0-m_HexHalfSize*cos30), y0+m_HexHalfSize/2) };
    
            pDC->DrawPolygon(sizeof(points)/sizeof(*points), points);
        }
    }
    
    
    
    else         //  ********** SIMPLE ICONS
    {
        // Gate
        if (pLand && (pLand->Flags&LAND_STR_GATE))
        {
            GetNextIconPos(point, BOTTOM_LEFT_CORNER);
            pDC->SetPen  (*m_pPenBlack);
            pDC->SetBrush(*m_pBrushBlack);
        #ifdef __WXMSW__
            pDC->DrawRectangle(SymLeft, SymBottom-ICON_SIZE, ICON_SIZE, ICON_SIZE);
        #else
            pDC->DrawRectangle(SymLeft, SymBottom-ICON_SIZE, ICON_SIZE+1, ICON_SIZE+1);
        #endif

            pDC->SetBrush(*m_pBrushWhite);
            pDC->DrawCircle(SymLeft+ICON_SIZE/2, SymBottom-ICON_SIZE/2, ICON_SIZE/2);

            //GetNextIconPos(x0, y0, SymLeft, SymBottom);
        }

        // Shaft
        if (pLand && (pLand->Flags&LAND_STR_SHAFT))
        {
            GetNextIconPos(point, BOTTOM_LEFT_CORNER);
            pDC->SetPen  (*m_pPenBlack);
            pDC->DrawLine(SymLeft          , SymBottom-ICON_SIZE, SymLeft          , SymBottom);
            pDC->DrawLine(SymLeft+ICON_SIZE, SymBottom-ICON_SIZE, SymLeft+ICON_SIZE, SymBottom);

            pDC->DrawLine(SymLeft, SymBottom-3          , SymLeft+ICON_SIZE, SymBottom-3          );
            pDC->DrawLine(SymLeft, SymBottom-ICON_SIZE+2, SymLeft+ICON_SIZE, SymBottom-ICON_SIZE+2);

            //GetNextIconPos(x0, y0, SymLeft, SymBottom);
        }

        // Ship
        if (pLand && pLand->Flags & LAND_STR_MOBILE)
        {
            GetNextIconPos(point, BOTTOM_LEFT_CORNER);
            wxPoint tri[3] =
            {
                wxPoint(SymLeft,             SymBottom-ICON_SIZE),
                wxPoint(SymLeft+ICON_SIZE/2, SymBottom-1),
                wxPoint(SymLeft+ICON_SIZE,   SymBottom-ICON_SIZE)
            };

            pDC->SetPen  (*m_pPenBlack);
            pDC->SetBrush(*m_pBrushBlack);
            pDC->DrawPolygon(3, tri, 0, 0, wxWINDING_RULE);

            //GetNextIconPos(x0, y0, SymLeft, SymBottom);
        }

        // Generic struct
        if (pLand && pLand->Flags & LAND_STR_GENERIC)
        {
            GetNextIconPos(point, BOTTOM_LEFT_CORNER);
            wxPoint tri[3] =
            {
                wxPoint(SymLeft,             SymBottom-1),
                wxPoint(SymLeft+ICON_SIZE/2, SymBottom-ICON_SIZE),
                wxPoint(SymLeft+ICON_SIZE,   SymBottom-1)
            };

            pDC->SetPen  (*m_pPenBlack);
            pDC->SetBrush(*m_pBrushBlack);
            pDC->DrawPolygon(3, tri, 0, 0, wxWINDING_RULE);

            //GetNextIconPos(x0, y0, SymLeft, SymBottom);
        }

        // Battle
        if (pLand && (pLand->Flags&LAND_BATTLE))
        {
            pDC->SetPen  (*m_pPenRed);

            pDC->DrawLine(x0-5, y0-5, x0+6, y0+6);
            pDC->DrawLine(x0+5, y0-5, x0-6, y0+6);
        }

        // Located units
        if (pLand && pLand->Flags & LAND_LOCATED_UNITS)
        {
            pDC->SetPen  (*m_pPenSel);
            pDC->SetBrush( *wxTRANSPARENT_BRUSH);

            pDC->DrawCircle(x0, y0, m_HexHalfSize);
        }

        // the hex itself was located
        if (pLand && pLand->Flags & LAND_LOCATED_LAND)
        {
            pDC->SetPen  (*m_pPenRed );
            pDC->SetBrush(*m_pBrushRed);

            wxPoint points[] = { wxPoint(x0, y0-m_HexHalfSize),
                                 wxPoint((int)(x0+m_HexHalfSize*cos30), y0+m_HexHalfSize/2),
                                 wxPoint((int)(x0-m_HexHalfSize*cos30), y0+m_HexHalfSize/2) };

            pDC->DrawPolygon(sizeof(points)/sizeof(*points), points);
        }
    }
}

//--------------------------------------------------------------------------

void CMapPane::DrawHexFlags(wxDC * pDC, CLand * pLand, int x0, int y0)
{
    int i;
    static int dfx[LAND_FLAG_COUNT] = {-4,  0,  4};
    static int dfy[LAND_FLAG_COUNT] = { 4,  0,  4};

    for (i=0; i<LAND_FLAG_COUNT; i++)
        if (pLand && !pLand->FlagText[i].IsEmpty())
        {
            pDC->SetPen  (*m_pPenFlag[i]);
            pDC->DrawLine(x0+dfx[i],   y0+dfy[i]   , x0+dfx[i]  , y0+dfy[i]-10);
            pDC->DrawLine(x0+dfx[i],   y0+dfy[i]-10, x0+dfx[i]+6, y0+dfy[i]-7 );
            pDC->DrawLine(x0+dfx[i]+6, y0+dfy[i]-7 , x0+dfx[i],   y0+dfy[i]-4 );
        }
}

//--------------------------------------------------------------------------

void CMapPane::GetNextIconPos(wxPoint * point, int Position)
{
    BOOL reverse=FALSE;
    int x1,y1,x2,y2;
    int spacer = m_HexSize / 4;
    if(spacer < 2) spacer = 2;
    int increment_x = ICON_SIZE + (spacer / 4) + 1;
//    int increment_y = 0;

    int previous_icons = m_HexIcons[Position];

    switch(Position)
    {
        case(TOP_LEFT_CORNER):
            x1 = point[1].x + (spacer / 4);
            x2 = (point[2].x + point[1].x) / 2 + spacer;
            y1 = point[1].y + spacer + ICON_SIZE;
            y2 = y1;
            break;
        case(TOP_RIGHT_CORNER):
            x1 = (point[2].x + point[1].x) / 2 + (spacer / 4);
            x2 = point[2].x - ICON_SIZE - (spacer / 4);
            y1 = point[2].y + spacer + ICON_SIZE;
            y2 = y1;
            reverse=TRUE;
            increment_x = - ICON_SIZE - (spacer / 4) - 1;
            break;
        case(RIGHT_CORNER):
            x1 = (point[2].x + point[1].x) / 2 + (spacer / 2);
            x2 = point[3].x - (spacer / 2) - ICON_SIZE - 1;
            y1 = point[3].y + (ICON_SIZE / 2) + 1;
            y2 = y1;
            reverse = TRUE;
            increment_x = - ICON_SIZE - (spacer / 4) - 1;
            break;
        case(LEFT_CORNER):
            x1 = point[0].x + (spacer / 2) + 2;
            x2 = (point[2].x + point[1].x) / 2 - (spacer / 2) - ICON_SIZE - 1;
            y1 = point[0].y + (ICON_SIZE / 2) + 1;
            y2 = y1;
            break;
        case(BOTTOM_LEFT_CORNER):
            x1 = point[5].x + (spacer / 4);
            x2 = (point[2].x + point[1].x) / 2 + spacer;
            y1 = point[5].y - spacer;
            y2 = y1;
            break;
        case(BOTTOM_RIGHT_CORNER):
            x1 = (point[2].x + point[1].x) / 2 + (spacer / 4);
            x2 = point[4].x - ICON_SIZE - (spacer / 4);
            y1 = point[4].y - spacer;
            y2 = y1;
            reverse=TRUE;
            increment_x = - ICON_SIZE - (spacer / 4) - 1;
            break;
        case(CENTER): // for consistency purposes
            x1 = (point[0].x + point[3].x) / 2 - spacer + ICON_SIZE + 1;
            x2 = (point[0].x + point[3].x) / 2 + spacer - ICON_SIZE - 1;
            y1 = point[0].y - ICON_SIZE - 1;
            y2 = point[0].y + ICON_SIZE + 1;
            break;
    }
    
    SymBottom = y1;
    if(reverse)
    {
        SymLeft = x2 + previous_icons * increment_x;
    } else
    {
        SymLeft = x1 + previous_icons * increment_x;
    }

    if((SymLeft<x1)||(SymLeft>x2))
    {
        if(SymBottom < point[0].y)
        {
            SymBottom = y1 + ICON_SIZE + 1;
        } else
        {
            SymBottom = y1 - ICON_SIZE - 1;
        }
        int second_row_icons = previous_icons;
        int x = SymLeft - (previous_icons - second_row_icons) * increment_x;
        if((x<x1) || (x>x2))
        {
            for(second_row_icons = previous_icons; second_row_icons >= 0; second_row_icons--)
            {
                x = SymLeft - (previous_icons - second_row_icons) * increment_x;
                if((x>=x1) && (x<=x2)) break;
            }
        }
        // adjust the spacing for the right and left corner second rows
        // - they would overflow the hex corners otherwise
        if((Position == RIGHT_CORNER)||(Position == LEFT_CORNER)) second_row_icons++;
        if(reverse)
        {
            SymLeft = x2 + second_row_icons * increment_x;
        } else
        {
            SymLeft = x1 + second_row_icons * increment_x;
        }
    }
    m_HexIcons[Position] += 1;
}

//--------------------------------------------------------------------------


void CMapPane::DrawOneHex(int NoX, int NoY, int x0, int y0, wxDC * pDC, CLand * pLand, CPlane * pPlane)
{

//    char          buf[128]  = {0};
    int           i;
    //long          x;
    //EValueType    type;
    wxBrush     * pBrush;
    wxColour      pUnitColor;
    wxColour      pGuardColor;
    wxColour      pDarkColor;
//    wxPen       * pPen;


    wxPoint       point[7]  =
    {
        wxPoint(x0 - m_HexSize    , y0                   ),   //0
        wxPoint(x0 - m_HexHalfSize, y0 - m_HexHalfHeight ),   //1
        wxPoint(x0 + m_HexHalfSize, y0 - m_HexHalfHeight ),   //2
        wxPoint(x0 + m_HexSize    , y0                   ),   //3
        wxPoint(x0 + m_HexHalfSize, y0 + m_HexHalfHeight ),   //4
        wxPoint(x0 - m_HexHalfSize, y0 + m_HexHalfHeight ),   //5
        wxPoint(x0 - m_HexSize    , y0                   )    //6
    };

    for(int pos = 0; pos < 7; pos++)
    {
        m_HexIcons[pos] = 0;
    }

    if (m_Detail<EDGE_DETAIL)
        pDC->SetPen(*wxTRANSPARENT_PEN);
    else
        pDC->SetPen(*m_pPen);

    // Hexes
    pBrush = GetLandBrush(pLand, FALSE);
    pDC->SetBrush(*pBrush);
    pDC->DrawPolygon(7, point, 0, 0, wxWINDING_RULE);

    if (m_Hatch && pLand && (0==(pLand->Flags&LAND_VISITED)))
    {
        pBrush = GetLandBrush(pLand, TRUE);
        pDC->SetBrush(*pBrush);
        pDC->DrawPolygon(7, point, 0, 0, wxWINDING_RULE);
    }

    // Roads (Atlantis-Standard-Type)
    if (m_Detail>=ROAD_DETAIL)
        DrawRoads(pDC, pLand, x0, y0);

    // Coastline
    for (i=0; i<6; i++)  // just assuming we have six directions!
        if(pLand && (pLand->CoastBits&ExitFlags[i]))
            DrawCoastalLine(pDC, point, i);

    // Edge structures
    DrawEdgeStructures(pDC, pLand, point, x0, y0);

    // Tax and Production
    if (pLand && m_Detail>=ECONOMIC_DETAIL)
        DrawTaxTrade(pDC, pLand, point);

    // Cities
    if (pLand && m_Detail>=TOWN_DETAIL && !pLand->CityName.IsEmpty())
        DrawCity(pDC, pLand, x0, y0);

    // Units
    if (pLand && pLand->Flags&LAND_UNITS && m_Detail>=UNIT_DETAIL)
        DrawUnits(pDC, pLand, point);

    if (m_Detail>=BATTLE_DETAIL)
        DrawStructures(pDC, pLand, point, x0, y0);

    // Flags
    if (m_Detail>=FLAG_DETAIL)
        DrawHexFlags(pDC, pLand, x0, y0);
}




//--------------------------------------------------------------------------

void CMapPane::DrawShieldIcon(wxDC * pDC, int x, int y, wxColour pGuardColor, wxColour pDarkColor, BOOL large, BOOL mark)
{
        if (large)
        {
            wxPen * pen = new wxPen(pGuardColor,1,wxSOLID);
            pDC->SetPen(*pen);
            pDC->DrawLine(x+1, y-7, x+1, y-2);
            pDC->DrawLine(x+2, y-7, x+2, y);
            pDC->DrawLine(x+3, y-7, x+3, y);
            pDC->DrawLine(x+4, y-7, x+4, y);
            pDC->DrawLine(x+5, y-7, x+5, y);
            pDC->DrawLine(x+6, y-7, x+6, y-2);
            delete pen;

            wxPen * pen1 = new wxPen(pDarkColor,1,wxSOLID);;
            pDC->SetPen(*pen1);
            pDC->DrawLine(x+4, y-7, x+7, y-7);
            pDC->DrawLine(x+5, y-6, x+7, y-6);
            pDC->DrawLine(x+5, y-5, x+7, y-5);
            pDC->DrawLine(x+5, y-4, x+7, y-4);
            pDC->DrawLine(x+4, y-3, x+7, y-3);
            pDC->DrawLine(x+4, y-2, x+6, y-2);
            pDC->DrawLine(x+4, y-1, x+6, y-1);
            delete pen1;

            pDC->SetPen(*m_pPenGrey);
            pDC->DrawLine(x+1, y-8, x+8, y-8);
            pDC->DrawLine(x  , y-7, x  , y-2);
            pDC->DrawLine(x+7, y-7, x+7, y-2);
            pDC->DrawLine(x+1, y-2, x+1, y);
            pDC->DrawLine(x+6, y-2, x+6, y);
            pDC->DrawLine(x+2, y  , x+6, y);

            if (mark)
            {
                pGuardColor = *m_pUnitColor[ATT_FRIEND2];
                wxPen * pen = new wxPen(pGuardColor,1,wxSOLID);
                pDC->SetPen(*pen);
                pDC->DrawLine(x+2, y-5, x+2, y-2);
                pDC->DrawLine(x+3, y-4, x+3, y-1);
                delete pen;
            }
        }
        else
        {
            pDC->SetPen(*m_pPenGrey);
            pDC->DrawLine(x+2, y-6, x+5, y-6);
            pDC->DrawLine(x+1, y-5, x+6, y-5);
            pDC->DrawLine(x  , y-4, x+7, y-4);
            pDC->DrawLine(x  , y-3, x+7, y-3);
            pDC->DrawLine(x  , y-2, x+7, y-2);
            pDC->DrawLine(x+1, y-1, x+6, y-1);
            pDC->DrawLine(x+2, y  , x+5, y  );
            
            if (mark)
            {
                pDarkColor = pGuardColor;
                pGuardColor = *m_pUnitColor[ATT_FRIEND2];
            }
            wxPen * pen = new wxPen(pGuardColor,1,wxSOLID);
            pDC->SetPen(*pen);
            pDC->DrawLine(x+1, y-4, x+1, y-1);
            pDC->DrawLine(x+2, y-5, x+2, y);
            pDC->DrawLine(x+3, y-5, x+3, y-2);
            delete pen;
            
            wxPen * pen1 = new wxPen(pDarkColor,1,wxSOLID);
            pDC->SetPen(*pen1);
            pDC->DrawLine(x+3, y-2, x+3, y);
            pDC->DrawLine(x+4, y-5, x+4, y);
            pDC->DrawLine(x+5, y-4, x+5, y-1);
            delete pen1;
        }
}

//--------------------------------------------------------------------------

void CMapPane::DrawFlagIcon(wxDC * pDC, int x, int y, wxColour FlagColor, wxColor DarkColor, BOOL large, BOOL presence)
{
        if (large)
        {
            pDC->SetPen  (*m_pPenBlack);
            pDC->DrawLine(x  , y-8, x  , y+1);
            pDC->SetPen  (*m_pPenGrey);
            pDC->DrawLine(x  , y-9, x+4, y-9);
            pDC->DrawLine(x+4, y-8, x+8, y-8);
            pDC->DrawLine(x+1, y-3, x+5, y-3);
            pDC->DrawLine(x+5, y-2, x+8, y-2);
            
            wxPen * pen = new wxPen(DarkColor,1,wxSOLID);
            pDC->SetPen(*pen);
            pDC->DrawLine(x+1, y-6, x+1, y-3);
            pDC->DrawLine(x+2, y-8, x+4, y-8);
            pDC->DrawLine(x+3, y-8, x+3, y-4);
            pDC->DrawLine(x+4, y-5, x+4, y-3);
            pDC->DrawLine(x+5, y-3, x+4, y-4);
            pDC->DrawLine(x+5, y-7, x+5, y-6);
            pDC->DrawLine(x+6, y-6, x+6, y-4);
            pDC->DrawLine(x+7, y-7, x+6, y-6);
            pDC->DrawLine(x+7, y-4, x+7, y-2);
            delete pen;
            
            wxPen * pen1 = new wxPen(FlagColor,1,wxSOLID);
            pDC->SetPen(*pen1);
            pDC->DrawLine(x+1, y-8, x+1, y-6);
            pDC->DrawLine(x+2, y-7, x+2, y-3);
            pDC->DrawLine(x+3, y-4, x+2, y-4);
            pDC->DrawLine(x+4, y-7, x+4, y-5);
            pDC->DrawLine(x+5, y-6, x+5, y-3);
            pDC->DrawLine(x+6, y-7, x+6, y-6);
            pDC->DrawLine(x+6, y-4, x+6, y-2);
            delete pen1;
            
            if (presence)
            {
                pDC->SetPen  (*m_pPenGrey);
                pDC->DrawLine(x+1, y-1, x+5, y-1);
                DarkColor = * m_pDarkColor[ATT_FRIEND2];
                wxPen * pen = new wxPen(DarkColor,1,wxSOLID);
                pDC->SetPen(*pen);
                pDC->DrawLine(x+1, y  , x+5, y);
                delete pen;
                FlagColor = * m_pUnitColor[ATT_FRIEND2];
                wxPen * pen1 = new wxPen(FlagColor,1,wxSOLID);
                pDC->SetPen(*pen1);
                pDC->DrawLine(x+2, y-1, x+4, y-1);
                pDC->DrawLine(x+2, y  , x+4, y);
                delete pen1;
            }
        }
        else
        {
            pDC->SetPen  (*m_pPenBlack);
            pDC->DrawLine(x  , y-7, x  , y+1);
            pDC->SetPen  (*m_pPenGrey);
            pDC->DrawLine(x  , y-8, x+3, y-8);
            pDC->DrawLine(x+3, y-7, x+4, y-7);
            pDC->DrawLine(x+4, y-6, x+4, y-4);
            pDC->DrawLine(x+5, y-4, x+7, y-4);
            pDC->DrawLine(x+1, y-3, x+4, y);
            pDC->DrawLine(x+3, y-1, x+6, y-1);
            
            wxPen * pen = new wxPen(DarkColor,1,wxSOLID);
            pDC->SetPen(*pen);
            pDC->DrawLine(x+1, y-4, x+1, y-3);
            pDC->DrawLine(x+2, y-7, x+2, y-4);
            pDC->DrawLine(x+3, y-3, x+3, y-1);
            pDC->DrawLine(x+4, y-4, x+4, y-3);
            pDC->DrawLine(x+5, y-2, x+5, y-1);
            delete pen;
            
            wxPen * pen1 = new wxPen(FlagColor,1,wxSOLID);
            pDC->SetPen(*pen1);
            pDC->DrawLine(x+1, y-7, x+1, y-4);
            pDC->DrawLine(x+2, y-4, x+2, y-2);
            pDC->DrawLine(x+3, y-6, x+3, y-3);
            pDC->DrawLine(x+4, y-3, x+4, y-1);
            pDC->DrawLine(x+5, y-3, x+7, y-3);
            delete pen1;
            
            if (presence)
            {
                pDC->SetPen  (*m_pPenGrey);
                pDC->DrawLine(x+1, y-1, x+5, y-1);
                DarkColor = * m_pDarkColor[ATT_FRIEND2];
                wxPen * pen = new wxPen(DarkColor,1,wxSOLID);
                pDC->SetPen(*pen);
                pDC->DrawLine(x+1, y  , x+5, y);
                delete pen;
                FlagColor = * m_pUnitColor[ATT_FRIEND2];
                wxPen * pen1 = new wxPen(FlagColor,1,wxSOLID);
                pDC->SetPen(*pen1);
                pDC->DrawLine(x+2, y-1, x+4, y-1);
                pDC->DrawLine(x+2, y  , x+4, y);
                delete pen1;
            }
        }
}

//--------------------------------------------------------------------------

void CMapPane::DrawHex(int NoX, int NoY, wxDC * pDC, CLand * pLand, CPlane * pPlane, wxRect * prect)
{
    if (!ValidHexNo(NoX, NoY))
        return;

    int           x0, y0;  // hex center
    int           dwx;

    //NormalizeHexX(NoX, pPlane);
    GetHexCenter(NoX, NoY, x0, y0);

    if (pPlane && (pPlane->Width>0) )
    {
        dwx = (m_HexSize * 3 / 2) * pPlane->Width;

        while (x0 > prect->x-m_HexSize)
            x0 -= dwx;
        x0 += dwx;

        while (x0<prect->x + prect->width + m_HexSize)
        {
            DrawOneHex(NoX, NoY, x0, y0, pDC, pLand, pPlane);
            x0 += dwx;
        }

    }
    else
        DrawOneHex(NoX, NoY, x0, y0, pDC, pLand, pPlane);

}

//--------------------------------------------------------------------------

void CMapPane::DrawOneHexBorder(int x0, int y0, wxDC * pDC, BOOL IsSelected, BOOL FullBorder, wxPen ** pPens, int ExitBits)
{
    int  i;
    int  IsExit;
    int  ExitPen = 1;

    wxPoint point[7] =
    {
        wxPoint(x0 - m_HexSize    , y0                   ),   //0
        wxPoint(x0 - m_HexHalfSize, y0 - m_HexHalfHeight ),   //1
        wxPoint(x0 + m_HexHalfSize, y0 - m_HexHalfHeight ),   //2
        wxPoint(x0 + m_HexSize    , y0                   ),   //3
        wxPoint(x0 + m_HexHalfSize, y0 + m_HexHalfHeight ),   //4
        wxPoint(x0 - m_HexHalfSize, y0 + m_HexHalfHeight ),   //5
        wxPoint(x0 - m_HexSize    , y0                   )    //6
    };
    static int Dir[] = {Northwest, North, Northeast,   Southeast,   South,   Southwest};



    for (i=0; i<6; i++)
    {
        IsExit = ((ExitBits&ExitFlags[Dir[i]])>0)?1:0;
        if (IsSelected || !IsExit)
        {
            if ( ExitPen ^ IsExit )
            {
                pDC->SetPen(*pPens[ExitPen]);
                ExitPen ^= 1;
            }
            pDC->DrawLine(point[i].x, point[i].y, point[i+1].x, point[i+1].y);
        }
    }
    if (0==ExitPen)
        pDC->SetPen(*pPens[0]);
}

//--------------------------------------------------------------------------

void CMapPane::DrawHexBorder(int NoX, int NoY, wxDC * pDC, CLand * pLand, BOOL FullBorder, CPlane * pPlane, wxRect * prect, int DrawPhase, BOOL AutoEmpty)
{
    if (!ValidHexNo(NoX, NoY))
        return;

    // draw empty hexes first, then non-empty
    if (pLand)
    {
        if (0==DrawPhase)
            return;
    }
    else
        if (DrawPhase>0)
            return;


    BOOL          IsSelected = (NoX==m_SelHexX && NoY==m_SelHexY);
    wxPen        * pPen;
    wxPen        * Pens[2];
    int           x0, y0;  // hex center
    int           dwx;
    int           ExitBits;
    int           i;


    if ( (m_Detail<EDGE_DETAIL && !IsSelected) ||
         (m_Detail<UNIT_DETAIL)
       )
       return;




    //NormalizeHexX(NoX, pPlane);
    GetHexCenter(NoX, NoY, x0, y0);
    if (IsSelected)
        pPen = m_pPenSel;
    else
        pPen = m_pPen;

    Pens[0] = pPen;
    Pens[1] = m_pPenWall;


    if (pLand)
    {
        ExitBits = pLand->ExitBits;
        //if (ExitBits!=63)
        //{
        //    int x = 0;
        //}
    }
    else
        if (AutoEmpty)
        {
            int  x, y;

            ExitBits = 0;
            for (i=0; i<6; i++)
            {
                x = NoX;
                y = NoY;

                switch (i)
                {
                case North     : y -= 2;     break;
                case Northeast : y--; x++;   break;
                case Southeast : y++; x++;   break;
                case South     : y += 2;     break;
                case Southwest : y++; x--;   break;
                case Northwest : y--; x--;   break;
                }

                pLand = gpApp->m_pAtlantis->GetLand(x, y, m_SelPlane, TRUE);
                if ( pLand && (pLand->Flags&LAND_VISITED) )
                {
                    if (pLand->ExitBits&EntryFlags[i])
                        ExitBits |= ExitFlags[i];
                }
                else
                    ExitBits |= ExitFlags[i]; // assume there is an exit if nothing is known
            }
        }
        else
            ExitBits = 0xFF;

    pDC->SetPen(*pPen);

    if (pPlane && (pPlane->Width>0) )
    {
        dwx = (m_HexSize * 3 / 2) * pPlane->Width;

        while (x0>prect->x-m_HexSize)
            x0 -= dwx;
        x0 += dwx;

        while (x0 < prect->x + prect->width + m_HexSize)
        {
            DrawOneHexBorder(x0, y0, pDC, IsSelected, FullBorder, Pens, ExitBits);
            x0 += dwx;
        }

    }
    else
        DrawOneHexBorder(x0, y0, pDC, IsSelected, FullBorder, Pens, ExitBits);

}

//--------------------------------------------------------------------------
// This will draw a ring of specified radius around the current selection

void CMapPane::DrawRing(wxDC * pDC, wxRect * pRect, CPlane * pPlane)
{
    int dx, dy;
    int DirFlags;
    
    if (m_RingRadius<=0)
        return;
        
////enum       eDirection     { North=0, Northeast,   Southeast,   South,   Southwest,   Northwest };
//int          ExitFlags [] = { 0x01,    0x02,        0x04,        0x08,    0x10,        0x20      };
        
        
    for (dx=-m_RingRadius; dx<=m_RingRadius; dx++)
    {
        if (abs(dx)==m_RingRadius)
            for (dy=-abs(dx); dy<=abs(dx); dy+=2)
            {
                if (dx<0)
                    DirFlags = (1<<Southwest) | (1<<Northwest);
                else
                    DirFlags = (1<<Southeast) | (1<<Northeast);
                if (dy==-abs(dx))    
                    DirFlags |= (1<<North);
                if (dy==abs(dx))    
                    DirFlags |= (1<<South);
                    
                DrawRingSegment(pDC, pRect, pPlane, dx, dy, DirFlags);
            }
        else
        {
            if (dx<0)
                DirFlags = (1<<Southwest) | (1<<South);
            else if (dx==0)
                DirFlags = (1<<Southwest) | (1<<South) | (1<<Southeast);
            else
                DirFlags = (1<<Southeast) | (1<<South);
            dy = m_RingRadius*2 - abs(dx);
            DrawRingSegment(pDC, pRect, pPlane, dx, dy, DirFlags);
            
            if (dx<0)
                DirFlags = (1<<Northwest) | (1<<North);
            else if (dx==0)
                DirFlags = (1<<Northwest) | (1<<North) | (1<<Northeast);
            else
                DirFlags = (1<<Northeast) | (1<<North);
            dy = - dy;
            DrawRingSegment(pDC, pRect, pPlane, dx, dy, DirFlags);
        }
    }
}

//--------------------------------------------------------------------------

void  CMapPane::DrawRingSegment(wxDC * pDC, wxRect * pRect, CPlane * pPlane, int dx, int dy, int DirFlags)
{
    int NoX, NoY;
    int x0, y0;
    int i;
    int x1,y1,x2,y2;
    
    NoX = m_RingX + dx;
    NoY = m_RingY + dy;
    
    GetHexCenter(NoX, NoY, x0, y0);
    
    pDC->SetPen(*m_pPenRing);

    for (i=0; i<6; i++)
        if (DirFlags&(1<<i))
        {
            switch (i)
            {
            case North:
                x1 = x0-m_HexHalfSize;  y1 = y0-m_HexHalfHeight;
                x2 = x0+m_HexHalfSize;  y2 = y0-m_HexHalfHeight;
                break;
            case Northeast:
                x1 = x0+m_HexHalfSize;  y1 = y0-m_HexHalfHeight;
                x2 = x0+m_HexSize;      y2 = y0;
                break;
            case Southeast:
                x1 = x0+m_HexSize;      y1 = y0;
                x2 = x0+m_HexHalfSize;  y2 = y0+m_HexHalfHeight;
                break;
            case South:
                x1 = x0+m_HexHalfSize;  y1 = y0+m_HexHalfHeight;
                x2 = x0-m_HexHalfSize;  y2 = y0+m_HexHalfHeight;
                break;
            case Southwest:
                x1 = x0-m_HexHalfSize;  y1 = y0+m_HexHalfHeight;
                x2 = x0-m_HexSize;      y2 = y0;
                break;
            case Northwest:
                x1 = x0-m_HexSize;      y1 = y0;
                x2 = x0-m_HexHalfSize;  y2 = y0-m_HexHalfHeight;
                break;
            }
            
            if (pPlane && (pPlane->Width>0) )
            {
                int dwx = (m_HexSize * 3 / 2) * pPlane->Width;
        
                while (x1 > pRect->x - m_HexSize)
                {
                    x1 -= dwx;
                    x2 -= dwx;
                }
                //x1 += dwx;
                //x2 += dwx;
        
                while (x1 < pRect->x + pRect->width + m_HexSize)
                {
                    pDC->DrawLine(x1, y1, x2, y2);
                    x1 += dwx;
                    x2 += dwx;
                }
        
            }
            else
                pDC->DrawLine(x1, y1, x2, y2);
        }
        
}

//--------------------------------------------------------------------------

int  CMapPane::Distance(int x1, int y1, int x2, int y2)
{
    int dx, dy, d;
    
    dx = x1-x2;
    if (dx<0)
        dx = -dx;
        
    dy = y1-y2;
    if (dy<0)
        dy = -dy;
        
    if (dy>dx)
        d = (dx+dy)/2;
    else
        d = dx;
    return d;
}

//--------------------------------------------------------------------------

void CMapPane::DrawOneHexWeatherLine(int NoX, int NoY, wxDC * pDC, int DrawBits)
{
    int  i;
    int  x0, y0;
    int  x, y;

    if ((NoX + NoY) % 2 > 0)
        return;

    if (NoX==m_SelHexX && NoY==m_SelHexY) // IsSelected
        return;


    GetHexCenter(NoX, NoY, x0, y0);


    wxPoint point[7] =
    {
        wxPoint(x0 - m_HexSize    , y0                   ),   //0
        wxPoint(x0 - m_HexHalfSize, y0 - m_HexHalfHeight ),   //1
        wxPoint(x0 + m_HexHalfSize, y0 - m_HexHalfHeight ),   //2
        wxPoint(x0 + m_HexSize    , y0                   ),   //3
        wxPoint(x0 + m_HexHalfSize, y0 + m_HexHalfHeight ),   //4
        wxPoint(x0 - m_HexHalfSize, y0 + m_HexHalfHeight ),   //5
        wxPoint(x0 - m_HexSize    , y0                   )    //6
    };
    static int Dir[] = {Northwest, North, Northeast,   Southeast,   South,   Southwest};



    for (i=0; i<6; i++)
    {
        x = NoX;
        y = NoY;

        switch (Dir[i])
        {
        case North     : y -= 2;     break;
        case Northeast : y--; x++;   break;
        case Southeast : y++; x++;   break;
        case South     : y += 2;     break;
        case Southwest : y++; x--;   break;
        case Northwest : y--; x--;   break;
        }

        if ( (DrawBits & ExitFlags[Dir[i]] ) > 0)
        {
            if ( (x==m_SelHexX && y==m_SelHexY)  )
                continue;
            else
                pDC->DrawLine(point[i].x, point[i].y, point[i+1].x, point[i+1].y);
        }
    }
}

//--------------------------------------------------------------------------

void CMapPane::DrawCitiesAndWeather(wxDC * pDC, wxRect * pRect, CPlane * pPlane)
{
    int               i;
    int               wx, wy;
    int               nx, ny, nz;
    CLand           * pLand;
    int               dwx;
    wxCoord           w, h, descent, ext;
    int               nx1, ny1;
    int               nx2, ny2, width, height;

    pDC->SetPen (*m_pPen);
    pDC->SetFont(*gpApp->m_Fonts[FONT_MAP_TEXT]);
    pDC->SetBackgroundMode(wxTRANSPARENT);

    for (i=0; i<m_pCities->Count(); i++)
    {
        pLand = (CLand*)m_pCities->At(i);
        LandIdToCoord(pLand->Id, nx, ny, nz);
        GetHexCenter(nx, ny, wx, wy);
        pDC->GetTextExtent(pLand->CityName.GetData(), &w, &h, &descent, &ext);

        if (pPlane && (pPlane->Width>0) )
        {
            dwx = (m_HexSize * 3 / 2) * pPlane->Width;

            while (wx > pRect->x - m_HexSize)
                wx -= dwx;
            wx += dwx;
            while (wx < pRect->x + pRect->width + m_HexSize)
            {
                pDC->DrawText(pLand->CityName.GetData(), wx-w/2, wy-ext);
                wx += dwx;
            }
        }
        else
            pDC->DrawText(pLand->CityName.GetData(), wx-w/2, wy-ext);
    }

    // now for the weather
    if (m_Detail >= WEATHER_DETAIL && pPlane && pPlane->TropicZoneMin <= pPlane->TropicZoneMax)
    {
        pDC->SetPen (*m_pPenTropic);

        // get raw hex numbers
        GetClientSize(&width, &height);
        GetHexNo(nx1, ny1, 0    , 0   );
        GetHexNo(nx2, ny2, width, height);

        if (pPlane && (pPlane->Width>0) && (nx2-nx1+1 > pPlane->Width) )
        {
            nx1 = pPlane->WestEdge;
            nx2 = pPlane->EastEdge;
        }
        else
        {
            nx1--;
            nx2++;
        }

        nx = nx1-1;
        while (++nx <= nx2)
        {
            if (ny1 <= pPlane->TropicZoneMin+1 && ny2 >= pPlane->TropicZoneMin-1)
            {
                DrawOneHexWeatherLine(nx, pPlane->TropicZoneMin  , pDC, Flags_NW_N_NE);
                DrawOneHexWeatherLine(nx, pPlane->TropicZoneMin+1, pDC, Flags_N      );
            }

            if (ny1 <= pPlane->TropicZoneMax+1 && ny2 >= pPlane->TropicZoneMax-1)
            {
                DrawOneHexWeatherLine(nx, pPlane->TropicZoneMax  , pDC, Flags_SW_S_SE);
                DrawOneHexWeatherLine(nx, pPlane->TropicZoneMax-1, pDC, Flags_S      );
            }
        }
    }

}

//--------------------------------------------------------------------------

void CMapPane::NormalizeCoordinates(CPlane * pPlane, wxRect * pRect)
{
    int               dnx;
    int               wx;
    int               nx, ny;

    if (pPlane && (pPlane->Width>0))
    {
        dnx   = 0;

        wx    = pRect->x + pRect->width/2;
        GetHexNo(nx, ny, wx, pRect->y);
        if (nx > pPlane->WestEdge)
            dnx = -((nx - pPlane->WestEdge)/pPlane->Width)*pPlane->Width;
        else
            if (nx < pPlane->EastEdge)
                dnx = ((pPlane->EastEdge - nx)/pPlane->Width)*pPlane->Width;

        m_AtlaX0 += dnx*(m_HexSize * 3 / 2);
    }

}

//--------------------------------------------------------------------------

void CMapPane::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    int               width, height;

    GetClientSize(&width, &height);

    wxPaintDC         dc(this); // device context for painting
    wxRect            rect(0, 0, width, height);
    int               nx1, ny1;
    int               nx2, ny2;
    int               nx, ny, nz;
    int               i;
    int               wx, wy;
    CPlane          * pPlane;
    CLand           * pLand;
    CUnit           * pUnit;
    wxBrush         * pbrush;

    m_pCities->DeleteAll();
    dc.SetFont (*gpApp->m_Fonts[FONT_MAP_TEXT]);

    if (m_ShowState & SHOW_COORD)
    {
        width  -= COORD_PANE_WIDTH;
        height -= COORD_PANE_HEIGHT;
        dc.SetClippingRegion(0,0,width,height);
    }

    pPlane   = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);

    if (m_Detail >= ROAD_DETAIL) // big hexes
    {
        // get raw hex numbers
        GetHexNo(nx1, ny1, 0    , 0   );
        GetHexNo(nx2, ny2, width, height);

        if (pPlane && (pPlane->Width>0) && (nx2-nx1+1 > pPlane->Width) )
        {
            nx1 = pPlane->WestEdge;
            nx2 = pPlane->EastEdge;
        }
        else
        {
            nx1--;
            nx2++;
        }

        nx = nx1-1;
        while (++nx <= nx2)
            for (ny=ny1-1; ny<=ny2+1; ny++)
            {
                pLand = gpApp->m_pAtlantis->GetLand(nx, ny, m_SelPlane, TRUE);
                if ((m_ShowState & SHOW_NAMES)
                       &&(m_Detail>=NAME_DETAIL)&&(pLand && !pLand->CityName.IsEmpty()))
                    m_pCities->Insert(pLand);
                DrawHex(nx, ny, &dc, pLand, pPlane, &rect);
            }

    /*
        for (dp=0; dp<=MAX_DP_BORDER; dp++)
        {
        */
            nx = nx1-1;
            while (++nx <= nx2)
                for (ny=ny1-1; ny<=ny2+1; ny++)
                {
                    pLand = gpApp->m_pAtlantis->GetLand(nx, ny, m_SelPlane, TRUE);
                    DrawHexBorder(nx, ny, &dc, pLand, FALSE, pPlane, &rect, MAX_DP_BORDER /*dp*/, FALSE);
                }
    /*
        }
        */
    }
    else // small hexes
    {
        pbrush = GetLandBrush(NULL, FALSE);
        dc.SetBrush(*pbrush);
        dc.SetPen  (*wxTRANSPARENT_PEN);
        dc.DrawRectangle(0, 0, width, height);
        if (pPlane)
            for (i=0; i<pPlane->Lands.Count(); i++)
            {
                pLand = (CLand*)pPlane->Lands.At(i);
                LandIdToCoord(pLand->Id, nx, ny, nz);

                GetHexCenter(nx, ny, wx, wy);
                DrawHex(nx, ny, &dc, pLand, pPlane, &rect);

            }
    }

    pLand = gpApp->m_pAtlantis->GetLand(m_SelHexX, m_SelHexY, m_SelPlane, TRUE);
    DrawEdge(&dc, pPlane);
    DrawHexBorder(m_SelHexX, m_SelHexY, &dc, pLand, TRUE, pPlane, &rect, pLand?MAX_DP_BORDER:0, FALSE);

    DrawRing(&dc, &rect, pPlane);

    pUnit = gpApp->GetSelectedUnit();
    RedrawTracksForUnit(pPlane, pUnit, &dc, FALSE);

    DrawCitiesAndWeather(&dc, &rect, pPlane);
    PaintRectangle(&dc);

    // draw coord panes
    if (m_ShowState & SHOW_COORD)
    {
        dc.DestroyClippingRegion();
        DrawCoordPanes(&dc, width, height, pPlane);
    }

}

//--------------------------------------------------------------------------

void CMapPane::DrawCoordPanes(wxDC * pDC, int mapwidth, int mapheight, CPlane * pPlane)
{
    wxCoord   w, h, descent, ext;
    wxCoord   txtx, txty;
    int       x, y, nx, ny ;
    wxBrush * pBrush = GetLandBrush(NULL, FALSE);
    char      buf[32];


    int left  =  5;
    int right =  mapwidth-5;
    int top   =  5;
    int bottom=  mapheight-5;

    pDC->SetBrush(*pBrush);
    pDC->SetPen  (*wxTRANSPARENT_PEN);
    pDC->SetFont (*gpApp->m_Fonts[FONT_MAP_COORD]);
    pDC->SetBackgroundMode(wxTRANSPARENT);

    pDC->DrawRectangle(0       , mapheight, mapwidth+COORD_PANE_WIDTH, COORD_PANE_HEIGHT);
    pDC->DrawRectangle(mapwidth, 0        ,          COORD_PANE_WIDTH, mapheight        );


    GetHexNo(nx, ny, left, top);

    // horizontal pane
    pDC->GetTextExtent("8", &w, &h, &descent, &ext);
    txty = mapheight + (COORD_PANE_HEIGHT-h)/2;
    nx--;
    while (TRUE)
    {
        nx++;
        GetHexCenter(nx, ny, x, y);
        sprintf(buf, "%d", NormalizeHexX(nx, pPlane));
        pDC->GetTextExtent(buf, &w, &h, &descent, &ext);
        txtx = x - w/2;
        if (txtx < left)
            continue;
        if (x + w/2 >= right)
            break;
        pDC->DrawText(buf, txtx, txty);
        left = x + w/2 + 5;
    }

    // vertical pane
    ny--;
    while (TRUE)
    {
        ny++;
        GetHexCenter(nx, ny, x, y);
        sprintf(buf, "%d", ny);
        pDC->GetTextExtent(buf, &w, &h, &descent, &ext);
        txtx = mapwidth + (COORD_PANE_WIDTH-w)/2;
        txty = y - h/2;
        if (txty < top)
            continue;
        if (y + h/2 >= bottom)
            break;
        pDC->DrawText(buf, txtx, txty);
        top = y + h/2 + 10;
    }

}

//--------------------------------------------------------------------------

/*
void CMapPane::SelectHex(int NoX, int NoY)
{
    int              wx, wy;
    int              width, height;

    if (NoX!=m_SelHexX || NoY!=m_SelHexY)
    {
        m_SelHexX = NoX;
        m_SelHexY = NoY;

        GetClientSize(&width, &height);
        GetHexCenter(NoX, NoY, wx, wy);

        if (wx-m_HexSize < 0)
            m_AtlaX0 -= (0 - (wx-m_HexSize) + m_HexSize);
        else
            if ((wx+m_HexSize > width))
                m_AtlaX0 += (wx+m_HexSize - width + m_HexSize);


        if (wy-m_HexHalfHeight < 0)
            m_AtlaY0 -= (0  - (wy-m_HexHalfHeight) + m_HexHalfHeight);
        else
            if ((wy+m_HexHalfHeight > height))
                m_AtlaY0 += (wy+m_HexHalfHeight - height + m_HexHalfHeight);

        //UpdateEditPane();
        gpApp->OnMapSelectionChange();
    }
}
*/


//--------------------------------------------------------------------------

void CMapPane::RedrawTracksForUnit(CPlane * pPlane, CUnit * pUnit, wxDC * pDC, BOOL DoDrawCitiesAndWeather)
{
    int            i, X, Y, Z;
    int            wx0, wy0, dx;
    long           HexId;
    BOOL           CloseDC = FALSE;
    CLand        * pLand;
    wxRect         rect;
    int            copyno=0;
    int            dp;

    if (!pPlane)
        return;

    if (NULL==pDC)
    {
        CloseDC = TRUE;
        pDC = new wxClientDC(this);
    }
    GetClientSize(&rect.width, &rect.height);
    if (CloseDC && (m_ShowState & SHOW_COORD))
    {
        rect.width  -= COORD_PANE_WIDTH;
        rect.height -= COORD_PANE_HEIGHT;
        pDC->SetClippingRegion(0,0,rect.width,rect.height);
    }


    NormalizeCoordinates(pPlane, &rect);
    pDC->SetFont (*gpApp->m_Fonts[FONT_MAP_TEXT]);



    // Erase old tracks
    for (i=0; i<m_pTrackHexes->Count(); i++)
    {
        HexId = (long)m_pTrackHexes->At(i);
        LandIdToCoord(HexId, X, Y, Z);
        pLand = gpApp->m_pAtlantis->GetLand(HexId);

        DrawHex(X, Y, pDC, pLand, pPlane, &rect);
    }
    for (dp=0; dp<=MAX_DP_BORDER; dp++)
        for (i=0; i<m_pTrackHexes->Count(); i++)
        {
            HexId = (long)m_pTrackHexes->At(i);
            LandIdToCoord(HexId, X, Y, Z);
            pLand = gpApp->m_pAtlantis->GetLand(HexId);

            DrawHexBorder(X, Y, pDC, pLand, TRUE, pPlane, &rect, dp, TRUE);
        }
    m_pTrackHexes->DeleteAll();

    pLand = gpApp->m_pAtlantis->GetLand(m_SelHexX, m_SelHexY, m_SelPlane, TRUE);
    DrawHexBorder(m_SelHexX, m_SelHexY, pDC, pLand, TRUE, pPlane, &rect, pLand?MAX_DP_BORDER:0, TRUE);


    pDC->SetPen(*m_pPenSel);

    // draw new tracks and remeber hexes
    if (pUnit && pUnit->pMovement)
    {
        m_pTrackHexes->Insert((void*)pUnit->LandId);
        LandIdToCoord(pUnit->LandId, X, Y, Z);
        GetHexCenter(X, Y, wx0, wy0);

        if (pPlane->Width>0)
        {
            dx = (m_HexSize * 3 / 2) * pPlane->Width;
            while (wx0-m_HexSize >= rect.x)
                wx0 -= dx;
            wx0+=dx;
            while (wx0+m_HexSize <= rect.x + rect.width)
            {
                DrawSingleTrack(X, Y, wx0, wy0, pDC, pUnit, pPlane, copyno++);
                wx0 += dx;
            }
        }
        else
            DrawSingleTrack(X, Y, wx0, wy0, pDC, pUnit, pPlane, copyno);
    }

    if (DoDrawCitiesAndWeather)
        DrawCitiesAndWeather(pDC, &rect, pPlane);

    if (CloseDC)
        delete pDC;
}

//--------------------------------------------------------------------------

void CMapPane::DrawSingleTrack(int X, int Y, int wx, int wy, wxDC * pDC, CUnit * pUnit, CPlane * pPlane,int copyno)
{
    int            i, X1, Y1, Z;
    long           HexId;
    int            wx0, wy0;
    int            wx_a, wy_a, wx0_a, wy0_a;
    BOOL           Arcadia3Sail = FALSE;
    CStruct      * pStruct;
    CLand        * pLand;
    EValueType     type;
    long           n1;
    int            LocA3;

    if (pUnit->pMovement && pUnit->pMoveA3Points && pUnit->pMoveA3Points->Count() == pUnit->pMovement->Count())
        Arcadia3Sail = TRUE;

    if (Arcadia3Sail && pUnit->GetProperty(PRP_STRUCT_ID, type, (const void *&)n1) && eLong==type)
    {
        pLand = gpApp->m_pAtlantis->GetLand(pUnit->LandId);
        if (pLand)
        {
            pStruct  = pLand->GetStructById(n1);
            if (pStruct && NO_LOCATION != pStruct->Location)
            {
                wx_a = wx; wy_a = wy;
                AdjustForA3Location(wx_a, wy_a, pStruct->Location);
            }
        }
    }

    for (i=0; i<pUnit->pMovement->Count(); i++)
    {
        wx0 = wx;
        wy0 = wy;

        wx0_a = wx_a;
        wy0_a = wy_a;

        HexId = (long)pUnit->pMovement->At(i);
        if (0==copyno)
            m_pTrackHexes->Insert((void*)HexId);

        LandIdToCoord(HexId, X1, Y1, Z);

        if (X1-X > 1)
            X1 = X-1;
        if (X-X1 > 1)
            X1 = X+1;

        wx += (m_HexSize * 3 / 2)*(X1-X);
        wy += m_HexHalfHeight*(Y1-Y);

        if (Arcadia3Sail)
        {
            LocA3 = (long)pUnit->pMoveA3Points->At(i);
            wx_a = wx; wy_a = wy;
            AdjustForA3Location(wx_a, wy_a, LocA3);
        }

        if (pPlane->Width>0)
        {
            if (X1>pPlane->EastEdge)
                X1 = pPlane->WestEdge;
            if (X1<pPlane->WestEdge)
                X1 = pPlane->EastEdge;
        }
        X = X1;
        Y = Y1;

        if (Arcadia3Sail)
        {
            pDC->DrawLine(wx0_a, wy0_a, wx_a, wy_a);
            if (i == pUnit->pMovement->Count()-1)
                DrawTrackArrow(pDC, wx0_a, wy0_a, wx_a, wy_a);
        }
        else
        {
            pDC->DrawLine(wx0, wy0, wx, wy);
            if (i == pUnit->pMovement->Count()-1)
                DrawTrackArrow(pDC, wx0, wy0, wx, wy);
        }
    }
}

//--------------------------------------------------------------------------

void CMapPane::DrawTrackArrow(wxDC * pDC, int wx0, int wy0, int wx, int wy)
{
    int wxa, wya, wx1, wy1;
    int scale = 4;

    wxa = wx - (wx-wx0)/scale;
    wya = wy - (wy-wy0)/scale;

    wx1 = wxa + (wy-wya);
    wy1 = wya - (wx-wxa);
    pDC->DrawLine(wx, wy, wx1, wy1);

    wx1 = wxa - (wy-wya);
    wy1 = wya + (wx-wxa);
    pDC->DrawLine(wx, wy, wx1, wy1);

}

//--------------------------------------------------------------------------

void CMapPane::AdjustForA3Location(int & wx, int & wy, int Location)
{
    int wx1, wy1;

    switch (Location)
    {
    case North:
        wx1 = wx;
        wy1 = wy - m_HexHalfHeight;
        break;
    case Northeast:
        wx1 = wx + (int)(m_HexHalfHeight *  + 0.5);
        wy1 = wy - m_HexHalfHeight/2;
        break;
    case Southeast:
        wx1 = wx + (int)(m_HexHalfHeight * cos30 + 0.5);
        wy1 = wy + m_HexHalfHeight/2;
        break;
    case South:
        wx1 = wx;
        wy1 = wy + m_HexHalfHeight;
        break;
    case Southwest:
        wx1 = wx - (int)(m_HexHalfHeight * cos30 + 0.5);
        wy1 = wy + m_HexHalfHeight/2;
        break;
    case Northwest:
        wx1 = wx - (int)(m_HexHalfHeight * cos30 + 0.5);
        wy1 = wy - m_HexHalfHeight/2;
        break;
    default:
        return;
    }

    wx = (3*wx1 + wx)/4;
    wy = (3*wy1 + wy)/4;
}


//--------------------------------------------------------------------------

void CMapPane::Center()
{
    CPlane  * pPlane;
    CLand   * pLand;
    long       sx = 0;
    long       sy = 0;
    int       i;
    int       x, y, z;
    long      ax, ay;
    int       width, height;

    pPlane = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);
    if (pPlane)
    {
        for (i=0; i<pPlane->Lands.Count(); i++)
        {
            pLand = (CLand*)pPlane->Lands.At(i);
            LandIdToCoord(pLand->Id, x, y, z);
            sx += x;
            sy += y;
        }
        x = sx/pPlane->Lands.Count();
        y = sy/pPlane->Lands.Count();

        if (!ValidHexNo(x,y))
            y++;

        GetClientSize(&width, &height);

        ax = x * m_HexSize * 3 / 2;
        ay = y * m_HexHalfHeight;

        m_AtlaX0 = ax - width/2;
        m_AtlaY0 = ay - height/2;
    }
}

//--------------------------------------------------------------------------

BOOL CMapPane::EnsureLandVisible(int nx, int ny, int nz, BOOL Anyway)
{
    BOOL NeedRefresh     = FALSE;
    int  wx, wy;
    int  width, height;


    if (m_SelPlane!=nz)
    {
        SavePlaneConfig();
        m_SelPlane=nz;
        LoadPlaneConfig();
        NeedRefresh=TRUE;
    }

    if (NeedRefresh || (m_SelHexX!=nx) || (m_SelHexY!=ny) || Anyway)
    {
        GetClientSize(&width, &height);
        if (m_ShowState & SHOW_COORD)
        {
            width  -= COORD_PANE_WIDTH;
            height -= COORD_PANE_HEIGHT;
        }
        GetHexCenter(nx, ny, wx, wy);

        if (wx-m_HexSize < 0)
        {
            m_AtlaX0 -= (0 - (wx-m_HexSize) + m_HexSize);
            NeedRefresh = TRUE;
        }
        else
            if ((wx+m_HexSize > width))
            {
                m_AtlaX0 += (wx+m_HexSize - width + m_HexSize);
                NeedRefresh = TRUE;
            }

        if (wy-m_HexHalfHeight < 0)
        {
            m_AtlaY0 -= (0  - (wy-m_HexHalfHeight) + m_HexHalfHeight);
            NeedRefresh = TRUE;
        }
        else
            if ((wy+m_HexHalfHeight > height))
            {
                m_AtlaY0 += (wy+m_HexHalfHeight - height + m_HexHalfHeight);
                NeedRefresh = TRUE;
            }
    }
    return NeedRefresh;
}

//--------------------------------------------------------------------------

void CMapPane::SetSelection(int nx, int ny, CUnit * pUnit, CPlane * pPlane, BOOL Anyway)
{
    int           oldx, oldy;
    int           x, y;
    CLand       * pLand;
    wxClientDC    dc(this);

    wxRect        rect;
    int           dp = MAX_DP_BORDER;

    GetClientSize(&rect.width, &rect.height);
    if (m_ShowState & SHOW_COORD)
    {
        rect.width  -= COORD_PANE_WIDTH;
        rect.height -= COORD_PANE_HEIGHT;
        dc.SetClippingRegion(0,0,rect.width,rect.height);
    }
    NormalizeCoordinates(pPlane, &rect);

    if (nx!=m_SelHexX || ny!=m_SelHexY || Anyway)
    {
        oldx = m_SelHexX;
        oldy = m_SelHexY;
        m_SelHexX = nx;
        m_SelHexY = ny;
        dc.SetFont (*gpApp->m_Fonts[FONT_MAP_TEXT]);

        for (x=oldx-1; x<=oldx+1; x++)
            for (y=oldy-2; y<=oldy+2; y++)
            {
                pLand = gpApp->m_pAtlantis->GetLand(x, y, m_SelPlane, TRUE);
                DrawHex      (x, y, &dc, pLand, pPlane, &rect);
            }
        for (dp=0; dp<=MAX_DP_BORDER; dp++)
            for (x=oldx-1; x<=oldx+1; x++)
                for (y=oldy-2; y<=oldy+2; y++)
                {
                    pLand = gpApp->m_pAtlantis->GetLand(x, y, m_SelPlane, TRUE);
                    DrawHexBorder(x, y, &dc, pLand, TRUE,  pPlane, &rect, dp, TRUE);
                }

        pLand = gpApp->m_pAtlantis->GetLand(nx, ny, m_SelPlane, TRUE);
        DrawHex      (nx, ny, &dc, pLand, pPlane, &rect);
        DrawHexBorder(nx, ny, &dc, pLand, TRUE,  pPlane, &rect, pLand?MAX_DP_BORDER:0, TRUE);
        DrawEdge(&dc, pPlane);

        RedrawTracksForUnit(pPlane, pUnit, &dc, FALSE);
        DrawCitiesAndWeather(&dc, &rect, pPlane);
        gpApp->OnMapSelectionChange();
        
        DrawRing(&dc, &rect, pPlane);
    }

}

//--------------------------------------------------------------------------

void CMapPane::RemoveRectangle()
{
    m_Rect_IsPaused    = false;
    m_Rect_IsDragging  = false;
    m_Rect_x1          = 0;
    m_Rect_y1          = 0;
    m_Rect_x2          = 0;
    m_Rect_y2          = 0;
}

//--------------------------------------------------------------------------

void CMapPane::StartRectangle(int x, int y)
{
    wxClientDC    dc(this);

    PaintRectangle(&dc);

    m_Rect_IsPaused    = false;
    m_Rect_IsDragging  = true;
    m_Rect_x1          = x;
    m_Rect_y1          = y;
    m_Rect_x2          = x;
    m_Rect_y2          = y;
}

//--------------------------------------------------------------------------

void CMapPane::EndRectangle  (int x, int y)
{
    if (m_Rect_IsDragging)
    {
        m_Rect_IsDragging  = false;
        m_Rect_IsPaused    = false;
        m_Rect_x2          = x;
        m_Rect_y2          = y;
    }
}

//--------------------------------------------------------------------------

void CMapPane::MoveRectangle (int x, int y)
{
    if (m_Rect_IsDragging && !m_Rect_IsPaused )
    {
        wxClientDC    dc(this);

        PaintRectangle(&dc);
        m_Rect_x2          = x;
        m_Rect_y2          = y;
        PaintRectangle(&dc);
    }
}

//--------------------------------------------------------------------------

void CMapPane::PauseRectangle()
{
    if (m_Rect_IsDragging)
        m_Rect_IsPaused = true;
}

//--------------------------------------------------------------------------

void CMapPane::ResumeRectangle()
{
    if (m_Rect_IsDragging)
        m_Rect_IsPaused = false;
}

//--------------------------------------------------------------------------

bool CMapPane::ShouldDrawRectangle()
{
    // TBD: write some more decent code checking if there is a hex center inside the rect
//    return
//        m_Rect_x1 - m_Rect_x2 > 10 && m_Rect_y1 - m_Rect_y2 > 10  ||
//        m_Rect_x1 - m_Rect_x2 > 10 && m_Rect_y1 - m_Rect_y2 < 10  ||
//        m_Rect_x1 - m_Rect_x2 < 10 && m_Rect_y1 - m_Rect_y2 > 10  ||
//        m_Rect_x1 - m_Rect_x2 < 10 && m_Rect_y1 - m_Rect_y2 < 10  ;

    int nx1, ny1, nx2, ny2;

    GetHexNo(nx1, ny1, m_Rect_x1, m_Rect_y1);
    GetHexNo(nx2, ny2, m_Rect_x2, m_Rect_y2);

    return (nx1!=nx2 || ny1!=ny2);

}

//--------------------------------------------------------------------------

void CMapPane::PaintRectangle(wxDC * pDC)
{
    if (ShouldDrawRectangle())
    {
        int     OldFunc = pDC->GetLogicalFunction();
        wxPen   OldPen  = pDC->GetPen();
        pDC->SetLogicalFunction(wxXOR);

        pDC->SetPen  (*m_pPenSel);//m_pPenRed);

        pDC->DrawLine(m_Rect_x1, m_Rect_y1, m_Rect_x2, m_Rect_y1);
        pDC->DrawLine(m_Rect_x2, m_Rect_y1, m_Rect_x2, m_Rect_y2);
        pDC->DrawLine(m_Rect_x2, m_Rect_y2, m_Rect_x1, m_Rect_y2);
        pDC->DrawLine(m_Rect_x1, m_Rect_y2, m_Rect_x1, m_Rect_y1);


        // now mark all selected hexes
        CPlane * pPlane = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);
        CLand  * pLand;
        int nl;
        for (nl=0; nl<pPlane->Lands.Count(); nl++)
        {
            pLand = (CLand*)pPlane->Lands.At(nl);
            if (HexIsSelected(pLand))
            {
                int x,y,z;
                int wx,wy;
                LandIdToCoord(pLand->Id, x,y,z);
                GetHexCenter(x,y, wx, wy);
                pDC->DrawLine(wx-m_HexHalfSize, wy-m_HexHalfHeight, wx+m_HexHalfSize, wy+m_HexHalfHeight);
                pDC->DrawLine(wx-m_HexHalfSize, wy+m_HexHalfHeight, wx+m_HexHalfSize, wy-m_HexHalfHeight);
            }
        }


        pDC->SetPen(OldPen);
        pDC->SetLogicalFunction(OldFunc);
    }
}

//--------------------------------------------------------------------------

/*
BOOL CMapPane::GetSelectedHexes(int & x1, int & y1, int & x2, int & y2 )
{
    int       xx;
    BOOL      ToTheRight;  // drawn from left to right
    BOOL      EdgeCrossed;
    CPlane  * pPlane = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);

    if (m_Rect_x1==m_Rect_x2 || m_Rect_y1==m_Rect_y2)
        return FALSE;   // no selection

    GetHexNo(x1, y1, m_Rect_x1, m_Rect_y1);
    GetHexNo(x2, y2, m_Rect_x2, m_Rect_y2);

    x1 = NormalizeHexX(x1, pPlane);
    x2 = NormalizeHexX(x2, pPlane);

    ToTheRight = m_Rect_x2 > m_Rect_x1;
    if (!ToTheRight)
    {
        xx = x1; x1 = x2; x2 = xx; // normalize the rect
    }

    EdgeCrossed = x1>x2;

    if (y1 > y2)
    {
        xx = y1; y1 = y2; y2 = xx;
    }

    return TRUE;
}
*/

//--------------------------------------------------------------------------

BOOL CMapPane::HexIsSelected(CLand * pLand)
{
    int x,y,z;
    int wx,wy;

    if (!pLand)
        return FALSE;

    if (0==(pLand->Flags&LAND_VISITED))
        return FALSE; // non-visited hexes does not have any value

    if (m_Rect_x1==m_Rect_x2 || m_Rect_y1==m_Rect_y2)
        return FALSE;   // no selection

    CPlane * pPlaneCrnt = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);
    if (pPlaneCrnt != pLand->pPlane)
        return FALSE;

    LandIdToCoord(pLand->Id, x,y,z);
    GetHexCenter(x,y, wx, wy);
    if (
        (m_Rect_x1 < m_Rect_x2 && wx > m_Rect_x1 && wx < m_Rect_x2 ||
         m_Rect_x1 > m_Rect_x2 && wx < m_Rect_x1 && wx > m_Rect_x2)   &&
        (m_Rect_y1 < m_Rect_y2 && wy > m_Rect_y1 && wy < m_Rect_y2 ||
         m_Rect_y1 > m_Rect_y2 && wy < m_Rect_y1 && wy > m_Rect_y2)
         )
        return TRUE;
    else
        return FALSE;
}

//--------------------------------------------------------------------------

void CMapPane::GetSelectedOrAllHexes(CBaseColl & Hexes, BOOL Selected)
{
    int      np,nl;
    CPlane * pPlane;
    CLand  * pLand;

    Hexes.DeleteAll(); //must be empty, anyway
    if (Selected && (m_Rect_x1==m_Rect_x2 || m_Rect_y1==m_Rect_y2))
        return; // none selected

    for (np=0; np<gpApp->m_pAtlantis->m_Planes.Count(); np++)
    {
        pPlane = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(np);
        if (Selected && m_SelPlane != np)
            continue;

        for (nl=0; nl<pPlane->Lands.Count(); nl++)
        {
            pLand = (CLand*)pPlane->Lands.At(nl);
            if (Selected && !HexIsSelected(pLand))
                continue;
            //{
            //    LandIdToCoord(pLand->Id, x,y,z);
            //    if (
            //            (x1<=x2  && (x<x1 || x>x2)) ||
            //            (x1> x2  && (x<x1 && x>x2)) ||
            //            (y<y1 || y>y2)
            //        )
            //            continue;
            //}
            Hexes.Insert(pLand);
        }
    }
}

//--------------------------------------------------------------------------

BOOL CMapPane::HaveSelection()
{
    return (m_Rect_x1!=m_Rect_x2 && m_Rect_y1!=m_Rect_y2);
}

//--------------------------------------------------------------------------

void CMapPane::OnMouseEvent(wxMouseEvent& event)
{
    wxCoord       xpos, ypos;
    int           nx, ny;

    event.GetPosition(&xpos, &ypos);

    if (event.LeftDown())
    {
        CPlane * pPlane = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);

        StartRectangle(xpos, ypos);
        GetHexNo(nx, ny, xpos, ypos);
        nx = NormalizeHexX(nx, pPlane);
        SetSelection(nx, ny, gpApp->GetSelectedUnit(), pPlane, FALSE);
    }

    else if (event.LeftUp())
        EndRectangle(xpos, ypos);

    else if (event.Dragging())
        MoveRectangle(xpos, ypos);

    else if (event.Leaving())
        PauseRectangle();

    else if (event.Entering())
        ResumeRectangle();

    else if ( (event.RightDown()  &&  atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_RCLICK_CENTERS))) ||
              (event.LeftDClick() && !atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_RCLICK_CENTERS))) )
    {
        // center
        wxPoint  point(xpos, ypos);

        CenterClick(point);
    }

    else if ( (event.RightDown()  && !atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_RCLICK_CENTERS))) ||
              (event.LeftDClick() &&  atol(gpApp->GetConfig(SZ_SECT_COMMON, SZ_KEY_RCLICK_CENTERS))) )
    {
        // popup menu

        wxMenu  menu;
        int     nx, ny;

        m_PopupPoint.x = xpos;
        m_PopupPoint.y = ypos;

        GetHexNo(nx, ny, xpos, ypos);
        m_pPopupLand   = gpApp->m_pAtlantis->GetLand(nx, ny, m_SelPlane, TRUE);

        if (m_pPopupLand)
        {
            menu.Append(menu_Popup_Flag, "Flags");
            if (m_pPopupLand->Flags&LAND_BATTLE)
                menu.Append(menu_Popup_Battles, "Battles");
            menu.Append(menu_Popup_Financial   , "Financial details for the hex");
            
            if (0==strcmp(m_pPopupLand->Name.GetData(), SZ_MANUAL_HEX_PROVINCE))
                menu.Append(menu_Popup_Del_Hex   , "Remove Hex terrain");
        }
        else
        {
            menu.Append(menu_Popup_New_Hex   , "Set Hex terrain");
        }

        menu.Append(menu_Popup_Center      , "Center");
        menu.Append(menu_Popup_WhoMovesHere, "Find units moving here");
        menu.Append(menu_Popup_DistanceRing, "Distance ring");
        
        



        PopupMenu( &menu, xpos, ypos);
    }
    else
    {
        //That must be the mouse move event
    }
}

//--------------------------------------------------------------------------

void CMapPane::CenterClick(wxPoint point)
{
    wxRect        rect;
    CPlane      * pPlane;

    GetClientSize(&rect.width, &rect.height);
    pPlane   = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);

    m_AtlaX0 += (point.x - rect.width/2);
    m_AtlaY0 += (point.y - rect.height/2);

    RemoveRectangle();
    NormalizeCoordinates(pPlane, &rect);
    Refresh(FALSE, NULL);
}

//--------------------------------------------------------------------------

void CMapPane::MarkFoundHexes(CHexFilterDlg * pFilter)
{
    CStr             Property [HEX_SIMPLE_FLTR_COUNT];
    CStr             Compare  [HEX_SIMPLE_FLTR_COUNT];
    CStr             sValue   [HEX_SIMPLE_FLTR_COUNT];
    long             lValue   [HEX_SIMPLE_FLTR_COUNT];
    eCompareOp       CompareOp[HEX_SIMPLE_FLTR_COUNT];
    int              i,k,n;
    CLand          * pLand;
    CPlane         * pPlane;
//    CStruct        * pStruct;
    CStr             LandList(64), sCoord(32), Msg;

    if (!pFilter)
        return;

    // read boxes values
    for (i=0; i<HEX_SIMPLE_FLTR_COUNT; i++)
    {
        Property[i] = pFilter->m_cbProperty[i]->GetValue();
        Compare [i] = pFilter->m_cbCompare [i]->GetValue();
        sValue  [i] = pFilter->m_tcValue   [i]->GetValue();

        Property[i].TrimRight(TRIM_ALL);    Property[i].TrimLeft(TRIM_ALL);
        Compare [i].TrimRight(TRIM_ALL);    Compare [i].TrimLeft(TRIM_ALL);
        sValue  [i].TrimRight(TRIM_ALL);    sValue  [i].TrimLeft(TRIM_ALL);

        CompareOp[i] = NOP;
        for (k=GT; k<NOP; k++)
            if (0==stricmp(HEX_FILTER_OPERATION[k], Compare[i].GetData()))
            {
                CompareOp[i] = (eCompareOp)k;
                break;
            }
        lValue[i] = atol(sValue[i].GetData());
    }


    for (n=0; n<gpApp->m_pAtlantis->m_Planes.Count(); n++)
    {
        pPlane = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(n);
        for (i=0; i<pPlane->Lands.Count(); i++)
        {
            pLand = (CLand*)pPlane->Lands.At(i);
            if ( EvaluateBaseObjectByBoxes(pLand, Property, CompareOp, sValue, lValue, HEX_SIMPLE_FLTR_COUNT))
            {
                gpApp->m_pAtlantis->ComposeLandStrCoord(pLand, sCoord);
                LandList << pLand->TerrainType << " (" << sCoord << ")" << EOL_SCR;
                pLand->Flags |= LAND_LOCATED_LAND;
            }
        }
    }

    if (LandList.IsEmpty())
        wxMessageBox("None found");
    else
    {
        Msg << "=====================" << EOL_SCR << LandList;
        gpApp->ShowError(Msg.GetData(), Msg.GetLength(), TRUE);
    }
}

//--------------------------------------------------------------------------

void CMapPane::UnMarkFoundHexes()
{
    int              i,n;
    CLand          * pLand;
    CPlane         * pPlane;

    for (n=0; n<gpApp->m_pAtlantis->m_Planes.Count(); n++)
    {
        pPlane = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(n);
        for (i=0; i<pPlane->Lands.Count(); i++)
        {
            pLand = (CLand*)pPlane->Lands.At(i);
            pLand->Flags &= ~LAND_LOCATED_LAND;
        }
    }
}

//--------------------------------------------------------------------------

void CMapPane::FindHexes()
{
    CHexFilterDlg dlg(gpApp->m_Panes[AH_PANE_MAP], SZ_SECT_WND_HEX_FLTR);

    UnMarkFoundHexes(); // yes, we need a way to just clean the old marks!
    if (wxID_OK == dlg.ShowModal())
        MarkFoundHexes(&dlg);
    Refresh(FALSE);
}

//--------------------------------------------------------------------------

void CMapPane::OnPopupMenuCenter(wxCommandEvent& WXUNUSED(event))
{
    CenterClick(m_PopupPoint);
}

//--------------------------------------------------------------------------

void CMapPane::OnPopupMenuFlag(wxCommandEvent& WXUNUSED(event))
{
    int      Cmd;
    int      i;

    if (m_pPopupLand)
    {
        wxPoint     point = ClientToScreen(m_PopupPoint);
        CMapFlagDlg Dlg(gpApp->m_Frames[AH_FRAME_MAP], m_pPopupLand, point);

        Cmd = Dlg.ShowModal();

        switch (Cmd)
        {
        case wxID_OK:
            for (i=0; i<LAND_FLAG_COUNT; i++)
                m_pPopupLand->FlagText[i] = Dlg.m_FlagText[i]->GetValue();
            break;
        case wxID_NO:
            for (i=0; i<LAND_FLAG_COUNT; i++)
                m_pPopupLand->FlagText[i].Empty();
            break;
        default:
            return;
        }

//        gpApp->m_LandFlagsChanged = TRUE;
        Refresh(FALSE);
        gpApp->OnMapSelectionChange();
    }
}

//--------------------------------------------------------------------------

void CMapPane::OnPopupMenuBattles(wxCommandEvent & event)
{
    int       i;
    long      id;
    CBattle * pBattle;
    CBaseColl Battles;

    for (i=0; i<gpApp->m_pAtlantis->m_Battles.Count(); i++)
    {
        pBattle = (CBattle * )gpApp->m_pAtlantis->m_Battles.At(i);

        if (gpApp->m_pAtlantis->LandStrCoordToId(pBattle->LandStrId.GetData(), id) && id==m_pPopupLand->Id)
            Battles.Insert(pBattle);
    }
    gpApp->ShowDescriptionList(Battles, "Battles");

    Battles.DeleteAll();
}

//--------------------------------------------------------------------------

void CMapPane::OnPopupWhoMovesHere(wxCommandEvent & event)
{
    CLand  * pCurLand;
    CPlane * pCurPlane;
    long     HexId;

    pCurPlane = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);
    pCurLand  = gpApp->m_pAtlantis->GetLand(m_SelHexX, m_SelHexY, m_SelPlane, TRUE);
    
    if (pCurLand)
        HexId = pCurLand->Id;  // maybe GetLand produces better Id, so use it if awailable?
    else
        HexId = LandCoordToId(m_SelHexX, m_SelHexY, m_SelPlane);
    
    gpApp->ShowUnitsMovingIntoHex(HexId, pCurPlane);
}

//--------------------------------------------------------------------------

void CMapPane::OnPopupFinancial   (wxCommandEvent & event)
{
    CLand  * pCurLand;

    pCurLand  = gpApp->m_pAtlantis->GetLand(m_SelHexX, m_SelHexY, m_SelPlane, TRUE);
    gpApp->ShowLandFinancial(pCurLand);
}

//--------------------------------------------------------------------------

void CMapPane::OnPopupNewHex(wxCommandEvent & event)
{
    CPlane * pPlane   = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);
    gpApp->AddTempHex(m_SelHexX, m_SelHexY, m_SelPlane);
    SetSelection(m_SelHexX, m_SelHexY, NULL /*gpApp->GetSelectedUnit()*/, pPlane, TRUE);
}


//--------------------------------------------------------------------------


void CMapPane::OnPopupDeleteHex(wxCommandEvent & event)
{
    CPlane * pPlane   = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);
    gpApp->DelTempHex(m_SelHexX, m_SelHexY, m_SelPlane);
    SetSelection(m_SelHexX, m_SelHexY, NULL /*gpApp->GetSelectedUnit()*/, pPlane, TRUE);
}

//--------------------------------------------------------------------------

void CMapPane::OnPopupDistanceRing(wxCommandEvent & event)
{
    m_RingRadius = wxGetNumberFromUser( "Ring radius should be no more than 16,\nCancel removes the ring.", 
                                        "Enter ring radius", 
                                        "Distance ring", 
                                        m_RingRadius>0 ? m_RingRadius : 4, 
                                        0, 16, this);
    m_RingX = m_SelHexX;
    m_RingY = m_SelHexY;

/*    int               width, height;
    
    GetClientSize(&width, &height);
    CPlane        * pPlane   = (CPlane*)gpApp->m_pAtlantis->m_Planes.At(m_SelPlane);
    wxRect          rect(0, 0, width, height);
    wxPaintDC       dc(this); // device context for painting

    DrawRing(&dc, &rect, pPlane);*/
    
    Refresh(FALSE, NULL);
}

//--------------------------------------------------------------------------

void CMapPane::OnEraseBackground(wxEraseEvent& event)
{
}

//--------------------------------------------------------------------------

void CMapPane::OnToolbarCmd(wxCommandEvent& event)
{
//    int  i;


    switch (event.GetId())
    {
    case tool_zoomin    :
        if (SetHexSize(m_HexSizeIdx+1))
        {
            RemoveRectangle();
            Refresh(FALSE, NULL);
        }
        break;

    case tool_zoomout   :
        if (SetHexSize(m_HexSizeIdx-1))
        {
            RemoveRectangle();
            Refresh(FALSE, NULL);
        }
        break;

    case tool_centerout :
        SetHexSize(0);
        Center();
        RemoveRectangle();
        Refresh(FALSE, NULL);
        break;

    case tool_prevzoom  :
        if (SetHexSize(m_HexSizeIdxOld))
        {
            RemoveRectangle();
            Refresh(FALSE, NULL);
        }
        break;

    case tool_showcoord :
        m_ShowState ^= SHOW_COORD;
        Refresh(FALSE, NULL);
        break;
    
    case tool_shownames :
        m_ShowState ^= SHOW_NAMES;
        Refresh(FALSE, NULL);
        break;

    case tool_planeup   :
        if (m_SelPlane > 0)
        {
            SavePlaneConfig();
            m_SelPlane--;
            LoadPlaneConfig();
            Refresh(FALSE, NULL);
            gpApp->OnMapSelectionChange();
        }
        break;

    case tool_planedwn  :
        if (m_SelPlane < gpApp->m_pAtlantis->m_Planes.Count()-1)
        {
            SavePlaneConfig();
            m_SelPlane++;
            LoadPlaneConfig();
            Refresh(FALSE, NULL);
            gpApp->OnMapSelectionChange();
        }
        break;

    case tool_nextturn  :
        gpApp->SwitchToRep(repNext);
        break;

    case tool_prevturn  :
        gpApp->SwitchToRep(repPrev);
        break;

    case tool_lastturn  :
        gpApp->SwitchToRep(repLast);
        break;

    case tool_lastvisitturn  :
        gpApp->SwitchToRep(repLastVisited);
        break;

    case tool_findhex:
        FindHexes();
        break;
    }
}

//--------------------------------------------------------------------------

BOOL CMapPane::IsToolActive(wxUpdateUIEvent& event)
{
    BOOL Ok = FALSE;
    int  i;

    switch (event.GetId())
    {
    case tool_zoomin    :
        Ok = (m_HexSizeIdx < m_HexSizes.Count()-1);
        break;

    case tool_zoomout   :
        Ok = (m_HexSizeIdx > 0);
        break;

    case tool_centerout :
        Ok = TRUE;
        break;

    case tool_prevzoom  :
        Ok = (m_HexSizeIdx != m_HexSizeIdxOld);
        break;

    case tool_showcoord :
        Ok = TRUE;
        if (m_ShowState & SHOW_COORD)
            event.Check(TRUE);
        else
            event.Check(FALSE);
        break;
        
    case tool_shownames :
        Ok = TRUE;
        if (m_ShowState & SHOW_NAMES)
            event.Check(TRUE);
        else
            event.Check(FALSE);
        break;

    case tool_planeup   :
        Ok = (m_SelPlane > 0);
        break;

    case tool_planedwn  :
        Ok = (m_SelPlane < gpApp->m_pAtlantis->m_Planes.Count()-1);
        break;

    case tool_nextturn  :
        Ok = gpApp->CanSwitchToRep(repNext, i);
        break;

    case tool_prevturn  :
        Ok = gpApp->CanSwitchToRep(repPrev, i);
        break;

    case tool_lastturn  :
        Ok = gpApp->CanSwitchToRep(repLast, i);
        break;

    case tool_lastvisitturn  :
        Ok = gpApp->CanSwitchToRep(repLastVisited, i);
        break;

    case tool_findhex:
        Ok = TRUE;
        break;

    }

    return Ok;
}
