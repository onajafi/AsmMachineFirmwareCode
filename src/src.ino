/*     Simple Stepper Motor Control Exaple Code
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
 // Test on arduino Nano (w.ith old bootloader)

#include "./peripheral/sliderMove.h"
#include "./cpu/cpu8008.h"
#include "./peripheral/physicalInstructionMemory.h"
#include "./peripheral/virtualInstructionMemory.h"
#include "./peripheral/irReader.h"
#include "./peripheral/singletonLogger.h"
#include "./peripheral/ledInterfaceHandler.h"
#include "./peripheral/concurrentMotorAndDisplay.h"

// defines pins numbers
const int stepPin = 3; 
const int dirPin = 4; 
int inputPinList[8] = {5, 6, 7, 8, A1, A2, A3, A4};

SliderMove* sliderMove;
Cpu8008* cpu8008;
PhysicalInstructionMemory* instMem;
VirtualInstructionMemory* virtualInstMem;
IrReader* irReader;
LedInterfaceHandler* ledInterfaceHandler;
ConcurrentMotorAndDisplay* concurrentMotorAndDisplay;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting the ASM machine");
  
  sliderMove = new SliderMove(stepPin, dirPin);

  Serial.println("Setting up the CPU");
  cpu8008 = new Cpu8008();

  Serial.println("Setting up the IR Reader");
  irReader = new IrReader();
  for(int i=0; i<8; i++){
    irReader->setInputPin(inputPinList[i], i);
  }

  Serial.println("Setting up the memory");
  instMem = new PhysicalInstructionMemory(sliderMove, irReader);
  Serial.println("Setting up the virtual memory");
  virtualInstMem = new VirtualInstructionMemory();

  Serial.println("Setting up the led interface");
  ledInterfaceHandler = LedInterfaceHandlerSingleton::getInstance();
  
  SingletonLogger::getInstance().setLevel(DebugLevel::DEBUG);
  // SingletonLogger::getInstance().setLevel(DebugLevel::INFO);
  // concurrentMotorAndDisplay = new ConcurrentMotorAndDisplay(sliderMove, irReader, ledInterfaceHandler);
}

void waitUntilPunchCardInsertion(){


  //Step0 - Run motor until a 0xAA is saw
  while(true){
    static int count_print = 0;
    if (count_print++ % 10 == 0)
      Serial.println("Waiting for the first instruction");
    // delay(1000);
    //Run as long as the first non-0xff is found
    sliderMove->runStepperByDuration(50000, HIGH, 2.0);
    if(irReader->read() == 0xAA){
      break;
    }
  }
  // delay(5000);
  //Calibrate the with the first 0xAA row
  Serial.println("Starting the calibration process.");

  //Step1 - Get to the first 0xAA row
  while(true){
    Serial.println("Calibrating Step1 - Calibrating the first row");
    //Run as long as the first non-0xff is found
    sliderMove->runStepperByLength(1, LOW, 0.5);
    if(irReader->read() != 0xAA){
      break;
    }
  }
  Serial.println("Finished Step1");

  //Step2 - We are a little behind from the 0xAA row. Lets get back 
  // at the beginning of it.
  while(true){
    Serial.println("Calibrating Step2 - Finding the beginning of 0xAA");
    //Run as long as the first non-0xff is found
    sliderMove->runStepperByLength(1, HIGH, 0.5);
    if(irReader->read() == 0xAA){
      break;
    }
  }
  Serial.println("Finished Step2");
  
  //Step3 - Get to the end of the 0xAA row one millimetre 
  // at a time until we don't read any 0xAA
  int length = 0;
  while(true){
    Serial.println("Calibrating Step3 - Trying to findout the row length");
    //Run as long as the first non-0xff is found
    sliderMove->runStepperByLength(1, HIGH, 0.5);
    if(irReader->read() != 0xAA){
      break;
    }
    length++;
  }
  Serial.println("Finished Step3");
  Serial.print("Length of the first row is: ");
  Serial.print(length, DEC);
  Serial.println("mm");
  // sliderMove->setDistanceMmBetweenRows(8);
  sliderMove->setDistanceMmBetweenRows(6);
  
  //Step4 - Get to the middle of the row
  Serial.println("Calibrating Step3 - Get to the middle of the row");
  sliderMove->runStepperByLength(length/2, LOW, 0.5);
  sliderMove->runStepperByLength(length, HIGH, 0.5); // The index-0 is the row after 0xAA. So we will go one step ahead
  // Now we are ready to compute. our current index is 0
  Serial.println("Finished Step4");
}
bool first_loop = true;
void loop() {
  SingletonLogger& logger = SingletonLogger::getInstance();

  if (first_loop){
    ledInterfaceHandler->setSevenSegment(0,0);
    ledInterfaceHandler->setSevenSegment(1,0);
    ledInterfaceHandler->setSevenSegment(2,0);
    ledInterfaceHandler->setSevenSegment(3,0);
    ledInterfaceHandler->setSevenSegment(4,0);
    ledInterfaceHandler->setSevenSegment(5,0);
    ledInterfaceHandler->setSevenSegment(6,0);
    ledInterfaceHandler->setSevenSegment(7,0);
    for(int i=0; i<8; i++){
      ledInterfaceHandler->setMatrix8x8(i, 0b00000000);
    }
    // ledInterfaceHandler->setMatrix8x8(0, 0b00011000);
    // ledInterfaceHandler->setMatrix8x8(1, 0b00100100);
    // ledInterfaceHandler->setMatrix8x8(2, 0b01000010);
    // ledInterfaceHandler->setMatrix8x8(3, 0b10000001);
    // ledInterfaceHandler->setMatrix8x8(4, 0b10000001);
    // ledInterfaceHandler->setMatrix8x8(5, 0b01000010);
    // ledInterfaceHandler->setMatrix8x8(6, 0b00100100);
    // ledInterfaceHandler->setMatrix8x8(7, 0b00011000);

    // ledInterfaceHandler->turnOff();


  }

  waitUntilPunchCardInsertion();
  ledInterfaceHandler->displayWithTimeLimit(200);

  // //Simulate the instruction memory
  // virtualInstMem->setInstruction(0, 0b00101110);
  // virtualInstMem->setInstruction(1, 0);//W = 0
  // virtualInstMem->setInstruction(2, 0b00000110);
  // virtualInstMem->setInstruction(3, 0b00000001);//A = 1
  // virtualInstMem->setInstruction(4, 0b11111000);//Mem = W
  // virtualInstMem->setInstruction(5, 0b00111100);
  // virtualInstMem->setInstruction(6, 0b11111111);//CMP (if A==255)
  // virtualInstMem->setInstruction(7, 0b00000010);//A << 1
  // virtualInstMem->setInstruction(8, 0b00110100);
  // virtualInstMem->setInstruction(9, 0b00000001);//A = A | 1
  // virtualInstMem->setInstruction(10, 0b00101000);//W++
  // virtualInstMem->setInstruction(11, 0b01001000);
  // virtualInstMem->setInstruction(12, 0b00000100);//JNZ (jump to 4 if A!=0)
  // virtualInstMem->setInstruction(13, 0b00000000);//HLT

  //Simulate the cpu8008
  Serial.println("Starting the CPU emulation1");
  //Resetting the registers
  cpu8008->resetRegisters();
  instMem->resetIndex();
  int pc = 0;
  for(int i=0; true; i++){
    logger.log(DebugLevel::DEBUG, "Begin cycle process");
    // Serial.print("pc: ");
    // Serial.println(pc);
    byte inst ;
    inst = instMem->getInstruction(pc);
    // ledInterfaceHandler->displayWithTimeLimit(1000000);
    // Serial.print("Current Inst:\t");
    // printBinary(inst);
    // Serial.println("S2-getting the virtual inst");
    // ledInterfaceHandler->displayWithTimeLimit(300000);
    // inst = virtualInstMem->getInstruction(pc);

    // Serial.print("Current Inst:\t");
    // printBinary(inst);

    // Serial.println("S3-process");
    // ledInterfaceHandler->displayWithTimeLimit(300000);
    pc = cpu8008->processInstruction(inst);
    // delay(10);

    ledInterfaceHandler->setSevenSegment(0, cpu8008->getPC()/10%10);
    ledInterfaceHandler->setSevenSegment(1, cpu8008->getPC()%10);

    ledInterfaceHandler->setSevenSegment(2, cpu8008->getReg(0)/10%10);
    ledInterfaceHandler->setSevenSegment(3, cpu8008->getReg(0)%10);

    ledInterfaceHandler->setSevenSegment(4, cpu8008->getReg(1)/10%10);
    ledInterfaceHandler->setSevenSegment(5, cpu8008->getReg(1)%10);

    ledInterfaceHandler->setSevenSegment(4, cpu8008->getReg(5)/10%10);
    ledInterfaceHandler->setSevenSegment(5, cpu8008->getReg(5)%10);
    
    for(uint8_t j=0; j<8; j++){
      ledInterfaceHandler->setMatrix8x8(j, cpu8008->getMemory(j));
    }
    ledInterfaceHandler->displayAll();
    if (cpu8008->isHalted()){
      break;
    }
  }
  logger.log(DebugLevel::INFO, "Finished the CPU emulation");


  ledInterfaceHandler->displayWithTimeLimit(1000000);//This works as a delay
  first_loop = false;
}

void printBinary(unsigned char _data){
  
  for(int i=7; i>=0; i--){
    Serial.print((_data & (0x01 << i)) ? "1":"0");
  }
  Serial.print("\n");
}