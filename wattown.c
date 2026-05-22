#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"

#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#include "ws2812.pio.h"
#include "ws2812.h"

#include "config.h"
#include "util.h"
#include "clock.h"
#include "demo.h"

int main() {

    // For logging over USB
    stdio_init_all();

    // For reading generator powers from ADC boards
    i2c_init(i2c_default, 48000);

    demo_init();
    demo_reset();

    alarm_id_t demo_alarm = add_alarm_in_ms(1000, demo_advance, NULL, false);

    gpio_set_irq_enabled(conf.pins.buttons.playpause, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(conf.pins.buttons.reset, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(demo_button_handler);
    irq_set_enabled(IO_IRQ_BANK0, true);

    while (true) {}

    pio_remove_program_and_unclaim_sm(
        &ws2812_program, conf.pios.clock.pio, conf.pios.clock.sm,
        conf.pios.clock.offset);

}
