/*
 * Chip8 emulator
 * Developed by Roman Kovrigin in 2021
 * Spec: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
*/

#include <string.h>
#include <iostream>
#include "chip8.h"
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>
#include <chrono>


using namespace std;

bool first_draw = true;

Chip8::Chip8() {
    memset(&ram, 0, sizeof(ram));
    memset(&V, 0, sizeof(V));
    memset(&stack, 0, sizeof(stack));
    load_hex_digit_sprites();
    init_screen_buffs();
    programs_start_location = 0x200;
    programs_start_location_eti660 = 0x600;
    default_frequency = 500;
    I = 0; // 16 bits register
    SP = 0; // Stack pointer
    PC = programs_start_location; // Program counter
    DT = 0; // Display timer
    ST = 0; // Sound timer
    width = 64;
    hight = 32;

    mvprintw(0, 65, "sizeof mem %d\n", sizeof(ram));
    mvprintw(1, 65, "sizeof registers %d\n", sizeof(V));
    mvprintw(2, 65, "sizeof stack %d\n", sizeof(stack));
}

void Chip8::load_hex_digit_sprites() {
    ram[0] = 0xF0; ram[1] = 0x90; ram[2] = 0x90; ram[3] = 0x90; ram[4] = 0xF0;      // 0
    ram[5] = 0x20; ram[6] = 0x60; ram[7] = 0x20; ram[8] = 0x20; ram[9] = 0x70;      // 1
    ram[10] = 0xF0; ram[11] = 0x10; ram[12] = 0xF0; ram[13] = 0x80; ram[14] = 0xF0; // 2
    ram[15] = 0xF0; ram[16] = 0x10; ram[17] = 0xF0; ram[18] = 0x10; ram[19] = 0xF0; // 3
    ram[20] = 0x90; ram[21] = 0x90; ram[22] = 0xF0; ram[23] = 0x10; ram[24] = 0x10; // 4
    ram[25] = 0xF0; ram[26] = 0x80; ram[27] = 0xF0; ram[28] = 0x10; ram[29] = 0xF0; // 5
    ram[30] = 0xF0; ram[31] = 0x80; ram[32] = 0xF0; ram[33] = 0x90; ram[34] = 0xF0; // 6
    ram[35] = 0xF0; ram[36] = 0x10; ram[37] = 0x20; ram[38] = 0x40; ram[39] = 0x40; // 7
    ram[40] = 0xF0; ram[41] = 0x90; ram[42] = 0xF0; ram[43] = 0x90; ram[44] = 0xF0; // 8
    ram[45] = 0xF0; ram[46] = 0x90; ram[47] = 0xF0; ram[48] = 0x10; ram[49] = 0xF0; // 9
    ram[50] = 0xF0; ram[51] = 0x90; ram[52] = 0xF0; ram[53] = 0x90; ram[54] = 0x90; // A
    ram[55] = 0xE0; ram[56] = 0x90; ram[57] = 0xE0; ram[58] = 0x90; ram[59] = 0xE0; // B
    ram[60] = 0xF0; ram[61] = 0x80; ram[62] = 0x80; ram[63] = 0x80; ram[64] = 0xF0; // C
    ram[65] = 0xE0; ram[66] = 0x90; ram[67] = 0x90; ram[68] = 0x90; ram[69] = 0xE0; // D
    ram[70] = 0xF0; ram[71] = 0x80; ram[72] = 0xF0; ram[73] = 0x80; ram[74] = 0xF0; // E
    ram[75] = 0xF0; ram[76] = 0x80; ram[77] = 0xF0; ram[78] = 0x80; ram[79] = 0x80; // F
}

char PIXEL_OFF = ' ';
char PIXEL_ON  = '#';

void Chip8::init_screen_buffs() {
    screen = new uint8_t*[width]();
    for(int i = 0; i < width; ++i){
        screen[i] = new uint8_t[hight]();
        memset(screen[i], 0, hight);
    }
}

uint8_t Chip8::draw_sprite(uint8_t Vx, uint8_t Vy, uint16_t I, uint8_t n) {
    Vy %= hight;
    Vx %= width;
    mvprintw(20, 65, "Print %d[%d, %d]%d", I, Vx, Vy, n);
    for (int i = 0; i < n; ++i) {
        mvprintw(20 + i + 1, 65, "Sprite line 0x%x", ram[I + i]);
    }

    uint8_t vf = 0;
    for (uint8_t i = 0; i < n; ++i) {
        uint8_t sprite_line = ram[I + i];
        uint8_t pos_y = (Vy + i);
        if (pos_y >= hight) 
            return vf;
        for (int shift = 0; shift < 8; ++shift) {
            uint8_t pos_x = (Vx + 7 - shift);
            if (pos_x >= width) 
                continue;
            uint8_t tmp = screen[pos_x][pos_y];
            //screen[pos_x][pos_y] ^= ((sprite_line >> shift) & 1);
            screen[pos_x][pos_y] = ((sprite_line >> shift) & 1); // works better, don't know why
            if ((tmp == 1) && (screen[pos_x][pos_y] == 0)) {
                vf = 1;
            }
        }
    }
    draw();
    return vf;
}

void Chip8::draw() {
    for(int i = 0; i < width; ++i){
        for(int j = 0; j < hight; ++j) {
            mvaddch(j, i, screen[i][j] == 1 ? PIXEL_ON : PIXEL_OFF);
        }
    }
}

void Chip8::clearscr() {
    for(int i = 0; i < width; ++i){
        screen[i] = new uint8_t[hight]();
        memset(screen[i], 0, hight);
    }
    draw();
}

void Chip8::load_file_to_ram(string path) {
    FILE * filp = fopen(path.c_str(), "rb");
    program_length = fread(&ram[programs_start_location], 
                            sizeof(uint8_t), 
                            sizeof(ram) - programs_start_location, 
                            filp);
}

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
using std::chrono::high_resolution_clock;
using namespace std::chrono;

void Chip8::run() {
    PC = programs_start_location;
    srand (time(NULL));

    int cnt = 0;
    while(true) {
        mvprintw(0, 65, "---PC [0x%x] {%d}---\n", PC, cnt++);
        high_resolution_clock::time_point s = high_resolution_clock::now();
        execute_one_instruction();
        high_resolution_clock::time_point f = high_resolution_clock::now();

        duration<double> time_span = duration_cast<duration<double> >(f - s);

        double d = (1.0/500.0 - time_span.count());
        if (cnt % 1 == 0) 
            getch();
    }
}

void Chip8::execute_one_instruction() {
    uint8_t byte1_even = ram[PC];
    uint8_t byte2_odd  = ram[PC + 1];

    /*
     high byte  low byte
     byte1_even byte2_odd
     f    x     y    l
    |----|----||----|----|
         |<--nnn 12bit-->|
               |<kk 8bit>|
    */

    uint8_t f = (byte1_even >> 4) & 0xf;
    uint8_t x = byte1_even & 0xf;
    uint8_t y = (byte2_odd >> 4) & 0xf;
    uint8_t l = byte2_odd & 0xf;

    //Spec definitions
    uint8_t n = l;
    uint16_t nnn = (((uint16_t)byte1_even) << 8 | byte2_odd) & 0xfff;
    uint8_t kk = byte2_odd;
    uint8_t F = 0xF;
    
    mvprintw(1, 65, "f    [0x%02x]  [%d]\n", f, f);
    mvprintw(2, 65, "nnn  [0x%04x]  [%d]\n", nnn, nnn);
    mvprintw(3, 65, "n    [0x%02x]  [%d]\n", n, n);
    mvprintw(4, 65, "x    [0x%02x]  [%d]\n", x, x);
    mvprintw(5, 65, "y    [0x%02x]  [%d]\n", y, y);
    mvprintw(6, 65, "kk   [0x%02x]  [%d]\n", kk, kk);
    mvprintw(7, 65, "bytes[0x%x%x]\n\n", byte1_even, byte2_odd);

    PC += 2;
    switch (f)
    {
        case 0:
            switch(byte2_odd) 
            {
                case 0xE0: clearscr(); break;
                case 0xEE: PC = stack[SP]; --SP; break;
                default: break; //PC = nnn; break;
            }
            break;
        case 1: PC = nnn; break;
        case 2: ++SP; stack[SP] = PC; PC = nnn; break;
        case 3: if (V[x] == kk) PC += 2; break;
        case 4: if (V[x] != kk) PC += 2; break;
        case 5: if (V[x] == V[y]) PC += 2; break;
        case 6: V[x] = kk; break;
        case 7: V[x] += kk; break;
        case 8:
            switch (l) //math operations
            {
                case 0: V[x] = V[y]; break;
                case 1: V[x] |= V[y]; break;
                case 2: V[x] &= V[y]; break;
                case 3: V[x] ^= V[y]; break;
                case 4: V[F] = ((uint16_t)V[x] + (uint16_t)V[y])>255; V[x] = (uint8_t)(V[x] + V[y]); break;
                case 5: V[F] = V[x] > V[y]; V[x] -= V[y]; break;
                case 6: V[F] = V[x] & 1; V[x] /= 2; break;
                case 7: V[F] = V[y] > V[x]; V[x] = V[y] - V[x]; break;
                case 0xE: V[F] = (V[x] >> 7) & 1; V[x] *= 2; break;
                default: break;
            }
            break;
        case 9: if (V[x] != V[y]) PC += 2; break;
        case 0xA: I = nnn; break;
        case 0xB: PC = nnn + V[0]; break;
        case 0xC: V[x] = (rand() & 256) & kk; break;
        case 0xD: V[F] = draw_sprite(V[x], V[y], I, n); break;
        case 0xE:
            switch (byte2_odd)
            {
                case 0x9E:
                    /*
                    Ex9E - SKP Vx
                    Skip next instruction if key with the value of Vx is pressed.
                    Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
                    */
                    break;
                case 0xA1:
                    /*
                    Skip next instruction if key with the value of Vx is not pressed.
                    Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
                    */
                    break;
                default: 
                    break;
            }
            break;
        case 0xF:
            switch (byte2_odd) {
                case 0x07: V[x] = DT; break;
                case 0x0A: 
                    /*
                    Wait for a key press, store the value of the key in Vx.
                    All execution stops until a key is pressed, then the value of that key is stored in Vx.
                    */
                    break;
                case 0x15: DT = V[x]; break;
                case 0x18: ST = V[x]; break;
                case 0x1E: I = (V[x] + I) & 0x0fff; break;
                case 0x29: I = V[x]*5; break;
                case 0x33: 
                    ram[I + 0] = V[V[x]] / 100;
                    ram[I + 1] = (V[V[x]] / 10) % 10;
                    ram[I + 2] = V[V[x]] % 10;
                    break;
                case 0x55:
                    for (uint8_t i = 0; i <= x; ++i) {
                        ram[I + i] = V[i];
                    }
                    break;
                case 0x65:
                    for (uint8_t i = 0; i <= x; ++i) {
                        V[i] = ram[I + i];
                    }
                    break;
                default: 
                    break;
            }
            break;
        default:
            mvprintw(60, 65, "Unknown command; Exit(1)\n");
            break;
    }
}