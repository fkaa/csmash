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

extern BaseView theView;

extern Player* thePlayer;
extern Player* comPlayer;
extern Ball theBall;
extern Event theEvent;

extern long mode;
extern long gameMode;
extern long wins;

PlayGame::PlayGame() {
  m_Score1 = 0;
  m_Score2 = 0;
}

PlayGame::~PlayGame() {
}

long
PlayGame::GetService() {
  switch ( gameMode ) {
  case GAME_5PTS:
    return ((m_Score1+m_Score2) & 1 ? -1 : 1);
  case GAME_11PTS:
    if ( m_Score1 > 9 && m_Score2 > 9 ) {	// Deuce
      return ((m_Score1+m_Score2) & 1 ? -1 : 1);
    } else {
      if ( (m_Score1 + m_Score2)%10 >= 5 )
	return -1;
      else
	return 1;
    }
  case GAME_21PTS:
    if ( m_Score1 > 19 && m_Score2 > 19 ) {	// Deuce
      return ((m_Score1+m_Score2) & 1 ? -1 : 1);
    } else {
      if ( (m_Score1 + m_Score2)%10 >= 5 )
	return -1;
      else
	return 1;
    }
  }
  return 0;
}

long
PlayGame::GetScore( Player *p ) {
  if ( mode == MODE_SOLOPLAY || mode == MODE_MULTIPLAY ) {
    if ( p->GetSide() > 0 )
      return m_Score1;
    else
      return m_Score2;
  } else {	// Training
    return m_Score1;
  }
}

void
PlayGame::ChangeScore() {
  if ( mode == MODE_SOLOPLAY || mode == MODE_MULTIPLAY ) {
    if ( theBall.GetStatus() == 0 || theBall.GetStatus() == 3 ||
	 theBall.GetStatus() == 4 || theBall.GetStatus() == 6 ) {
      if ( thePlayer->GetSide() > 0 )
	m_Score2++;
      else
	m_Score1++;
    } else {
      if ( thePlayer->GetSide() > 0 )
	m_Score1++;
      else
	m_Score2++;
    }
  }
}

bool
PlayGame::IsGameEnd() {
  switch ( gameMode ) {
  case GAME_5PTS:
    if ( (m_Score1 > 4 || m_Score2 > 4) )
      return true;
    else
      return false;
  case GAME_11PTS:
    if ( (m_Score1 > 10 || m_Score2 > 10) && abs( m_Score1-m_Score2 ) > 1 )
      return true;
    else
      return false;
  case GAME_21PTS:
    if ( (m_Score1 > 20 || m_Score2 > 20) && abs( m_Score1-m_Score2 ) > 1 )
      return true;
    else
      return false;
  }

  return false;
}

void
PlayGame::EndGame() {
  // $B:F=i4|2=$9$k(B
  if ( GetScore(thePlayer) > GetScore(comPlayer) )
    wins++;
  else
    wins = 0;

  if ( wins > 0 )
    mode = MODE_SELECT;
  else
    mode = MODE_TITLE;

  m_Score1 = m_Score2 = 0;
}