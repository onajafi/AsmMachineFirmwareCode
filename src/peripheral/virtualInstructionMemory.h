#include "../peripheral/instructionMemory.h"
#include "singletonLogger.h"

typedef uint8_t byte;
#define MAX_INSTRUCTIONS 32

class VirtualInstructionMemory : public InstructionMemory{
    public:
        VirtualInstructionMemory(){}

        void setInstruction(byte address, byte instruction){
            instructions[address] = instruction;
        }

        byte getInstruction(uint32_t address){
            if(address >= MAX_INSTRUCTIONS){
                SingletonLogger::getInstance().log(DebugLevel::ERROR, "Address out of bounds: %d", address);
                return 0;
            }
            return instructions[address];
        }
    private:
        byte instructions[MAX_INSTRUCTIONS];//This has to be low, other wise the memory will get filled up
};