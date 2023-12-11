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

// source: util.c -- Quake utilities

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "q_shared.h"
#include "quake.h"
#include "com.h"

#define STDC17 GNU_STD17
#define Z_MAGIC 0x1d1d

typedef struct zhead_s {
	struct zhead_s *prev;
	struct zhead_s *next;
	short magic;
	short tag;
	int size;
} zhead_t;

static zhead_t z_chain;
static int z_count = 0;
static int z_bytes = 0;

#if (__GCC__ > 12) && (__STDC_VERSION__ > STDC17)
__attribute__ ((access (write_only, 1), access (read_only, 2)))
char *va (char *dst, const char *fmt, ...)
{
	va_list args;
	va_start(args);
	vsprintf(dst, fmt, args);
	va_end(args);
	return dst;
}
#else
char *va (char *dst, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf(dst, fmt, args);
	va_end(args);
	return dst;
}
#endif

int Z_Init (void)
{
	z_chain.prev = NULL;
	z_chain.next = NULL;
	z_chain.magic = ~Z_MAGIC;
	z_chain.tag = 0xffff;
	z_chain.size = sizeof(zhead_t);
	return ERR_ENONE;
}

void *Z_Free (void *ptr)
{
	if (!ptr) {
		return NULL;
	}

	zhead_t *z = ((zhead_t *) ptr) - 1;

	if (z->magic != Z_MAGIC) {
		Com_Error(ERR_FATAL, "Z_Free: bad magic\n");
		return ptr;
	}

	if (z->prev) {
		z->prev->next = z->next;
	}

	if (z->next) {
		z->next->prev = z->prev;
	}

	--z_count;
	z_bytes -= z->size;

	free(z);
	z = NULL;
	return z;
}

int Z_TagFree (short const tag)
{
	zhead_t *z;
	zhead_t *next;
	qboolean sw = False;
	for (z = z_chain.next; z; z = next) {
		next = z->next;
		if (tag == z->tag) {
			void *ptr = ((void *) (z + 1));
			ptr = Z_Free(ptr);
			if (ptr) {
				Com_Error(ERR_FATAL, "Z_TagFree\n");
				sw = True;
			}
		}
	}

	int rc = (sw)? ERR_FATAL : ERR_ENONE;
	return rc;
}

void *Z_TagMalloc (int const sizeObj, short const tag)
{
	int const size = sizeObj + sizeof(zhead_t);
	zhead_t *z = malloc(size);
	if (!z) {
		Com_Error(ERR_FATAL, "Z_TagMalloc: failed to allocate %d bytes\n", size);
		return NULL;
	}

	memset(z, 0, size);
	z->magic = Z_MAGIC;
	z->tag = tag;
	z->size = size;

	z->next = z_chain.next;
	z->prev = &z_chain;

	if (z_chain.next) {
		z_chain.next->prev = z;
	}

	z_chain.next = z;

	++z_count;
	z_bytes += size;

	return ((void *) (z + 1));
}

void *Z_Malloc (int size)
{
	short const tag = 0;
	return Z_TagMalloc(size, tag);
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
char *CopyString (const char *src)
{
	int const sz = strlen(src) + 1;
	void *ptr = Z_Malloc(sz);
	if (!ptr) {
		return NULL;
	}

	char *dst = ptr;
	strcpy(dst, src);
	return dst;
}
