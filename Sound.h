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

#ifndef _Sound_
#define _Sound_

class Sound {
public:
  Sound();
  virtual ~Sound();

  virtual bool Init( long sndMode );

  bool Play( char *sndData, long count );
  bool Play( long soundID );

  long GetSoundMode();
  bool SetSoundMode( long mode );

  long InitBGM( char *filename );
  long PlayBGM();
  long SkipBGM();

private:
#ifdef HAVE_LIBESD
  int m_fd[5];
  int m_sndfd;

  int m_ossfd;

  // BGM$BMQ(B
  int m_bgminfd;
  int m_bgmoutfd;
#endif
#ifdef WIN32
  // BGM $BMQ(B
  char *m_bgmSound;
#endif

#ifdef HAVE_LIBSDL_MIXER
  Mix_Chunk *m_sound[16];
#else
  char *m_sound[16];
  int  m_soundSize[16];
#endif

  long m_soundMode;
};

#endif // _Sound_
