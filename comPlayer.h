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

#ifndef _comPlayer_
#define _comPlayer_

class ComPlayer {
public:
  ComPlayer();
  virtual ~ComPlayer();

// 1turn$B8e(B(0.01$BIC(B)$B$N>uBV$K0\9T$9$k(B
//  virtual bool Move( unsigned long *KeyHistory, long *MouseXHistory,
//		     long *MouseYHistory, unsigned long *MouseBHistory,
//		     int Histptr );

protected:
// $B;W9M%k!<%A%s(B
  virtual bool Think() = 0;

// $B;W9M%k!<%A%sMQJQ?t(B
  int _prevBallstatus;		// $B0JA0$N%\!<%k$N>uBV(B
  double _hitX;			// $BBG5eE@(B
  double _hitY;

// $BBG5eE@$r7W;;$9$k(B
  virtual bool Hitarea( double &hitX, double &hitY) = 0;
};

#endif // _comPlayer_