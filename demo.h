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

DemoState demo = {
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

void demo_init_controls() {
    init_button(conf.pins.buttons.playpause);
    init_button(conf.pins.buttons.reset);
    init_led(conf.pins.leds.playpause, true);
}

void demo_init_storageleds() {
    for (size_t i = 0; i < NUM_STORAGE_LEDS; i += 1) {
        init_led(conf.pins.leds.storage[i], false);
    }
}

void demo_init() {
    demo_init_controls();
    demo_init_clockleds();
    demo_init_storageleds();
}

void demo_update_storageleds() {
    // TODO: PWM for brightness levels instead of on / off
    const uint8_t storage_incr = 15;
    for (size_t i = 0; i < NUM_STORAGE_LEDS; i += 1) {
        gpio_put(conf.pins.leds.storage[i], demo.storage_soc > (i + 1) * storage_incr);
    }
}

void demo_toggle_pause() {

    demo.paused ^= 1;

    if (demo.paused) {
        gpio_put(conf.pins.leds.playpause, false);
        printf("[Simulation paused]\n");
    } else {
        gpio_put(conf.pins.leds.playpause, true);
        printf("[Simulation resumed]\n");
    }

}

void demo_reset() {
    demo.t = 0;
    demo.storage_soc = 0;
    demo_update_storageleds();
    demo_update_clockleds(0, demo.solar[0]);
    printf("[Simulation reset]\n");
}

void demo_button_handler(uint pin, uint32_t event_mask) {
    if (pin == conf.pins.buttons.reset) demo_reset();
    else if (pin == conf.pins.buttons.playpause) demo_toggle_pause();
}

void demo_update_storage() {

    uint8_t t = demo.t;
    uint8_t surplus = 0;
    uint8_t shortfall = 0;
    uint8_t supply = demo.wind[t] + demo.solar[t];

    if (supply >= demo.demand[t]) {
        surplus = supply - demo.demand[t];
    } else {
        shortfall = demo.demand[t] - supply;
    }

    if (surplus) {
        demo.storage_soc += surplus;
    } else if (demo.storage_soc >= shortfall) {
        demo.storage_soc -= shortfall;
    } else {
        demo.storage_soc = 0;
    }

}

int64_t demo_advance(alarm_id_t id, __unused void* user_data) {

    if (demo.paused) return 250000;

    uint8_t t = demo.t;
    uint8_t date = t / 24 + 1;
    uint8_t time = t % 24;

    if (!(time % 12)) printf("Day\tTime\tDemand (MW)\tWind (MW)\tSolar (MW)\tReservoir (MWh)\n");

    demo_update_clockleds(time, demo.solar[t]);

    demo_update_storage();
    demo_update_storageleds();

    printf("%d\t%d:00\t%d\t\t%d\t\t%d\t\t%d\n", date, time,
        demo.demand[t], demo.wind[t], demo.solar[t],
        demo.storage_soc);

    demo.t = t+1 == N_PERIODS ? 0 : t+1;

    return 1000000;

}

#endif // demo_h_INCLUDED
