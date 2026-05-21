#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "ws2812.h"

#define N_PERIODS 96
#define HOURS_PER_DAY 24

#define PLAYPAUSE_LED_PIN PICO_DEFAULT_LED_PIN

#define PLAYPAUSE_BUTTON_PIN 0
#define RESET_BUTTON_PIN 1

#define STORAGE_LED_PIN_1 10
#define STORAGE_LED_PIN_2 11
#define STORAGE_LED_PIN_3 12
#define STORAGE_LED_PIN_4 13

#define CLOCK_PIXELS_PIN 2

PIO pio;
uint sm;
uint offset;

#include "util.h"
#include "clock.h"
#include "demo.h"

int main() {

    stdio_init_all();

    DemoState state = demo_init();

    alarm_id_t demo_alarm = add_alarm_in_ms(1000, demo_advance, &state, false);

    bool pause_pressed = false;
    bool pause_pressed_prev = false;

    bool reset_pressed = false;
    bool reset_pressed_prev = false;

    // TODO: Use interrupts for play/pause and reset
    // gpio_set_irq_enabled_with_callback(
    //     RESET_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, demo_button_handler);
    // gpio_set_irq_enabled(PLAYPAUSE_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true);

    while (true) {

        // TODO: Mode change button

        pause_pressed = gpio_get(PLAYPAUSE_BUTTON_PIN);
        reset_pressed = gpio_get(RESET_BUTTON_PIN);

        if (pause_pressed && !pause_pressed_prev) demo_toggle_pause(&state);
        if (reset_pressed && !reset_pressed_prev) demo_reset(&state);

        pause_pressed_prev = pause_pressed;
        reset_pressed_prev = reset_pressed;

        sleep_ms(10);

    }

    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);

}
