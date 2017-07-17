//6, 7, 5 //CLOCK, data, SH (PD6, PD7, PD5)
//9, 10, 8 //CLOCK, data, SH (PB1, PB2, PB0)

#define NUMBER_OF_SHIFT_CHIPS   1

/* Width of data (how many ext lines).
*/
#define DATA_WIDTH   NUMBER_OF_SHIFT_CHIPS * 8

/* Width of pulse to trigger the shift register to read and latch.
*/
#define PULSE_WIDTH_USEC   5

/* Optional delay between shift register reads.
*/
#define POLL_DELAY_MSEC   1

/* You will need to change the "int" to "long" If the
 * NUMBER_OF_SHIFT_CHIPS is higher than 2.
*/
#define BYTES_VAL_T unsigned int

int ploadPin        = 5;  // Connects to Parallel load pin the 165
//int clockEnablePin  = 9;  // Connects to Clock Enable pin the 165
int dataPin         = 7; // Connects to the Q7 pin the 165
int clockPin        = 6; // Connects to the Clock pin the 165

BYTES_VAL_T pinValues;
BYTES_VAL_T oldPinValues;

/* This function is essentially a "shift-in" routine reading the
 * serial Data from the shift register chips and representing
 * the state of those pins in an unsigned integer (or long).
*/
BYTES_VAL_T read_shift_regs()
{
    long bitVal;
    BYTES_VAL_T bytesVal = 0;

    /* Trigger a parallel Load to latch the state of the data lines,
    */
    digitalWrite(ploadPin, LOW);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(ploadPin, HIGH);

    /* Loop to read each bit value from the serial out line
     * of the SN74HC165N.
    */
    for(int i = 0; i < DATA_WIDTH; i++)
    {
        bitVal = digitalRead(dataPin);

        /* Set the corresponding bit in bytesVal.
        */
        bytesVal |= (bitVal << ((DATA_WIDTH-1) - i));

        /* Pulse the Clock (rising edge shifts the next bit).
        */
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(PULSE_WIDTH_USEC);
        digitalWrite(clockPin, LOW);
    }

    return(bytesVal);
}

/* Dump the list of zones along with their current status.
*/
void display_pin_values()
{
    Serial.print("Pin States:\r\n");

    for(int i = 0; i < DATA_WIDTH; i++)
    {
        Serial.print("  Pin-");
        Serial.print(i);
        Serial.print(": ");

        if((pinValues >> i) & 1)
            Serial.print("HIGH");
        else
            Serial.print("LOW");

        Serial.print("\r\n");
    }

    Serial.print("\r\n");
}

void setup()
{
    Serial.begin(9600);

    /* Initialize our digital pins...
    */
    pinMode(ploadPin, OUTPUT);

    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, INPUT);

    digitalWrite(clockPin, LOW);
    digitalWrite(ploadPin, HIGH);

    /* Read in and display the pin states at startup.
    */
    pinValues = read_shift_regs();
    display_pin_values();
    oldPinValues = pinValues;
}

void loop()
{
    /* Read the state of all zones.
    */
    pinValues = read_shift_regs();

    /* If there was a chage in state, display which ones changed.
    */
    if(pinValues != oldPinValues)
    {
        Serial.print("*Pin value change detected*\r\n");
        display_pin_values();
        oldPinValues = pinValues;
    }

    delay(POLL_DELAY_MSEC);
}
