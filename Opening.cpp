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
#include <signal.h>

extern BaseView theView;
extern long mode;

extern long gameLevel;
extern long gameMode;

extern Sound theSound;

extern void Keyboard( unsigned char key, int x, int y );
extern void KeyUp( unsigned char key, int x, int y );

extern Player* thePlayer;
extern Player* comPlayer;
extern Ball theBall;

Opening::Opening() {
  m_View = NULL;
  m_count = 0;
  m_bgmCount = 0;
  m_pid = 0;
}

Opening::~Opening() {
  if ( m_View ){
    theView.RemoveView( m_View );
    delete m_View;
  }

#ifndef WIN32
  if ( m_pid > 0 ) {
    kill ( m_pid, SIGKILL );
  }
#endif
}

bool
Opening::Init() {
  m_View = new OpeningView();
  m_View->Init( this );

  theView.AddView( m_View );

  thePlayer = Player::Create( 1, 1, 0 );
  comPlayer = Player::Create( 2, -1, 0 );

#if 0
  thePlayer->Init();
  comPlayer->Init();
#else
  thePlayer->m_View = new PlayerView();
  thePlayer->m_View->Init( thePlayer );

  comPlayer->m_View = new PlayerView();
  comPlayer->m_View->Init( comPlayer );

  HitMark::Init();
#endif

#ifdef HAVE_LIBESD
  m_pid = theSound.InitBGM( SOUND_OPENING );
#endif
#ifdef WIN32
  theSound.InitBGM( SOUND_OPENING );
#endif

  return true;
}

Opening*
Opening::Create() {
  Opening *newOpening;

  Event::ClearObject();

  newOpening = new Opening();
  newOpening->Init();

  return newOpening;
}

bool
Opening::Move( unsigned long *KeyHistory, long *MouseXHistory,
	       long *MouseYHistory, unsigned long *MouseBHistory,
	       int Histptr ) {
  long phrase, mod;

  GetPhrase( phrase, mod );

  if ( m_count == 0 ) {
    thePlayer->m_x = -1.5;
    thePlayer->m_y = -TABLELENGTH*3/2;

    comPlayer->m_x = 1.5;
    comPlayer->m_y = TABLELENGTH*3/2;

    //m_count = 5700;
  } else if ( phrase < 32 ) {	// $BAG?6$j(B
    thePlayer->m_swing = (m_count%100)/2;
    thePlayer->m_swingType = SWING_CUT;
    comPlayer->m_swing = (m_count%100)/2;
    comPlayer->m_swingType = SWING_DRIVE;

    if ( (m_count%200) < 100 )
      theBall.m_x = 0.0;
    else
      theBall.m_x = -3.0;

    theBall.m_z = -1.0;
  } else if ( phrase < 36 ) {	// $B8+2s$9(B
    thePlayer->m_targetX = TABLELENGTH*5*
      sin((m_count-1600)/200.0*3.14159265);
    thePlayer->m_targetY = fabs(TABLELENGTH*5*
      cos((m_count-1600)/200.0*3.14159265));
  } else if ( phrase < 48 ) {	// $BBfA0$X(B
    thePlayer->m_x = -1.5+(m_count-1800)*1.5/600.0;
    thePlayer->m_y = -TABLELENGTH*3/2+(m_count-1800)*TABLELENGTH/600.0;
    comPlayer->m_x = 1.5-(m_count-1800)*1.5/600.0;
    comPlayer->m_y = TABLELENGTH*3/2-(m_count-1800)*TABLELENGTH/600.0;

    theBall.m_z = -1.0;
  } else if ( m_count == 2400 ) {	// $B?3H=$+$i%\!<%k$r<u$1<h$k(B
    theBall.m_x = -TABLEWIDTH/2;
    theBall.m_y = 0;
    theBall.m_z = TABLEHEIGHT+NETHEIGHT*3;
    theBall.m_vx = (TABLEWIDTH/2+0.3)/2.0;
    theBall.m_vy = -TABLELENGTH/2/2.0;
    theBall.m_vz = 1.0;
    theBall.m_status = -1;

    thePlayer->m_targetX = -TABLEWIDTH/2+0.2;
    thePlayer->m_targetY = TABLELENGTH/2-0.2;
  } else if ( phrase < 52 ) {	// $B%\!<%kE>$,$k(B
    theBall.Move();
    theBall.m_status = -1;
  } else if ( m_count == 2600 ) {	// $B%\!<%k<h$k(B
    theBall.m_status = 8;
    theBall.m_x = thePlayer->GetX()+0.3;
    theBall.m_y = thePlayer->GetY();
    theBall.m_z = TABLEHEIGHT + 0.15;
    theBall.m_vx = 0.0;
    theBall.m_vy = 0.0;
    theBall.m_vz = 0.0;
  } else if ( phrase < 56 ) {	// $B9=$($k(B
    thePlayer->m_vy = -0.1;
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    theBall.Move();
  } else if ( phrase < 68 ) {
  } else if ( phrase == 68 && mod == 0 ) {	// $B%H%9(B
    thePlayer->m_swing = 0;
    theBall.Toss( thePlayer, 3 );
    thePlayer->StartSwing( 3 );
  } else if ( phrase < 72 ) {	// $B%5!<%V(B
    if ( mod == 0 ) {
      for ( int i = 0 ; i < 15 ; i++ ) {
	theBall.Move();
	thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
	comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
      }
    }
  } else if ( phrase < 76 ) {	// $B%l%7!<%V(B
    if ( phrase == 72 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2;
    }

    if ( phrase == 73 && mod == 0 ) {
      double vx, vy, vz;
      theBall.Warp( TABLEWIDTH/4, -TABLELENGTH/4, TABLEHEIGHT+0.01,
		    0.0, 0.0, 0.0, 0.0, 0 );
      theBall.TargetToV( -TABLEWIDTH/2+0.1, TABLELENGTH/2-0.1, 0.7, 0.0,
		       vx, vy, vz, 0.1, 20.0 );
      theBall.Warp( TABLEWIDTH/4, -TABLELENGTH/4, TABLEHEIGHT+0.01,
		    vx, vy, vz, 0.0, 0 );
    }
    if ( phrase >= 73 ) {
      comPlayer->m_vx = (-TABLEWIDTH+0.1)/0.7;
      comPlayer->m_vy = 0.5/0.7;
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    thePlayer->m_status = 200;
    comPlayer->m_status = 200;

    if ( phrase == 73 && mod == 26 ) {
      comPlayer->StartSwing( 0.5 );
    }
    if ( phrase == 74 && mod == 25 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.5 );
    }
  } else if ( phrase < 80 ) {	// $B%+%C%H(B
    if ( phrase == 76 && mod == 0 ) {
      theBall.m_vx = theBall.m_vy = 0.0;
      theBall.m_vz = 2.0;
      thePlayer->m_targetX = TABLEWIDTH/2;
    }
    if ( phrase == 77 && mod == 20 ) {
      double vx, vy, vz;
      theBall.Warp( -TABLEWIDTH/2, TABLELENGTH/2, TABLEHEIGHT+NETHEIGHT,
		    0.0, 0.0, 0.0, 0.0, 1 );
      theBall.TargetToV( TABLEWIDTH/2-0.1, -TABLELENGTH/2+0.3, 0.8, 1.0,
		       vx, vy, vz, 0.1, 20.0 );
      theBall.Warp( -TABLEWIDTH/2, TABLELENGTH/2, TABLEHEIGHT+NETHEIGHT,
		    vx, vy, vz, 1.0, 2 );
    }
    if ( phrase >= 77 && phrase <= 78 ) {
      thePlayer->m_vx = (TABLEWIDTH-0.1)/0.7;
      thePlayer->m_vy = -TABLELENGTH/2/0.7;
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    thePlayer->m_status = 200;
    comPlayer->m_status = 200;

    if ( phrase == 77 && mod == 26 ) {
      thePlayer->StartSwing( 0.5 );
    }
    if ( phrase == 78 && mod == 25 ) {	// $BBG5e(B0.1$BICA0(B
      thePlayer->Swing( 0.5 );
    }
  } else if ( phrase < 84 ) {	// $B%P%C%/$X%I%i%$%V(B
    if ( phrase == 80 && mod == 0 ) {
      comPlayer->m_x = -TABLEWIDTH/2;
      comPlayer->m_y = TABLELENGTH/2+0.5;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;

      double vx, vy, vz;
      theBall.Warp( TABLEWIDTH/2+0.5, -TABLELENGTH, TABLEHEIGHT+NETHEIGHT,
		    0.0, 0.0, 0.0, 0.0, 0 );
      theBall.TargetToV( TABLEWIDTH/2-0.1, TABLELENGTH/2-0.1, 0.53, -1.0,
		       vx, vy, vz, 0.1, 20.0 );
      theBall.Warp( TABLEWIDTH/2+0.5, -TABLELENGTH, TABLEHEIGHT+NETHEIGHT,
		    vx, vy, vz, -1.0, 0 );
    }
    if ( phrase >= 81 ) {
      comPlayer->m_vx = (TABLEWIDTH+0.1)/0.7;
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    thePlayer->m_status = 200;
    comPlayer->m_status = 200;

    if ( phrase == 81 && mod == 26 ) {
      comPlayer->StartSwing( 0.5 );
    }
    if ( phrase == 82 && mod == 25 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.5 );
    }
  } else if ( phrase < 88 ) {	// $B%P%C%/%+%C%H(B
    if ( phrase == 84 && mod == 0 ) {
      thePlayer->m_x = TABLEWIDTH/2;
      thePlayer->m_y = -TABLELENGTH-0.5;
      thePlayer->m_vx = -(TABLEWIDTH+0.4)/0.7;
      thePlayer->m_vy = 0.0;
      thePlayer->m_targetX = -TABLEWIDTH/2;
    }
    if ( phrase == 85 && mod == 20 ) {
      double vx, vy, vz;
      theBall.Warp( TABLEWIDTH/2-0.2, TABLELENGTH/2, TABLEHEIGHT+NETHEIGHT,
		    0.0, 0.0, 0.0, 0.0, 1 );
      theBall.TargetToV( -TABLEWIDTH/2+0.2, -TABLELENGTH/2+0.3, 0.8, 1.0,
		       vx, vy, vz, 0.1, 20.0 );
      theBall.Warp( TABLEWIDTH/2-0.2, TABLELENGTH/2, TABLEHEIGHT+NETHEIGHT,
		    vx, vy, vz, 1.0, 2 );
    }
    if ( phrase >= 85 ) {
      theBall.Move();
      thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
      comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

      thePlayer->m_status = 200;
      comPlayer->m_status = 200;
    }

    if ( phrase == 85 && mod == 26 ) {
      thePlayer->StartSwing( 0.5 );
    }
    if ( phrase == 86 && mod == 25 ) {	// $BBG5e(B0.1$BICA0(B
      thePlayer->Swing( 0.5 );
    }
    if ( phrase == 87 && mod == 0 ) {
      thePlayer->m_vx = 0.0;
    }
  } else if ( phrase < 92 ) {	// $B%9%H%C%W(B
    if ( phrase == 88 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2+0.1;
      comPlayer->m_y = TABLELENGTH+0.5;
      comPlayer->m_vx = -(TABLEWIDTH/2+0.3)/1.05;
      comPlayer->m_vy = -TABLELENGTH/1.05;
      comPlayer->m_targetX = -TABLEWIDTH/2+0.3;
      comPlayer->m_targetY = -TABLELENGTH/2+1.0;

      double vx, vy, vz;
      theBall.Warp( -TABLEWIDTH/2-0.4, -TABLELENGTH-0.5, TABLEHEIGHT+NETHEIGHT,
		    0.0, 0.0, 0.0, 0.0, 0 );
      theBall.TargetToV( -TABLEWIDTH/2+0.2, TABLELENGTH/2-0.85, 0.7, -1.0,
			 vx, vy, vz, 0.1, 20.0 );
      theBall.Warp( -TABLEWIDTH/2-0.4, -TABLELENGTH-0.5, TABLEHEIGHT+NETHEIGHT,
		    vx, vy, vz, -1.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    thePlayer->m_status = 200;
    comPlayer->m_status = 200;

    if ( phrase == 89 && mod == 26 ) {
      comPlayer->StartSwing( 0.5 );
    }
    if ( phrase == 90 && mod == 25 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.5 );
    }
    if ( phrase == 91 && mod == 0 ) {
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
    }
  } else if ( phrase < 96 ) {	// $B%/%m%9$X%9%^%C%7%e(B
    if ( phrase == 92 && mod == 0 ) {
      thePlayer->m_x = TABLEWIDTH/2+0.4;
      thePlayer->m_y = -TABLELENGTH-0.5;
      thePlayer->m_vx = -(TABLEWIDTH/2+0.5)/0.7;
      thePlayer->m_vy = (TABLELENGTH/2+1.0)/0.7;
      thePlayer->m_targetX = TABLEWIDTH/2-0.1;
      thePlayer->m_targetY = TABLELENGTH/2-0.8;

      double vx, vy, vz;
      theBall.Warp( -0.5, TABLELENGTH/2-0.5, TABLEHEIGHT+0.05,
		    0.0, 0.0, 0.0, 0.0, 1 );
      theBall.TargetToV( -TABLEWIDTH/2+0.3, -TABLELENGTH/2+0.8, 0.5, -0.5,
			 vx, vy, vz, 0.1, 20.0 );
      theBall.Warp( -0.5, TABLELENGTH/2-0.5, TABLEHEIGHT+0.05,
		    vx, vy, vz, -0.5, 2 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    thePlayer->m_status = 200;
    comPlayer->m_status = 200;

    if ( phrase == 93 && mod == 26 ) {
      thePlayer->StartSwing( 0.5 );
    }
    if ( phrase == 94 && mod == 25 ) {	// $BBG5e(B0.1$BICA0(B
      thePlayer->Swing( 0.5 );
    }
    if ( phrase == 94 && mod == 0 ) {
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;
    }
    if ( phrase == 95 && mod == 0 ) {	// $B9bB.2=(B
      theBall.TargetToV( thePlayer->m_targetX, thePlayer->m_targetY, 1.0, 0.0,
			 theBall.m_vx, theBall.m_vy, theBall.m_vz, 0.1, 10.0 );
    }
  } else if ( phrase < 100 ) {	// $B%j%W%l%$(B
    if ( phrase == 96 && mod == 0 ) {
      thePlayer->m_x = TABLEWIDTH/2+0.4;
      thePlayer->m_y = -TABLELENGTH-0.5;
      thePlayer->m_vx = -(TABLEWIDTH/2+0.5)/0.7;
      thePlayer->m_vy = (TABLELENGTH/2+1.0)/0.7;
      thePlayer->m_targetX = TABLEWIDTH/2-0.1;
      thePlayer->m_targetY = TABLELENGTH/2-0.8;

      double vx, vy, vz;
      theBall.Warp( -0.5, TABLELENGTH/2-0.5, TABLEHEIGHT+0.05,
		    0.0, 0.0, 0.0, 0.0, 1 );
      theBall.TargetToV( -TABLEWIDTH/2+0.3, -TABLELENGTH/2+0.8, 0.5, -0.5,
			 vx, vy, vz, 0.1, 20.0 );
      theBall.Warp( -0.5, TABLELENGTH/2-0.5, TABLEHEIGHT+0.05,
		    vx, vy, vz, -0.5, 2 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    thePlayer->m_status = 200;
    comPlayer->m_status = 200;

    if ( phrase == 98 && mod == 1 ) {
      thePlayer->StartSwing( 0.5 );
    }
    if ( phrase == 98 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      thePlayer->Swing( 0.5 );
    }
    if ( phrase == 98 && mod == 0 ) {
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;
    }
    if ( phrase == 99 && mod == 0 ) {	// $B9bB.2=(B
      theBall.TargetToV( thePlayer->m_targetX, thePlayer->m_targetY, 1.0, 0.0,
			 theBall.m_vx, theBall.m_vy, theBall.m_vz, 0.1, 10.0 );
    }
  } else if ( phrase < 104 ) {	// $B%\!<%kE>$,$k(B
    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
  } else if ( phrase < 106 ) {	// $B%9%^%C%7%eO"BG(B(1)
    if ( phrase == 104 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH/2-0.3;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;

      comPlayer->m_x = -TABLEWIDTH/2+0.5;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = -TABLEWIDTH/2+0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    -1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 104 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 104 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 108 ) {	// $B%9%^%C%7%eO"BG(B(2)
    if ( phrase == 106 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2+0.1;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = -TABLEWIDTH/2+0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 106 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 106 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 110 ) {	// $B%9%^%C%7%eO"BG(B(3)
    if ( phrase == 108 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH/2-0.3;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;

      comPlayer->m_x = -TABLEWIDTH/2+0.5;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = TABLEWIDTH/2-0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    -1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 108 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 108 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 112 ) {	// $B%9%^%C%7%eO"BG(B(4)
    if ( phrase == 110 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2+0.1;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = TABLEWIDTH/2-0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 110 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 110 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 114 ) {	// $B%9%^%C%7%eO"BG(B(5)
    if ( phrase == 112 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH/2-0.3;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;

      comPlayer->m_x = -TABLEWIDTH/2+0.5;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = -TABLEWIDTH/2+0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    -1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 112 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 112 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 116 ) {	// $B%9%^%C%7%eO"BG(B(6)
    if ( phrase == 114 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2+0.1;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = -TABLEWIDTH/2+0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 114 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 114 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 118 ) {	// $B%9%^%C%7%eO"BG(B(7)
    if ( phrase == 116 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH/2-0.3;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;

      comPlayer->m_x = -TABLEWIDTH/2+0.5;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = TABLEWIDTH/2-0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    -1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 116 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 116 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 120 ) {	// $B%9%^%C%7%eO"BG(B(8)
    if ( phrase == 118 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2+0.1;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = TABLEWIDTH/2-0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 118 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 118 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 122 ) {	// $B%9%^%C%7%eO"BG(B(9)
    if ( phrase == 120 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH/2-0.3;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;

      comPlayer->m_x = -TABLEWIDTH/2+0.5;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = -TABLEWIDTH/2+0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    -1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 120 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 120 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 124 ) {	// $B%9%^%C%7%eO"BG(B(10)
    if ( phrase == 122 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2+0.1;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = -TABLEWIDTH/2+0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 122 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 122 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 126 ) {	// $B%9%^%C%7%eO"BG(B(11)
    if ( phrase == 124 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH/2-0.3;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;

      comPlayer->m_x = -TABLEWIDTH/2+0.5;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = TABLEWIDTH/2-0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    -1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 124 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 124 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 128 ) {	// $B%9%^%C%7%eO"BG(B(12)
    if ( phrase == 126 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2+0.1;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = TABLEWIDTH/2-0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 126 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 126 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 130 ) {	// $B%9%^%C%7%eO"BG(B(13)
    if ( phrase == 128 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH/2-0.3;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;

      comPlayer->m_x = -TABLEWIDTH/2+0.5;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = -TABLEWIDTH/2+0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    -1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 128 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 128 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 132 ) {	// $B%9%^%C%7%eO"BG(B(14)
    if ( phrase == 130 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2+0.1;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = -TABLEWIDTH/2+0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 130 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 130 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 134 ) {	// $B%9%^%C%7%eO"BG(B(15)
    if ( phrase == 132 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH/2-0.3;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;

      comPlayer->m_x = -TABLEWIDTH/2+0.5;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = TABLEWIDTH/2-0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    -1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 132 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 132 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 136 ) {	// $B%9%^%C%7%eO"BG(B(16)
    if ( phrase == 134 && mod == 0 ) {
      comPlayer->m_x = TABLEWIDTH/2+0.1;
      comPlayer->m_y = TABLELENGTH/2+0.3;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_targetX = TABLEWIDTH/2-0.1;
      comPlayer->m_targetY = -TABLELENGTH/2+0.1;

      theBall.Warp( 0.0, TABLELENGTH/2-0.5, TABLEHEIGHT+0.01,
		    1.6, 2.2, -5.0, 0.0, 0 );
    }

    theBall.Move();
    thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

    comPlayer->m_status = 200;

    if ( phrase == 134 && mod == 1 ) {
      comPlayer->StartSwing( 0.5 );
    }

    if ( phrase == 134 && mod == 26 ) {	// $BBG5e(B0.1$BICA0(B
      comPlayer->Swing( 0.0 );
      comPlayer->m_pow = 10;
    }
  } else if ( phrase < 168 ) {	// $B%9%m!<%b!<%7%g%s(B(1)
    static Ball ball1, ball2;
    if ( phrase == 136 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH*3/2;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;
      thePlayer->m_swing = 1;
      thePlayer->m_swingType = SWING_CUT;

      comPlayer->m_x = 0.0;
      comPlayer->m_y = TABLELENGTH/2;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_swing = 1;

      ball1.Warp( -0.3, -TABLELENGTH/2-1.2, TABLEHEIGHT+NETHEIGHT,
		  0.0, -8.2, -1.0, 0.0, 3 );
      ball2.Warp( 0.0, TABLELENGTH/4, TABLEHEIGHT+0.01,
		    -1.6, 3.5, -5.0, 0.0, 0 );
    }

    if ( phrase%2 ) {
      theBall = ball1;
      if ( (mod%15) == 0 ) {
	if ( thePlayer->m_swing == 10 )
	  thePlayer->Swing( 0.0 );

	theBall.Move();
	thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
      }
      ball1 = theBall;
    } else {
      theBall = ball2;
      if ( (mod%15) == 0 ) {
	if ( comPlayer->m_swing == 10 )
	  comPlayer->Swing( 0.0 );

	theBall.Move();
	comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
      }
      ball2 = theBall;
    }
  } else if ( phrase < 200 ) {	// $B%9%m!<%b!<%7%g%s(B(2)
    static Ball ball1, ball2;
    if ( phrase == 168 && mod == 0 ) {
      thePlayer->m_x = 0.0;
      thePlayer->m_y = -TABLELENGTH*3/2;
      thePlayer->m_vx = 0.0;
      thePlayer->m_vy = 0.0;
      thePlayer->m_swing = 1;

      comPlayer->m_x = 0.0;
      comPlayer->m_y = TABLELENGTH/2;
      comPlayer->m_vx = 0.0;
      comPlayer->m_vy = 0.0;
      comPlayer->m_swing = 1;

      ball1.Warp( 0.3, -TABLELENGTH/2-1.2, TABLEHEIGHT+NETHEIGHT,
		  0.0, -8.2, -1.0, 0.0, 3 );
      ball2.Warp( 0.0, TABLELENGTH/4, TABLEHEIGHT+0.01,
		    1.6, 3.5, -5.0, 0.0, 0 );
    }

    if ( phrase%2 ) {
      theBall = ball1;
      if ( (mod%15) == 0 ) {
	if ( thePlayer->m_swing == 10 )
	  thePlayer->Swing( 0.0 );

	theBall.Move();
	thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
      }
      ball1 = theBall;
    } else {
      theBall = ball2;
      if ( (mod%15) == 0 ) {
	if ( comPlayer->m_swing == 10 )
	  comPlayer->Swing( 0.0 );

	theBall.Move();
	comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
      }
      ball2 = theBall;
    }
  } else if ( phrase < 216 ) {	// $B;_$a3((B, $B2s$j9~$_(B (1)
    if ( phrase == 200 && mod == 0 ) {
      thePlayer->m_x = -1.2;
      thePlayer->m_y = -TABLELENGTH*3/2;
      thePlayer->m_swing = 2;
      thePlayer->m_swingType = SWING_CUT;

      comPlayer->m_x = 0.0;
      comPlayer->m_y = TABLELENGTH*3/4;
      comPlayer->m_swing = 30;
      comPlayer->m_swingType = SWING_DRIVE;

      theBall.Warp( 0.2, 0.1, TABLEHEIGHT+NETHEIGHT*2,
		    -0.1, -3.0, 1.0, 0.6, 0 );
    }

    if ( (m_count%10) == 0 ) {
      theBall.Move();
      thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
      comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
    }
  } else if ( phrase < 232 ) {	// $B;_$a3((B, $B2s$j9~$_(B (2)
    if ( phrase == 216 && mod == 0 ) {
      thePlayer->m_x = 1.2;
      thePlayer->m_y = -TABLELENGTH;
      thePlayer->m_swing = 28;
      thePlayer->m_swingType = SWING_CUT;

      comPlayer->m_x = -1.0;
      comPlayer->m_y = TABLELENGTH/2;
      comPlayer->m_swing = 1;
      comPlayer->m_swingType = SWING_DRIVE;

      theBall.Warp( 0.6, -TABLELENGTH/2, TABLEHEIGHT+NETHEIGHT,
		    -1.0, 2.0, 3.0, -0.8, 0 );
    }

    if ( (m_count%10) == 0 ) {
      theBall.Move();
      thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
      comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
    }
  }

#ifdef WIN32
  if ( m_count == 0 )
    theSound.PlayBGM();
#endif

  m_count++;

#ifdef HAVE_LIBESD
  //while ( m_count >= (m_bgmCount+441*2*2)/(441*2*2) )
  if ( m_count >= (m_bgmCount+441*2*2)/(441*2*2) )
    m_bgmCount += theSound.SkipBGM();

  if ( m_count >= m_bgmCount/(441*2*2) )
    m_bgmCount += theSound.PlayBGM();

#endif

  if ( KeyHistory[Histptr] == 27 || MouseBHistory[Histptr] ) {	// ESC
    mode = MODE_TITLE;
  }

  return true;
}

bool
Opening::LookAt( double &srcX, double &srcY, double &srcZ,
		 double &destX, double &destY, double &destZ ) {
  long phrase, mod;
  GetPhrase( phrase, mod );

  if ( phrase < 16 ) {
    srcX = TABLELENGTH*2*cos(m_count*3.14159265/720.0);
    srcY = TABLELENGTH*2*sin(m_count*3.14159265/720.0) + TABLELENGTH/2;
    srcZ = TABLEHEIGHT*4;
  } else if ( phrase < 24 ) {
    srcX = thePlayer->GetX()-2.0*cos((m_count-800)*3.141592/800.0);
    srcY = thePlayer->GetY()+2.0*sin((m_count-800)*3.141592/800.0);
    srcZ = (double)(m_count-800)/400.0+0.5;
    destX = thePlayer->GetX();
    destY = thePlayer->GetY();
    destZ = 1.5;
  } else if ( phrase < 32 ) {
    srcX = comPlayer->GetX()+2.0*cos((m_count-800)*3.141592/800.0);
    srcY = comPlayer->GetY()-2.0*sin((m_count-800)*3.141592/800.0);
    srcZ = (double)(m_count-1200)/400.0+0.5;
    destX = comPlayer->GetX();
    destY = comPlayer->GetY();
    destZ = 1.5;
  } else if ( phrase < 48 ) {
    srcX = thePlayer->GetX();
    srcY = thePlayer->GetY()-1.5;
    srcZ = TABLEHEIGHT+1.15;
    destX = thePlayer->GetX()+0.3;
    destY = thePlayer->GetY();
    destZ = TABLEHEIGHT+0.15;
  } else if ( phrase < 56 ) {
    srcX = thePlayer->GetX()+(1.0+0.5*(2800-m_count)/400.0)*
      sin((m_count-2400)*3.141592/800.0);
    srcY = thePlayer->GetY()-(1.0+0.5*(2800-m_count)/400.0)*
      cos((m_count-2400)*3.141592/800.0);
    srcZ = TABLEHEIGHT+1.15-(m_count-2400)/400.0;
    destX = thePlayer->GetX()+0.3;
    destY = thePlayer->GetY();
    destZ = TABLEHEIGHT+0.15;
  } else if ( phrase < 72 ) {
    srcX = thePlayer->GetX()+1.0;
    srcY = thePlayer->GetY();
    srcZ = TABLEHEIGHT+0.15;
    destX = thePlayer->GetX()+0.3;
    destY = thePlayer->GetY();
    destZ = TABLEHEIGHT+0.15;
  } else if ( phrase < 76 ) {
    srcX = comPlayer->GetX()-1.3;
    srcY = comPlayer->GetY()-1.0;
    srcZ = TABLEHEIGHT+NETHEIGHT+0.5;
    destX = comPlayer->GetX()-0.3;
    destY = comPlayer->GetY();
    destZ = TABLEHEIGHT+NETHEIGHT;
  } else if ( phrase < 80 ) {
    srcX = thePlayer->GetX()+1.3;
    srcY = thePlayer->GetY()+2.0;
    srcZ = TABLEHEIGHT+NETHEIGHT+0.5;
    destX = thePlayer->GetX()+0.3;
    destY = thePlayer->GetY();
    destZ = TABLEHEIGHT+NETHEIGHT;
  } else if ( phrase < 84 ) {
    srcX = theBall.GetX();
    srcY = theBall.GetY()-1.0;
    srcZ = theBall.GetZ()+0.1;
    destX = comPlayer->GetX()-0.3;
    destY = comPlayer->GetY();
    destZ = theBall.GetZ();
  } else if ( phrase < 88 ) {
    srcX = thePlayer->GetX()+1.3;
    srcY = thePlayer->GetY()+2.0;
    srcZ = TABLEHEIGHT+NETHEIGHT+0.5;
    destX = thePlayer->GetX()+0.3;
    destY = thePlayer->GetY();
    destZ = TABLEHEIGHT+NETHEIGHT;
  } else if ( phrase < 92 ) {
    srcX = theBall.GetX();
    srcY = theBall.GetY()-1.0;
    srcZ = theBall.GetZ()+0.1;
    destX = comPlayer->GetX()-0.3;
    destY = comPlayer->GetY();
    destZ = theBall.GetZ();
  } else if ( phrase < 96 ) {
    srcX = thePlayer->GetX();
    srcY = thePlayer->GetY()-0.3;
    srcZ = TABLEHEIGHT+NETHEIGHT+0.5;
    destX = 0.0;
    destY = TABLELENGTH/2;
    destZ = TABLEHEIGHT;
  } else if ( phrase < 100 ) {
    srcX = -TABLELENGTH/2-0.2;
    srcY = 0.0;
    srcZ = TABLEHEIGHT+NETHEIGHT+0.5;
    destX = theBall.GetX();
    destY = theBall.GetY();
    destZ = TABLEHEIGHT+NETHEIGHT;
  } else if ( phrase < 104 ) {
    srcX = comPlayer->GetX();
    srcY = comPlayer->GetY()+0.5;
    srcZ = TABLEHEIGHT+NETHEIGHT+0.5;
    destX = theBall.GetX();
    destY = theBall.GetY();
    destZ = theBall.GetZ();
  } else if ( phrase < 112 ) {
    srcX = -TABLEWIDTH/2-1.5;
    srcY = -TABLELENGTH;
    srcZ = TABLEHEIGHT+NETHEIGHT+0.5;
    destX = theBall.GetX();
    destY = theBall.GetY();
    destZ = theBall.GetZ();
  } else if ( phrase < 120 ) {
    srcX = 0.0;
    srcY = -TABLELENGTH*2;
    srcZ = 2.5;
    destX = theBall.GetX();
    destY = theBall.GetY();
    destZ = theBall.GetZ();
  } else if ( phrase < 128 ) {
    srcX = TABLELENGTH/2+1.5;
    srcY = TABLELENGTH/2+0.3;
    srcZ = TABLEHEIGHT;
    destX = theBall.GetX();
    destY = theBall.GetY();
    destZ = theBall.GetZ();
  } else if ( phrase < 136 ) {
    srcX = -TABLEWIDTH/2-1.5;
    srcY = -TABLELENGTH;
    srcZ = TABLEHEIGHT+NETHEIGHT+0.5;
    destX = theBall.GetX();
    destY = theBall.GetY();
    destZ = theBall.GetZ();
  } else if ( phrase < 168 ) {
    if ( phrase%2 ) {
      srcX = thePlayer->GetX() - 1.0;
      srcY = thePlayer->GetY() + 1.0;
      srcZ = TABLEHEIGHT+NETHEIGHT+0.3;
      destX = thePlayer->GetX();
      destY = thePlayer->GetY();
      destZ = 1.0;
    } else {
      srcX = comPlayer->GetX() - 1.0;
      srcY = comPlayer->GetY() - 1.0;
      srcZ = TABLEHEIGHT+NETHEIGHT+0.3;
      destX = comPlayer->GetX();
      destY = comPlayer->GetY();
      destZ = 1.0;
    }
  } else if ( phrase < 200 ) {
    if ( phrase%2 ) {
      srcX = thePlayer->GetX() + 1.0;
      srcY = thePlayer->GetY() + 1.0;
      srcZ = TABLEHEIGHT+NETHEIGHT+0.3;
      destX = thePlayer->GetX();
      destY = thePlayer->GetY();
      destZ = 1.0;
    } else {
      srcX = comPlayer->GetX() + 1.0;
      srcY = comPlayer->GetY() - 1.0;
      srcZ = TABLEHEIGHT+NETHEIGHT+0.3;
      destX = comPlayer->GetX();
      destY = comPlayer->GetY();
      destZ = 1.0;
    }
  } else if ( phrase < 216 ) {
    srcX = TABLELENGTH*1.5*cos(((phrase-200)*35.65+mod)*3.14159265/720.0);
    srcY = TABLELENGTH*1.5*sin(((phrase-200)*35.65+mod)*3.14159265/720.0);
    srcZ = TABLEHEIGHT*2;
    destX = 0.0;
    destY = 0.0;
    destZ = TABLEHEIGHT;
  } else if ( phrase < 232 ) {
    srcX = TABLELENGTH*1.5*cos(3.14159265-((phrase-200)*35.65+mod)*3.14159265/720.0);
    srcY = TABLELENGTH*1.5*sin(3.14159265-((phrase-200)*35.65+mod)*3.14159265/720.0);
    srcZ = TABLEHEIGHT*2;
    destX = 0.0;
    destY = 0.0;
    destZ = TABLEHEIGHT;
  } else {
    srcX = -TABLEWIDTH/2-1.5;
    srcY = -TABLELENGTH;
    srcZ = TABLEHEIGHT+NETHEIGHT+0.5;
    destX = theBall.GetX();
    destY = theBall.GetY();
    destZ = theBall.GetZ();
  }

  return true;
}

void
Opening::GetPhrase( long &phrase, long &mod ) {
  if ( m_count <= 3200 ) {
    phrase = m_count/50;
    mod = m_count%50;
  } else if ( m_count <= 3292 ) {
    phrase = 64;
    mod = 0;
  } else {
    phrase = 67 + (m_count-3292)/35.65;
    mod = (m_count-3292) - (phrase-67)*35.65;
  }
}
