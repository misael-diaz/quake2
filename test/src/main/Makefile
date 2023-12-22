#!/usr/bin/make
#
# Quake-II						December 05, 2023
#
# source: test/src/main/Makefile
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

all: $(MAIN)

$(MAIN): $(MAIN_O)
	$(CC) $(CCOPT) $(OBJS) -o $(MAIN)

$(MAIN_O): $(HEADERS) $(MAIN_C)
	$(CC) $(INC) $(CCOPT) -c $(MAIN_C) -o $(MAIN_O)

clean:
	/bin/rm -f *.o $(MAIN)
