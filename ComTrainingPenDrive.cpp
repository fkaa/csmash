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
#include "ComTrainingPenDrive.h"
#include "Ball.h"
#include "Player.h"
#include "Control.h"
#include "PlayGame.h"
#include "Training.h"

extern Ball   theBall;
extern Player *thePlayer;
extern Player *comPlayer;

extern Control *theControl;

extern long    gameLevel;

ComTrainingPenDrive::ComTrainingPenDrive() : ComPenDrive() {
}

ComTrainingPenDrive::ComTrainingPenDrive(long side) :
  ComPenDrive(side) {
}

ComTrainingPenDrive::ComTrainingPenDrive( long playerType, long side,
					  double x, double y, double z,
					  double vx, double vy, double vz,
					  long status, long swing,
					  long swingType, bool swingSide,
					  long afterSwing, long swingError,
					  double targetX, double targetY,
					  double eyeX, double eyeY,
					  double eyeZ,
					  long pow, double spin,
					  double stamina, long statusMax ) :
  ComPenDrive( playerType, side, x, y, z, vx, vy, vz, status, swing, swingType,
	       swingSide, afterSwing, swingError, targetX, targetY,
	       eyeX, eyeY, eyeZ, pow, spin, stamina, statusMax ) {
}

ComTrainingPenDrive::~ComTrainingPenDrive() {
}

bool
ComTrainingPenDrive::Think() {
  double hitTX, hitTY;	// estimation time until ball reaches _hitX, _hitY
  double mx;

  // If the ball status changes, change _hitX, _hitY
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

  if ( fabs( _hitX-(m_x+m_side*0.3) ) < fabs( _hitX-(m_x-m_side*0.3) ) ||
       theBall.GetStatus() == 8 || _hitX*m_side > 0 )
    mx = m_x+m_side*0.3;
  else
    mx = m_x-m_side*0.3;

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

  // Toss
  if ( theBall.GetStatus() == 8 &&
       ((PlayGame *)theControl)->GetService() == GetSide() &&
       fabs(m_vx) < 0.1 && fabs(m_vy) < 0.1 &&
       fabs(m_x+m_side*0.3-_hitX) < 0.1 && fabs(m_y-_hitY) < 0.1 &&
       m_swing == 0 ){
    theBall.Toss( this, 2 );
    StartSwing( 3 );
    m_targetY = TABLELENGTH/8*m_side;

    return true;
  }

  // Calc the ball location of 0.1 second later. 
  // This part seems to be the same as Swing(). Consider again. 
  Ball *tmpBall;

  tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
		      theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
		      theBall.GetSpin(), theBall.GetStatus() );

  for ( int i = 0 ; i < 10 ; i++ )
    tmpBall->Move();

  if ( ((tmpBall->GetStatus() == 3 && m_side == 1) ||
	(tmpBall->GetStatus() == 1 && m_side == -1)) &&
       (m_y-tmpBall->GetY())*m_side < 0.3 &&
       (m_y-tmpBall->GetY())*m_side > 0.0 ){
    _hitX = tmpBall->GetX();
    _hitY = tmpBall->GetY();

    Player *opponent;
    if ( m_side == -1 )
      opponent = thePlayer;
    else
      opponent = comPlayer;

    SetTargetX( opponent );

    Swing( 3 );
  }
  delete tmpBall;

  return true;
}

bool
ComTrainingPenDrive::HitBall() {
  double vx, vy, vz;
  double level;

  // Serve
  if ( ( (m_side == 1 && theBall.GetStatus() == 6) ||
         (m_side ==-1 && theBall.GetStatus() == 7) ) &&
       fabs( m_x-theBall.GetX() ) < 0.6 && fabs( m_y-theBall.GetY() ) < 0.3 ){
    PenDrive::HitBall();
  } else {
    if ( ((m_side == 1 && theBall.GetStatus() == 3) ||
	  (m_side ==-1 && theBall.GetStatus() == 1)) ) {
      ((Training *)theControl)->AddTrainingCount();

      level = 1.0 - 1.0/((double)((Training *)theControl)->GetTrainingCount()/20.0+1.5);
      theBall.TargetToV( -TABLEWIDTH/5*2+TABLEWIDTH/5*4*(((Training *)theControl)->GetTrainingCount()%2),
			 TABLELENGTH/5*2*m_side, level, m_spin,
			 vx, vy, vz, 0.1, 20.0 );

      theBall.Hit( vx, vy, vz, m_spin, this );
    } else
      m_swingError = SWING_MISS;
  }

  return true;
}
