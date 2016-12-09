CFLAGS = -Wall -Wextra
CXXFLAGS = $(CFLAGS)

GCC=gcc $(CFLAGS) -ggdb -pthread
GCCSO=$(GCC) -fPIC -shared
CPP=g++ $(CXXFLAGS) -ggdb -pthread
CPPSO=$(CPP) -fPIC -shared

EXEC=udpping udpping.so

all: $(EXEC)

.PHONY: module
module: all

clean:
	/bin/rm -f $(EXEC)

udpping: udpping.c
	$(CPP) -o $@ $<
udpping.so: udpping.c
	$(CPPSO) -o $@ $<
