/* $Id$ */

// Copyright (C) 2000-2004  $B?@Fn(B $B5H9((B(Kanna Yoshihiro)
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
#include "Ball.h"
#include "BaseView.h"
#include "Control.h"
#include "PlayGame.h"
#include "Sound.h"
#include "Event.h"
#include "Network.h"
#include "RCFile.h"
#include "LobbyClient.h"

#ifdef LOGGING
#include "Logging.h"
#endif

extern RCFile *theRC;

extern Ball theBall;
extern long mode;

#if 0
inline double LOG(double f) { return log(f); }
#else
inline double LOG(double f)
{
    if (f <= 0) {
	printf("log(%g)!\n", f);
	return -10e10;
    } else {
	return log(f);
    }
}
#endif

Ball::Ball() : m_x(0.0), m_v(0.0), m_spin(0.0) {
  m_status = -1000;

  m_View = NULL;

  m_lastSendCount = 0;
}

Ball::Ball( const vector3d _x, const vector3d _v, const vector2d _spin, long _status ) {
  m_x = _x;
  m_v = _v;
  m_spin = _spin;
  m_status = _status;

  m_View = NULL;

  m_lastSendCount = 0;
}

Ball::Ball( Ball *ball ) {
  m_x = ball->m_x;
  m_v = ball->m_v;
  m_spin = ball->m_spin;
  m_status = ball->m_status;

  m_View = NULL;

  m_lastSendCount = 0;
}


Ball::~Ball() {
  if ( m_View && &theBall == this ){
    BaseView::TheView()->RemoveView( m_View );
    delete m_View;
  }
}

bool
Ball::Init() {
  m_View = (BallView *)View::CreateView( VIEW_BALL );

  m_View->Init();
  BaseView::TheView()->AddView( m_View );

  return true;
}

bool
Ball::Move() {
  double netT , tableT;        /* Flag for bound on the table, hit net */
  vector3d x;                  /* Hold x,y */
  double tableY;               /* Hold y on bounding */

// Return ball immidiately when ball dead
  if ( m_status < 0 )
    m_status--;

  if ( m_status < -100 || m_status == 8 ){
    if ( Control::TheControl()->IsPlaying() ) {
      Player *player;

      if ( ((PlayGame *)Control::TheControl())->GetService() == Control::TheControl()->GetThePlayer()->GetSide() )
	player = Control::TheControl()->GetThePlayer();
      else
	player = Control::TheControl()->GetComPlayer();

      if ( ((PlayGame *)Control::TheControl())->GetService() > 0 ) {
	m_x[0] = player->GetX()[0]+0.3;
	m_x[1] = player->GetX()[1];
      } else {
	m_x[0] = player->GetX()[0]-0.3;
	m_x[1] = player->GetX()[1];
      }

      m_x[2] = TABLEHEIGHT + 0.15;
      m_v[0] = m_v[1] = m_v[2] = 0.0;

      m_status = 8;

      if ( &theBall == this &&
	   ((PlayGame *)Control::TheControl())->IsGameEnd() == true ){
	BaseView::TheView()->EndGame();
	((PlayGame *)Control::TheControl())->EndGame();
      }

      // To Fix the possibility of score mismatch
      if ( mode == MODE_MULTIPLAY && &theBall == this &&
	   Control::TheControl()->GetThePlayer()->GetSide() > 0 ) {
	m_lastSendCount++;
	if ( m_lastSendCount >= 100 ) {
	  Event::TheEvent()->SendBall();
	  m_lastSendCount = 0;
	}
      }
    } else {
      m_x[0] = Control::TheControl()->GetThePlayer()->GetX()[0]+0.3;
      m_x[1] = Control::TheControl()->GetThePlayer()->GetX()[1];

      m_x[2] = TABLEHEIGHT + 0.15;
      m_v[0] = m_v[1] = m_v[2] = 0.0;

      m_status = 8;
    }

    return true;
  }

/* Add velocity */ 
  x = m_x;

  m_x[0] += (m_v[0]*2-PHY*m_v[0]*TICK)/2*TICK;
  m_x[1] += (m_v[1]*2-PHY*m_v[1]*TICK)/2*TICK;
  m_x[2] += (m_v[2]*2-GRAVITY(m_spin[1])*TICK-PHY*m_v[2]*TICK)/2*TICK;

/* Collision check */
  if ( x[1]*m_x[1] <= 0.0 ){
    netT = fabs( x[1]/((m_x[1]-x[1])/TICK) );
    if ( x[2]+(m_x[2]-x[2])*netT/TICK < TABLEHEIGHT ||
	 x[2]+(m_x[2]-x[2])*netT/TICK > TABLEHEIGHT+NETHEIGHT ||
	 x[0]+(m_x[0]-x[0])*netT/TICK < -TABLEWIDTH/2-NETHEIGHT ||
	 x[0]+(m_x[0]-x[0])*netT/TICK > TABLEWIDTH/2+NETHEIGHT )
      netT = TICK*100;
  } else
    netT = TICK*100;

  if ( (x[2]-TABLEHEIGHT)*(m_x[2]-TABLEHEIGHT) <= 0.0 ){
    tableT = fabs( (x[2]-TABLEHEIGHT)/((m_x[2]-x[2])/TICK) );
    if ( tableT <= 0.0 || x[1]+(m_x[1]-x[1])*tableT/TICK < -TABLELENGTH/2 ||
	 x[1]+(m_x[1]-x[1])*tableT/TICK > TABLELENGTH/2 ||
	 x[0]+(m_x[0]-x[0])*tableT/TICK < -TABLEWIDTH/2 ||
	 x[0]+(m_x[0]-x[0])*tableT/TICK > TABLEWIDTH/2 )
      tableT = TICK*100;
  } else
    tableT = TICK*100;

  if ( netT < tableT ){	// Hit net
    m_v[0] *= 0.5;
    m_v[1] *= -0.2;
    m_spin *= -0.8;

    m_x[1] = m_v[1]*(TICK-netT);
  }

  if ( tableT < netT ){	// Bound on the table
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x );
    }
    tableY = x[1]+m_v[1]*tableT;
    if ( tableY < 0 ){		// Table of my side
      switch( m_status ){
      case 2:
	m_status = 3;
	break;
      case 4:
	m_status = 0;
	break;
      default:
	BallDead();
      }
    } else {			// Table of opponent side
      switch( m_status ) {
      case 0:
	m_status = 1;
	break;
      case 5:
	m_status = 2;
	break;
      default:
	BallDead();
      }
    }

    m_v[2] -= GRAVITY(m_spin[1])*tableT;
    m_v[2] += -PHY*m_v[2]*tableT;
    m_v[2] *= -TABLE_E;
    m_x[2] = TABLEHEIGHT + (TICK-tableT)*m_v[2];
    m_v[2] -= GRAVITY(m_spin[1])*(TICK-tableT);
    m_v[2] += -PHY*m_v[2]*(TICK-tableT);

    m_v[1] += -PHY*m_v[1]*tableT;

    if ( m_v[1] > 0 )
      m_v[1] += m_spin[1]*0.8;
    else
      m_v[1] -= m_spin[1]*0.8;

    m_v[1] += -PHY*m_v[1]*(TICK-tableT);
    m_v[0] += -PHY*m_v[0]*TICK;

    m_spin[0] *= 0.95;
    m_spin[1] *= 0.8;

    return true;
  }

/* Collision check with walls */
  if ( m_x[0] < -AREAXSIZE/2 ){
    m_x[0] = -AREAXSIZE/2;
    m_v[0] = -m_v[0]*TABLE_E/2;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x );
    }
    BallDead();
  }
  else if ( m_x[0] > AREAXSIZE/2 ){
    m_x[0] = AREAXSIZE/2;
    m_v[0] = -m_v[0]*TABLE_E/2;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x );
    }
    BallDead();
  }

  if ( m_x[1] < -AREAYSIZE/2 ){
    m_x[1] = -AREAYSIZE/2;
    m_v[1] = -m_v[1]*TABLE_E/2;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x );
    }
    BallDead();
  }
  else if ( m_x[1] > AREAYSIZE/2 ){
    m_x[1] = AREAYSIZE/2;
    m_v[1] = -m_v[1]*TABLE_E/2;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x );
    }
    BallDead();
  }

  if ( m_x[2] < 0 ){
    m_x[2] = 0;
    m_v[2] = -m_v[2]*TABLE_E;
    if ( m_v[1] > 0 )
      m_v[1] += m_spin[1]*0.8;
    else
      m_v[1] -= m_spin[1]*0.8;

    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x );
    }
    BallDead();
  }
  else if ( m_x[2] > AREAZSIZE ){
    m_x[2] = AREAZSIZE;
    m_v[2] = -m_v[2]*0.1;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x );
    }
    BallDead();
  }

// Gravity
  m_v[2] -= GRAVITY(m_spin[1])*TICK;

// Spin
  //TODO: apply spinX
  /*
  double rotVx = m_vx*cos(m_spinX)-m_vy*sin(m_spinX);
  double rotVy = m_vx*sin(m_spinX)+m_vy*cos(m_spinX);

  m_vx = rotVx; m_vy = rotVy;
  */

// Air resistance
  m_v = m_v - PHY*TICK*m_v;

  return true;
}

// Ball inpact
bool
Ball::Hit( const vector3d v, const vector2d spin, Player *player ) {
// Normal inpact
  if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_RACKET, m_x );
  }

  m_spin = spin;

  m_v = v;

  // m_status should be updated by the information from the opponent
  if ( player == Control::TheControl()->GetComPlayer() &&
       mode == MODE_MULTIPLAY ) {
    return true;
  }

  if ( m_status == 6 )
    m_status = 4;
  else if ( m_status == 7 )
    m_status = 5;
  else if ( m_status == 3 )
    m_status = 0;
  else if ( m_status == 1 ) {
    m_status = 2;
  }

  if ( player == Control::TheControl()->GetThePlayer() &&
       mode == MODE_MULTIPLAY && &theBall == this ) {
    Event::TheEvent()->SendBall();
  }

  return true;
}

bool
Ball::Toss( Player *player , long power ) {
  m_v[2] = 2.5;
  m_spin[0] = m_spin[1] = 0.0;

  if ( player->GetSide() > 0 )
    m_status = 6;
  else
    m_status = 7;

  if ( player == Control::TheControl()->GetThePlayer() &&
       mode == MODE_MULTIPLAY && &theBall == this ) {
    Event::TheEvent()->SendBall();
  }

  return true;
}

void
Ball::Warp( const vector3d x, const vector3d v, const vector2d spin, long status ) {
  m_x = x;
  m_v = v;
  m_spin = spin;

  m_status = status;
}

void
Ball::Warp( char *buf ) {
  char *b = buf;
  long nextStatus;

  for ( int i = 0 ; i < 3 ; i++ )
    b = ReadDouble( b, m_x[i] );

  for ( int i = 0 ; i < 3 ; i++ )
    b = ReadDouble( b, m_v[i] );

  for ( int i = 0 ; i < 2 ; i++ )
    b = ReadDouble( b, m_spin[i] );

  b = ReadLong( b, nextStatus );

  if ( m_status >= 0 && nextStatus < 0 )
    BallDead();
  else
    m_status = nextStatus;
}

// Get ball initial speed from current ball location, target, height
// target --- The point where ball will bound
// level ---  hit power(percentage)
// TODO: apply m_spinX
bool
Ball::TargetToV( vector2d target, double level, const vector2d spin, 
		 vector3d &v, double vMin, double vMax ) {
  double y;
  double vyMin = 0.1, vyMax;
  double t1, t2, z1;

  vyMax = fabs(target[1]-m_x[1])/hypot(target[0]-m_x[0], target[1]-m_x[1])*vMax;

  if ( target[1] < m_x[1] ){
    y = -m_x[1];
    target[1] = -target[1];
  } else
    y = m_x[1];

  if ( target[1]*y >= 0 ) {	// Never go over the net
    v[1] = vyMax*level*0.5;

    // t2 = time until ball reaches targetY
    t2 = -LOG( 1- PHY*(target[1]-y)/v[1] ) / PHY;

    // define vz which satisfy z=TABLEHEIGHT when t=t2
    if (0 != t2) {
	v[2] = (PHY*(TABLEHEIGHT-m_x[2])+GRAVITY(spin[1])*t2)/(1-exp(-PHY*t2)) -
	    GRAVITY(spin[1])/PHY;
	v[0] = PHY*(target[0]-m_x[0]) / (1-exp(-PHY*t2));
    } else {
	v[2] = m_x[2];
	v[0] = m_x[0];
    }


    if ( y != m_x[1] )
      v[1] = -v[1];

    return true;
  }

  while (vyMax-vyMin > 0.001) {
    v[1] = (vyMin+vyMax)/2;

    // t2 = time until ball reaches targetY
    t2 = -LOG( 1- PHY*(target[1]-y)/v[1] ) / PHY;

    // t1 = time until ball reaches the net
    t1 = -LOG( 1- PHY*(-y)/v[1] ) / PHY;

    // define vz which satisfy z=TABLEHEIGHT when t=t2
    if (0 != t2) {
	v[2] = (PHY*(TABLEHEIGHT-m_x[2])+GRAVITY(spin[1])*t2)/(1-exp(-PHY*t2)) -
	  GRAVITY(spin[1])/PHY;
    } else {
	v[2] = m_x[2];
    }

    // z1 = height of the ball when t=t2
    z1 = -(v[2]+GRAVITY(spin[1])/PHY)*exp(-PHY*t1)/PHY - GRAVITY(spin[1])*t1/PHY +
      (v[2]+GRAVITY(spin[1])/PHY)/PHY;

    if ( z1 < TABLEHEIGHT+NETHEIGHT-m_x[2] )
      vyMax = v[1];
    else
      vyMin = v[1];
  }

  v[1] *= level;

  t2 = -LOG( 1- PHY*(target[1]-y)/v[1] ) / PHY;
  if (0 != t2) {
      v[2] = (PHY*(TABLEHEIGHT-m_x[2])+GRAVITY(spin[1])*t2)/(1-exp(-PHY*t2)) -
	GRAVITY(spin[1])/PHY;
  } else {
      v[2] = m_x[2];
  }

  if ( y != m_x[1] )
    v[1] = -v[1];

  v[0] = PHY*(target[0]-m_x[0]) / (1-exp(-PHY*t2));

  return true;
}

//TODO: apply m_spinX
bool
Ball::TargetToVS( vector2d target, double level, 
		  const vector2d spin, vector3d &v ) {
  double boundY = -TABLELENGTH/2;
  double y;
  vector3d tmpV = vector3d(0.0);

  if ( target[1] < m_x[1] ){
    y = -m_x[1];
    target[1] = -target[1];
  } else
    y = m_x[1];

  for ( boundY = -TABLELENGTH/2 ; boundY < 0 ; boundY += TICK ) {
    double vyMin = 0.1;
    double vyMax = 30.0;
    vector3d vCurrent = vector3d();
    double t1, t2, t3;
    double z;

    while (vyMax-vyMin > 0.001) {
      v[1] = (vyMin+vyMax)/2;

      // t2 = time until the ball reaches boundY
      t2 = -LOG( 1- PHY*(boundY-y)/v[1] ) / PHY;

      // define vz which satisfy z=TABLEHEIGHT when t=t2
      if (0 != t2) {
	  v[2] = (PHY*(TABLEHEIGHT-m_x[2])+GRAVITY(spin[1])*t2)/(1-exp(-PHY*t2)) -
	      GRAVITY(spin[1])/PHY;
      } else {
	  v[2] = m_x[2];
      }

      // Bound
      vCurrent[1] = v[1]*exp(-PHY*t2);
      vCurrent[2] = (v[2]+GRAVITY(spin[1])/PHY)*exp(-PHY*t2) - GRAVITY(spin[1])/PHY;

      vCurrent[1] += spin[1]*0.8;
      vCurrent[2] *= -TABLE_E;

      t1 = -LOG( 1- PHY*(target[1]-boundY)/vCurrent[1] ) / PHY;

      z = -( vCurrent[2]+GRAVITY(spin[1]*0.8)/PHY)*exp(-PHY*t1)/PHY
	 - GRAVITY(spin[1]*0.8)/PHY*t1
	 + (vCurrent[2]+GRAVITY(spin[1]*0.8)/PHY)/PHY;

      if ( z > 0 )
	vyMax = v[1];
      else
	vyMin = v[1];
    }

    if ( fabs(z) < TICK ) {
      t3 = -LOG( 1- PHY*(-boundY)/vCurrent[1] ) / PHY;
      z = -( vCurrent[2]+GRAVITY(spin[1]*0.8)/PHY)*exp(-PHY*t3)/PHY
	 - GRAVITY(spin[1]*0.8)/PHY*t3
	 + (vCurrent[2]+GRAVITY(spin[1]*0.8)/PHY)/PHY;
      if ( z > NETHEIGHT+(1.0-level)*0.1 ) {	// temporary
        if ( v[1] > tmpV[1] ) {
	  if (0 != t1+t2) {
	    tmpV[0] = PHY*(target[0]-m_x[0]) / (1-exp(-PHY*(t1+t2)));
	  } else {
	    tmpV[0] = v[0];
	  }
	  tmpV[1] = v[1];
	  tmpV[2] = v[2];
	}
      }
    }
  }
  v[0] = tmpV[0];
  v[1] = tmpV[1];

  if ( y != m_x[1] )
    v[1] = -v[1];

  v[2] = tmpV[2];

  return true;
}

void
Ball::BallDead() {
  if ( m_status >= 0 ) {
    if ( Control::TheControl()->IsPlaying() && &theBall == this ) {
      ((PlayGame *)Control::TheControl())->ChangeScore();

      m_status = -1;

      if ( mode == MODE_MULTIPLAY &&
	   Control::TheControl()->GetThePlayer()->GetSide() > 0 ) {
	Event::TheEvent()->SendBall();

	if ( LobbyClient::TheLobbyClient() )
	  LobbyClient::TheLobbyClient()->
	    SendSC( ((PlayGame *)Control::TheControl())->GetScore(1), 
		    ((PlayGame *)Control::TheControl())->GetScore(-1) );
      }
    }

    m_status = -1;
  }
}


char *
Ball::Send( char *buf ) {
  long l;
  double d;
  int c = 0;

  for ( int i = 0 ; i < 3 ; i++ ) {
    d = SwapDbl(m_x[i]);
    memcpy( &(buf[c]), (char *)&d, 8 );
    c += 8;
  }

  for ( int i = 0 ; i < 3 ; i++ ) {
    d = SwapDbl(m_v[i]);
    memcpy( &(buf[c]), (char *)&d, 8 );
    c += 8;
  }

  for ( int i = 0 ; i < 2 ; i++ ) {
    d = SwapDbl(m_spin[i]);
    memcpy( &(buf[c]), (char *)&d, 8 );
    c += 8;
  }

  l = SwapLong(m_status);
  memcpy( &(buf[c]), (char *)&l, 4 );
  c += 4;

#ifdef LOGGING
  Logging::GetLogging()->LogBall( LOG_COMBALL, this );
#endif

  return buf;
}
