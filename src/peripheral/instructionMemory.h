#pragma once

class InstructionMemory{
    public:
        InstructionMemory(){}

        virtual void setInstruction(byte address, byte instruction) = 0;

        virtual byte getInstruction(byte address) = 0;
};
