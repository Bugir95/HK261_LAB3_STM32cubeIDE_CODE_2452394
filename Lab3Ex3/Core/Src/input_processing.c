
#include "input_processing.h"
#include "input_reading.h"
#include "software_timer.h"

#define AUTO_REPEAT_MS 500

enum ButtonState
{
    BUTTON_RELEASED,
    BUTTON_PRESSED,
    BUTTON_PRESSED_MORE_THAN_1_SECOND
};

static enum ButtonState buttonState[NO_OF_BUTTONS];
static uint8_t buttonEvent[NO_OF_BUTTONS];

void input_processing_init(void)
{
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++)
    {
        buttonState[i] = BUTTON_RELEASED;
        buttonEvent[i] = 0;
    }
}

uint8_t button_event(uint8_t index)
{
    if (index >= NO_OF_BUTTONS)
    {
        return 0;
    }

    if (buttonEvent[index])
    {
        buttonEvent[index] = 0;
        return 1;
    }

    return 0;
}

void fsm_for_input_processing(void)
{
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++)
    {
        switch (buttonState[i])
        {
            case BUTTON_RELEASED:
                if (is_button_pressed(i))
                {
                    buttonState[i] = BUTTON_PRESSED;
                    buttonEvent[i] = 1;
                }
                break;

            case BUTTON_PRESSED:
                if (!is_button_pressed(i))
                {
                    buttonState[i] = BUTTON_RELEASED;
                }
                else if (is_button_pressed_1s(i))
                {
                    buttonState[i] = BUTTON_PRESSED_MORE_THAN_1_SECOND;
                    setTimer(TIMER_REPEAT0 + i, AUTO_REPEAT_MS);
                }
                break;

            case BUTTON_PRESSED_MORE_THAN_1_SECOND:
                if (!is_button_pressed(i))
                {
                    buttonState[i] = BUTTON_RELEASED;
                }
                else if (isTimerExpired(TIMER_REPEAT0 + i))
                {
                    buttonEvent[i] = 1;
                    setTimer(TIMER_REPEAT0 + i, AUTO_REPEAT_MS);
                }
                break;

            default:
                buttonState[i] = BUTTON_RELEASED;
                buttonEvent[i] = 0;
                break;
        }
    }
}
