#include "blinky.h"


state_t led_state;

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN4, OUTPUT);

    pinMode(FREQ_BTN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FREQ_BTN), handle_btn, FALLING);

    led_state = s0_2hz;
}

volatile bool interrupt = false;
void handle_btn()
{
    interrupt = true;
}

void update_state(state_t &state)
{
    noInterrupts();
    switch (led_state)
    {
    case s0_2hz:
        led_state = s1_4hz;
        break;
    case s1_4hz:
        led_state = s2_8hz;
        break;
    case s2_8hz:
        led_state = s3_1hz;
        break;
    case s3_1hz:
        led_state = s0_2hz;
        break;
    }
    interrupts();
}


uint32_t get_period(state_t state)
{
    switch (state)
    {
    case s0_2hz:
        return T_2HZ_MS;
    case s1_4hz:
        return T_4HZ_MS;
    case s2_8hz:
        return T_8HZ_MS;
    case s3_1hz:
        return T_1HZ_MS;
    }
}

void loop()
{
    if (interrupt)
    {
        update_state(led_state);
        delay(100);
        interrupt = false;
    }

    uint32_t t_ms = get_period(led_state);

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PIN4, HIGH);
    delay(t_ms >> 1);

    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(PIN4, LOW);
    delay(t_ms >> 1);
}