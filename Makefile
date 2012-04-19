COMPILER ?= $(GCC_PATH)g++
FLAGS ?= -std=c++0x -O3 -DDEBUG -DWARNING -Wall `pkg-config --cflags opencv` $(GCC_SUPPFLAGS)
LDFLAGS ?= -g
LDLIBS = `pkg-config --libs opencv`
EXECUTABLE = run

SRCS=$(wildcard src/*.cc)
OBJS=$(SRCS:src/%.cc=obj/%.o)

all: release

release: $(OBJS)
	$(COMPILER) $(LDFLAGS) -o $(EXECUTABLE) $(OBJS) $(LDLIBS) 

obj/%.o: src/%.cc
	$(COMPILER) $(FLAGS) -o $@ -c $<

clean:
	rm -f obj/*

dist-clean: clean
	rm -f $(EXECUTABLE) *~ .depend 
	
#automatically handle include dependencies
depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	@$(foreach SRC, $(SRCS), $(COMPILER) $(FLAGS) -MT $(SRC:src/%.cc=obj/%.o) -MM $(SRC) >> .depend;)

include .depend	
