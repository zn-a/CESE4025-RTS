#include <stdbool.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include "gpio.h"

const struct gpio_dt_spec led_board_0 = GPIO_DT_SPEC_GET(DT_NODELABEL(board_led0), gpios);
const struct gpio_dt_spec led_board_1 = GPIO_DT_SPEC_GET(DT_NODELABEL(board_led1), gpios);
const struct gpio_dt_spec led_board_2 = GPIO_DT_SPEC_GET(DT_NODELABEL(board_led2), gpios);
const struct gpio_dt_spec led_board_3 = GPIO_DT_SPEC_GET(DT_NODELABEL(board_led3), gpios);
const struct gpio_dt_spec led_synth_0 = GPIO_DT_SPEC_GET(DT_NODELABEL(synth_led0), gpios);
const struct gpio_dt_spec led_synth_1 = GPIO_DT_SPEC_GET(DT_NODELABEL(synth_led1), gpios);
const struct gpio_dt_spec led_synth_2 = GPIO_DT_SPEC_GET(DT_NODELABEL(synth_led2), gpios);
const struct gpio_dt_spec led_synth_3 = GPIO_DT_SPEC_GET(DT_NODELABEL(synth_led3), gpios);
const struct gpio_dt_spec led_synth_4 = GPIO_DT_SPEC_GET(DT_NODELABEL(synth_led4), gpios);
const struct gpio_dt_spec *leds[8] = {&led_board_0, &led_board_1, &led_board_2, &led_board_3, &led_synth_0, &led_synth_1,
                                     &led_synth_2, &led_synth_3};
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_NODELABEL(button), gpios);

int init_pins() {    
    for (int i = 0; i < 8; i++) {
        if (!gpio_is_ready_dt(leds[i])) {
            printk("Error initializing LED.\n");
            return -1;
        }

        if (!device_is_ready(button.port)) {
            printk("Error initializing Button.\n");
            return -1;
        }
        gpio_pin_configure_dt(leds[i], GPIO_OUTPUT_ACTIVE);
        gpio_pin_configure_dt(&button, GPIO_INPUT);
        gpio_pin_set_dt(leds[i], false);
    }
    return 0;
}

int set_pin(const struct gpio_dt_spec *led) {
    gpio_pin_set_dt(led, true);
    return 0;
}

int unset_pin(const struct gpio_dt_spec *led) {
    gpio_pin_set_dt(led, false);
    return 0;
}

void reset_as() {
    // Set leds correctly
    for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) {
        unset_pin(leds[i]);
    }
}