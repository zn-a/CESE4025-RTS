
#include <zephyr/drivers/gpio.h>

#include "peripherals.h"

// GPIOs
const struct gpio_dt_spec sw1_dn = GPIO_DT_SPEC_GET(DT_ALIAS(switch4), gpios);
const struct gpio_dt_spec sw1_up = GPIO_DT_SPEC_GET(DT_ALIAS(switch5), gpios);
const struct gpio_dt_spec sw2_dn = GPIO_DT_SPEC_GET(DT_ALIAS(switch6), gpios);
const struct gpio_dt_spec sw2_up = GPIO_DT_SPEC_GET(DT_ALIAS(switch7), gpios);
const struct gpio_dt_spec sw3_dn = GPIO_DT_SPEC_GET(DT_ALIAS(switch2), gpios);
const struct gpio_dt_spec sw3_up = GPIO_DT_SPEC_GET(DT_ALIAS(switch3), gpios);

//Switches
enum SwitchState switches[3];

int init_peripherals(){
    for(int i=0; i<3; i++){
        switches[i] = Neutral;
    }
    return 0;
}

int peripherals_update(){
    // Update the switches
    bool up, dn;

    up = gpio_pin_get_dt(&sw1_up);
    dn = gpio_pin_get_dt(&sw1_dn);
    if (up == dn) {
        switches[0] = Neutral;
    } else if (up) {
        switches[0] = Up;
    } else {
        switches[0] = Down;
    }

    up = gpio_pin_get_dt(&sw2_up);
    dn = gpio_pin_get_dt(&sw2_dn);
    if (up == dn) {
        switches[1] = Neutral;
    } else if (up) {
        switches[1] = Up;
    } else {
        switches[1] = Down;
    }

    up = gpio_pin_get_dt(&sw3_up);
    dn = gpio_pin_get_dt(&sw3_dn);
    if (up == dn) {
        switches[2] = Neutral;
    } else if (up) {
        switches[2] = Up;
    } else {
        switches[2] = Down;
    }

    return 0;
}