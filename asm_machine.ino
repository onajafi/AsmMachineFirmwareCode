/*     Simple Stepper Motor Control Exaple Code
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
 // Test on arduino Nano (w.ith old bootloader)

#include "./sliderMove.h"
#include "./cpu8008.h"
#include "./physicalInstructionMemory.h"
#include "./virtualInstructionMemory.h"
#include "./irReader.h"
#include "./singletonSerialComm.h"

// defines pins numbers
const int stepPin = 3; 
const int dirPin = 4; 
int inputPinList[8] = {6, 7, 8, 9, A1, A2, A3, A4};

SliderMove* sliderMove;
Cpu8008* cpu8008;
InstructionMemory* instMem;
InstructionMemory* virtualInstMem;
IrReader* irReader;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting the ASM machine");
  
  sliderMove = new SliderMove(stepPin, dirPin);

  cpu8008 = new Cpu8008();

  irReader = new IrReader();
  for(int i=0; i<8; i++){
    irReader->setInputPin(inputPinList[i], i);
  }

  instMem = new PhysicalInstructionMemory(sliderMove, irReader);
  virtualInstMem = new VirtualInstructionMemory();
  
}


void loop() {
  
  //Clear the rail
  // sliderMove->runStepperByLength(250, HIGH, 3);
  // delay(500);
  // //Get to the first row
  // sliderMove->runStepperByLength(12, HIGH, 1);
  // delay(100);

  while(true){
    
    Serial.println("Waiting for the first instruction");
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
  delay(20000);


  //Simulate the instruction memory
  virtualInstMem->setInstruction(0x00, 0b01000100);
  virtualInstMem->setInstruction(0x01, 0x04);
  virtualInstMem->setInstruction(0x02, 0b01000100);
  virtualInstMem->setInstruction(0x03, 0x00);
  virtualInstMem->setInstruction(0x04, 0b01000100);
  virtualInstMem->setInstruction(0x05, 0x02);
  //Simulate the cpu8008
  int pc = 0;
  for(int i=0; i<50; i++){
    byte inst;
    inst = instMem->getInstruction(pc);
    Serial.print("Current Inst:\t");
    printBinary(inst);
    inst = virtualInstMem->getInstruction(pc);
    
    pc = cpu8008->processInstruction(inst);
    Serial.print("Next PC:\t");
    Serial.println(pc, DEC);
    // delay(100);
  }

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
  delay(10000); // One second delay
  
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
