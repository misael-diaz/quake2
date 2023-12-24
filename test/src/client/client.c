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

// source: client.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "q_shared.h"
#include "q_types.h"
#include "q_common.h"
#include "client.h"

typedef struct {
	char text[MAX_STRING_CHARS];
} console_t;

static qboolean quit = False;
static console_t *console = NULL;

qboolean Con_Quit (void)
{
	const char *text = console->text;
	if (strstr(text, "quit")) {
		Com_Error(ERR_ENONE, "quitting quake ...\n");
		Cvar_Set("quit", "1");
		quit = True;
		return True;
	}

	return False;
}

void Con_Prompt (void)
{
	Com_Error(ERR_ENONE, "quake> ");
}

int Con_Read (void)
{
	memset(console->text, '\0', MAX_STRING_CHARS);
	ssize_t const sz = read(0, console->text, MAX_STRING_CHARS);
	if (sz == -1) {
		Com_Printf("Con_Read: %s\n", strerror(errno));
		return ERR_ENONE;
	}

	if (sz > 0) {
		if (!quit) {
			if (!Con_Quit()) {
				Con_Prompt();
			}
		}
	}

	return ERR_ENONE;
}

int Con_Free (void)
{
	if (!console) {
		return ERR_ENONE;
	}

	void *ptr = console;
	ptr = Z_Free(ptr);
	console = NULL;
	return ERR_ENONE;
}

int Con_Init (void)
{
	void *ptr = Z_Malloc(sizeof(console_t));
	if (!ptr) {
		Com_Error(ERR_FATAL, "Con_Init: malloc error\n");
		return ERR_FATAL;
	}

	int const fd_stdin = 0;
	int const flags = fcntl(fd_stdin, F_GETFD);
	if (flags == -1) {
		Com_Error(ERR_FATAL, "Con_Init: error: %s\n", strerror(errno));
		return ERR_FATAL;
	}

	int status = fcntl(fd_stdin, F_SETFD, flags | O_NONBLOCK);
	if (status == -1) {
		Com_Error(ERR_FATAL, "Con_Init: error: %s\n", strerror(errno));
		return ERR_FATAL;
	}

	console = (console_t *) ptr;
	memset(console->text, '\0', MAX_STRING_CHARS);
	Com_Error(ERR_ENONE, "\nquake> ");
	return ERR_ENONE;
}

int CL_Free (void)
{
	return Con_Free();
}

int CL_Init (void)
{
	int rc;
	rc = Con_Init();
	if (rc != ERR_ENONE) {
		Com_Error(ERR_FATAL, "CL_Init: error\n");
		return rc;
	}

	return ERR_ENONE;
}
