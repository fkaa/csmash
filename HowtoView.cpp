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

extern long winWidth;
extern long winHeight;

GLuint HowtoView::m_textures[4] = {0, 0, 0, 0};

HowtoView::HowtoView() {
}

HowtoView::~HowtoView() {
}

bool
HowtoView::Init( Howto *howto ) {
  ImageData image;
  int i, j, k;
#ifndef HAVE_LIBZ
  FILE *fp;
#else
  gzFile fp;
#endif

  static char fname[][30] = {"images/Mouse1.ppm", "images/Mouse2.ppm",
			     "images/Mouse3.ppm", "images/Mouse4.ppm"};
  static char arrowname[][30] = {"images/rightArrow.ppm",
				 "images/downArrow.ppm",
				 "images/leftArrow.ppm",
				 "images/upArrow.ppm"};
  static char howtoText[][30] = {"images/Howto1.ppm", "images/Howto2.ppm", 
				 "images/Howto3.ppm", "images/Howto4.ppm", 
				 "images/Howto5.ppm"};

  m_howto = howto;

  if ( m_textures[0] == 0 ) {
    glGenTextures( 4, m_textures );

    for ( i = 0 ; i < 4 ; i++ ){
      image.LoadPPM( &(fname[i][0]) );
      for ( j = 0 ; j < image.GetWidth() ; j++ ) {
	for ( k = 0 ; k < image.GetHeight() ; k++ ) {
	  if ( image.GetPixel( j, k, 0 ) != 0 ||
	       image.GetPixel( j, k, 1 ) != 0 ||
	       image.GetPixel( j, k, 2 ) != 0 )
	    image.SetPixel( j, k, 3 , 0 );
	}
      }

      glBindTexture(GL_TEXTURE_2D, m_textures[i] );
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glTexImage2D(GL_TEXTURE_2D, 0, 4, image.GetWidth(), image.GetHeight(), 
		   0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetImage() );
    }
  }

  for ( i = 0 ; i < 5 ; i++ ) {
#ifndef HAVE_LIBZ
    if( (fp = fopen(&howtoText[i][0], "r")) == NULL ){
      return false;
    }
#else
    if (NULL == (fp = gzopenx(&howtoText[i][0], "rs"))) return false;
#endif

    for ( j = 99 ; j >= 0 ; j-- ) {
      for ( k = 0 ; k < 400/8 ; k++ ) {
	m_howtoText[i][j*50+k] = strtol( getWord(fp), NULL, 16 );
      }
    }

#ifndef HAVE_LIBZ
    fclose(fp);
#else
    gzclose(fp);
#endif
  }

  for ( i = 0 ; i < 4 ; i++ ) {
#ifndef HAVE_LIBZ
    if( (fp = fopen(arrowname[i], "r")) == NULL ){
      return false;
    }
#else
    if (NULL == (fp = gzopenx(arrowname[i], "rs"))) return false;
#endif

    for ( j = 49 ; j >= 0 ; j-- ) {
      for ( k = 0 ; k < 50/8+1 ; k++ ) {
	m_arrow[i][j*(50/8+1)+k] = strtol( getWord(fp), NULL, 16 );
      }
    }

#ifndef HAVE_LIBZ
    fclose(fp);
#else
    gzclose(fp);
#endif
  }

  glGenTextures( 1, m_keyboard );
  image.LoadPPM( "images/Keyboard.ppm" );
  for ( j = 0 ; j < image.GetWidth() ; j++ ) {
    for ( k = 0 ; k < image.GetHeight() ; k++ ) {
      if ( image.GetPixel( j, k, 0 ) == 255 ||
	   image.GetPixel( j, k, 1 ) == 255 ||
	   image.GetPixel( j, k, 2 ) == 255 )
	image.SetPixel( j, k, 3 , 0 );
    }
  }

  glBindTexture(GL_TEXTURE_2D, m_keyboard[0]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTexImage2D(GL_TEXTURE_2D, 0, 4, image.GetWidth(), image.GetHeight(),
	       0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetImage() );

  return true;
}

bool
HowtoView::Redraw() {
#if 0
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0, (GLfloat)winWidth, 0.0, (GLfloat)winHeight );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);

  switch ( m_howto->GetMode() ) {	// $BGX7J(B
  case 0:
  case 1:
  case 3:
  case 4:
    glColor4f( 0.0, 0.0, 0.0, 0.5 );
    glBegin(GL_QUADS);
    glVertex2i(             0,               0);
    glVertex2i(IMAGE_WIDTH+50,               0);
    glVertex2i(IMAGE_WIDTH+50, IMAGE_HEIGHT+50);
    glVertex2i(             0, IMAGE_HEIGHT+50);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textures[m_howto->GetMouseB()] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex2i( m_howto->GetMouseX(), m_howto->GetMouseY() );
    glTexCoord2f(1.0, 1.0);
    glVertex2i( IMAGE_WIDTH+m_howto->GetMouseX(), m_howto->GetMouseY() );
    glTexCoord2f(1.0, 0.0);
    glVertex2i( IMAGE_WIDTH+m_howto->GetMouseX(),
		IMAGE_HEIGHT+m_howto->GetMouseY() );
    glTexCoord2f(0.0, 0.0);
    glVertex2i( m_howto->GetMouseX(), IMAGE_HEIGHT+m_howto->GetMouseY() );
    glEnd();
    glDisable(GL_TEXTURE_2D);

    break;
  case 2:
    if ( m_howto->GetCount() > 100 && m_howto->GetCount() < 1000 ) {
      glColor4f( 0.0, 0.0, 0.0, 1.0 );

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_keyboard[0] );
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 1.0); glVertex2i( 190, -300 );
      glTexCoord2f(1.0, 1.0); glVertex2i( 610, -300 );
      glTexCoord2f(1.0, 0.0); glVertex2i( 610,  300 );
      glTexCoord2f(0.0, 0.0); glVertex2i( 190,  300 );
      glEnd();
      glDisable(GL_TEXTURE_2D);
    }
    break;
  }

  switch ( m_howto->GetMode() ) {
  case 0:
    glColor4f( 1.0, 1.0, 1.0, 0.0 );
    switch ( (m_howto->GetCount()%400)/100 ) {
    case 0:
      glRasterPos2i( IMAGE_WIDTH, IMAGE_HEIGHT/2 );
      break;
    case 1:
      glRasterPos2i( IMAGE_WIDTH/2, 0 );
      break;
    case 2:
      glRasterPos2i( 0, IMAGE_HEIGHT/2 );
      break;
    case 3:
      glRasterPos2i( IMAGE_WIDTH/2, IMAGE_HEIGHT );
      break;
    }

    glBitmap( 50, 50, 0.0, 0.0, 0.0, 0, 
	      &m_arrow[(m_howto->GetCount()%400)/100][0] );
    break;
  case 2:
    glColor4f( 0.0, 0.5, 0.0, 1.0 );
    if ( m_howto->GetCount() > 200 && m_howto->GetCount() < 1000 &&
	 m_howto->GetCount()%100 > 0 && m_howto->GetCount()%100 < 30 ) {
      glBegin(GL_QUADS);
      glVertex2i( 202+m_howto->GetMouseX()*36, 297-m_howto->GetMouseY()*72 );
      glVertex2i( 235+m_howto->GetMouseX()*36, 297-m_howto->GetMouseY()*72 );
      glVertex2i( 235+m_howto->GetMouseX()*36, 252-m_howto->GetMouseY()*72 );
      glVertex2i( 202+m_howto->GetMouseX()*36, 252-m_howto->GetMouseY()*72 );
      glEnd();
    }
    break;
  case 4:
    glColor4f( 1.0, 1.0, 1.0, 0.0 );
    glRasterPos2i( 470, 170 );

    glBitmap( 50, 50, 0.0, 0.0, 0.0, 0, &m_arrow[1][0] );
    break;
  }

  glColor4f( 1.0, 1.0, 1.0, 0.0 );
  if ( m_howto->GetCount()%100 < 75 ) {
    glRasterPos2i( 10, 450 );
    glBitmap( 400, 100, 0.0, 0.0, 0.0, 0,
	      &m_howtoText[m_howto->GetMode()][0] );
  }

  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
#endif

  return true;
}

bool
HowtoView::RedrawAlpha() {
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0, (GLfloat)winWidth, 0.0, (GLfloat)winHeight );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);

  switch ( m_howto->GetMode() ) {	// $BGX7J(B
  case 0:
  case 1:
  case 3:
  case 4:
    glColor4f( 0.0, 0.0, 0.0, 0.5 );
    glBegin(GL_QUADS);
    glVertex2i(             0,               0);
    glVertex2i(IMAGE_WIDTH+50,               0);
    glVertex2i(IMAGE_WIDTH+50, IMAGE_HEIGHT+50);
    glVertex2i(             0, IMAGE_HEIGHT+50);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textures[m_howto->GetMouseB()] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex2i( m_howto->GetMouseX(), m_howto->GetMouseY() );
    glTexCoord2f(1.0, 1.0);
    glVertex2i( IMAGE_WIDTH+m_howto->GetMouseX(), m_howto->GetMouseY() );
    glTexCoord2f(1.0, 0.0);
    glVertex2i( IMAGE_WIDTH+m_howto->GetMouseX(),
		IMAGE_HEIGHT+m_howto->GetMouseY() );
    glTexCoord2f(0.0, 0.0);
    glVertex2i( m_howto->GetMouseX(), IMAGE_HEIGHT+m_howto->GetMouseY() );
    glEnd();
    glDisable(GL_TEXTURE_2D);

    break;
  case 2:
    if ( m_howto->GetCount() > 100 && m_howto->GetCount() < 1000 ) {
      glColor4f( 0.0, 0.0, 0.0, 1.0 );

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_keyboard[0] );
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 1.0); glVertex2i( 190, -300 );
      glTexCoord2f(1.0, 1.0); glVertex2i( 610, -300 );
      glTexCoord2f(1.0, 0.0); glVertex2i( 610,  300 );
      glTexCoord2f(0.0, 0.0); glVertex2i( 190,  300 );
      glEnd();
      glDisable(GL_TEXTURE_2D);
    }
    break;
  }

  switch ( m_howto->GetMode() ) {
  case 0:
    glColor4f( 1.0, 1.0, 1.0, 0.0 );
    switch ( (m_howto->GetCount()%400)/100 ) {
    case 0:
      glRasterPos2i( IMAGE_WIDTH, IMAGE_HEIGHT/2 );
      break;
    case 1:
      glRasterPos2i( IMAGE_WIDTH/2, 0 );
      break;
    case 2:
      glRasterPos2i( 0, IMAGE_HEIGHT/2 );
      break;
    case 3:
      glRasterPos2i( IMAGE_WIDTH/2, IMAGE_HEIGHT );
      break;
    }

    glBitmap( 50, 50, 0.0, 0.0, 0.0, 0, 
	      &m_arrow[(m_howto->GetCount()%400)/100][0] );
    break;
  case 2:
    glColor4f( 0.0, 0.5, 0.0, 1.0 );
    if ( m_howto->GetCount() > 200 && m_howto->GetCount() < 1000 &&
	 m_howto->GetCount()%100 > 0 && m_howto->GetCount()%100 < 30 ) {
      glBegin(GL_QUADS);
      glVertex2i( 202+m_howto->GetMouseX()*36, 297-m_howto->GetMouseY()*72 );
      glVertex2i( 235+m_howto->GetMouseX()*36, 297-m_howto->GetMouseY()*72 );
      glVertex2i( 235+m_howto->GetMouseX()*36, 252-m_howto->GetMouseY()*72 );
      glVertex2i( 202+m_howto->GetMouseX()*36, 252-m_howto->GetMouseY()*72 );
      glEnd();
    }
    break;
  case 4:
    glColor4f( 1.0, 1.0, 1.0, 0.0 );
    glRasterPos2i( 470, 170 );

    glBitmap( 50, 50, 0.0, 0.0, 0.0, 0, &m_arrow[1][0] );
    break;
  }

  glColor4f( 1.0, 1.0, 1.0, 0.0 );
  if ( m_howto->GetCount()%100 < 75 ) {
    glRasterPos2i( 10, 450 );
    glBitmap( 400, 100, 0.0, 0.0, 0.0, 0,
	      &m_howtoText[m_howto->GetMode()][0] );
  }

  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  return true;
}