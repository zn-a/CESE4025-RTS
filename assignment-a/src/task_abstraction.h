#ifndef MY_ZEPHYR_APP_TASK_ABSTRACTION_H
#define MY_ZEPHYR_APP_TASK_ABSTRACTION_H

#include <zephyr/kernel/thread.h>
#include <zephyr/drivers/gpio.h>
#include "peripherals.h"
#include "scheduler.h"

extern k_tid_t scheduler_thread;

/// @brief Represents an Aperiodic Task and all additional information necessary to schedule the task.
typedef struct {
    int switch_number;
    enum SwitchState switch_state;
    unsigned int execution_time;
    const struct gpio_dt_spec *pin;
}AperiodicTask;

/// @brief Spawn a new instance of `Task`.
/// @param execution_time the execution time of the task (in ms)
/// @param period the period after which the task will spawn (in ms)
/// @param pin LED that the task sets to high when it is executing should be one of the options defined in `gpio.h`.
bool spawn_task(int execution_time, int period, const struct gpio_dt_spec *pin);

/// @brief Spawn a predefined Aperiodic task.
/// @param execution_time the execution time of the task (in ms)
/// @param pin LED that the task sets to high when it is executing should be one of the options defined in `gpio.h`.
void spawn_aperiodictask(unsigned int execution_time, const struct gpio_dt_spec *pin);

/// @brief Define a new instance of `Aperiodic_Task`.
/// @param switch_number the switch number that the task is associated with
/// @param switch_state the state of the switch that the task is associated with
/// @param execution_time the execution time of the task (in ms)
/// @param pin LED that the task sets to high when it is executing should be one of the options defined in `gpio.h`.
void define_aperiodic_task(int switch_number, enum SwitchState switch_state, unsigned int execution_time, const struct gpio_dt_spec *pin);

/// @brief Set the task that will be executed when the scheduler yields. This function is not blocking.
/// @param task_tid the task id of the task that should be executed next
void set_active_task(k_tid_t task_tid);

/// @brief This function will halt all the current task
void set_idle();

/// @brief Interrupt Handler function. Called when user_button is pressed
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

/// @brief Starts the main scheduler loop
/// @param type Select the which scheduler you want to use
void run_scheduler(SchedulerType type);

/// @brief This function can be used to choose which scheduler should be used.
/// @param type select the type of schedule to execute
/// @param t is an array of periodic tasks, of length `n`.
/// @param at is an array of aperiodic tasks.
/// @param n total number of periodic tasks
/// @param finished states whether schedule was called because the previous task was finished (true) or because the timer expired (false).
/// @return k_timeout_t - Returns the time in milliseconds until the next task should be executed.
k_timeout_t schedule(SchedulerType type, Task *t, Task *at,int n, bool finished);

#endif