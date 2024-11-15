
#ifndef MY_ZEPHYR_APP_PERIPHERALS_H
#define MY_ZEPHYR_APP_PERIPHERALS_H


enum SwitchState  { Down = -1, Neutral = 0, Up = 1 };

extern enum SwitchState switches[3];

/// @brief Peripherals initialization function
/// Call this function during initialization before calling the rest of the fuctions
/// @return 0 on success, -ERRNO otherwise
int init_peripherals();

/// @brief Call this function to update the encoders and switches
/// @return 0 on success, -ERRNO otherwise
int peripherals_update();


#endif //MY_ZEPHYR_APP_PERIPHERALS_H
