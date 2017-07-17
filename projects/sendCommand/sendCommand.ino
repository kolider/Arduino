boolean sendCommand(char* command);

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

boolean sendCommand(char* command)
{
    int len = strlen(command);
    String buffer = "";
    
    for (int i = 0; i < len; i++){
        Serial.print(command[i]);
        while(!Serial.available());
        char c = Serial.read();
        if (c != command[i]){
            return false;
        }
    }
    Serial.print("\r\n");
    
    while(!Serial.available());
    
    while ( Serial.available() ){
      char volatile c = ""; 
//      Serial.readBytes(&c, 1);
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
