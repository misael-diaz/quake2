#ifndef GUARD_QUAKE_TYPE_CVAR_H
#define GUARD_QUAKE_TYPE_CVAR_H

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

// source: cvar_t.h -- dynamic command variable type

typedef struct cvar_s {
	struct cvar_s *next;
	const char *name;
	const char *string;
	const char *latched_string;
	qboolean modified;
	float value;
	int flags;
} cvar_t;

#endif
