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

extern Player* thePlayer;
extern Player* comPlayer;
extern Ball theBall;
extern Event theEvent;

extern void CopyPlayerData( struct PlayerData& dest, Player* src );

SoloPlay::SoloPlay() {
  m_smash = false;
  m_smashCount = 0;
  m_smashPtr = -1;
}

SoloPlay::~SoloPlay() {
}

bool
SoloPlay::Init() {
  return true;
}

SoloPlay*
SoloPlay::Create( long player, long com ) {
  SoloPlay *newSoloPlay;

  Event::ClearObject();

  newSoloPlay = new SoloPlay();
  newSoloPlay->Init();

  thePlayer = Player::Create( player, 1, 0 );
  comPlayer = Player::Create( com, -1, 1 );

  thePlayer->Init();
  comPlayer->Init();

  // View, $B$+(B?
  glutSetCursor( GLUT_CURSOR_NONE );

  return newSoloPlay;
}

bool
SoloPlay::Move( unsigned long *KeyHistory, long *MouseXHistory,
		    long *MouseYHistory, unsigned long *MouseBHistory,
		    int Histptr ) {
  bool reDraw = false;
  long prevStatus = theBall.GetStatus();

  if ( m_smashPtr >= 0 ) {	// $B%9%^%C%7%e%j%W%l%$(B
    if ( Histptr == m_smashPtr ) {
      //theEvent.SmashEffect(false);
      SmashEffect(false, Histptr);
      m_smashCount++;
      if ( m_smashCount > 2 ) {
	m_smashCount = 0;
	m_smashPtr = -1;
	m_smash = false;
      } else {
	//m_smashPtr = theEvent.SmashEffect(true);
	m_smashPtr = SmashEffect(true, Histptr);
      }
    } else {
      theBall.Move();
      thePlayer->Move( KeyHistory, MouseXHistory,
		       MouseYHistory, MouseBHistory, Histptr );
      theEvent.BackTrack(Histptr);
    }

    return true;
  }

  theBall.Move();
  reDraw |= thePlayer->Move( KeyHistory, MouseXHistory,
			     MouseYHistory, MouseBHistory, Histptr );
  reDraw |= comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

  if ( theBall.GetStatus() == 1 && prevStatus != 1 ) {
    if ( hypot( theBall.GetVY(), theBall.GetVZ() ) > 8.0 ) {
      m_smash = true;
    } else
      m_smash = false;
  } else if ( theBall.GetStatus() == 2 || theBall.GetStatus() == 8 ) {
    m_smash = false;
  }

  if ( m_smash && theBall.GetStatus() < 0 )
    //m_smashPtr = theEvent.SmashEffect(true);
    m_smashPtr = SmashEffect(true, Histptr);

  return true;
}

bool
SoloPlay::LookAt( double &srcX, double &srcY, double &srcZ,
		  double &destX, double &destY, double &destZ ) {
  if (thePlayer) {
    if ( m_smashPtr >= 0 ) {	// $B%9%^%C%7%e%j%W%l%$(B
      switch (m_smashCount) {
      case 0:
	srcX = 0;
	srcY = TABLELENGTH*thePlayer->GetSide();
	srcZ = 1.6;

	destX = 0;
	destY = -TABLELENGTH/2*thePlayer->GetSide();
	destZ = TABLEHEIGHT;
	break;
      case 1:
	srcX = -TABLEWIDTH;
	srcY = 0;
	srcZ = 1.6;

	destX = theBall.GetX();
	destY = theBall.GetY();
	destZ = theBall.GetZ();
	break;
      case 2:
	srcX = thePlayer->GetX() + thePlayer->GetEyeX();
	srcY = thePlayer->GetY() + thePlayer->GetEyeY();
	srcZ = thePlayer->GetZ() + thePlayer->GetEyeZ();
	destX = thePlayer->GetLookAtX();
	destY = thePlayer->GetLookAtY();
	destZ = thePlayer->GetLookAtZ();
	break;
      default:
	printf( "%d\n", (int)m_smashCount );
      }
    } else {
      srcX = thePlayer->GetX() + thePlayer->GetEyeX();
      srcY = thePlayer->GetY() + thePlayer->GetEyeY();
      srcZ = thePlayer->GetZ() + thePlayer->GetEyeZ();
      destX = thePlayer->GetLookAtX();
      destY = thePlayer->GetLookAtY();
      destZ = thePlayer->GetLookAtZ();
    }
  }

  return true;
}

long
SoloPlay::SmashEffect( bool start, long histPtr ) {
  static struct PlayerData p1, p2;
  static Ball b;
  long smashPtr;

  smashPtr = histPtr-1;
  if ( smashPtr < 0 )
    smashPtr = MAX_HISTORY;

  if (start) {
    CopyPlayerData( p1, thePlayer );
    CopyPlayerData( p2, comPlayer );
    b = theBall;

    for ( int i = 0 ; i < MAX_HISTORY ; i++ ) {
      theEvent.BackTrack( histPtr );
      if ( theBall.GetStatus() == 2 )
	break;

      histPtr--;
      if ( histPtr < 0 )
	histPtr = MAX_HISTORY;
    }

    theEvent.BackTrack( histPtr );
  } else {
    thePlayer->Reset( &p1 );
    comPlayer->Reset( &p2 );
    theBall = b;
  }

  return smashPtr;
}
