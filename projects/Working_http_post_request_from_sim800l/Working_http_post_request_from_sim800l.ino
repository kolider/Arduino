#include <SoftwareSerial.h>

SoftwareSerial serialGSM (2,3); //RX, TX

void initialGSM(void);
void sendRequest(void);
void setup() {
  pinMode(10,0);
  digitalWrite(10,1);
  Serial.begin(9600);
  serialGSM.begin(9600);
  serialGSM.print("AT\r");
  delay(10000);
  initialGSM();
}

void loop() {
  if (Serial.available()) serialGSM.write(Serial.read());
  if (serialGSM.available()) Serial.write(serialGSM.read());

  if (!digitalRead(10)){
      sendRequest();
    }
}

void initialGSM (void) 
{
  serialGSM.print("AT+CIPSHUT\r\n");

  delay(1000);
  serialGSM.print("AT+CGDCONT=1,\"IP\",\"www.kyivstar.net\"\r\n");

  delay(1000);
  serialGSM.print("AT+CSTT=\"www.kyivstar.net\"\r");

  delay(1000);;
  serialGSM.print("AT+CIICR\r");

  delay(5000);
}

void sendRequest(void)
{
  serialGSM.write("AT+CIPSHUT\r");

  delay(5000);
  serialGSM.write("AT+CIPSTART=\"TCP\",\"online.tyche.ua\",\"747\"\r");

  delay(5000);
  //char data[] = "{\"s1\":\"1\",\"s2\":\"1\",\"s3\":\"0\"}\r\n";
  scanf()
  serialGSM.print("AT+CIPSEND\r");

  delay(1000);
  serialGSM.print("POST / HTTP/1.1\r\nHost: online.tyche.ua:747\r\nConnection: Keep-Alive\r\nContent-Type: application/json; charset=UTF-8\r\nContent-Length: 28\r\n\r\n");

  serialGSM.print(data);

  delay(1000);
  serialGSM.write("\r");  

  delay(1000);
  serialGSM.print("AT+CIPSHUT\r");
}

