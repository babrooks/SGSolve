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

# Makefile for examples

# These use the pencil sharpening algorithm
MAINSPS= as_twostate abreusannikov
# These use max-min-max
MAINSMM= pd bos pd_twostate as_twostate_maxminmax	\
	risksharing_3player risksharing_3player_merged
# These programs use gurobi
MAINSGRB=as_twostate_jyc abs_jyc as_twostate_maxminmax_grb	\
	contribution risksharing_maxminmax

QHULLDIR=../../qhull

include ../localsettings.mk

.PHONY: all ps grb mm libsg.a clean

all: libsg.a ps grb mm

ps: $(MAINSPS)

grb: $(MAINSGRB)

mm: $(MAINSMM)

$(MAINSGRB): % : $(EXAMPLEDIR)/%.cpp $(HPPDIR)/sgsolver_jyc.hpp $(HPPDIR)/sgsolver_maxminmax.hpp ../lib/libsg.a
	$(CXX) $(CFLAGS) $< \
	-I$(GRBINCLDIR) -L$(GRBLIBDIR)	\
	-lgurobi_c++ -l$(GRBNAME) -L$(LIBDIR) -lsg $(STATIC)	\
	-lboost_serialization $(DYNAMIC) $(LDFLAGS) -o $@

libsg.a: 
	make -C ../lib

$(MAINSMM) $(MAINSPS): % : $(EXAMPLEDIR)/%.cpp ../lib/libsg.a $(HPPDIR)/*.hpp
	$(CXX) $(CFLAGS) $< -L$(LIBDIR) -lsg \
	$(STATIC) -lboost_serialization \
	$(DYNAMIC) $(LDFLAGS) -o $@

clean:
	rm -rf *.o *.a $(MAINS) $(LIBDIR)/libsg.a $(MAINSGRB) $(MAINS).dSYM
	make clean -C ../lib
