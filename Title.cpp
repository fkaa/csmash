/* $Id$ */

// Copyright (C) 2000  $B?@Fn(B $B5H9((B(Kanna Yoshihiro)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "ttinc.h"

extern BaseView theView;
extern long mode;

extern Sound theSound;

extern long gameLevel;
extern long gameMode;

extern bool isWireFrame;

extern void Keyboard( unsigned char key, int x, int y );
extern void KeyUp( unsigned char key, int x, int y );

extern Player* thePlayer;
extern Player* comPlayer;
extern Ball theBall;

Title::Title() {
  m_View = NULL;
  m_selected = 0;
  m_selectMode = 0;
  m_count = 0;
}

Title::~Title() {
  if ( m_View ){
    theView.RemoveView( m_View );
    delete m_View;
  }
}

bool
Title::Init() {
  m_View = new TitleView();
  m_View->Init( this );

  theView.AddView( m_View );

  thePlayer = Player::Create( RAND(3), 1, 1 );
  comPlayer = Player::Create( RAND(3), -1, 1 );

  thePlayer->Init();
  comPlayer->Init();

  return true;
}

Title*
Title::Create() {
  Title *newTitle;

  Event::ClearObject();

  newTitle = new Title();
  newTitle->Init();

  // View, $B$+(B?
  glutSetCursor( GLUT_CURSOR_INHERIT );

  return newTitle;
}

bool
Title::Move( unsigned long *KeyHistory, long *MouseXHistory,
		    long *MouseYHistory, unsigned long *MouseBHistory,
		    int Histptr ) {
  long last = Histptr-1;
  if ( last < 0 )
    last = MAX_HISTORY-1;

  theBall.Move();
  thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
  comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

  switch ( m_selectMode ) {
  case MENU_MAIN:
    // $B%-!<F~NOBP1~(B
    if ( KeyHistory[Histptr] != KeyHistory[last] ) {
      switch ( KeyHistory[Histptr] ) {
      case '8':
	m_selected--;
	break;
      case '2':
	m_selected++;
	break;
      }
      MouseYHistory[Histptr] = m_selected*BaseView::GetWinHeight()/
	GetMenuNum(MENU_MAIN);
    } else {
      m_selected = MouseYHistory[Histptr]*GetMenuNum( MENU_MAIN )/
	BaseView::GetWinHeight();
    }
    break;
  case MENU_CONFIG:
    // $B%-!<F~NOBP1~(B
    if ( KeyHistory[Histptr] != KeyHistory[last] ) {
      switch ( KeyHistory[Histptr] ) {
      case '8':
	if ( MouseXHistory[Histptr] < BaseView::GetWinWidth()/2 ) {
	  MouseYHistory[Histptr] -= (BaseView::GetWinHeight()-100)/
		       GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL );
	} else {
	  MouseYHistory[Histptr] -= (BaseView::GetWinHeight()-100)/
	  (GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE )
	 +GetMenuNum( MENU_CONFIG, MENU_CONFIG_PLAYER ));
	}
	if ( MouseYHistory[Histptr] < 0 )
	  MouseYHistory[Histptr] = 0;
	break;
      case '6':
	MouseXHistory[Histptr] = BaseView::GetWinWidth()/4*3;
	break;
      case '4':
	MouseXHistory[Histptr] = BaseView::GetWinWidth()/4;
	break;
      case '2':
	if ( MouseXHistory[Histptr] < BaseView::GetWinWidth()/2 ) {
	  MouseYHistory[Histptr] += (BaseView::GetWinHeight()-100)/
		       GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL );
	} else {
	  MouseYHistory[Histptr] += (BaseView::GetWinHeight()-100)/
	  (GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE )
	 +GetMenuNum( MENU_CONFIG, MENU_CONFIG_PLAYER ));
	}
	if ( MouseYHistory[Histptr] > BaseView::GetWinHeight() )
	  MouseYHistory[Histptr] = BaseView::GetWinHeight();
	break;
      }
    }

    if ( MouseXHistory[Histptr] < BaseView::GetWinWidth()/2 ) {
      m_selected = MouseYHistory[Histptr]*
	GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL )/
	(BaseView::GetWinHeight()-100);
      if ( m_selected >= GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL )-1 )
	m_selected = GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL )-2;
    } else {
      m_selected = MouseYHistory[Histptr]*
	(GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE )
	 +GetMenuNum( MENU_CONFIG, MENU_CONFIG_PLAYER ))/
	(BaseView::GetWinHeight()-100)
	+GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL );
      if ( m_selected >= GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE )+
	   GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL )+
	   GetMenuNum( MENU_CONFIG, MENU_CONFIG_PLAYER ) )
	m_selected = GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE )+
	  GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL )+
	  GetMenuNum( MENU_CONFIG, MENU_CONFIG_PLAYER )-1;
    }

    if ( MouseYHistory[Histptr] > BaseView::GetWinHeight()-100 )
      m_selected = GetMenuNum( MENU_CONFIG, MENU_ALL )-1;
  }

  if ( m_selectMode == MENU_CONFIG && KeyHistory[Histptr] == 27 ) {	// ESC
    m_selectMode = MENU_MAIN;
    return true;
  }

  if ( m_selected < 0 )
    m_selected = 0;
  else {
    if ( m_selectMode == MENU_MAIN && m_selected >= GetMenuNum( MENU_MAIN ) )
      m_selected = GetMenuNum( MENU_MAIN )-1;
  }

  if ( ((MouseBHistory[Histptr]&BUTTON_LEFT) && 
       !(MouseBHistory[last]&BUTTON_LEFT)) ||
       (KeyHistory[Histptr] == 13 && KeyHistory[last] != 13) ) {
    switch ( m_selectMode ) {
    case MENU_MAIN:
      switch ( m_selected ) {
      case 0:	// Start
	//theBall.EndGame();
	theBall.Warp( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1000 );
	mode = MODE_SELECT;
	break;
      case 1:	// Training
	//theBall.EndGame();
	theBall.Warp( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1000 );
	mode = MODE_TRAININGSELECT;
	break;
      case 2:	// Howto
	//theBall.EndGame();
	theBall.Warp( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1000 );
	mode = MODE_HOWTO;
	break;
      case 3:	// Config...
	m_selectMode = MENU_CONFIG;
	break;
      case 4:	// Quit
	Event::KeyboardFunc( 'Q', 0, 0 );
	Event::KeyUpFunc( 'Q', 0, 0 );
	break;
      }
      break;
    case MENU_CONFIG:
      if ( m_selected < GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL ) )
	gameLevel = m_selected;
      else if ( m_selected < GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL )+
		             GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE ) )
	gameMode = m_selected-GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL );
      else if ( m_selected < GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL )+
		GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE )+
		GetMenuNum( MENU_CONFIG, MENU_CONFIG_PLAYER ) ) {
#if 0
	theSound.SetSoundMode( m_selected-GetMenuNum( MENU_CONFIG,
						      MENU_CONFIG_LEVEL )
			       -GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE ) );
#else
	if ( m_selected-GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL )
	     -GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE ) == 0 )
	  isWireFrame = true;
	else
	  isWireFrame = false;
#endif
      } else if ( m_selected == GetMenuNum( MENU_CONFIG, MENU_ALL )-1 )
	m_selectMode = MENU_MAIN;
    }
  }

  m_count++;

  return true;
}

long
Title::GetSelected() {
  return m_selected;
}

long
Title::GetSelectMode() {
  return m_selectMode;
}

long
Title::GetCount() {
  return m_count;
}

long
Title::GetMenuNum( long major, long minor ) {
  switch ( major ) {
  case MENU_MAIN:
    return 5;
  case MENU_CONFIG:
    switch ( minor ) {
    case MENU_ALL:
      return 10;
    case MENU_CONFIG_LEVEL:
      return 4;
    case MENU_CONFIG_MODE:
      return 3;
    case MENU_CONFIG_PLAYER:
      return 2;
    }
  }

  return -1;
}

bool
Title::LookAt( double &srcX, double &srcY, double &srcZ,
	       double &destX, double &destY, double &destZ ) {
  srcX = TABLELENGTH*2*cos(GetCount()*3.14159265/720.0);
  srcY = TABLELENGTH*2*sin(GetCount()*3.14159265/720.0) + TABLELENGTH/2;
  srcZ = TABLEHEIGHT*4;

  return true;
}
