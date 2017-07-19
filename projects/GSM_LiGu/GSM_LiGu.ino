#define COUNT_L 0 ///////////////////////////////////Кількість регістрів в першому плечі !!!!!!!!!!!!!!!!!!!!!!!!
#define COUNT_R 1 ///////////////////////////////////Кількість регістрів в другому плечі !!!!!!!!!!!!!!!!!!!!!!!!
#define BITS 8
#define REG_L 6, 7, 3, BITS*COUNT_L //CLOCK(PD6), data(PD7), SH(PD3), розрядність плеча 
#define REG_R 9, 10, 8, BITS*COUNT_R //CLOCK(PB1), data(PB2), SH(PB0), розрядність плеча 
#define REG_DELAY_ms 10

#define SIZE_BUFFER ((BITS*COUNT_L + BITS*COUNT_R)*10)+3 //for json-fotmating before request to server

boolean sendCommandGSM(char* command);
boolean initialGSM ();
String sendRequest(char* request);      //Відправляє зформовані дані у тілі запиту
String Response();                      //Для роботи функції sendRequest() (повертає відповідь від серверу)
void clearSerial();
String readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin, uint8_t dataWidth); ////Для роботи функції (повертає стан по одному з плечей)
String loadStatusRegs();                //Повертає стан датчиків по двом плечам послідовно
boolean check(String presentStatus, String maskStatus);    //повертає ПРАВДА якщо змін не має
boolean changeStatus(String presentStatus, String &globalStatus);
String parseFromJSON(String json);
String prepareListenMaskToRecord(String presentStatus, String inputMask);

String status = "";

void setup() {
    
    pinMode(6,OUTPUT); //clock reg1    {       <-- Setting Shift Register 
    pinMode(7,INPUT); //data reg1
    pinMode(3,OUTPUT);//latch reg1
    
    pinMode(9,OUTPUT);  //clock reg2
    pinMode(10,INPUT);  //data reg2
    pinMode(8,OUTPUT);  //latch reg2
    
    pinMode(5,INPUT); //bad pin
    
    digitalWrite(6,LOW);  //clock reg1
    digitalWrite(3,HIGH);  //latch reg1
  
    digitalWrite(9,LOW);  //clock reg2  
    digitalWrite(8,HIGH);  //latch reg2    }    <-- Setting Shift Register 
    
    Serial.begin(9600); //                 {    <-- Setting GSM Module
    while(!Serial);

//    delay(10000);
//    do {    /////////////////////////////////////////////////////////////////////In future will be add to sendCommandGSM function22222222222
//        Serial.write("AT\r\n");
//        Serial.flush();
//        delay(20);
//    }while(!Serial.available());
//    clearSerial();  /////////////////////////////////////////////////////////////TO THAT2222222222222222222222222222222222222222222222222222

    while(!initialGSM()); //               {    <-- Setting GSM Module
}

void loop() {
  
    String presentStatus = loadStatusRegs();    

    if (!check(presentStatus, status)){
        changeStatus(presentStatus, status);
    }

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

        while(Serial.available()){
            control = (char)Serial.read();
        }
        
        if (control != command[i])
            if ((control != 0x0D) && (control != 0x0A) && (control != 0)){
                Serial.write("\r\n");
                Serial.flush();
                Serial.write("AT\r\n");
                Serial.flush();
                return false;
            }

        control = "";
    }

    Serial.write("\r\n");
    Serial.flush();
    unsigned long time = millis();
    while(!Serial.available())
    {
        if ((millis() - time) > 2000) return false;
    }    
    
    while(Serial.available()){
        check += (char)Serial.read();
        delay(20);
    }       
    if (check.indexOf("ERROR") >= 0){
        return false;
    }
    else{
        return true;
    }    
}

boolean initialGSM()
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
        String responseBodyJSON = "", temp = "";
        
        while ( Serial.available() ){
            char c = ""; 
            Serial.readBytes(&c, 1);
            temp += c;
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
        int length = temp.length();
        if (length){
            int position = temp.indexOf("{");
            responseBodyJSON = temp.substring(position, length-2);    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            return responseBodyJSON;
        }
        return temp;
        
    }else{
        return "";
    }
}

void clearSerial()
{
     while(Serial.available()){
        Serial.read();
        if (Serial.available()) continue;
        else delay(200);
    }
}

String readReg(uint8_t clockPin, uint8_t dataPin, uint8_t latchPin, uint8_t dataWidth = 8)
{
    boolean temp;
    String inputPins = "";
    
    digitalWrite(latchPin, LOW);
    delay(REG_DELAY_ms);
    digitalWrite(latchPin, HIGH);
    
    for (int i = 0; i < dataWidth; i++) {
      temp = digitalRead(dataPin);
      
      inputPins += (temp) ? 'o' : 'c';
      
      digitalWrite(clockPin, HIGH);
      delay(REG_DELAY_ms);
      digitalWrite(clockPin, LOW);
      delay(REG_DELAY_ms);   
    }
    return inputPins;
}

String loadStatusRegs()
{
    String statusPins = "";

    statusPins = readReg(REG_L);
    statusPins += readReg(REG_R);

    return statusPins;
}

boolean check(String presentStatus, String maskStatus)//Функція перевірки по масці. Якщо d не співпадає з теперішнім станом то не буде реагувати
{
    int length = presentStatus.length();
    
    if (length != maskStatus.length()) return false;
    
    for (int i = 0; i < length; i++){
        if (presentStatus.charAt(i) != maskStatus.charAt(i)){
            if (maskStatus.charAt(i) == 'd'){
                continue;
            }
            else return false;
        }
    }
    return true;
        
}

boolean changeStatus(String presentStatus, String &globalStatus)
{
    char buffer[SIZE_BUFFER];
    int length = presentStatus.length();
    String json = "", mask = "";

    if (!length && !globalStatus.length()) return false;
    
    buffer[0] = '{';
    int position = 1;
    for (int sensor = 1; sensor <= length; sensor++){
        buffer[position] = '"';
        position++;
        buffer[position] = 's';
        position++;
        if (sensor >= 10){
            if (sensor >= 20){
                if (sensor >= 30){
                    if (sensor >= 40){
                        if (sensor >= 50){
                            if (sensor >= 60){
                                return false;
                            }else{
                                buffer[position] = '5';
                                position++;
                            }
                        }else{
                            buffer[position] = '4';
                            position++;
                        }
                    }else{
                        buffer[position] = '3';
                        position++;
                    }
                }else{
                    buffer[position] = '2';
                    position++;
                }
            }else{
                buffer[position] = '1';
                position++;
            }
        }else{
            buffer[position] = '0';
            position++;
        }
        buffer[position] = sensor + '0';
        position++;
        buffer[position] = '"';
        position++;
        buffer[position] = ':';
        position++;
        buffer[position] = '"';
        position++;
        
        if (globalStatus.charAt(sensor-1) == 'd'){
            buffer[position] = globalStatus.charAt(sensor-1);
        }else{
            buffer[position] = presentStatus.charAt(sensor-1);            
        }
        //buffer[position] = (globalStatus.charAt(sensor-1) == 'd') ? 'd' : presentStatus.charAt(sensor-1);
        
        position++;
        buffer[position] = '"';
        position++;
        if ((sensor+1) <= length){
           buffer[position] = ',';
           position++; 
        }
    }

    buffer[position] ='}'; 
    buffer[position+1] ='\0';
    
    json = sendRequest(buffer);
    
    mask = parseFromJSON(json); //Тут зробити запис тільки по символі 'd' !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    globalStatus = prepareListenMaskToRecord(presentStatus, mask);

    return true;
}

String parseFromJSON(String json)
{
    String parseString = "";
    int length = json.length();

    for (int i = 8; i < length; i+=10){
        parseString += json.charAt(i);
    }
    return parseString;
}

String prepareListenMaskToRecord(String presentStatus, String inputMask)
{
    int length = inputMask.length();
    String toRecord = "";

    for (int i = 0; i < length; i++){
        char c = inputMask.charAt(i);
        if (c == 'd'){
            toRecord += c;
        }else{
            toRecord += (char)presentStatus.charAt(i);
        }
    }

    return toRecord;
}
