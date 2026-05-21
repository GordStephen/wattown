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

#endif // util_h_INCLUDED
