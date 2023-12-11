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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// source: quake.c -- main program

#include <stdio.h>

#include "q_shared.h"
#include "quake.h"
#include "com.h"
#include "util.h"
#include "cmd.h"
#include "cbuf.h"

int Quake_Free (void)
{
	return Z_TagFree(0);
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 2)))
#endif
int Quake_Init (int const argc, const char **argv)
{
	int rc = 0;
	rc = Z_Init();
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = Com_InitArgv(argc, argv);
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = Cbuf_Init();
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = Cmd_Init();
	if (rc != ERR_ENONE) {
		Quake_Free();
		return rc;
	}

	qboolean clear = False;
	rc = Cbuf_AddEarlyCommands(clear);
	if (rc != ERR_ENONE) {
		Quake_Free();
		return rc;
	}

	rc = Cbuf_Execute();
	if (rc != ERR_ENONE) {
		Quake_Free();
		return rc;
	}

	return rc;
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 2)))
#endif
int main (int const argc, const char **argv)
{
	int rc;
	rc = Quake_Init(argc, argv);
	if (rc != ERR_ENONE) {
		return rc;
	}

//	printf("%s\n", com_argv[0]);

	rc = Quake_Free();
	if (rc != ERR_ENONE) {
		return rc;
	}

	return 0;
}
