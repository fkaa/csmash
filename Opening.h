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

#ifndef _Opening_
#define _Opening_
#include "Control.h"
#include "OpeningView.h"

class Opening : public Control {
public:
  Opening();
  virtual ~Opening();

  virtual bool Init();

  static Opening* Create();

  virtual bool Move( unsigned long *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  virtual bool LookAt( double &srcX, double &srcY, double &srcZ,
		       double &destX, double &destY, double &destZ );

  long GetCount() { return m_count; };
  void GetPhrase( long &phrase, long &mod );

  virtual bool IsPlaying() { return false; };

  virtual View *GetView() { return m_View; };
protected:
  OpeningView *m_View;
  long m_count;
  long m_bgmCount;
  long m_pid;
};

#endif	// _Opening_
