#pragma once

class IrReader{
public:
    IrReader(){}

    void setInputPin(int irPin, int idx){
        this->inputPinList[idx] = irPin;
        pinMode(irPin, INPUT_PULLUP);
    }

    byte read(){
        unsigned char output = 0x00;

        for(int i=0; i<8; i++){
            output = output << 1;
            output |= digitalRead(this->inputPinList[i]) ? 0x00 : 0x01;
        }
        
        return output;
    }
private:
    int inputPinList[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
};