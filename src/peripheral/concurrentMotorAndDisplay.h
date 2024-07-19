

/**
 * This class will handle the motor driving and led displays concurrently
*/
class ConcurrentMotorAndDisplay {

    SliderMove* sliderMove;
    IrReader* irReader;
    LedInterfaceHandler* ledInterfaceHandler;
    int stepsLeftToGo = 0;
    int sliderAddress = 0;

    int sgn(int val){
        return (0 < val) - (val < 0);
    }

    int magnitude(int val){
        return val < 0 ? -val : val;
    }

    void move2Target(uint32_t max_time_us_for_function){
        uint32_t start = micros();
        if(this->stepsLeftToGo == 0){
            return true;
        }

        int32_t maxStepSizeToGo = sliderMove->stepsForDuration(max_time_us_for_function);
        int tmpStepsToGo = sgn(this->stepsLeftToGo) * min(magnitude(this->stepsLeftToGo), maxStepSizeToGo);
        
        sliderMove->runStepper(magnitude(tmpStepsToGo), tmpStepsToGo > 0 ? HIGH : LOW, 1.5);
        this->stepsLeftToGo -= tmpStepsToGo;
        
        return false;
    }

public:
    ConcurrentMotorAndDisplay(SliderMove* _sliderMove, IrReader* _irReader, LedInterfaceHandler* _ledInterfaceHandler){
        this->sliderMove = _sliderMove;
        this->irReader = _irReader;
        this->ledInterfaceHandler = _ledInterfaceHandler;
    }

    //Return true if it has reached
    bool setTargetAddress(byte address){
        if(this->stepsLeftToGo != 0)
            return false;
        this->stepsLeftToGo = sliderMove->giveNumberOfStepsToMoveNRows(address - this->sliderAddress);
        this->sliderAddress = address;
        
        return stepsLeftToGo == 0;
    }

    //This function runs a single cycle of all the tasks it has to do:
    void runCycle(){
        // uint32_t max_time_us_for_cycle = 1000;

        move2Target(20000);
        // ledInterfaceHandler->displayWithTimeLimit(400);

    }

};