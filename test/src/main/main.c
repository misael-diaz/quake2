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

// source: main.c -- main program

#include <stdio.h>

#include "q_shared.h"
#include "quake.h"
#include "com.h"
#include "util.h"
#include "cmd.h"
#include "cbuf.h"
#include "cvar.h"
#include "fs.h"

int Quake_Free(void);

#ifdef GCC
int Quake_Init(int const argc, const char **argv)
__attribute__ ((access (read_only, 2), nonnull (2)));
#else
int Quake_Init(int const argc, const char **argv);
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
