PROGRAM = Boggle
OBJECTS = Boggle.o

CPPOPTIONS = -IStanfordCPPLib -fvisibility-inlines-hidden
LDOPTIONS = -L.
LIB = -lStanfordCPPLib

all: $(PROGRAM)

Boggle: $(OBJECTS)
	g++ -o $(PROGRAM) $(LDOPTIONS) $(OBJECTS) $(LIB)

Boggle.o: Boggle.cpp Makefile libStanfordCPPLib.a
	g++ -c $(CPPOPTIONS) Boggle.cpp

libStanfordCPPLib.a:
	@rm -f libStanfordCPPLib.a
	(cd StanfordCPPLib; make all)
	ln -s StanfordCPPLib/libStanfordCPPLib.a .


tidy:
	rm -f ,* .,* *~ core a.out *.err

clean scratch: tidy
	rm -f *.o *.a $(PROGRAM)

