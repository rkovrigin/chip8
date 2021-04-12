GCC=g++
LDFLAGS=-L/usr/local/opt/ncurses/lib
CPPFLAGS=-I/usr/local/opt/ncurses/include -Wno-c++11-extensions

chip8:
	$(GCC) $(LDFLAGS) $(CPPFLAGS) -c chip8.cpp -o chip8.o

all: chip8
	$(GCC) $(LDFLAGS) $(CPPFLAGS) main.cpp chip8.o -o emulator -lmenu -lncurses

clean:
	rm emulator chip8.o

.DEFAULT_GOAL := all
