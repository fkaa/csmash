/***********************************************************************
 *	d:/Werk/src/csmash-0.4.0/xerror.cpp
 *	$Id$
 *
 *	Copyright by ESESoft.
 *	You are granted the right to redistribute this file under
 *	the "Artistic license". See "ARTISTIC" for detail.
 *
 ***********************************************************************/
#include "ttinc.h"

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#ifdef WIN32
#ifndef __CYGWIN__
#include <iostream>
#include "win32/wsaerror.h"
#endif
#endif

void do_xerror(const char *string, va_list va)
{
#ifdef WIN32
#ifndef __CYGWIN__
    DWORD err = WSAGetLastError();
    WSASetLastError(0);
    if (0 != err) {
	// WSAError!
	fputs(wsaerrorstring(err), stderr);
    } else 
#endif
#endif
      fputs(strerror(errno), stderr);

    fputs(": ", stderr);
    vfprintf(stderr, string, va);
    fputc('\n', stderr);
    fflush(stderr);
}

void xerror(const char *string, ...)
{
    va_list va;
    va_start(va, string);
    do_xerror(string, va);
    va_end(va);
}

/***********************************************************************
 *	END OF xerror.cpp
 ***********************************************************************/
