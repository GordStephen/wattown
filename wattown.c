#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"

#define N_PERIODS 96

int main() {

    stdio_init_all();


    const uint8_t solar[N_PERIODS] = {
        0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 3, 3, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 0, 0, 0, 0, 0, 0
    };

    const uint8_t wind[N_PERIODS] = {
        5, 4, 5, 6, 7, 6, 5, 4, 3, 4, 5, 6, 5, 6, 5, 4, 3, 4, 5, 6, 7, 7, 7, 6,
        5, 4, 3, 2, 1, 2, 1, 0, 1, 2, 1, 2, 3, 2, 3, 2, 1, 0, 1, 0, 1, 2, 3, 2,
        5, 4, 5, 6, 7, 6, 5, 4, 3, 4, 5, 6, 5, 6, 5, 4, 3, 4, 5, 6, 7, 7, 7, 6,
        5, 4, 3, 2, 1, 2, 1, 0, 1, 2, 1, 2, 3, 2, 3, 2, 1, 0, 1, 0, 1, 2, 3, 2
    };

    const uint8_t demand[N_PERIODS] = {
        3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3,
        3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3,
        3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3,
        3, 2, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 5, 5, 6, 6, 7, 6, 5, 4, 3
    };

    uint8_t storage_soc = 0;

    while (true) {

        for (uint8_t t = 0; t < N_PERIODS; t += 1) {

            uint8_t date = t / 24 + 1;
            uint8_t time = t % 24;

            if (!(t % 12)) printf("Day\tTime\tDemand (MW)\tWind (MW)\tSolar (MW)\tReservoir (MWh)\n");

            uint8_t surplus = 0;
            uint8_t shortfall = 0;
            uint8_t supply = wind[t] + solar[t];

            if (supply >= demand[t]) {
                surplus = supply - demand[t];
            } else {
                shortfall = demand[t] - supply;
            }

            if (surplus) {
                storage_soc += surplus;
            } else if (storage_soc >= shortfall) {
                storage_soc -= shortfall;
            } else {
                storage_soc = 0;
            }

            printf("%d\t%d:00\t%d\t\t%d\t\t%d\t\t%d\n", date, time,
                demand[t], wind[t], solar[t], storage_soc);

            sleep_ms(1000);

        }
    }

}
