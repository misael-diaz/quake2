#ifndef GUARD_QUAKE_CBUF_H
#define GUARD_QUAKE_CBUF_H

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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// source: cbuf.h -- Quake command buffer

#ifdef GCC
void SZ_Init(sizebuf_t *buf, byte *data, int length)
__attribute__ ((access (read_write, 1), access (read_only, 2), nonnull (1, 2)));

void SZ_Clear(sizebuf_t* buf)
__attribute__ ((access (read_write, 1), nonnull (1)));

void *SZ_GetSpace(sizebuf_t *buf, int length)
__attribute__ ((access (read_write, 1), nonnull (1)));

int SZ_Write(sizebuf_t *buf, const void *src, int length)
__attribute__ ((access (read_write, 1), access (read_only, 2), nonnull (1, 2)));

int Cbuf_AddText(const char *text)
__attribute__ ((access (read_only, 1), nonnull (1)));

int Cbuf_Init(void);
int Cbuf_AddEarlyCommands(qboolean clear);
int Cbuf_Execute(void);
#else
void SZ_Init(sizebuf_t *buf, byte *data, int length);
void SZ_Clear(sizebuf_t* buf);
void *SZ_GetSpace(sizebuf_t *buf, int length);
int  SZ_Write(sizebuf_t *buf, const void *src, int length);

int  Cbuf_AddText(const char *text);
int  Cbuf_Init(void);
int  Cbuf_AddEarlyCommands(qboolean clear);
int  Cbuf_Execute(void);
#endif

#endif
