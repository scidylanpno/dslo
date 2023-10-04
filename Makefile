DEBUG := -O3 # -DTURNOFFGRAPHOUTPUT # -Wall -Wextra
CPP := g++

GUROBI_HOME := # add your gurobi home path here
GUROBI_INCLUDE_PATH := ${GUROBI_HOME}/include/
GUROBI_LIB_PATH := ${GUROBI_HOME}/lib/

CFLAGS := -g --std=c++11 ${DEBUG} -I. -I${GUROBI_INCLUDE_PATH}
LDFLAGS := -L${GUROBI_LIB_PATH} -lm -lgurobi_c++ -lgurobi91

# SOURCES
HEADERS := macro.h utility.h data_structure.h hypergraph.h lp_algorithms.h algorithms.h ${GUROBI_INCLUDE_PATH}/gurobi_c++.h
SOURCES := utility.cc data_structure.cc hypergraph.cc lp_algorithms.cc algorithms.cc
FINAL_SOURCES := min_and_remove.cc

# OBJECTS
OBJECTS := ${SOURCES:.cc=.o} 
FINAL_OBJECTS := ${FINAL_SOURCES:.cc=.o}
ALL_LOCAL_OBJECTS := ${OBJECTS} ${FINAL_OBJECTS}

# BINARIES
BINARIES := dslo

all: ${ALL_LOCAL_OBJECTS} ${BINARIES} 

${ALL_LOCAL_OBJECTS}: %.o: %.cc ${HEADERS}
	${CPP} -o $@ -c $< ${CFLAGS}

dslo: min_and_remove.o ${OBJECTS}
	${CPP} -o $@ $^ ${LDFLAGS}
	@chmod a+x $@ 

clean:
	rm -f ${ALL_LOCAL_OBJECTS}
	rm -f ${BINARIES}
	rm -f *.log

get_object: 
	@echo ${ALL_LOCAL_OBJECTS}
