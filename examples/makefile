# Makefile for examples

OBJFILES=sggame.o sgsolver.o sgutilities.o sgcomparator.o sgsolution.o
MAINS= as_twostate abreusannikov pd guitester risksharing
MAINSGRB=as_twostate_jyc kocherlakota2_jyc guitester_jyc  abs_jyc

include ../localsettings.mk

all: libsg.a $(MAINS)

# Next correspondsp to targets for each of the object files. We compile
# them into object code and then package them into a static library.
$(OBJFILES): %.o: $(CPPDIR)/%.cpp $(HPPDIR)/%.hpp $(HPPDIR)/sgcommon.hpp
	$(CXX)  $(CFLAGS) $< -c 

$(MAINSGRB): % : $(EXAMPLEDIR)/%.cpp $(HPPDIR)/sgjycsolver.hpp libsg.a
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
