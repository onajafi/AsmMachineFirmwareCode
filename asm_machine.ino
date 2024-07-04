/*     Simple Stepper Motor Control Exaple Code
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
 // Test on arduino Nano (w.ith old bootloader)

#include "./sliderMove.h"
#include "./cpu8008.h"

// defines pins numbers
const int stepPin = 3; 
const int dirPin = 4; 
int inputPinList[8] = {6, 7, 8, 9, A1, A2, A3, A4};

SliderMove* sliderMove;
Cpu8008* cpu8008;

void setup() {
  // Sets the two pins as Outputs
  sliderMove = new SliderMove(stepPin, dirPin);

  for(int i=0; i<8; i++){
    pinMode(inputPinList[i], INPUT_PULLUP);
  }
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

static int delay_process = 0;
void loop() {
  //Clear the rail
  sliderMove->runStepperBylength(250, HIGH, 3);
  delay(1000);
  //Get to the first row
  sliderMove->runStepperBylength(12, HIGH, 1);
  delay(1000);


  while(1){
    if (Serial.available() > 0) {
      delay(3);//Wait for all the input to come (3ms)
      
      int new_idx = readSerialNumber();
      go2Row(new_idx);
    }
    delay(500);
    printBinary(readInstruct());
  }

//  runStepperBylength(80, HIGH, 500);
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
