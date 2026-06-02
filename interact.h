#ifndef interact_h_INCLUDED
#define interact_h_INCLUDED

typedef struct InteractiveState {
    uint8_t wind[NUM_WIND_TURBINES];
    uint8_t solar[NUM_PV_PANELS];
    bool caes;
    alarm_id_t caes_alarm;
    uint8_t t;
    uint8_t storage_soc;
    bool paused;
} InteractiveState;

InteractiveState interact = {
    .wind = { 0, 0, 0, 0, },
    .solar = { 0, 0, 0, 0, },
    .caes = false,
    .caes_alarm = 0,
    .t = 0,
    .storage_soc = 0,
    .paused = false,
};

int64_t interact_advance(alarm_id_t id, __unused void* user_data) {

    if (interact.paused) return 250000;

    uint8_t t = interact.t;
    uint8_t date = t / 24 + 1;
    uint8_t time = t % 24;

    if (!(time % 12)) printf("Day\tTime\tDemand (MW)\tWind (MW)\tSolar (MW)\tReservoir (MWh)\n");

    demo_update_clockleds(time, interact.solar[t]);

    // demo_update_storage();
    // demo_update_storageleds();

    printf("%d\t%d:00\t%d\t\t%d\t\t%d\t\t%d\n", date, time,
        0, interact.wind[t], interact.solar[t],
        interact.storage_soc);

    interact.t = t+1 == 24 ? 0 : t+1;

    return 1000000;

}

#endif // interact_h_INCLUDED
