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
#define DATA_WIDTH_REG 7

//Прототипи
char* readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin);
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
  //while (!initialGSM());
}

//Робочий Цикл
void loop() {

}

//Реалізація функцій
char* readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin)
{
  char readRegPin[DATA_WIDTH_REG] = "";
  
  digitalWrite(latchPin, LOW);
  delay(REGISTER_DELAY_MS);
  digitalWrite(latchPin, HIGH);
  delay(REGISTER_DELAY_MS);
  
  for (int i = (DATA_WIDTH_REG-1); i >= 0; i--) {
    
    readRegPin[i] = digitalRead(dataPin) ? 'a' : 'n';

    digitalWrite(clockPin, HIGH);
    delay(REGISTER_DELAY_MS);
    digitalWrite(clockPin, LOW);
    delay(REGISTER_DELAY_MS);   
  }
  return readRegPin;
}

boolean initialGSM (void) 
{
  if (!sendCommandGSM("AT\r")) return false;
  
  if (!sendCommandGSM("AT+CIPSHUT\r\n")) return false;

  if (!sendCommandGSM("AT+CGDCONT=1,\"IP\",\"www.kyivstar.net\"\r")) return false;

  if (!sendCommandGSM("AT+CSTT=\"www.kyivstar.net\"\r")) return false;

  if (!sendCommandGSM("AT+CIICR\r")) return false;

  return true;
}

boolean sendCommandGSM(char* command)
{

    String buffer = "";
    unsigned long timeStart = millis();
    
    Serial.print(command);
    Serial.print("\r");

    while(!Serial.available()){
        if ((millis() - timeStart) > 5000) return false;
    }
    
    while (Serial.available() ){
      char c = ""; 
      Serial.readBytes(&c, 1);
      buffer += c;
      if ( Serial.available() ){
           continue;
      }else delay(10);    
    }
    
    if (buffer.indexOf("OK") >= 0) return true;
    else return false;    
}

void sendRequest(void)
{
  Serial.write("AT+CIPSHUT\r");

  delay(5000);
  Serial.write("AT+CIPSTART=\"TCP\",\"online.tyche.ua\",\"747\"\r");

  delay(5000);
  char data[] = "{\"s1\":\"1\",\"s2\":\"1\",\"s3\":\"0\"}\r\n";
  Serial.print("AT+CIPSEND\r");

  delay(1000);
  Serial.print("POST / HTTP/1.1\r\nHost: online.tyche.ua:747\r\nConnection: Keep-Alive\r\nContent-Type: application/json; charset=UTF-8\r\nContent-Length: 28\r\n\r\n");

  Serial.print(data);

  delay(1000);
  Serial.write("\r");  

  delay(1000);
  Serial.print("AT+CIPSHUT\r");
}
