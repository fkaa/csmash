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

#ifndef _PlayerSelect_
#define _PlayerSelect_

#define PLAYERS 3

class PlayerSelectView;

class PlayerSelect {
public:
  PlayerSelect();
  virtual ~PlayerSelect();

  bool Init();

  virtual bool Move( unsigned long *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  long GetRotate();
  long GetSelected();
protected:
  PlayerSelectView *m_View;
  long m_rotate;	// $BI=<(2hLL$N2sE>3QEY(B
  long m_selected;	// $BA*Br$5$l$?(B : m_selected > 0
};

#endif	// _PlayerSelect_