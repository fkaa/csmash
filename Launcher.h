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

#include <gtk/gtk.h>

// $B=i4|%&%#%s%I%&$N>eIt(B. 
class LauncherHeader {
public:
  LauncherHeader();
  ~LauncherHeader();

  void Init( GtkBox *box );

protected:
  GtkWidget * FullScreenFrame();
  GtkWidget * GraphicsFrame();

  static void Toggle( GtkWidget *widget, gpointer data );
};


// $B2<H>J,$N(B NoteBook $BIt(B
class ModeNote {
public:
  ModeNote();
  ~ModeNote();

  void Init( GtkBox *box );

protected:
  GtkWidget * InitSoloPlayPanel();
  GtkWidget * InitLANPlayPanel();
  GtkWidget * InitInternetPlayPanel();

  static void Toggle( GtkWidget *widget, gpointer data );
  static void StartGame( GtkWidget *widget, gpointer data );
  static void LANStartGame( GtkWidget *widget, gpointer data );
  static void InternetStartGame( GtkWidget *widget, gpointer data );
};

class Launcher {
public:
  Launcher();
  ~Launcher();

  void Init();

  static void Destroy(GtkWidget *widget, gpointer data);
protected:
  GtkWidget *m_window;		// $B%a%$%s%&%#%s%I%&(B
  LauncherHeader *m_header;	// $B>eIt(B
  ModeNote *m_note;		// solo, LAN, Internet $B$N3F(Bmode notebook

  GtkWidget *m_quit;		// quit button
};
