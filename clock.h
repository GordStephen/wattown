#ifndef time_h_INCLUDED
#define time_h_INCLUDED

#define HOURS_PER_DAY 24

typedef enum TimeOfDay { nite, twil, day, num_timesofday} TimeOfDay;

const TimeOfDay timesofday[HOURS_PER_DAY] = {
    nite, nite, nite, nite, nite, twil,
    twil, day, day,  day,  day,  day,
    day,  day,  day,  day,  day, twil,
    twil, nite, nite, nite, nite, nite
};

typedef enum PixelType { ground, sky, orb, num_pixeltypes } PixelType;

const PixelType pixeltypes[HOURS_PER_DAY] = {
    ground, ground, ground, ground, ground, sky,
    sky,    sky,    sky,    sky,    sky,    sky,
    sky,    sky,    sky,    sky,    sky,    sky,
    sky, ground, ground, ground, ground, ground
};

uint32_t clock_base_colors[num_timesofday][num_pixeltypes] = {
    [nite] = { [orb] = 0x0004000F, [sky] = 0x00000000, [ground] = 0x00010101 },
    [twil] = { [orb] = 0x00000F00, [sky] = 0x00010400, [ground] = 0x00020100 },
    [day]  = { [orb] = 0x000F0F00, [sky] = 0x0000000F, [ground] = 0x00040000 },
};

void demo_init_clockleds() {

    pio_claim_free_sm_and_add_program_for_gpio_range(
        &ws2812_program,
        &conf.pios.clock.pio, &conf.pios.clock.sm, &conf.pios.clock.offset,
        conf.pins.pixels.clock, 1, true);

    ws2812_program_init(
        conf.pios.clock.pio, conf.pios.clock.sm, conf.pios.clock.offset,
        conf.pins.pixels.clock, 800000, false);

}

void demo_init_cityleds() {

    pio_claim_free_sm_and_add_program_for_gpio_range(
        &ws2812_program,
        &conf.pios.city.pio, &conf.pios.city.sm, &conf.pios.city.offset,
        conf.pins.pixels.city, 1, true);

    ws2812_program_init(
        conf.pios.city.pio, conf.pios.city.sm, conf.pios.city.offset,
        conf.pins.pixels.city, 800000, false);

}

void demo_update_clockleds(uint8_t time, uint8_t solarlevel) {

    TimeOfDay timeofday = timesofday[time];
    uint8_t brightness = ((uint16_t)solarlevel << 4) / 7;
    brightness || (brightness = 1);

    for (size_t p = 0; p < HOURS_PER_DAY; p += 1) {
        PixelType pixeltype = p == time ? orb : pixeltypes[p];
        uint32_t color = clock_base_colors[timeofday][pixeltype] * brightness;
        put_pixel(conf.pios.clock.pio, conf.pios.clock.sm, color);
    }

}

void demo_update_cityleds(uint8_t time, uint8_t demandlevel) {

    for (size_t p = 0; p < 99; p += 1) {

        uint32_t color = 0;

        if ((p < 4) ) {  // Uphill distribution lines
            // Do nothing for now
        } else if (p < 7) { // Wattown sign
            if (time < 6 || time > 17) color = 0x00884400;
        } else if (p < 94) { // City buildings
            color = 0x00010101 + 0x00010101 * (demandlevel - 1) * 36;
        } else { // Lowland distribution lines
            // Do nothing for now
        }

        put_pixel(conf.pios.city.pio, conf.pios.city.sm, color);
    }

}
#endif // time_h_INCLUDED
