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

#include "ttinc.h"
#include "OpeningView.h"
#include "Player.h"
#include "Opening.h"

extern Player* thePlayer;
extern Player* comPlayer;

OpeningView::OpeningView() {
}

OpeningView::~OpeningView() {
}

bool
OpeningView::Init( Opening *opening ) {
  m_opening = opening;

  return true;
}

bool
OpeningView::Redraw() {
  GLfloat light_intensity_amb[] = { 0.6, 0.6, 0.6, 1.0 };
  GLfloat light_intensity_dif[] = { 1.0, 1.0, 1.0, 1.0 };

  long phrase, mod;

  m_opening->GetPhrase( phrase, mod );

  if ( phrase < 56 ) {
  } else if ( phrase < 64 ) {
    light_intensity_amb[0] *= (3200-m_opening->GetCount())/400.0;
    light_intensity_amb[1] *= (3200-m_opening->GetCount())/400.0;
    light_intensity_amb[2] *= (3200-m_opening->GetCount())/400.0;
    light_intensity_dif[0] *= (3200-m_opening->GetCount())/400.0;
    light_intensity_dif[1] *= (3200-m_opening->GetCount())/400.0;
    light_intensity_dif[2] *= (3200-m_opening->GetCount())/400.0;
  } else if ( phrase < 72 ) {
    if ( phrase >= 68 && mod > 20 ) {
      /*
      light_intensity_amb[0] *= (10-mod)/10.0;
      light_intensity_amb[1] *= (10-mod)/10.0;
      light_intensity_amb[2] *= (10-mod)/10.0;
      light_intensity_dif[0] *= (10-mod)/10.0;
      light_intensity_dif[1] *= (10-mod)/10.0;
      light_intensity_dif[2] *= (10-mod)/10.0;
      */
    } else {
      light_intensity_amb[0] *= 0.0;
      light_intensity_amb[1] *= 0.0;
      light_intensity_amb[2] *= 0.0;
      light_intensity_dif[0] *= 0.0;
      light_intensity_dif[1] *= 0.0;
      light_intensity_dif[2] *= 0.0;
    }
  }

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_intensity_amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_intensity_dif);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_intensity_dif);

  thePlayer->m_View->Redraw();
  comPlayer->m_View->Redraw();

  return true;
}

bool
OpeningView::RedrawAlpha() {
  thePlayer->m_View->RedrawAlpha();
  comPlayer->m_View->RedrawAlpha();

  return true;
}
