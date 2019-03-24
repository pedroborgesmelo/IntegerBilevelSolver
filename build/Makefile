SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

CONCERTDIR = /opt/ibm/ILOG/CPLEX_Studio128/concert
CPLEXDIR = /opt/ibm/ILOG/CPLEX_Studio128/cplex
CPOPTIMIZERDIR = /opt/ibm/ILOG/CPLEX_Studio128/cpoptimizer

# my folders
INTEGERBILEVELSOLVERSRC = ../src
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

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) 
CLNDIRS   = -L$(CPLEXLIBDIR) 
CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -lpthread -ldl
CLNFLAGS  = -lcplex -lm -lpthread -ldl

all:
	make build
	mv integerBilevelSolver.so $(INTEGERBILEVELSOLVERLIB)/libIntegerBilevelSolver.so

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CPOPTIMIZERINCDIR = $(CPOPTIMIZERDIR)/include

CFLAGS  = $(COPT)  -I$(CPLEXINCDIR) -I$(CPOPTIMIZERINCDIR) 
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(CPOPTIMIZERINCDIR) -I$(INTEGERBILEVELSOLVERINCLUDE)

CPP_EX = integerBilevelSolver.o integerBilevelSolver.so

build: $(CPP_EX)

clean :
	/bin/rm -rf *.o
	/bin/rm -rf $(CPP_EX)

integerBilevelSolver.o:
	$(CCC) -c $(CCFLAGS) $(INTEGERBILEVELSOLVERSRC)/integerBilevelSolver.cpp -o integerBilevelSolver.o
integerBilevelSolver.so: integerBilevelSolver.o
	$(CCC) -c -shared $(CCFLAGS) $(INTEGERBILEVELSOLVERSRC)/integerBilevelSolver.cpp -o integerBilevelSolver.so
