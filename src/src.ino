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
  // concurrentMotorAndDisplay = new ConcurrentMotorAndDisplay(sliderMove, irReader, ledInterfaceHandler);
}


void loop() {
  SingletonLogger& logger = SingletonLogger::getInstance();
  
  //Clear the rail
  // sliderMove->runStepperByLength(250, HIGH, 3);
  // delay(500);
  // //Get to the first row
  // sliderMove->runStepperByLength(12, HIGH, 1);
  // delay(100);

  ledInterfaceHandler->setSevenSegment(0,0);
  ledInterfaceHandler->setSevenSegment(1,1);
  ledInterfaceHandler->setSevenSegment(2,2);
  ledInterfaceHandler->setSevenSegment(3,3);
  ledInterfaceHandler->setSevenSegment(4,4);
  ledInterfaceHandler->setSevenSegment(5,5);
  ledInterfaceHandler->setSevenSegment(6,6);
  ledInterfaceHandler->setSevenSegment(7,7);
  for(int i=0; i<8; i++){
    ledInterfaceHandler->setMatrix8x8(i, 0b00000000);
  }
  ledInterfaceHandler->setMatrix8x8(0, 0b00011000);
  ledInterfaceHandler->setMatrix8x8(1, 0b00100100);
  ledInterfaceHandler->setMatrix8x8(2, 0b01000010);
  ledInterfaceHandler->setMatrix8x8(3, 0b10000001);
  ledInterfaceHandler->setMatrix8x8(4, 0b10000001);
  ledInterfaceHandler->setMatrix8x8(5, 0b01000010);
  ledInterfaceHandler->setMatrix8x8(6, 0b00100100);
  ledInterfaceHandler->setMatrix8x8(7, 0b00011000);

  // ledInterfaceHandler->turnOff();

  // while(1){

  //   ledInterfaceHandler->displayWithTimeLimit(200000);
  //   // digitalWrite(2, LOW);
  //   // ledInterfaceHandler->displayWithTimeLimit(200000);
  //   // digitalWrite(2, HIGH);
  //   // delay(100);
  // }

  // while(1){
  //   while(!concurrentMotorAndDisplay->setTargetAddress(20)){
  //     concurrentMotorAndDisplay->runCycle();
  //   }
  //   while(!concurrentMotorAndDisplay->setTargetAddress(1)){
  //     concurrentMotorAndDisplay->runCycle();
  //   }
  // }
  

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
  // delay(10000);
  //Step2 - We are a little behind the 0xAA row. Lets get back 
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
  // delay(10000);
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
  // delay(10000);
  //Step4 - Get to the middle of the row
  Serial.println("Calibrating Step3 - Get to the middle of the row");
  sliderMove->runStepperByLength(length/2 + 1, LOW, 0.5);
  // Now we are ready to compute. our current index is 0
  Serial.println("Finished Step4");
  // delay(200);
  ledInterfaceHandler->displayWithTimeLimit(200);


  //Simulate the instruction memory
  // byte* inst_list = [
  //   0b00101110,
  //   0,
  //   0b11111110
  //   ];
  virtualInstMem->setInstruction(0, 0b00101110);
  virtualInstMem->setInstruction(1, 0);//W = 0
  virtualInstMem->setInstruction(2, 0b00111110);//
  virtualInstMem->setInstruction(3, 0b00000001);//Mem = 1(Imm)
  virtualInstMem->setInstruction(4, 0b00101000);//W++
  virtualInstMem->setInstruction(5, 0b00111110);//
  virtualInstMem->setInstruction(6, 0b00000011);//Mem = 3(Imm)
  virtualInstMem->setInstruction(7, 0b00101000);//W++
  virtualInstMem->setInstruction(8, 0b00111110);//
  virtualInstMem->setInstruction(9, 0b00000111);//Mem = 7(Imm)
  virtualInstMem->setInstruction(10, 0b00101000);//W++
  virtualInstMem->setInstruction(11, 0b00111110);//
  virtualInstMem->setInstruction(12, 0b00001111);//Mem = 15(Imm)
  virtualInstMem->setInstruction(13, 0b00101000);//W++
  virtualInstMem->setInstruction(14, 0b00111110);//
  virtualInstMem->setInstruction(15, 0b00011111);//Mem = 31(Imm)
  virtualInstMem->setInstruction(16, 0b00101000);//W++
  virtualInstMem->setInstruction(17, 0b00111110);//
  virtualInstMem->setInstruction(18, 0b00111111);//Mem = 63(Imm)
  virtualInstMem->setInstruction(19, 0b00101000);//W++
  virtualInstMem->setInstruction(20, 0b00111110);//
  virtualInstMem->setInstruction(21, 0b01111111);//Mem = 127(Imm)
  virtualInstMem->setInstruction(22, 0b00101000);//W++
  virtualInstMem->setInstruction(23, 0b00111110);//
  virtualInstMem->setInstruction(24, 0b11111111);//Mem = 127(Imm)
  virtualInstMem->setInstruction(25, 0b00101000);//W++

  virtualInstMem->setInstruction(26, 0b00000000);
  virtualInstMem->setInstruction(27, 2);//JMP 2

  //Simulate the cpu8008
  Serial.println("Starting the CPU emulation");
  int pc = 0;
  for(int i=0; true; i++){
    logger.log(DebugLevel::DEBUG, "Begin cycle process");
    Serial.print("pc: ");
    Serial.println(pc);
    byte inst ;
    inst = instMem->getInstruction(pc);
    // ledInterfaceHandler->displayWithTimeLimit(500000);
    // Serial.print("Current Inst:\t");
    // printBinary(inst);
    Serial.println("S2-getting the virtual inst");
    // ledInterfaceHandler->displayWithTimeLimit(300000);
    inst = virtualInstMem->getInstruction(pc);

    Serial.print("Current Inst:\t");
    printBinary(inst);

    Serial.println("S3-process");
    // ledInterfaceHandler->displayWithTimeLimit(300000);
    pc = cpu8008->processInstruction(inst);
    delay(10);

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
  Serial.println("Finished the CPU emulation");


//  runStepperByLength(80, HIGH, 500);
  // delay(1000); // One second delay
  ledInterfaceHandler->displayWithTimeLimit(100000000);//This works as a delay
}

void printBinary(unsigned char _data){
  
  for(int i=7; i>=0; i--){
    Serial.print((_data & (0x01 << i)) ? "1":"0");
  }
  Serial.print("\n");
}

int readSerialNumber(){
  int output = 0;
  while(Serial.available() > 0){
    int num_char = Serial.read();
    if('0' <= num_char && num_char <='9'){
      output = output*10 + (num_char - '0');
    }
  }
  return output; 
}


void go2Row(int idx){
  static int current_row = 1;

  sliderMove->stepNRows(idx - current_row, 1.5);
  current_row = idx;

}
