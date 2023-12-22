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

// source: cvar.c -- dynamic command

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "q_shared.h"
#include "quake.h"
#include "q_common.h"

static cvar_t *cvars = NULL;
static qboolean userinfo_modified = False;

#ifdef GCC
static qboolean Cvar_InfoValidate(const char *string)
__attribute__ ((access (read_only, 1), nonnull (1)));

static cvar_t *Cvar_FindVar(const char *var_name)
__attribute__ ((access (read_only, 1), nonnull (1)));

static int Cvar_Set2ForceFreeLatch(cvar_t *var)
__attribute__ ((access (read_write, 1), nonnull (1)));

static int Cvar_Set2Default (cvar_t *var, const char *var_name, const char *var_value)
__attribute__ ((access (read_write, 1),
		access (read_only, 2),
		access (read_only, 3),
		nonnull (1, 2, 3)));

static int Cvar_Set2(const char *var_name, const char *var_value, qboolean const force)
__attribute__ ((access (read_only, 1), access (read_only, 2), nonnull (1, 2)));
#else
static qboolean Cvar_InfoValidate(const char *string);
static cvar_t *Cvar_FindVar(const char *var_name);
static int Cvar_Set2ForceFreeLatch(cvar_t *var);
static int Cvar_Set2Default(cvar_t *var, const char *var_name, const char *var_value);
static int Cvar_Set2(const char *var_name, const char *var_value, qboolean const force);
#endif

static qboolean Cvar_InfoValidate (const char *string)
{
	if (strstr(string, "\\")) {
		return False;
	} else if (strstr(string, "\"")) {
		return False;
	} else if (strstr(string, ";")) {
		return False;
	} else {
		return True;
	}
}

static cvar_t *Cvar_FindVar (const char *var_name)
{
	cvar_t *var = NULL;
	for (var = cvars; var; var = var->next) {
		if (!strcmp(var_name, var->name)) {
			break;
		}
	}

	return var;
}

const cvar_t *Cvar_FindVarConst (const char *var_name)
{
	const cvar_t *var = NULL;
	for (var = cvars; var; var = var->next) {
		if (!strcmp(var_name, var->name)) {
			break;
		}
	}

	return var;
}

const char *Cvar_VariableString (const char *var_name)
{
	cvar_t *var = Cvar_FindVar(var_name);
	if (!var) {
		return "\0";
	} else {
		return var->string;
	}
}

int Cvar_Get (const char *var_name, const char *var_value, int const flags)
{
	if (flags & (CVAR_USERINFO | CVAR_SERVERINFO)) {
		if (!Cvar_InfoValidate(var_value)) {
			char *msg = "Cvar_Get: invalid info cvar name %s cvar value %s\n";
			Com_Printf(msg, var_name, var_value);
			return ERR_ENONE;
		}
	}

	cvar_t *var = Cvar_FindVar(var_name);
	if (var) {
		var->flags |= flags;
		return ERR_ENONE;
	}

	void *ptr = Z_Malloc(sizeof(cvar_t));
	if (!ptr) {
		Com_Error(ERR_FATAL, "Cvar_Get: failed to allocate cvar object\n");
		return ERR_FATAL;
	}

	var = (cvar_t*) ptr;
	var->name = CopyString(var_name);
	if (!var->name) {
		Com_Error(ERR_FATAL, "Cvar_Get: failed to allocate var->name\n");
		return ERR_FATAL;
	}

	var->string = CopyString(var_value);
	if (!var->string) {
		Com_Error(ERR_FATAL, "Cvar_Get: failed to allocaote var->string\n");
		return ERR_FATAL;
	}

	var->latched_string = NULL;

	var->modified = True;

	errno = 0;
//	var->value = atof(var_value);
	var->value = strtof(var_value, NULL);
	if (errno == ERANGE) {
		Com_Printf("Cvar_Get: cvar value %s overflows float type\n", var_value);
	}

	var->flags = flags;

	var->next = cvars;
	cvars = var;

	return ERR_ENONE;
}

int Cvar_FullSet (const char *var_name, const char *var_value, int const flags)
{
	cvar_t *var = Cvar_FindVar(var_name);
	if (!var) {
		int const rc = Cvar_Get(var_name, var_value, flags);
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "Cvar_FullSet: error\n");
			return rc;
		}
	}

	var->modified = True;

	if (var->flags & CVAR_USERINFO) {
		userinfo_modified = True;
	}

	var->string = Z_FreeConst(var->string);
	var->string = CopyString(var_value);
	if (!var->string) {
		Com_Error(ERR_FATAL, "Cvar_FullSet: allocation error\n");
		return ERR_FATAL;
	}

	errno = 0;
//	var->value = atof(var_value);
	var->value = strtof(var_value, NULL);
	if (errno == ERANGE) {
		const char errmsg[] = "Cvar_FullSet: cvar value %s overflows "
			              "float type\n";
		Com_Printf(errmsg, var_value);
	}

	var->flags = flags;

	return ERR_ENONE;
}

static int Cvar_Set2ForceFreeLatch (cvar_t *var)
{
	if (var->latched_string) {
		var->latched_string = Z_FreeConst(var->latched_string);
	}

	return ERR_ENONE;
}

static int Cvar_Set2Default (cvar_t *var, const char *var_name, const char *var_value)
{
	if (var->flags & CVAR_NOSET) {
		char *msg = "Cvar_Set2: write protected variable %s\n";
		Com_Printf(msg, var_name);
		return ERR_ENONE;
	}

	if (var->flags & CVAR_LATCH) {

		if (var->latched_string) {

			if (strcmp(var_value, var->latched_string) == 0) {
				return ERR_ENONE;
			}

			var->latched_string = Z_FreeConst(var->latched_string);

		} else {

			if (strcmp(var_value, var->string) == 0) {
				return ERR_ENONE;
			}

		}

		if (Com_ServerState()) {

			char msg[] = "Cvar_Set2: value %s of var %s will be "
				     "changed on the next game\n";
			Com_Printf(msg, var_value, var_name);

			var->latched_string = CopyString(var_value);
			if (!var->latched_string) {
				char errmsg[] = "Cvar_Set2: allocation error\n";
				Com_Error(ERR_FATAL, errmsg);
				return ERR_FATAL;
			}

		} else {

			var->string = Z_FreeConst(var->string);
			var->string = CopyString(var_value);
			if (!var->string) {
				char errmsg[] = "Cvar_Set2: allocation error\n";
				Com_Error(ERR_FATAL, errmsg);
				return ERR_FATAL;
			}

			errno = 0;
//			var->value = atof(var_value);
			var->value = strtof(var_value, NULL);
			if (errno == ERANGE) {
				char msg[] = "Cvar_Set2: cvar value %s overflows "
					     "float type\n";
				Com_Printf(msg, var_value);
			}

//			TODO: FS_SetGameDir() if var->name is `game` and FS_ExecAutoExec()
		}
	}

	return ERR_ENONE;
}

static int Cvar_Set2 (const char *var_name,
	              const char *var_value,
	              qboolean const force)
{
	cvar_t *var = Cvar_FindVar(var_name);
	if (!var) {
		return Cvar_Get(var_name, var_value, 0);
	}

	if (var->flags & (CVAR_USERINFO | CVAR_SERVERINFO)) {
		if (!Cvar_InfoValidate(var_value)) {
			char msg[] = "Cvar_Set2: invalid info cvar name %s "
			             "cvar value %s\n";
			Com_Printf(msg, var_name, var_value);
			return ERR_ENONE;
		}
	}

	if (force) {

		int rc = Cvar_Set2ForceFreeLatch(var);
		if (rc != ERR_ENONE) {
			return rc;
		}

	} else {

		int rc = Cvar_Set2Default(var, var_name, var_value);
		if (rc != ERR_ENONE) {
			return rc;
		}

	}

	if (!strcmp(var_value, var->string)) {
		return ERR_ENONE;
	}

	var->modified = True;

	if (var->flags & CVAR_USERINFO) {
		userinfo_modified = True;
	}

	var->string = Z_FreeConst(var->string);
	var->string = CopyString(var_value);
	if (!var->string) {
		Com_Error(ERR_FATAL, "Cvar_Set2: failed to allocaote var->string\n");
		return ERR_FATAL;
	}

	errno = 0;
//	var->value = atof(var_value);
	var->value = strtof(var_value, NULL);
	if (errno == ERANGE) {
		Com_Printf("Cvar_Set2: cvar value %s overflows float type\n", var_value);
	}

	return ERR_ENONE;
}

int Cvar_Set (const char *var_name, const char *var_value)
{
	qboolean const force_free_latch = False;
	return Cvar_Set2(var_name, var_value, force_free_latch);
}

static int Cvar_Set_f (void)
{
	int args = Cmd_Argc();
	if (args != 3 && args != 4) {
		Com_Printf("Cvar_Set_f: usage: set <variable> <value> [u|s]\n");
		return ERR_ENONE;
	}

	if (args == 3) {

		int const rc = Cvar_Set(Cmd_Argv(1), Cmd_Argv(2));
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "Set_Var_f: error\n");
			return rc;
		}

	} else {

		if (!strcmp(Cmd_Argv(3), "u") && !strcmp(Cmd_Argv(3), "s")) {
			Com_Printf("Cvar_Set_f: optional flag can only be `u` or `s`\n");
			return ERR_ENONE;
		}

		qboolean const userinfo = (!strcmp(Cmd_Argv(3), "u"));
		int const flags = (userinfo)? CVAR_USERINFO : CVAR_SERVERINFO;
		int const rc = Cvar_FullSet(Cmd_Argv(1), Cmd_Argv(2), flags);
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "Set_Var_f: error\n");
			return rc;
		}

	}

	return ERR_ENONE;
}

static int Cvar_List_f (void)
{
	int count = 0;
	for (cvar_t *var = cvars; var; var = var->next) {
		++count;
	}

	Com_Printf("Cvar_List_f: %d registered cvars\n", count);
	return ERR_ENONE;
}

int Cvar_Init (void)
{
	int rc;
	const char errmsg[] = "Cvar_Init: error\n";
	rc = Cmd_AddCommand("set", Cvar_Set_f);
	if (rc != ERR_ENONE) {
		Com_Error(ERR_FATAL, errmsg);
		return rc;
	}

	rc = Cmd_AddCommand("cvarlist", Cvar_List_f);
	if (rc != ERR_ENONE) {
		Com_Error(ERR_FATAL, errmsg);
		return rc;
	}

	return ERR_ENONE;
}
