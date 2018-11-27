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

# Makefile for examples

MAINS= as_twostate abreusannikov pd pd_twostate guitester risksharing finiteresource \
	as_twostate_maxminmax predation contribution
MAINSGRB=as_twostate_jyc kocherlakota2_jyc guitester_jyc  abs_jyc threeplayer as_twostate_maxminmax_grb risksharing_maxminmax
GRBTEST=gurobibasistest
QHULLMAINS=qhulltest
QHULLGRBMAINS=threeplayer2

QHULLDIR=../../qhull

include ../localsettings.mk

all: libsg.a $(MAINS)

$(MAINSGRB): % : $(EXAMPLEDIR)/%.cpp $(HPPDIR)/sgjycsolver.hpp $(HPPDIR)/sgsolver_v3.hpp libsg.a
	$(CXX) $(CFLAGS) $< \
	-I$(GRBINCLDIR) -L$(GRBLIBDIR)	\
	-lgurobi_c++ -l$(GRBNAME) -L$(LIBDIR) -lsg $(STATIC)	\
	-lboost_serialization $(DYNAMIC) $(LDFLAGS) -o $@

$(GRBTEST): % : $(EXAMPLEDIR)/%.cpp
	$(CXX) $(CFLAGS) $< \
	-I$(GRBINCLDIR) -L$(GRBLIBDIR)	\
	-lgurobi_c++ -l$(GRBNAME) -L$(LIBDIR) $(STATIC)	\
	$(DYNAMIC) $(LDFLAGS) -o $@

$(QHULLMAINS): % : $(EXAMPLEDIR)/%.cpp
	$(CXX) $(CFLAGS) -I$(QHULLDIR)/src $< -L$(QHULLDIR)/lib/ -lqhullstatic_r -lqhullcpp \
	 $(LDFLAGS) -o $@

$(QHULLGRBMAINS): % : $(EXAMPLEDIR)/%.cpp
	$(CXX) $(CFLAGS) -I$(QHULLDIR)/src $< -L$(QHULLDIR)/lib/ -lqhullcpp -lqhullstatic_r \
	-I$(GRBINCLDIR) -L$(GRBLIBDIR)	\
	-lgurobi_c++ -l$(GRBNAME)  \
	 $(LDFLAGS) -o $@

libsg.a: 
	make -C ../lib

$(MAINS): % : $(EXAMPLEDIR)/%.cpp libsg.a $(HPPDIR)/sg.hpp $(HPPDIR)/sgsolution.hpp
	$(CXX) $(CFLAGS) $< -L$(LIBDIR) -lsg \
	$(STATIC) -lboost_serialization \
	$(DYNAMIC) $(LDFLAGS) -o $@

clean:
	rm -rf *.o *.a $(MAINS) $(LIBDIR)/libsg.a $(MAINSGRB) $(MAINS).dSYM
	make clean -C ../lib
