#!/usr/bin/make
#
# Quake-II					December 05, 2023
#
# source: Makefile
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

export CC
export CCOPT

all: tests

tests:
	@$(MAKE) -C test

clean:
	@$(MAKE) -C test clean
