/* $Id$ */

// Copyright (C) 2000, 2001  $B?@Fn(B $B5H9((B(Kanna Yoshihiro)
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
#include "FieldView.h"
#include "LoadImage.h"

extern bool isLighting;
extern bool isTexture;
extern long gmode;

FieldView::FieldView() {
}

FieldView::~FieldView() {
}

bool
FieldView::Init() {
  int i;
// Set texture
  ImageData image;

  image.LoadFile( "images/Floor.jpg" );

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures( 1, &m_floor );
  glBindTexture( GL_TEXTURE_2D, m_floor );
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  glTexImage2D(GL_TEXTURE_2D, 0, 3, image.GetWidth(), image.GetHeight(),
	       0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetImage() );

  static char pname[][30] = {"images/Left.jpg", "images/Front.jpg", 
			     "images/Right.jpg", "images/Back.jpg" };

  glGenTextures( 4, m_wall );
  for ( i = 0 ; i < 4 ; i++ ) {
    image.LoadFile( &(pname[i][0]) );

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture( GL_TEXTURE_2D, m_wall[i] );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, image.GetWidth(), image.GetHeight(),
		 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetImage() );
  }

  m_offset = glGenLists(1);

  glNewList( m_offset, GL_COMPILE );

  if (gmode == GMODE_SIMPLE) {
    glBegin(GL_LINE_LOOP);			// Draw floor
      glNormal3f( 0.0F, 0.0F, 1.0F );
      glVertex3f( -AREAXSIZE,  AREAYSIZE, 0.0F );
      glVertex3f( -AREAXSIZE, -AREAYSIZE, 0.0F );
      glVertex3f(  AREAXSIZE, -AREAYSIZE, 0.0F );
      glVertex3f(  AREAXSIZE,  AREAYSIZE, 0.0F );
    glEnd();
  } else {
    glBindTexture( GL_TEXTURE_2D, m_floor );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glBegin(GL_QUADS);				// Draw floor
      glNormal3f( 0.0F, 0.0F, 1.0F );
      glTexCoord2f(0.0F, 0.0F); glVertex3f( -AREAXSIZE*2,  AREAYSIZE*2, 0.0F );
      glTexCoord2f(4.0F, 0.0F); glVertex3f( -AREAXSIZE*2, -AREAYSIZE*2, 0.0F );
      glTexCoord2f(4.0F, 4.0F); glVertex3f(  AREAXSIZE*2, -AREAYSIZE*2, 0.0F );
      glTexCoord2f(0.0F, 4.0F); glVertex3f(  AREAXSIZE*2,  AREAYSIZE*2, 0.0F );
    glEnd();
  }

  glEndList();

  return true;
}

bool
FieldView::Redraw() {
  if ( isTexture )
    glEnable(GL_TEXTURE_2D);

  if ( gmode != GMODE_SIMPLE )
    glColor4f(0.8F, 0.8F, 0.8F, 0.0F);
  else
    glColor4f(0.6F, 0.4F, 0.2F, 0.0F);

  glCallList( m_offset );

  if ( gmode == GMODE_SIMPLE ) {
    glBegin(GL_LINE_LOOP);			// Draw wall
      glNormal3f( 1.0F, 0.0F, 0.0F );
      glVertex3f( -AREAXSIZE,  AREAYSIZE, 0.0F );
      glVertex3f( -AREAXSIZE, -AREAYSIZE, 0.0F );
      glVertex3f( -AREAXSIZE, -AREAYSIZE, AREAZSIZE);
      glVertex3f( -AREAXSIZE,  AREAYSIZE, AREAZSIZE);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glNormal3f( 0.0F, 1.0F, 0.0F );
      glVertex3f( -AREAXSIZE, -AREAYSIZE, 0.0F );
      glVertex3f(  AREAXSIZE, -AREAYSIZE, 0.0F );
      glVertex3f(  AREAXSIZE, -AREAYSIZE, AREAZSIZE);
      glVertex3f( -AREAXSIZE, -AREAYSIZE, AREAZSIZE);
    glEnd();

    glBegin(GL_LINE_LOOP);
      glNormal3f( -1.0F, 0.0F, 0.0F );
      glVertex3f(  AREAXSIZE, -AREAYSIZE, 0.0F );
      glVertex3f(  AREAXSIZE,  AREAYSIZE, 0.0F );
      glVertex3f(  AREAXSIZE,  AREAYSIZE, AREAZSIZE);
      glVertex3f(  AREAXSIZE, -AREAYSIZE, AREAZSIZE);
    glEnd();

    glBegin(GL_LINE_LOOP);
      glNormal3f( 0.0F, -1.0F, 0.0F );
      glVertex3f(  AREAXSIZE, AREAYSIZE, 0.0F );
      glVertex3f( -AREAXSIZE, AREAYSIZE, 0.0F );
      glVertex3f( -AREAXSIZE, AREAYSIZE, AREAZSIZE);
      glVertex3f(  AREAXSIZE, AREAYSIZE, AREAZSIZE);
    glEnd();
  } else {
    glBindTexture( GL_TEXTURE_2D, m_wall[0] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glBegin(GL_QUADS);			// Draw wall
      glNormal3f( 1.0F, 0.0F, 0.0F );
      glTexCoord2f(0.0F, 1.0F); glVertex3f( -AREAXSIZE,  AREAYSIZE, 0.0F );
      glTexCoord2f(1.0F, 1.0F); glVertex3f( -AREAXSIZE, -AREAYSIZE, 0.0F );
      glTexCoord2f(1.0F, 0.0F); glVertex3f( -AREAXSIZE, -AREAYSIZE, AREAZSIZE);
      glTexCoord2f(0.0F, 0.0F); glVertex3f( -AREAXSIZE,  AREAYSIZE, AREAZSIZE);
    glEnd();

    glBindTexture( GL_TEXTURE_2D, m_wall[1] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glBegin(GL_QUADS);
    glNormal3f( 0.0F, 1.0F, 0.0F );
      glTexCoord2f(0.0F, 1.0F); glVertex3f( -AREAXSIZE, -AREAYSIZE, 0.0F );
      glTexCoord2f(1.0F, 1.0F); glVertex3f(  AREAXSIZE, -AREAYSIZE, 0.0F );
      glTexCoord2f(1.0F, 0.0F); glVertex3f(  AREAXSIZE, -AREAYSIZE, AREAZSIZE);
      glTexCoord2f(0.0F, 0.0F); glVertex3f( -AREAXSIZE, -AREAYSIZE, AREAZSIZE);
    glEnd();

    glBindTexture( GL_TEXTURE_2D, m_wall[2] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glBegin(GL_QUADS);
      glNormal3f( -1.0F, 0.0F, 0.0F );
      glTexCoord2f(0.0F, 1.0F); glVertex3f(  AREAXSIZE, -AREAYSIZE, 0.0F );
      glTexCoord2f(1.0F, 1.0F); glVertex3f(  AREAXSIZE,  AREAYSIZE, 0.0F );
      glTexCoord2f(1.0F, 0.0F); glVertex3f(  AREAXSIZE,  AREAYSIZE, AREAZSIZE);
      glTexCoord2f(0.0F, 0.0F); glVertex3f(  AREAXSIZE, -AREAYSIZE, AREAZSIZE);
    glEnd();

    glBindTexture( GL_TEXTURE_2D, m_wall[3] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glBegin(GL_QUADS);
      glNormal3f( 0.0F, -1.0F, 0.0F );
      glTexCoord2f(0.0F, 1.0F); glVertex3f(  AREAXSIZE, AREAYSIZE, 0.0F );
      glTexCoord2f(1.0F, 1.0F); glVertex3f( -AREAXSIZE, AREAYSIZE, 0.0F );
      glTexCoord2f(1.0F, 0.0F); glVertex3f( -AREAXSIZE, AREAYSIZE, AREAZSIZE);
      glTexCoord2f(0.0F, 0.0F); glVertex3f(  AREAXSIZE, AREAYSIZE, AREAZSIZE);
    glEnd();

    if ( isTexture )
      glDisable(GL_TEXTURE_2D);

    glColor4f(0.1F, 0.1F, 0.1F, 0.0F);
    glBegin(GL_QUADS);			// Draw ceiling
      glNormal3f( 0.0F, 0.0F, -1.0F );
      glVertex3f(  AREAXSIZE*2,  AREAYSIZE*2, AREAZSIZE);
      glVertex3f( -AREAXSIZE*2,  AREAYSIZE*2, AREAZSIZE);
      glVertex3f( -AREAXSIZE*2, -AREAYSIZE*2, AREAZSIZE);
      glVertex3f(  AREAXSIZE*2, -AREAYSIZE*2, AREAZSIZE);
    glEnd();

    if ( isLighting ) {
      glColor4f(0.0F, 0.2F, 0.0F, 0.0F);
      static GLfloat mat_board[] = { 0.0F, 0.2F, 0.0F, 0.0F };
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat_board);

      glBegin(GL_QUADS);			// Draw ball stopper
        glNormal3f( 1.0F, 0.0F, 0.0F );
	glVertex3f( -AREAXSIZE/2, -AREAYSIZE/2, 0.0F );
	glVertex3f( -AREAXSIZE/2,  AREAYSIZE/2, 0.0F );
	glVertex3f( -AREAXSIZE/2,  AREAYSIZE/2, TABLEHEIGHT );
	glVertex3f( -AREAXSIZE/2, -AREAYSIZE/2, TABLEHEIGHT );

	glNormal3f( -1.0F, 0.0F, 0.0F );
	glVertex3f(  AREAXSIZE/2, -AREAYSIZE/2, 0.0F );
	glVertex3f(  AREAXSIZE/2,  AREAYSIZE/2, 0.0F );
	glVertex3f(  AREAXSIZE/2,  AREAYSIZE/2, TABLEHEIGHT );
	glVertex3f(  AREAXSIZE/2, -AREAYSIZE/2, TABLEHEIGHT );

	glNormal3f( 0.0F, -1.0F, 0.0F );
	glVertex3f( -AREAXSIZE/2,  AREAYSIZE/2, 0.0F );
	glVertex3f(  AREAXSIZE/2,  AREAYSIZE/2, 0.0F );
	glVertex3f(  AREAXSIZE/2,  AREAYSIZE/2, TABLEHEIGHT );
	glVertex3f( -AREAXSIZE/2,  AREAYSIZE/2, TABLEHEIGHT );

	glNormal3f( 0.0F, 1.0F, 0.0F );
	glVertex3f(  AREAXSIZE/2, -AREAYSIZE/2, 0.0F );
	glVertex3f( -AREAXSIZE/2, -AREAYSIZE/2, 0.0F );
	glVertex3f( -AREAXSIZE/2, -AREAYSIZE/2, TABLEHEIGHT );
	glVertex3f(  AREAXSIZE/2, -AREAYSIZE/2, TABLEHEIGHT );
      glEnd();
    }
  }

  glColor4f(0.4F, 0.4F, 0.4F, 0.0F);
  static GLfloat mat_leg[] = { 0.0F, 0.4F, 0.0F, 0.0F };
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_leg);

  glBegin(GL_QUADS);			// Draw table legs
    glVertex3f(-TABLEWIDTH/2+0.1F, -TABLELENGTH/2+0.1F,  0.0F);
    glVertex3f(-TABLEWIDTH/2+0.1F, -TABLELENGTH/2+0.1F,  TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.1F, -TABLELENGTH/2+0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.1F, -TABLELENGTH/2+0.13F, 0.0F);

    glVertex3f(-TABLEWIDTH/2+0.1F, -TABLELENGTH/2+0.13F, 0.0F);
    glVertex3f(-TABLEWIDTH/2+0.1F, -TABLELENGTH/2+0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F,-TABLELENGTH/2+0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F,-TABLELENGTH/2+0.13F, 0.0F);

    glVertex3f(-TABLEWIDTH/2+0.13F, -TABLELENGTH/2+0.1F, 0.0F);
    glVertex3f(-TABLEWIDTH/2+0.13F, -TABLELENGTH/2+0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F,-TABLELENGTH/2+0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F,-TABLELENGTH/2+0.13F, 0.0F);

    glVertex3f(-TABLEWIDTH/2+0.1F, -TABLELENGTH/2+0.1F,  0.0F);
    glVertex3f(-TABLEWIDTH/2+0.1F, -TABLELENGTH/2+0.1F,  TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F,-TABLELENGTH/2+0.1F,  TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F,-TABLELENGTH/2+0.1F,  0.0F);


    glVertex3f(-TABLEWIDTH/2+0.1F,  TABLELENGTH/2-0.1F,  0.0F);
    glVertex3f(-TABLEWIDTH/2+0.1F,  TABLELENGTH/2-0.1F,  TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.1F,  TABLELENGTH/2-0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.1F,  TABLELENGTH/2-0.13F, 0.0F);

    glVertex3f(-TABLEWIDTH/2+0.1F,  TABLELENGTH/2-0.13F, 0.0F);
    glVertex3f(-TABLEWIDTH/2+0.1F,  TABLELENGTH/2-0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F, TABLELENGTH/2-0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F, TABLELENGTH/2-0.13F, 0.0F);

    glVertex3f(-TABLEWIDTH/2+0.13F, TABLELENGTH/2-0.1F,  0.0F);
    glVertex3f(-TABLEWIDTH/2+0.13F, TABLELENGTH/2-0.1F,  TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F, TABLELENGTH/2-0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F, TABLELENGTH/2-0.13F, 0.0F);

    glVertex3f(-TABLEWIDTH/2+0.1F, TABLELENGTH/2-0.1F, 0.0F);
    glVertex3f(-TABLEWIDTH/2+0.1F, TABLELENGTH/2-0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F, TABLELENGTH/2-0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f(-TABLEWIDTH/2+0.13F, TABLELENGTH/2-0.1F, 0.0F);


    glVertex3f( TABLEWIDTH/2-0.1F,-TABLELENGTH/2+0.1F, 0.0F);
    glVertex3f( TABLEWIDTH/2-0.1F,-TABLELENGTH/2+0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.1F,-TABLELENGTH/2+0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.1F,-TABLELENGTH/2+0.13F, 0.0F);

    glVertex3f( TABLEWIDTH/2-0.1F,-TABLELENGTH/2+0.13F, 0.0F);
    glVertex3f( TABLEWIDTH/2-0.1F,-TABLELENGTH/2+0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F,-TABLELENGTH/2+0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F,-TABLELENGTH/2+0.13F, 0.0F);

    glVertex3f( TABLEWIDTH/2-0.13F,-TABLELENGTH/2+0.1F, 0.0F);
    glVertex3f( TABLEWIDTH/2-0.13F,-TABLELENGTH/2+0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F,-TABLELENGTH/2+0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F,-TABLELENGTH/2+0.13F, 0.0F);

    glVertex3f( TABLEWIDTH/2-0.1F,-TABLELENGTH/2+0.1F, 0.0F);
    glVertex3f( TABLEWIDTH/2-0.1F,-TABLELENGTH/2+0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F,-TABLELENGTH/2+0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F,-TABLELENGTH/2+0.1F, 0.0F);


    glVertex3f( TABLEWIDTH/2-0.1F, TABLELENGTH/2-0.1F, 0.0F);
    glVertex3f( TABLEWIDTH/2-0.1F, TABLELENGTH/2-0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.1F, TABLELENGTH/2-0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.1F, TABLELENGTH/2-0.13F, 0.0F);

    glVertex3f( TABLEWIDTH/2-0.1F, TABLELENGTH/2-0.13F, 0.0F);
    glVertex3f( TABLEWIDTH/2-0.1F, TABLELENGTH/2-0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F, TABLELENGTH/2-0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F, TABLELENGTH/2-0.13F, 0.0F);

    glVertex3f( TABLEWIDTH/2-0.13F, TABLELENGTH/2-0.1F, 0.0F);
    glVertex3f( TABLEWIDTH/2-0.13F, TABLELENGTH/2-0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F, TABLELENGTH/2-0.13F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F, TABLELENGTH/2-0.13F, 0.0F);

    glVertex3f( TABLEWIDTH/2-0.1F, TABLELENGTH/2-0.1F, 0.0F);
    glVertex3f( TABLEWIDTH/2-0.1F, TABLELENGTH/2-0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F, TABLELENGTH/2-0.1F, TABLEHEIGHT-TABLETHICK);
    glVertex3f( TABLEWIDTH/2-0.13F, TABLELENGTH/2-0.1F, 0.0F);
  glEnd();

  return true;
}

// Draw transparent object
bool
FieldView::RedrawAlpha() {
  glColor4f(0.0F, 0.0F, 0.3F, 0.7F);
  static GLfloat mat_table[] = { 0.0F, 0.0F, 0.3F, 1.0F };
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_table);

  glDepthMask(0);
  glBegin(GL_QUADS);			// Draw table
  if ( isLighting && gmode != GMODE_SIMPLE ) {
    glNormal3f( 0.0F, 0.0F, -1.0 );
    glVertex3f( -TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );
    glVertex3f( -TABLEWIDTH/2,  TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );
    glVertex3f(  TABLEWIDTH/2,  TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );
    glVertex3f(  TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );

    glNormal3f( 1.0, 0.0F, 0.0F );
    glVertex3f( TABLEWIDTH/2,  TABLELENGTH/2, TABLEHEIGHT );
    glVertex3f( TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT );
    glVertex3f( TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );
    glVertex3f( TABLEWIDTH/2,  TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );

    glNormal3f( -1.0, 0.0F, 0.0F );
    glVertex3f( -TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT );
    glVertex3f( -TABLEWIDTH/2,  TABLELENGTH/2, TABLEHEIGHT );
    glVertex3f( -TABLEWIDTH/2,  TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );
    glVertex3f( -TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );

    glNormal3f( 0.0F, 1.0, 0.0F );
    glVertex3f( -TABLEWIDTH/2, TABLELENGTH/2, TABLEHEIGHT );
    glVertex3f( -TABLEWIDTH/2, TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );
    glVertex3f(  TABLEWIDTH/2, TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );
    glVertex3f(  TABLEWIDTH/2, TABLELENGTH/2, TABLEHEIGHT );

    glNormal3f( 0.0F, -1.0, 0.0F );
    glVertex3f( -TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT );
    glVertex3f( -TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );
    glVertex3f(  TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT-TABLETHICK );
    glVertex3f(  TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT );
  }

    glNormal3f( 0.0F, 0.0F, 1.0 );
    glVertex3f( -TABLEWIDTH/2,  TABLELENGTH/2, TABLEHEIGHT );
    glVertex3f( -TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT );
    glVertex3f(  TABLEWIDTH/2, -TABLELENGTH/2, TABLEHEIGHT );
    glVertex3f(  TABLEWIDTH/2,  TABLELENGTH/2, TABLEHEIGHT );
  glEnd();
  glDepthMask(1);

  static GLfloat mat_default[] = { 0.0F, 0.0F, 0.0F, 1.0F };
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_default);

  if ( isLighting ) {
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);

    glBegin(GL_QUADS);		// Draw white line
      glNormal3f( 0.0F, 0.0F, 1.0F );
      glVertex3f( -TABLEWIDTH/2,       TABLELENGTH/2, TABLEHEIGHT );
      glVertex3f( -TABLEWIDTH/2,      -TABLELENGTH/2, TABLEHEIGHT );
      glVertex3f( -TABLEWIDTH/2+0.02F, -TABLELENGTH/2+0.02F, TABLEHEIGHT );
      glVertex3f( -TABLEWIDTH/2+0.02F,  TABLELENGTH/2-0.02F, TABLEHEIGHT );

      glNormal3f( 0.0F, 0.0F, 1.0F );
      glVertex3f( -TABLEWIDTH/2,      -TABLELENGTH/2, TABLEHEIGHT );
      glVertex3f(  TABLEWIDTH/2,      -TABLELENGTH/2, TABLEHEIGHT );
      glVertex3f(  TABLEWIDTH/2-0.02F, -TABLELENGTH/2+0.02F, TABLEHEIGHT );
      glVertex3f( -TABLEWIDTH/2+0.02F, -TABLELENGTH/2+0.02F, TABLEHEIGHT );

      glNormal3f( 0.0F, 0.0F, 1.0F );
      glVertex3f(  TABLEWIDTH/2,      -TABLELENGTH/2, TABLEHEIGHT );
      glVertex3f(  TABLEWIDTH/2,       TABLELENGTH/2, TABLEHEIGHT );
      glVertex3f(  TABLEWIDTH/2-0.02F,  TABLELENGTH/2-0.02F, TABLEHEIGHT );
      glVertex3f(  TABLEWIDTH/2-0.02F, -TABLELENGTH/2+0.02F, TABLEHEIGHT );

      glNormal3f( 0.0F, 0.0F, 1.0F );
      glVertex3f(  TABLEWIDTH/2,       TABLELENGTH/2, TABLEHEIGHT );
      glVertex3f( -TABLEWIDTH/2,       TABLELENGTH/2, TABLEHEIGHT );
      glVertex3f( -TABLEWIDTH/2+0.02F,  TABLELENGTH/2-0.02F, TABLEHEIGHT );
      glVertex3f(  TABLEWIDTH/2-0.02F,  TABLELENGTH/2-0.02F, TABLEHEIGHT );
    glEnd();

    glBegin(GL_LINES);
      glVertex3f( 0.0F, -TABLELENGTH/2, TABLEHEIGHT );
      glVertex3f( 0.0F,  TABLELENGTH/2, TABLEHEIGHT );
    glEnd();
  }

  glColor4f(0.0F, 0.2F, 0.0F, 0.9F);

  static GLfloat mat_net[] = { 0.0F, 0.2F, 0.0F, 1.0F };
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_net);
  glBegin(GL_QUADS);			// Draw net
    glNormal3f( 0.0F, 1.0F, 0.0F );
    glVertex3f( -TABLEWIDTH/2-NETHEIGHT, 0.0F, TABLEHEIGHT );
    glVertex3f( -TABLEWIDTH/2-NETHEIGHT, 0.0F, TABLEHEIGHT+NETHEIGHT-0.01F );
    glVertex3f(  TABLEWIDTH/2+NETHEIGHT, 0.0F, TABLEHEIGHT+NETHEIGHT-0.01F );
    glVertex3f(  TABLEWIDTH/2+NETHEIGHT, 0.0F, TABLEHEIGHT );
  glEnd();
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_default);

  if ( isLighting ) {
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);

    glBegin(GL_QUADS);
      glNormal3f( 0.0F, 1.0F, 0.0F );
      glVertex3f( -TABLEWIDTH/2-NETHEIGHT, 0.0F, TABLEHEIGHT+NETHEIGHT );
      glVertex3f( -TABLEWIDTH/2-NETHEIGHT, 0.0F, TABLEHEIGHT+NETHEIGHT-0.01F );
      glVertex3f(  TABLEWIDTH/2+NETHEIGHT, 0.0F, TABLEHEIGHT+NETHEIGHT-0.01F );
      glVertex3f(  TABLEWIDTH/2+NETHEIGHT, 0.0F, TABLEHEIGHT+NETHEIGHT );
    glEnd();
    glColor4f( 0.2F, 1.0F, 0.2F, 1.0F );	// Why? But my RIVA128 need it.
  }

//  glBlendFunc(GL_ONE, GL_SRC_ALPHA);

  return true;
}
