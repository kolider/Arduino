
#define REG_1 6, 7, 3 //CLOCK, data, SH (PD6, PD7, PD3)
#define REG_2 9, 10, 8 //CLOCK, data, SH (PB1, PB2, PB0)
#define REG_DELAY_us 10
#define DATA_WIDTH_REG 8

unsigned int status[2];

unsigned int readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  pinMode(6,OUTPUT); //clock reg1
  pinMode(7,INPUT); //data reg1
  pinMode(3,OUTPUT);//latch reg1
  
  pinMode(9,OUTPUT);  //clock reg2
  pinMode(10,INPUT);  //data reg2
  pinMode(8,OUTPUT);  //latch reg2
  
  pinMode(5,INPUT); //bad pin
  
  digitalWrite(6,LOW);  //clock reg1
  digitalWrite(3,HIGH);  //latch reg1

  digitalWrite(9,LOW);  //clock reg2  
  digitalWrite(8,HIGH);  //latch reg2
  
  Serial.print("Test is OK: ");
  Serial.println(status[0]);
  Serial.println(status[1]);
}

void loop() {
    unsigned int reg1 = readReg(REG_1);
    unsigned int reg2 = readReg(REG_2);
    if (status[0] != reg1){ 
        Serial.println(reg1, HEX);
        status[0] = reg1;
    }
    if (status[1] != reg2){ 
        Serial.println(reg2, HEX);
        status[1] = reg2;
    }
}

unsigned int readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin) {

  unsigned int byteR = 0, temp;
  
  digitalWrite(latchPin, LOW);
  delay(REG_DELAY_us);
  digitalWrite(latchPin, HIGH);
  
  for (int i = 0; i < DATA_WIDTH_REG; i++) {
    temp = digitalRead(dataPin);
    
    byteR |= (temp << ((DATA_WIDTH_REG-1) - i));
    
    digitalWrite(clockPin, HIGH);
    delay(REG_DELAY_us);
    digitalWrite(clockPin, LOW);
    delay(REG_DELAY_us);   
  }
  return byteR;
}

