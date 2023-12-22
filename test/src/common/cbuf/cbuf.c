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

// source: cbuf.c -- Quake command buffer

#include <string.h>

#include "q_shared.h"
#include "q_types.h"
#include "q_common.h"

#define MAX_CMD_LEN 0x800
#define MAX_SZBUF_LEN (4 * MAX_CMD_LEN)

#ifdef GCC
static qboolean Cbuf_ValidChar(int const c);
static qboolean Cbuf_ValidText(const char *text)
__attribute__ ((access (read_only, 1), nonnull (1)));
#else
static qboolean Cbuf_ValidChar(int const c);
static qboolean Cbuf_ValidText(const char *text);
#endif

static sizebuf_t cmd_text;
static byte cmd_text_buf[MAX_SZBUF_LEN];
extern qboolean cmd_wait;

void SZ_Init (sizebuf_t *buf, byte *data, int length)
{
	memset(buf, 0, sizeof(sizebuf_t));
	buf->data = data;
	buf->allowoverflow = False;
	buf->overflowed = False;
	buf->readcount = 0;
	buf->maxsize = length;
	buf->cursize = 0;
}

void SZ_Clear (sizebuf_t* buf)
{
	buf->overflowed = False;
	buf->cursize = 0;
}

void *SZ_GetSpace (sizebuf_t *buf, int length)
{
	if ((buf->cursize + length) >= buf->maxsize) {

		if (!buf->allowoverflow) {
			char *msg = "SZ_GetSpace: overflow without allow overflow\n";
			Com_Error(ERR_FATAL, msg);
			return NULL;
		}

		if (length >= buf->maxsize) {
			char *msg = "SZ_GetSpace: length %d exceeds buffer size %d\n";
			Com_Error(ERR_FATAL, msg, length, MAX_SZBUF_LEN);
			return NULL;
		}

		Com_Printf("SZ_GetSpace: overflow\n");
		SZ_Clear(buf);
		buf->overflowed = True;
	}

	void *data = buf->data + buf->cursize;
	return data;
}

int SZ_Write (sizebuf_t *buf, const void *src, int length)
{
	void *dst = SZ_GetSpace(buf, length);
	if (!dst) {
		Com_Error(ERR_FATAL, "SZ_Write: overflow error\n");
		return ERR_FATAL;
	}

	memcpy(dst, src, length);
	buf->cursize += length;
	return ERR_ENONE;
}

int Cbuf_Init (void)
{
	memset(cmd_text_buf, 0, sizeof(cmd_text_buf));
	SZ_Init(&cmd_text, cmd_text_buf, sizeof(cmd_text_buf));
	return ERR_ENONE;
}

static qboolean Cbuf_ValidChar (int const c)
{
	return ((c >= 0 && c < 127)? True : False);
}

static qboolean Cbuf_ValidText (const char *text)
{
	int c;
	const char *t = text;
	while ((c = *t)) {

		if (!Cbuf_ValidChar(c)) {
			return False;
		}

		++t;
	}

	return True;
}

int Cbuf_AddText (const char *text)
{
	if (!Cbuf_ValidText(text)) {
		Com_Error(ERR_FATAL, "Cbuf_AddText: invalid text found\n");
		return ERR_FATAL;
	}

	int const length = strlen(text);
	return SZ_Write(&cmd_text, text, length);
}

int Cbuf_InsertText (const char *text)
{
	if (!Cbuf_ValidText(text)) {
		Com_Error(ERR_FATAL, "Cbuf_AddText: invalid text found\n");
		return ERR_FATAL;
	}

	Com_Printf("Cbuf_InsertText: NOT YET IMPLEMENTED\n");
	return ERR_ENONE;
}

int Cbuf_AddEarlyCommands (qboolean clear)
{
	int rc;
	int i = 0;
	char cmd[MAX_CMD_LEN];
	const char *fmt = "set %s %s\n";
	while (i < Com_Argc()) {

		const char *arg = Com_Argv(i);
		if (strcmp(arg, "+set")) {
			++i;
			continue;
		}

		int const len = strlen(Com_Argv(i + 1)) +
			        strlen(Com_Argv(i + 2)) +
				8;

		if (len >= MAX_CMD_LEN) {
			char msg[] = "Cbuf_AddEarlyCommands: cmd length %d exceeds the "
				     "MAX_CMD_LEN %d\n";
			Com_Error(ERR_FATAL, msg, len, MAX_CMD_LEN);
			return ERR_FATAL;
		}

		rc = va(cmd, sizeof(cmd), fmt, Com_Argv(i + 1), Com_Argv(i + 2));
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "Cbuf_AddEarlyCommands: truncated cmd\n");
			return rc;
		}

		rc = Cbuf_AddText(cmd);
		if (rc != ERR_ENONE) {
			return rc;
		}

		if (clear) {
			Com_ClearArgv(i);
			Com_ClearArgv(i + 1);
			Com_ClearArgv(i + 2);
		}
		i += 3;
	}

	return ERR_ENONE;
}

int Cbuf_Execute (void)
{
	int i;
	char line[MAX_CMD_LEN];
	while (cmd_text.cursize) {

		qboolean matched = False;
		char *text = (char *) cmd_text.data;
		for (i = 0; i != cmd_text.cursize; ++i) {
			if (text[i] == '\n') {
				matched = True;
				break;
			}
		}

		int const len = i;
		if (len == 0) {
			Com_Error(ERR_FATAL, "Cbuf_Execute: unexpected error\n");
			return ERR_FATAL;
		}

		if (!matched) {
			Com_Error(ERR_FATAL, "Cbuf_Execute: maybe trash in cmd buffer\n");
			return ERR_FATAL;
		}

		if (len >= MAX_CMD_LEN)
		{
			char errmsg[] = "Cbuf_Execute: the command length %d exceeds "
				        "the MAX_CMD_LEN %d\n";
			Com_Error(ERR_FATAL, errmsg, len, MAX_CMD_LEN);
			return ERR_FATAL;
		}

		memcpy(line, text, len);
		line[len] = '\0';

		if (len == cmd_text.cursize) {
			cmd_text.cursize = 0;
		} else {
			++i;
			void *dst = text;
			const void *src = text + i;
			cmd_text.cursize -= i;
			memmove(dst, src, cmd_text.cursize);
		}

		Cmd_ExecuteString(line);

		if (cmd_wait) {
			cmd_wait = False;
			break;
		}
	}

	return ERR_ENONE;
}
