#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <stdint.h>

class chip
{
public:
    virtual ~chip() {}
    virtual void emulateCycle() = 0;
    virtual bool loadApplication(char *fileName) = 0;
    virtual int getSize() = 0;
    virtual bool isDrawn() = 0;
    virtual void setDrawnFlag(int value) = 0;
    virtual uint8_t getDisplayData(int pos) = 0;
    virtual int getDisplayWidth() = 0;
    virtual int getDisplayHeight() = 0;
    virtual void setKey(int pos, int value) = 0;
};
class chip8 : public chip
{
private:
    enum chip8MemoryMap
    {
        // Chip 8 interpreter (contains font set in emu)
        interpreterBegin = 0x000,
        interpreterEnd = 0x1FF,
        // Used for the built in 4x5 pixel font set (0-F)
        fontBegin = 0x050,
        fontEnd = 0x0A0,
        // Program ROM and work RAM
        romRamBegin = 0x200,
        romRamEnd = 0xFFF
    };
    // The Chip 8 has 35 opcodes which are all two bytes long
    uint16_t opcode;

    // The Chip 8 has 4K memory in total
    uint8_t memory[4096];

    // The Chip 8 has 15 8-bit general purpose registers named V0
    uint8_t registerV[16];

    // Index register I and a program counter (pc) which can have a value from 0x000 to 0xFFF
    uint16_t index;
    uint16_t programCounter;

    // the screen has a total of 2048 pixels (64 x 32)
    uint8_t gfx[64 * 32];

    // Interupts and hardware registers, count at 60 Hz
    uint8_t delayTimer;
    uint8_t soundTimer;

    // The system has 16 levels of stack and a stack pointer is used to remember level of the stack
    uint16_t stack[16];
    uint16_t stackPointer;
    // the Chip 8 has a HEX based keypad (0x0-0xF)
    uint8_t key[16];
    uint8_t drawFlag;
    void opcodeOperation(uint16_t opcode);
    void initialize();
    void debugRender();

public:
    chip8() {}
    ~chip8() {}
    int getSize() override
    {
        return sizeof(gfx);
    }
    bool isDrawn() override
    {
        return drawFlag;
    }
    uint8_t getDisplayData(int pos) override
    {
        return gfx[pos];
    }
    int getDisplayWidth() override
    {
        return 64;
    }
    int getDisplayHeight() override
    {
        return 32;
    }
    void setDrawnFlag(int value) override
    {
        drawFlag = value;
    }
    void setKey(int pos, int value) override
    {
        if (pos > 16)
            return;
        key[pos] = value;
    }
    void emulateCycle() override;
    bool loadApplication(char *fileName) override;
};
#endif