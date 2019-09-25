# This file is part of the SGSolve library for stochastic games
# Copyright (C) 2019 Benjamin A. Brooks
# 
# SGSolve free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# SGSolve is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see
# <http://www.gnu.org/licenses/>.
# 
# Benjamin A. Brooks
# ben@benjaminbrooks.net
# Chicago, IL

# Configured for Ben's macbook pro
GRBDIR=/Library/gurobi811/mac64
GRBINCLDIR=$(GRBDIR)/include
GRBLIBDIR=$(GRBDIR)/lib
GRBNAME=gurobi81

STATIC=
DYNAMIC=

AR=/usr/bin/ar
RANLIB=/usr/bin/ranlib

BOOSTINCLDIR=/usr/local/include/
BOOSTDIR=/usr/local/lib/

CXX=clang++ -O0 -w

RMCMD=rm -rf 

DEBUG=-g
# DEBUG=-DNDEBUG -O

CSYSFLAGS = -fPIC -std=gnu++11
CMTFLAGS = 
LDMTFLAGS = -lpthread -lm

CPPDIR=../src/cpp
HPPDIR=../src/hpp
EXAMPLEDIR=./cpp
LIBDIR=../lib

CFLAGS = $(CSYSFLAGS) $(DEBUG) \
	-I$(BOOSTDIR) \
	$(CMTFLAGS) \
	-I$(HPPDIR) \
	-I./hpp/ \
	-I$(BOOSTINCLDIR) \
	$(OPTIONS) 

LDFLAGS = -L$(BOOSTDIR) \
	$(LDMTFLAGS) \
	-L.
