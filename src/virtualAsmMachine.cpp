

#ifndef ARDUINO

#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <vector>
#include "./cpu/cpu8008.h"
#include "./peripheral/virtualInstructionMemory.h"

// Cpu8008* cpu8008;
// InstructionMemory* instMem;
// InstructionMemory* virtualInstMem;




#include <bitset>

int main() {
    // Call the test functions
    std::shared_ptr<Cpu8008> cpu8008 = std::make_shared<Cpu8008>();
    std::shared_ptr<VirtualInstructionMemory> virtualInstMem = std::make_shared<VirtualInstructionMemory>();

    //Initialize the data memory
    cpu8008->setMemory(0, 63);
    cpu8008->setMemory(1, 7);
    cpu8008->setMemory(2, 127);
    cpu8008->setMemory(3, 31);
    cpu8008->setMemory(4, 255);
    cpu8008->setMemory(5, 15);
    cpu8008->setMemory(6, 1);
    cpu8008->setMemory(7, 3);

    //Simulate the instruction memory
    std::vector<uint8_t> instructions = {
        
        0b00010110,
        0b00000111, // C = 7
        0b00101110,
        0b00000000, // W = 0
        0b11000111, // A = Mem
        0b00101000, // INR W
        0b10111111, // CMP Mem
        0b01110000, // JMP if A >= Mem
        0b00001110, //  to 14
        0b11001111, // B = Mem
        0b11111000, // Mem = A
        0b00101001, // DCR W
        0b11111001, // Mem = B
        0b00101000, // INR W
        0b11000101, // A = W
        0b10111010, // CMP compare A with C
        0b01001000, // JNZ
        0b00000100, //  to 4
        0b00010001, // DCR C
        0b11000010, // A = C
        0b00111100, // CPI compare A
        0b00000000, //  with 0
        0b01001000, // JNZ
        0b00000010, //  to 2
        0b00000000, // HLT
    };

    // Set instructions to the virtual instruction memory
    for (size_t i = 0; i < instructions.size(); i++) {
        virtualInstMem->setInstruction(i, instructions[i]);
    }

    //Simulate the cpu8008
    SingletonLogger::getInstance().setLevel(DebugLevel::DEBUG);
    while(1){
        byte inst;
        inst = virtualInstMem->getInstruction(cpu8008->getPC());
        
        std::cout << "[" << cpu8008->getPC() << "]:\t" << std::bitset<8>(inst) << std::endl;
        
        cpu8008->processInstruction(inst);
        std::cout << "Ra:\t" << (int) cpu8008->getReg(0) << std::endl;
        std::cout << "Rb:\t" << (int) cpu8008->getReg(1) << std::endl;
        std::cout << "Rc:\t" << (int) cpu8008->getReg(2) << std::endl;
        std::cout << "Rw:\t" << (int) cpu8008->getReg(5) << std::endl;
        //Print a chunck of memory (index 0 to 7)
        for(uint8_t j=0; j<8; j++){
            std::cout << "M[" << (int) j << "]:\t" << (int) cpu8008->getMemory(j) << std::endl;
        }
        // std::cout << "Next PC:\t" << cpu8008->getPC() << std::endl;

        if(cpu8008->isHalted()){
            std::cout << "Finished" << std::endl;
            break;
        }

        //wait for 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
    }

    return 0;
}

#endif