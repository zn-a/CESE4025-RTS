#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "gpio.h"
#include "task_abstraction.h"

void task_main(const struct gpio_dt_spec *pin, int execution_time, int* thread_value) {
    int64_t task_value = 0;
    while(true) {
        // To convert ms into ticks multiply by 30, since ticks/sec = 30,000
        int64_t target = k_uptime_ticks() + (execution_time*30);
        int64_t last = k_uptime_ticks();
        while (true) {
            // Set leds correctly
            set_pin(pin);

            int64_t current = k_uptime_ticks();

            // Task was preempted, we don't count this as execution time
            if (current - last > 10) {
                target += (current - last);
            }
            last = current;

            // Task Action
            *thread_value = *thread_value + 1;

            // If this task finished, break the loop
            if (current >= target) {
                break;
            }
        }
        unset_pin(pin);
        k_wakeup(scheduler_thread);
    }
}
