
#ifdef ARDUINO

class ShiftRegister{
  //Pin connected to latch pin (ST_CP) of 74HC595
  int latchPin;
  //Pin connected to clock pin (SH_CP) of 74HC595
  int clockPin;
  //Pin connected to Data in (DS) of 74HC595
  int dataPin;
public:
  ShiftRegister(int latchPin, int clockPin, int dataPin){
    this->latchPin = latchPin;
    this->clockPin = clockPin;
    this->dataPin = dataPin;
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
  }

  void write(byte _data){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, _data);
    digitalWrite(latchPin, HIGH);
  }
  
  void writeX2(uint16_t _data){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, _data /256 %256);
    shiftOut(dataPin, clockPin, MSBFIRST, _data % 256);
    digitalWrite(latchPin, HIGH);
  }
};

class DigitOn7Seg{
  ShiftRegister* sevenSegRegister;
public:
  DigitOn7Seg(ShiftRegister* sevenSegRegister){
    this->sevenSegRegister = sevenSegRegister;
  }

  void writeDigit(int dig){
    byte a = ~0b10000000;
    byte b = ~0b01000000;
    byte c = ~0b00000100;
    byte d = ~0b00001000;
    byte e = ~0b00010000;
    byte f = ~0b00100000;
    byte g = ~0b00000010;
    byte dp = ~0b00000001;
    switch(dig){
      case 0:
        sevenSegRegister->write(a&b&c&d&e&f);
        break;
      case 1:
        sevenSegRegister->write(b&c);
        break;
      case 2:
        sevenSegRegister->write(a&b&d&e&g);
        break;
      case 3:
        sevenSegRegister->write(a&b&c&d&g);
        break;
      case 4:
        sevenSegRegister->write(b&c&f&g);
        break;
      case 5:
        sevenSegRegister->write(a&c&d&f&g);
        break;
      case 6:
        sevenSegRegister->write(a&c&d&e&f&g);
        break;
      case 7:
        sevenSegRegister->write(a&b&c);
        break;
      case 8:
        sevenSegRegister->write(a&b&c&d&e&f&g);
        break;
      case 9:
        sevenSegRegister->write(a&b&c&d&f&g);
        break;
    }
  }
};

class MatrixRowHandler{
  ShiftRegister* matrixRowRegister;
public:
  MatrixRowHandler(ShiftRegister* matrixRowRegister){
    this->matrixRowRegister = matrixRowRegister;
  }

  void writeRow(uint8_t dig){
    byte map[8];
    //This is the map for the 8x8 matrix it should be modified according to the wiring
    map[0] = 0b00100000;
    map[1] = 0b00000100;
    map[2] = 0b00000001;//
    map[3] = 0b00000010;//
    map[4] = 0b00001000;
    map[5] = 0b01000000;
    map[6] = 0b00010000;
    map[7] = 0b10000000;

    byte output = 0x00;

    //Do a matrix product with dig and write it to output
    for(int i=0; i<8; i++){
        if(dig & map[i]){
            output |= 0x01 << i;
        }
    }
    
    matrixRowRegister->write(output);
  }
};

class ColumnHandler{
  ShiftRegister* sr;
public:
  ColumnHandler(ShiftRegister* sevenSegRegister){
    this->sr = sevenSegRegister;
  }

  clear(){
    this->sr->writeX2(0b1111111100000000);
  }

  //This part should be modified according to the wiring
  turnOnColumn(int col){
    switch(col){
      case 0://7-seg 0
        this->sr->writeX2(0b1111111100000001);
        break;
      case 1://7-seg 1
        this->sr->writeX2(0b1111111100000010);
        break;
      case 2://7-seg 2
        this->sr->writeX2(0b1111111100000100);
        break;
      case 3://7-seg 3
        this->sr->writeX2(0b1111111100001000);
        break;
      case 4://7-seg 4
        this->sr->writeX2(0b1111111100010000);
        break;
      case 5://7-seg 5
        this->sr->writeX2(0b1111111100100000);
        break;
      case 6://7-seg 6
        this->sr->writeX2(0b1111111101000000);
        break;
      case 7://7-seg 7
        this->sr->writeX2(0b1111111110000000);
        break;

      case 8://col 0
        this->sr->writeX2(0b1101111100000000);
        break;
      case 9://col 1
        this->sr->writeX2(0b1111111000000000);
        break;
      case 10://col 2
        this->sr->writeX2(0b1111110100000000);
        break;
      case 11://col 3
        this->sr->writeX2(0b0111111100000000);
        break;
      case 12://col 4
        this->sr->writeX2(0b1111101100000000);
        break;
      case 13://col 5
        this->sr->writeX2(0b1011111100000000);
        break;
      case 14://col 6
        this->sr->writeX2(0b1110111100000000);
        break;
      case 15://col 7
        this->sr->writeX2(0b1111011100000000);
        break;
    }
  }
  
};


class LedInterfaceHandler{
  ShiftRegister* sr1;
  ShiftRegister* sr2;
  DigitOn7Seg* digitOn7Seg;
  ColumnHandler* columnHandler;
  MatrixRowHandler* matrixRowHandler;

  uint8_t sevenSeg[8];
  uint8_t matrix8x8[8];
public:
  LedInterfaceHandler(int rowSrPinLatchPin, int rowSrPinClockPin, int rowSrPinDataPin, int  colSrPinLatchPin, int colSrPinClockPin, int colSrPinDataPin){
    sr1 = new ShiftRegister(rowSrPinLatchPin, rowSrPinClockPin, rowSrPinDataPin);
    digitOn7Seg = new DigitOn7Seg(sr1);
    sr2 = new ShiftRegister(colSrPinLatchPin, colSrPinClockPin, colSrPinDataPin);
    matrixRowHandler = new MatrixRowHandler(sr1);

    columnHandler = new ColumnHandler(sr2);
  }

  ~LedInterfaceHandler(){
    delete sr1;
    delete sr2;
    delete digitOn7Seg;
    delete matrixRowHandler;
    delete columnHandler;
  }

  void setSevenSegment(int idx, int digit){
    sevenSeg[idx] = digit;
  }

  void setMatrix8x8(int idx, int data){
    matrix8x8[idx] = data;
  }

  void displayAll(uint32_t delay_us){
    for(int i=0; i<8; i++){
      columnHandler->clear();
      digitOn7Seg->writeDigit(sevenSeg[i]);
      columnHandler->turnOnColumn(i);
      delayMicroseconds(delay_us);
    //   delay(1000);
    }
    for(int i=0; i<8; i++){
      columnHandler->clear();
      matrixRowHandler->writeRow(matrix8x8[i]);
      columnHandler->turnOnColumn(i + 8);
      delayMicroseconds(delay_us);

    //   delay(1000);
    }
  }

  void turnOff(){
    columnHandler->clear();
    
  }

};


#endif //ARDUINO