#include <SoftwareSerial.h>

SoftwareSerial serialGSM (2,3); //RX, TX
char json_data[256];

void initialGSM();
void sendRequest();
void response();

void setup() {
  pinMode(10,0);
  digitalWrite(10,1);
  Serial.begin(9600);
  serialGSM.begin(9600);
  serialGSM.print("AT\r\n");
  delay(10000);
  initialGSM();
}

void loop() {
  if (Serial.available()) serialGSM.write(Serial.read());
  if (serialGSM.available()) Serial.write(serialGSM.read());
//  if (serialGSM.available()){
//      response();
//      Serial.write(json_data);
//  }

  if (!digitalRead(10)){
      sendRequest();
//      Serial.write("JSON: \r\n");
    }
}

void initialGSM () 
{
  serialGSM.print("AT+CIPSHUT\r\n");

  delay(1000);
  serialGSM.print("AT+CGDCONT=1,\"IP\",\"www.kyivstar.net\"\r\n");

  delay(1000);
  serialGSM.print("AT+CSTT=\"www.kyivstar.net\"\r\n");

  delay(2000);;
  serialGSM.print("AT+CIICR\r\n");

  delay(5000);
  serialGSM.write("AT+CIPSHUT\r\n");
}

void sendRequest()
{
  serialGSM.write("AT+CIPSHUT\r\n");

  delay(5000);
  serialGSM.write("AT+CIPSTART=\"TCP\",\"online.tyche.ua\",\"747\"\r");

  delay(5000);
  char data[] = "{\"p1\":{\"s1\":\"n\",\"s2\":\"d\"},\"p2\":{\"s1\":\"a\",\"s2\":\"n\"},\"p3\":{\"s1\":\"n\",\"s2\":\"n\"},\"p4\":{\"s1\":\"d\",\"s2\":\"d\"},\"p5\":{\"s1\":\"e\",\"s2\":\"e\"},\"p6\":{\"s1\":\"e\",\"s2\":\"d\"},\"p7\":{\"s1\":\"e\",\"s2\":\"d\"},\"p8\":{\"s1\":\"e\",\"s2\":\"d\"}}\r";
  serialGSM.print("AT+CIPSEND\r\n");

  delay(1000);
  serialGSM.print("POST / HTTP/1.1\r\nHost: online.tyche.ua:747\r\nConnection: Keep-Alive\r\nContent-Type: application/json; charset=UTF-8\r\nContent-Length: 202\r\n\r\n");

  serialGSM.print(data);

  delay(1000);
  serialGSM.write("\r\n");  
  response();
  
  serialGSM.print("AT+CIPSHUT\r\n");
}

void response () {
  char slice[] = {"\0\0\0\0"};
  for (uint8_t i = 0; serialGSM.available();){
      char inChar = serialGSM.read();
      if (((inChar == '\r') || (inChar == '\n')) && (slice[3] != '\n')){
          if ((i == 1) && (inChar == '\r')){
              i = 0;  
          }          
          slice[i] = inChar;
          i++;          
          if (i == 4) i = 0;    
          Serial.println(i, DEC);
          Serial.println(inChar);      
        }        
      else {
        if (slice[3] == '\n'){
          Serial.write("\r\nIn condition write response to variable\r\n");
          json_data[i] = inChar;
          i++;
        }
        else {
          i = 0;
        }
      }   
    }
    for (uint8_t i = 0; json_data[i] > 0; i++){
        if ((json_data[i] == '\r') && (json_data[i+1] == '\n')){
          json_data[i] = '\0';
          break;
        }
    }
}
