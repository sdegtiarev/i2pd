UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	include Makefile.osx
else ifeq ($(UNAME), FreeBSD)
	include Makefile.bsd
else
	include Makefile.linux
endif
CXXFLAGS= $(CFLAGS)

all: obj i2p

i2p: $(OBJECTS:obj/%=obj/%)
	$(CXX) -o $@ $^ $(LDLIBS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

obj/%.o : %.cpp
	$(CXX) -o $@ $< -c $(CXXFLAGS) $(NEEDED_CXXFLAGS) $(INCFLAGS) $(CPU_FLAGS)

obj:
	mkdir -p obj

clean:
	rm -fr obj i2p
	
io: io.cc
	g++ -g -std=c++11 io.cc -o io -lboost_system

ios: ios.cc
	g++ -g -std=c++11 $< -o$@ -lboost_system


.PHONY: all
.PHONY: clean

ver:
	@echo $(CXXVER) flags: $(CFLAGS)
