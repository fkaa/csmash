/* $Id$ */

// Copyright (C) 2001  $B?@Fn(B $B5H9((B(Kanna Yoshihiro)
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
#include "MultiPlay.h"
#include "Event.h"
#include "Ball.h"
#include "Player.h"
#include "Logging.h"

Logging* Logging::m_logging = NULL;

Logging::Logging() {
}

Logging::~Logging() {
}

Logging*
Logging::GetLogging() {
  if ( !Logging::m_logging )
    return false;

  Logging::m_logging = new Logging();

  return Logging::m_logging;
}

bool
Logging::Init() {
  static char* fname[8] = { "com_ball.log", "com_thePlayer.log", 
			    "com_comPlayer.log", "com_misc.log", 
			    "act_ball.log", "act_thePlayer.log", 
			    "act_comPlayer.log", "act_misc.log" };

  for ( int i = 0 ; i < 7 ; i++ ) {
    if ( (m_fp[i] = fopen( fname[i], "w" )) == 0 )
      return false;
  }

  return true;
}

bool
Logging::Log( long logType, char *logString ) {
  fputs( logString, m_fp[logType] );

  return true;
}

bool
Logging::StartLog() {
  struct timeb tb;
  char buf[256];

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

  sprintf( buf, "--- START LOGGING %d.%3d ---", (int)tb.time, (int)tb.millitm );

  for ( int i = 0 ; i < 7 ; i++ )
    Log( i, buf );

  return true;
}

bool
Logging::LogTime( long logType, struct timeb *tb ) {
  char buf[64];

  sprintf( buf, "%d.%3d: ", (int)tb->time, (int)tb->millitm );
  Log( logType, buf );

  return true;
}

bool
Logging::LogTime( long logType ) {
  char buf[64];

  sprintf( buf, "%d.%3d: ", 
	   (int)Event::m_lastTime.time, (int)Event::m_lastTime.millitm );
  Log( logType, buf );

  return true;
}

bool
Logging::LogBall( long logType, Ball *ball ) {
  char buf[1024];

  LogTime( logType );
  sprintf( buf, "status = %2d x = %4.2f y = %4.2f z = %4.2f vx = %4.2f vy = %4.2f vz = %4.2f spin = %3.2f\n",
	   (int)ball->GetStatus(), ball->GetX(), ball->GetY(), ball->GetZ(), ball->GetVX(), ball->GetVY(), ball->GetVZ(), ball->GetSpin() );
  Log( logType, buf );

  return true;
}

bool
Logging::LogPlayer( long logType, Player *player ) {
  char buf[1024];

  LogTime( logType );
  sprintf( buf, "playerType=%1d side=%2d x=%4.2f y=%4.2f z=%4.2f vx=%4.2f vy=%4.2f vz=%4.2f status=%2d swing=%2d swingType=%1d swingSide=%2d afterSwing=%2d swingError=%1d targetX=%4.2f targetY=%4.2f eyeX=%4.2f eyeY=%4.2f eyeZ=%4.2f lookAtX=%4.2f lookAtY=%4.2f lookAtZ=%4.2f pow=%1d spin=%3.2f stamina=%2.0f dragX=%2d dragY=%2d\n",
	   (int)player->GetPlayerType(), (int)player->GetSide(), 
	   player->GetX(), player->GetY(), player->GetZ(), 
	   player->GetVX(), player->GetVY(), player->GetVZ(), 
	   (int)player->GetStatus(), (int)player->GetSwing(),
	   (int)player->GetSwingType(), (int)player->GetSwingSide(),
	   (int)player->GetAfterSwing(), (int)player->GetSwingError(), 
	   player->GetTargetX(), player->GetTargetY(),
	   player->GetEyeX(), player->GetEyeY(), player->GetEyeZ(),
	   player->GetLookAtX(), player->GetLookAtY(), player->GetLookAtZ(),
	   (int)player->GetPower(), player->GetSpin(), player->GetStamina(),
	   (int)player->GetDragX(), (int)player->GetDragY() );
  Log( logType, buf );

  return true;
}

bool
Logging::LogRecvBVMessage( ExternalBVData *bv ) {
  char buf[256];

  LogTime( LOG_COMBALL );
  sprintf( buf, "recv: %d.%3d ", (int)bv->sec, (int)bv->count );
  Log( LOG_COMBALL, buf );
  Ball *tmpBall = new Ball();
  tmpBall->Warp(bv->data);

  sprintf( buf, "x=%4.2f y=%4.2f z=%4.2f vx=%4.2f vy=%4.2f vz=%4.2f spin=%3.2f status=%2d\n",
	   tmpBall->GetX(), tmpBall->GetY(), tmpBall->GetZ(), 
	   tmpBall->GetVX(), tmpBall->GetVY(), tmpBall->GetVZ(), 
	   tmpBall->GetSpin(), (int)tmpBall->GetStatus() );
  Log( LOG_COMBALL, buf );

  return true;
}

bool
Logging::LogSendPVMessage( Player *player ) {
  char buf[256];

  LogTime( LOG_COMTHEPLAYER );
  sprintf( buf, "send PV: x=%4.2f y=%4.2f z=%4.2f vx=%4.2f vy=%4.2f vz=%4.2f\n",
	   player->GetX(), player->GetY(), player->GetZ(), 
	   player->GetVX(), player->GetVY(), player->GetVZ() );
  Log( LOG_COMTHEPLAYER, buf );

  return true;
}

bool
Logging::LogRecvPVMessage( ExternalPVData *pv ) {
  char buf[256];

  LogTime( LOG_COMCOMPLAYER );
  sprintf( buf, "recv PV: %d.%3d ", (int)pv->sec, (int)pv->count );
  Log( LOG_COMCOMPLAYER, buf );
  Player *tmpPlayer = new Player();
  tmpPlayer->Warp(pv->data);

  sprintf( buf, "x=%4.2f y=%4.2f z=%4.2f vx=%4.2f vy=%4.2f vz=%4.2f\n",
	   tmpPlayer->GetX(), tmpPlayer->GetY(), tmpPlayer->GetZ(), 
	   tmpPlayer->GetVX(), tmpPlayer->GetVY(), tmpPlayer->GetVZ() );
  Log( LOG_COMCOMPLAYER, buf );

  return true;
}

bool
Logging::LogSendPSMessage( Player *player ) {
  char buf[256];

  LogTime( LOG_COMTHEPLAYER );

  sprintf( buf, "send PS: pow=%2d spin=%3.2f swingType=%1d swingSide=%2d swing=%2d\n",
	   (int)player->GetPower(), player->GetSpin(),
	   (int)player->GetSwingType(), player->GetSwingSide(),
	   (int)player->GetSwing() );
  Log( LOG_COMTHEPLAYER, buf );

  return true;
}

bool
Logging::LogRecvPSMessage( ExternalPSData *ps ) {
  char buf[256];

  LogTime( LOG_COMCOMPLAYER );
  sprintf( buf, "recv PS: %d.%3d ", (int)ps->sec, (int)ps->count );
  Log( LOG_COMCOMPLAYER, buf );
  Player *tmpPlayer = new Player();
  tmpPlayer->ExternalSwing(ps->data);

  sprintf( buf, "pow=%2d spin=%3.2f swingType=%1d swingSide=%2d swing=%2d\n",
	   (int)tmpPlayer->GetPower(), tmpPlayer->GetSpin(),
	   (int)tmpPlayer->GetSwingType(), tmpPlayer->GetSwingSide(),
	   (int)tmpPlayer->GetSwing() );
  Log( LOG_COMCOMPLAYER, buf );

  return true;
}
