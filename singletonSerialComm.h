#pragma once

class SingletonSerialComm{
private:


  SingletonSerialComm(const SingletonSerialComm&) = delete;
  SingletonSerialComm& operator=(const SingletonSerialComm&) = delete;

public:
    static SingletonSerialComm instance;
    static SingletonSerialComm& getInstance(){
        return SingletonSerialComm::instance;
    }

    setup(){
        Serial.begin(9600);
        
        // while (!Serial) {
        //     ; // wait for serial port to connect. Needed for native USB port only
        // }
    }

    
};