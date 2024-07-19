class SliderMove {
private:
    int stepPin;
    int dirPin;
    int pulseWidthUs = 1000;
    float stepsPerMm = 27.5;
    float distanceMmBetweenRows = 8;

public:
    void runStepper(unsigned long steps, bool dir, double speed = 1.0){
        unsigned long tmpPulseWidthUs = this->pulseWidthUs / speed;
        digitalWrite(dirPin, dir);
        for(int x = 0; x < steps; x++) {
            digitalWrite(stepPin,HIGH); 
            delayMicroseconds(tmpPulseWidthUs); 
            digitalWrite(stepPin,LOW); 
            delayMicroseconds(tmpPulseWidthUs); 
        }
    }

public:
    SliderMove(int stepPin, int dirPin) {
        this->stepPin = stepPin;
        this->dirPin = dirPin;

        pinMode(stepPin, OUTPUT);
        pinMode(dirPin, OUTPUT);
    }

    void setPulseWidth(int pulseWidthUs) {
        this->pulseWidthUs = pulseWidthUs;
    }

    void moveSlider(int position) {
        // Code to move the slider to the specified position
    }

    void runStepperByLength(unsigned long len_mm, bool dir, double speed = 1.0){
        unsigned long steps = len_mm * stepsPerMm;
        this->runStepper(steps, dir, speed);
    }

    void runStepperByDuration(unsigned long duration_us, bool dir, double speed = 1.0){
        unsigned long tmpPulseWidthUs = this->pulseWidthUs / speed;
        unsigned long steps = duration_us / (tmpPulseWidthUs * 2);
        this->runStepper(steps, dir, speed);
    }

    uint32_t stepperForDuration(uint32_t duration_us, double speed = 1.0){
        unsigned long tmpPulseWidthUs = this->pulseWidthUs / speed;
        unsigned long steps = duration_us / (tmpPulseWidthUs * 2);
        return steps;
    }

    void stepNRows(int n, double speed = 1.0){
        bool dir;
        if(n>0){
            dir = HIGH;
        }else{
            dir = LOW;
            n *= -1;
        }
        runStepperByLength(n * distanceMmBetweenRows, dir, speed);
    }

    int giveNumberOfStepsToMoveNRows(int n){
        return n * distanceMmBetweenRows * stepsPerMm;
    }
};
