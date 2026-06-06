#ifndef config_h_INCLUDED
#define config_h_INCLUDED

#define N_PERIODS 96
#define HOURS_PER_DAY 24

#define NUM_WIND_TURBINES 4
#define NUM_PV_PANELS 4
#define NUM_CAES 2

#define NUM_STORAGE_LEDS 4

typedef struct PIOConfig {
    PIO pio;
    uint sm;
    uint offset;
} PIOConfig;

// UART defaults (unused): 0 (TX), 1 (RX)
// I2C defaults: 4 (SDA), 5 (SCL)
// SPI defaults:  16 (RX), 17 (CSn), 18 (SCK), 19 (TX)

struct {

    const struct {

        struct {
            uint playpause;
            uint modeselect;
            uint reset;
        } buttons;

        struct {
            uint clock;
            uint city;
        } pixels;

        struct {
            uint playpause;
            uint storage[NUM_STORAGE_LEDS];
            uint wind_tx;
            uint import_tx;
        } leds;

        struct {
            uint caes[NUM_CAES];
        } sensors;

        struct {
            uint sda;
            uint scl;
        } i2c;

        struct {
            uint wind1;
            uint wind2;
        } pwm;

        struct {
            uint wind[NUM_WIND_TURBINES];
        } relays;

    } pins;

    struct {
        ADCAddress wind[NUM_WIND_TURBINES];
        ADCAddress pv[NUM_PV_PANELS];
    } gen_inputs;

    struct {
        PIOConfig clock;
        PIOConfig city;
    } pios;

} conf = {

    .pins = {

        .buttons = {
            .playpause = 0,
            .modeselect = 1,
            .reset = 27,
        },

        .pixels = {
            .clock = 16,
            .city = 3,
        },

        .leds = {
            .storage = { 10, 11 , 12, 13 },
            .wind_tx = 14,
            .import_tx = 15,
        },

        .sensors = {
            .caes = { 8, 9 },
        },

        .i2c = {
            .sda = 4,
            .scl = 5,
        },

        .pwm = {
            .wind1 = 6,
            .wind2 = 7,
        },

        .relays = {
            .wind = { 18, 19, 20, 21 },
        },

    },


    // While not listed here, the I2C for these runs off of GPIO pins 4 and 5
    // Default ADC board address is 0x48, 0x49-0x51 can available too with jumpers
    // TODO: Adjust these based on how things are wired
    .gen_inputs = {

        .wind = {
            { .addr = 0x48, .ch = 4 },
            { .addr = 0x48, .ch = 5 },
            { .addr = 0x48, .ch = 6 },
            { .addr = 0x48, .ch = 7 },
        },

        .pv = {
            { .addr = 0x48, .ch = 0 },
            { .addr = 0x48, .ch = 1 },
            { .addr = 0x48, .ch = 2 },
            { .addr = 0x48, .ch = 3 },
        },

    },

    .pios = {
        .clock = { 0 }, // to be initialized
        .city= { 0 } // to be initialized
    }

};

#endif // config_h_INCLUDED
