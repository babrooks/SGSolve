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

include ../localsettings.mk

OBJFILES=sggame.o sgsolver.o sgutilities.o sgapprox.o sgpoint.o sgtuple.o sgaction.o sgaction_maxminmax.o sgenv.o sgsimulator.o sgiteration.o sghyperplane.o sgsolver_maxminmax.o sgsolver_maxminmax_3player.o sgpolicy.o

# removed  sgapprox_v2.o sgsolver_v2.o from objfiles

all: libsg.a 

# Next corresponds to targets for each of the object files. We compile
# them into object code and then package them into a static library.
$(OBJFILES): %.o : $(CPPDIR)/%.cpp $(HPPDIR)/%.hpp $(HPPDIR)/sgcommon.hpp
	$(CXX)  $(CFLAGS) $< -c 

libsg.a: $(OBJFILES)
	$(AR) ru $(LIBDIR)/libsg.a $(OBJFILES)
	$(RANLIB) $(LIBDIR)/libsg.a
	touch $(LIBDIR)/libsg.a

clean:
	$(RMCMD) *.o ../lib/*.a
