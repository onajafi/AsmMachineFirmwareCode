#include "instructionMemory.h"
#include "irReader.h"
#include "singletonLogger.h"

int sgn(int val){
    return (0 < val) - (val < 0);
}

int magnitude(int val){
    return val < 0 ? -val : val;
}

class PhysicalInstructionMemory : public InstructionMemory{
public:
    PhysicalInstructionMemory(SliderMove* _sliderMove, IrReader* _irReader):
        sliderMove(_sliderMove), irReader(_irReader){
            this->resetIndex();
        }

    void resetIndex(){
        sliderIndex = 0;
        stepsLeftToGo = 0;
    }

    void setInstruction(byte address, byte instruction) override{
        Serial.println("Can't write to Physical Memory");
    }

    byte getInstruction(uint32_t address) override{
        sliderMove->stepNRows(address - this->sliderIndex, 4.0);
        this->sliderIndex = address;
        return irReader->read();
    }

    bool goToAddress(byte address, uint32_t max_time_us_for_cycle){
        
        //TODO make concurrent singleton
        return false;
    }
    
    byte readInstruction(){
        return irReader->read();
    }

private:
    byte instructions[256];
    SliderMove* sliderMove;
    IrReader* irReader;
    int sliderIndex;
    int stepsLeftToGo;
};