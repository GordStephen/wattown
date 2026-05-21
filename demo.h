#ifndef demo_h_INCLUDED
#define demo_h_INCLUDED

const uint8_t solar_clearsky[HOURS_PER_DAY] = {
    0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0
};

typedef struct DemoState {
    uint8_t demand[N_PERIODS];
    uint8_t wind[N_PERIODS];
    uint8_t solar[N_PERIODS];
    uint8_t t;
    uint8_t storage_soc;
    bool paused;
} DemoState;

void demo_init_controls(DemoState* state) {
    init_button(PLAYPAUSE_BUTTON_PIN);
    init_button(RESET_BUTTON_PIN);
    init_led(PLAYPAUSE_LED_PIN, true);
}

void demo_init_storageleds(DemoState* state) {
    init_led(STORAGE_LED_PIN_1, false);
    init_led(STORAGE_LED_PIN_2, false);
    init_led(STORAGE_LED_PIN_3, false);
    init_led(STORAGE_LED_PIN_4, false);
}

void demo_update_storageleds(DemoState* state) {
    // TODO: PWM for brightness levels instead of on / off
    gpio_put(STORAGE_LED_PIN_1, state->storage_soc > 15);
    gpio_put(STORAGE_LED_PIN_2, state->storage_soc > 30);
    gpio_put(STORAGE_LED_PIN_3, state->storage_soc > 45);
    gpio_put(STORAGE_LED_PIN_4, state->storage_soc > 60);
}

void demo_init_clockleds(DemoState* state) {
    pio_claim_free_sm_and_add_program_for_gpio_range(
        &ws2812_program, &pio, &sm, &offset, CLOCK_PIXELS_PIN, 1, true);
    ws2812_program_init(pio, sm, offset, CLOCK_PIXELS_PIN, 800000, false);
}

void demo_update_clockleds(DemoState* state) {

    uint8_t time = state->t % 24;

    TimeOfDay timeofday = timesofday[time];
    uint8_t brightness = ((uint16_t)state->solar[state->t] << 4) / 7;
    brightness || (brightness = 1);

    for (size_t p = 0; p < HOURS_PER_DAY; p += 1) {
        PixelType pixeltype = p == time ? orb : pixeltypes[p];
        uint32_t color = clock_base_colors[timeofday][pixeltype] * brightness;
        put_pixel(pio, sm, color);
    }

}

DemoState demo_init() {

    DemoState state = {
        .demand = {
            3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3,
            3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3,
            3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3,
            3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3 },
        .wind = {
            5, 4, 5, 6, 7, 6, 5, 4, 3, 4, 5, 6, 5, 6, 5, 4, 3, 4, 5, 6, 7, 7, 7, 6,
            5, 4, 3, 2, 1, 2, 1, 0, 1, 2, 1, 2, 3, 2, 3, 2, 1, 0, 1, 0, 1, 2, 3, 2,
            5, 4, 5, 6, 7, 6, 5, 4, 3, 4, 5, 6, 5, 6, 5, 4, 3, 4, 5, 6, 7, 7, 7, 6,
            5, 4, 3, 2, 1, 2, 1, 0, 1, 2, 1, 2, 3, 2, 3, 2, 1, 0, 1, 0, 1, 2, 3, 2 },
        .solar = {
            0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 3, 3, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 0, 0, 0, 0, 0, 0 },
        .t = 0,
        .storage_soc = 0,
        .paused = false,
    };

    demo_init_controls(&state);
    demo_init_clockleds(&state);
    demo_init_storageleds(&state);

    return state;

}

void demo_toggle_pause(DemoState* state) {

    state->paused ^= 1;

    if (state->paused) {
        gpio_put(PLAYPAUSE_LED_PIN, false);
        printf("[Simulation paused]\n");
    } else {
        gpio_put(PLAYPAUSE_LED_PIN, true);
        printf("[Simulation resumed]\n");
    }

}

void demo_reset(DemoState* state) {
    state->t = 0;
    state->storage_soc = 0;
    demo_update_storageleds(state);
    demo_update_clockleds(state);
    printf("[Simulation reset]\n");
}

// void demo_button_handler(uint pin, uint32_t event_mask) {
//     if (pin == RESET_BUTTON_PIN) demo_reset(state)
//     else if (pin == PLAYPAUSE_BUTTON_PIN) demo_toggle_pause(state);
// }

void demo_update_storage(DemoState* state) {

    uint8_t t = state->t;
    uint8_t surplus = 0;
    uint8_t shortfall = 0;
    uint8_t supply = state->wind[t] + state->solar[t];

    if (supply >= state->demand[t]) {
        surplus = supply - state->demand[t];
    } else {
        shortfall = state->demand[t] - supply;
    }

    if (surplus) {
        state->storage_soc += surplus;
    } else if (state->storage_soc >= shortfall) {
        state->storage_soc -= shortfall;
    } else {
        state->storage_soc = 0;
    }

}

int64_t demo_advance(alarm_id_t id, void* user_data) {

    DemoState* state = user_data;

    if (state->paused) return 250000;

    uint8_t t = state->t;
    uint8_t date = t / 24 + 1;
    uint8_t time = t % 24;

    if (!(time % 12)) printf("Day\tTime\tDemand (MW)\tWind (MW)\tSolar (MW)\tReservoir (MWh)\n");

    demo_update_clockleds(state);

    demo_update_storage(state);
    demo_update_storageleds(state);

    printf("%d\t%d:00\t%d\t\t%d\t\t%d\t\t%d\n", date, time,
        state->demand[t], state->wind[t], state->solar[t],
        state->storage_soc);

    state->t = t+1 == N_PERIODS ? 0 : t+1;

    return 1000000;

}

#endif // demo_h_INCLUDED
