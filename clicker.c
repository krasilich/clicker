#include <avr/io.h>
#include <util/delay.h>

#define LED_DDR         DDRB
#define LED_PORT        PORTB
#define LED_CLOCK       PB0
#define LED_DATA        PB1
#define LED_LATCH       PB2
#define LED_SEG_0       PB3
#define LED_SEG_1       PB4
#define LED_SEG_2       PB5
#define LED_OUTPUT      PB6
#define LED_SEG_DELAY   5

#define BUTTON_DDR      DDRD
#define BUTTON_PORT     PORTD
#define BUTTON_PIN      PIND
#define BUTTON_CLICK    PD3
#define BUTTON_ROW      PD2

#define BUZZER_DDR      DDRD
#define BUZZER_PORT     PORTD
#define BUZZER          PD4

#define DEBOUNCE_TIME   1

int clicks = 0;

uint8_t button_click_pressed = 0;

//    abcdefgDP
uint8_t numbers[] = {
    0b11111100,
    0b01100000,
    0b11011010,
    0b11110010,
    0b01100110,
    0b10110110,
    0b10111110,
    0b11100000,
    0b11111110,
    0b11110110
};

void put_value_to_led(uint8_t value);
void enable_led_segment(uint8_t value);
void enable_led_output();
void disable_led_output();
uint8_t get_value_for_seg(uint8_t seg);
uint8_t debouncePress(void);

int main(void)
{
    LED_DDR  = 0;
    LED_DDR |= (1 << LED_CLOCK)
    | (1 << LED_DATA)
    | (1 << LED_LATCH)
    | (1 << LED_OUTPUT)
    | (1 << LED_SEG_0)
    | (1 << LED_SEG_1)
    | (1 << LED_SEG_2);

    LED_PORT = 0;
    LED_PORT |= (1 << LED_LATCH);

    BUTTON_DDR = 0;
    BUTTON_PORT |= (1 << BUTTON_CLICK);
    BUTTON_PORT |= (1 << BUTTON_ROW);

    BUZZER_DDR |= (1 << BUZZER);
    BUZZER_PORT &= ~(1 << BUZZER);

    while(1) {
        //TODO: Move to interruption
        if (debouncePress()) {
            if (button_click_pressed == 0) {
                ++clicks;
                button_click_pressed = 1;
            }
        } else {
            button_click_pressed = 0;
        }

        for (uint8_t segment = 0; segment < 3; ++segment) {
            put_value_to_led(get_value_for_seg(segment));
            enable_led_segment(segment);
            enable_led_output();
            _delay_ms(LED_SEG_DELAY);
            disable_led_output();
        }
    }

    return 0;
}

void put_value_to_led(uint8_t value)
{
    LED_PORT &= ~(1 << LED_LATCH);

    for (uint8_t bit = 0; bit < 8; bit++) {
        if (value & (1 << bit)) {
            LED_PORT |= (1 << LED_DATA);
        } else {
            LED_PORT &= ~(1 << LED_DATA);
        }
        LED_PORT |= (1 << LED_CLOCK);
        LED_PORT &= ~(1 << LED_DATA);
        LED_PORT &= ~(1 << LED_CLOCK);
    }

    LED_PORT |= (1 << LED_LATCH);

    return;
}

void enable_led_segment(uint8_t seg)
{
    LED_PORT &= ~(1 << LED_SEG_0);
    LED_PORT &= ~(1 << LED_SEG_1);
    LED_PORT &= ~(1 << LED_SEG_2);

    switch (seg) {
        case 0:
            LED_PORT |= (1 << LED_SEG_0);
            break;
        case 1:
            LED_PORT |= (1 << LED_SEG_1);
            break;
        case 2:
            LED_PORT |= (1 << LED_SEG_2);
            break;
    }

    return;
}

void enable_led_output()
{
    LED_PORT &= ~(1 << LED_OUTPUT);
}

void disable_led_output()
{
    LED_PORT |= (1 << LED_OUTPUT);
}

uint8_t get_value_for_seg(uint8_t seg)
{
    int number = clicks, digit = 0;

    do {
        digit = number % 10;
        number /= 10;
    } while ((seg--) > 0);

    return numbers[digit];
}

uint8_t debouncePress()
{
    if (bit_is_clear(BUTTON_PIN, BUTTON_CLICK)) {
        _delay_ms(DEBOUNCE_TIME);
        if (bit_is_clear(BUTTON_PIN, BUTTON_CLICK)) {
            return 1;
        }
    }

    return 0;
}
