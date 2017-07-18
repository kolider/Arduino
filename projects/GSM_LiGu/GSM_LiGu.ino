#include <SoftwareSerial.h>

SoftwareSerial SerialGSM (2,3);

boolean sendCommandGSM(char* command);
boolean initialGSM (void);
boolean sendRequest(char* request);

void setup() {
    
    Serial.begin(9600);
    while(!Serial);
    
    SerialGSM.begin(9600);
    while(!SerialGSM);

    while(!initialGSM());

    sendRequest("{\"s01\":\"o\",\"s02\":\"d\"}");
    
}

void loop() {

}

boolean sendCommandGSM(char* command)
{    
    do {
        SerialGSM.write("AT\r\n");
        SerialGSM.flush();
        delay(20);
    }while(!SerialGSM.available());
    while(SerialGSM.available()){
        SerialGSM.read();
        delay(20);        
    }
        
    String check = "";
    char control = "";
    int len = strlen(command);
    
    for (int i = 0; i < len; i++){
        SerialGSM.write(command[i]);
        SerialGSM.flush();
        delay(20);        
                                //////////////////////////////////////////////////// Можливо тут вставити очікування для приходу відповіді від модуля
        while(SerialGSM.available()){
            control = (char)SerialGSM.read();
        }
        
        if (control != command[i])
            if ((control != 0x0D) && (control != 0x0A) && (control != 0)){
                return false;
            }

        control = "";
    }

    SerialGSM.write("\r\n");
    SerialGSM.flush();
    delay(20);    
    
    while(SerialGSM.available()){
        check += (char)SerialGSM.read();
        delay(20);
    }       
    if (check.indexOf("ERROR") >= 0){
        Serial.print("Error: ");
        Serial.println(command);
        Serial.print(check);
        return false;
    }
    else{
        Serial.print(check);
        return true;
    }    
}

boolean initialGSM(void)
{
    if (!sendCommandGSM("AT+CIPSHUT")) return false;

    if (!sendCommandGSM("AT+CGDCONT=1,\"IP\",\"www.kyivstar.net\"")) return false;
  
    if (!sendCommandGSM("AT+CSTT=\"www.kyivstar.net\"")) return false;
  
    if (!sendCommandGSM("AT+CIICR")) return false;

    if (!sendCommandGSM("AT+CIPSHUT")) return false;

    return true;
}

boolean sendRequest(char* request)
{
    int len = strlen(request);
    
    sendCommandGSM("AT+CIPSHUT");
    delay(5000);
  
    SerialGSM.write("AT+CIFSR\r\n");
    delay(2000);
    
    sendCommandGSM("AT+CIPSTART=\"TCP\",\"online.tyche.ua\",\"747\"");

    delay(2000);
  
    //char data[] = "{\"s01\":\"c\",\"s02\":\"o\",\"s03\":\"d\"}\r\n";
    
    SerialGSM.print("AT+CIPSEND\r\n");
  
    delay(1000);
    while(SerialGSM.available()){
        Serial.print((char)SerialGSM.read());
        delay(20);
    }
    
    SerialGSM.print("POST / HTTP/1.1\r\nHost: online.tyche.ua:747\r\n");
    SerialGSM.flush();

    while(SerialGSM.available()){
        Serial.println("Point 1");
        Serial.print((char)SerialGSM.read());
        Serial.flush();
        delay(20);
        if (SerialGSM.available()) continue;
        else delay(200);
    }
    
    SerialGSM.print("Connection: Keep-Alive\r\nContent-Type: application/json; charset=UTF-8\r\nContent-Length: ");
    SerialGSM.flush();
    SerialGSM.print(len+2);
    SerialGSM.flush();
    SerialGSM.print("\r\n\r\n");
    SerialGSM.flush();

    while(SerialGSM.available()){
        Serial.println("Point 2");
        Serial.print((char)SerialGSM.read());
        Serial.flush();
        delay(20);
        if (SerialGSM.available()) continue;
        else delay(200);
    }
  
    SerialGSM.write(request);
    SerialGSM.flush();
    delay(500);
    SerialGSM.write("\r\n");
    SerialGSM.flush();
    delay(20);
    while(SerialGSM.available()){
        Serial.println("Point 3");
        Serial.print((char)SerialGSM.read());
        Serial.flush();
        delay(20);
        if (SerialGSM.available()) continue;
        else delay(200);
    }
  
    delay(1000);
    sendCommandGSM("");      //SerialGSM.write((char)26);   
    while(SerialGSM.available()){
        Serial.println("Point 4");
        Serial.print((char)SerialGSM.read());
        Serial.flush();
        delay(20);
    }
  
    delay(1000);
    sendCommandGSM("AT+CIPSHUT");  
    while(SerialGSM.available()){
        Serial.println("Point 5");
        Serial.print((char)SerialGSM.read());
        Serial.flush();
        delay(20);
    }
}
