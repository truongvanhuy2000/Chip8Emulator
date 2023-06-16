#include <chip8/chip8.hpp>
#include <cstring>
#include <iostream>
#include <fstream>

uint8_t chip8FontSet[80] =
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
// Initialize essential component
void chip8::initialize()
{
    programCounter = chip8MemoryMap::romRamBegin; // Program counter starts at 0x200
    opcode = 0;                                   // Reset current opcode
    index = 0;                                    // Reset index register
    stackPointer = 0;                             // Reset stack pointer to 0

    memset(gfx, 0, sizeof(gfx));             // Clear display
    memset(registerV, 0, sizeof(registerV)); // Clear register V0 - VF
    memset(memory, 0, sizeof(memory));       // Clear memory
    memset(stack, 0, sizeof(stack));         // Clear stack

    // Load font Set
    for (int i = 0; i < chip8MemoryMap::fontEnd - chip8MemoryMap::fontBegin; i++)
    {
        memory[i + chip8MemoryMap::fontBegin] = chip8FontSet[i];
    }
    // Reset timer
    delayTimer = 0;
    soundTimer = 0;

    drawFlag = true;
}

// Load the game to memory
bool chip8::loadApplication(const char *fileName)
{
    initialize();
    std::cout << "Loading: " << fileName << std::endl;
    // First open the file

    std::ifstream infile(fileName);

    if (!infile)
    {
        std::cerr << "Failed to open file" << std::endl;
        return false;
    }

    // determine the size of the file by seeking to the end of the file using the
    infile.seekg(0, std::ios::end);
    int filesize = infile.tellg();
    // then seek back to the beginning of the file
    infile.seekg(0, std::ios::beg);
    // read the file
    uint8_t *buffer = new uint8_t[filesize];
    infile.read((char *)buffer, filesize);
    // Close file
    infile.close();

    // store game file to memory
    if (4096 - chip8MemoryMap::romRamBegin < filesize)
    {
        delete[] buffer;
        std::cerr << "File size too large" << std::endl;
        return false;
    }
    for (int i = 0; i < filesize; i++)
    {
        memory[chip8MemoryMap::romRamBegin + i] = buffer[i];
    }
    delete[] buffer;
    return true;
}
void chip8::emulateCycle()
{
    opcode = memory[programCounter] << 8 | memory[programCounter + 1];
    opcodeOperation(opcode);
    if (delayTimer > 0)
    {
        --delayTimer;
    }
    if (soundTimer > 0)
    {
        if (soundTimer == 1)
        {
            std::cout << "BEEP!!" << std::endl;
        }
        soundTimer--;
    }
}
void chip8::opcodeOperation(uint16_t opcode)
{
    int x = ((opcode & 0x0F00) >> 8);
    int y = ((opcode & 0x00F0) >> 4);

    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0: // 0x00E0: Clears the screen
            memset(gfx, 0, sizeof(gfx));
            drawFlag = true;
            programCounter += 2;
            break;
        case 0x00EE: // 0x00EE: Returns from subroutine
            --stackPointer;
            programCounter = stack[stackPointer];
            programCounter += 2;
            break;
        default:
            std::cerr << "Uknown Opcode" << std::endl;
            break;
        }
        break;

    case 0x1000: // 0x1NNN: Jumps to address NNN
        programCounter = opcode & 0x0FFF;
        break;

    case 0x2000: // 0x2NNN: Calls subroutine at NNN.
        stack[stackPointer] = programCounter;
        stackPointer++;
        programCounter = opcode & 0x0FFF;
        break;

    case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN
        if (registerV[x] == (opcode & 0x00FF))
        {
            programCounter += 2;
        }
        programCounter += 2;
        break;

    case 0x4000: // 0x4XNN: Skips the next instruction if VX doesn't equal NN
        if (registerV[x] != (opcode & 0x00FF))
        {
            programCounter += 2;
        }
        programCounter += 2;
        break;

    case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY.

        if (registerV[x] == registerV[y])
        {
            programCounter += 2;
        }
        programCounter += 2;
        break;

    case 0x6000: // 0x6XNN: Sets VX to NN.
        registerV[x] = opcode & 0x00FF;
        programCounter += 2;
        break;

    case 0x7000: // 0x7XNN: Adds NN to VX.
        registerV[x] += opcode & 0x00FF;
        programCounter += 2;
        break;

    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000: // 0x8XY0: Sets VX to the value of VY
            registerV[x] = registerV[y];
            programCounter += 2;
            break;

        case 0x0001: // 0x8XY1: Sets VX to "VX OR VY"
            registerV[x] |= registerV[y];
            programCounter += 2;
            break;

        case 0x0002: // 0x8XY2: Sets VX to "VX AND VY"
            registerV[x] &= registerV[y];
            programCounter += 2;
            break;

        case 0x0003: // 0x8XY3: Sets VX to "VX XOR VY"
            registerV[x] ^= registerV[y];
            programCounter += 2;
            break;

        case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
            if (registerV[y] > (0xFF - registerV[x]))
                registerV[0xF] = 1; // carry
            else
                registerV[0xF] = 0;
            registerV[x] += registerV[y];
            programCounter += 2;
            break;

        case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
            if (registerV[y] > registerV[x])
                registerV[0xF] = 0; // borrow
            else
                registerV[0xF] = 1;
            registerV[x] -= registerV[y];
            programCounter += 2;
            break;

        case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
            registerV[0xF] = registerV[x] & 0x1;
            registerV[x] >>= 1;
            programCounter += 2;
            break;

        case 0x0007:                         // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
            if (registerV[x] > registerV[y]) // VY-VX
                registerV[0xF] = 0;          // there is a borrow
            else
                registerV[0xF] = 1;
            registerV[x] = registerV[y] - registerV[x];
            programCounter += 2;
            break;

        case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
            registerV[0xF] = registerV[x] >> 7;
            registerV[x] <<= 1;
            programCounter += 2;
            break;
        default:
            std::cerr << "Uknown Opcode" << std::endl;
        }
        break;
    case 0x9000:
        if (registerV[x] != registerV[y])
        {
            programCounter += 2;
        }
        programCounter += 2;
        break;
    case 0xA000: // ANNN: Sets I to the address NNN
        index = opcode & 0x0FFF;
        programCounter += 2;
        break;
    case 0xB000: // BNNN: Jumps to the address NNN plus V0
        programCounter = opcode & 0x0FFF + registerV[0];
        break;
    case 0xC000: // CXNN: Sets VX to a random number and NN
        registerV[x] = (rand() % 0xFF) & (opcode & 0x00FF);
        programCounter += 2;
        break;
    case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
    {
        uint8_t xCoord = registerV[x];
        uint8_t yCoord = registerV[y];
        uint8_t height = opcode & 0x000F;
        uint8_t width = 8;
        registerV[0xF] = 0;
        uint8_t pixel;
        for (int yPos = 0; yPos < height; yPos++)
        {
            pixel = memory[index + yPos];
            for (int xPos = 0; xPos < width; xPos++)
            {
                if (pixel & (0x80 >> xPos))
                {
                    if (gfx[(xCoord + xPos) + (yCoord + yPos) * 64] == 1)
                    {
                        registerV[0xF] = 1;
                    }
                    gfx[(xCoord + xPos) + (yCoord + yPos) * 64] ^= 1;
                }
            }
        }
        // debugRender();
        drawFlag = true;
        programCounter += 2;
        break;
    }
    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed
            if (key[registerV[x]] != 0)
                programCounter += 2;
            programCounter += 2;
            break;

        case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
            if (key[registerV[x]] == 0)
                programCounter += 2;
            programCounter += 2;
            break;

        default:
            std::cerr << "Uknown Opcode" << std::endl;
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007: // FX07: Sets VX to the value of the delay timer
            registerV[x] = delayTimer;
            programCounter += 2;
            break;

        case 0x000A: // FX0A: A key press is awaited, and then stored in VX
        {
            bool keyPress = false;
            for (int i = 0; i < 16; ++i)
            {
                if (key[i] != 0)
                {
                    registerV[(opcode & 0x0F00) >> 8] = i;
                    keyPress = true;
                }
            }
            // If we didn't received a keypress, skip this cycle and try again.
            if (!keyPress)
                return;

            programCounter += 2;
        }
        break;

        case 0x0015: // FX15: Sets the delay timer to VX
            delayTimer = registerV[x];
            programCounter += 2;
            break;

        case 0x0018: // FX18: Sets the sound timer to VX
            soundTimer = registerV[x];
            programCounter += 2;
            break;

        case 0x001E:                          // FX1E: Adds VX to I
            if (index + registerV[x] > 0xFFF) // VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
                registerV[0xF] = 1;
            else
                registerV[0xF] = 0;
            index += registerV[x];
            programCounter += 2;
            break;

        case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
            index = registerV[x] * 0x5 + chip8MemoryMap::fontBegin;
            programCounter += 2;
            std::cout << "font location: " << index << std::endl;
            break;

        case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
            memory[index] = registerV[x] / 100;
            memory[index + 1] = (registerV[x] / 10) % 10;
            memory[index + 2] = (registerV[x] % 100) % 10;
            programCounter += 2;
            break;

        case 0x0055: // FX55: Stores V0 to VX in memory starting at address I
            for (int i = 0; i <= x; ++i)
                memory[index + i] = registerV[i];

            // On the original interpreter, when the operation is done, I = I + X + 1.
            index += x + 1;
            programCounter += 2;
            break;

        case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I
            for (int i = 0; i <= x; ++i)
                registerV[i] = memory[index + i];

            // On the original interpreter, when the operation is done, I = I + X + 1.
            index += x + 1;
            programCounter += 2;
            break;

        default:
            std::cerr << "Uknown Opcode" << std::endl;
        }
        break;
    default:
        break;
    }
}
void chip8::debugRender()
{
    // Draw
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (gfx[x + y * 64] == 0)
                printf(" ");
            else
                printf("0");
        }
        printf("\n");
    }
    printf("\n");
}