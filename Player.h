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

#ifndef _Player_
#define _Player_
#include "PlayerView.h"
#include "PlayerView2D.h"

#include <vector>
#include "matrix"
typedef Vector<3, double> vector3d;
typedef Vector<2, double> vector2d;

// m_playerType
#define PLAYER_PROTO		0	// Prototype player
#define PLAYER_PENATTACK	1	// Pen Attack
#define PLAYER_SHAKECUT		2	// Cut
#define PLAYER_PENDRIVE		3	// Pen Drive


// m_swingType
#define SWING_NORMAL	0	// 
#define SWING_POKE	1	// push?
#define SWING_SMASH	2	// 
#define SWING_DRIVE	3	// 
#define SWING_CUT	4	// 
#define SWING_BLOCK	5	// 


class PlayerView;
class HitMark;
class Ball;

class Player {
  friend class Howto;
  friend class Opening;
  friend class OpeningView;
public:
  Player();
  Player( long side );
  Player( long playerType, long side, const vector3d x, const vector3d v,
	  long status, long swing, long swingType, bool swingSide, long afterSwing,
	  long swingError, const vector2d target, const vector3d eye,
	  long pow, const vector2d spin, double stamina, long statusMax );

  virtual ~Player();

  void operator=(Player&);

  static Player* Create( long player, long side, long type );

  virtual bool Init();

  //virtual bool Reset( struct PlayerData *p );
  virtual bool Reset( Player *p );

  virtual bool Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  virtual bool AddStatus( long diff );

  virtual View *GetView() { return m_View; };

  virtual long   GetSide() { return m_side; }
  virtual long   GetPlayerType() { return m_playerType; }

  virtual vector3d GetX() { return m_x; }
  virtual vector3d GetV() { return m_v; }
  virtual long   GetPower() { return m_pow; }
  virtual vector2d GetSpin() { return m_spin; }
  virtual vector2d GetTarget() { return m_target; }
  virtual vector3d GetEye() { return m_eye; }
  virtual vector3d GetLookAt() { return m_lookAt; }
  virtual double GetStamina() { return m_stamina; }
  virtual long   GetStatus() { return m_status; }
  virtual long   GetSwing() { return m_swing; }
  virtual long   GetSwingType() { return m_swingType; }
  virtual bool   GetSwingSide() { return m_swingSide; }
  virtual long   GetSwingError() { return m_swingError; }
  virtual long   GetAfterSwing() { return m_afterSwing; }

  virtual long   GetDragX() { return m_dragX; }
  virtual long   GetDragY() { return m_dragY; }

  // true  -> forehand
  // false -> backhand
  virtual bool ForeOrBack();

  virtual bool Warp( const vector3d &x, const vector3d &v );
  virtual bool ExternalSwing( long pow, const vector2d &spin,
			      long swingType, long swing );

  virtual bool Warp( char *buf );
  virtual bool ExternalSwing( char *buf );

  virtual char * SendSwing( char *buf );
  virtual char * SendLocation( char *buf );
  virtual bool SendAll( int sd );

  virtual bool GetModifiedTarget( vector2d &target );

  virtual void CalcLevel( Ball *ball, double &diff, double &level, double &maxVy );

  long StatusBorder();
protected:
  long m_playerType;	// Player type

  long m_side;		// 1  --- ( y < 0 )
			// -1 --- ( y > 0 )

  vector3d m_x;		// player location
  vector3d m_v;		// player velocity

  long m_status;	// status gauge
  long m_swing;		// swing status
  long m_swingType;	// swing type
  bool m_swingSide;	// forehand or backhand
  long m_afterSwing;	// 
  long m_swingError;	// Error when hitting
                        // 0 --- Perfect
                        // 1 --- Great
                        // 2 --- Good
                        // 3 --- Boo
                        // 4 --- Miss
  vector2d m_target;	// location of target circle

  vector3d m_eye;	// Viewpoint

  vector3d m_lookAt;

  long m_pow;		// power
  vector2d m_spin;	// {sidespin, topspin/backspin}

  double m_stamina;

  long m_statusMax;	// Max status value

  long m_dragX;
  long m_dragY;		// Mouse drag

  PlayerView* m_View;

  vector3d m_lastSendX;
  vector3d m_lastSendV;
  long m_lastSendCount;

  virtual bool KeyCheck( SDL_keysym *KeyHistory, long *MouseXHistory,
			 long *MouseYHistory, unsigned long *MouseBHistory,
			 int Histptr );
  virtual bool Swing( long power );
  virtual bool StartSwing( long power );

  virtual bool HitBall();

  virtual bool SwingError();

  void UpdateLastSend();

  void AddError( vector3d &v );
};

#endif // _Player_
