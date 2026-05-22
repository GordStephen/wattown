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

const uint8_t adc_channel_cmd[8] = {
    0x83, 0xC3, 0x93, 0xD3, 0xA3, 0xE3, 0xB3, 0xF3
};

uint8_t get_generation(ADCAddress adc) {
    uint8_t result = 0;
    i2c_write_blocking(i2c_default, adc.addr, &adc_channel_cmd[adc.ch], 1, false);
    i2c_read_blocking(i2c_default, adc.addr, &result, 1, false);
    return result;
}

#endif // util_h_INCLUDED
