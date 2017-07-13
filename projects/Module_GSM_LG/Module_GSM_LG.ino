//Прошива під контроллер на макетці. Защелка першого регістру змінена з PD5 на PD3
//Реалізується: Зчитування з регістрів і засовування данних в формат json

//Для першого здвигового регістру
#define CLOCK_PIN_1 6 //PD6
#define DATA_PIN_1 7 //PD7
#define LATCH_PIN_1 3 //PD3

//Для другого здвигового регістру
#define CLOCK_PIN_2 9 //PB1
#define DATA_PIN_2 10 //PB2
#define LATCH_PIN_2 8 //PB0

//Загальні настройки роботи здвигових регістрів
#define REGISTER_1  CLOCK_PIN_1, DATA_PIN_1, LATCH_PIN_1 
#define REGISTER_2  CLOCK_PIN_2, DATA_PIN_2, LATCH_PIN_2 
#define REGISTER_DELAY_MS 10
#define DATA_WIDTH_REG 8

uint8_t readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  pinMode(CLOCK_PIN_1,OUTPUT); //clock reg1
  pinMode(DATA_PIN_1,INPUT); //data reg1
  pinMode(LATCH_PIN_1,OUTPUT);//latch reg1
  
  pinMode(CLOCK_PIN_2,OUTPUT);  //clock reg2
  pinMode(DATA_PIN_2,INPUT);  //data reg2
  pinMode(LATCH_PIN_2,OUTPUT);  //latch reg2
  
  pinMode(5,INPUT); //bad pin
  
  digitalWrite(CLOCK_PIN_1,LOW);  //clock reg1
  digitalWrite(LATCH_PIN_1,HIGH);  //latch reg1

  digitalWrite(CLOCK_PIN_2,LOW);  //clock reg2  
  digitalWrite(LATCH_PIN_2,HIGH);  //latch reg2
}

void loop() {

}

uint8_t readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin)
{
  uint8_t byteR = 0;
  
  digitalWrite(latchPin, LOW);
  delay(REGISTER_DELAY_MS);
  digitalWrite(latchPin, HIGH);
  delay(REGISTER_DELAY_MS);
  
  for (int i = 0, temp; i < DATA_WIDTH_REG; i++) {
    temp = digitalRead(dataPin);
    
    byteR |= (temp << ((DATA_WIDTH_REG-1) - i));
    
    digitalWrite(clockPin, HIGH);
    delay(REGISTER_DELAY_MS);
    digitalWrite(clockPin, LOW);
    delay(REGISTER_DELAY_MS);   
  }
  return byteR;
}
