#include <pins_arduino.h>
void setup()
{
    pinMode(PIN2, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    analogReference(DEFAULT);
    Serial.begin(115200);
}

void loop()
{
    uint32_t pot_val = analogRead(A0);
    uint32_t duty = map(pot_val, 0, 1023, 0, 15);

    if (pot_val == 0)
        digitalWrite(PIN2, HIGH);
    else
    {
        digitalWrite(PIN2, HIGH);
        delay(20 - duty);
        digitalWrite(PIN2, LOW);
        delay(duty);
    }

    char buff[255];
    sprintf(buff, "%04d\r", pot_val);
    Serial.print(buff);
}