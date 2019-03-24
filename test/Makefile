SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

CONCERTDIR = /opt/ibm/ILOG/CPLEX_Studio128/concert
CPLEXDIR = /opt/ibm/ILOG/CPLEX_Studio128/cplex
CPOPTIMIZERDIR = /opt/ibm/ILOG/CPLEX_Studio128/cpoptimizer

# integer bilevel solver
INTEGERBILEVELSOLVERINCLUDE = ../include
INTEGERBILEVELSOLVERLIB = ../lib

CCC = g++ -O0
CC  = gcc -O0
JAVAC = javac 

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD
COPT  = -m64 -fPIC -fno-strict-aliasing

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) -L$(INTEGERBILEVELSOLVERLIB)
CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -lpthread -ldl -lIntegerBilevelSolver

all:
	make all_cpp

execute: all
	make execute_cpp

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CPOPTIMIZERINCDIR = $(CPOPTIMIZERDIR)/include

CFLAGS  = $(COPT)  -I$(CPLEXINCDIR) -I$(CPOPTIMIZERINCDIR) 
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(CPOPTIMIZERINCDIR) -I$(INTEGERBILEVELSOLVERINCLUDE)

CPP_EX = test01 test01nonuniqueness test02 test02nonuniqueness test03

all_cpp: $(CPP_EX)

execute_cpp: $(CPP_EX)
	 ./test01

clean :
	/bin/rm -rf *.o
	/bin/rm -rf $(CPP_EX)
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp

test01: test01.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o test01 test01.o $(CCLNFLAGS)
test01.o: test01.cpp
	$(CCC) -c $(CCFLAGS) test01.cpp -o test01.o

test01nonuniqueness: test01nonuniqueness.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o test01nonuniqueness test01nonuniqueness.o $(CCLNFLAGS)
test01nonuniqueness.o: test01nonuniqueness.cpp
	$(CCC) -c $(CCFLAGS) test01nonuniqueness.cpp -o test01nonuniqueness.o

test02: test02.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o test02 test02.o $(CCLNFLAGS)
test02.o: test02.cpp
	$(CCC) -c $(CCFLAGS) test02.cpp -o test02.o

test02nonuniqueness: test02nonuniqueness.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o test02nonuniqueness test02nonuniqueness.o $(CCLNFLAGS)
test02nonuniqueness.o: test02nonuniqueness.cpp
	$(CCC) -c $(CCFLAGS) test02nonuniqueness.cpp -o test02nonuniqueness.o

test03: test03.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o test03 test03.o $(CCLNFLAGS)
test03.o: test03.cpp
	$(CCC) -c $(CCFLAGS) test03.cpp -o test03.o
