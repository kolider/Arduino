char json_data[256];

void response();

void setup() {
  Serial.begin(9600);

}

void loop() {
  if (Serial.available()){
      char Ch = Serial.read();  
      if ( Ch == '!' ){
        while (1)
        {
            if (Serial.available()){
              response();
            }
        }
      }
      else {
          Serial.print(Ch,HEX);
      }
  }   
}

void response () {
  char slice[] = {'\0','\0','\0','\0'};
  for (uint8_t i = 0; Serial.available();){
      char inChar = Serial.read();
      if (((inChar == '\r') || (inChar == '\n')) && (slice[3] != '\n')){
          slice[i] = inChar;
          i++;
          if (i == 4) i = 0;
        }        
      else {
        if (slice[3] == '\n'){
          json_data[i] = inChar;
          i++;
        }
        else {
          i = 0;
        }
      }   
    }
    for (uint8_t i = 0; json_data[i]; i++){
        if ((json_data[i] == '\r') && (json_data[i+1] == '\n')){
          json_data[i] = '\0';
          break;
        }
    }
}
