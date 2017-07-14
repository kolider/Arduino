char status[7] = "eeddeed";

void setup() {
  Serial.begin(9600);
  while(!Serial);
  digitalWrite(2,HIGH);

  //int val = strncmp('a','b',1);

  for (int i = 0; i < 7; i++){
      
      Serial.print(i);
      Serial.print(": in line ");
      Serial.print(status[i], HEX);
      Serial.print("   ");

      if (status[i] == 'd'){
          Serial.print(status[i], HEX);
          Serial.print(" = ");
          Serial.println('d', HEX);
      }else{
          Serial.print(status[i], HEX);
          Serial.print(" != ");
          Serial.println('d', HEX);
      }
      
  }
  
}

void loop() {

}
