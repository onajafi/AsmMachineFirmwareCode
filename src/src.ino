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

// defines pins numbers
const int stepPin = 3; 
const int dirPin = 4; 
int inputPinList[8] = {5, 6, 7, 8, A1, A2, A3, A4};

SliderMove* sliderMove;
Cpu8008* cpu8008;
InstructionMemory* instMem;
InstructionMemory* virtualInstMem;
IrReader* irReader;
LedInterfaceHandler* ledInterfaceHandler;

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
  ledInterfaceHandler = new LedInterfaceHandler(A5, A0, 12, 9, 11, 10);
  
}


void loop() {
  
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
  ledInterfaceHandler->setMatrix8x8(0, 0b00000001);
  ledInterfaceHandler->setMatrix8x8(1, 0b00000011);
  ledInterfaceHandler->setMatrix8x8(2, 0b00000111);
  ledInterfaceHandler->setMatrix8x8(3, 0b00001111);
  ledInterfaceHandler->setMatrix8x8(4, 0b00011111);
  ledInterfaceHandler->setMatrix8x8(5, 0b00111111);
  ledInterfaceHandler->setMatrix8x8(6, 0b01111111);
  ledInterfaceHandler->setMatrix8x8(7, 0b11111111);

  ledInterfaceHandler->turnOff();
  

  while(true){
    
    Serial.println("Waiting for the first instruction");
    // delay(1000);
    //Run as long as the first non-0xff is found
    sliderMove->runStepperByDuration(50000, HIGH, 1.0);
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
  delay(200);


  //Simulate the instruction memory
  virtualInstMem->setInstruction(0x00, 0b01000100);
  virtualInstMem->setInstruction(0x01, 0x04);
  virtualInstMem->setInstruction(0x02, 0b01000100);
  virtualInstMem->setInstruction(0x03, 0x00);
  virtualInstMem->setInstruction(0x04, 0b01000100);
  virtualInstMem->setInstruction(0x05, 0x02);

  //Simulate the cpu8008
  Serial.println("Starting the CPU emulation");
  int pc = 0;
  for(int i=0; i<50; i++){
    byte inst;
    // inst = instMem->getInstruction(pc);
    // Serial.print("Current Inst:\t");
    // printBinary(inst);
    inst = virtualInstMem->getInstruction(pc);
    
    pc = cpu8008->processInstruction(inst);
    // Serial.print("Next PC:\t");
    // Serial.println(pc, DEC);
    ledInterfaceHandler->displayAll(610);
  }
  Serial.println("Finished the CPU emulation");

  // while(1){
  //   if (Serial.available() > 0) {
  //     delay(3);//Wait for all the input to come (3ms)
      
  //     int new_idx = readSerialNumber();
  //     go2Row(new_idx);
  //   }
  //   delay(500);
  //   printBinary(readInstruct());
  // }

//  runStepperByLength(80, HIGH, 500);
  delay(1000); // One second delay
  
}

void printBinary(unsigned char _data){
  
  for(int i=7; i>=0; i--){
    Serial.write(_data & (0x01 << i) ? '1':'0');
  }
  Serial.write('\n');
}

unsigned char readInstruct(){
  unsigned char output = 0x00;

  for(int i=0; i<8; i++){
    output = output << 1;
    output |= digitalRead(inputPinList[i]) ? 0x00 : 0x01;
  }
  
  return output;
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
