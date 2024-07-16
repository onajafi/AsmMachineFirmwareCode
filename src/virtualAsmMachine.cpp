#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
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
    virtualInstMem->setInstruction(0, 0b00000110);
    virtualInstMem->setInstruction(1, 1);//A = 1
    
    virtualInstMem->setInstruction(2, 0b00001000);//Inc B
    virtualInstMem->setInstruction(3, 0b11000001);//A = B

    virtualInstMem->setInstruction(4, 0b00111100);//CMP IMM
    virtualInstMem->setInstruction(5, 5);

    virtualInstMem->setInstruction(6, 0b01110000);//JMP is sign = 1 ( A is lower )
    virtualInstMem->setInstruction(7, 2);

    //Simulate the cpu8008
    SingletonLogger::getInstance().setLevel(DebugLevel::DEBUG);
    while(1){
        byte inst;
        inst = virtualInstMem->getInstruction(cpu8008->getPC());
        
        std::cout << "[" << cpu8008->getPC() << "]:\t" << std::bitset<8>(inst) << std::endl;
        
        cpu8008->processInstruction(inst);
        std::cout << "Ra:\t" << (int) cpu8008->getReg(0) << std::endl;
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
