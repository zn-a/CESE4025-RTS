#ifndef MY_ZEPHYR_APP_GPIO_H
#define MY_ZEPHYR_APP_GPIO_H

#include <zephyr/drivers/gpio.h>

extern const struct gpio_dt_spec led_board_0;
extern const struct gpio_dt_spec led_board_1;
extern const struct gpio_dt_spec led_board_2;
extern const struct gpio_dt_spec led_board_3;
extern const struct gpio_dt_spec led_synth_0;
extern const struct gpio_dt_spec led_synth_1;
extern const struct gpio_dt_spec led_synth_2;
extern const struct gpio_dt_spec led_synth_3;
extern const struct gpio_dt_spec led_synth_4;
extern const struct gpio_dt_spec *leds[];
extern const struct gpio_dt_spec button;

/// @brief Initialize the LEDs and the User Button
/// @return int - Returns 0 if the initialization is successful, -1 otherwise
int init_pins();

/// @brief Set the LED to On
/// @param led - The LED to be set
/// @return int - Returns 0 if the initialization is successful, -1 otherwise
int set_pin(const struct gpio_dt_spec *led);

/// @brief Set the LED to Off
/// @param led - The LED to be unset
/// @return int - Returns 0 if the initialization is successful, -1 otherwise
int unset_pin(const struct gpio_dt_spec *led);

/// @brief Unset all the LEDs
void reset_as();

#endif