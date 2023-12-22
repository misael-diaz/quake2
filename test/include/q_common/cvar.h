#ifndef GUARD_QUAKE_CVAR_H
#define GUARD_QUAKE_CVAR_H

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

// source: cvar.h -- dynamic command

#ifdef GCC
const cvar_t *Cvar_FindVarConst(const char *var_name)
__attribute__ ((access (read_only, 1), nonnull (1)));

const char *Cvar_VariableString(const char *var_name)
__attribute__ ((access (read_only, 1), nonnull (1), returns_nonnull));

int Cvar_Get(const char *var_name, const char *var_value, int const flags)
__attribute__ ((access (read_only, 1), access (read_only, 2), nonnull (1, 2)));

int Cvar_FullSet(const char *var_name, const char *var_value, int const flags)
__attribute__ ((access (read_only, 1), access (read_only, 2), nonnull (1, 2)));

int Cvar_Set(const char *var_name, const char *var_value)
__attribute__ ((access (read_only, 1), nonnull (1, 2)));

int Cvar_Init(void);
#else
const char *Cvar_VariableString(const char *var_name);
int Cvar_Get(const char *var_name, const char *var_value, int const flags);
int Cvar_FullSet(const char *var_name, const char *var_value, int const flags);
int Cvar_Set(const char *var_name, const char *var_value);
int Cvar_Init(void);
#endif

#endif
