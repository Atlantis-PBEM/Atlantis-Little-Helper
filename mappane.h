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

#ifndef __AH_MAP_PANE_INCL__
#define __AH_MAP_PANE_INCL__


class CLand;
class CPlane;
class CCollection;

#define SHOW_COORD   0x0001
#define SHOW_NAMES   0x0002

enum {
    TOP_LEFT_CORNER,
    TOP_RIGHT_CORNER,
    RIGHT_CORNER,
    LEFT_CORNER,
    BOTTOM_LEFT_CORNER,
    BOTTOM_RIGHT_CORNER,
    CENTER
};


//---------------------------------------------------------------------

class CBrushColl : public CCollection
{
public:
    CBrushColl() : CCollection() {};
    CBrushColl(int nDelta) : CCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem) { delete (wxBrush*)pItem; };
};

//---------------------------------------------------------------------

class CEdgeStructProperties
{
public:
    CEdgeStructProperties()
    {
        pen = NULL;
    }
    ~CEdgeStructProperties()
    {
        if (pen)
            delete pen;
    }
    CStr    name;
    int     shape;
    wxPen * pen;
};


class CEdgePropColl : public CSortedCollection
{
public:
    CEdgePropColl()           : CSortedCollection() {};
    CEdgePropColl(int nDelta) : CSortedCollection(nDelta) {};
protected:
    virtual void FreeItem(void * pItem) { delete (CEdgeStructProperties*)pItem; };
    virtual int  Compare(void * pItem1, void * pItem2)
    {
        return stricmp( ((CEdgeStructProperties*)pItem1)->name.GetData(),
                        ((CEdgeStructProperties*)pItem2)->name.GetData() );
    }
};

//---------------------------------------------------------------------

class CMapPane : public wxWindow
{
public:
    CMapPane(wxWindow * parent, wxWindowID id, int layout);
    virtual ~CMapPane();
    virtual void Init(CAhFrame * pParentFrame);
    virtual void Done();
    BOOL     SetHexSize(int HexSizeIdx);  // was size really changed?
    void     GetHexCenter(int NoX, int NoY, int & WinX, int & WinY);
    BOOL     ValidHexNo(int NoX, int NoY);
    //void     SelectHex(int NoX, int NoY);
    void     SetSelection(int nx, int ny, CUnit * pUnit, CPlane * pPlane, BOOL Anyway);
    BOOL     EnsureLandVisible(int nx, int ny, int nz, BOOL Anyway);
    void     Center();
    void     SavePlaneConfig();
    void     LoadPlaneConfig();
    void     ApplyFonts();
    void     ApplyColors();
    void     ApplyIcons();
    void     RedrawTracksForUnit(CPlane * pPlane, CUnit * pUnit, wxDC * pDC, BOOL DoDrawCitiesAndWeather);
    void     OnToolbarCmd(wxCommandEvent& event);
    BOOL     IsToolActive(wxUpdateUIEvent& event);
//  void     CleanCities() {    m_pCities->DeleteAll(); };
    void     GetSelectedOrAllHexes(CBaseColl & Hexes, BOOL Selected);
    BOOL     HexIsSelected(CLand * pLand);
    BOOL     HaveSelection();
    void     RemoveRectangle();
    void     MarkFoundHexes(CHexFilterDlg * pFilter);
    void     UnMarkFoundHexes();
    void     FindHexes();

    int         m_HexSizeIdx;
    int         m_HexSizeIdxOld;
    int         m_HexSize;
    int         m_HexHalfSize;
    int         m_HexHeight;
    int         m_HexHalfHeight;

    long        m_AtlaX0; // corresponds to WinX=0
    long        m_AtlaY0; // corresponds to WinY=0

    long        m_ShowState;
    long        m_MinSelMen;

    long        m_SelHexX;
    long        m_SelHexY;
    long        m_SelPlane;

    BOOL        m_bAdvancedIcons;

    CLongColl   m_HexSizes;


protected:

    void      WinToAtla(int   WinX, int   WinY, long & AtlaX, long & AtlaY);
    void      AtlaToWin(int & WinX, int & WinY, long   AtlaX, long   AtlaY);

    void      GetHexNo(int & NoX, int & NoY, int WinX, int WinY);
    void      DrawHex         (int NoX, int NoY, wxDC * pDC, CLand * pLand, CPlane * pPlane, wxRect * prect);
    void      DrawOneHex      (int NoX, int NoY, int x0, int y0, wxDC * pDC, CLand * pLand, CPlane * pPlane);
    void      DrawHexBorder   (int NoX, int NoY, wxDC * pDC, CLand * pLand, BOOL FullBorder, CPlane * pPlane, wxRect * prect, int DrawPhase, BOOL AutoEmpty);
    void      DrawOneHexBorder(int x0, int y0,   wxDC * pDC, BOOL IsSelected, BOOL FullBorder, wxPen ** pPens, int ExitBits);
    void      DrawHexTest     (int NoX, int NoY, wxDC * pDC, CLand * pLand) ;
    void      DrawCitiesAndWeather      (wxDC * pDC, wxRect * pRect, CPlane * pPlane);
    void      DrawOneHexWeatherLine(int NoX, int NoY, wxDC * pDC, int DrawBits);
    void      DrawEdgeStructures(wxDC * pDC, CLand * pLand, wxPoint * point, int x0, int y0);
    void      NormalizeCoordinates(CPlane * pPlane, wxRect * pRect);
    int       NormalizeHexX(int NoX, CPlane * pPlane);
    void      DrawEdge(wxDC * pDC, CPlane * pPlane);
    void      DrawUnitColumn(wxDC * pDC, int x0, int y0, int height);
    void      DrawCoastalLine(wxDC * pDC, wxPoint * point, int direction);
    void      DrawRoads(wxDC * pDC, CLand * pLand, int x0, int y0);
    void      DrawTaxTrade(wxDC * pDC, CLand * pLand, wxPoint * point);
    void      DrawCity(wxDC * pDC, CLand * pLand, int x0, int y0);
    void      DrawUnits(wxDC * pDC, CLand * pLand, wxPoint * point);
    void      DrawStructures(wxDC * pDC, CLand * pLand, wxPoint * point, int x0, int y0);
    void      DrawHexFlags(wxDC * pDC, CLand * pLand, int x0, int y0);
    void      DrawRing(wxDC * pDC, wxRect * pRect, CPlane * pPlane);
    void      DrawRingSegment(wxDC * pDC, wxRect * pRect, CPlane * pPlane, int dx, int dy, int DirFlags);

    void      DrawSingleTrack(int X, int Y, int wx, int wy, wxDC * pDC, CUnit * pUnit, CPlane * pPlane, int copyno);
    void      DrawTrackArrow(wxDC * pDC, int wx0, int wy0, int wx, int wy);
    void      CenterClick(wxPoint point);
    wxBrush * GetLandBrush(CLand * pLand, BOOL GetHatched);
    CEdgeStructProperties * GetEdgeProps(const char * name);
    //BOOL      GetSelectedHexes(int & x1, int & y1, int & x2, int & y2 );

    void      ApplyOneColor(wxColour & cr, const char * name);
//    void      ApplyOneEdgeColor(const char * name, const char * value);
    void      DrawCoordPanes(wxDC * pDC, int mapwidth, int mapheight, CPlane * pPlane);
    void      GetNextIconPos(wxPoint * point, int Position);
    void      AdjustForA3Location(int & wx, int & wy, int Location);
    int       Distance(int x1, int y1, int x2, int y2);

    void      OnPaint            (wxPaintEvent   & event);
    void      OnMouseEvent       (wxMouseEvent   & event);
    void      OnEraseBackground  (wxEraseEvent   & event);
    void      OnPopupMenuFlag    (wxCommandEvent & event);
    void      OnPopupMenuCenter  (wxCommandEvent & event);
    void      OnPopupMenuBattles (wxCommandEvent & event);
    void      OnPopupWhoMovesHere(wxCommandEvent & event);
    void      OnPopupFinancial   (wxCommandEvent & event);
    void      OnPopupNewHex      (wxCommandEvent & event);
    void      OnPopupDeleteHex   (wxCommandEvent & event); 
    void      OnPopupDistanceRing(wxCommandEvent & event); 

    void      StartRectangle(int x, int y);
    void      EndRectangle  (int x, int y);
    void      MoveRectangle (int x, int y);
    void      PauseRectangle();
    void      ResumeRectangle();
    bool      ShouldDrawRectangle();
    void      PaintRectangle(wxDC * pDC);

    void      DrawShieldIcon(wxDC * pDC, int x, int y, wxColour pGuardColor, wxColour pDarkColor, BOOL large, BOOL mark);
    void      DrawFlagIcon(wxDC * pDC, int x, int y, wxColour FlagColor, wxColor DarkColor, BOOL large, BOOL presence);

    wxPen        * m_pPen;
    wxPen        * m_pPenWall;
    wxPen        * m_pPenSel;
    wxPen        * m_pPenTropic;
    wxPen        * m_pPenRing;
    wxPen        * m_pPenRoad;
    wxPen        * m_pPenRoadBad;
    wxPen        * m_pPenBlack;
    wxPen        * m_pPenRed;
    wxPen        * m_pPenRed2;
    wxPen        * m_pPenGrey;
    wxPen        * m_pPenFlag[LAND_FLAG_COUNT];
    wxPen        * m_pPenCoastline;
    wxBrush      * m_pBrushBlack; 
    wxBrush      * m_pBrushWhite; 
    wxBrush      * m_pBrushRed  ;
    wxColour     * m_pUnitColor[ATT_UNDECLARED];
    wxColour     * m_pDarkColor[ATT_UNDECLARED];
    BOOL           m_Hatch;
    int            m_Detail;
    int            m_HexIcons[7];
    int            SymLeft, SymBottom;

    CBrushColl     m_TerrainBrushes;
    CBufColl       m_TerrainNames;
    int            m_UnknownColorIdx;

    CEdgePropColl  m_EdgeProps;

    CCollection  * m_pCities;
    CCollection  * m_pTrackHexes;

    CAhFrame     * m_pFrame;

    CLand        * m_pPopupLand;
    wxPoint        m_PopupPoint;

    bool           m_Rect_IsPaused;
    bool           m_Rect_IsDragging;
    int            m_Rect_x1;
    int            m_Rect_y1;
    int            m_Rect_x2;
    int            m_Rect_y2;
    
    int            m_RingRadius;
    int            m_RingX;
    int            m_RingY;


    DECLARE_EVENT_TABLE()
};


//---------------------------------------------------------------------



#endif
