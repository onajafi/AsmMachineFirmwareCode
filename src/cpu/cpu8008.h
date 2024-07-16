
#include "../peripheral/singletonLogger.h"

typedef unsigned char byte;

enum InstState{
    NONE,
    JMP_IMM,
    MVI_IMM,

};

class Cpu8008{
    /**Here every thing is simplified as much as possible (e.g. 16-bit address space is 
    *  turned to an 8-bit (byte) address space)
    */

    //The Registers
    byte A,B,C,D,E,W;
    byte PC;//Program Counter

    //The Data Memory
    byte memory[256];

    /**Instructions that are more than one byte require multiple times of reading until it
     * is fully read. We need to preserve the state of the machine to know what to do next;
     * Whether to read the next byte as the continuation of the instruction or as a new one.
     */
    enum InstState state;
    //Last instructions that are used for multi-byte instructions
    byte lastInst[2];

    //Logger
    SingletonLogger& logger = SingletonLogger::getInstance();


    //This function simplifies the process of accessing the registers and memory
    byte& getRegOrMem(uint8_t add){
        switch (add){
        case 0:
            return A;
            break;
        case 1:
            return B;
            break;
        case 2:
            return C;
            break;
        case 3:
            return D;
            break;
        case 4:
            return E;
            break;
        case 5:
            return W;
            break;
        case 7:
            return memory[W];//Read from memory
            break;
        default:
            // Handle error or return default value
            break;
        }
    }

public:
    Cpu8008(){
        A=0;//0
        B=0;//1
        C=0;//2
        D=0;//3
        E=0;//4
        W=0;//5(Technically this replaces the L & W in the 8008 architecture)
        PC=0;
        state = NONE;

        //Initialize data memory with zeros
        for(int i=0; i<256; i++){
            memory[i] = 0;
        }
    }

    int getPC() const{
        return PC;
    }

    uint8_t getReg(uint8_t add){
        return getRegOrMem(add);
    }

    uint8_t getMemory(byte address){
        return memory[address];
    }

    byte processInstruction(byte instruction){
        byte next_PC = PC + 1;
        if(state != NONE){
            switch(state){
                case JMP_IMM:
                    next_PC = instruction;
                    state = NONE;
                    break;
                case MVI_IMM:
                    getRegOrMem(lastInst[0] & 0b00111000) = instruction;
                    uint8_t Rd = (instruction & 0b00111000) >> 3;
                    getRegOrMem(Rd) = instruction;//The second byte in the MVI instruction is the Immediate value
                    state = NONE;
                    break;
            }
        }else if((instruction & 0b11000000) == 0b11000000){ // MOV
            uint8_t Rd = (instruction & 0b00111000) >> 3;
            uint8_t Rs = (instruction & 0b00000111);
            getRegOrMem(Rd) = getRegOrMem(Rs);
        }else if((instruction & 0b11000111) == 0b00000110){ // MVI (2-bytes)
            // logger.log("MVI instruction detected\n");
            lastInst[0] = instruction;
            state = MVI_IMM;
        }else if((instruction & 0b11000111) == 0b00000000){ // INR 
            uint8_t Rd = (instruction & 0b00111000) >> 3;
            getRegOrMem(Rd) +=1;
        }else if((instruction & 0b11000111) == 0b00000001){ // DCR 
            uint8_t Rd = (instruction & 0b00111000) >> 3;
            getRegOrMem(Rd) -=1;
        }else{
            switch(instruction){
                case 0b00000000:
                case 0b11111111:
                    //HLT
                    state = NONE;
                    break;
                case 0b01000100:
                    //JMP Imm
                    state = JMP_IMM;
                    break;
                
            }
        }

        this->PC = next_PC;
        return next_PC;
    }
            

};