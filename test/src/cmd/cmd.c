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

// source: cmd.c -- Quake script command processing module

#include <stddef.h>
#include <string.h>

#include "q_shared.h"
#include "quake.h"
#include "util.h"
#include "com.h"
#include "cvar.h"

typedef struct cmd_function_s {
	struct cmd_function_s *next;
	const char *name;
	xcommand_t function;
} cmd_function_t;

typedef enum {
	NULL_CHAR,
	NEW_LINE,
	NON_WHITESPACE
} match_t;

static cmd_function_t *cmd_functions = NULL;
static int cmd_argc = 0;
static char *cmd_argv[MAX_STRING_TOKENS];
static char cmd_args[MAX_STRING_CHARS];
static char cmd_token[MAX_TOKEN_CHARS];
qboolean cmd_wait = False;

int Cmd_Argc (void)
{
	return cmd_argc;
}

char *Cmd_Argv (int const args)
{
	int const argc = (args < 0)? -args : args;
	if (argc >= cmd_argc) {
		return NULL;
	} else {
		return cmd_argv[argc];
	}
}

match_t Cmd_SkipWhitespace (const char **string_p)
{
	const char *text = *string_p;
	while (*text && *text <= ' ' && *text != '\n')  {
		++text;
	}

	match_t rc;
	if (*text == '\0') {
		rc = NULL_CHAR;
	} else if (*text == '\n') {
		rc = NEW_LINE;
	} else {
		rc = NON_WHITESPACE;
	}

	*string_p = text;
	return rc;
}

#if defined(DEBUG) && DEBUG
void Cmd_CopyCMDArgs (const char *text)
{
	cmd_args[0] = '\0';
	strcpy(cmd_args, text);
	for (int i = (strlen(cmd_args) - 1); i >= 0; --i) {
		if (cmd_args[i] <= ' ') {
			cmd_args[i] = '\0';
		} else {
			break;
		}
	}
	Com_Printf("Cmd_CopyCMDArgs: %s\n", cmd_args);
}
#else
void Cmd_CopyCMDArgs (const char *text)
{
	cmd_args[0] = '\0';
	strcpy(cmd_args, text);
	for (int i = (strlen(cmd_args) - 1); i >= 0; --i) {
		if (cmd_args[i] <= ' ') {
			cmd_args[i] = '\0';
		} else {
			break;
		}
	}
}
#endif

char *Cmd_Parse (const char **string_p)
{
	int chr;
	int nxt;
	int len = 0;
	const char *text = *string_p;
	char *token = cmd_token;
	token[0] = '\0';

	if (!text) {
		string_p = NULL;
		return "";
	}

skipWhiteSpace:

	while ((chr = *text) <= ' ') {

		if (!chr) {
			string_p = NULL;
			return "";
		}

		++text;
	}

	// skips entire lines of comments
	if (chr == '/' && (nxt = text[1]) == '/') {

		while (*text && *text != '\n') {
			++text;
		}

		goto skipWhiteSpace;
	}

	// TODO: add code to handle quoted strings

	do {
		if (len < MAX_TOKEN_CHARS) {
			token[len] = chr;
			++len;
		}

		++text;
		chr = *text;
	} while (chr > 32 && chr < 128);

	if (len == MAX_TOKEN_CHARS) {
		len = 0;
		Com_Printf("Cmd_Parse: ignored long token\n");
		string_p = NULL;
		return "";
	}

	token[len] = '\0';
	*string_p = text;
	return token;
}

void Cmd_TokenizeString (const char **string_p)
{
	for (int i = 0; i != cmd_argc; ++i) {
		cmd_argv[i] = Z_Free(cmd_argv[i]);
	}

	// TODO: add MACRO expanding code

	cmd_argc = 0;
	int num_tokens = 0;
	qboolean parse = True;
	const char *text = *string_p;
	char *token = NULL;
	while (parse) {

		match_t const m = Cmd_SkipWhitespace(string_p);
		text = *string_p;

		switch (m) {
		case (NULL_CHAR):
			parse = False;
			break;
		case (NEW_LINE):
			++text;
			*string_p = text;
			parse = False;
			break;
		default:
			parse = True;
		}

		if (!parse) {
			break;
		}

		if (num_tokens == 1) {
			Cmd_CopyCMDArgs(text);
		}

		*string_p = text;
		token = Cmd_Parse(string_p);
		if (!string_p) {
			break;
		}

		text = *string_p;
		if (num_tokens < MAX_STRING_TOKENS) {
			int const sz = strlen(token) + 1;
			cmd_argv[cmd_argc] = Z_Malloc(sz);
			strcpy(cmd_argv[cmd_argc], token);
			++num_tokens;
			++cmd_argc;
		}
	}
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
void Cmd_ExecuteString (const char *line)
{
#if defined(DEBUG) && DEBUG
	Com_Printf("%s\n", line);
#endif
	const char **cli = &line;
	Cmd_TokenizeString(cli);	// TODO: enable MACRO expanding

	if (!Cmd_Argc()) {
		return;
	}

	for (cmd_function_t *cmd = cmd_functions; cmd; cmd = cmd->next) {
		if (!strcmp(cmd_argv[0], cmd->name)) {	// TODO: ignore case of cmd name
			if (cmd->function) {
				cmd->function();
				return;
			} else {
				// TODO: forward command to server
				const char msg[] = "Cmd_ExecuteString: unregistered "
					           "function (in client): %s\n";
				Com_Printf(msg, cmd_argv[0]);
			}
		}
	}
// TODO:
// [ ] add cmd aliasing code
// [ ] check cmd in cvars via Cvar_Command()
// [ ] forward command to server if cmd is not registered via Cmd_ForwardToServer()
}

static int Cmd_List_f (void)
{
	int count = 0;
	cmd_function_t *cmd = NULL;
	for (cmd = cmd_functions; cmd; cmd = cmd->next, ++count) {
		Com_Printf("%s\n", cmd->name);
	}

	Com_Printf("%d commands\n", count);
	return ERR_ENONE;
}

static int Cmd_Wait_f (void)
{
	cmd_wait = True;
	return ERR_ENONE;
}

#if defined(DEBUG) && DEBUG
static int Cmd_Echo_f (void)
{
	if (Cmd_Argc()) {
		Com_Printf("Cmd_Echo_f: %s ", Cmd_Argv(0));
	}

	for (int i = 1; i != Cmd_Argc(); ++i) {
		Com_Printf("%s ", Cmd_Argv(i));
	}

	return ERR_ENONE;
}
#else
static int Cmd_Echo_f (void)
{
	for (int i = 1; i != Cmd_Argc(); ++i) {
		Com_Printf("%s ", Cmd_Argv(i));
	}

	return ERR_ENONE;
}
#endif

#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
int Cmd_AddCommand (const char *name, xcommand_t function)
{
	if (Cvar_VariableString(name)) {
		Com_Printf("Cmd_AddCommand: %s already defined as a var\n", name);
		return ERR_ENONE;
	}

	cmd_function_t *cmd = NULL;
	for (cmd = cmd_functions; cmd; cmd = cmd->next) {
		if (!strcmp(name, cmd->name)) {
			Com_Printf("Cmd_AddCommand: %s already defined\n", name);
			return ERR_ENONE;
		}
	}

	void *ptr = Z_Malloc(sizeof(cmd_function_t));
	if (!ptr) {
		return ERR_FATAL;
	}

	cmd = ((cmd_function_t*) ptr);
	cmd->name = name;
	cmd->function = function;
	cmd->next = cmd_functions;
	cmd_functions = cmd;

	return ERR_ENONE;
}

int Cmd_Init (void)
{
	int rc;
	rc = Cmd_AddCommand("cmdlist", Cmd_List_f);
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = Cmd_AddCommand("echo", Cmd_Echo_f);
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = Cmd_AddCommand("wait", Cmd_Wait_f);
	if (rc != ERR_ENONE) {
		return rc;
	}

	return rc;
}
