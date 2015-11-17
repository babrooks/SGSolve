CXX=g++
CPPDIR=../src/cpp
HPPDIR=../src/hpp
EXAMPLEDIR=./cpp
DEBUG=-g
LIBDIR=../lib
# DEBUG=-DNDEBUG -O

OBJFILES=sggame.o sgsolver.o sgutilities.o sgcomparator.o sgsolution.o
MAINS= as_twostate kocherlakota abreusannikov kocherlakota2 pd guitester

# CPLEX
SYSTEM = x86-64_sles10_4.1
LIBFORMAT = static_pic
CSYSFLAGS  = -fPIC -DIL_STD -std=gnu++11
# Compilation and linking flags enabling Multi-threading
CMTFLAGS = -DILOUSEMT -D_REENTRANT
LDMTFLAGS = -lpthread
# Code optimization/debugging options

# GRB
MAINSGRB=as_twostate_jyc kocherlakota2_jyc guitester_jyc
GRBDIR=/opt/gurobi563/linux64
GRBINCLDIR=$(GRBDIR)/include
GRBLIBDIR=$(GRBDIR)/lib

BOOSTINCLDIR=/usr/local/include/boost/serialization/
BOOSTDIR=/usr/local/lib/

CFLAGS = $(CSYSFLAGS) $(DEBUG) \
	-I$(BOOSTDIR) \
	$(CMTFLAGS) \
	-I$(HPPDIR) \
	-I$(BOOSTINCLDIR) \
	$(OPTIONS)  
LDFLAGS = -L$(BOOSTDIR) \
	$(LDMTFLAGS) \
	-L.

OS := $(shell uname)
ifeq ($(OS),Linux)
	STATIC=-Wl,-Bstatic
	DYNAMIC=-Wl,-Bdynamic
else
	STATIC=
	DYNAMIC=
endif


all: libsg.a $(MAINS)

# Next correspondsp to targets for each of the object files. We compile
# them into object code and then package them into a static library.
$(OBJFILES): %.o: $(CPPDIR)/%.cpp $(HPPDIR)/%.hpp $(HPPDIR)/sgcommon.hpp
	$(CXX)  $(CFLAGS) $< -c 

$(MAINSGRB): % : $(EXAMPLEDIR)/%.cpp $(HPPDIR)/sgjycsolver.hpp libsg.a
	$(CXX) -I$(GRBINCLDIR) $(CFLAGS) $< -L$(GRBLIBDIR)	\
	-lgurobi_c++ -lgurobi56 -L$(LIBDIR) -lsg -Wl,-Bstatic	\
	-lboost_serialization -Wl,-Bdynamic $(LDFLAGS) -o $@

libsg.a: 
	make -C ../src

$(MAINS): % : $(EXAMPLEDIR)/%.cpp libsg.a $(HPPDIR)/sg.hpp $(HPPDIR)/sgsolution.hpp
	$(CXX) $(CFLAGS) $< -L$(LIBDIR) -lsg \
	$(STATIC) -lboost_serialization \
	$(DYNAMIC) $(LDFLAGS) -o $@

clean:
	rm -rf *.o *.a $(MAINS) $(LIBDIR)/libsg.a $(MAINSGRB)
	make clean -C ../src
