#include "stm32f10x.h"
#include "key.h"
#include "timer.h"

static GPIO_TypeDef *const ports[] = { KEY1_PORT, KEY2_PORT, KEY3_PORT, KEY4_PORT, KEY5_PORT, KEY6_PORT };
static const uint16_t pins[] = { KEY1_PIN, KEY2_PIN, KEY3_PIN, KEY4_PIN, KEY5_PIN, KEY6_PIN };
static uint8_t raw[6], stable[6];
static uint32_t changed_at[6];

void Key_Init(void)
{
    GPIO_InitTypeDef init;
    uint8_t i;
    RCC_APB2PeriphClockCmd(KEY1_RCC | KEY2_RCC | KEY3_RCC | KEY4_RCC | KEY5_RCC | KEY6_RCC, ENABLE);
    init.GPIO_Mode = GPIO_Mode_IPU;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    for (i = 0; i < 6; i++) {
        init.GPIO_Pin = pins[i];
        GPIO_Init(ports[i], &init);
        raw[i] = stable[i] = 1;
        changed_at[i] = 0;
    }
}

uint8_t Key_GetNum(void)
{
    uint8_t i, level;
    uint32_t now = millis();
    for (i = 0; i < 6; i++) {
        level = GPIO_ReadInputDataBit(ports[i], pins[i]);
        if (level != raw[i]) {
            raw[i] = level;
            changed_at[i] = now;
        }
        if (level != stable[i] && (uint32_t)(now - changed_at[i]) >= 20u) {
            stable[i] = level;
            if (level == 1) return i + 1;
        }
    }
    return 0;
}
