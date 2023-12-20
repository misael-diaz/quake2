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

// source: fs.c -- Quake filesystem FS

#include <stdio.h>
#include <string.h>

#include "q_shared.h"
#include "quake.h"
#include "com.h"
#include "util.h"
#include "cmd.h"
#include "cbuf.h"
#include "cvar.h"

typedef struct {
	char name[MAX_QPATH];
	int filepos;
	int filelen;
} packfile_t;

typedef struct pack_s {
	FILE *handle;
	packfile_t *files;
	char filename[MAX_OSPATH];
	int numfiles;
} pack_t;

typedef struct filelink_s {
	struct filelink_s *next;
	const char *from;
	const char *to;
	int fromlength;
} filelink_t;

typedef struct searchpath_s {
	struct searchpath_s *next;
	const char *filename;
	pack_t *pack;
} searchpath_t;

static char fs_cddir[MAX_OSPATH];
static char fs_gamedir[MAX_OSPATH];
static char fs_basedir[MAX_OSPATH];
static filelink_t *links = NULL;
static searchpath_t *searchpaths = NULL;
static searchpath_t *base_searchpaths = NULL;
static const cvar_t *gamedir = NULL;
static const cvar_t *basedir = NULL;
static const cvar_t *cddir = NULL;

#ifdef GCC
static void FS_Unlink(filelink_t *this, filelink_t **prev)
__attribute__ ((access (read_write, 1), access (read_write, 2), nonnull (1, 2)));

static qboolean FS_ValidFileName (const char *f)
__attribute__ ((access (read_only, 1), nonnull (1)));

static int FS_AddGameDirectory (const char *dir)
__attribute__ ((access (read_only, 1), nonnull (1)));
#else
static void FS_Unlink(filelink_t *this, filelink_t **prev);
static qboolean FS_ValidFileName (const char *f);
static int FS_AddGameDirectory (const char *dir);
#endif

static void FS_Unlink (filelink_t *this, filelink_t **prev)
{
	filelink_t *head = links;
	if (*prev == head && this == head) {
		*prev = this->next;
	} else {
		filelink_t *that = *prev;
		that->next = this->next;
	}

	if (this->to) {
		this->to = Z_FreeConst(this->to);
	}

	if (this->from) {
		this->from = Z_FreeConst(this->from);
	}

	this->next = NULL;
	this = Z_Free(this);
}

static int FS_Link_f (void)
{
	if (Cmd_Argc() != 3) {
		Com_Printf("FS_Link_f: usage: link <from> <to>\n");
		return ERR_ENONE;
	}

	const char *to = Cmd_Argv(2);
	const char *from = Cmd_Argv(1);
	filelink_t **prev = ((links)? &links : NULL);
	for (filelink_t *link = links; link; link = link->next) {

		if (!strcmp(from, link->from)) {

			if (!strlen(to)) {
				FS_Unlink(link, prev);
				return ERR_ENONE;
			}

			link->to = Z_FreeConst(link->to);
			link->to = CopyString(to);
			return ERR_ENONE;
		}

		prev = &link;
	}

	// surely the Quake authors did not intend a link to nothing so this msg
	// is to tell the user that the resource is not linked so the request to
	// unlink it has been ignored
	if (!strlen(to)) {
		Com_Printf("FS_Link_f: <from> %s is not linked\n", from);
		return ERR_ENONE;
	}

	void *ptr = Z_Malloc(sizeof(filelink_t));
	if (!ptr) {
		const char errmsg[] = "FS_Link_f: memory allocation error\n";
		Com_Error(ERR_FATAL, errmsg);
		return ERR_FATAL;
	}

	filelink_t *link = ptr;
	link->to = CopyString(to);
	link->from = CopyString(from);
	link->fromlength = strlen(from);
	link->next = links;
	links = link;
	return ERR_ENONE;
}

static int FS_Path_f (void)
{
	const searchpath_t *sps = searchpaths;
	const searchpath_t *base = base_searchpaths;
	Com_Printf("FS_Path_f: Current search path:\n");
	for (const searchpath_t *sp = sps; sp; sp = sp->next) {

		if (sp == base) {
			Com_Printf("FS_Path_f: --------\n");
		} else if (sp->pack) {
			const char *filename = sp->pack->filename;
			int const numfiles = sp->pack->numfiles;
			Com_Printf("FS_Path_f: pack %s files %d\n", filename, numfiles);
		} else {
			Com_Printf("FS_Path_f: file %s\n", sp->filename);
		}
	}

	Com_Printf("\nFS_Path_f: Links:\n");
	for (const filelink_t *link = links; link; link = link->next) {
		Com_Printf("FS_Path_f: from %s : to %s\n", link->from, link->to);
	}

	return ERR_ENONE;
}

static int FS_AddGameDirectory (const char *dir)
{
	strcpy(fs_gamedir, dir);
	searchpath_t *sp = Z_Malloc(sizeof(searchpath_t));
	if (!sp) {
		Com_Error(ERR_FATAL, "FS_AddGameDirectory: path allocation error\n");
		return ERR_FATAL;
	}
//	TODO: consider adding code for loading pack files
	sp->filename = fs_gamedir;
	sp->pack = NULL; // we don't have any pack files so no need to look them up
	sp->next = searchpaths;
	searchpaths = sp;
	return ERR_ENONE;
}

static qboolean FS_ValidFileName (const char *f)
{
	if (strstr(f, "..") || strstr(f, "/") || strstr(f, "\\") || strstr(f, ":")) {
		Com_Printf("FS_ValidFileName: expects a filename not a path\n");
		return False;
	} else {
		return True;
	}
}

static void FS_FreeSearchPath (void)
{
	searchpath_t *next = NULL;
	for (searchpath_t *path = searchpaths; path; path = next) {

		if (path == base_searchpaths) {
			break;
		}

		if (path->pack) {
			fclose(path->pack->handle);
			path->pack->files = Z_Free(path->pack->files);
			path->pack = Z_Free(path->pack);
		}

		next = path->next;
		path = Z_Free(path);
	}
}

#ifdef DEDICATED
static void FS_RestartDedicated (void)	// disabled until we add the dedicated cvar
{
	if (Q_Dedicated() && !Q_DedicatedValue()) {
		Cbuf_AddText("vid_restart\n");	// video
		Cbuf_AddText("snd_restart\n");	// sound
	}
}
#else
static void FS_RestartDedicated (void)
{
}
#endif

static int FS_SetGameDirNone (void)
{
	int rc;
	rc = Cvar_FullSet("gamedir", "", CVAR_NOSET | CVAR_SERVERINFO);
	if (rc != ERR_ENONE) {
		Com_Error(ERR_FATAL, "FS_SetGameDirNone: set gamedir error\n");
		return rc;
	}

	rc = Cvar_FullSet("game", "", CVAR_LATCH | CVAR_SERVERINFO);
	if (rc != ERR_ENONE) {
		Com_Error(ERR_FATAL, "FS_SetGameDirNone: set game error\n");
		return rc;
	}

	return ERR_ENONE;
}

static int FS_FullSetGameDir (const char *dir)
{
	int rc;
	rc = Cvar_FullSet("gamedir", dir, CVAR_NOSET | CVAR_SERVERINFO);
	if (rc != ERR_ENONE) {
		Com_Error(ERR_FATAL, "FS_FullSetGameDir: set game error\n");
		return rc;
	}

	if (cddir && cddir->string[0]) {

		rc = va(fs_cddir, sizeof(fs_cddir), "%s/%s", cddir->string, dir);
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "FS_FullSetGameDir: overflow error\n");
			return rc;
		}

		FS_AddGameDirectory(fs_cddir);
	}

	if (!basedir) {

		const char msg[] = "FS_FullSetGameDir: surprisingly basedir cvar "
				   "is undefined ... trying to proceed anyways\n";
		Com_Printf(msg);

	} else {

		if (basedir->string[0]) {

			const char *string = basedir->string;
			rc = va(fs_gamedir, sizeof(fs_gamedir), "%s/%s", string, dir);
			if (rc != ERR_ENONE) {
				char errmsg[] = "FS_FullSetGameDir: gamedir overflow\n";
				Com_Error(ERR_FATAL, errmsg);
				return rc;
			}

			FS_AddGameDirectory(fs_gamedir);
		}
	}

	return ERR_ENONE;
}

int FS_SetGameDir (const char *dir)
{
	int rc;
	if (!FS_ValidFileName(dir)) {
		Com_Printf("FS_SetGameDir: expects the game filename not a path\n");
		return ERR_ENONE;
	}

	FS_FreeSearchPath();
	FS_RestartDedicated();	// TODO: add dedicated cvar

	if (!strcmp(dir, BASEDIRNAME) || !*dir) {

		rc = FS_SetGameDirNone();
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "FS_SetGameDir: error\n");
			return rc;
		}

	} else {

		rc = FS_FullSetGameDir(dir);
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "FS_SetGameDir: error\n");
			return rc;
		}

	}

	return ERR_ENONE;
}

int FS_InitFileSystem (void)
{
	Cmd_AddCommand("path", FS_Path_f);
	Cmd_AddCommand("link", FS_Link_f);
//	Cmd_AddCommand("dir",  FS_Dir_f);

	int rc;
	rc = Cvar_Get("basedir", ".", CVAR_NOSET);
	if (rc != ERR_ENONE) {
		const char errmsg[] = "FS_InitFileSystem: basedir cvar malloc error\n";
		Com_Error(ERR_FATAL, errmsg);
		return rc;
	}

	basedir = Cvar_FindVarConst("basedir");
	if (!basedir) {
		const char errmsg[] = "FS_InitFileSystem: basedir cvar not found error\n";
		Com_Error(ERR_FATAL, errmsg);
		return rc;
	}

	va(fs_basedir, sizeof(fs_basedir), "%s", basedir->string);

	rc = Cvar_Get("cddir", "", CVAR_NOSET);
	if (rc != ERR_ENONE) {
		const char errmsg[] = "FS_InitFileSystem: CD dir cvar malloc error\n";
		Com_Error(ERR_FATAL, errmsg);
		return rc;
	}

	cddir = Cvar_FindVarConst("cddir");
	if (!basedir) {
		const char errmsg[] = "FS_InitFileSystem: CD dir cvar not found error\n";
		Com_Error(ERR_FATAL, errmsg);
		return rc;
	}

	if (cddir->string[0]) {

		char dir[MAX_OSPATH];
		rc = va(dir, sizeof(dir), "%s/"BASEDIRNAME, cddir->string);
		if (rc != ERR_ENONE) {
			Com_Error(ERR_FATAL, "FS_InitFileSystem: cddir malloc error\n");
			return rc;
		}

		FS_AddGameDirectory(dir);
	}

	char dir[MAX_OSPATH];
	rc = va(dir, sizeof(dir), "%s/"BASEDIRNAME, basedir->string);
	if (rc != ERR_ENONE) {
		Com_Error(ERR_FATAL, "FS_InitFileSystem: basedir malloc error\n");
		return rc;
	}

	FS_AddGameDirectory(dir);

	base_searchpaths = searchpaths;

	rc = Cvar_Get("game", "", CVAR_LATCH | CVAR_SERVERINFO);
	if (rc != ERR_ENONE) {
		const char errmsg[] = "FS_InitFileSystem: game dir malloc error\n";
		Com_Error(ERR_FATAL, errmsg);
		return rc;
	}

	gamedir = Cvar_FindVarConst("game");
	if (!gamedir) {
		const char errmsg[] = "FS_InitFileSystem: game cvar not found error\n";
		Com_Error(ERR_FATAL, errmsg);
		return rc;
	}

	if (gamedir->string[0]) {
		FS_SetGameDir(gamedir->string);
	}

	return ERR_ENONE;
}
