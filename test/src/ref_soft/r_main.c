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

// source: src/ref_soft/r_main.c

#include <string.h>

#include "q_types/type/byte.h"
#include "q_types/type/qboolean.h"
#include "q_types/type/xcommand_t.h"
#include "q_types/type/cvar_t.h"
#include "client/ref.h"
#include "ref_soft/r_local.h"

static refimport_t ri;
static clipplane_t view_clipplanes[4];

// TODO: add missing code from original
qboolean R_Init (void *hinstance, void *wndproc)
{
	// TODO: check if zero initializing the fields is consistent with original source
	memset(view_clipplanes, 0, sizeof(view_clipplanes));
	view_clipplanes[0].leftedge = True;
	view_clipplanes[0].rightedge = False;
	view_clipplanes[1].leftedge = False;
	view_clipplanes[1].rightedge = True;
	view_clipplanes[2].leftedge = False;
	view_clipplanes[2].rightedge = False;
	view_clipplanes[3].leftedge = False;
	view_clipplanes[3].rightedge = False;

	return True;
}

// TODO: add missing code from original
void R_Shutdown (void)
{
	return;
}

// TODO: add missing code from original
refexport_t GetRefAPI (refimport_t const rimp)
{
	ri = rimp;
	refexport_t const re = {
		.api_version = API_VERSION,
		.Init = R_Init,
		.Shutdown = R_Shutdown
	};

	return re;
}
