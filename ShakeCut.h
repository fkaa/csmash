/**
 * @file
 * @brief Definition of ShakeCut class. 
 * @author KANNA Yoshihiro
 * @version $Id$
 */

// Copyright (C) 2000, 2004  $B?@Fn(B $B5H9((B(Kanna Yoshihiro)
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

#ifndef _ShakeCut_
#define _ShakeCut_
#include "Player.h"

/**
 * ShakeCut class is a Player class of shake cut type player. 
 */
class ShakeCut : public Player {
public:
  ShakeCut();
  ShakeCut(long side);
  ShakeCut( long playerType, long side, const vector3d x, const vector3d v,
	    long status, long swing, long swingType, bool swingSide, long afterSwing,
	    long swingError, const vector2d target, const vector3d eye,
	    long pow, const vector2d spin, double stamina, long statusMax );

  virtual ~ShakeCut();

  virtual bool AddStatus( long diff );

  virtual bool Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  virtual bool GetModifiedTarget( vector2d &target );

  virtual void CalcLevel( Ball *ball, double &diff, double &level, double &maxVy );
protected:
  virtual bool Swing( long spin );
  virtual bool StartSwing( long spin );

  virtual bool HitBall();

private:
  bool SwingType( Ball *ball, long spin );
};

#endif // _ShakeCut_
