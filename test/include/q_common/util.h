#ifndef GUARD_QUAKE_UTIL_H
#define GUARD_QUAKE_UTIL_H

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

// source: util.h

#ifdef GCC
int   va(char * __restrict__ dst, int const size, const char * __restrict__ fmt, ...)
__attribute__ ((access (write_only, 1), access (read_only, 3), nonnull (1, 3),
		format (printf, 3, 4)));

int   Z_Init(void);
void *Z_Free(void *ptr) __attribute__ ((access (read_only, 1)));
const void *Z_FreeConst(const void *ptr) __attribute__ ((access (read_only, 1)));
int   Z_TagFree(short const tag);
void *Z_TagMalloc(int const sizeObj, short const tag);
void *Z_Malloc(int size);
const char *CopyString(const char *src)
__attribute__ ((access (read_only, 1), nonnull (1)));
#else
int   va(char *dst, int const size, const char *fmt, ...);
int   Z_Init(void);
const void *Z_FreeConst(const void *ptr);
void *Z_Free(void *ptr);
int   Z_TagFree(short const tag);
void *Z_TagMalloc(int const sizeObj, short const tag);
void *Z_Malloc(int size);
const char *CopyString(const char *src);
#endif

#endif
