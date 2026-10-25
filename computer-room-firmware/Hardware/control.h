#ifndef CONTROL_H
#define CONTROL_H
#include <stdint.h>
#define CONTROL_MANUAL 0u
#define CONTROL_AUTO 1u
uint8_t Control_GetMode(void);
void Control_SetMode(uint8_t value, const char *source);
void Control_AutoUpdate(uint8_t temperature, uint8_t humidity,
                        uint8_t light, uint8_t climate_valid);
#endif
