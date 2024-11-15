#include "task_abstraction.h"
#include "scheduler.h"
#include <zephyr/kernel.h>
#include "stdbool.h"
#include "gpio.h"


static int active_task = 0;
k_timeout_t example_scheduler(Task *tasks, int n, bool finished) {
    set_active_task(tasks[active_task].task_id);
    active_task = (active_task + 1) % n;
    return K_FOREVER;
}

k_timeout_t rate_monotonic(Task *tasks, int n, bool finished) {
    return K_FOREVER;
}

k_timeout_t earliest_deadline_first(Task *tasks, int n, bool finished) {
    return K_FOREVER;
}

k_timeout_t background_scheduler(Task *tasks, Task *aperiodicTask, int n, bool finished){
    return K_FOREVER;
}

k_timeout_t schedule_immediately(Task *tasks, Task *aperiodicTask, int n, bool finished){
    return K_FOREVER;
}

