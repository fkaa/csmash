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

#ifndef _Title_
#define _Title_

// major menu
#define MENU_MAIN	0
#define MENU_CONFIG	1

// minor menu
#define MENU_ALL		0	// $B2hLLFb$NA4%a%K%e!<?t(B
#define MENU_CONFIG_LEVEL	1	// $B%l%Y%k(B(Config$B%a%K%e!<Fb(B)
#define MENU_CONFIG_MODE	2	// $B%b!<%I(B(Config$B%a%K%e!<Fb(B)
#define MENU_CONFIG_SOUND	3	// $B%5%&%s%I(B(Config$B%a%K%e!<Fb(B)

class TitleView;

//class Title : public Control {
class Title : public Control {
public:
  Title();
  virtual ~Title();

  virtual bool Init();

  static Title* Create();

  virtual bool Move( unsigned long *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  long GetSelected();
  long GetSelectMode();
  long GetCount();
  long GetMenuNum( long major, long minor=0 );

  virtual bool LookAt( double &srcX, double &srcY, double &srcZ,
		       double &destX, double &destY, double &destZ );

  virtual bool IsPlaying() { return false; };
protected:
  TitleView *m_View;
  long m_selected;	// $BA*Br$5$l$?(B : m_selected > 0
  long m_selectMode;	// $B4pK\A*Br(B   : 0
			// config$BA*Br(B  : 1
  long m_count;
};

#endif	// _Title_
