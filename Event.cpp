/**
 * @file
 * @brief Implementation of Event class. 
 * @author KANNA Yoshihiro
 * @version $Id$
 */

// Copyright (C) 2000-2004, 2007  Kanna Yoshihiro
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
#include "Event.h"
#include "Control.h"
#include "BaseView.h"
#include "BaseView2D.h"
#include "SoloPlay.h"
#include "LogPlay.h"
#include "PracticePlay.h"
#include "MultiPlay.h"
#include "Network.h"
#include "PlayerSelect.h"
#include "MultiPlayerSelect.h"
#include "Title.h"
#include "Opening.h"
#include "Howto.h"
#include "TrainingSelect.h"
#include "Training.h"
#include "PracticeSelect.h"
#include "RCFile.h"

#ifdef LOGGING
#include "Logging.h"
#endif

extern RCFile *theRC;

extern Ball theBall;
extern long mode;

extern void Timer( int value );
struct timeb Event::m_lastTime = {0, 0, 0, 0};

Event* Event::m_theEvent = NULL;

extern int theSocket;

extern long wins;

extern SDL_mutex *networkMutex;
extern long timeAdj;

long _perfCount;
long perfs;

long _backTrackCount = 0;
double backTracks = 0;

void CopyPlayerData( Player& dest, Player* src );

void QuitGame();

/**
 * Default Constructor. 
 * Initialize history buffer and m_External list. 
 */
Event::Event() {
  memset( &(m_KeyHistory[0]), 0, sizeof(SDL_keysym) );
  m_MouseXHistory[0] = BaseView::GetWinWidth()/2;
  m_MouseYHistory[0] = BaseView::GetWinHeight()/2;
  m_MouseBHistory[0] = 0;
  m_Histptr = 0;

  m_External = NULL;

  m_mouseButton = 0;
}

/**
 * Destructor. 
 * Do nothing. 
 */
Event::~Event() {
}

/**
 * Getter method of singleton Event object. 
 * 
 * @return returns singleton Event object. 
 */
Event*
Event::TheEvent() {
  if ( !m_theEvent )
    m_theEvent = new Event();
  return m_theEvent;
}

/**
 * Initializer method. 
 * 
 * Referring the game mode, this method creates initial Control object. 
 * @return returns true if succeeds. 
 */
bool
Event::Init() {
  m_External = new ExternalNullData();

  switch ( mode ){
  case MODE_SOLOPLAY:
    SoloPlay::Create( 1, RAND(3)+1 );
    break;
  case MODE_MULTIPLAY:
    MultiPlay::Create( 1, RAND(3)+1 );
    break;
  case MODE_HOWTO:
    Howto::Create();
    break;
  case MODE_SELECT:
    PlayerSelect::Create();
    break;
  case MODE_MULTIPLAYSELECT:
    MultiPlayerSelect::Create();
    break;
  case MODE_TITLE:
    Title::Create();
    break;
  case MODE_OPENING:
    Opening::Create();
    break;
  case MODE_LOGPLAY:
    LogPlay::Create();
    break;
  }

  theBall.Init();

  return true;
}

/**
 * Idle event handler. 
 * 
 * When event queue is empty, this method is called by PollEvent(). 
 * First time when this method is called, this method cleans up backtrack
 * buffer. Otherwise, this method calls Event::Move() method. After that, 
 * this method redraws screen if necessary. 
 * On network play, this method processes incoming messages, too. 
 */
void
Event::IdleFunc() {
  struct timeb tb;

  long diffsec, diffmsec;
  long diffcount = 0;
  bool reDraw = false;

#ifndef WIN32
  struct timeval tv;
  struct timezone tz;
#endif

#ifdef WIN32
  ftime( &tb );
#else
  gettimeofday( &tv, &tz );
  tb.time = tv.tv_sec;
  tb.millitm = tv.tv_usec/1000;
#endif

  if ( m_lastTime.time == 0 ){
    m_lastTime = tb;
    _perfCount = 0;
    perfs = 0;

    ClearBacktrack();

    return;
  }

  diffsec = tb.time - m_lastTime.time;
  diffmsec = tb.millitm + diffsec*1000 - m_lastTime.millitm;

  diffcount = diffmsec / 10;
  if ( diffcount == 0 ) {
//    usleep( (10-diffmsec)*1000 );
    return;
  }

  perfs +=diffcount;
  _perfCount++;

  for ( int i = 0 ; i < diffcount ; i++ ) {
    // While pause, never move objects. (Solo Play)
    if ( Control::TheControl()->IsPlaying() &&
	 ((PlayGame *)Control::TheControl())->IsPause() &&
	 mode != MODE_MULTIPLAY && mode != MODE_MULTIPLAYSELECT) {
      reDraw = true;
    } else {
      reDraw |= Event::TheEvent()->Move();
      if (mode == MODE_LOGPLAY)
	break;	// Move only once. 
    }

    m_lastTime.millitm += 10;
    if ( m_lastTime.millitm > 1000 ) {
      m_lastTime.time++;
      m_lastTime.millitm -= 1000;
    }
  }

  if ( mode == MODE_MULTIPLAY ) {
    long preMode = mode;
    Event::TheEvent()->ReadData();
    Event::TheEvent()->IsModeChanged( preMode );
  } else if ( mode == MODE_MULTIPLAYSELECT ) {
    Event::TheEvent()->ReadSelectData();
  }

  if ( mode != MODE_OPENING && mode != MODE_TITLE &&
       !(Control::TheControl()->IsPlaying() &&
	 ((PlayGame *)Control::TheControl())->IsPause()) )
    SDL_WarpMouse((unsigned short)Event::TheEvent()->m_MouseXHistory[Event::TheEvent()->m_Histptr],
		  (unsigned short)Event::TheEvent()->m_MouseYHistory[Event::TheEvent()->m_Histptr] );

  if ( reDraw )
    BaseView::TheView()->RedrawAll();
}

/**
 * Move all objects. 
 * 
 * This method calls Move() method of current Control object. 
 * Then record current objects to history buffer and backtrack buffer. 
 * 
 * @return returns true if the screen should be redrawn. 
 */
bool
Event::Move() {
  long preMode = mode;
  bool reDraw = false;

  reDraw |= Control::TheControl()->Move( m_KeyHistory, m_MouseXHistory,
					 m_MouseYHistory, m_MouseBHistory,
					 m_Histptr );

  reDraw |= IsModeChanged( preMode );

  Record();

  return reDraw;
}

/**
 * Check whether game mode has changed or not. 
 * This method compares the current game mode with parameter. 
 * If game mode has changed, update Control object. 
 * 
 * @param preMode game mode which is compared with current one. 
 * @return returns true if game mode has changed. 
 */
bool
Event::IsModeChanged( long preMode ) {
  if ( mode != preMode ) {	// mode change
    long p, q;

#ifdef LOGGING
  if (Control::TheControl()->IsPlaying())
    Event::TheEvent()->Event::RemainingLog();
#endif

    switch ( mode ) {
    case MODE_SOLOPLAY:
      p = ((PlayerSelect *)Control::TheControl())->GetPlayerNum()+1;
      q = ((PlayerSelect *)Control::TheControl())->GetOpponentNum()+1;

      SoloPlay::Create( p, q );
      break;
    case MODE_MULTIPLAY:
      p = ((PlayerSelect *)Control::TheControl())->GetPlayerNum()+1;
      q = ((PlayerSelect *)Control::TheControl())->GetOpponentNum()+1;
      MultiPlay::Create( p, q );
      break;
    case MODE_SELECT:
      PlayerSelect::Create();
      break;
    case MODE_MULTIPLAYSELECT:
      MultiPlayerSelect::Create();
      break;
    case MODE_TITLE:
      Title::Create();
      break;
    case MODE_OPENING:
      Opening::Create();
      break;
    case MODE_HOWTO:
      Howto::Create();
      break;
    case MODE_TRAININGSELECT:
      TrainingSelect::Create();
      break;
    case MODE_TRAINING:
      p = ((TrainingSelect *)Control::TheControl())->GetPlayerNum();
      Training::Create( p, p );
      break;
    case MODE_PRACTICESELECT:
      PracticeSelect::Create();
      break;
    case MODE_PRACTICE:
      p = ((PlayerSelect *)Control::TheControl())->GetPlayerNum()+1;
      q = ((PlayerSelect *)Control::TheControl())->GetOpponentNum()+1;
      PracticePlay::Create( p, q );
      break;
    }

    ClearBacktrack();

    return true;
  }

  return false;
}

/**
 * Record current objects to history buffer and backtrack buffer. 
 * This method saves current theBall, thePlayer, comPlayer objects to 
 * history buffer and backtrack buffer. Additionally, this method moves 
 * mouse pointer. 
 */
void
Event::Record() {
  long x, y;
  SDL_keysym key;
  long btn;

  x = m_MouseXHistory[m_Histptr];
  y = m_MouseYHistory[m_Histptr];
  key = m_KeyHistory[m_Histptr];
  btn = m_MouseBHistory[m_Histptr];

  m_Histptr++;

  if ( m_Histptr == MAX_HISTORY )
    m_Histptr = 0;

#ifdef LOGGING
  if (Control::TheControl()->IsPlaying())
    logRecord();
#endif

  /*
  if ( (mode == MODE_SOLOPLAY || mode == MODE_PRACTICE) &&
       Control::TheControl() &&
       ((SoloPlay *)Control::TheControl())->GetSmashPtr() >= 0 )
    return;
  */

  // Warp Mouse Pointer
  SetNextMousePointer( x, y );

  memset( &(m_KeyHistory[m_Histptr]), 0, sizeof(SDL_keysym) );
  m_MouseXHistory[m_Histptr] = x;
  m_MouseYHistory[m_Histptr] = y;
  m_MouseBHistory[m_Histptr] = btn;

  m_BacktrackBuffer[m_Histptr].sec = m_lastTime.time;
  m_BacktrackBuffer[m_Histptr].count = m_lastTime.millitm/10;

  m_BacktrackBuffer[m_Histptr].theBall = theBall;
  CopyPlayerData( m_BacktrackBuffer[m_Histptr].thePlayer,
		  Control::GetThePlayer() );
  CopyPlayerData( m_BacktrackBuffer[m_Histptr].comPlayer,
		  Control::GetComPlayer() );

  if ( mode == MODE_SOLOPLAY || mode == MODE_MULTIPLAY ||
       mode == MODE_PRACTICE ) {
    m_BacktrackBuffer[m_Histptr].score1 =
      ((PlayGame *)Control::TheControl())->GetScore(1);
    m_BacktrackBuffer[m_Histptr].score2 =
      ((PlayGame *)Control::TheControl())->GetScore(-1);
  }

  vector3d src, dest;
  Control::TheControl()->LookAt(src, dest);
  m_BacktrackBuffer[m_Histptr].camera = src;
  m_BacktrackBuffer[m_Histptr].target = dest;

  return;
}

/**
 * Toggle fullscreen mode and window mode. 
 */
void HotKey_ToggleFullScreen(void)
{
  SDL_Surface *screen = SDL_GetVideoSurface();
  if ( SDL_WM_ToggleFullScreen(screen) ) {
    theRC->fullScreen = (screen->flags & SDL_FULLSCREEN) ? 1 : 0;
    fprintf(stderr, _("Toggled fullscreen mode - now %s\n"),
            theRC->fullScreen  ? _("fullscreen") : _("windowed"));
  } else {
    fprintf(stderr, _("Unable to toggle fullscreen mode\n"));
  }
}

/**
 * Keyboard event handler. 
 * This method accepts keyboard event. If 'Q' is pressed, it quit the game. 
 * If ESC is pressed, the game is paused. This method records key event to
 * history buffer. 
 * 
 * @param key keyboard event
 * @param x
 * @param y
 */
void
Event::KeyboardFunc( SDL_Event key, int x, int y ) {
  if ( (key.key.keysym.sym == SDLK_RETURN) &&
       (key.key.keysym.mod & KMOD_ALT) ) {
    HotKey_ToggleFullScreen();
    return;
  }
  if ( key.key.keysym.unicode == 'Q' ) {
    if ( mode == MODE_SOLOPLAY || mode == MODE_PRACTICE ||
	 mode == MODE_SELECT || mode == MODE_HOWTO ||
	 mode == MODE_TRAININGSELECT || mode == MODE_TRAINING ||
	 mode == MODE_PRACTICESELECT ) {
    } else
      QuitGame();
  } else if ( key.key.keysym.unicode == SDLK_ESCAPE &&
	      Control::TheControl()->IsPlaying() ) {
    if ( ((PlayGame *)Control::TheControl())->IsPause() ) {
      SDL_WM_GrabInput( SDL_GRAB_ON );
      SDL_ShowCursor(SDL_DISABLE);
      ((PlayGame *)Control::TheControl())->SetPause( false );
    } else {
      SDL_WM_GrabInput( SDL_GRAB_OFF );
      SDL_ShowCursor(SDL_ENABLE);
      ((PlayGame *)Control::TheControl())->SetPause( true );
    }
  }

  Event::TheEvent()->m_KeyHistory[Event::TheEvent()->m_Histptr] = key.key.keysym;
}

/**
 * Keyup event handler. 
 */
void
Event::KeyUpFunc( SDL_Event key, int x, int y ) {
  // $BGQ;_$7$F$b$h$$(B
#if 0
  if ( key.key.keysym.unicode == 'Q' ) {
    QuitGame();
  }
#endif
}

/**
 * Mouse motion event handler. 
 * When mouse moves, this method records location of the mouse pointer 
 * to history buffer. 
 * 
 * @param x x-coordinate of the mouse pointer. 
 * @param y y-coordinate of the mouse pointer. 
 */
void
Event::MotionFunc( int x, int y ) {
  Event *e = Event::TheEvent();
  e->m_MouseXHistory[e->m_Histptr] = x;
  e->m_MouseYHistory[e->m_Histptr] = y;
}

/**
 * Mouse button event handler. 
 * When mouse button status is changed, this method is called and this method
 * records mouse button status to history buffer. 
 * 
 * @param button button ID (left, middle, right)
 * @param state button status (pressed, released, etc. )
 * @param x x-coordinate of the mouse pointer. 
 * @param y y-coordinate of the mouse pointer. 
 */
void
Event::ButtonFunc( int button, int state, int x, int y ) {
  Event *e = Event::TheEvent();
  if ( state == SDL_MOUSEBUTTONDOWN ) {
    switch ( button ) {
    case 1:
      e->m_MouseBHistory[e->m_Histptr] |= BUTTON_LEFT;
      e->m_mouseButton |= BUTTON_LEFT;
      break;
    case 2:
      e->m_MouseBHistory[e->m_Histptr] |= BUTTON_MIDDLE;
      e->m_mouseButton |= BUTTON_MIDDLE;
      break;
    case 3:
      e->m_MouseBHistory[e->m_Histptr] |= BUTTON_RIGHT;
      e->m_mouseButton |= BUTTON_RIGHT;
      break;
    }
  } else {	// Relase Button
    switch ( button ) {
    case 1:
      e->m_MouseBHistory[e->m_Histptr] &= ~BUTTON_LEFT;
      e->m_mouseButton &= ~BUTTON_LEFT;
      break;
    case 2:
      e->m_MouseBHistory[e->m_Histptr] &= ~BUTTON_MIDDLE;
      e->m_mouseButton &= ~BUTTON_MIDDLE;
      break;
    case 3:
      e->m_MouseBHistory[e->m_Histptr] &= ~BUTTON_RIGHT;
      e->m_mouseButton &= ~BUTTON_RIGHT;
      break;
    }
  }

  e->m_MouseXHistory[e->m_Histptr] = x;
  e->m_MouseYHistory[e->m_Histptr] = y;
}

/**
 * Copy method for Player object. 
 * 
 * @param dest contents of src is copied to this object. 
 * @param src  Player object to be copied. 
 */
void
CopyPlayerData( Player& dest, Player* src ) {
  if ( !src )
    return;

  dest = *src;
}

/**
 * Get new ExternalData object and add it to ExternalData list. 
 * This method reads new ExternalData which comes from opponent machine.
 * Then, Insert the ExternalData object to ExternalData list. ExternalData list
 * should be sorted by timestamp. 
 * 
 * @param ext top of ExternalData list. 
 * @param side m_side of player. 
 * @return returns true if new ExternalData is added. 
 */
bool
Event::GetExternalData( ExternalData *&ext, long side ) {
  ExternalData *extNow;

  if ( !(extNow = ExternalData::ReadData( side )) )
    return false;

  if ( ext->isNull() || ext->sec > extNow->sec || 
	      (ext->sec == extNow->sec && ext->count > extNow->count) ) {
    extNow->next = ext;
    ext = extNow;
  } else {
    ExternalData *extTmp = ext;
    while ( !extTmp->next->isNull() &&
	    (extTmp->next->sec < extNow->sec || 
	    (extTmp->next->sec == extNow->sec &&
	     extTmp->next->count < extNow->count) ) ) {
      extTmp = extTmp->next;
    }

    extNow->next = extTmp->next;
    extTmp->next = extNow;
  }

  return true;
}

/**
 * Backtrack to the specified time. 
 * This method rollbacks theBall, thePlayer and comPlayer object to the 
 * specified time. 
 * 
 * @param Histptr time to which objects should be rollbacked. 
 * @return returns true if possible. 
 */
bool
Event::BackTrack( long Histptr ) {
  theBall = m_BacktrackBuffer[Histptr].theBall;
  Control::GetThePlayer()->Reset( &m_BacktrackBuffer[Histptr].thePlayer );
  Control::GetComPlayer()->Reset( &m_BacktrackBuffer[Histptr].comPlayer );

  if ( mode == MODE_SOLOPLAY || mode == MODE_MULTIPLAY ||
       mode == MODE_PRACTICE || mode == MODE_LOGPLAY) {
    ((PlayGame *)Control::TheControl())->
      ChangeScore( m_BacktrackBuffer[Histptr].score1,
		   m_BacktrackBuffer[Histptr].score2 );
  }

  m_Histptr = Histptr;
  return true;
}

struct Backtrack *
Event::GetBackTrack(long histPtr) {
  return &(m_BacktrackBuffer[histPtr]);
}

/**
 * Apply ExternalData to this machine. 
 * First, search for the oldest ExternalData. If its timestamp is older than
 * now, rollback to the time of the oldest ExternalData. 
 * Then, apply the ExternalData and roll-forward. 
 * During roll-forwarding, other ExternalData are also applied. 
*/
void
Event::ReadData() {
  // Caution!! exchanged with WaitForData()

  SDL_mutexP( networkMutex );

  // externalData$B$N@hF,$^$G(Bbacktrack$B$9$k(B. 
  long btCount = 0;
  ExternalData *externalOld;
  long btHistptr;
  while ( !(m_External->isNull()) ) {	// $B8E$9$.$k>pJs$r<N$F$k(B. 
    btCount = (m_lastTime.time-m_External->sec)*100 + 
      (m_lastTime.millitm/10-m_External->count);
    if ( btCount > MAX_HISTORY ) {
#ifdef LOGGING
      char buf[1024];
      sprintf( buf, "Event disposed: %d %d %d %d\n", m_External->dataType,
	       m_External->side, m_External->sec, m_External->count );
      Logging::GetLogging()->Log( LOG_COMMISC, buf );
#endif
      externalOld = m_External;
      m_External = m_External->next;
      delete externalOld;
      continue;
    }
    if ( btCount <= MAX_HISTORY )
      break;
  }

  if ( !(m_External->isNull()) && btCount > 0 ) {
    backTracks += btCount;
    _backTrackCount++;

    btHistptr = m_Histptr - btCount;
    if ( btHistptr < 0 )
      btHistptr += MAX_HISTORY;

    long mtime = m_lastTime.millitm - btCount*10;
    while ( mtime < 0 ) {
      mtime += 1000;
      m_lastTime.time--;
    }
    m_lastTime.millitm = (unsigned short)mtime;

    bool fTheBall, fThePlayer, fComPlayer;
    fTheBall = fThePlayer = fComPlayer = false;

    BackTrack( btHistptr );

    // $BE,MQ$9$k(B -> $B?J$a$k$r(BbtCount$B7+$jJV$9(B
    while (1) {
      if ( fTheBall )
	theBall.Move();

      if ( fThePlayer )
	Control::GetThePlayer()->Move( m_KeyHistory, m_MouseXHistory,
				       m_MouseYHistory, m_MouseBHistory,
				       m_Histptr );

      if ( fComPlayer )
	Control::GetComPlayer()->Move( NULL, NULL, NULL, NULL, 0 );

      while ( !(m_External->isNull()) &&
	      m_External->sec == m_lastTime.time &&
	      m_External->count == m_lastTime.millitm/10 ) {
	Player *targetPlayer;
	if ( m_External->side == Control::GetThePlayer()->GetSide() )
	  targetPlayer = Control::GetThePlayer();
	else if ( m_External->side == Control::GetComPlayer()->GetSide() )
	  targetPlayer = Control::GetComPlayer();
	else {
	  xerror("%s(%d) ExternalData", __FILE__, __LINE__);
	  exit(1);
	}

	m_External->Apply( targetPlayer, fThePlayer, fComPlayer, fTheBall );

	externalOld = m_External;
	m_External = m_External->next;
	delete externalOld;
      }

      m_Histptr++;
      if ( m_Histptr == MAX_HISTORY )
	m_Histptr = 0;

      if ( fTheBall ) {
	m_BacktrackBuffer[m_Histptr].theBall = theBall;
	if ( mode == MODE_SOLOPLAY || mode == MODE_MULTIPLAY ||
	     mode == MODE_PRACTICE ) {
	  m_BacktrackBuffer[m_Histptr].score1 =
	    ((PlayGame *)Control::TheControl())->GetScore(1);
	  m_BacktrackBuffer[m_Histptr].score2 =
	    ((PlayGame *)Control::TheControl())->GetScore(-1);
	}
      }
      if ( fThePlayer )
	CopyPlayerData( m_BacktrackBuffer[m_Histptr].thePlayer,
			Control::GetThePlayer() );
      if ( fComPlayer )
	CopyPlayerData( m_BacktrackBuffer[m_Histptr].comPlayer, 
			Control::GetComPlayer() );

      m_lastTime.millitm += 10;
      if ( m_lastTime.millitm >= 1000 ) {
	m_lastTime.millitm -= 1000;
	m_lastTime.time++;
      }

      btCount--;
      if ( btCount <= 0 )
	break;
    }

    if (!fTheBall) {
      theBall = m_BacktrackBuffer[m_Histptr].theBall;
      ((PlayGame *)Control::TheControl())->
	ChangeScore( m_BacktrackBuffer[m_Histptr].score1,
		     m_BacktrackBuffer[m_Histptr].score2 );
    }
    if (!fThePlayer)
      Control::GetThePlayer()->Reset(&m_BacktrackBuffer[m_Histptr].thePlayer);
    if (!fComPlayer)
      Control::GetComPlayer()->Reset(&m_BacktrackBuffer[m_Histptr].comPlayer);
  }

  SDL_mutexV( networkMutex );
}

/**
 * Read ExternalData from the opponent machine during player selection. 
 * This method is called during player selection to handle ExternalData
 * which comes from the opponent machine. This method read ExternalData, 
 * and apply it (change the opponent player type, fix the opponent player
 * type, etc. ). 
 */
void
Event::ReadSelectData() {
  bool dum;
  ExternalData *externalOld;

  SDL_mutexP( networkMutex );

  while ( !(m_External->isNull()) ) {
    if ( m_External->dataType != DATA_PT )  // Selection is already finished
      break;

    m_External->Apply( NULL, dum, dum, dum );

    externalOld = m_External;
    m_External = m_External->next;
    delete externalOld;
  }

  SDL_mutexV( networkMutex );
}

/**
 * Clean up backtrack buffer and history buffer. 
 */
void
Event::ClearBacktrack() {
  Ball aBall;

  for ( int i = 0 ; i < MAX_HISTORY ; i++ ) {
    Event::TheEvent()->m_MouseXHistory[i] = 0;
    Event::TheEvent()->m_MouseYHistory[i] = 0;
    Event::TheEvent()->m_MouseBHistory[i] = 0;
    Event::TheEvent()->m_BacktrackBuffer[i].sec = m_lastTime.time;
    Event::TheEvent()->m_BacktrackBuffer[i].count = m_lastTime.millitm/10;
    Event::TheEvent()->m_BacktrackBuffer[i].theBall = aBall;
    CopyPlayerData( Event::TheEvent()->m_BacktrackBuffer[i].thePlayer, 
		    Control::GetThePlayer() );
    CopyPlayerData( Event::TheEvent()->m_BacktrackBuffer[i].comPlayer,
		    Control::GetComPlayer() );

    Event::TheEvent()->m_BacktrackBuffer[i].score1 = 0;
    Event::TheEvent()->m_BacktrackBuffer[i].score2 = 0;
  }
}

/**
 * Close everything and send quit message. 
 * This method is called when the game player push 'Q' key. 
 * This method closes everything and send SDL_QUIT message. 
 */
void
QuitGame() {
  printf( _("Avg = %f\n"), (double)perfs/_perfCount );
  if (_backTrackCount) printf( _("BackTrack = %f\n"), backTracks/_backTrackCount);

  Event::TheEvent()->m_lastTime.time = 0;
  _perfCount = 0;
  perfs = 0;
  backTracks = 0;
  _backTrackCount = 0;

  close(theSocket);
  theSocket = -1;

  SDL_WM_GrabInput( SDL_GRAB_OFF );

  SDL_Event e;
  e.type = SDL_QUIT;
  SDL_PushEvent( &e );
}

/**
 * Move mouse pointer. 
 * During playing or selecting player, this method moves mouse pointer toward
 * the center of the screen. 
 * 
 * @param x x-coordinate of the mouse pointer. 
 * @param y y-coordinate of the mouse pointer. 
 */
void
Event::SetNextMousePointer( long &x, long &y ) {
  if ( mode == MODE_TITLE ) {
    return;
  } else if ( mode == MODE_SELECT || mode == MODE_TRAININGSELECT ||
	      mode == MODE_PRACTICESELECT || mode == MODE_MULTIPLAYSELECT ) {
    x = BaseView::GetWinWidth()/2 + (x-BaseView::GetWinWidth()/2)*15/16;
  } else {
    x = BaseView::GetWinWidth()/2 + (x-BaseView::GetWinWidth()/2)*15/16;
    y = BaseView::GetWinHeight()/2 + (y-BaseView::GetWinHeight()/2)*15/16;
  }
}

#ifdef LOGGING
/**
 * Get time adjusted with the opponent machine. 
 * 
 * @param sec second
 * @param cnt count (1/100 second)
 */
void
Event::GetAdjustedTime( long &sec, long &cnt ) {
  if ( mode == MODE_MULTIPLAY )
    cnt += timeAdj;

  while ( cnt < 0 ) {
    sec--;
    cnt += 100;
  }
  while ( cnt >= 100 ) {
    sec++;
    cnt -= 100;
  }
}

/**
 * Output remaining backtrack buffer to the log file. 
 */
void
Event::RemainingLog() {
  for ( int i = 0 ; i < MAX_HISTORY ; i++ ) {
    m_Histptr++;
    if ( m_Histptr == MAX_HISTORY )
      m_Histptr = 0;

    logRecord();
  }
}

void
Event::logRecord() {
  long sec = m_BacktrackBuffer[m_Histptr].sec;
  long cnt = m_BacktrackBuffer[m_Histptr].count;
  char buf[1024];

  if (m_BacktrackBuffer[m_Histptr].theBall.GetStatus() == -1000)
    return;	// not valid log. 

  if ( mode == MODE_MULTIPLAY )
    cnt += timeAdj;

  while ( cnt < 0 ) {
    sec--;
    cnt += (int)(1/TICK);
  }
  while ( cnt >= (int)(1/TICK) ) {
    sec++;
    cnt -= (int)(1/TICK);
  }

  struct timeb tb;
  tb.time = sec;
  tb.millitm = cnt*(TICK*1000);

  Logging::GetLogging()->LogTime(LOG_ACTBALL, &tb);
  Logging::GetLogging()->LogScore(LOG_ACTBALL, 
				  m_BacktrackBuffer[m_Histptr].score1, 
				  m_BacktrackBuffer[m_Histptr].score2);
  Logging::GetLogging()->LogBall(LOG_ACTBALL,
				 &m_BacktrackBuffer[m_Histptr].theBall);


  Player *player;
  player = &m_BacktrackBuffer[m_Histptr].thePlayer;

  Logging::GetLogging()->LogTime(LOG_ACTTHEPLAYER, &tb);
  Logging::GetLogging()->LogPlayer(LOG_ACTTHEPLAYER, player);

  player = &m_BacktrackBuffer[m_Histptr].comPlayer;

  Logging::GetLogging()->LogTime(LOG_ACTCOMPLAYER, &tb);
  Logging::GetLogging()->LogPlayer(LOG_ACTCOMPLAYER, player);

  Logging::GetLogging()->LogTime(LOG_CAMERA, &tb);
  Logging::GetLogging()->LogCamera(m_BacktrackBuffer[m_Histptr].camera, 
				   m_BacktrackBuffer[m_Histptr].target);
}
#endif

/**
 * Get new ExternalData object and add it to ExternalData list. 
 * This method is equivalent with GetExternal( m_External, side );
 * 
 * @param side m_side of player. 
 * @return returns true if new ExternalData is added. 
 */
bool
Event::GetExternalData( long side ) {
  return GetExternalData( m_External, side );
}
