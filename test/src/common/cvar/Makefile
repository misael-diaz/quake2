#!/usr/bin/make
#
# Quake-II						December 05, 2023
#
# source: test/src/cvar/Makefile
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

include make-inc

all: $(CVAR_O)

$(CVAR_O): $(HEADERS) $(CVAR_C)
	$(CC) $(INC) $(CCOPT) -c $(CVAR_C) -o $(CVAR_O)

clean:
	/bin/rm -f *.o
