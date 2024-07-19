#pragma once

class InstructionMemory{
    public:
        InstructionMemory(){}

        virtual void setInstruction(byte address, byte instruction) = 0;

        virtual bool goToAddress(byte address, uint32_t max_time_us_for_cycle){};
        virtual byte getInstruction(byte address){};
        virtual byte readInstruction(){};
};
