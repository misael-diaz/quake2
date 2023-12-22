#ifndef GUARD_QUAKE_FS_H
#define GUARD_QUAKE_FS_H

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

// source: fs.h -- Quake File System FS

#ifdef GCC
int FS_SetGameDir(const char *dir)
__attribute__ ((access (read_only, 1), nonnull (1)));

int FS_FreeFile(void **vdata)
__attribute__ ((access (read_write, 1), nonnull (1)));

int FS_LoadFile(const char *filename, void **vdata)
__attribute__ ((access (read_only, 1), access (read_write, 2), nonnull (1, 2)));

int FS_InitFileSystem(void);
#else
int FS_SetGameDir(const char *dir);
int FS_FreeFile(void **vdata);
int FS_LoadFile(const char *filename, void **vdata)
int FS_Init(void);
#endif

#endif
