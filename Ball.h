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

#ifndef _Ball_
#define _Ball_

// Ball Class$B$NDj5A(B
class Player;
class BallView;

class Ball {
public:
  Ball();
  Ball( double _x, double _y, double _z, double _vx, double _vy, double _vz,
	double _spin, long _status );
  virtual ~Ball();

  virtual bool Init();

  double GetX();
  double GetY();
  double GetZ();
  double GetVX();
  double GetVY();
  double GetVZ();
  double GetSpin();

  long GetStatus();

  long GetService();
  long GetScore( Player *p );	// $B%9%3%"<hF@(B. 

  bool Move();	// 1turn$B8e(B(0.01$BIC(B)$B$N>uBV$K0\9T$9$k(B

  bool Hit( double vx, double vy, double vz, double spin, Player *player );
  						// $BBG5e(B
  bool Toss( Player *player, long power );	// $B%H%9(B

  void Warp( double x, double y, double z, double vx, double vy, double vz, 
	     double spin, long status );
  void Warp( char *buf );

  // $BMn2<L\I8COE@$+$i(Bvy, vz$B$r5a$a$k(B
  bool TargetToV( double targetX, double targetY, double height, double spin, 
		  double &vx, double &vy, double &vz, double vMin = 0.1,
		  double vMax = 30.0 );
  // $B%5!<%VMQ(B
  bool TargetToVS( double targetX, double targetY, double height, double spin, 
		   double &vx, double &vy, double &vz );

  bool IsGameEnd();	// $B%2!<%`=*N;%A%'%C%/(B
  void EndGame();

  bool Send( int sd );

  long m_count;		// Training$BMQ%+%&%s%?(B
protected:
  double m_x;		// ball$B$N0LCV(B
  double m_y;
  double m_z;
  double m_vx;		// ball$B$NB.EY(B
  double m_vy;
  double m_vz;

  double m_spin;	// $B2sE>(B. $B@5(B --- $B%H%C%W%9%T%s(B   $BIi(B --- $B%P%C%/%9%T%s(B

  long m_status;	// 0 --- $B<+J,$,BG$C$F$+$i#1%P%&%s%I$^$G(B
  			// 1 --- $B%3%s%T%e!<%?$,BG$F$k>uBV(B
  			// 2 --- $B%3%s%T%e!<%?$,BG$C$F$+$i#1%P%&%s%I$^$G(B
  			// 3 --- $B<+J,$,BG$F$k>uBV(B
  			// 4 --- $B<+J,$N%5!<%V8e#1%P%&%s%I$^$G(B
  			// 5 --- $B%3%s%T%e!<%?$N%5!<%V8e#1%P%&%s%I$^$G(B
  			// 6 --- $B<+J,$,%H%9$7$?8e(B
  			// 7 --- $B%3%s%T%e!<%?$,%H%9$7$?8e(B
  			// 8 --- player$B$,%5!<%V$9$kA0(B
  			//-1 --- $B%\!<%k%G%C%I(B

  BallView* m_View;

  // $B%9%3%"JQ99(B
  void ChangeScore();

  long m_Score1;	// $B<jA0%W%l%$%d!<$NF@E@(B
  long m_Score2;	// $B1|%W%l%$%d!<$NF@E@(B
};

#endif // _Ball_
