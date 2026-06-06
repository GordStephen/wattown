#ifndef demo_h_INCLUDED
#define demo_h_INCLUDED

void init_controls() {
    init_toggle(conf.pins.buttons.playpause);
    init_toggle(conf.pins.buttons.modeselect);
    init_button(conf.pins.buttons.reset);
}

void init_storageleds() {
    for (size_t i = 0; i < NUM_STORAGE_LEDS; i += 1) {
        init_led(conf.pins.leds.storage[i], false);
    }
}

void update_storageleds() {
    const uint8_t storage_incr = 15;
    for (size_t i = 0; i < NUM_STORAGE_LEDS; i += 1) {
        gpio_put(conf.pins.leds.storage[i], state.storage_soc > (i + 1) * storage_incr);
    }
}

void init_transmissionleds() {
    init_led(conf.pins.leds.wind_tx, false);
    init_led(conf.pins.leds.import_tx, false);
}

void update_transmissionleds() {
    gpio_put(conf.pins.leds.wind_tx, state.wind[state.t] > 2);
    gpio_put(conf.pins.leds.import_tx, state.wind[state.t] > 2);
}

void reset_peripherals() {
    update_storageleds();
    update_transmissionleds();
    demo_update_clockleds(0, state.solar[0]);
}

bool is_running() {
    return gpio_get(conf.pins.buttons.playpause);
}

bool is_interactive() {
    return gpio_get(conf.pins.buttons.modeselect);
}

void update_storage() {

    uint8_t t = state.t;
    uint8_t surplus = 0;
    uint8_t shortfall = 0;
    uint8_t supply = state.wind[t] + state.solar[t];

    if (supply >= state.demand[t]) {
        surplus = supply - state.demand[t];
    } else {
        shortfall = state.demand[t] - supply;
    }

    if (surplus) {
        state.storage_soc += surplus;
    } else if (state.storage_soc >= shortfall) {
        state.storage_soc -= shortfall;
    } else {
        state.storage_soc = 0;
    }

}

void advance_demo() {

    set_wind_write();

    bool wind_blowing = state.wind[state.t] > 2;

    if (wind_blowing && !state.wind_alarm) {
        state.wind_alarm =
            add_alarm_in_us(sample_delay, wind_advance, NULL, false);
    } else if (!wind_blowing && state.wind_alarm) {
        cancel_alarm(state.wind_alarm);
        stop_pwm();
        state.wind_alarm = 0;
        wind_reset();
    }

    uint8_t t = state.t;
    uint8_t date = t / 24 + 1;
    uint8_t time = t % 24;

    if (!(time % 12)) printf("Day\tTime\tDemand (MW)\tWind (MW)\tSolar (MW)\tReservoir (MWh)\n");

    demo_update_clockleds(t, state.solar[t]);
    demo_update_cityleds(t, state.demand[t]);

    update_transmissionleds();

    update_storage();
    update_storageleds();

    printf("%d\t%d:00\t%d\t\t%d\t\t%d\t\t%d\n", date, time,
        state.demand[t], state.wind[t], state.solar[t],
        state.storage_soc);

}

#endif // demo_h_INCLUDED
