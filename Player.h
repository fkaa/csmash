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

#ifndef _Player_
#define _Player_

// m_playerType
#define PLAYER_PROTO		0	// $B%W%m%H%?%$%W(BPlayer
#define PLAYER_PENATTACK	1	// $BN"%Z%sB.96(B
#define PLAYER_SHAKECUT		2	// $B%+%C%H(B
#define PLAYER_PENDRIVE		3	// $B%Z%s%I%i%$%V(B


// m_swingType
#define SWING_NORMAL	0	// $B4pACBG$A(B
#define SWING_POKE	1	// $B%D%C%D%-(B($BK\Ev$O(Bpush$B$i$7$$$,(B...)
#define SWING_SMASH	2	// $B6/BG(B
#define SWING_DRIVE	3	// $B%I%i%$%V(B
#define SWING_CUT	4	// $B%+%C%H(B
#define SWING_BLOCK	5	// $B%V%m%C%/(B, $B%9%H%C%W(B

// m_swingError
#define SWING_PERFECT	0
#define SWING_GREAT	1
#define SWING_GOOD	2
#define SWING_BOO	3
#define SWING_MISS	4

// Player Class$B$NDj5A(B

class PlayerView;
class HitMark;

class Player {
  friend class Howto;
public:
  Player();
  Player( long side );
  Player( long playerType, long side, double x, double y, double z, 
	  double vx, double vy, double vz,long status, long swing, 
	  long swingType, long afterSwing, long swingError, 
	  double targetX, double targetY, double eyeX, double eyeY,
	  double eyeZ, long pow, double spin, double stamina );

  virtual ~Player();

  virtual bool Init();

  virtual bool Reset( struct PlayerData *p );

  virtual bool Move( unsigned long *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  virtual bool AddStatus( long diff );

  virtual long   GetSide();
  virtual long   GetPlayerType();

  virtual double GetX();
  virtual double GetY();
  virtual double GetZ();
  virtual double GetVX();
  virtual double GetVY();
  virtual double GetVZ();
  virtual long   GetPower();
  virtual double GetSpin();
  virtual double GetTargetX();
  virtual double GetTargetY();
  virtual double GetEyeX();
  virtual double GetEyeY();
  virtual double GetEyeZ();
  virtual double GetStamina();
  virtual long   GetStatus();
  virtual long   GetSwing();
  virtual long   GetSwingType();
  virtual long   GetSwingError();
  virtual long   GetAfterSwing();

  virtual bool   GetShoulder( double &x, double &y, double &deg );
  virtual bool   GetElbow( double &degx, double& degy );
  virtual bool   GetHand( double &degx, double &degy, double &degz );

// Get{Shoulder|Elbow|Hand}$B$+$i8F$S=P$5$l$k(B. 
// $B%\!<%k$N0LCV$+$i%U%)%"$GBG$D$+%P%C%/$+$rA*Br(B. 
// true  -> $B%U%)%"(B
// false -> $B%P%C%/(B
  virtual bool ForeOrBack();

  virtual bool Warp( double x, double y, double z,
		     double vx, double vy, double vz );
  virtual bool ExternalSwing( long pow, double spin, long swingType, long swing );

  virtual bool Warp( char *buf );
  virtual bool ExternalSwing( char *buf );

  virtual bool SendSwing( int sd );
  virtual bool SendLocation( int sd );
  virtual bool SendAll( int sd );

protected:
  long m_playerType;	// Player$B$N<oN`(B

  long m_side;		// 1  --- $B<jA0B&(B( y < 0 )
			// -1 --- $B1|(B( y > 0 )

  double m_x;		// player$B$N0LCV(B($B;kE@$N0LCV(B)
  double m_y;
  double m_z;
  double m_vx;		// player$B$NB.EY(B
  double m_vy;
  double m_vz;

  long m_status;	// $BBN@*%2!<%8CM(B
  long m_swing;		// $B%9%$%s%0$N>uBV(B
  long m_swingType;	// $B%9%$%s%0$N<oN`(B
  long m_afterSwing;	// $B%9%$%s%08e$N9ED>;~4V(B
  long m_swingError;	// $B%\!<%k$rBG$C$?$H$-$N8m:9(B. 
                        // 0 --- Perfect
                        // 1 --- Great
                        // 2 --- Good
                        // 3 --- Boo
                        // 4 --- Miss
  double m_targetX;	// $BBG5e$NL\I8Mn2<E@(B
  double m_targetY;	// $BBG5e$NL\I8Mn2<E@(B

  double m_eyeX;	// $B;kE@(B
  double m_eyeY;
  double m_eyeZ;

  long m_pow;		// $B<e(B, $BCf(B, $B6/(B
  double m_spin;	// $B%H%C%W(B/$B%P%C%/%9%T%s(B

  double m_stamina;	// $BBNNO(B

  PlayerView* m_View;

  HitMark *m_hitMark;

// Move$B$+$i8F$S=P$5$l$k(B
  virtual bool KeyCheck( unsigned long *KeyHistory, long *MouseXHistory,
			 long *MouseYHistory, unsigned long *MouseBHistory,
			 int Histptr );		// $B%-!<F~NO=hM}(B
  virtual bool Swing( long power, double spin ) = 0;
  virtual bool StartSwing( long power, double spin ) = 0;

  virtual bool HitBall() = 0;

  virtual bool SwingError();
};

#endif // _Player_
