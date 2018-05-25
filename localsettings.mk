# This file is part of the SGSolve library for stochastic games
# Copyright (C) 2016 Benjamin A. Brooks
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

ifneq "$(wildcard /Users/benjaminbrooks/Applications/IBM/ILOG/CPLEX_Studio1262/cplex/bin/x86-64_osx/cplex )" "" 
 $(info Compiling on Bens macbook pro)
 GRBDIR=/Library/gurobi800/mac64
 GRBINCLDIR=$(GRBDIR)/include
 GRBLIBDIR=$(GRBDIR)/lib
 GRBNAME=gurobi80

 STATIC=
 DYNAMIC=

 BOOSTINCLDIR=/usr/local/include/
 BOOSTDIR=/usr/local/lib/

 CXX=clang++ -O0 -w

 RMCMD=rm -rf 

endif
ifneq "$(wildcard /opt/ibm/ILOG/CPLEX_Studio1251/cplex/bin/x86-64_sles10_4.1/cplex )" "" 
 $(info Compiling on Bens desktop)
 GRBDIR=/opt/gurobi563/linux64
 GRBINCLDIR=$(GRBDIR)/include
 GRBLIBDIR=$(GRBDIR)/lib
 GRBNAME=gurobi56

 STATIC=-Wl,-Bstatic
 DYNAMIC=-Wl,-Bdynamic

 BOOSTINCLDIR=/usr/local/include/boost/serialization/
 BOOSTDIR=/usr/local/lib/

 CXX=g++

 RMCMD=rm -rf 

endif
ifneq "$(wildcard C:\Users\babrooks\ )" "" 
 $(info Compiling on Bens windows laptop)

 STATIC=-Wl,-Bstatic
 DYNAMIC=-Wl,-Bdynamic

 BOOSTINCLDIR="C:/Program Files/boost/boost_1_60_0/boost/serialization/"
 BOOSTDIR="C:/Program Files/boost/boost_1_60_0/"

 CXX=g++

 RMCMD=del 

endif

# General settings
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
