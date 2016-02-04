include ../localsettings.mk

OBJFILES=sggame.o sgsolver.o sgutilities.o sgapprox.o sgpoint.o sgtuple.o sgaction.o sgenv.o sgsimulator.o sgiteration.o

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
