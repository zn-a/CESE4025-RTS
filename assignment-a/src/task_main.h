#ifndef MY_ZEPHYR_APP_TASK_MAIN_H
#define MY_ZEPHYR_APP_TASK_MAIN_H

#include <zephyr/drivers/gpio.h>

/// @brief The main task that will be executed by the scheduler
/// @param pin the pin that the task is associated with (LED)
/// @param execution_time the execution time of the task (in ms)
/// @param thread_value the value that the task will set
void task_main(const struct gpio_dt_spec *pin, int execution_time, int* thread_value);

#endif