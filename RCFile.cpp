/* $Id$ */

// Copyright (C) 2001  $B?@Fn(B $B5H9((B(Kanna Yoshihiro)
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
#include "RCFile.h"

RCFile* RCFile::m_rcFile = NULL;

RCFile::RCFile() {
  isTexture = true;
  isWireFrame = true;
  fullScreen = false;
  gmode = GMODE_FULL;
  gameLevel = LEVEL_EASY;
  gameMode = GAME_21PTS;
  sndMode = SOUND_SDL;

  serverName[0] = '\0';
  nickname[0] = '\0';
  message[0] = '\0';

  csmash_port = CSMASH_PORT;

}

RCFile*
RCFile::GetRCFile() {
  if ( !RCFile::m_rcFile )
    RCFile::m_rcFile = new RCFile();

  return m_rcFile;
}

bool
RCFile::ReadRCFile() {
  FILE *fp = OpenRCFile( "r" );
  char buf[1024];
  char *p;

  while ( fgets( buf, 1024, fp ) ) {
    if ( strncmp( buf, "fullscreen=", 11 ) == 0 ) {
      if ( buf[11] == '1' )
	fullScreen = true;
      else
	fullScreen = false;
    } else if ( strncmp( buf, "graphics=", 9 ) == 0 ) {
      if ( strncmp( &buf[9], "simple", 6 ) == 0 ) {
	gmode = GMODE_SIMPLE;
	isTexture = false;
      } else if ( strncmp( &buf[9], "2D", 2 ) == 0 ) {
	gmode = GMODE_2D;
      }
    } else if ( strncmp( buf, "server=", 7 ) == 0 ) {
      strncpy( serverName , &buf[7], 256 );
      if ( (p = strchr( serverName, '\r' )) )
	*p = '\0';
      else if ( (p = strchr( serverName, '\n' )) )
	*p = '\0';
    } else if ( strncmp( buf, "nickname=", 9 ) == 0 ) {
      strncpy( nickname , &buf[9], 32 );
      if ( (p = strchr( nickname, '\r' )) )
	*p = '\0';
      else if ( (p = strchr( nickname, '\n' )) )
	*p = '\0';
    } else if ( strncmp( buf, "message=", 8 ) == 0 ) {
      strncpy( message , &buf[8], 64 );
      if ( (p = strchr( message, '\r' )) )
	*p = '\0';
      else if ( (p = strchr( message, '\n' )) )
	*p = '\0';
    } else if ( strncmp( buf, "wireframe=", 10 ) == 0 ) {
      if ( buf[10] == '1' )
	isWireFrame = true;
      else
	isWireFrame = false;
    } else if ( strncmp( buf, "gamelevel=", 10 ) == 0 ) {
      gameLevel = buf[10]-'0';
      if ( gameLevel > LEVEL_TSUBORISH || gameLevel < LEVEL_EASY )
	gameLevel = LEVEL_EASY;
    } else if ( strncmp( buf, "gamemode=", 9 ) == 0 ) {
      gameMode = buf[9]-'0';
      if ( gameMode > GAME_21PTS || gameMode < GAME_5PTS )
	gameMode = GAME_21PTS;
    } else if ( strncmp( buf, "soundmode=", 10 ) == 0 ) {
      sndMode = buf[9]-'0';
      if ( sndMode > SOUND_SDL || sndMode < SOUND_NONE )
	sndMode = SOUND_NONE;
    }
  }

  fclose( fp );

  return true;
}

bool
RCFile::WriteRCFile() {
  FILE *fp = OpenRCFile( "w" );

  fprintf( fp, "fullscreen=%d\n", fullScreen ? 1 : 0 );

  fprintf( fp, "graphics=" );
  if ( gmode == GMODE_SIMPLE )
    fprintf( fp, "simple\n" );
  else if ( gmode == GMODE_2D )
    fprintf( fp, "2D\n" );
  else
    fprintf( fp, "full\n" );

  fprintf( fp, "server=%s\n", serverName );
  fprintf( fp, "nickname=%s\n", nickname );
  fprintf( fp, "message=%s\n", message );

  fprintf( fp, "wireframe=%d\n", isWireFrame ? 1 : 0 );
  fprintf( fp, "gamelevel=%d\n", gameLevel );
  fprintf( fp, "gamemode=%d\n", gameMode );
  fprintf( fp, "soundmode=%d\n", sndMode );

  fclose( fp );

  return true;
}

FILE *
RCFile::OpenRCFile( char *mode ) {
  char *csmashrc;

  csmashrc = getenv( "CSMASH_RC" );

  if ( csmashrc == NULL ) {
    if ( getenv( "HOME" ) == NULL ) {
#ifdef WIN32
      csmashrc = new char[256+12];
      GetWindowsDirectory( csmashrc, 256 );
      strcat( csmashrc, "\\csmash.ini" );
#else
      fprintf( stderr, "No home directory.\n" );
      exit(1);
#endif
    } else {
#ifdef WIN32
      csmashrc = new char[strlen(getenv("HOME"))+12];
      sprintf( csmashrc, "%s\\csmash.ini", getenv("HOME") );
#else
      csmashrc = new char[strlen(getenv("HOME"))+12];
      sprintf( csmashrc, "%s/.csmashrc", getenv("HOME") );
#endif
    }
  }

  FILE *fp = fopen( csmashrc, mode );
  if ( fp == NULL ) {
    fp = fopen( csmashrc, "w" );	// touch
  }

  if ( fp == NULL ) {
    fprintf( stderr, "Cannot open rc file %s.\n", csmashrc );
    exit(1);
  }

  return fp;
}
