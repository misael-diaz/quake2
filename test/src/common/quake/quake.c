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

// source: quake.c -- Quake initializer

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "q_shared.h"
#include "q_types.h"
#include "q_common.h"
#include "server.h"
#include "client.h"

static qboolean quit = False;

int Quake_Free (void)
{
	CL_Free();
	return Z_TagFree(0);
}

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

	rc = Cvar_Init();
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

	rc = FS_InitFileSystem();
	if (rc != ERR_ENONE) {
		Quake_Free();
		return rc;
	}

	rc = Netchan_Init();
	if (rc != ERR_ENONE) {
		Quake_Free();
		return rc;
	}

	rc = SV_Init();
	if (rc != ERR_ENONE) {
		Quake_Free();
		return rc;
	}

	rc = CL_Init();
	if (rc != ERR_ENONE) {
		Quake_Free();
		return rc;
	}

	rc = Cvar_Get("quit", "0", 0);
	if (rc != ERR_ENONE) {
		Quake_Free();
		return rc;
	}

	return rc;
}

int Quake_Quit (void)
{
	const cvar_t *q = Cvar_FindVarConst("quit");
	if (!q) {
		Com_Error(ERR_FATAL, "Quake_Quit: error\n");
		return ERR_FATAL;
	}

	if (!strcmp(q->string, "1"))
	{
		quit = True;
	}

	return ERR_ENONE;
}

int Quake_Loop (void)
{
	int rc;
	time_t const walltime = 60;
	time_t const t_start = time(NULL);
	while (True) {
		time_t const t_end = time(NULL);

		rc = Con_Read();
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "Quake_Loop: error\n");
			return ERR_FATAL;
		}

		rc = Quake_Quit();
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "Quake_Loop: error\n");
			return ERR_FATAL;
		}

		if (quit) {
			break;
		}

		time_t const etime = (t_end - t_start);
		if (etime > walltime) {
			break;
		}
	}

	return ERR_ENONE;
}
