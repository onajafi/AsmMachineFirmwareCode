#pragma once

class SingletonLogger{
private:


    SingletonLogger(const SingletonLogger&) = delete;
    SingletonLogger& operator=(const SingletonLogger&) = delete;
    SingletonLogger(){};

public:
    static SingletonLogger _instance;
    static SingletonLogger& getInstance(){
        return SingletonLogger::_instance;
    }

    template<typename... Args>
    void log(const char* format, Args... args){
        #ifdef ARDUINO
            char buffer[256];
            snprintf(buffer, sizeof(buffer), format, args...);
            Serial.println(buffer);
        #else
            printf(format, args...);
        #endif
    }

    
};

SingletonLogger SingletonLogger::_instance;
