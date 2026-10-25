#include "control.h"
#include "actuators.h"
#include "usart.h"

static uint8_t mode = CONTROL_MANUAL;
extern volatile uint8_t report_asap;

uint8_t Control_GetMode(void) { return mode; }

void Control_SetMode(uint8_t value, const char *source)
{
    if (value > CONTROL_AUTO || value == mode) return;
    UsartPrintf(USART_DEBUG, "mode: %s -> %s, source=%s\r\n",
                mode ? "AUTO" : "MANUAL", value ? "AUTO" : "MANUAL", source);
    mode = value;
    report_asap = 1;
}

static uint8_t OutputBits(void)
{
    return Led_GetState() | (CoolingFan_GetState() << 1)
        | (Heater_GetState() << 2) | (Humidifier_GetState() << 3)
        | (DehumidifyingFan_GetState() << 4);
}

void Control_AutoUpdate(uint8_t temperature, uint8_t humidity,
                        uint8_t light, uint8_t climate_valid)
{
    uint8_t before;
    if (mode != CONTROL_AUTO) return;
    before = OutputBits();
    if (climate_valid) {
        if (temperature >= 30) CoolingFan_ON();
        else if (temperature <= 28) CoolingFan_OFF();
        if (temperature <= 15) Heater_ON();
        else if (temperature >= 18) Heater_OFF();
        if (humidity <= 30) Humidifier_ON();
        else if (humidity >= 40) Humidifier_OFF();
        if (humidity >= 70) DehumidifyingFan_ON();
        else if (humidity <= 60) DehumidifyingFan_OFF();
    } else {
        Heater_OFF();
        Humidifier_OFF();
    }
    if (light <= 20) Led_ON();
    else if (light >= 30) Led_OFF();
    if (before != OutputBits()) report_asap = 1;
}
