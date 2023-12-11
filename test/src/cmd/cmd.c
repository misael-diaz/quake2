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

// source: cmd.c -- Quake script command processing module

#include <stddef.h>
#include <string.h>

#include "q_shared.h"
#include "quake.h"
#include "util.h"
#include "com.h"

typedef struct cmd_function_s {
	struct cmd_function_s *next;
	const char *name;
	xcommand_t function;
} cmd_function_t;

static cmd_function_t *cmd_functions = NULL;
qboolean cmd_wait = False;

#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
void Cmd_ExecuteString (const char *line)
{
	Com_Printf("%s\n", line);
}

void Cmd_List_f (void)
{
	int count = 0;
	cmd_function_t *cmd = NULL;
	for (cmd = cmd_functions; cmd; cmd = cmd->next, ++count) {
		Com_Printf("%s\n", cmd->name);
	}

	Com_Printf("%d commands\n", count);
}

void Cmd_Wait_f (void)
{
	cmd_wait = True;
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
int Cmd_AddCommand (const char *name, xcommand_t function)
{
	cmd_function_t *cmd = NULL;
	for (cmd = cmd_functions; cmd; cmd = cmd->next) {
		if (!strcmp(name, cmd->name)) {
			Com_Printf("Cmd_AddCommand: %s already defined\n", name);
			return ERR_ENONE;
		}
	}

	void *ptr = Z_Malloc(sizeof(cmd_function_t));
	if (!ptr) {
		return ERR_FATAL;
	}

	cmd = ((cmd_function_t*) ptr);
	cmd->name = name;
	cmd->function = function;
	cmd->next = cmd_functions;
	cmd_functions = cmd;

	return ERR_ENONE;
}

int Cmd_Init (void)
{
	int rc;
	rc = Cmd_AddCommand("cmdlist", Cmd_List_f);
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = Cmd_AddCommand("wait", Cmd_Wait_f);
	if (rc != ERR_ENONE) {
		return rc;
	}

	return rc;
}
