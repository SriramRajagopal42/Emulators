#include <cstdint>
#include <fstream>
#include <chrono>
#include <string>
#include <random>
#include "SDL.h"
#include <iostream>

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

class Platform
{
public:
	Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
	{
		SDL_Init(SDL_INIT_VIDEO);

		SDL_CreateWindowAndRenderer(title, windowWidth, windowHeight, 0, &window, &renderer);

		texture = SDL_CreateTexture(
			renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
	}

	~Platform()
	{
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void Update(void const* buffer, int pitch)
	{
		SDL_UpdateTexture(texture, nullptr, buffer, pitch);
		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	bool ProcessInput(uint8_t* keys)
	{
		bool quit = false;

		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
				{
					quit = true;
				} break;

				case SDL_EVENT_KEY_DOWN:
				{
					switch (event.key.key)
					{
						case SDLK_ESCAPE:
						{
							quit = true;
						} break;

						case SDLK_X:
						{
							keys[0] = 1;
						} break;

						case SDLK_1:
						{
							keys[1] = 1;
						} break;

						case SDLK_2:
						{
							keys[2] = 1;
						} break;

						case SDLK_3:
						{
							keys[3] = 1;
						} break;

						case SDLK_Q:
						{
							keys[4] = 1;
						} break;

						case SDLK_W:
						{
							keys[5] = 1;
						} break;

						case SDLK_E:
						{
							keys[6] = 1;
						} break;

						case SDLK_A:
						{
							keys[7] = 1;
						} break;

						case SDLK_S:
						{
							keys[8] = 1;
						} break;

						case SDLK_D:
						{
							keys[9] = 1;
						} break;

						case SDLK_Z:
						{
							keys[0xA] = 1;
						} break;

						case SDLK_C:
						{
							keys[0xB] = 1;
						} break;

						case SDLK_4:
						{
							keys[0xC] = 1;
						} break;

						case SDLK_R:
						{
							keys[0xD] = 1;
						} break;

						case SDLK_F:
						{
							keys[0xE] = 1;
						} break;

						case SDLK_V:
						{
							keys[0xF] = 1;
						} break;
					}
				} break;

				case SDL_EVENT_KEY_UP:
				{
					switch (event.key.key)
					{
						case SDLK_X:
						{
							keys[0] = 0;
						} break;

						case SDLK_1:
						{
							keys[1] = 0;
						} break;

						case SDLK_2:
						{
							keys[2] = 0;
						} break;

						case SDLK_3:
						{
							keys[3] = 0;
						} break;

						case SDLK_Q:
						{
							keys[4] = 0;
						} break;

						case SDLK_W:
						{
							keys[5] = 0;
						} break;

						case SDLK_E:
						{
							keys[6] = 0;
						} break;

						case SDLK_A:
						{
							keys[7] = 0;
						} break;

						case SDLK_S:
						{
							keys[8] = 0;
						} break;

						case SDLK_D:
						{
							keys[9] = 0;
						} break;

						case SDLK_Z:
						{
							keys[0xA] = 0;
						} break;

						case SDLK_C:
						{
							keys[0xB] = 0;
						} break;

						case SDLK_4:
						{
							keys[0xC] = 0;
						} break;

						case SDLK_R:
						{
							keys[0xD] = 0;
						} break;

						case SDLK_F:
						{
							keys[0xE] = 0;
						} break;

						case SDLK_V:
						{
							keys[0xF] = 0;
						} break;
					}
				} break;
			}
		}

		return quit;
	}

private:
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_Texture* texture{};
};

class Chip8 {
    #define Vx ((opcode & 0x0F00u) >> 8u)
    #define Vy ((opcode & 0x00F0u) >> 4u)
    #define nnn (opcode & 0x0FFFu)
    #define kk (opcode & 0x00FFu)

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

            // Set up opcode hash map
            table[0x0] = &Table0;
            table[0x1] = &op_1nnn;
            table[0x2] = &op_2nnn;
            table[0x3] = &op_3xkk;
            table[0x4] = &op_4xkk;
            table[0x5] = &op_5xy0;
            table[0x6] = &op_6xkk;
            table[0x7] = &op_7xkk;
            table[0x8] = &Table8;
            table[0x9] = &op_9xy0;
            table[0xA] = &op_Annn;
            table[0xB] = &op_Bnnn;
            table[0xC] = &op_Cxkk;
            table[0xD] = &op_Dxyn;
            table[0xE] = &TableE;
            table[0xF] = &TableF;

            // Not all spots in table0, tableE, and tableF will be mapped to actual operations, so fill in everything with nops first so that an accidental index won't break anything
            // All spots in table 8 have a function mapped to them, however, so we don't need to fill in nops for that
            for (uint8_t i = 0; i <= 0xEu; i++) {
                table0[i] = &nop;
                tableE[i] = &nop;
            }

            for (uint8_t i = 0; i <= 0x65u; i++) {
                tableF[i] = &nop;
            }

            table0[0x0] = &op_00E0;
            table0[0xE] = &op_00EE;

            table8[0x0] = &op_8xy0;
            table8[0x1] = &op_8xy1;
            table8[0x2] = &op_8xy2;
            table8[0x3] = &op_8xy3;
            table8[0x4] = &op_8xy4;
            table8[0x5] = &op_8xy5;
            table8[0x6] = &op_8xy6;
            table8[0x7] = &op_8xy7;
            table8[0xE] = &op_8xyE;

            tableE[0x1] = &op_ExA1;
            tableE[0xE] = &op_Ex9E;

            tableF[0x07] = &op_Fx07;
            tableF[0x0A] = &op_Fx0A;
            tableF[0x15] = &op_Fx15;
            tableF[0x18] = &op_Fx18;
            tableF[0x1E] = &op_Fx1E;
            tableF[0x29] = &op_Fx29;
            tableF[0x33] = &op_Fx33;
            tableF[0x55] = &op_Fx55;
            tableF[0x65] = &op_Fx65;    
        }

        void cycle() {
            // Fetch next two bytes in memory, since one instruction is two bytes
            opcode = (memory[pc] << 8u) | memory[pc + 1];

            // Set program counter to point to next instruction
            pc += 2;

            // Use function hashmap to find correct function to use, using the first hex digit as the hash function
            ((*this).*(table[opcode >> 12u]))(); // Right shift here is logical since opcode is unsigned, so no worry about MSB bleeding over into rest of the number

            // Decrement timers every cycle
            if (delay_timer) {
                delay_timer--;
            }

            if (sound_timer) {
                sound_timer--;
            }
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

        // Creates type for function in Chip8 so that I can use it to make function table
        typedef void (Chip8::*cpu_instr)();

        // Create opcode hash table
        // One main table, plus subtables for opcodes that have multiple instructions that start with a certain hex digit
        cpu_instr table[0xF + 1];
        cpu_instr table0[0xE + 1];
        cpu_instr table8[0xE + 1];
        cpu_instr tableE[0xE + 1];
        cpu_instr tableF[0x65 + 1];

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
            pc = nnn;
        }

        void op_2nnn() {
            // Call subroutine at nnn
            sp++;
            stack[sp] = pc;
            pc = nnn;
        }

        void op_3xkk() {
            // Skip next instruction if Vx == kk
            pc += 2 * (registers[Vx] == kk);
        }

        void op_4xkk() {
            // Skip next instruction if Vx != kk
            pc += 2 * (registers[Vx] != kk);
        }

        void op_5xy0() {
            // Skip next instruction if Vx == Vy
            pc += 2 * (registers[Vx] == registers[Vy]);
        }

        void op_6xkk() {
            // Set Vx = kk
            registers[Vx] = kk;
        }

        void op_7xkk() {
            // Set Vx = Vx + kk
            registers[Vx] += kk;
        }

        void op_8xy0() {
            // Set Vx = Vy
            registers[Vx] = registers[Vy];
        }

        void op_8xy1() {
            // Set Vx OR Vy
            registers[Vx] |= registers[Vy];
        }

        void op_8xy2() {
            // Set Vx AND Vy
            registers[Vx] &= registers[Vy];
        }

        void op_8xy3() {
            // Set Vx XOR Vy
            registers[Vx] ^= registers[Vy];
        }

        void op_8xy4() {
            // Set Vx = Vx + Vy, set VF = carry
            uint16_t sum = registers[Vx] + registers[Vy]; // Do it like this because we need to cast to 16 bits since we are checking for overflow
            registers[0xFu] = (sum > 255u); // Set VF true if overflow
            registers[Vx] = (sum & 0xFFu); // Gets bottom 8 bits of sum and stores in Vx
        }

        void op_8xy5() {
            // Set Vx = Vx - Vy, set VF = NOT borrow
            registers[0xFu] = (registers[Vx] > registers[Vy]);
            registers[Vx] -= registers[Vy];
        }

        void op_8xy6() {
            // Set Vx = Vx SHR 1
            registers[0xFu] = registers[Vx] & 0x1u;
            registers[Vx] >>= 1;
        }

        void op_8xy7() {
            // Set Vx = Vy - Vx, set VF = NOT borrow
            registers[0xFu] = (registers[Vy] > registers[Vx]);
            registers[Vx] = registers[Vy] - registers[Vx];
        }

        void op_8xyE() {
            // Set Vx = Vx SHL 1
            registers[0xFu] = (registers[Vx] & 0x80u) >> 7u;
            registers[Vx] <<= 1;
        }

        void op_9xy0() {
            // Skip next instruction if Vx != Vy
            pc += 2 * (registers[Vx] != registers[Vy]);
        }

        void op_Annn() {
            // Set I = nnn
            index = opcode & nnn;
        }

        void op_Bnnn() {
            // Jump to location nnn + V0
            pc = nnn + registers[0];
        }

        void op_Cxkk() {
            // Set Vx = random byte AND kk
            registers[Vx] = randByte(randGen) & kk;
        }

        void op_Dxyn() {
            // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
            uint8_t n = opcode & 0x000Fu;

            // Get top left coords + wrap
            uint8_t x_coord = registers[Vx] % DISPLAY_WIDTH;
            uint8_t y_coord = registers[Vy] % DISPLAY_HEIGHT;

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
            pc += 2 * (keypad[registers[Vx]]);
        }

        void op_ExA1() {
            // Skip next instruction if key with the value of Vx is not pressed
            pc += 2 * (!keypad[registers[Vx]]);
        }

        void op_Fx07() {
            // Set Vx = delay timer value
            registers[Vx] = delay_timer;
        }

        // https://retrocomputing.stackexchange.com/questions/358/how-are-held-down-keys-handled-in-chip-8
        void op_Fx0A() {
            // Wait for a key press, store the value of the key in Vx

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
                        break;
                    }
                }
            }
        }

        void op_Fx15() {
            // Set delay timer = Vx
            delay_timer = registers[Vx];
        }

        void op_Fx18() {
            // Set sound timer = Vx
            sound_timer = registers[Vx];
        }

        void op_Fx1E() {
            // Set I = I + Vx
            index += registers[Vx];
        }

        void op_Fx29() {
            // Set I = location of sprite for digit Vx
            index = FONTSET_START_ADDRESS + (5 * registers[Vx]); // Each digit is 5 bytes long, so multiply desired digit by 5 to get correct byte offset
        }

        void op_Fx33() {
            // Store BCD representation of Vx in memory locations I, I + 1, and I + 2
            
            // So 158 would have 1 at I, 5 at I + 1, and 8 at I + 3
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
            for (uint8_t i = 0; i <= Vx; i++) {
                memory[index + i] = registers[i];
            }
        }

        void op_Fx65() {
            // Read registers V0 through Vx from memory starting at location I
            for (uint8_t i = 0; i <= Vx; i++) {
                registers[i] = memory[index + i];
            }
        }

        // When instruction starts with relevant hex digit, call one of these functions to call the correct function within the subtable
        void Table0() {
            ((*this).*(table0[opcode & 0x000Fu]))();
        }

        void Table8() {
            ((*this).*(table8[opcode & 0x000Fu]))();
        }

        void TableE() {
            ((*this).*(tableE[opcode & 0x000Fu]))();
        }

        void TableF() {
            ((*this).*(tableF[opcode & 0x000FFu]))(); // Instruction chosen for this table depends on last two hex digitd
        }

        // NOP instruction used to fill in holes in opcode hash table
        void nop() {}
};

int main(int argc, char** argv) {
    if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

    int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];

	Platform platform("CHIP-8 Emulator", DISPLAY_WIDTH * videoScale, DISPLAY_HEIGHT * videoScale, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	Chip8 chip8;
	chip8.load_rom(romFilename);

	int videoPitch = sizeof(chip8.display[0]) * DISPLAY_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform.ProcessInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.cycle();

			platform.Update(chip8.display, videoPitch);
		}
	}

	return 0;

}