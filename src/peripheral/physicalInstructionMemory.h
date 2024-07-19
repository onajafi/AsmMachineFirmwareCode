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
        sliderMove(_sliderMove), irReader(_irReader){}

    void setInstruction(byte address, byte instruction) override{
        Serial.println("Can't write to Physical Memory");
    }

    bool goToAddress(byte address, uint32_t max_time_us_for_cycle){
        Serial.print("this->stepsLeftToGo: ");
        Serial.println(this->stepsLeftToGo);
        if(this->sliderIndex != address){
            this->stepsLeftToGo = sliderMove->giveNumberOfStepsToMoveNRows(address - this->sliderIndex);
            this->sliderIndex = address;
        }
        Serial.print("New val: ");
        Serial.println(this->stepsLeftToGo);
        if(this->stepsLeftToGo == 0){
            return true;
        }

        int32_t maxStepsToGo = sliderMove->stepperForDuration(max_time_us_for_cycle);
        int tmpStepsToGo = sgn(this->stepsLeftToGo) * min(magnitude(this->stepsLeftToGo), maxStepsToGo);
        Serial.print("tmpStepsToGo: ");
        Serial.println(tmpStepsToGo);
        sliderMove->runStepper(magnitude(tmpStepsToGo), tmpStepsToGo > 0 ? HIGH : LOW, 1.5);
        this->stepsLeftToGo -= tmpStepsToGo;
        
        return false;
    }
    
    byte readInstruction(){
        return irReader->read();
    }
private:
    byte instructions[256];
    SliderMove* sliderMove;
    IrReader* irReader;
    int sliderIndex = 0;
    int stepsLeftToGo = 0;
};