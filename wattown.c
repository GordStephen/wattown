#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"

#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"

#include "ws2812.pio.h"
#include "ws2812.h"

#include "util.h"

#include "config.h"
#include "state.h"

#include "clock.h"
#include "wind.h"

#include "interact.h"
#include "demo.h"

// Storage state is mode-agnostic, abstract that code out of demo.h
// Define city lights agnostically as well
// Clock drawing is mode-agnostic and already abstracted

void init() {

    stdio_init_all(); // For logging over USB

    init_controls();
    demo_init_clockleds();
    demo_init_cityleds();
    init_storageleds();
    init_transmissionleds();
    init_pwm();

    init_windrelays();
    init_i2c(conf.pins.i2c.sda, conf.pins.i2c.scl);

    reset_state();

}

int64_t advance(alarm_id_t id, __unused void* user_data) {

    if (!is_running()) return 250000;

    state.t = (state.t + 1) % N_PERIODS;

    if (is_interactive()) advance_interactive();
    else advance_demo();

    return 1000000;

}

void irq_handler(uint pin, uint32_t event_mask) {
    if (pin == conf.pins.buttons.reset) {
        reset_state();
        reset_peripherals();
    }
    else if (pin == conf.pins.sensors.caes[0]) caes_handler(0);
    else if (pin == conf.pins.sensors.caes[1]) caes_handler(1);
}

int main() {

    init();

    add_alarm_in_ms(1000, advance, NULL, false);


    // TODO: Move these to global init and interactive-mode specific
    // interrupt setup
    gpio_set_irq_enabled(conf.pins.buttons.reset, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(conf.pins.sensors.caes[0], GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(conf.pins.sensors.caes[1], GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(irq_handler);
    irq_set_enabled(IO_IRQ_BANK0, true);

    while (true) {}

    pio_remove_program_and_unclaim_sm(
        &ws2812_program, conf.pios.clock.pio, conf.pios.clock.sm,
        conf.pios.clock.offset);

}
