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

#ifndef _Event_
#define _Event_

#include "Ball.h"
#include "Player.h"

struct Backtrack {
  long sec;
  char count;
  Ball theBall;
  Player thePlayer;
  Player comPlayer;
  long score1;
  long score2;
};

class ExternalData;

class Event {
public:
  Event();
  ~Event();

  bool Init();

  static void IdleFunc();
  static void KeyboardFunc( SDL_Event key, int x, int y );
  static void KeyUpFunc( SDL_Event key, int x, int y );
  static void MotionFunc( int x, int y );
  static void ButtonFunc( int button, int state, int x, int y );

  bool SendSwing( Player *player );
  bool SendPlayer( Player *player );
  bool SendBall();
  bool SendPlayerAndBall( Player *player );

  static void ClearObject();

  bool BackTrack( long Histptr );

  static struct timeb m_lastTime;

  long m_mouseButton;					// For skip replay
protected:
  bool Move();
  bool IsModeChanged( long preMode );
  void Record();
  void ReadData();

  void PlayInit( long player, long com );
  void DemoInit();
  void SelectInit();
  void TrainingInit( long player, long com );
  void TrainingSelectInit();

  bool GetExternalData( ExternalData *&ext, long side );

  static void ClearBacktrack();

  unsigned long m_KeyHistory[MAX_HISTORY];	// History of key input
  long m_MouseXHistory[MAX_HISTORY];		// History of mouse move
  long m_MouseYHistory[MAX_HISTORY];		// History of mouse move
  unsigned long m_MouseBHistory[MAX_HISTORY];	// History of mouse button
  int  m_Histptr;

  ExternalData *m_External;
  struct Backtrack m_BacktrackBuffer[MAX_HISTORY];
  bool m_backtrack;		// Now backtracking or not
};

#endif	// _Event_
