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

#endif // time_h_INCLUDED
