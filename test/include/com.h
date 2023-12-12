#ifndef GUARD_QUAKE_COM_H
#define GUARD_QUAKE_COM_H

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

// source: com.h -- client / server communications

void Com_Printf(const char* fmt, ...);
void Com_Error(int const code, const char* fmt, ...);
int  Com_InitArgv(int const argc, const char **argv);
int  Com_AddParam(const char *parm);
int  Com_Argc(void);
void Com_ClearArgv(int const i);
const char *Com_Argv(int const i);
int  Com_ServerState(void);

#endif
