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

extern Ball theBall;
extern Player* thePlayer;
extern Player* comPlayer;
extern Event theEvent;
extern BaseView theView;
extern PlayerSelect* theSelect;
extern Title* theTitle;
extern Howto* theHowto;
extern long mode;

extern long timeAdj;

extern long trainingCount;

extern void Timer( int value );
struct timeb m_lastTime = {0, 0, 0, 0};	// $BD>A0$K(BTimerEvent$B$,8F$P$l$?$H$-$N;~9o(B

extern int theSocket;

extern bool isLighting;
extern bool isFog;
extern bool isTexture;
extern bool isPolygon;
extern long winWidth;
extern long winHeight;

extern long wins;

extern bool isComm;
extern char serverName[256];

long _perfCount;
long perfs;

void CopyPlayerData( struct PlayerData& dest, Player* src );
bool GetExternalData( struct ExternalData **ext, int sd, long side );

Event::Event() {
  m_KeyHistory[0] = 0;
  m_MouseXHistory[0] = winWidth/2;
  m_MouseYHistory[0] = winHeight/2;
  m_MouseBHistory[0] = 0;
  m_Histptr = 0;

  m_External = NULL;
}

Event::~Event() {
}

bool
Event::Init() {
  switch ( mode ){
  case MODE_PLAY:
    PlayInit( 0, RAND(2) );
    break;
  case MODE_SELECT:
    SelectInit();
    break;
  case MODE_DEMO:
    DemoInit();
    break;
  case MODE_TITLE:
    TitleInit();
    break;
  }
  theBall.Init();

  glutKeyboardFunc( Event::KeyboardFunc );

#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
  glutKeyboardUpFunc( theEvent.KeyUpFunc );
#endif

  glutIdleFunc( Event::IdleFunc );
  glutMotionFunc( Event::MotionFunc );
  glutPassiveMotionFunc( Event::MotionFunc );
  glutMouseFunc( Event::ButtonFunc );
}

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

    for ( int i = 0 ; i < MAX_HISTORY ; i++ ) {
      theEvent.m_MouseXHistory[i] = 0;
      theEvent.m_MouseYHistory[i] = 0;
      theEvent.m_MouseBHistory[i] = 0;
      theEvent.m_BacktrackBuffer[i].sec = m_lastTime.time;
      theEvent.m_BacktrackBuffer[i].count = m_lastTime.millitm;
      theEvent.m_BacktrackBuffer[i].theBall = theBall;
      CopyPlayerData( theEvent.m_BacktrackBuffer[i].thePlayer, thePlayer );
      CopyPlayerData( theEvent.m_BacktrackBuffer[i].comPlayer, comPlayer );
    }

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
    reDraw |= theEvent.Move();
//    printf( "sec=%d count=%d status=%d\nx=%f y=%f z=%f spin = %f\n",
//	    m_lastTime.time, m_lastTime.millitm/10, theBall.GetStatus(),
//	    theBall.GetX(), theBall.GetY(), theBall.GetZ(), theBall.GetSpin() );
    theEvent.Record();

    m_lastTime.millitm += 10;
    if ( m_lastTime.millitm > 1000 ) {
      m_lastTime.time++;
      m_lastTime.millitm -= 1000;
    }
  }

  if ( theSocket >= 0 ) {
    // $B>pJs<u?.(B
    fd_set rdfds;
    struct timeval to;

    while (1) {
      FD_ZERO( &rdfds );
      FD_SET( theSocket, &rdfds );

      to.tv_sec = to.tv_usec = 0;

      if ( select( theSocket+1, &rdfds, NULL, NULL, &to ) > 0 ) {
	GetExternalData( &theEvent.m_External, theSocket, comPlayer->GetSide());
      } else
	break;
    }

    // externalData$B$N@hF,$^$G(Bbacktrack$B$9$k(B
    long btCount;
    struct ExternalData *externalOld;
    long btHistptr;
    while ( theEvent.m_External ) {	// $B<N$F$k(B?
      btCount = (m_lastTime.time-theEvent.m_External->sec)*100 + 
	(m_lastTime.millitm/10-theEvent.m_External->count);
      if ( btCount > MAX_HISTORY ) {
	externalOld = theEvent.m_External;
	theEvent.m_External = theEvent.m_External->next;
	delete externalOld;
	continue;
      }
      if ( btCount <= MAX_HISTORY )
	break;
    }

    if ( theEvent.m_External ) {
      if ( btCount >= 0 ) {
	btHistptr = theEvent.m_Histptr - btCount;
	if ( btHistptr < 0 )
	  btHistptr += MAX_HISTORY;

	long mtime = m_lastTime.millitm - btCount*10;
	while ( mtime < 0 ) {
	  mtime += 1000;
	  m_lastTime.time--;
	}
	m_lastTime.millitm = mtime;

	bool fTheBall, fThePlayer, fComPlayer;
	fTheBall = fThePlayer = fComPlayer = false;

//	theBall = m_BacktrackBuffer[btHistptr].theBall;
//	thePlayer->Reset( &m_BacktrackBuffer[btHistptr].thePlayer );
//	comPlayer->Reset( &m_BacktrackBuffer[btHistptr].comPlayer );

	// $BE,MQ$9$k(B -> $B?J$a$k$r(BbtCount$B7+$jJV$9(B
	while (1) {
	  if ( fTheBall )
	    theBall.Move();
	  if ( fThePlayer ) {
	    thePlayer->Move( theEvent.m_KeyHistory, theEvent.m_MouseXHistory,
			     theEvent.m_MouseYHistory, theEvent.m_MouseBHistory, btHistptr );
	  }
	  if ( fComPlayer )
	    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

	  while ( theEvent.m_External && theEvent.m_External->sec == m_lastTime.time &&
		 theEvent. m_External->count == m_lastTime.millitm/10 ) {
	    Player *targetPlayer;
	    if ( theEvent.m_External->side == thePlayer->GetSide() )
	      targetPlayer = thePlayer;
	    else if ( theEvent.m_External->side == comPlayer->GetSide() )
	      targetPlayer = comPlayer;
	    else
	      exit(1);

	    switch ( theEvent.m_External->dataType ) {
	    case DATA_PV:
	      targetPlayer->Warp( theEvent.m_External->data );
	      if ( targetPlayer == thePlayer )
		fThePlayer = true;
	      else if ( targetPlayer == comPlayer )
		fComPlayer = true;

//	      printf( "PV: sec = %d count = %d\n",
//		      m_External->sec, m_External->count);
//	      printf( "x=%f y=%f z=%f vx=%f vy=%f vz=%f\n",
//		      x, y, z, vx, vy, vz );
//	      fflush(0);
	      break;
	    case DATA_PS:
	      targetPlayer->ExternalSwing( theEvent.m_External->data );
	      if ( targetPlayer == thePlayer )
		fThePlayer = true;
	      else if ( targetPlayer == comPlayer )
		fComPlayer = true;

//	      printf( "PS: sec = %d count = %d swing = %d\n",
//		      m_External->sec, m_External->count, swing);
//	      fflush(0);
	      break;
	    case DATA_BV:
	      theBall.Warp( theEvent.m_External->data );
	      fTheBall = true;

//	      printf( "BV: sec = %d count = %d\n",
//		      m_External->sec, m_External->count);
//	      printf( "x=%f y=%f z=%f vx=%f vy=%f vz=%f\n",
//		      x, y, z, vx, vy, vz );
//	      fflush(0);
	      break;
	    }
	    externalOld = theEvent.m_External;
	    theEvent.m_External = theEvent.m_External->next;
	    delete externalOld;
	  }

//	  printf( "sec=%d count=%d status=%d\nx=%f y=%f z=%f spin = %f\n",
//		  m_lastTime.time, m_lastTime.millitm/10,	theBall.GetStatus(),
//		  theBall.GetX(), theBall.GetY(), theBall.GetZ(), theBall.GetSpin() );

	  btHistptr++;
	  if ( btHistptr == MAX_HISTORY )
	    btHistptr = 0;

	  m_lastTime.millitm += 10;
	  if ( m_lastTime.millitm >= 1000 ) {
	    m_lastTime.millitm -= 1000;
	    m_lastTime.time++;
	  }

	  btCount--;
	  if ( btCount < 0 )
	    break;
	}
      }
    }
  }

  if ( mode != MODE_TITLE )
    glutWarpPointer( theEvent.m_MouseXHistory[theEvent.m_Histptr],
		     theEvent.m_MouseYHistory[theEvent.m_Histptr] );

  if ( reDraw )
    glutPostRedisplay();
}

bool
Event::Move() {
  bool reDraw = false;
  long preMode = mode;

  switch ( mode ){
  case MODE_PLAY:
    theBall.Move();
    reDraw |= thePlayer->Move( m_KeyHistory, m_MouseXHistory,
			       m_MouseYHistory, m_MouseBHistory, m_Histptr );
    reDraw |= comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
    break;
  case MODE_SELECT:
  case MODE_TRAININGSELECT:
    reDraw |= theSelect->Move( m_KeyHistory, m_MouseXHistory,
			       m_MouseYHistory, m_MouseBHistory, m_Histptr );
    break;
  case MODE_DEMO:
    theBall.Move();
    reDraw |= thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    reDraw |= comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    if ( (m_MouseXHistory[m_Histptr] - winWidth/2) / (winWidth/40) ||
	 (m_MouseYHistory[m_Histptr] - winHeight/2) / (winHeight/40) ||
	 m_MouseBHistory[m_Histptr] || m_KeyHistory[m_Histptr] ) {
      theBall.EndGame();
    }

    reDraw = true;
    break;
  case MODE_TITLE:
    theBall.Move();
    reDraw |= thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    reDraw |= comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    reDraw |= theTitle->Move( m_KeyHistory, m_MouseXHistory,
			      m_MouseYHistory, m_MouseBHistory, m_Histptr );
    break;
  case MODE_HOWTO:
    if ( theHowto->IsMove() ) {
      theBall.Move();
      reDraw |= thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
      reDraw |= comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
    }

    reDraw |= theHowto->Move( m_KeyHistory, m_MouseXHistory,
			      m_MouseYHistory, m_MouseBHistory, m_Histptr );
    break;
  case MODE_TRAINING:
    theBall.Move();
    reDraw |= thePlayer->Move( m_KeyHistory, m_MouseXHistory,
			       m_MouseYHistory, m_MouseBHistory, m_Histptr );
    reDraw |= comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
    break;
  case MODE_SMASH:
    if ( m_Histptr == m_smashPtr ) {
      SmashEffect(false);
      m_smashCount++;
      if ( m_smashCount > 2 ) {
	mode = preMode = MODE_PLAY;
	m_smashCount = 0;
      } else {
	SmashEffect(true);
      }
    } else {
      theBall.Move();
      reDraw |= thePlayer->Move( m_KeyHistory, m_MouseXHistory,
				 m_MouseYHistory, m_MouseBHistory, m_Histptr );
      theBall = m_BacktrackBuffer[m_Histptr].theBall;
      thePlayer->Reset( &m_BacktrackBuffer[m_Histptr].thePlayer );
      comPlayer->Reset( &m_BacktrackBuffer[m_Histptr].comPlayer );
    }
    break;
  }

  if ( mode != preMode ){	// $B%b!<%IJQ99$"$j(B
    long p;

    switch ( mode ){
    case MODE_PLAY:
      if ( theSelect->GetRotate() < 0 )
	p = (360+(theSelect->GetRotate()%360))/(360/PLAYERS);
      else
	p = (theSelect->GetRotate()%360)/(360/PLAYERS);

      PlayInit( p, (p+wins+1)%PLAYERS );
      break;
    case MODE_SELECT:
      SelectInit();
      break;
    case MODE_DEMO:
      DemoInit();
      break;
    case MODE_TITLE:
      TitleInit();
      break;
    case MODE_HOWTO:
      HowtoInit();
      break;
    case MODE_TRAININGSELECT:
      TrainingSelectInit();
      break;
    case MODE_TRAINING:
      if ( theSelect->GetRotate() < 0 )
	p = (360+(theSelect->GetRotate()%360))/(360/TRAININGPLAYERS);
      else
	p = (theSelect->GetRotate()%360)/(360/TRAININGPLAYERS);

      TrainingInit( p, p );
      break;
    case MODE_SMASH:
      SmashEffect(true);
      break;
    }

    preMode = mode;
    reDraw = true;
  }

  return reDraw;
}

void
Event::Record() {
  long x, y;
  long key;
  long btn;

  x = m_MouseXHistory[m_Histptr];
  y = m_MouseYHistory[m_Histptr];
  key = m_KeyHistory[m_Histptr];
  btn = m_MouseBHistory[m_Histptr];

  m_Histptr++;
  if ( m_Histptr == MAX_HISTORY )
    m_Histptr = 0;

  if ( mode == MODE_SMASH )
    return;

  m_KeyHistory[m_Histptr] = 0;
  if ( mode == MODE_TITLE ) {
    m_MouseXHistory[m_Histptr] = x;
    m_MouseYHistory[m_Histptr] = y;
  } else if ( mode == MODE_SELECT || mode == MODE_TRAININGSELECT ) {
    m_MouseXHistory[m_Histptr] = winWidth/2 + (x-winWidth/2)*15/16;
    m_MouseYHistory[m_Histptr] = y;
  } else {
    m_MouseXHistory[m_Histptr] = winWidth/2 + (x-winWidth/2)*15/16;
    m_MouseYHistory[m_Histptr] = winHeight/2 + (y-winHeight/2)*15/16;
  }
  m_MouseBHistory[m_Histptr] = btn;

  //if ( theSocket >= 0 ) {
    m_BacktrackBuffer[m_Histptr].sec = m_lastTime.time;
    m_BacktrackBuffer[m_Histptr].count = m_lastTime.millitm;
    while ( m_BacktrackBuffer[m_Histptr].count > 100 ) {
      m_BacktrackBuffer[m_Histptr].sec--;
      m_BacktrackBuffer[m_Histptr].count += 100;
    }
    m_BacktrackBuffer[m_Histptr].theBall = theBall;
    CopyPlayerData( m_BacktrackBuffer[m_Histptr].thePlayer, thePlayer );
    CopyPlayerData( m_BacktrackBuffer[m_Histptr].comPlayer, comPlayer );
  //}

  return;
}

void
Event::KeyboardFunc( unsigned char key, int x, int y ) {
#if (GLUT_API_VERSION < 4 && GLUT_XLIB_IMPLEMENTATION < 13)
  if ( key == 'Q' ) {
    printf( "Avg = %f\n", (double)perfs/_perfCount );
    exit(0);
  }
#endif
  theEvent.m_KeyHistory[theEvent.m_Histptr] = key;
}

void
Event::KeyUpFunc( unsigned char key, int x, int y ) {
  if ( key == 'Q' ) {
    printf( "Avg = %f\n", (double)perfs/_perfCount );
    exit(0);
  }
}

void
Event::MotionFunc( int x, int y ) {
  theEvent.m_MouseXHistory[theEvent.m_Histptr] = x;
  theEvent.m_MouseYHistory[theEvent.m_Histptr] = y;
}

void
Event::ButtonFunc( int button, int state, int x, int y ) {
  if ( state == GLUT_DOWN ){
    switch ( button ){
    case GLUT_LEFT_BUTTON:
      theEvent.m_MouseBHistory[theEvent.m_Histptr] |= BUTTON_LEFT;
      break;
    case GLUT_MIDDLE_BUTTON:
      theEvent.m_MouseBHistory[theEvent.m_Histptr] |= BUTTON_MIDDLE;
      break;
    case GLUT_RIGHT_BUTTON:
      theEvent.m_MouseBHistory[theEvent.m_Histptr] |= BUTTON_RIGHT;
      break;
    }
  }
  else{
    switch ( button ){
    case GLUT_LEFT_BUTTON:
      theEvent.m_MouseBHistory[theEvent.m_Histptr] &= ~BUTTON_LEFT;
      break;
    case GLUT_MIDDLE_BUTTON:
      theEvent.m_MouseBHistory[theEvent.m_Histptr] &= ~BUTTON_MIDDLE;
      break;
    case GLUT_RIGHT_BUTTON:
      theEvent.m_MouseBHistory[theEvent.m_Histptr] &= ~BUTTON_RIGHT;
      break;
    }
  }

  theEvent.m_MouseXHistory[theEvent.m_Histptr] = x;
  theEvent.m_MouseYHistory[theEvent.m_Histptr] = y;
}

void
CopyPlayerData( struct PlayerData& dest, Player* src ) {
  if ( !src )
    return;

  dest.playerType = src->GetPlayerType();
  dest.side = src->GetSide();
  dest.x = src->GetX();
  dest.y = src->GetY();
  dest.z = src->GetZ();
  dest.vx = src->GetVX();
  dest.vy = src->GetVY();
  dest.vz = src->GetVZ();
  dest.status = src->GetStatus();
  dest.swing = src->GetSwing();
  dest.swingType = src->GetSwingType();
  dest.swingError = src->GetSwingError();
  dest.targetX = src->GetTargetX();
  dest.targetY = src->GetTargetY();
  dest.eyeX = src->GetEyeX();
  dest.eyeY = src->GetEyeY();
  dest.eyeZ = src->GetEyeZ();
  dest.pow = src->GetPower();
  dest.spin = src->GetSpin();
  dest.stamina = src->GetStamina();
}

void
ReadTime( int sd, long *sec, char *count ) {
  char buf[256];
  long len = 0;
  long ctmp;

  while (1) {
    if ( (len+=recv( sd, buf+len, 5-len, 0 )) == 5 )
      break;
  }
  memcpy( sec, &buf[0], 4 );
  memcpy( count, &buf[4], 1 );

  ctmp = *count;
  ctmp -= timeAdj;

  while ( ctmp >= 100 ) {
    ctmp -= 100;
    (*sec)++;
  }
  while ( ctmp < 0 ) {
    ctmp += 100;
    (*sec)--;
  }
  *count = ctmp;
}

void SendTime( int sd ) {
  char v;
  long time, millitm;

  time = m_lastTime.time;
  millitm = m_lastTime.millitm/10;

  millitm += timeAdj;

  while ( millitm >= 100 ) {
    millitm -= 100;
    time++;
  }
  while ( millitm < 0 ) {
    millitm += 100;
    time--;
  }

  send( sd, (char *)&time, 4, 0 );
  v = (char)(millitm);
  send( sd, (char *)&v, 1, 0 );
}

bool
GetExternalData( struct ExternalData **ext, int sd, long side ) {
  char buf[256];
  struct ExternalData *extNow;
  long len;

  if ( recv( sd, buf, 2, 0 ) != 2 )
    return false;

  extNow = new struct ExternalData;
  extNow->side = side;
  memset( extNow->data, 0, 256 );
  extNow->next = NULL;

  if ( !strncmp( buf, "PV", 2 ) ) {
    extNow->dataType = DATA_PV;
    ReadTime( sd, &extNow->sec, &extNow->count );

    len = 0;
    while (1) {
      if ( (len+=recv( sd, extNow->data+len, 48-len, 0 )) == 48 )
	break;
    }
//    printf( "Get PV : %d %d\n", extNow->sec, extNow->count );
  } else if ( !strncmp( buf, "PS", 2 ) ) {
    extNow->dataType = DATA_PS;
    ReadTime( sd, &extNow->sec, &extNow->count );

    len = 0;
    while (1) {
      if ( (len+=recv( sd, extNow->data+len, 20-len, 0 )) == 20 )
	break;
    }
  } else if ( !strncmp( buf, "BV", 2 ) ) {
    extNow->dataType = DATA_BV;
    ReadTime( sd, &extNow->sec, &extNow->count );

    len = 0;
    while (1) {
      if ( (len+=recv( sd, extNow->data+len, 60-len, 0 )) == 60 )
	break;
    }
  } else
    return false;

  if ( !(*ext) ) {
    extNow->next = NULL;
    *ext = extNow;
  } else if ( (*ext)->sec > extNow->sec || 
	      ((*ext)->sec == extNow->sec && (*ext)->count > extNow->count) ) {
    extNow->next = *ext;
    *ext = extNow;
  } else {
    struct ExternalData *ext2 = *ext;

    while ( ext2->next ) {
      if ( ext2->next->sec > extNow->sec || 
	   (ext2->next->sec == extNow->sec &&
	    ext2->next->count > extNow->count ) ) {
	extNow->next = ext2->next;
	ext2->next = extNow;
	break;
      }
      ext2 = ext2->next;
    }
    if ( !ext2->next )
      ext2->next = extNow;
  }

  return true;
}

bool
Event::SendSwing( int sd, Player *player ) {
  if ( m_backtrack || mode != MODE_PLAY )
    return false;

  send( sd, "PS", 2, 0 );
  SendTime( sd );

  player->SendSwing( sd );

  return true;
}

bool
Event::SendPlayer( int sd, Player *player ) {
  if ( m_backtrack || mode != MODE_PLAY )
    return false;

  send( sd, "PV", 2, 0 );
  SendTime( sd );

  player->SendLocation( sd );

  return true;
}

bool
Event::SendBall( int sd ) {
  if ( m_backtrack || mode != MODE_PLAY )
    return false;

  send( sd, "BV", 2, 0 );
  SendTime( sd );

  theBall.Send( sd );

  return true;
}

void
Event::ClearObject() {
  if ( thePlayer && wins == 0 ) {
    delete thePlayer;
    thePlayer = NULL;
    if ( theSocket != -1 ) {
      send( theSocket, "QT", 2, 0 );
      closesocket( theSocket );
      theSocket = -1;
    }
  }
  if ( comPlayer ) {
    delete comPlayer;
    comPlayer = NULL;
  }
  if ( theSelect ) {
    delete theSelect;
    theSelect = NULL;
  }
  if ( theTitle ) {
    delete theTitle;
    theTitle = NULL;
  }
  if ( theHowto ) {
    delete theHowto;
    theHowto = NULL;
  }
}

void
Event::PlayInit( long player, long com ) {
  long side;

  ClearObject();

  if (isComm) {
    if ( !(serverName[0]) )
      side = 1;		// server$BB&(B
    else
      side = -1;	// client$BB&(B

    if ( thePlayer == NULL ) {
      thePlayer = Player::Create( player, side, 0 );
    }

    if ( side == 1 ) {
      StartServer();
    } else {
      StartClient();
    }
  } else {
    thePlayer = Player::Create( player, 1, 0 );
    comPlayer = Player::Create( com, -1, 1 );
  }

  thePlayer->Init();
  comPlayer->Init();

  glutSetCursor( GLUT_CURSOR_NONE );
}

void
Event::DemoInit() {
  ClearObject();

  // $B8e$G(BSelect$B$K0\F0(B?
  thePlayer = Player::Create( RAND(3), 1, 1 );
  comPlayer = Player::Create( RAND(3), -1, 1 );

  thePlayer->Init();
  comPlayer->Init();

  glutSetCursor( GLUT_CURSOR_NONE );
}

void
Event::SelectInit() {
  ClearObject();

  theSelect = new PlayerSelect();

  theSelect->Init();

  glutSetCursor( GLUT_CURSOR_NONE );
}

void
Event::TitleInit() {
  ClearObject();

  theTitle = new Title();

  theTitle->Init();

  // $B8e$G(BSelect$B$K0\F0(B?
  thePlayer = Player::Create( RAND(3), 1, 1 );
  comPlayer = Player::Create( RAND(3), -1, 1 );

  thePlayer->Init();
  comPlayer->Init();

  glutSetCursor( GLUT_CURSOR_INHERIT );
}

void
Event::HowtoInit() {
  ClearObject();

  theHowto = new Howto();

  theHowto->Init();

  thePlayer = new PenAttack(1);
  comPlayer = new ShakeCut(-1);

  thePlayer->Init();
  comPlayer->Init();
}

void
Event::TrainingSelectInit() {
  ClearObject();

  theSelect = new TrainingSelect();

  theSelect->Init();

  glutSetCursor( GLUT_CURSOR_NONE );
}

void
Event::TrainingInit( long player, long com ) {
  long side;

  ClearObject();

  thePlayer = Player::Create( player, 1, 2 );
  comPlayer = Player::Create( com, -1, 3 );

  thePlayer->Init();
  comPlayer->Init();

  trainingCount = 0;

  glutSetCursor( GLUT_CURSOR_NONE );
}

#if 0
void
Event::GetHittingPlayerData( Player *player, long count, bool isThePlayer ) {
  static struct PlayerData p;
  static Ball b;

  if ( count > 0 ) {
    CopyPlayerData( p, player );
    b = theBall;

    long ptr = m_Histptr;
    for ( int i = 0 ; i < MAX_HISTORY ; i++ ) {
      if ( m_BacktrackBuffer[ptr].theBall.GetStatus() == 0 )
	break;

      ptr--;
      if ( ptr < 0 )
	ptr = MAX_HISTORY;
    }

    ptr += count;
    if ( ptr >= MAX_HISTORY )
      ptr -= MAX_HISTORY;

    if ( isThePlayer )
      player->Reset( &m_BacktrackBuffer[ptr].thePlayer );
    else
      player->Reset( &m_BacktrackBuffer[ptr].comPlayer );
    theBall = m_BacktrackBuffer[ptr].theBall;
  } else {
    player->Reset( &p );
    theBall = b;
  }
}

void
Event::GetHittingBallData( Ball &ball, long count ) {
  static Ball b;

  if ( count > 0 ) {
    b = ball;

    long ptr = m_Histptr;
    for ( int i = 0 ; i < MAX_HISTORY ; i++ ) {
      if ( m_BacktrackBuffer[ptr].theBall.GetStatus() == 0 )
	break;

      ptr--;
      if ( ptr < 0 )
	ptr = MAX_HISTORY;
    }

    ptr += count;
    if ( ptr >= MAX_HISTORY )
      ptr -= MAX_HISTORY;

    ball = m_BacktrackBuffer[ptr].theBall;
  } else {
    ball = b;
  }
}
#endif

void
Event::SmashEffect( bool start ) {
  static struct PlayerData p1, p2;
  static Ball b;

  if (start) {
    CopyPlayerData( p1, thePlayer );
    CopyPlayerData( p2, comPlayer );
    b = theBall;

    m_smashPtr = m_Histptr-1;
    if ( m_smashPtr < 0 )
      m_smashPtr = MAX_HISTORY;

    for ( int i = 0 ; i < MAX_HISTORY ; i++ ) {
      if ( m_BacktrackBuffer[m_Histptr].theBall.GetStatus() == 2 )
	break;

      m_Histptr--;
      if ( m_Histptr < 0 )
	m_Histptr = MAX_HISTORY;
    }

    thePlayer->Reset( &m_BacktrackBuffer[m_Histptr].thePlayer );
    comPlayer->Reset( &m_BacktrackBuffer[m_Histptr].comPlayer );
    theBall = m_BacktrackBuffer[m_Histptr].theBall;
  } else {
    thePlayer->Reset( &p1 );
    comPlayer->Reset( &p2 );
    theBall = b;
  }
}
