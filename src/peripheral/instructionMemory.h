#pragma once

class InstructionMemory{
    public:
        InstructionMemory(){}

        virtual void setInstruction(byte address, byte instruction) = 0;

        virtual bool goToAddress(uint32_t address, uint32_t max_time_us_for_cycle){};
        virtual byte getInstruction(uint32_t address)=0;
        virtual byte readInstruction(){};
};
