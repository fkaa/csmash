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

#ifndef _PlayerView_
#define _PlayerView_

class partsmotion;

class PlayerView : public View {
public:
  PlayerView();
  virtual ~PlayerView();

  bool Init( Player *player );

  virtual bool Redraw();
  virtual bool RedrawAlpha();

private:
  Player  *m_player;	// Model

  partsmotion *m_Fnormal;
  partsmotion *m_Bnormal;
  partsmotion *m_Fdrive;
  partsmotion *m_Bdrive;
  partsmotion *m_Fcut;
  partsmotion *m_Bcut;
  partsmotion *m_Fpeck;
  partsmotion *m_Bpeck;
  partsmotion *m_Fsmash;
  partsmotion *m_Bsmash;

  bool SubRedraw();
};

#endif	// _PlayerView
