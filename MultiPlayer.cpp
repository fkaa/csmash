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

#ifdef _WIN32

typedef int socklen_t;		/* mimic Penguin's typedef */

#else	/* ! _WIN32 */

#define closesocket(FD) close(FD)

#endif

extern Ball theBall;
extern Player *thePlayer;
extern Player *comPlayer;

extern char serverName[256];

extern long timeAdj;

extern int theSocket;
bool endian;

// endian$BJQ49(B
double
SwapDbl( double d ) {
  if ( endian ) {
    return d;
  } else {
    double swp;
    char *c1 = (char *)&d, *c2 = (char *)&swp;

    for ( int i = 0 ; i < 8 ; i++ )
      *(c2+7-i) = *(c1+i);

    return swp;
  }
}

long
SwapLong( long l ) {
  if ( endian ) {
    return l;
  } else {
    long swp;
    char *c1 = (char *)&l, *c2 = (char *)&swp;

    for ( int i = 0 ; i < 4 ; i++ )
      *(c2+3-i) = *(c1+i);

    return swp;
  }
}

// endian$B%F%9%H(B
void
EndianCheck() {
  long n = 1;
  if ( *((char *)&n) == 1 )
    endian = false;
  else
    endian = true;
}

bool
SendDouble( int sd, double d ) {
  d = SwapDbl(d);

  if ( send( sd, (char *)&d, 8, 0 ) == 8 )
    return true;
  else
    return false;
}

bool
SendLong( int sd, long l ) {
  l = SwapLong(l);

  if ( send( sd, (char *)&l, 4, 0 ) == 4 )
    return true;
  else
    return false;
}

// $B$A$g$C$HJQ$J;EMM$+$J$!(B...
char *
ReadDouble( char *buf, double& d ) {
  memcpy( &d, buf, 8 );
  d = SwapDbl(d);

  return buf+8;
}

char *
ReadLong( char *buf, long& l ) {
  memcpy( &l, buf, 4 );
  l = SwapLong(l);

  return buf+4;
}

void
SendTime( int sd, struct timeb* tb ) {
  long millitm = tb->millitm;
  send( sd, "TM", 2, 0 );
  SendLong( sd, tb->time );
  SendLong( sd, millitm );
}

void
ReadTime( int sd, struct timeb* tb ) {
  char buf[256];
  long len;

  if ( recv( sd, buf, 2, 0 ) != 2 ) {
    xerror("%s(%d) recv", __FILE__, __LINE__);
    exit(1);
  }
  len = 0;
  while (1) {
    if ( (len+=recv( sd, buf+len, 8-len, 0 )) == 8 )
      break;
  }

  char *b = buf;
  long millitm;
  b = ReadLong( b, tb->time );
  b = ReadLong( b, millitm );
  tb->millitm = millitm;
}

// PlayerData$BAw?.(B
void
SendPlayerData() {
  send( theSocket, "PI", 2, 0 );
  thePlayer->SendAll( theSocket );
}

// PlayerData$B<u?.(B
Player *
ReadPlayerData() {
  double x, y, z, vx, vy, vz, spin;
  long playerType, side, swing, swingType, afterSwing, swingError, pow;
  double targetX, targetY, eyeX, eyeY, eyeZ, stamina;
  long stat;
  char buf[256];
  long len;
  Player *player;

  if ( recv( theSocket, buf, 2, 0 ) != 2 )
    return 0;

  if ( strncmp( buf, "PI", 2 ) )
    return 0;

  len = 0;
  while (1) {
    if ( (len+=recv( theSocket, buf+len, 136-len, 0 )) == 136 )
      break;
  }

  char *b = buf;

  b = ReadLong( b, playerType );
  b = ReadLong( b, side );

  b = ReadDouble( b, x );
  b = ReadDouble( b, y );
  b = ReadDouble( b, z );
  b = ReadDouble( b, vx );
  b = ReadDouble( b, vy );
  b = ReadDouble( b, vz );

  b = ReadLong( b, stat );
  b = ReadLong( b, swing );
  b = ReadLong( b, swingType );
  b = ReadLong( b, afterSwing );
  b = ReadLong( b, swingError );

  b = ReadDouble( b, targetX );
  b = ReadDouble( b, targetY );
  b = ReadDouble( b, eyeX );
  b = ReadDouble( b, eyeY );
  b = ReadDouble( b, eyeZ );

  b = ReadLong( b, pow );
  b = ReadDouble( b, spin );
  b = ReadDouble( b, stamina );

  switch ( playerType ) {
  case PLAYER_PENATTACK:
    player = new PenAttack( playerType, side, x, y, z, vx, vy, vz, stat,
			    swing, swingType, afterSwing, swingError,
			    targetX, targetY, eyeX, eyeY, eyeZ, pow,
			    spin, stamina );
    break;
  case PLAYER_SHAKECUT:
    player = new ShakeCut( playerType, side, x, y, z, vx, vy, vz, stat,
			   swing, swingType, afterSwing, swingError,
			   targetX, targetY, eyeX, eyeY, eyeZ, pow,
			   spin, stamina );
    break;
  case PLAYER_PENDRIVE:
    player = new PenDrive( playerType, side, x, y, z, vx, vy, vz, stat,
			   swing, swingType, afterSwing, swingError,
			   targetX, targetY, eyeX, eyeY, eyeZ, pow,
			   spin, stamina );
    break;
  default:
    return 0;
  }

  return player;
}

bool
AcceptClient() {
  int sock;
  socklen_t fromlen;
  struct sockaddr_in saddr, faddr;

  if ( (sock = socket( PF_INET, SOCK_STREAM, 0 )) < 0 ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    exit(1);
  }

  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(CSMASH_PORT);
  if ( bind( sock, (struct sockaddr *)&saddr, sizeof(saddr) ) < 0 ) {
    xerror("%s(%d) bind", __FILE__, __LINE__);
    exit(1);
  }

  int sb;
  struct sockaddr_in sba;
  if (0 > (sb = socket(PF_INET, SOCK_DGRAM, 0))) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    exit(1);
  }
  sba.sin_addr.s_addr = INADDR_ANY;
  sba.sin_family = AF_INET;
  sba.sin_port = htons(CSMASH_PORT);
  if (0 > bind(sb, (struct sockaddr*)&sba, sizeof(sba))) {
    xerror("%s(%d) bind", __FILE__, __LINE__);
    exit(1);
  }

  if ( listen( sock, 1 ) < 0 ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    exit(1);
  }

  // wait for connection / broadcast packet
  printf("server selecting\n");
  do {
    int max;

    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
    FD_SET(sb, &fd);

    max = sock > sb ? sock : sb;
    if (0 <= select(max+1, &fd, NULL, NULL, NULL)) {
      if (FD_ISSET(sb, &fd)) {
	// datagram to udp port
	printf("server recived broadcast packet");
	char buf[1];
	sockaddr_in client;
	socklen_t l = sizeof(client);

	if ( 0 <= recvfrom(sb, (char*)&buf, sizeof(buf), 0,
			  (sockaddr*)&client, &l)) {
	  printf("%s\n", inet_ntoa(client.sin_addr));
	  struct hostent *h;
	  char buf[128];
	  if (0 == gethostname(buf, sizeof(buf))) {
	    h = gethostbyname(buf);
	    if (h) {
	      sockaddr_in a;
	      memcpy(&a.sin_addr, h->h_addr, h->h_length);
	      printf("send %s\n", inet_ntoa(a.sin_addr));
	      l = sizeof(client);
	      sendto(sb, (char*)&a.sin_addr.s_addr, sizeof(a.sin_addr.s_addr),
		     0, (sockaddr*)&client, l);
	    }
	  }
	}
      }
      else {
	// connection to tcp port
	break;
      }
    } else {
      // select error... No, Timeout
      xerror("%s(%d): select");
    }
  } while (1);
  closesocket(sb);

  fromlen = sizeof(faddr);
  theSocket = accept( sock, (struct sockaddr *)&faddr, &fromlen );

  if (0 > theSocket) {
    xerror("%s(%d) accept", __FILE__, __LINE__);
    exit(1);
  }

  return true;
}

void
StartServer() {
  int i;
  char buf[256];
  long len;

  AcceptClient();

  // $B%?%$%^D4@0(B
  for ( i = 0 ; i < 100 ; i++ ) {
#ifndef WIN32
    struct timeval tv;
    struct timezone tz;
#endif
    struct timeb tb1, tb2, tb3;
    long diff;

#ifdef WIN32
    ftime( &tb1 );
#else
    gettimeofday( &tv, &tz );
    tb1.time = tv.tv_sec;
    tb1.millitm = tv.tv_usec/1000;
#endif

    SendTime( theSocket, &tb1 );
    ReadTime( theSocket, &tb3 );

#ifdef WIN32
    ftime( &tb2 );
#else
    gettimeofday( &tv, &tz );
    tb2.time = tv.tv_sec;
    tb2.millitm = tv.tv_usec/1000;
#endif
    diff = (long)(tb2.time-tb1.time)*1000 + tb2.millitm-tb1.millitm;
    long mtm = tb1.millitm+diff;
    while ( mtm >= 1000 ) {
      mtm -= 1000;
      tb1.time++;
    }
    while ( mtm < 1000 ) {
      mtm += 1000;
      tb1.time--;
    }
    tb1.millitm = mtm;

    timeAdj += (tb3.time-tb1.time)*1000 + tb3.millitm-tb1.millitm;
  }

  timeAdj /= 1000;	/* 100*10 */

  // Ball Data$B$NFI$_9~$_(B
  if ( recv( theSocket, buf, 2, 0 ) != 2 ) {
    xerror("%s(%d) recv", __FILE__, __LINE__);
    exit(1);
  }

  if ( strncmp( buf, "BI", 2 ) )
    exit(1);

  len = 0;
  while (1) {
    if ( (len+=recv( theSocket, buf+len, 60-len, 0 )) == 60 )
      break;
  }

  theBall.Warp( buf );

  SendPlayerData();

  if ( !(comPlayer = ReadPlayerData()) )
    exit(1);
}

void
StartClient() {
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));

  if (1 == serverName[0]) { // Broadcast mode
    struct sockaddr_in sba;
    memset(&sba, 0, sizeof(sba));
    int sb;
    if (0 > (sb = socket(PF_INET, SOCK_DGRAM, 0))) {
      xerror("%s(%d) socket", __FILE__, __LINE__);
      exit(1);
    }
    int one = 1;
    setsockopt(sb, SOL_SOCKET, SO_BROADCAST, (char*)&one, sizeof(one));
    sba.sin_family = AF_INET;
    sba.sin_addr.s_addr = INADDR_ANY;
    sba.sin_port = 0;
    if (0 != bind(sb, (sockaddr*)&sba, sizeof(sba))) {
      xerror("%s(%d) bind", __FILE__, __LINE__);
      exit(1);
    }

    int x = 0;
    do {
      char buf[8];
      memset(&sba, 0, sizeof(sba));
      sba.sin_family = AF_INET;
      sba.sin_addr.s_addr = INADDR_BROADCAST;
      sba.sin_port = htons(CSMASH_PORT);

      sendto(sb, buf, 0, 0, (sockaddr*)&sba, sizeof(sba));
      fd_set fd;
      FD_ZERO(&fd);
      FD_SET(sb, &fd);
      timeval tv;
      memset(&tv, 0, sizeof(tv));
      tv.tv_sec = 1;
      if (0 < select(sb+1, &fd, 0, 0, &tv)) {
	socklen_t l = sizeof(sba);
	int i;
	i = recvfrom(sb, buf, sizeof(buf), 0, (sockaddr*)&sba, &l);
	printf("recvfrom %d %s\n", i, inet_ntoa(sba.sin_addr));      
	break;
      } else {
	printf("%d ", x);
      }
    } while (x++ < 30);
    closesocket(sb);
    if (30 <= x) {
      // notfound
      printf("timeout\n");
      exit(1);
    }
    saddr.sin_addr.s_addr = sba.sin_addr.s_addr;
  } else {
    struct hostent *hent;
    hent = gethostbyname( serverName );
    memcpy( &saddr.sin_addr, hent->h_addr, hent->h_length );
  }

  printf("server is %s\n", inet_ntoa(saddr.sin_addr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(CSMASH_PORT);

  // connect
  if ( (theSocket = socket( PF_INET, SOCK_STREAM, 0 )) < 0 ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    exit(1);
  }
  if ( connect( theSocket, (struct sockaddr *)&saddr, sizeof(saddr) ) ) {
    xerror("%s(%d) connect", __FILE__, __LINE__);
    exit(1);
  }

  // $B%?%$%^D4@0(B
  struct timeb tb;
#ifndef WIN32
  struct timeval tv;
  struct timezone tz;
#endif
  int i;

  for ( i = 0 ; i < 100 ; i++ ) {
    ReadTime( theSocket, &tb );	// $B<N$F$k(B

#ifdef WIN32
    ftime( &tb );
#else
    gettimeofday( &tv, &tz );
    tb.time = tv.tv_sec;
    tb.millitm = tv.tv_usec/1000;
#endif

    SendTime( theSocket, &tb );
  }

  // Ball Data$B$NAw?.(B
  send( theSocket, "BI", 2, 0 );
  theBall.Send( theSocket );

//      printf( "read\n" );
  if ( !(comPlayer = ReadPlayerData()) )
    exit(1);
//      printf( "write\n" );
  SendPlayerData();
}