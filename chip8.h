#include <string>

using namespace std;

class Chip8 {
    private:
        uint8_t ram[4096];
        uint8_t V[16];
        uint16_t stack[16];
        uint16_t default_frequency;
        
        uint16_t programs_start_location;
        uint16_t programs_start_location_eti660;
        uint16_t I; // only 12 bits are used. Values [0~4095]
        uint16_t program_length;

        uint8_t SP; // Stack Pointer
        uint16_t PC; // Program Counter; only 12 bits are used. Values [0~4095]

        uint8_t DT; // Delay Timer
        uint8_t ST; // Sound Timer
        uint8_t width, hight;
        uint8_t **screen;
        uint8_t active_buffer_idx; // active screen buffer index
        uint8_t Hz_rate;

        void execute_one_instruction();
        void load_hex_digit_sprites();
        void init_screen_buffs();
        uint8_t draw_sprite(uint8_t Vx, uint8_t Vy, uint16_t I, uint8_t n);
        void clearscr();
        void draw();
        uint8_t get_key();

    public:
        Chip8();
        void run();
        void load_file_to_ram(string path);
};