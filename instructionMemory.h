
typedef unsigned char byte;

class VirtualInstructionMemory{
    public:
        InstructionMemory(){}

        void setInstruction(byte address, byte instruction){
            instructions[address] = instruction;
        }

        byte getInstruction(byte address){
            return instructions[address];
        }
    private:
        byte instructions[256];
};