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
#ifdef HAVE_LIBZ
#include "z.h"
#endif

ImageData::ImageData() {
  m_image = NULL;
  m_width = m_height = m_bytes = 0;
}

ImageData::ImageData( long width, long height, long bytes) {
  m_image = new GLubyte[width*height*bytes];

  if ( m_image ) {
    m_width = width;
    m_height = height;
    m_bytes = bytes;
  }
}

ImageData::~ImageData() {
  if ( m_image )
    delete m_image;
}

GLubyte
ImageData::GetPixel( long width, long height, long bytes ) {
  if ( width > m_width || width < 0 || height > m_height || height < 0 ||
       bytes > m_bytes || bytes < 0 )
    return 0;

  return m_image[height*m_width*m_bytes + width*m_bytes + bytes];
}

bool
ImageData::SetPixel( long width, long height, long bytes , GLubyte val ) {
  if ( width > m_width || width < 0 || height > m_height || height < 0 ||
       bytes > m_bytes || bytes < 0 )
    return false;

  m_image[height*m_width*m_bytes + width*m_bytes + bytes] = val;
  return true;
}

bool
ImageData::LoadPPM( char* filename ) {
  int i, j;
#ifndef HAVE_LIBZ
  FILE *fp;
#else
  gzFile fp;
#endif
  char *w;
  int pb;

#ifndef HAVE_LIBZ
  if( (fp = fopen(filename, "r")) == NULL ) {
    return false;
  }
#else
  if (NULL == (fp = gzopenx(filename, "rs"))) return false;
#endif

  w = getWord(fp);
  if( !strcmp("P3", w) )
    pb = 3;
  else if( !strcmp("P2", w) )
    pb = 2;
  else if ( !strcmp("P5", w) )
    pb = 5;
  else if ( !strcmp("P6", w) )
    pb = 6;
  else
    return false;

  m_width = atoi( getWord(fp) );
  m_height = atoi( getWord(fp) );
  m_bytes = 4;

  getWord( fp );

  if ( m_image )
    delete m_image;

  m_image = new GLubyte[m_width*m_height*m_bytes];
  if ( !m_image ) {
    m_width = m_height = m_bytes = 0;
    return false;
  }

  if ( pb == 5 ) {
    GLubyte b;
    for ( i = 0 ; i < m_height ; i++ ){
      for ( j = 0 ; j < m_width ; j++ ){
#ifndef HAVE_LIBZ
	fread( &b , 1, 1, fp );
#else
	gzread(fp, &b, 1 * 1);
#endif
	SetPixel( j, i, 0, b );
	SetPixel( j, i, 1, b );
	SetPixel( j, i, 2, b );
	SetPixel( j, i, 3, 255 );
      }
    }
  } else if ( pb == 6 ) {
    GLubyte b;
    for ( i = 0 ; i < m_height ; i++ ){
      for ( j = 0 ; j < m_width ; j++ ){
#ifndef HAVE_LIBZ
	fread( &b , 1, 1, fp ); SetPixel( j, i, 0, b );
	fread( &b , 1, 1, fp ); SetPixel( j, i, 1, b );
	fread( &b , 1, 1, fp ); SetPixel( j, i, 2, b );
#else
	gzread(fp, &b , 1 * 1); SetPixel( j, i, 0, b );
	gzread(fp, &b , 1 * 1); SetPixel( j, i, 1, b );
	gzread(fp, &b , 1 * 1); SetPixel( j, i, 2, b );
#endif
	SetPixel( j, i, 3, 255 );
      }
    }
  } else {
    for ( i = 0 ; i < m_height ; i++ ){
      for ( j = 0 ; j < m_width ; j++ ){
	if ( pb == 3 ) {
	  SetPixel( j, i, 0, (GLubyte)atoi(getWord( fp ) ) );
	  SetPixel( j, i, 1, (GLubyte)atoi(getWord( fp ) ) );
	  SetPixel( j, i, 2, (GLubyte)atoi(getWord( fp ) ) );
	  SetPixel( j, i, 3, 255 );
	} else if ( pb == 2 ) {
	  SetPixel( j, i, 0, (GLubyte)atoi(getWord( fp ) ) );
	  SetPixel( j, i, 1, (GLubyte)atoi(getWord( fp ) ) );
	  SetPixel( j, i, 2, (GLubyte)atoi(getWord( fp ) ) );
	  SetPixel( j, i, 3, (GLubyte)atoi(getWord( fp ) ) );
	}
      }
    }
  }

#ifndef HAVE_LIBZ
  fclose( fp );
#else
  gzclose(fp);
#endif

  return true;
}
  

#ifdef HAVE_LIBZ
char*
getWord( gzFile fp ) {
  static char buf[256];
  static char* ptr = buf;
  char* ptr2;

  while(1) {
    if( *ptr == 0 || *ptr == '#' ) {
      gzgets(fp, buf, 256);
      ptr = buf;
      continue;
    } else if( isspace(*ptr) ) {
      ptr++;
      continue;
    } else
      break;
  }

  ptr2 = ptr;
  while( !isspace(*ptr) && *ptr != 0 ) {
    ptr++;
  }
  *ptr = 0;
  ptr++;

  return ptr2;
}
#else
char*
getWord( FILE *fp ) {
  static char buf[256];
  static char* ptr = buf;
  char* ptr2;

  while(1) {
    if( *ptr == 0 || *ptr == '#' ) {
      fgets( buf, 256, fp );
      ptr = buf;
      continue;
    } else if( isspace(*ptr) ) {
      ptr++;
      continue;
    } else
      break;
  }

  ptr2 = ptr;
  while( !isspace(*ptr) && *ptr != 0 ) {
    ptr++;
  }
  *ptr = 0;
  ptr++;

  return ptr2;
}
#endif