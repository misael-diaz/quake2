#ifndef GUARD_QUAKE_CLIENT_REF_H
#define GUARD_QUAKE_CLIENT_REF_H

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

// source: include/client/ref.h

typedef struct {
	int api_version;
	qboolean (*Init) (void *hinstance, void *wndproc);
	void (*Shutdown) (void);
} refexport_t;

typedef struct {
	int (*Cmd_AddCommand) (const char *name, xcommand_t function);
	int (*Cmd_Argc) (void);
	const char *(*Cmd_Argv) (int const args);
	int (*FS_LoadFile) (const char *filename, void **vdata);
	int (*FS_FreeFile) (void **vdata);
	const cvar_t *(*Cvar_FindVarConst) (const char *var_name);
	int (*Cvar_Get) (const char *var_name, const char *var_value, int const flags);
	int (*Cvar_Set) (const char *var_name, const char *var_value);
} refimport_t;

#endif
