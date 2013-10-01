/***************************************************************************
 *   Copyright (C) 2004 by Max Shariy                                      *
 *   mshariy@shaw.ca                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/*
#include <wx/wx.h>
#include "ah25.h"
#include "cstr.h"

BEGIN_EVENT_TABLE( ah25Frame, wxFrame )
	EVT_MENU( Menu_File_Quit, ah25Frame::OnQuit )
	EVT_MENU( Menu_File_About, ah25Frame::OnAbout )
END_EVENT_TABLE()

IMPLEMENT_APP(ah25app)
	

bool 
ah25app::OnInit()
{
	ah25Frame *frame = new ah25Frame( wxT( "Hello World" ), wxPoint(50,50), wxSize(450,340) );

	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
} 

ah25Frame::ah25Frame( const wxString& title, const wxPoint& pos, const wxSize& size )
	: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	
	menuFile->Append( Menu_File_About, wxT( "&About..." ) );
	menuFile->AppendSeparator();
	menuFile->Append( Menu_File_Quit, wxT( "E&xit" ) );
	
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, wxT( "&File" ) );
	
	SetMenuBar( menuBar );
	
	CreateStatusBar();
	SetStatusText( wxT( "Welcome to Kdevelop wxWidgets app!" ) );
}

void 
ah25Frame::OnQuit( wxCommandEvent& WXUNUSED( event ) )
{
	Close(TRUE);
}

void 
ah25Frame::OnAbout( wxCommandEvent& WXUNUSED( event ) )
{
    CStr S;
    int  x;
    
    S = "Hi there";
    x = 1;
    S.Format("%d", x);
    
	wxMessageBox( wxT( "This is a wxWidgets Hello world sample" ),
			wxT( "About Hello World" ), wxOK | wxICON_INFORMATION, this );
}

*/

