#include "instructionMemory.h"
#include "irReader.h"
#include "singletonLogger.h"


class PhysicalInstructionMemory : public InstructionMemory{
public:
    PhysicalInstructionMemory(SliderMove* _sliderMove, IrReader* _irReader):
        sliderMove(_sliderMove), irReader(_irReader){}

    void setInstruction(byte address, byte instruction) override{
        Serial.println("Can't write to Physical Memory");
    }
    
    byte getInstruction(byte address) override{
        sliderMove->stepNRows(address - this->sliderIndex, 1.5);
        this->sliderIndex = address;
        return irReader->read();
    }
private:
    byte instructions[256];
    SliderMove* sliderMove;
    IrReader* irReader;
    int sliderIndex = 0;
};