
#include "../peripheral/singletonLogger.h"

#define MEMORY_SIZE 64

typedef uint8_t byte;

enum InstState{
    NONE,

    MVI_IMM,
    ADI_IMM,
    SUI_IMM,
    ANI_IMM,
    XRI_IMM,
    ORI_IMM,
    CPI_IMM,

    JMP_IMM,
};

class Cpu8008{
    /**Here every thing is simplified as much as possible (e.g. 16-bit address space is 
    *  turned to an 8-bit (byte) address space)
    */

    //The Registers
    byte A,B,C,D,E,W;
    byte PC;//Program Counter

    //Flags
    bool sign;
    bool zero;
    bool parity;
    bool carry;

    bool halted; // This is not part of the 8008 architecture it just tells if the program is finished or not

    //The Data Memory (The Data storage is way more bigger than this in the actual 8008)
    byte memory[MEMORY_SIZE];

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
            logger.log(DebugLevel::ERROR, "Invalid Register or Memory Address: %d", add);
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

        sign = 0;
        zero = 0;
        parity = 0;//TODO:: Implement this
        carry = 0;//TODO:: Implement this

        //Initialize data memory with zeros
        for(int i=0; i<MEMORY_SIZE; i++){
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

    bool isHalted(){
        return halted;
    }

    // Process the instruction and return the next PC
    // Ref #0: https://en.wikichip.org/wiki/intel/mcs-8/isa#pc_and_stack
    // Ref #1: https://petsd.net/8008.php
    // Ref #2: https://en.wikipedia.org/wiki/Intel_8008
    byte processInstruction(byte instruction){
        byte next_PC = PC + 1;
        
        if(state != NONE){
            uint8_t Rd;
            switch(state){
                case MVI_IMM:
                    logger.log(DebugLevel::DEBUG, "MVI (2/2)");
                    getRegOrMem(lastInst[0] & 0b00111000) = instruction;
                    Rd = (instruction & 0b00111000) >> 3;
                    getRegOrMem(Rd) = instruction;//The second byte in the instruction is the Immediate value
                    state = NONE;
                    break;
                case ADI_IMM:
                    logger.log(DebugLevel::DEBUG, "ADI (2/2)");
                    //Set the carry flag:
                    if(((uint16_t) A + (uint16_t) instruction) > 255){
                        carry = 1;
                    }
                    A += instruction;//The second byte in the instruction is the Immediate value
                    state = NONE;
                    break;
                case SUI_IMM:
                    logger.log(DebugLevel::DEBUG, "SUI (2/2)");
                    //Set the carry flag:
                    if(A < instruction){
                        carry = 1;
                    }
                    A -= instruction;//The second byte in the instruction is the Immediate value
                    state = NONE;
                    break;
                case ANI_IMM:
                    logger.log(DebugLevel::DEBUG, "ANI (2/2)");
                    A &= instruction;//The second byte in the instruction is the Immediate value
                    state = NONE;
                    break;
                case XRI_IMM:
                    logger.log(DebugLevel::DEBUG, "XRI (2/2)");
                    A ^= instruction;//The second byte in the instruction is the Immediate value
                    state = NONE;
                    break;
                case ORI_IMM:
                    logger.log(DebugLevel::DEBUG, "ORI (2/2)");
                    A |= instruction;//The second byte in the instruction is the Immediate value
                    state = NONE;
                    break;
                case CPI_IMM:
                    logger.log(DebugLevel::DEBUG, "CPI (2/2)");
                    //The second byte in the instruction is the Immediate value
                    sign = (A < instruction); //A - Rs < 0
                    zero = (A == instruction); //A - Rs = 0
                    state = NONE;
                    break;
                
                case JMP_IMM:
                    logger.log(DebugLevel::DEBUG, "JMP IMM (2/2)");
                    next_PC = instruction;
                    state = NONE;
                    break;
            }
        }
        //Index Register Instructions
        else if((instruction & 0b11000000) == 0b11000000){ // MOV
            logger.log(DebugLevel::DEBUG, "MOV");
            uint8_t Rd = (instruction & 0b00111000) >> 3;
            uint8_t Rs = (instruction & 0b00000111);
            getRegOrMem(Rd) = getRegOrMem(Rs);
        }else if((instruction & 0b11000111) == 0b00000110){ // MVI (2-bytes)
            logger.log(DebugLevel::DEBUG, "MVI (1/2)");
            lastInst[0] = instruction;
            state = MVI_IMM;
        }else if((instruction & 0b11000111) == 0b00000000 && instruction != 0b00000000 /*Can't use Reg(A) for this*/){ // INR 
            logger.log(DebugLevel::DEBUG, "INR");
            uint8_t Rd = (instruction & 0b00111000) >> 3;
            //Set the carry flag:
            if(getRegOrMem(Rd) == 255){
                carry = 1;
            }
            getRegOrMem(Rd) +=1;
        }else if((instruction & 0b11000111) == 0b00000001 && instruction != 0b00000001 /*Can't use Reg(A) for this*/){ // DCR 
            logger.log(DebugLevel::DEBUG, "DCR");
            uint8_t Rd = (instruction & 0b00111000) >> 3;
            //Set the carry flag:
            if(getRegOrMem(Rd) == 0){
                carry = 1;
            }
            getRegOrMem(Rd) -=1;
        }
        //Accumulator Group Instructions
        else if((instruction & 0b11111000) == 0b10000000){ // ADD 
            logger.log(DebugLevel::DEBUG, "ADD");
            uint8_t Rs = (instruction & 0b00000111);
            //Set the carry flag:
            if(((uint16_t) A + (uint16_t) getRegOrMem(Rs)) > 255){
                carry = 1;
            }
            A += getRegOrMem(Rs);
        }else if((instruction & 0b11111111) == 0b00000100){ // ADI (2-bytes)
            logger.log(DebugLevel::DEBUG, "ADI (1/2)");
            lastInst[0] = instruction;
            state = ADI_IMM;
        }else if((instruction & 0b11111000) == 0b10010000){ // SUB 
            logger.log(DebugLevel::DEBUG, "SUB");
            uint8_t Rs = (instruction & 0b00000111);
            //Set the carry flag:
            if(A < getRegOrMem(Rs)){
                carry = 1;
            }
            A -= getRegOrMem(Rs);
        }else if((instruction & 0b11111111) == 0b00010100){ // SUI (2-bytes)
            logger.log(DebugLevel::DEBUG, "SUI (1/2)");
            lastInst[0] = instruction;
            state = SUI_IMM;
        }else if((instruction & 0b11111000) == 0b10100000){ // ANA 
            logger.log(DebugLevel::DEBUG, "ANA");
            uint8_t Rs = (instruction & 0b00000111);
            A &= getRegOrMem(Rs);
        }else if((instruction & 0b11111111) == 0b00100100){ // ANI (2-bytes)
            logger.log(DebugLevel::DEBUG, "ANI (1/2)");
            lastInst[0] = instruction;
            state = ANI_IMM;
        }else if((instruction & 0b11111000) == 0b10101000){ // XRA 
            logger.log(DebugLevel::DEBUG, "XRA");
            uint8_t Rs = (instruction & 0b00000111);
            A ^= getRegOrMem(Rs);
        }else if((instruction & 0b11111111) == 0b00101100){ // XRI (2-bytes)
            logger.log(DebugLevel::DEBUG, "XRI (1/2)");
            lastInst[0] = instruction;
            state = XRI_IMM;
        }else if((instruction & 0b11111000) == 0b10110000){ // ORA 
            logger.log(DebugLevel::DEBUG, "ORA");
            uint8_t Rs = (instruction & 0b00000111);
            A |= getRegOrMem(Rs);
        }else if((instruction & 0b11111111) == 0b00110100){ // ORI (2-bytes)
            logger.log(DebugLevel::DEBUG, "ORI (1/2)");
            lastInst[0] = instruction;
            state = ORI_IMM;
        }else if((instruction & 0b11111000) == 0b10111000){ // CMP 
            logger.log(DebugLevel::DEBUG, "CMP");
            uint8_t Rs = (instruction & 0b00000111);
            sign = A < getRegOrMem(Rs); //A - Rs < 0
            zero = A == getRegOrMem(Rs); //A - Rs = 0
        }else if((instruction & 0b11111111) == 0b00111100){ // CPI (2-bytes)
            logger.log(DebugLevel::DEBUG, "CPI (1/2)");
            lastInst[0] = instruction;
            state = CPI_IMM;
        }
        //Program Counter and Stack Control Instructions
        //The actual 8008 processor has a PC size of 2-bytes. This 
        // results in a 16-bit address space. Thus, the JMP instructions 
        // had to be 3-bytes long. However, for simplicity we used a 
        // single byte for the PC and the JMP instructions are only 2-bytes 
        // long.
        else if((instruction & 0b11000111) == 0b01000100){ // JMP (2-bytes)
            logger.log(DebugLevel::DEBUG, "JMP IMM (1/2)");
            state = JMP_IMM;
        }else if(instruction == 0b01000000){ // JNC (2-bytes)
            logger.log(DebugLevel::DEBUG, "JNC IMM (1/2)");
            if(!carry)
                state = JMP_IMM;
        }else if(instruction == 0b01001000){ // JNZ (2-bytes)
            logger.log(DebugLevel::DEBUG, "JNZ IMM (1/2)");
            if(!zero)
                state = JMP_IMM;
        }else if(instruction == 0b01010000){ // JP (2-bytes)
            logger.log(DebugLevel::DEBUG, "JP IMM (1/2)");
            if(!sign)
                state = JMP_IMM;
        }else if(instruction == 0b01011000){ // JPO (2-bytes)
            logger.log(DebugLevel::DEBUG, "JPO IMM (1/2)");
            if(!parity)
                state = JMP_IMM;
        }else if(instruction == 0b01100000){ // JC (2-bytes)
            logger.log(DebugLevel::DEBUG, "JC IMM (1/2)");
            if(carry)
                state = JMP_IMM;
        }else if(instruction == 0b01101000){ // JZ (2-bytes)
            logger.log(DebugLevel::DEBUG, "JZ IMM (1/2)");
            if(zero)
                state = JMP_IMM;
        }else if(instruction == 0b01110000){ // JM (2-bytes)
            logger.log(DebugLevel::DEBUG, "JM IMM (1/2)");
            if(sign)
                state = JMP_IMM;
        }else if(instruction == 0b01111000){ // JPE (2-bytes)
            logger.log(DebugLevel::DEBUG, "JPE IMM (1/2)");
            if(parity)
                state = JMP_IMM;
        }
        //Machine Instructions
        else{
            switch(instruction){
                case 0b00000000:
                case 0b11111111:
                    logger.log(DebugLevel::DEBUG, "HLT");//Halt
                    halted = true;
                    next_PC = this->PC; // No moving!!!
                    state = NONE;
                    break;
                default:
                    logger.log(DebugLevel::ERROR, "Unknown Instruction: %d", instruction);
                    break;
            }
        }

        this->PC = next_PC;
        return next_PC;
    }
            

};