String buffer;

void setup() {
  Serial.begin(9600);
}

void loop() {
    if (buffer.length()){
        uint8_t position = buffer.indexOf("{");
        buffer.remove(0,position-1);
        Serial.println(buffer);
        buffer = "";
    }
}

void serialEvent() {
  buffer = "";
  while (Serial.available() ){
    char c = ""; 
    Serial.readBytes(&c, 1);
    buffer += c;
    if ( Serial.available() ){
         continue;
    }else delay(10);    
  }
}
