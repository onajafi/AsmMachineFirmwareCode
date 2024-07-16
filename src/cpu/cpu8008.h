
typedef unsigned char byte;

enum InstState{
    NONE,
    JMP_IMM,

};

class Cpu8008{
    /**Here every thing is simplified as much as possible (e.g. 16-bit address space is 
    *  turned to an 8-bit (byte) address space)
    */

    //The Registers
    byte A,B,C,D,E,W;
    byte PC;//Program Counter

    /**Instructions that are more than one byte require multiple times of reading until it
     * is fully read. We need to preserve the state of the machine to know what to do next;
     * Whether to read the next byte as the continuation of the instruction or as a new one.
     */
    enum InstState state;

public:
    Cpu8008(){
        A=0;
        B=0;
        C=0;
        D=0;
        E=0;
        W=0;
        PC=0;
        state = NONE;
    }

    byte processInstruction(byte instruction){
        byte next_PC = PC + 1;
        if(state != NONE){
            switch(state){
                case JMP_IMM:
                    next_PC = instruction;
                    state = NONE;
                    break;
            }
        }

        if(instruction & 0b11000000 == 0b11000000){
            // MOV Rd, Rs
            // MOV Rd, MEM
            // MOV MEM, Rs
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