#include "../peripheral/instructionMemory.h"

typedef unsigned char byte;

class VirtualInstructionMemory : public InstructionMemory{
    public:
        VirtualInstructionMemory(){}

        void setInstruction(byte address, byte instruction){
            instructions[address] = instruction;
        }

        byte getInstruction(byte address){
            return instructions[address];
        }
    private:
        byte instructions[32];//This has to be low, other wise the memory will get filled up
};