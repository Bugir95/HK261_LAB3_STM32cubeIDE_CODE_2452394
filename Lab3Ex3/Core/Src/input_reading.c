
#include "main.h"
#include "input_reading.h"

#define BUTTON_IS_PRESSED    GPIO_PIN_RESET
#define BUTTON_IS_RELEASED   GPIO_PIN_SET

#define BUTTON_SAMPLE_MS     10
#define LONG_PRESS_SAMPLES   (1000 / BUTTON_SAMPLE_MS)

static const uint16_t buttonPin[NO_OF_BUTTONS] =
{
    BUTTON1_Pin,
    BUTTON2_Pin,
    BUTTON3_Pin
};

static GPIO_PinState buttonBuffer[NO_OF_BUTTONS];
static GPIO_PinState debounceBuffer1[NO_OF_BUTTONS];
static GPIO_PinState debounceBuffer2[NO_OF_BUTTONS];
static uint16_t counterForPress1s[NO_OF_BUTTONS];
static uint8_t flagForPress1s[NO_OF_BUTTONS];
static uint8_t sampleDivider = 0;

void button_init(void)
{
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++)
    {
        buttonBuffer[i] = BUTTON_IS_RELEASED;
        debounceBuffer1[i] = BUTTON_IS_RELEASED;
        debounceBuffer2[i] = BUTTON_IS_RELEASED;
        counterForPress1s[i] = 0;
        flagForPress1s[i] = 0;
    }

    sampleDivider = 0;
}

void button_reading(void)
{
    if (++sampleDivider < MS_TO_TICKS(BUTTON_SAMPLE_MS))
    {
        return;
    }

    sampleDivider = 0;

    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++)
    {
        debounceBuffer2[i] = debounceBuffer1[i];
        debounceBuffer1[i] = HAL_GPIO_ReadPin(
            BUTTON1_GPIO_Port,
            buttonPin[i]
        );

        if (debounceBuffer1[i] == debounceBuffer2[i])
        {
            buttonBuffer[i] = debounceBuffer1[i];
        }

        if (buttonBuffer[i] == BUTTON_IS_PRESSED)
        {
            if (counterForPress1s[i] < LONG_PRESS_SAMPLES)
            {
                counterForPress1s[i]++;
            }
            else
            {
                flagForPress1s[i] = 1;
            }
        }
        else
        {
            counterForPress1s[i] = 0;
            flagForPress1s[i] = 0;
        }
    }
}

unsigned char is_button_pressed(unsigned char index)
{
    if (index >= NO_OF_BUTTONS)
    {
        return 0;
    }

    return buttonBuffer[index] == BUTTON_IS_PRESSED;
}

unsigned char is_button_pressed_1s(unsigned char index)
{
    if (index >= NO_OF_BUTTONS)
    {
        return 0;
    }

    return flagForPress1s[index] == 1;
}
