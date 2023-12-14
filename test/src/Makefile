#!/usr/bin/make
#
# Quake-II					December 05, 2023
#
# source: test/src/Makefile
# author: @misael-diaz
#
# Synopsis:
# Defines the Makefile for building the program with GNU make.
#
# Copyright (c) 2023 Misael Diaz-Maldonado
# This file is released under the GNU General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#

all: Quake

coms:
	@$(MAKE) -C com

utils: coms
	@$(MAKE) -C util

cmds: coms utils
	@$(MAKE) -C cmd

cbufs: coms cmds
	@$(MAKE) -C cbuf

cvars: coms utils cmds
	@$(MAKE) -C cvar

Quake: coms utils cmds cbufs cvars
	@$(MAKE) -C quake

clean:
	@$(MAKE) -C util clean
	@$(MAKE) -C com clean
	@$(MAKE) -C cmd clean
	@$(MAKE) -C cbuf clean
	@$(MAKE) -C cvar clean
	@$(MAKE) -C quake clean
