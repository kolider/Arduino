#include <SoftwareSerial.h>

SoftwareSerial SSerial (2,4);

boolean sendCommandGSM(char* command);
boolean initialGSM (void);
String sendRequest(char* request);
String Response();
void clearSerial(void);

void setup() {
    
    SSerial.begin(9600);
    
    Serial.begin(9600);
    while(!Serial);

    do {
        Serial.write("AT\r\n");
        Serial.flush();
        delay(20);
    }while(!Serial.available());
    while(Serial.available()){
        Serial.read();
        delay(20);        
    }

    while(!initialGSM());
    
}

void loop() {

}

boolean sendCommandGSM(char* command)
{            
    String check = "";
    char control = "";
    int len = strlen(command);
    
    for (int i = 0; i < len; i++){
        Serial.write(command[i]);
        Serial.flush();
        delay(20);        
                                //////////////////////////////////////////////////// Можливо тут вставити очікування для приходу відповіді від модуля
        while(Serial.available()){
            control = (char)Serial.read();
        }
        
        if (control != command[i])
            if ((control != 0x0D) && (control != 0x0A) && (control != 0)){
                return false;
            }

        control = "";
    }

    Serial.write("\r\n");
    Serial.flush();
    delay(20);    
    
    while(Serial.available()){
        check += (char)Serial.read();
        delay(20);
    }       
    if (check.indexOf("ERROR") >= 0){
        SSerial.print("Error: ");
        SSerial.println(command);
        SSerial.print(check);
        return false;
    }
    else{
        SSerial.print("Success: ");
        SSerial.println(command);
        SSerial.print(check);
        return true;
    }    
}

boolean initialGSM(void)
{ 
    while (!sendCommandGSM("AT+CIPSHUT"));

    while (!sendCommandGSM("AT+CGDCONT=1,\"IP\",\"www.kyivstar.net\""));
  
    while (!sendCommandGSM("AT+CSTT=\"www.kyivstar.net\""));
  
    while (!sendCommandGSM("AT+CIICR")){
        delay(1000);
        sendCommandGSM("AT+CIPSHUT");
    }

    while (!sendCommandGSM("AT+CIPSHUT"));

    return true;
}

String sendRequest(char* request)
{
    int len = strlen(request);
    
    while(!sendCommandGSM("AT+CIPSTART=\"TCP\",\"online.tyche.ua\",\"747\"")){
        delay(1000);
        sendCommandGSM("AT+CIPSHUT");
    }

    delay(2000);
    
    Serial.print("AT+CIPSEND\r\n");
    Serial.flush();
  
    delay(1000);
    
    Serial.print("POST /forTest/ HTTP/1.1\r\nHost: online.tyche.ua:747\r\n");
    Serial.flush();
    Serial.print("Connection: Keep-Alive\r\nContent-Type: application/json; charset=UTF-8\r\nContent-Length: ");
    Serial.print(len+2);
    Serial.print("\r\n\r\n");

    Serial.flush();
  
    Serial.write(request);
    Serial.write("\r\n");
    Serial.flush();
    delay(500);

    clearSerial();
    Serial.write((char)26);      //sendCommandGSM(""); 
    while(!Serial.available());
    
    String responseVal = Response();
        
    while(!sendCommandGSM("AT+CIPSHUT")); 

     return responseVal;

}

String Response()
{
    if ( Serial.available() ){
        String response = "";
        
        while ( Serial.available() ){
            char c = ""; 
            Serial.readBytes(&c, 1);
            response += c;
            if ( Serial.available() ){
                 continue;
            }else {
                delay(10);
                if ( Serial.available() ){
                    continue;
                }
                else delay (1000);
            }    
        }
        if (response.length()){
            uint8_t position = response.indexOf('{');
            response.remove(0,position);
            
            position = response.length();
            response = response.substring(0,position-2);
        }
        return response;
        
    }else{
        return "";
    }
}

void clearSerial(void)
{
     while(Serial.available()){
        Serial.read();
        if (Serial.available()) continue;
        else delay(200);
    }
}
