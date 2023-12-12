/*

Derived from Quake-II

Copyright (C) 1997-2001 Id Software, Inc.
Copyright (C) 2023 Misael Díaz-Maldonado

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "q_shared.h"

#define STDC17 GNU_STD17

#define MAX_NUM_ARGVS 40

static int server_state = 0;
static int com_argc = 0;
static const char *com_argv[MAX_NUM_ARGVS];

#if defined(__GCC__)
__attribute__ ((access (read_write, 1), access (read_only, 2)))
#endif
void Com_Base (FILE *stream, const char *fmt, va_list args)
{
	vfprintf(stream, fmt, args);
}

#if (__GCC__ > 12) && (__STDC_VERSION__ > STDC17)
__attribute__ ((access (read_only, 1)))
void Com_Printf (const char* fmt, ...)
{
	va_list args;
	va_start(args);
	Com_Base(stdout, fmt, args);
	va_end(args);
}
#else
void Com_Printf (const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Com_Base(stdout, fmt, args);
	va_end(args);
}
#endif

#if (__GCC__ > 12) && (__STDC_VERSION__ > STDC17)
__attribute__ ((access (read_only, 2)))
void Com_Error (int const code, const char* fmt, ...)
{
	va_list args;
	va_start(args);
	Com_Base(stderr, fmt, args);
	va_end(args);
}
#else
void Com_Error (int const code, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Com_Base(stderr, fmt, args);
	va_end(args);
}
#endif

#if defined(__GCC__)
__attribute__ ((access (read_only, 2)))
#endif
int Com_InitArgv (int const argc, const char **argv)
{
	com_argc = argc;
	void *ptr = com_argv;
	memset(ptr, 0, sizeof(com_argv));
	if (com_argc > MAX_NUM_ARGVS) {
		Com_Error(ERR_FATAL, "argc > MAX_NUM_ARGVS");
		return ERR_FATAL;
	}

	for (int i = 0; i != argc; ++i) {
		if (!argv[i] || (strlen(argv[i])) >= MAX_TOKEN_CHARS) {
			char msg[] = "Com_InitArgv: ignoring the argument number %d "
				     "because it exceeds the MAX_TOKEN_CHARS %d\n";
			Com_Printf(msg, i, MAX_TOKEN_CHARS);
			com_argv[i] = "";
		} else {
			com_argv[i] = argv[i];
		}
	}

	return ERR_ENONE;
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
int Com_AddParam (const char *parm)
{
	if (com_argc == MAX_NUM_ARGVS) {
		Com_Error(ERR_FATAL, "Com_AddParam: MAX_NUM_ARGVS\n");
		return ERR_FATAL;
	}

	com_argv[com_argc] = parm;
	++com_argc;

	return ERR_ENONE;
}

int Com_Argc (void)
{
	return com_argc;
}

const char *Com_Argv (int const i)
{
	if (i < 0 || i >= com_argc || !com_argv[i]) {
		return "";
	} else {
		return com_argv[i];
	}
}

void Com_ClearArgv (int const i)
{
	if (i < 0 || i >= com_argc || !com_argv[i]) {
		return;
	}

	com_argv[i] = "";
}

int Com_ServerState (void)
{
	return server_state;
}
