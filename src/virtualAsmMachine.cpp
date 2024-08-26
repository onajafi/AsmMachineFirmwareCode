

#ifndef ARDUINO

#include <iostream>
#include <memory>
#include <regex>
#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include "./cpu/cpu8008.h"
#include "./peripheral/virtualInstructionMemory.h"

// Cpu8008* cpu8008;
// InstructionMemory* instMem;
// InstructionMemory* virtualInstMem;




#include <bitset>

int main(int argc, char* argv[]) {
    // Call the test functions
    std::shared_ptr<Cpu8008> cpu8008 = std::make_shared<Cpu8008>();
    std::shared_ptr<VirtualInstructionMemory> virtualInstMem = std::make_shared<VirtualInstructionMemory>();

    //Check if the first argument is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }

    //Read the instructions from the file(s)
    for(int file_idx = 1; file_idx < argc; file_idx++){
        std::cout << "Reading file: " << argv[file_idx] << std::endl;
        std::ifstream file(argv[file_idx]);

        std::vector<uint8_t> instructions = {};
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                // Extract the binary part using regex
                std::regex binaryRegex("0b([01]+)");
                std::smatch match;
                if (std::regex_search(line, match, binaryRegex)) {
                    std::string binaryPart = match[1].str();
                    instructions.push_back(std::stoi(binaryPart, nullptr, 2));
                    // std::cout << binaryPart << "-" << std::stoi(binaryPart, nullptr, 2) << std::endl;
                }
            }
            file.close();
        }else{
            std::cerr << "Unable to open file" << std::endl;
            return 1;
        }

        // Set instructions to the virtual instruction memory
        for (size_t i = 0; i < instructions.size(); i++) {
            virtualInstMem->setInstruction(i, instructions[i]);
        }

        //Simulate the cpu8008
        cpu8008->resetRegisters();
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
            if(file_idx == argc-1){
                // Run the last one real slow (for debugging)
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            
        }
    }

    return 0;
}

#endif