
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

    //Simulate the instruction memory
    std::vector<uint8_t> instructions = {
        0b00101110,
        0b00000000, // W = 0
        0b00000110,
        0b00000001, // A = 1
        0b11111000, // Mem = W
        0b00000010, // A << 1
        0b00101000, // W++
        0b00111100, 
        0b00000000, // CMP (if A==0)
        0b01001000,
        0b00000100, // JNZ (jump to 4 if A!=0)
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
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
    }

    return 0;
}

#endif