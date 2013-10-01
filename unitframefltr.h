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

#ifndef __AH_UNIT_FRAME_FLTR_INCL__
#define __AH_UNIT_FRAME_FLTR_INCL__

//----------------------------------------------------------------

class CUnitFrameFltr : public CAhFrame
{
public:
    CUnitFrameFltr(wxWindow * parent);

    virtual void    Init(int layout, const char * szConfigSection);
    virtual void    Done(BOOL SetClosedFlag);

    static const char * GetConfigSection(int layout);

private:
    void OnCloseWindow(wxCloseEvent& event);


    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------


#endif

