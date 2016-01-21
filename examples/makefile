
CSYSFLAGS = -fPIC -std=gnu++11
CMTFLAGS = 
LDMTFLAGS = -lpthread -lm

CPPDIR=../src/cpp
HPPDIR=../src/hpp
EXAMPLEDIR=./cpp
LIBDIR=../lib
DEBUG=-g
DEBUG=-DNDEBUG -O

OBJFILES=sggame.o sgsolver.o sgutilities.o sgcomparator.o sgsolution.o
MAINS= as_twostate kocherlakota abreusannikov kocherlakota2 pd guitester risksharing

include ../grb.mk

BOOSTINCLDIR=/usr/local/include/boost/serialization/
BOOSTDIR=/usr/local/lib/

CFLAGS = $(CSYSFLAGS) $(DEBUG) \
	-I$(BOOSTDIR) \
	$(CMTFLAGS) \
	-I$(HPPDIR) \
	-I./hpp/ \
	-I$(BOOSTINCLDIR) \
	$(OPTIONS) \
	-m64
LDFLAGS = -L$(BOOSTDIR) \
	$(LDMTFLAGS) \
	-L.

# GRB
MAINSGRB=as_twostate_jyc kocherlakota2_jyc guitester_jyc  abs_jyc

all: libsg.a $(MAINS)

# Next correspondsp to targets for each of the object files. We compile
# them into object code and then package them into a static library.
$(OBJFILES): %.o: $(CPPDIR)/%.cpp $(HPPDIR)/%.hpp $(HPPDIR)/sgcommon.hpp
	$(CXX)  $(CFLAGS) $< -c 

$(MAINSGRB): % : $(EXAMPLEDIR)/%.cpp $(HPPDIR)/sgjycsolver.hpp
	$(CXX) -I$(GRBINCLDIR) $(CFLAGS) $< -L$(GRBLIBDIR)	\
	-lgurobi_c++ -l$(GRBNAME) -L$(LIBDIR) -lsg $(STATIC)	\
	-lboost_serialization $(DYNAMIC) $(LDFLAGS) -o $@

libsg.a: 
	make -C ../src

$(MAINS): % : $(EXAMPLEDIR)/%.cpp libsg.a $(HPPDIR)/sg.hpp $(HPPDIR)/sgsolution.hpp
	$(CXX) $(CFLAGS) $< -L$(LIBDIR) -lsg \
	$(STATIC) -lboost_serialization \
	$(DYNAMIC) $(LDFLAGS) -o $@

clean:
	rm -rf *.o *.a $(MAINS) $(LIBDIR)/libsg.a $(MAINSGRB)
	make clean -C ../src
