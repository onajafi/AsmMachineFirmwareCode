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
    virtualInstMem->setInstruction(1, 8);
    virtualInstMem->setInstruction(2, 0b00000000);
    virtualInstMem->setInstruction(3, 0b00000000);
    virtualInstMem->setInstruction(4, 0b01000100);
    virtualInstMem->setInstruction(5, 2);

    //Simulate the cpu8008
    SingletonLogger::getInstance().setLevel(DebugLevel::DEBUG);
    for(int i=0; i<50; i++){
        byte inst;
        inst = virtualInstMem->getInstruction(cpu8008->getPC());
        
        std::cout << "[" << cpu8008->getPC() << "]:\t" << std::bitset<8>(inst) << std::endl;
        
        cpu8008->processInstruction(inst);
        std::cout << "Ra:\t" << (int) cpu8008->getReg(0) << std::endl;
        // std::cout << "Next PC:\t" << cpu8008->getPC() << std::endl;

        //wait for 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
    }

    return 0;
}
