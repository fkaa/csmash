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
#include "LobbyClient.h"
#include "MultiPlay.h"

extern short csmash_port;

LobbyClient::LobbyClient() {
  m_timeout = 0;
}

LobbyClient::~LobbyClient() {
  if ( m_timeout > 0 )
    gtk_timeout_remove( m_timeout );
}

void
LobbyClient::Init( char *nick, char *message ) {
  char buf[1024];

  // $B%m%S!<%5!<%P$K@\B3(B
  struct sockaddr_in saddr;
  struct hostent *hent;

  memset(&saddr, 0, sizeof(saddr));

  hent = gethostbyname( LOBBYSERVER_NAME );
  memcpy( &saddr.sin_addr, hent->h_addr, hent->h_length );

  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(LOBBYSERVER_PORT);

  // connect
  if ( (m_socket = socket( PF_INET, SOCK_STREAM, 0 )) < 0 ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    gtk_main_quit();
    exit(1);
  }
  if ( connect( m_socket, (struct sockaddr *)&saddr, sizeof(saddr) ) ) {
    xerror("%s(%d) connect", __FILE__, __LINE__);
    gtk_main_quit();
    exit(1);
  }

  // Connect to Lobby Server
  send( m_socket, "CN", 2, 0 );
  long len = 7 + 32 + 64;
  SendLong( m_socket, len );

  // $B%P!<%8%g%sAw?.(B($B$F$-$H!<(B)
  char ver;
  ver = 0;
  send( m_socket, &ver, 1, 0 );
  ver = 6;
  send( m_socket, &ver, 1, 0 );
  ver = 0;
  send( m_socket, &ver, 1, 0 );

  // $B%]!<%HAw?.(B($BF1$8$/$F$-$H!<(B)
  SendLong( m_socket, csmash_port );

  // nick$BAw?.(B
  strncpy( buf, nick, 32 );
  send( m_socket, buf, 32, 0 );

  // message$BAw?.(B
  strncpy( buf, message, 64 );
  send( m_socket, buf, 64, 0 );

  // $B%G!<%?$b$i$&(B

  ReadHeader( buf );

  if ( strncmp( buf, "UI", 2 ) ) {
    xerror("%s(%d) read UI header", __FILE__, __LINE__);
    gtk_main_quit();
    exit(1);
  }

  ReadUI();

  m_timeout = gtk_timeout_add( 1000, LobbyClient::PollServerMessage, this );

  // $BI=<((B
  m_window = gtk_dialog_new();
  gtk_container_border_width (GTK_CONTAINER (m_window), 10);

  gtk_widget_show(m_window);
  gtk_window_set_modal( (GtkWindow *)m_window, true );
  gtk_widget_set_usize( m_window, 300, 200 );

  GtkWidget *scrolled_window;
  GtkWidget *button;

  scrolled_window = gtk_scrolled_window_new( NULL, NULL );
  gtk_container_border_width (GTK_CONTAINER (scrolled_window), 10);
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scrolled_window),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS );
  gtk_box_pack_start( GTK_BOX(GTK_DIALOG(m_window)->vbox), scrolled_window, 
		      TRUE, TRUE, 0 );
  gtk_widget_show (scrolled_window);

  gchar *titles[3] = { "Nickname", "Message" };
  m_table = gtk_clist_new_with_titles( 2, titles );

  gtk_clist_set_shadow_type( GTK_CLIST(m_table), GTK_SHADOW_OUT );
  gtk_clist_set_column_width( GTK_CLIST(m_table), 0, 150 );

  gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scrolled_window),
					  m_table );
  gtk_widget_show(m_table);

  UpdateTable();

  button = gtk_button_new_with_label ("close");

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (LobbyClient::Quit), this);
  //gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
  //(GtkSignalFunc) gtk_widget_destroy,
  //GTK_OBJECT (m_window));

  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (m_window)->action_area),
		      button, TRUE, TRUE, 0);

  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  
  gtk_widget_show (m_window);                     
}

gint
LobbyClient::PollServerMessage( gpointer data ) {
  LobbyClient *lobby = (LobbyClient *)data;
  fd_set rdfds;
  struct timeval to;

  FD_ZERO( &rdfds );
  FD_SET( lobby->m_socket, &rdfds );

  to.tv_sec = to.tv_usec = 0;

  if ( select( lobby->m_socket+1, &rdfds, NULL, NULL, &to ) <= 0 )
    return 1;
  else {
    char buf[1024];

    lobby->ReadHeader( buf );

    if ( strncmp( buf, "UI", 2 ) ) {
      xerror("%s(%d) read UI header", __FILE__, __LINE__);
      gtk_main_quit();
      exit(1);
    }

    lobby->ReadUI();
    lobby->UpdateTable();

    return 1;
  }
}

void
LobbyClient::UpdateTable() {
  gchar *row[3];

  gtk_clist_clear( GTK_CLIST(m_table) );

  for ( int i = 0 ; i < m_playerNum ; i++ ) {
    row[0] = m_player[i].m_nickname;
    row[1] = m_player[i].m_message;
    gtk_clist_append( GTK_CLIST(m_table), row );
  }
}

void
LobbyClient::Quit( GtkWidget *widget, gpointer data ) {
  LobbyClient *lobby = (LobbyClient *)data;

  // Send Quit Message
  send( lobby->m_socket, "QT", 2, 0 );
  shutdown( lobby->m_socket, 2 );
  
  gtk_widget_destroy( lobby->m_window );
  gtk_timeout_remove( lobby->m_timeout );

  delete lobby;
}

void
LobbyClient::ReadHeader( char *buf ) {
  long len = 0;
  while (1) {
    if ( (len+=recv( m_socket, buf+len, 2-len, 0 )) == 2 )
      break;
  }
}

void
LobbyClient::ReadUI() {
  // $BD9$5<hF@(B
  long protocolLength;
  long len = 0;
  char buf[1024];

  while (1) {
    if ( (len+=recv( m_socket, buf+len, 4-len, 0 )) == 4 )
      break;
  }
  ReadLong( buf, protocolLength );

  // $B$H$j$"$($:A4ItFI$`(B
  char *buffer = new char[protocolLength];
  len = 0;
  while (1) {
    if ( (len+=recv( m_socket, buffer+len, protocolLength-len, 0 )) == protocolLength )
      break;
  }

  // Player $B?t<hF@(B
  ReadLong( buffer, m_playerNum );
  len = 4;

  m_player = new PlayerInfo[m_playerNum];

  // $B<BFI$_9~$_(B
  int i, j;
  for ( i = 0 ; i < m_playerNum ; i++ ) {
    if ( buffer[len] )
      m_player[i].m_canBeServer = true;
    else
      m_player[i].m_canBeServer = false;
    len++;

    ReadLong( buffer+len, m_player[i].m_ID );
    len += 4;

    strncpy( m_player[i].m_nickname, &(buffer[len]), 32 );
    len += 32;

    strncpy( m_player[i].m_message, &(buffer[len]), 64 );
    len += 64;
  }	// protocolLen $B$N(B $B%A%'%C%/$rDI2C$7$?$$(B...
}

PlayerInfo::PlayerInfo() {
}

PlayerInfo::~PlayerInfo() {
}
