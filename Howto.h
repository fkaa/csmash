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

#ifndef _Howto_
#define _Howto_

class HowtoView;

class Howto : public Control {
public:
  Howto();
  virtual ~Howto();

  bool Init();

  static Howto* Create();

  virtual bool Move( unsigned long *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  long GetMode() {return m_mode;}
  long GetCount() {return m_count;}

  long GetMouseX() {return m_mouseX;}
  long GetMouseY() {return m_mouseY;}
  long GetMouseB() {return m_mouseB;}

  virtual bool LookAt( double &srcX, double &srcY, double &srcZ,
		       double &destX, double &destY, double &destZ );

  virtual bool IsPlaying() { return false; };
protected:
  HowtoView *m_View;
  long m_mode;
  long m_count;

  long m_mouseX;
  long m_mouseY;
  long m_mouseB;

  bool IsMove();
};

#endif	// _Howto_
