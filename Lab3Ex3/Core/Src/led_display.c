
#include "main.h"
#include "led_display.h"
#include "software_timer.h"

#define DISPLAY_SCAN_MS 10
#define NO_OF_DIGITS 4

static const uint8_t seg7[10] =
{
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static const uint16_t segmentPins[7] =
{
    SEG0_Pin, SEG1_Pin, SEG2_Pin, SEG3_Pin,
    SEG4_Pin, SEG5_Pin, SEG6_Pin
};

static const uint16_t digitPins[NO_OF_DIGITS] =
{
    EN0_Pin, EN1_Pin, EN2_Pin, EN3_Pin
};

static const uint16_t ledPins[6] =
{
    L1_Pin, L2_Pin, L3_Pin, L4_Pin, L5_Pin, L6_Pin
};

static uint8_t displayBuffer[NO_OF_DIGITS] = {0, 0, 0, 0};
static uint8_t scanIndex = 0;

static void all_digits_off(void)
{
    for (uint8_t i = 0; i < NO_OF_DIGITS; i++)
    {
        HAL_GPIO_WritePin(GPIOA, digitPins[i], GPIO_PIN_SET);
    }
}

static void write_segments(uint8_t value)
{
    uint8_t pattern = seg7[value % 10U];

    for (uint8_t i = 0; i < 7; i++)
    {
        HAL_GPIO_WritePin(
            GPIOB,
            segmentPins[i],
            (pattern & (1U << i)) ? GPIO_PIN_RESET : GPIO_PIN_SET
        );
    }
}

void display_init(void)
{
    for (uint8_t i = 0; i < NO_OF_DIGITS; i++)
    {
        displayBuffer[i] = 0;
    }

    scanIndex = 0;
    all_digits_off();
    lights_off();
    setTimer(TIMER_SCAN, DISPLAY_SCAN_MS);
}

void display_set(uint8_t left, uint8_t right)
{
    if (left > 99U) left = 99U;
    if (right > 99U) right = 99U;

    displayBuffer[0] = left / 10U;
    displayBuffer[1] = left % 10U;
    displayBuffer[2] = right / 10U;
    displayBuffer[3] = right % 10U;
}

static void display_digit(uint8_t index)
{
    all_digits_off();
    write_segments(displayBuffer[index]);

    HAL_GPIO_WritePin(GPIOA, digitPins[index], GPIO_PIN_RESET);
}

void display_run(void)
{
    if (isTimerExpired(TIMER_SCAN))
    {
        setTimer(TIMER_SCAN, DISPLAY_SCAN_MS);
        display_digit(scanIndex);
        scanIndex = (scanIndex + 1U) % NO_OF_DIGITS;
    }
}

void road_light(uint8_t road, Light_t color)
{
    if (road > 1U || color > LIGHT_GREEN)
    {
        return;
    }

    uint8_t base = road * 3U;

    for (uint8_t i = 0; i < 3; i++)
    {
        HAL_GPIO_WritePin(GPIOA, ledPins[base + i], GPIO_PIN_RESET);
    }

    HAL_GPIO_WritePin(GPIOA, ledPins[base + color], GPIO_PIN_SET);
}

void lights_off(void)
{
    for (uint8_t i = 0; i < 6; i++)
    {
        HAL_GPIO_WritePin(GPIOA, ledPins[i], GPIO_PIN_RESET);
    }
}

void blink_color(Light_t color, uint8_t on)
{
    if (color > LIGHT_GREEN)
    {
        return;
    }

    for (uint8_t road = 0; road < 2; road++)
    {
        uint8_t index = road * 3U + color;

        HAL_GPIO_WritePin(
            GPIOA,
            ledPins[index],
            on ? GPIO_PIN_SET : GPIO_PIN_RESET
        );
    }
}
