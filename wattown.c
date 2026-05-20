#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "ws2812.h"

#define N_PERIODS 96
#define CLOCK_PIXELS 24

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


void init_led(uint pin, bool state) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, state);
}

const uint8_t demo_solar[N_PERIODS] = {
    0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 3, 3, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 0, 0, 0, 0, 0, 0
};

const uint8_t demo_wind[N_PERIODS] = {
    5, 4, 5, 6, 7, 6, 5, 4, 3, 4, 5, 6, 5, 6, 5, 4, 3, 4, 5, 6, 7, 7, 7, 6,
    5, 4, 3, 2, 1, 2, 1, 0, 1, 2, 1, 2, 3, 2, 3, 2, 1, 0, 1, 0, 1, 2, 3, 2,
    5, 4, 5, 6, 7, 6, 5, 4, 3, 4, 5, 6, 5, 6, 5, 4, 3, 4, 5, 6, 7, 7, 7, 6,
    5, 4, 3, 2, 1, 2, 1, 0, 1, 2, 1, 2, 3, 2, 3, 2, 1, 0, 1, 0, 1, 2, 3, 2
};

const uint8_t demo_demand[N_PERIODS] = {
    3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3,
    3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3,
    3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3,
    3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3
};

uint8_t demo_t = 0;
uint8_t demo_storage_soc = 0;
bool demo_paused = false;

void demo_mode_update_storage() {
    // TODO: PWM for brightness levels instead of on / off
    gpio_put(STORAGE_LED_PIN_1, demo_storage_soc > 15);
    gpio_put(STORAGE_LED_PIN_2, demo_storage_soc > 30);
    gpio_put(STORAGE_LED_PIN_3, demo_storage_soc > 45);
    gpio_put(STORAGE_LED_PIN_4, demo_storage_soc > 60);
}

int64_t demo_mode_advance(alarm_id_t id, __unused void* user_data) {

    if (demo_paused) return 250000;

    uint8_t date = demo_t / 24 + 1;
    uint8_t time = demo_t % 24;

    if (!(demo_t % 12)) printf("Day\tTime\tDemand (MW)\tWind (MW)\tSolar (MW)\tReservoir (MWh)\n");

    for (size_t p = 0; p < CLOCK_PIXELS; p += 1) {
        uint32_t color = (p == time) ? urgb_u32(100, 100, 100) : urgb_u32(0, 0, 0);
        put_pixel(pio, sm, color);
    }


    uint8_t surplus = 0;
    uint8_t shortfall = 0;
    uint8_t supply = demo_wind[demo_t] + demo_solar[demo_t];

    if (supply >= demo_demand[demo_t]) {
        surplus = supply - demo_demand[demo_t];
    } else {
        shortfall = demo_demand[demo_t] - supply;
    }

    if (surplus) {
        demo_storage_soc += surplus;
    } else if (demo_storage_soc >= shortfall) {
        demo_storage_soc -= shortfall;
    } else {
        demo_storage_soc = 0;
    }

    demo_mode_update_storage();

    printf("%d\t%d:00\t%d\t\t%d\t\t%d\t\t%d\n", date, time,
        demo_demand[demo_t], demo_wind[demo_t], demo_solar[demo_t],
        demo_storage_soc);

    demo_t += 1;
    if (demo_t == N_PERIODS) demo_t = 0;

    return 1000000;

}

void toggle_demo_pause() {

    demo_paused ^= 1;

    if (demo_paused) {
        gpio_put(PLAYPAUSE_LED_PIN, false);
        printf("[Simulation paused]\n");
    } else {
        gpio_put(PLAYPAUSE_LED_PIN, true);
        printf("[Simulation resumed]\n");
    }

}

void demo_reset() {
    demo_t = 0;
    demo_storage_soc = 0;
    demo_mode_update_storage();
    put_pixel(pio, sm, urgb_u32(100,100,100));
    for (size_t p = 0; p < CLOCK_PIXELS - 1; p += 1) put_pixel(pio, sm, urgb_u32(0,0,0));
    printf("[Simulation reset]\n");
}

int main() {

    stdio_init_all();

    gpio_init(PLAYPAUSE_BUTTON_PIN);
    gpio_set_dir(PLAYPAUSE_BUTTON_PIN, GPIO_IN);

    gpio_init(RESET_BUTTON_PIN);
    gpio_set_dir(RESET_BUTTON_PIN, GPIO_IN);

    init_led(PLAYPAUSE_LED_PIN, true);

    init_led(STORAGE_LED_PIN_1, false);
    init_led(STORAGE_LED_PIN_2, false);
    init_led(STORAGE_LED_PIN_3, false);
    init_led(STORAGE_LED_PIN_4, false);

    pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, CLOCK_PIXELS_PIN, 1, true);
    ws2812_program_init(pio, sm, offset, CLOCK_PIXELS_PIN, 800000, false);

    alarm_id_t demo_alarm = add_alarm_in_ms(1000, demo_mode_advance, NULL, false);

    bool pause_pressed = false;
    bool pause_pressed_prev = false;

    bool reset_pressed = false;
    bool reset_pressed_prev = false;

    while (true) {

        // TODO: Mode change button

        pause_pressed = gpio_get(PLAYPAUSE_BUTTON_PIN);
        reset_pressed = gpio_get(RESET_BUTTON_PIN);

        if (pause_pressed && !pause_pressed_prev) toggle_demo_pause();
        if (reset_pressed && !reset_pressed_prev) demo_reset();

        pause_pressed_prev = pause_pressed;
        reset_pressed_prev = reset_pressed;

        sleep_ms(10);

    }

    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);

}
