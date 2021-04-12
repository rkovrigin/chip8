#include <iostream>
#include <stdio.h>
#include "chip8.h"
#include <ncurses.h>
#include <unistd.h>


using namespace std;


int main() {
    initscr();
    clear();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);

    Chip8 chip8;
    string name = "Chip8 Picture.ch8";
    // name = "test_opcode.ch8";
    // name = "IBM Logo.ch8";
    // name = "Maze (alt) [David Winter, 199x].ch8";
    // name = "Pong (alt).ch8";
    // name = "Random Number Test [Matthew Mikolay, 2010].ch8";
    // name = "Sierpinski [Sergey Naydenov, 2010].ch8";
    // name = "Trip8 Demo (2008) [Revival Studios].ch8";
    // name = "Zero Demo [zeroZshadow, 2007].ch8";
    // name = "Breakout [Carmelo Cortez, 1979].ch8";
    // name = "Chip8 emulator Logo [Garstyciuks].ch8";
    // name = "Particle Demo [zeroZshadow, 2008].ch8";
    // name = "Space Invaders [David Winter].ch8";
    // name = "Brix [Andreas Gustafsson, 1990].ch8";
    // name = "Clock Program [Bill Fisher, 1981].ch8";
    name = "Tetris [Fran Dachille, 1991].ch8";
    // name = "Breakout [Carmelo Cortez, 1979].ch8";
    chip8.load_file_to_ram("roms/" + name);
    chip8.run();

    endwin();
    return 0;
}