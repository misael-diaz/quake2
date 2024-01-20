#!/usr/bin/make
#
# Quake-II					December 05, 2023
#
# source: test/Makefile
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

QuakeCommon:
	@$(MAKE) -C common

servers: QuakeCommon
	@$(MAKE) -C server

clients: QuakeCommon
	@$(MAKE) -C client

ref_software: QuakeCommon servers clients
	@$(MAKE) -C ref_soft

Quake: QuakeCommon servers clients ref_software
	@$(MAKE) -C main

clean:
	@$(MAKE) -C common clean
	@$(MAKE) -C server clean
	@$(MAKE) -C client clean
	@$(MAKE) -C ref_soft clean
	@$(MAKE) -C main clean
