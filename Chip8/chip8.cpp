#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>

const unsigned int NUM_REGISTERS = 16;
const unsigned int MEM_SIZE = 4096;
const unsigned int STACK_SIZE = 16;
const unsigned int NUM_KEYS = 16;
const unsigned int DISPLAY_HEIGHT = 32;
const unsigned int DISPLAY_WIDTH = 64;

const unsigned int START_ADDRESS = 0x200;

const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;

uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class Chip8 {
    public:
        uint8_t registers[NUM_REGISTERS]{}; // 16 8-bit registers
        uint8_t memory[MEM_SIZE]{}; // 4KB of memory
        uint16_t index{}; // Index register
        uint16_t pc{}; // Program counter
        uint16_t stack[STACK_SIZE]{}; // Stack
        uint8_t sp{}; // Stack pointer
        uint8_t delay_timer{}; // Delay timer
        uint8_t sound_timer{}; // Sound timer
        uint8_t keypad[NUM_KEYS]{}; // Hex keypad
        uint32_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT]{}; // Monochrome display
        uint16_t opcode{}; // Current opcode

        Chip8() 
        : randGen(std::chrono::system_clock::now().time_since_epoch().count())
        {
            pc = START_ADDRESS; // Program counter starts at 0x200

            // Load fontset into memory
            for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
                memory[FONTSET_START_ADDRESS + i] = fontset[i];
            }

            // Initialize RNG
		    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
        }
        
        void load_rom(const char* filename) {
            // Open the file as a stream of binary and move the file pointer to the end
	        std::ifstream file(filename, std::ios::binary | std::ios::ate);

            if (file.is_open()) {
                // Get the size of the file and allocate a buffer to hold the contents
                std::streampos size = file.tellg();
                char* buffer = new char[size];

                // Go back to the beginning of the file and fill the buffer
                file.seekg(0, std::ios::beg);
                file.read(buffer, size);
                file.close();

                // Load the ROM contents into the Chip8's memory, starting at 0x200
                for (long i = 0; i < size; ++i) {
                    memory[START_ADDRESS + i] = buffer[i];
                }

                // Free the buffer
                delete[] buffer;
            }

        }

    private:
        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;
        uint8_t fav_key = -1; // Used for op_Fx0A()

        // Helper function to get Vx, Vy, and kk since I do that a lot
        uint8_t* get_Vx_other(bool need_kk) {
            // Returns an array with Vx as well as either Vy or kk
            uint8_t arr[2]{};

            arr[0] = (opcode & 0x0F00u) >> 8u; // Vx

            if (need_kk) {
                arr[1] = (opcode & 0x00FFu); // kk
            } else {
                arr[1] = (opcode & 0x00F0u) >> 4u; // Vy
            }

            return arr;
        }

        void op_00E0() {
            // Clear the display
            for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++i) {
                display[i] = 0;
            }
        }

        void op_00EE() {
            // Return from a subroutine
            pc = stack[sp];
            sp--;
        }

        void op_1nnn() {
            // Jump to location nnn
            pc = (opcode & 0x0FFFu);
        }

        void op_2nnn() {
            // Call subroutine at nnn
            sp++;
            stack[sp] = pc;
            pc = (opcode & 0x0FFFu);
        }

        void op_3xkk() {
            // Skip next instruction if Vx == kk
            uint8_t* arr = get_Vx_other(true);
            pc += 2 * (registers[arr[0]] == arr[1]);
        }

        void op_4xkk() {
            // Skip next instruction if Vx != kk
            uint8_t* arr = get_Vx_other(true);
            pc += 2 * (registers[arr[0]] != arr[1]);
        }

        void op_5xy0() {
            // Skip next instruction if Vx == Vy
            uint8_t* arr = get_Vx_other(false);
            pc += 2 * (registers[arr[0]] == registers[arr[1]]);
        }

        void op_6xkk() {
            // Set Vx = kk
            uint8_t* arr = get_Vx_other(true);
            registers[arr[0]] = arr[1];
        }

        void op_7xkk() {
            // Set Vx = Vx + kk
            uint8_t* arr = get_Vx_other(true);
            registers[arr[0]] += arr[1];
        }

        void op_8xy0() {
            // Set Vx = Vy
            uint8_t* arr = get_Vx_other(false);
            registers[arr[0]] = registers[arr[1]];
        }

        void op_8xy1() {
            // Set Vx OR Vy
            uint8_t* arr = get_Vx_other(false);
            registers[arr[0]] |= registers[arr[1]];
        }

        void op_8xy2() {
            // Set Vx AND Vy
            uint8_t* arr = get_Vx_other(false);
            registers[arr[0]] &= registers[arr[1]];
        }

        void op_8xy3() {
            // Set Vx XOR Vy
            uint8_t* arr = get_Vx_other(false);
            registers[arr[0]] ^= registers[arr[1]];
        }

        void op_8xy4() {
            // Set Vx = Vx + Vy, set VF = carry
            uint8_t* arr = get_Vx_other(false);
            uint16_t sum = registers[arr[0]] + registers[arr[1]]; // Do it like this because we need to cast to 16 bits since we are checking for overflow
            registers[0xFu] = (sum > 255u); // Set VF true if overflow
            registers[arr[0]] = (sum & 0xFFu); // Gets bottom 8 bits of sum and stores in Vx
        }

        void op_8xy5() {
            // Set Vx = Vx - Vy, set VF = NOT borrow
            uint8_t* arr = get_Vx_other(false);
            registers[0xFu] = (registers[arr[0] > registers[arr[1]]]);
            registers[arr[0]] -= registers[arr[1]];
        }

        void op_8xy6() {
            // Set Vx = Vx SHR 1
            int8_t Vx = (opcode & 0x0F00u) >> 8u;
            registers[0xFu] = registers[Vx] & 0x1u;
            registers[Vx] >>= 1;
        }

        void op_8xy7() {
            // Set Vx = Vy - Vx, set VF = NOT borrow
            uint8_t* arr = get_Vx_other(false);
            registers[0xFu] = (registers[arr[1] > registers[arr[0]]]);
            registers[arr[0]] = registers[arr[1]] - registers[arr[0]];
        }

        void op_8xyE() {
            // Set Vx = Vx SHL 1
            int8_t Vx = (opcode & 0x0F00u) >> 8u;
            registers[0xFu] = (registers[Vx] & 0x80u) >> 7u;
            registers[Vx] <<= 1;
        }

        void op_9xy0() {
            // Skip next instruction if Vx != Vy
            uint8_t* arr = get_Vx_other(false);
            pc += 2 * (registers[arr[0]] != registers[arr[1]]);
        }

        void op_Annn() {
            // Set I = nnn
            index = opcode & 0x0FFFu;
        }

        void op_Bnnn() {
            // Jump to location nnn + V0
            pc = (opcode & 0x0FFFu) + registers[0];
        }

        void op_Cxkk() {
            // Set Vx = random byte AND kk
            uint8_t* arr = get_Vx_other(true);
            registers[arr[0]] = randByte(randGen) & arr[1];
        }

        void op_Dxyn() {
            // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
            uint8_t* arr = get_Vx_other(false);
            uint8_t n = opcode & 0x000Fu;

            // Get top left coords + wrap
            uint8_t x_coord = registers[arr[0]] % DISPLAY_WIDTH;
            uint8_t y_coord = registers[arr[1]] % DISPLAY_HEIGHT;

            // Set default value of collision detector to 0
            registers[0xFu] = 0;

            // Loop through every row, then every pixel in that row, to check for collision and then XOR
            for (uint8_t row = 0; row < n; row++) {
                uint8_t cur_row = memory[index + row];

                for (uint8_t col = 0; col < 8; col++) {
                    bool sprite_pixel = (cur_row << col) & 0x80u; // Gets col'th bit in cur_row
                    uint32_t* screen_pixel = &display[((y_coord + row) * DISPLAY_WIDTH) + (x_coord + col)]; // y_coord + row finds the index of the correct row, and multiplying by WIDTH does the correct offset to get to the actual row in the display. The x_coord + col then takes you to the correct position within that row
                    
                    // Only need to do stuff if sprite pixel is on
                    if (sprite_pixel) {

                        // If screen pixel is also on, there's a collision
                        if (*screen_pixel) {
                            registers[0xFu] = 1;
                        }

                        *screen_pixel ^= 0xFFFFFFFF;
                    }
                }
            }
        }

        void op_Ex9E() {
            // Skip next instruction if key with the value of Vx is pressed
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            pc += 2 * (keypad[registers[Vx]]);
        }

        void op_ExA1() {
            // Skip next instruction if key with the value of Vx is not pressed
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            pc += 2 * (!keypad[registers[Vx]]);
        }

        void op_Fx07() {
            // Set Vx = delay timer value
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            registers[Vx] = delay_timer;
        }

        // https://retrocomputing.stackexchange.com/questions/358/how-are-held-down-keys-handled-in-chip-8
        void op_Fx0A() {
            // Wait for a key press, store the value of the key in Vx
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;

            // If key is held down, value is stored in fav_key until it is lifted, and only then is the key stored in Vx
            // Did this with while loop before, but that stops timers from ticking
            if (fav_key != -1 && !keypad[fav_key]) {
                registers[Vx] = fav_key;
                fav_key = -1;
            }
            else { // If no key has been lifted
                // Find value of key that is down
                for (uint8_t i = 0; i < NUM_KEYS; i++) {
                    if (keypad[i]) {
                        fav_key = i;
                    }
                }

                pc -= 2;
            }
        }

        void op_Fx15() {
            // Set delay timer = Vx
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            delay_timer = registers[Vx];
        }

        void op_Fx18() {
            // Set sound timer = Vx
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            sound_timer = registers[Vx];
        }

        void op_Fx1E() {
            // Set I = I + Vx
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            index += registers[Vx];
        }

        void op_Fx29() {
            // Set I = location of sprite for digit Vx
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            index = FONTSET_START_ADDRESS + (5 * registers[Vx]); // Each digit is 5 bytes long, so multiply desired digit by 5 to get correct byte offset
        }

        void op_Fx33() {
            // Store BCD representation of Vx in memory locations I, I + 1, and I + 2
            
            // So 158 would have 1 at I, 5 at I + 1, and 8 at I + 3
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            uint8_t val = registers[Vx]; // Actually creating a new variable since I will be modifying this, and don't want to modify registers[Vx]
            
            // Ex: Start with 158
            memory[index + 2] = val % 10; // 158 % 10 = 8, stored in I + 2
            val /= 10; // 158 / 10 = 15 (truncated)
            memory[index + 1] = val % 10; // 15 % 10 = 5, stored in I + 1
            val /= 10; // 15 / 10 = 1 (truncated)
            memory[index] = val; // 1 is stored in I
        }

        void op_Fx55() {
            // Store registers V0 through Vx in memory starting at location I
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            for (uint8_t i = 0; i <= Vx; i++) {
                memory[index + i] = registers[i];
            }
        }

        void op_Fx65() {
            // Read registers V0 through Vx from memory starting at location I
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            for (uint8_t i = 0; i <= Vx; i++) {
                registers[i] = memory[index + i];
            }
        }
};