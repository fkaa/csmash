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

extern Ball   theBall;
extern Player *thePlayer;
extern Player *comPlayer;

ComPenDrive::ComPenDrive() {
  ::PenDrive();

  _prevBallstatus = 0;		// $B0JA0$N%\!<%k$N>uBV(B
  _hitX = 0;			// $BBG5eE@(B
  _hitY = -TABLELENGTH/3;
}

ComPenDrive::ComPenDrive(long side) {
  ::ComPenDrive();

  if ( side < 0 ) {
    m_side = -1;
    m_y = -m_y;
    m_targetY = -m_targetY;
    _hitY = -_hitY;
  }
}

ComPenDrive::~ComPenDrive() {
}

bool
ComPenDrive::Move( unsigned long *KeyHistory, long *MouseXHistory,
		 long *MouseYHistory, unsigned long *MouseBHistory,
		 int Histptr ) {

  PenDrive::Move( KeyHistory, MouseXHistory, MouseYHistory, MouseBHistory,
		Histptr );
  Think();

  return true;
}

bool
ComPenDrive::Think() {
  double hitTX, hitTY;		// _hitX, _hitY$B$X$NM=A[E~C#;~4V(B
  double mx;

  // $B%\!<%k$N>uBV$,JQ$o$C$F$$$?$i(B, $B0\F0@h$r9M$(D>$9(B
  if ( _prevBallstatus != theBall.GetStatus() && m_swing == 0 ){
    Hitarea( _hitX, _hitY );

    _prevBallstatus = theBall.GetStatus();
  }

  if ( theBall.GetVX() != 0.0 )
    hitTX = (_hitX - theBall.GetX())/theBall.GetVX();
  else
    hitTX = -1.0;

  if ( theBall.GetVY() != 0.0 )
    hitTY = (_hitY - theBall.GetY())/theBall.GetVY();
  else
    hitTY = -1.0;

//  if ( fabs( _hitX-(m_x+m_side*0.3) ) < fabs( _hitX-(m_x-m_side*0.3) ) ||
//       theBall.GetStatus() == 8 || _hitX*m_side > 0 )
  mx = m_x+m_side*0.3;

  if ( hitTX > 0.0 ) {
    if ( m_vx > 0 && mx + m_vx*hitTX < _hitX )
      m_vx += 0.1;
    else if ( m_vx < 0 && mx + m_vx*hitTX > _hitX )
      m_vx -= 0.1;
    else if ( m_vx*fabs(m_vx*0.1)/2 < _hitX - mx )
      m_vx += 0.1;
    else
      m_vx -= 0.1;
  } else {
    if ( m_vx*fabs(m_vx*0.1)/2 < _hitX - mx )
      m_vx += 0.1;
    else
      m_vx -= 0.1;
  }

  if ( hitTY > 0.0 ) {
    if ( m_vy > 0 && m_y + m_vy*hitTY < _hitY )
      m_vy += 0.1;
    else if ( m_vy < 0 && m_y + m_vy*hitTY > _hitY )
      m_vy -= 0.1;
    else if ( m_vy*fabs(m_vy*0.1)/2 < _hitY - m_y )
      m_vy += 0.1;
    else
      m_vy -= 0.1;
  } else {
    if ( m_vy*fabs(m_vy*0.1)/2 < _hitY - m_y )
      m_vy += 0.1;
    else
      m_vy -= 0.1;
  }

  if ( m_vx > 5.0 )
    m_vx = 5.0;
  else if ( m_vx < -5.0 )
    m_vx = -5.0;
  if ( m_vy > 5.0 )
    m_vy = 5.0;
  else if ( m_vy < -5.0 )
    m_vy = -5.0;

// $B%H%9(B
  if ( theBall.GetStatus() == 8 &&
       theBall.GetService() == GetSide() &&
       fabs(m_vx) < 0.1 && fabs(m_vy) < 0.1 &&
       fabs(m_x+m_side*0.3-_hitX) < 0.1 && fabs(m_y-_hitY) < 0.1 &&
       m_swing == 0 ){
    theBall.Toss( this, 2 );
    StartSwing( 3, m_spin );
    m_targetY = TABLELENGTH/8*m_side;

    return true;
  }

  if ( fabs( theBall.GetY()+theBall.GetVY()*0.1 - _hitY ) < 0.2 /*&&
	  m_swing == 0*/ ){
    // 0.1$BIC8e$N%\!<%k$N0LCV$r?dDj$9$k(B. Swing$B$HFs=E$K$J$k$N$G(B
    // $BMW:F9M(B. 
    Ball *tmpBall;

    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );

    for ( int i = 0 ; i < 10 ; i++ )
      tmpBall->Move();

    if ( ((tmpBall->GetStatus() == 3 && m_side == 1) ||
	  (tmpBall->GetStatus() == 1 && m_side == -1)) &&
	 (m_y-tmpBall->GetY())*m_side < 0.3 &&
	 (m_y-tmpBall->GetY())*m_side > -0.6 && RAND(3) == 0 ){
      _hitX = tmpBall->GetX();
      _hitY = tmpBall->GetY();

      Player *opponent;
      if ( m_side == -1 )
	opponent = thePlayer;
      else
	opponent = comPlayer;

      if ( opponent->GetPlayerType() == PLAYER_PENDRIVE ) {
	switch ( RAND(4) ) {
	case 0:
	  m_targetX = -TABLEWIDTH*7/16;
	  break;
	case 1:
	  m_targetX = -TABLEWIDTH*5/16;
	  break;
	case 2:
	  m_targetX = -TABLEWIDTH*3/16;
	  break;
	case 3:
	  m_targetX = -TABLEWIDTH*1/16;
	break;
	}
      } else {
	switch ( RAND(8) ) {
	case 0:
	  m_targetX = -TABLEWIDTH*7/16;
	  break;
	case 1:
	  m_targetX = TABLEWIDTH*5/16;
	  break;
	case 2:
	  m_targetX = -TABLEWIDTH*3/16;
	  break;
	case 3:
	  m_targetX = -TABLEWIDTH*1/16;
	  break;
	case 4:
	  m_targetX = TABLEWIDTH*1/16;
	  break;
	case 5:
	  m_targetX = TABLEWIDTH*3/16;
	  break;
	case 6:
	  m_targetX = TABLEWIDTH*5/16;
	  break;
	case 7:
	  m_targetX = TABLEWIDTH*7/16;
	  break;
	}
      }

      if ( m_vx > 1.5 ) {
	m_targetX += TABLEWIDTH/2;
      } else if ( m_vx > 0.5 ) {
	m_targetX += TABLEWIDTH/4;
      } else if ( m_vx < -1.5 ) {
	m_targetX -= TABLEWIDTH/2;
      } else if ( m_vx < -0.5 ) {
	m_targetX -= TABLEWIDTH/4;
      }

      if ( m_targetX > TABLEWIDTH/2 )
	m_targetX = TABLEWIDTH*7/16;
      if ( m_targetX < -TABLEWIDTH/2 )
	m_targetX = -TABLEWIDTH*7/16;

      if ( (tmpBall->GetZ()-TABLEHEIGHT)/fabs(m_y - m_targetY) < 0.0 ) {
	m_targetY = TABLELENGTH/6*m_side;
	Swing( 1, 0.0 );
      } else if ( (tmpBall->GetZ()-TABLEHEIGHT)/fabs(m_y-m_targetY) < 0.1 ) {
	m_targetY = TABLELENGTH/4*m_side;
	Swing( 2, 0.0 );
      } else {
	m_targetY = TABLELENGTH/16*6*m_side;
	Swing( 3, 0.0 );
      }

      delete tmpBall;
    }
  }

  return true;
}

// $BBG5eE@$r7W;;$9$k(B
//(1) $B$^$@%\!<%k$,#1%P%&%s%I$9$kA0$@$C$?$i!"%P%&%s%I$9$k0LCV$r5a$a$k(B
//(2) $B8=:_$N%\!<%k$N0LCV>pJs$+%P%&%s%I$9$k>l=j$N>pJs$+$iBG5eE@$r7h$a$k(B
bool
ComPenDrive::Hitarea( double &hitX, double &hitY ) {
  Ball *tmpBall;
  double max = -1.0;             /* $B%\!<%k$N:G9bE@(B */
  double maxX = 0.0, maxY = 0.0;

  if ( (theBall.GetStatus() == 3 && m_side == 1) ||
       (theBall.GetStatus() == 2 && m_side == 1) ||
       (theBall.GetStatus() == 0 && m_side == -1) ||
       (theBall.GetStatus() == 1 && m_side == -1) ||
       (theBall.GetStatus() == 4 && m_side == -1) ||
       (theBall.GetStatus() == 5 && m_side == 1) ) {
    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );

    while ( tmpBall->GetStatus() != -1 ) {
      if ( (tmpBall->GetStatus() == 3 && m_side == 1) ||
	   (tmpBall->GetStatus() == 1 && m_side == -1) ) {
	if ( tmpBall->GetZ() > max &&
	     fabs(tmpBall->GetY()) < TABLELENGTH/2+1.0 ) {
	  max = tmpBall->GetZ();
	  maxX = tmpBall->GetX();
	  maxY = tmpBall->GetY();
	}
      }
      tmpBall->Move();
    }

    delete tmpBall;

    if ( max > 0 ) {
      hitX = maxX;
      hitY = maxY;
    }
  } else if ( theBall.GetStatus() == 8 ) {
    if ( theBall.GetService() == GetSide() ) {
    if ( RAND(2) )
      hitX = m_targetX;
    else
      hitX = -m_targetX;
    } else
      hitX = 0.0;
    hitY = -(TABLELENGTH/2+0.2)*m_side;
  }
    return true;
}