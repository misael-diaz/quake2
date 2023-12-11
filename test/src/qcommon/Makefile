#!/usr/bin/make
#
# OpenBDS						December 05, 2023
#
# source: test/qcommon/Makefile
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

all: $(TEST)

$(TEST): $(COMMON_O)
	$(CC) $(CCOPT) $(COMMON_O) -o $(TEST)

$(COMMON_O): $(COMMON_C)
	$(CC) $(CCOPT) -c $(COMMON_C) -o $(COMMON_O)

clean:
	/bin/rm -f *.o *.bin
