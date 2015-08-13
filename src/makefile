CXX=g++
CPPDIR=../src/cpp
HPPDIR=../src/hpp
LIBDIR=../lib
EXAMPLEDIR=./cpp
DEBUG=-g
# DEBUG=-DNDEBUG -O

OBJFILES=sggame.o sgsolver.o sgutilities.o sgapprox.o sgpoint.o sgtuple.o sgaction.o sgenv.o sgsimulator.o

# CPLEX
SYSTEM = x86-64_sles10_4.1
LIBFORMAT = static_pic
CSYSFLAGS  = -fPIC -DIL_STD -std=gnu++11
# Compilation and linking flags enabling Multi-threading
CMTFLAGS = -DILOUSEMT -D_REENTRANT
LDMTFLAGS = -lpthread
# Code optimization/debugging options

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

all: libsg.a 

# Next correspondsp to targets for each of the object files. We compile
# them into object code and then package them into a static library.
$(OBJFILES): %.o : $(CPPDIR)/%.cpp $(HPPDIR)/%.hpp $(HPPDIR)/sgcommon.hpp
	$(CXX)  $(CFLAGS) $< -c 

libsg.a: $(OBJFILES) 
	ar r $(LIBDIR)/libsg.a $(OBJFILES)
	ranlib $(LIBDIR)/libsg.a

clean:
	rm -rf *.o ../lib/*.a
