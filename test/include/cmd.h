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

void Cmd_ExecuteString(const char *line);
void Cmd_List_f(void);
void Cmd_Wait_f(void);
int  Cmd_AddCommand(const char *name, xcommand_t function);
int  Cmd_Init(void);

#endif
