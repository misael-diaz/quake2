#ifndef GUARD_QUAKE_SHARED_H
#define GUARD_QUAKE_SHARED_H

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

// source: q_shared.h -- included by all program modules

#define GNU_STD17 201710L
#define ERR_ENONE 0x00000000
#define ERR_FATAL 0xffffffff

#define MAX_STRING_CHARS 1024
#define MAX_TOKEN_CHARS 128
#define MAX_STRING_TOKENS 80

#define CVAR_USERINFO 2
#define CVAR_SERVERINFO 4
#define CVAR_NOSET 8
#define CVAR_LATCH 16

#endif
