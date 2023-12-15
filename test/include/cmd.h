#ifndef GUARD_QUAKE_CMD_H
#define GUARD_QUAKE_CMD_H

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

// source: cmd.h -- Quake script command processing module

#ifdef GCC
const char *Cmd_Argv(int const args)
__attribute__ ((returns_nonnull));

void Cmd_ExecuteString(const char *line)
__attribute__ ((access (read_only, 1), nonnull (1)));

int Cmd_AddCommand(const char *name, xcommand_t function)
__attribute__ ((access (read_only, 1), nonnull (1, 2)));

int Cmd_Init(void);
int Cmd_Argc(void);
#else
const char *Cmd_Argv(int const args);
void        Cmd_ExecuteString(const char *line);
int         Cmd_AddCommand(const char *name, xcommand_t function);
int         Cmd_Init(void);
int         Cmd_Argc(void);
#endif

#endif
