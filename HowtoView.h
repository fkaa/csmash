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

#ifndef _HowtoView_
#define _HowtoView_

class HowtoView : public View {
public:
  HowtoView();
  virtual ~HowtoView();

  bool Init( Howto * );

  virtual bool Redraw();
  virtual bool RedrawAlpha();

protected:
  Howto       *m_howto;
  GLuint       m_offset;
  static GLuint m_textures[4];
  ImageData    m_mouse[4];
  GLubyte      m_howtoText[8][400*100/8];
  GLubyte      m_arrow[4][50*(50/8+1)];
  GLuint       m_keyboard[1];
};

#endif	// _HowtoView_
