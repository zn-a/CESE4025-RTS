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

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int lcm(int a, int b) {
    return (a / gcd(a, b)) * b;
}

int calculateHyperperiod(Task *tasks, int n) {
    int hyperperiod = tasks[0].period;
    for (int i = 1; i < n; i++) {
        hyperperiod = lcm(tasks[i].period, hyperperiod);
    }
    return hyperperiod;
}


static Task *current_task = NULL;
static uint32_t last_scheduler_time = 0;

k_timeout_t rate_monotonic(Task *tasks, int n, bool finished) {
    uint32_t current_time = k_uptime_get();
    uint32_t elapsed_time = current_time - last_scheduler_time;
    last_scheduler_time = current_time;

    if (current_task != NULL) {
        if (elapsed_time >= current_task->remaining_time) {
            current_task->remaining_time = 0;
            finished = true;
        } else {
            current_task->remaining_time -= elapsed_time;
            finished = false;
        }
    }

    // All tasks arrive at the same time (time = 0), this bit is to update the next release time because the task
    // has to repeat before its next period (next release time)
    for (int i = 0; i < n; i++) {
        if (current_time >= tasks[i].next_release_time) {
            tasks[i].remaining_time = tasks[i].execution_time;
            tasks[i].next_release_time += tasks[i].period;
        }
    }

    // We loop through the tasks and check if they are unfinished
    // we take the task with the highest priority that is unfinished.

    Task *next_task = NULL;
    int max = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (tasks[i].remaining_time > 0 && tasks[i].period < max) {
            max = tasks[i].period;
            next_task = &tasks[i];
        }
    }

    // Decide whether to preempt the current task
    // If the remaining time is 0 finished is true and another task should be set
    // If there is a next task replace it and there is either no current task or the next task has a shorter period
    // than the current task we set the next task as the current ask.
    if (finished || (next_task != NULL && (current_task == NULL || next_task->period < current_task->period))) {
        if (current_task != next_task) {
            current_task = next_task;
            if (current_task != NULL) {
                set_active_task(current_task->task_id);
            }
        }
    }

    // Calculate the delay until the next event
    // we return this so the scheduler can sleep this amount of time if need be
    uint32_t next_release_time = UINT32_MAX;

    // Time until current task completion
    if (current_task != NULL) {
        if (current_task->remaining_time < next_release_time) {
            next_release_time = current_task->remaining_time;
        }
    }

    // Time until next task release
    for (int i = 0; i < n; i++) {
        uint32_t time_until_release = tasks[i].next_release_time - current_time;
        if (time_until_release < next_release_time) {
            next_release_time = time_until_release;
        }
    }

    // If no task is ready, suspend the current thread, set_idle() is a wrapper that handles this
    if (next_task == NULL) {
        set_idle();
    }

    return K_MSEC(next_release_time);
}

k_timeout_t earliest_deadline_first(Task *tasks, int n, bool finished) {
    uint32_t current_time = k_uptime_get();
    uint32_t elapsed_time = current_time - last_scheduler_time;
    last_scheduler_time = current_time;

    if (current_task != NULL) {
        if (elapsed_time >= current_task->remaining_time) {
            current_task->remaining_time = 0;
            finished = true;
        } else {
            current_task->remaining_time -= elapsed_time;
            finished = false;
        }
    }

    // All tasks arrive at the same time (time = 0), this bit is to update the next release time because the task
    // has to repeat before its next period (next release time)
    for (int i = 0; i < n; i++) {
        if (current_time >= tasks[i].next_release_time) {
            tasks[i].remaining_time = tasks[i].execution_time;
            tasks[i].next_release_time += tasks[i].period;
            tasks[i].deadline += tasks[i].period;
        }
    }

    // We loop through the tasks and check if they are unfinished
    // we take the task with the highest priority that is unfinished.

    Task *next_task = NULL;
    int max = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (tasks[i].remaining_time > 0 && tasks[i].deadline < max) {
            max = tasks[i].deadline;
            next_task = &tasks[i];
        }
    }

    // Decide whether to preempt the current task
    // If the remaining time is 0 finished is true and another task should be set
    // If there is a next task replace it and there is either no current task or the next task has a shorter period
    // than the current task we set the next task as the current ask.
    if (finished || (next_task != NULL && (current_task == NULL || next_task->deadline < current_task->deadline))) {
        if (current_task != next_task) {
            current_task = next_task;
            if (current_task != NULL) {
                set_active_task(current_task->task_id);
            }
        }
    }

    // Calculate the delay until the next event
    // we return this so the scheduler can sleep this amount of time if need be
    uint32_t next_release_time = UINT32_MAX;

    // Time until current task completion
    if (current_task != NULL) {
        if (current_task->remaining_time < next_release_time) {
            next_release_time = current_task->remaining_time;
        }
    }

    // Time until next task release
    for (int i = 0; i < n; i++) {
        uint32_t time_until_release = tasks[i].next_release_time - current_time;
        if (time_until_release < next_release_time) {
            next_release_time = time_until_release;
        }
    }

    // If no task is ready, suspend the current thread, set_idle() is a wrapper that handles this
    if (next_task == NULL) {
        set_idle();
    }

    return K_MSEC(next_release_time);
}

k_timeout_t background_scheduler(Task *tasks, Task *aperiodicTask, int n, bool finished){
    return K_FOREVER;
}

k_timeout_t schedule_immediately(Task *tasks, Task *aperiodicTask, int n, bool finished){
    return K_FOREVER;
}

