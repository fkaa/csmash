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

#ifndef _Event_
#define _Event_

// Event Handle Class$B$NDj5A(B

class Event {
public:

  Event();
  ~Event();

  void IdleFunc();
  void KeyboardFunc( unsigned char key, int x, int y );
  void KeyUpFunc( unsigned char key, int x, int y );
  void MotionFunc( long x, long y );
  void ButtonFunc( long button, long state, long x, long y );

#ifdef PTHREAD
  void MoveCursor();
#endif
protected:
  unsigned long m_KeyHistory[MAX_HISTORY];		// $B%-!<F~NOMzNr(B
  long m_MouseXHistory[MAX_HISTORY];			// $B%^%&%9MzNr(B
  long m_MouseYHistory[MAX_HISTORY];			// $B%^%&%9MzNr(B
  unsigned long m_MouseBHistory[MAX_HISTORY];		// $B%^%&%9%\%?%sMzNr(B
  unsigned int  m_Histptr;
};

#endif	// _Event_
