#pragma once


enum class DebugLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class SingletonLogger{
private:


    SingletonLogger(const SingletonLogger&) = delete;
    SingletonLogger& operator=(const SingletonLogger&) = delete;
    SingletonLogger(){};

    DebugLevel _level = DebugLevel::INFO;

public:
    static SingletonLogger _instance;
    static SingletonLogger& getInstance(){
        return SingletonLogger::_instance;
    }

    //Method to define the severity level of the logs to print
    void setLevel(DebugLevel level){
        _level = level;
    }


    template<typename... Args>
    void log(DebugLevel level, const char* format, Args... args){
        if(level < _level) 
            return;

        #ifdef ARDUINO
            char buffer[256];
            snprintf(buffer, sizeof(buffer), format, args...);
            Serial.print("[");
            switch(level) {
                case DebugLevel::DEBUG:
                    Serial.print("DEBUG");
                    break;
                case DebugLevel::INFO:
                    Serial.print("INFO");
                    break;
                case DebugLevel::WARNING:
                    Serial.print("WARNING");
                    break;
                case DebugLevel::ERROR:
                    Serial.print("ERROR");
                    break;
            }
            Serial.print("] ");
            Serial.println(buffer);
        #else
            if(level == DebugLevel::DEBUG)
                printf("[DEBUG] ");
            else if(level == DebugLevel::INFO)
                printf("[INFO] ");
            else if(level == DebugLevel::WARNING)
                printf("[WARNING] ");
            else if(level == DebugLevel::ERROR)
                printf("[ERROR] ");
            printf(format, args...);
            printf("\n");
        #endif
    }

    
};

SingletonLogger SingletonLogger::_instance;
