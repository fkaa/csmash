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
#include "TitleView.h"
#include "Title.h"
#include "MenuItem.h"
#include "LoadImage.h"
#include "Sound.h"
#include "BaseView.h"

extern long gameLevel;
extern long gameMode;

extern Sound theSound;

extern bool isWireFrame;

TitleView::TitleView() {
  m_View = NULL;
}

TitleView::~TitleView() {
}

bool
TitleView::Init( Title *title ) {
  int i, j, k;

  static char configTitle[][30] = {"images/LevelSelect.ppm",
				   "images/ModeSelect.ppm"
//				   ,"images/SoundSelect.ppm" };
				    };

#ifndef HAVE_LIBZ
  FILE *fp;
#else
  gzFile fp;
#endif

  ImageData image;

  m_title = title;

  for ( i = 0 ; i < 2 ; i++ ) {
#ifndef HAVE_LIBZ
    if( (fp = fopen(&configTitle[i][0], "r")) == NULL ){
      return false;
    }
#else
    if (NULL == (fp = gzopenx(&configTitle[i][0], "rs"))) return false;
#endif

    for ( j = 34 ; j >= 0 ; j-- ) {
      for ( k = 0 ; k < 200/8 ; k++ ) {
	m_configTitle[i][j*25+k] = strtol( getWord(fp), NULL, 16 );
      }
    }

#ifndef HAVE_LIBZ
    fclose(fp);
#else
    gzclose(fp);
#endif
  }

  return true;
}

bool
TitleView::AddView( View *view ) {
  view->m_next = m_View;
  m_View = view;

  return true;
}

bool
TitleView::RemoveView( View *view ) {
  View* _view = m_View;

  if ( _view == view ){
    m_View = _view->m_next;
    return true;
  }

  while ( _view ){
    if ( _view->m_next == view ){
      _view->m_next = view->m_next;
      return true;
    }
    _view = _view->m_next;
  }

  return false;
}
      
bool
TitleView::Redraw() {
  View *view;

  view = m_View;
  while ( view ){
    view->Redraw();
    view = view->m_next;
  }

  return true;
}

bool
TitleView::RedrawAlpha() {
  View *view;
  int i;

  glColor4f( 1.0, 1.0, 1.0, 0.0 );

  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0, (GLfloat)BaseView::GetWinWidth(),
	      0.0, (GLfloat)BaseView::GetWinHeight() );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDepthMask(0);

  switch ( m_title->GetSelectMode() ) {
  case MENU_MAIN:
    glColor4f( 1.0, 1.0, 0.0, 0.0 );
    glBegin(GL_TRIANGLES);
    glVertex2i( m_title->GetSelected()->GetX()-80,
		m_title->GetSelected()->GetY() );
    glVertex2i( m_title->GetSelected()->GetX()-20,
		m_title->GetSelected()->GetY()+35 );
    glVertex2i( m_title->GetSelected()->GetX()-80,
		m_title->GetSelected()->GetY()+70 );
    glEnd();
    break;
  case MENU_CONFIG:
    // Title
    glColor4f( 0.0, 0.2, 0.0, 1.0 );
    glBegin(GL_QUADS);
    glVertex2i(  30, 250 );
    glVertex2i( 330, 250 );
    glVertex2i( 330, 570 );
    glVertex2i(  30, 570 );

    glVertex2i( 430, 320 );
    glVertex2i( 730, 320 );
    glVertex2i( 730, 570 );
    glVertex2i( 430, 570 );

    glVertex2i( 430, 120 );
    glVertex2i( 730, 120 );
    glVertex2i( 730, 300 );
    glVertex2i( 430, 300 );
    glEnd();

    glColor4f( 1.0, 1.0, 1.0, 0.0 );
    glRasterPos2i( 80, 520 );
    glBitmap( 200, 35, 0.0, 0.0, 0.0, 0, &m_configTitle[0][0] );
    glRasterPos2i( 480, 520 );
    glBitmap( 200, 35, 0.0, 0.0, 0.0, 0, &m_configTitle[1][0] );
//    glRasterPos2i( 480, 250 );
//    glBitmap( 200, 35, 0.0, 0.0, 0.0, 0, &m_configTitle[2][0] );

    glColor4f( 1.0, 1.0, 0.0, 0.0 );
    glBegin(GL_TRIANGLES);
    if ( m_title->GetSelected()->GetHeight() == 70 ) {
      glVertex2i( m_title->GetSelected()->GetX()-80,
		  m_title->GetSelected()->GetY() );
      glVertex2i( m_title->GetSelected()->GetX()-20,
		  m_title->GetSelected()->GetY()+35 );
      glVertex2i( m_title->GetSelected()->GetX()-80,
		  m_title->GetSelected()->GetY()+70 );
    } else {
      glVertex2i( m_title->GetSelected()->GetX()-40,
		  m_title->GetSelected()->GetY() );
      glVertex2i( m_title->GetSelected()->GetX()-10,
		  m_title->GetSelected()->GetY()+18 );
      glVertex2i( m_title->GetSelected()->GetX()-40,
		  m_title->GetSelected()->GetY()+36 );
    }
    glEnd();
    break;
  }

  glDepthMask(1);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  view = m_View;
  while ( view ){
    view->RedrawAlpha();
    view = view->m_next;
  }

  return true;
}
