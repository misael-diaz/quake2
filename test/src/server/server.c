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

// source: server.c

#include "q_shared.h"
#include "q_types.h"
#include "q_common.h"
#include "server.h"

int SV_Init (void)
{
	sizebuf_t *net_message = Netchan_NetMessage();
	byte *net_message_buffer = Netchan_NetMessageBuffer();
	SZ_Init(net_message, net_message_buffer, MAX_MSGLEN);
	return ERR_ENONE;
}
