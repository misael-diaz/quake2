#ifndef GUARD_QUAKE_TYPE_SIZEBUF_H
#define GUARD_QUAKE_TYPE_SIZEBUF_H

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

// source: sizebuf_t.h -- defines the size buffer type

typedef struct sizebuf_s {
	byte *data;
	qboolean allowoverflow;
	qboolean overflowed;
	int readcount;
	int maxsize;
	int cursize;
} sizebuf_t;

#endif
