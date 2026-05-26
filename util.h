#ifndef util_h_INCLUDED
#define util_h_INCLUDED

void init_led(uint pin, bool state) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, state);
}

void init_button(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
}

void init_i2c() {

    uint sda_pin = conf.pins.i2c.sda;
    uint scl_pin = conf.pins.i2c.scl;

    i2c_init(i2c_default, 100 * 1000);

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

}

const uint8_t adc_channel_cmd[8] = {
    0x83, 0xC3, 0x93, 0xD3, 0xA3, 0xE3, 0xB3, 0xF3
};

uint8_t get_generation(ADCAddress adc) {

    int txsize = 0;
    uint8_t result = 0;

    txsize = i2c_write_blocking(i2c_default, adc.addr, &adc_channel_cmd[adc.ch], 1, false);
    if (txsize == PICO_ERROR_GENERIC) {
        printf("Error writing to ADC: address %d not acknowledged\n", adc.addr);
    }

    txsize = i2c_read_blocking(i2c_default, adc.addr, &result, 1, false);
    if (txsize == PICO_ERROR_GENERIC) {
        printf("Error reading from ADC: address %d not acknowledged\n", adc.addr);
    }

    return result;
}

#endif // util_h_INCLUDED
