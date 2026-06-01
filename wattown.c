#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"

#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"

#include "ws2812.pio.h"
#include "ws2812.h"

#include "config.h"
#include "util.h"
#include "clock.h"
#include "wind.h"
#include "interact.h"
#include "demo.h"

// Storage state is mode-agnostic, abstract that code out of demo.h
// Define city lights agnostically as well
// Clock drawing is mode-agnostic and already abstracted

// gpio_set_irq_callback(caes_handler);

int main() {

    // For logging over USB
    stdio_init_all();

    demo_init();
    demo_reset();

    alarm_id_t demo_alarm = add_alarm_in_ms(1000, demo_advance, NULL, false);
    alarm_id_t wind_alarm = add_alarm_in_us(sample_delay, turbine_advance, NULL, false);

    // TODO: Move these to global init and interactive-mode specific
    // interrupt setup
    init_button(conf.pins.sensors.caes);
    gpio_set_irq_enabled(conf.pins.sensors.caes, GPIO_IRQ_EDGE_FALL, true);

    gpio_set_irq_enabled(conf.pins.buttons.playpause, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(conf.pins.buttons.reset, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(demo_button_handler);
    irq_set_enabled(IO_IRQ_BANK0, true);

    while (true) {}

    pio_remove_program_and_unclaim_sm(
        &ws2812_program, conf.pios.clock.pio, conf.pios.clock.sm,
        conf.pios.clock.offset);

}
