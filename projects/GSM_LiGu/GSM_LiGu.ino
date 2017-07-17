#include <SoftwareSerial.h>

SoftwareSerial SerialGSM (2,3);

boolean sendCommandGSM(char* command);

void setup() {
    pinMode(10, 0);
    digitalWrite(10, 1);
    
    Serial.begin(9600);
    while(!Serial);
    
    SerialGSM.begin(9600);
    while(!SerialGSM);
    
    SerialGSM.write("AT\r\n");
    delay(20);
    while(SerialGSM.available()){
        SerialGSM.read();
        SerialGSM.flush();
        delay(20);
    }
    
    delay(2000);
    sendCommandGSM("AT");
    sendCommandGSM("AT+CSQ");
    sendCommandGSM("ATs");
}
void loop() {

}

boolean sendCommandGSM(char* command)
{
    String check = "";
    char control = "";
    int len = strlen(command);
    
    for (int i = 0; i < len; i++){
        SerialGSM.write(command[i]);
        SerialGSM.flush();
        delay(20);        
        
        if (SerialGSM.available()){
            while(SerialGSM.available()){
                control = (char)SerialGSM.read();
            }
        }
        if (control != command[i])
            if ((control != 0x0D) && (control != 0x0A) && (control != 0)){
                return false;
            }
    }

    SerialGSM.write("\r\n");
    SerialGSM.flush();
    delay(20);    
    
    while(SerialGSM.available()){
        check += (char)SerialGSM.read();
        delay(20);
    }       
    if (check.indexOf("ERROR") >= 0){
        return false;
    }
    else{
        return true;
    }    
}
