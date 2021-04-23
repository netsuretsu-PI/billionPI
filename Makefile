CXX = g++-8

SRCDIR = ./src
MULTIPRECDIR = ${SRCDIR}/multiprecision
LONGVECTORDIR = ${SRCDIR}/longVector
OBJDIR = ./obj

CFLAGS = -O0 -g -std=c++17 -D _GLIBCXX_DEBUG -D _GLIBCXX_DEBUG_PEDANTIC -D_DEBUG

ifeq ($(BUILD),release)
	CFLAGS = -O3 -std=c++17
endif

$(OBJDIR)/fileBasedVector.o: ${LONGVECTORDIR}/longVector.hpp ${LONGVECTORDIR}/fileBasedVector/fileBasedVector.hpp ${LONGVECTORDIR}/fileBasedVector/fileBasedVector.cpp 
	$(CXX) $(LONGVECTORDIR)/fileBasedVector/fileBasedVector.cpp -c $(CFLAGS) -o $(OBJDIR)/fileBasedVector.o

$(OBJDIR)/multiprec.o: ${MULTIPRECDIR}/multiprec.hpp ${MULTIPRECDIR}/multiprec.cpp 
	$(CXX) $(MULTIPRECDIR)/multiprec.cpp -c $(CFLAGS) -o $(OBJDIR)/multiprec.o

$(OBJDIR)/bigint.o: ${MULTIPRECDIR}/multiprec.hpp ${MULTIPRECDIR}/bigint.cpp ${MULTIPRECDIR}/bigint.hpp ${MULTIPRECDIR}/multiprec.hpp
	$(CXX) $(MULTIPRECDIR)/bigint.cpp -c $(CFLAGS) -o $(OBJDIR)/bigint.o
$(OBJDIR)/bigfloat.o: $(MULTIPRECDIR)/multiprec.hpp $(MULTIPRECDIR)/bigfloat.cpp $(MULTIPRECDIR)/bigfloat.hpp $(MULTIPRECDIR)/multiprec.hpp
	$(CXX) $(MULTIPRECDIR)/bigfloat.cpp -c $(CFLAGS) -o $(OBJDIR)/bigfloat.o

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	$(CXX) $(SRCDIR)/main.cpp -c $(CFLAGS) -o $(OBJDIR)/main.o
$(OBJDIR)/test.o: $(SRCDIR)/test.cpp
	$(CXX) $(SRCDIR)/test.cpp -c $(CFLAGS) -o $(OBJDIR)/test.o

pi: $(OBJDIR)/main.o $(OBJDIR)/bigint.o $(OBJDIR)/bigfloat.o $(OBJDIR)/multiprec.o
	$(CXX) $(OBJDIR)/main.o $(OBJDIR)/bigint.o $(OBJDIR)/bigfloat.o $(OBJDIR)/multiprec.o -o pi $(CFLAGS) -pthread

test: $(OBJDIR)/test.o $(OBJDIR)/bigint.o $(OBJDIR)/bigfloat.o $(OBJDIR)/multiprec.o $(OBJDIR)/fileBasedVector.o
	$(CXX) $(OBJDIR)/test.o $(OBJDIR)/bigint.o $(OBJDIR)/bigfloat.o $(OBJDIR)/multiprec.o $(OBJDIR)/fileBasedVector.o -o test $(CFLAGS) -pthread

clean:
	rm -f obj/* pi test out.txt