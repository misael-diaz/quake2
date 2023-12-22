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

// source: netchan.c -- Quake Network Channel

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "q_shared.h"
#include "q_types.h"
#include "q_common.h"

static const cvar_t *qport = NULL;

int Netchan_Init (void)
{
	int rc;
	// we are going to channel via the loopback interface so no need of random ports
	const char port[] = "2023";
	rc = Cvar_Get("qport", port, CVAR_NOSET);
	if (rc != ERR_ENONE) {
		Com_Error(ERR_FATAL, "Netchan_Init: malloc error\n");
		return rc;
	}

	qport = Cvar_FindVarConst("qport");
	if (!qport) {
		Com_Error(ERR_FATAL, "Netchan_Init: \n");
		return ERR_FATAL;
	}

	return ERR_ENONE;
}
