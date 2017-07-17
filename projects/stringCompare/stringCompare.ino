char* result;

void setup() {
    Serial.begin(9600);
    while(!Serial);
    char var[] = "Hello ";
    char var2[] = "world";
    result = strcat(var, var2);

    Serial.print("var sizeof: ");
    Serial.println(sizeof(var));

    Serial.print("var2 sizeof: ");
    Serial.println(sizeof(var2));

    Serial.print("result sizeof: ");
    Serial.println(sizeof(result));


    Serial.println(var);

    Serial.print(result);
}

void loop() {

}

