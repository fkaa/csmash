/* $Id$ */

// Copyright (C) 2001  �_�� �g�G(Kanna Yoshihiro)
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

#ifndef _PracticeSelect_
#define _PracticeSelect_

#include "PlayerSelect.h"
#include "PracticeSelectView.h"

class PracticeSelect : public PlayerSelect {
public:
  PracticeSelect();
  virtual ~PracticeSelect();

  virtual bool Init();

  static void Create();

  virtual bool Move( unsigned long *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  virtual long GetOpponentRotate() { return m_opponentRotate; };
  virtual long GetOpponentNum();
  virtual long GetOpponentSelected() { return m_opponentSelected; };

protected:
  long m_opponentRotate;	// Rotation of players
  long m_opponentSelected;	// Selected : m_selected > 0
};

#endif	// _PracticeSelect_
