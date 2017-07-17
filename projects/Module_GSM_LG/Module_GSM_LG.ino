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

//Прототипи
uint8_t readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin);
boolean initialGSM (void);
boolean sendCommandGSM(char* command);
void sendRequest(void);

//Глобальні змінні
char status[DATA_WIDTH_REG] = "eeddeee";
char status2[DATA_WIDTH_REG] = "eedeeee";

//Первинна настройка
void setup() {

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

  Serial.begin(9600);
  while(!Serial);
  while (!initialGSM());
  sendRequest();
}

//Робочий Цикл
void loop() {
    
}

//Реалізація функцій
uint8_t readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin)
{
  uint8_t readRegPin, byteVal = 0;
  
  digitalWrite(latchPin, LOW);
  delay(REGISTER_DELAY_MS);
  digitalWrite(latchPin, HIGH);
  delay(REGISTER_DELAY_MS);
  
  for (int i = 0; i < DATA_WIDTH_REG; i++) {
    
    readRegPin = digitalRead(dataPin);

    byteVal |= (readRegPin << ((DATA_WIDTH_REG - 1) - i));

    digitalWrite(clockPin, HIGH);
    delay(REGISTER_DELAY_MS);
    digitalWrite(clockPin, LOW);
    delay(REGISTER_DELAY_MS);   
  }
  return byteVal;
}

boolean initialGSM (void) 
{
  delay(7000);
  
  Serial.write("AT\r\n");

  delay(15000);
  
  sendCommandGSM("AT+CIPSHUT");

  sendCommandGSM("AT+CGDCONT=1,\"IP\",\"www.kyivstar.net\"");

  sendCommandGSM("AT+CSTT=\"www.kyivstar.net\"");

  while (!sendCommandGSM("AT+CIICR")){
      delay(2000);
      sendCommandGSM("AT+CIPSHUT");
  }

  return true;
}

boolean sendCommandGSM(char* command)
{    
    int len = strlen(command);
    String buffer = "";
    
    for (int i = 0; i < len; i++){
        Serial.print(command[i]);
        while(!Serial.available());
        int j;
        for (j = 0; Serial.available(); j++){
            Serial.read();
        }
        if (j>1) return false;
                                
    }
    
    Serial.print("\r\n");
    
    for(unsigned long i = millis(); !Serial.available();){
        unsigned long time = millis() - i;
        if (time > 3000) break;
    }
    
    while ( Serial.available() ){
      char volatile c = ""; 
      c = Serial.read();
      buffer += c;
      if ( Serial.available() ){
           continue;
      }else delay(10);    
    }
    delay(20);

    if (buffer.indexOf("ERROR") >= 0){ 
        return false;
    }
    else{
        return true;    
    }
}

void sendRequest(void)
{
  Serial.write("AT+CIPSHUT\r\n");

  delay(5000);
  Serial.write("AT+CIISR\r\n");
  delay(2000);
  
  Serial.write("AT+CIPSTART=\"TCP\",\"online.tyche.ua\",\"747\"\r");

  delay(2000);
  char data[] = "{\"s01\":\"c\",\"s02\":\"o\",\"s03\":\"d\"}\r\n";
  
  Serial.print("AT+CIPSEND\r");

  delay(1000);
  Serial.print("POST / HTTP/1.1\r\nHost: online.tyche.ua:747\r\nConnection: Keep-Alive\r\nContent-Type: application/json; charset=UTF-8\r\nContent-Length: 33\r\n\r\n");

  Serial.print(data);

  delay(1000);
  Serial.write("\r");  

  delay(1000);
  Serial.print("AT+CIPSHUT\r");
}
