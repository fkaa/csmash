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
#include "Title.h"
#include "BaseView.h"
#include "BaseView2D.h"
#include "Sound.h"
#include "Player.h"
#include "Ball.h"
#include "TitleView.h"
#include "TitleView2D.h"
#include "Event.h"
#include "MenuItem.h"

extern BaseView* theView;
extern long mode;

extern Sound theSound;

extern long gameLevel;
extern long gameMode;

extern bool isWireFrame;
extern long gmode;

extern void QuitGame();

extern Player* thePlayer;
extern Player* comPlayer;
extern Ball theBall;

Title::Title() {
  m_View = NULL;
  m_selected = 0;
  m_selectMode = 0;
  m_count = 0;

  for ( int i = 0 ; i < 16 ; i++ )
    m_menuItem[i] = NULL;
}

Title::~Title() {
  for ( int i = 0 ; i < 16 ; i++ ) {
    if ( m_menuItem[i] )
      delete m_menuItem[i];
  }

  if ( m_View ){
    theView->RemoveView( m_View );
    delete m_View;
  }
}

bool
Title::Init() {
  if ( gmode == GMODE_2D )
    m_View = new TitleView2D();
  else
    m_View = new TitleView();

  m_View->Init( this );

  theView->AddView( m_View );

  thePlayer = Player::Create( RAND(3), 1, 1 );
  comPlayer = Player::Create( RAND(3), -1, 1 );

  thePlayer->Init();
  comPlayer->Init();

  CreateMenu( MENU_MAIN );

  return true;
}

Title*
Title::Create() {
  Title *newTitle;

  Event::ClearObject();

  newTitle = new Title();
  newTitle->Init();

  // Move it to view?
  SDL_ShowCursor(1);
  SDL_WM_GrabInput( SDL_GRAB_OFF );

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
    // Get key input
    if ( KeyHistory[Histptr] != KeyHistory[last] ) {
      switch ( KeyHistory[Histptr] ) {
      case '8':
      case SDLK_UP:
	SetSelected( m_selected-1 );
	break;
      case '2':
      case SDLK_DOWN:
	SetSelected( m_selected+1 );
	break;
      }

      MouseYHistory[Histptr] = BaseView::GetWinHeight() -
	(m_menuItem[m_selected]->GetY()+m_menuItem[m_selected]->GetHeight()/2);
    } else {
      SetSelected( HitTest( MouseXHistory[Histptr], MouseYHistory[Histptr] ) );
    }
    break;
  case MENU_CONFIG:
    // Get key input
    if ( KeyHistory[Histptr] != KeyHistory[last] ) {
      switch ( KeyHistory[Histptr] ) {
      case '8':
      case SDLK_UP:
	SetSelected( m_selected-1 );
	break;
      case '6':
      case SDLK_RIGHT:
	MouseXHistory[Histptr] = BaseView::GetWinWidth()/4*3;
	SetSelected( HitTest( MouseXHistory[Histptr], MouseYHistory[Histptr] ) );
	break;
      case '4':
      case SDLK_LEFT:
	MouseXHistory[Histptr] = BaseView::GetWinWidth()/4;
	SetSelected( HitTest( MouseXHistory[Histptr], MouseYHistory[Histptr] ) );
	break;
      case '2':
      case SDLK_DOWN:
	SetSelected( m_selected+1 );
	break;
      }
      MouseYHistory[Histptr] = BaseView::GetWinHeight() -
	(m_menuItem[m_selected]->GetY()+m_menuItem[m_selected]->GetHeight()/2);
      MouseXHistory[Histptr] = 
	(m_menuItem[m_selected]->GetX()+m_menuItem[m_selected]->GetWidth()/2);
    } else {
      SetSelected( HitTest( MouseXHistory[Histptr], MouseYHistory[Histptr] ) );
    }
  }

  if ( m_selectMode == MENU_CONFIG && KeyHistory[Histptr] == SDLK_ESCAPE ) {
    m_selectMode = MENU_MAIN;
    CreateMenu( m_selectMode );
    m_selected = 0;

    return true;
  }

  if ( m_selected < 0 )
    SetSelected( 0 );
  else {
    if ( m_selectMode == MENU_MAIN && m_selected >= GetMenuNum( MENU_MAIN ) )
      SetSelected( GetMenuNum( MENU_MAIN )-1 );
  }

  if ( ((MouseBHistory[Histptr]&BUTTON_LEFT) && 
       !(MouseBHistory[last]&BUTTON_LEFT)) ||
       (KeyHistory[Histptr] == SDLK_RETURN &&
	KeyHistory[last] != SDLK_RETURN) ) {
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
	QuitGame();
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
	if ( m_selected-GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL )
	     -GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE ) == 0 )
	  isWireFrame = true;
	else
	  isWireFrame = false;
      } else if ( m_selected == GetMenuNum( MENU_CONFIG, MENU_ALL )-1 )
	m_selectMode = MENU_MAIN;
    }

    CreateMenu( m_selectMode );
    m_selected = 0;
  }

  m_count++;

  return true;
}

MenuItem *
Title::GetSelected() {
  return m_menuItem[m_selected];
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

void
Title::CreateMenu( long menuMajorNum ) {
  static char menu[][30] = {"images/StartGame", "images/Training",
			    "images/Howto", "images/Config",
			    "images/Quit"};
  static char configMenu[][30] = {"images/Easy", "images/Normal",
				  "images/Hard", "images/Tsuborish", 
				  "images/5point", "images/11point",
				  "images/21point",
				  "images/WireFrame", "images/Transparent" };
  static char configTitle[][30] = {"images/LevelSelect",
				   "images/ModeSelect"
				    };

  int i, j;

  if ( m_menuItem[0] ) {
    for ( i = 0 ; i < 16 ; i++ ) {
      if ( m_menuItem[i] ) {
	delete m_menuItem[i];
	m_menuItem[i] = NULL;
      }
    }
  }

  for ( i = 0 ; i < GetMenuNum( menuMajorNum ) ; i++ )
    m_menuItem[i] = new MenuItem();

  switch ( menuMajorNum ) {
  case MENU_MAIN:
    for ( i = 0 ; i < GetMenuNum( MENU_MAIN ) ; i++ )
      m_menuItem[i]->Init( 200, 450-i*100, 400, 70, &menu[i][0], this );
    break;
  case MENU_CONFIG:
    for ( i = 0 ; i < GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL ) ; i++ )
      m_menuItem[i]->Init( 100, 450-i*60, 200, 35, &configMenu[i][0], this );
    for ( i = 0 ; i < GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE ) ; i++ ) {
      j = i+GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL );
      m_menuItem[j]->Init( 500, 450-i*60, 200, 35, &configMenu[j][0], this );
    }
    for ( i = 0 ; i < GetMenuNum( MENU_CONFIG, MENU_CONFIG_PLAYER ) ; i++ ) {
      j = i+GetMenuNum( MENU_CONFIG, MENU_CONFIG_LEVEL ) +
	GetMenuNum( MENU_CONFIG, MENU_CONFIG_MODE );
      m_menuItem[j]->Init( 500, 190-i*60, 200, 35, &configMenu[j][0], this );
    }
    m_menuItem[j+1]->Init( 300, 20, 400, 70, &menu[4][0], this );

    m_menuItem[gameLevel]->SetSelected( true );

    m_menuItem[GetMenuNum(MENU_CONFIG, MENU_CONFIG_LEVEL)+gameMode]
      ->SetSelected( true );

    if ( isWireFrame )
      m_menuItem[GetMenuNum(MENU_CONFIG, MENU_CONFIG_LEVEL)+
		GetMenuNum(MENU_CONFIG, MENU_CONFIG_MODE)]
	->SetSelected( true );
    else
      m_menuItem[GetMenuNum(MENU_CONFIG, MENU_CONFIG_LEVEL)+
		GetMenuNum(MENU_CONFIG, MENU_CONFIG_MODE)+1]
	->SetSelected( true );

    break;
  }
}


long
Title::SetSelected( long selected ) {
  if ( selected < 0 || selected >= GetMenuNum( m_selectMode ) )
    return m_selected;

  switch ( m_selectMode ) {
  case MENU_MAIN:
    m_menuItem[m_selected]->SetSelected( false );
    m_menuItem[selected]->SetSelected( true );
    break;
  case MENU_CONFIG:
    if ( selected == 3 )
      return m_selected;

    break;
  }

  m_selected = selected;
  return m_selected;
}

long 
Title::HitTest( long x, long y ) {
  y = BaseView::GetWinHeight()-y;
  for ( int i = 0 ; i < GetMenuNum( m_selectMode ) ; i++ ) {
    if ( x > m_menuItem[i]->GetX() &&
	 x < m_menuItem[i]->GetX()+m_menuItem[i]->GetWidth() &&
	 y > m_menuItem[i]->GetY() &&
	 y < m_menuItem[i]->GetY()+m_menuItem[i]->GetHeight() )
      return i;
  }

  return -1;
}
