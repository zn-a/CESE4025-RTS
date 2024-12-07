#ifndef MY_ZEPHYR_APP_SCHEDULER_H
#define MY_ZEPHYR_APP_SCHEDULER_H

#include <zephyr/kernel.h>

/// This represents a Task and all additional information necessary to schedule the task.
typedef struct {
    /// The id assigned to this task by Zephyr. Used internally by `set_active_task`.
    k_tid_t task_id;
    /// The maximal time the task will take to execute in ms.
    int execution_time;
    /// The scheduler should aim to execute this task every `period` ms.
    int period;
    /// The value calculated by the task
    int value;
    // Flag to check if task is active
    bool active;
    /// Feel free to add your own fields that are used for scheduling below.

    // following two added for rm
    int next_release_time;
    int remaining_time;

    // following added for edf
    int deadline;
} Task;


/// @enum This enum represents the different types of schedulers that can be used.
typedef enum{
    RM = 0,     ///< Rate Monotonic
    EDF = 1,    ///< Earliest Deadline First
    BS = 2,     ///< Background Scheduler
    SIE = 3,    ///< Schedule it Immediately
    EG = 4      ///< Example Scheduler
}SchedulerType;

/// @brief This is a very simple scheduler that demonstrates how the provided APIs can be used.
/// @brief The function should call `set_active_task` to select which task should be running.
/// @brief Then, it should return a sleep time in milliseconds, when the scheduler should next be executed.
/// @param tasks The array of tasks that should be scheduled.
/// @param n The number of tasks in the array.
/// @param finished A boolean that is true when the task finished execution
k_timeout_t example_scheduler(Task *tasks, int n, bool finished);


/// @brief In this function you should write a rate monotonic scheduler.
/// @param tasks The array of tasks that should be scheduled.
/// @param n The number of tasks in the array.
/// @param finished A boolean that is true when the task finished execution
k_timeout_t rate_monotonic(Task *tasks, int n, bool finished);


/// @brief In this function you should write an earliest deadline first scheduler.
/// @param tasks The array of tasks that should be scheduled.
/// @param n The number of tasks in the array.
/// @param finished A boolean that is true when the task finished execution
k_timeout_t earliest_deadline_first(Task *tasks, int n, bool finished);


/// @brief In this function you should write an background hybrid task scheduler.
/// @param tasks The array of tasks that should be scheduled.
/// @param aperiodicTask The array of aperiodic tasks that should be scheduled.
/// @param n The number of tasks in the array.
/// @param finished A boolean that is true when the task finished execution
k_timeout_t background_scheduler(Task *tasks, Task *aperiodicTask, int n, bool finished);

/// @brief In this function you should write an schedule it immediately hybrid task scheduler.
/// @param tasks The array of tasks that should be scheduled.
/// @param aperiodicTask The array of aperiodic tasks that should be scheduled.
/// @param n The number of tasks in the array.
/// @param finished A boolean that is true when the task finished execution
k_timeout_t schedule_immediately(Task *tasks, Task *aperiodicTask, int n, bool finished);


#endif