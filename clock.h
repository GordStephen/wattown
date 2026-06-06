#ifndef time_h_INCLUDED
#define time_h_INCLUDED

#define HOURS_PER_DAY 24
#define NUM_CLOCKPIXELS 92

// 4 days:
// Green (wind + sun high), red (sun high), blue (wind high), white (neither high)
uint32_t daycolors[4] = { 0x00080000, 0x00000800, 0x000008, 0x00040404 };

const bool moonpixels[92] = {

    // Ring 5 (0-31)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0-15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16-31

    // Ring 4 (32-55)
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1,

    // Ring 3 (56-71)
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,

    // Ring 4 (72-83)
    0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,

    // Ring 5 (84-91)
    0, 0, 0, 0, 0, 0, 0, 0

};

void draw_moon_clock(uint8_t t) {

    uint8_t day = t / HOURS_PER_DAY; // 0-3
    uint8_t time = t % HOURS_PER_DAY; // 0-23

    uint8_t clock_pixel = (uint16_t)time * 32 / HOURS_PER_DAY;
    uint32_t clock_color = daycolors[day];

    uint8_t brightness = 8;

    if (time < 6) {
        brightness = 7 - time;
    } else if (time > 17) {
        brightness = time - 16;
    }

    for (size_t p = 0; p < NUM_CLOCKPIXELS; p += 1) {

        bool in_clock = clock_pixel == p;
        bool in_moon = moonpixels[p];

        uint32_t color = in_moon * brightness * 0x00000102 +
                         !in_moon * in_clock * clock_color;

        put_pixel(conf.pios.clock.pio, conf.pios.clock.sm, color);

    }

}

typedef enum SunPixel { core, inner_ray, mid_ray, outer_ray, off } SunPixel;

const SunPixel sunpixels[92] = {

    // Ring 5 (0-31)
    outer_ray, off, off, off, outer_ray, off, off, off, // 0-7
    outer_ray, off, off, off, outer_ray, off, off, off, // 8-15
    outer_ray, off, off, off, outer_ray, off, off, off, // 16-23
    outer_ray, off, off, off, outer_ray, off, off, off, // 24-31

    // Ring 4 (32-55)
    mid_ray, off, off, mid_ray, off, off, // 32-37
    mid_ray, off, off, mid_ray, off, off, // 38-43
    mid_ray, off, off, mid_ray, off, off, // 44-49
    mid_ray, off, off, mid_ray, off, off,  // 50-55

    // Ring 3 (56-71)
    inner_ray, off, inner_ray, off, inner_ray, off, inner_ray, off, // 56-63
    inner_ray, off, inner_ray, off, inner_ray, off, inner_ray, off, // 64-71

    // Ring 2 (72-83)
    core, core, core, core, core, core, core, core, core, core, core, core,

    // Ring 1 (84-91)
    core, core, core, core, core, core, core, core

};

void draw_sun_clock(uint8_t t, uint8_t solarlevel) {

    uint8_t day = t / 24; // 0-3
    uint8_t time = t % 24; // 0-23

    uint8_t brightness = solarlevel + 1; // 1-8
    uint8_t raylevel = solarlevel / 2; // 0-3

    uint8_t clock_pixel = (uint16_t)time * 32 / 24;
    uint32_t clock_color = daycolors[day];

    for (size_t p = 0; p < NUM_CLOCKPIXELS; p += 1) {

        bool in_clock = clock_pixel == p;

        SunPixel sunpixel = sunpixels[p];
        bool in_sun = sunpixel <= raylevel;

        uint32_t color = !in_clock * in_sun * brightness * 0x00010200 +
                         in_clock * clock_color;

        put_pixel(conf.pios.clock.pio, conf.pios.clock.sm, color);

    }

}

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

void demo_update_clockleds(uint8_t t, uint8_t solarlevel) {

    uint8_t time = t % 24;

    if (time < 6 || time >= 18) draw_moon_clock(t);
    else draw_sun_clock(t, solarlevel);

}

void demo_update_cityleds(uint8_t t, uint8_t demandlevel) {

    uint8_t time = t % 24;

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
