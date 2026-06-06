#ifndef interact_h_INCLUDED
#define interact_h_INCLUDED

typedef struct InteractiveState {
    uint8_t wind[NUM_WIND_TURBINES];
    uint8_t solar[NUM_PV_PANELS];
    bool caes[NUM_CAES];
    alarm_id_t caes_alarm[NUM_CAES];
    uint8_t t;
    uint8_t storage_soc;
    bool paused;
} InteractiveState;

InteractiveState interact = {
    .wind = { 0, 0, 0, 0, },
    .solar = { 0, 0, 0, 0, },
    .caes = false,
    .caes_alarm = { 0, 0 },
    .t = 0,
    .storage_soc = 0,
    .paused = false,
};

int64_t caes_timeout(alarm_id_t id, __unused void* user_data) {
    size_t i = 0; // TODO: Handle both caes sensors
    interact.caes[i] = false;
    interact.caes_alarm[i] = 0;
    printf("CAES %d inactive\n", i);
    return 0;
}

void caes_handler(size_t i) {

    if (interact.caes[i]) {
        cancel_alarm(interact.caes_alarm[i]);
    } else {
        interact.caes[i] = true;
        printf("CAES %d active\n", i);
    }

    // TODO: Timeout appropriate CAES - via user_data
    interact.caes_alarm[i] = add_alarm_in_ms(1000, caes_timeout, NULL, false);

}

int64_t advance_interactive() {

    set_wind_read();

    uint8_t t = interact.t;
    uint8_t date = t / 24 + 1;
    uint8_t time = t % 24;

    demo_update_clockleds(time, interact.solar[t]);

    // demo_update_storage();
    // demo_update_storageleds();
    printf("PV Panels\t%d\t%d\t%d\t%d\n",
        get_generation(conf.gen_inputs.pv[0]),
        get_generation(conf.gen_inputs.pv[1]),
        get_generation(conf.gen_inputs.pv[2]),
        get_generation(conf.gen_inputs.pv[3]));

    printf("Wind Turbines\t%d\t%d\t%d\t%d\n",
        get_generation(conf.gen_inputs.wind[0]),
        get_generation(conf.gen_inputs.wind[1]),
        get_generation(conf.gen_inputs.wind[2]),
        get_generation(conf.gen_inputs.wind[3]));

    interact.t = t+1 == 24 ? 0 : t+1;

    return 1000000;

}

#endif // interact_h_INCLUDED
