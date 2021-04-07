#include <iostream>
#include <stdio.h>
#include "chip8.h"
#include <ncurses.h>


using namespace std;

int main() {
    initscr();
    clear();

    Chip8 chip8;
    string name = "Chip8 Picture.ch8";
    name = "test_opcode.ch8";
    // name = "IBM Logo.ch8";
    // name = "Maze (alt) [David Winter, 199x].ch8";
    chip8.load_file_to_ram("roms/" + name);
    chip8.run();

    getch();

    endwin();
    return 0;
}