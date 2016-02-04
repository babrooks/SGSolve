ifneq "$(wildcard /Users/benjaminbrooks/Applications/IBM/ILOG/CPLEX_Studio1262/cplex/bin/x86-64_osx/cplex )" "" 
 $(info Compiling on Bens macbook pro)
 GRBDIR=/Library/gurobi650/mac64
 GRBINCLDIR=$(GRBDIR)/include
 GRBLIBDIR=$(GRBDIR)/lib
 GRBNAME=gurobi65

 STATIC=
 DYNAMIC=

 BOOSTINCLDIR=/usr/local/include/boost/serialization/
 BOOSTDIR=/usr/local/lib/

 CXX=clang++ -O0

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
endif

# General settings
# DEBUG=-g
DEBUG=-DNDEBUG -O

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
	$(OPTIONS) \
	-m64
LDFLAGS = -L$(BOOSTDIR) \
	$(LDMTFLAGS) \
	-L.
