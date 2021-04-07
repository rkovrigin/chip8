GCC=g++
LDFLAGS=-L/usr/local/opt/ncurses/lib
CPPFLAGS=-I/usr/local/opt/ncurses/include

chip8:
	$(GCC) -c chip8.cpp -o chip8.o

all: chip8
	$(GCC) $(LDFLAGS) $(CPPFLAGS) main.cpp chip8.o -o emulator -lmenu -lncurses

clean:
	rm emulator chip8.o
#export "
#export CPPFLAGS="-I/usr/local/opt/ncurses/include"

.DEFAULT_GOAL := all
