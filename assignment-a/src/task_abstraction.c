#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel/thread.h>
#include "gpio.h"
#include "task_main.h"
#include "stdbool.h"
#include "scheduler.h"
#include "peripherals.h"
#include "task_abstraction.h"

#define STACK_SIZE 512

// Note: Additional Stacks need to be defined in-case the value is increased
#define MAX_PERIODIC_THREADS 8
#define MAX_APERIODIC_THREADS 3

static int thread_counter = 0;
k_tid_t scheduler_thread = 0;

static AperiodicTask aperiodicTask[3];
static Task task_set[MAX_PERIODIC_THREADS];
static Task aperiodictask_set[MAX_APERIODIC_THREADS];
static k_tid_t current_task = NULL;

// Stack allocation for all the Periodic Tasks
K_THREAD_STACK_DEFINE(periodic_stack0, STACK_SIZE);
K_THREAD_STACK_DEFINE(periodic_stack1, STACK_SIZE);
K_THREAD_STACK_DEFINE(periodic_stack2, STACK_SIZE);
K_THREAD_STACK_DEFINE(periodic_stack3, STACK_SIZE);
K_THREAD_STACK_DEFINE(periodic_stack4, STACK_SIZE);
K_THREAD_STACK_DEFINE(periodic_stack5, STACK_SIZE);
K_THREAD_STACK_DEFINE(periodic_stack6, STACK_SIZE);
K_THREAD_STACK_DEFINE(periodic_stack7, STACK_SIZE);
// Stack Allocation for Aperiodic Tasks
K_THREAD_STACK_DEFINE(aperiodic_stack0, STACK_SIZE);
K_THREAD_STACK_DEFINE(aperiodic_stack1, STACK_SIZE);
K_THREAD_STACK_DEFINE(aperiodic_stack2, STACK_SIZE);

// Stack definition for all the threads
k_thread_stack_t *periodic_stacks[MAX_PERIODIC_THREADS] = {&periodic_stack0, &periodic_stack1, &periodic_stack2, &periodic_stack3, &periodic_stack4, &periodic_stack5, &periodic_stack6, &periodic_stack7};
k_thread_stack_t *aperiodic_stacks[MAX_APERIODIC_THREADS] = {&aperiodic_stack0, &aperiodic_stack1, &aperiodic_stack2};

// Thread definition for all the threads
struct k_thread ptsk_thread[MAX_PERIODIC_THREADS];
struct k_thread aptsk_thread[MAX_APERIODIC_THREADS];

// Callback function for interrupt
static struct gpio_callback button_cb_data;


bool spawn_task(int execution_time, int period, const struct gpio_dt_spec *pin) {
    Task task;

    task.execution_time = execution_time;
    task.period = period;
    task.value = 0;

    if (thread_counter == MAX_PERIODIC_THREADS) {
        printk("Spawned too many threads!\n");
        return false;
    }
    k_thread_stack_t *stack = periodic_stacks[thread_counter];
    struct k_thread *thread = &ptsk_thread[thread_counter];


    k_tid_t tid = k_thread_create(thread, stack, STACK_SIZE, (void *) task_main, (void *) pin, execution_time, &task_set[thread_counter].value, 5, 0,
                                  K_NO_WAIT);
    task.task_id = tid;
    task_set[thread_counter] = task;

    thread_counter++;

    k_thread_suspend(tid);
    printk("Spawned Task-> Execution Time: %d Period: %d\n", execution_time, period);
    return true;
}

void define_aperiodic_task(int switch_number, enum SwitchState switch_state, unsigned int execution_time, const struct gpio_dt_spec *pin){
    if(switch_number > 2 || switch_number < 0){
        printk("Illegal Switch Number!\n");
        return;
    }
    for(int i = 0; i< MAX_APERIODIC_THREADS; i++){
        if(aperiodicTask[i].pin == NULL){
            aperiodicTask[i].switch_number = switch_number;
            aperiodicTask[i].switch_state = switch_state;
            aperiodicTask[i].execution_time = execution_time;
            aperiodicTask[i].pin = pin;
            break;
        }
    }
}

void spawn_aperiodictask(unsigned int execution_time, const struct gpio_dt_spec *pin) {
    Task task;

    task.execution_time = execution_time;
    task.value = 0;

    int inactive_task = -1;
    for(int i = 0; i < MAX_APERIODIC_THREADS; i++){
        if(aperiodictask_set[i].active == false){
            inactive_task = i;
            break;
        }
    }
    // If all the aperiodic tasks are active, return
    // Note: Do not add a printk statement here, since this is called by a interrupt handler
    if(inactive_task == -1){
        return;
    }

    k_thread_stack_t *stack = aperiodic_stacks[inactive_task];
    struct k_thread *thread = &aptsk_thread[inactive_task];


    k_tid_t tid = k_thread_create(thread, stack, STACK_SIZE, (void *) task_main, (void *) pin, execution_time, &aperiodictask_set[inactive_task].value, 5, 0,
                                  K_NO_WAIT);
    task.task_id = tid;
    aperiodictask_set[inactive_task] = task;
    aperiodictask_set[inactive_task].active = true;

    k_thread_suspend(tid);
}


void set_active_task(k_tid_t task_tid) {
    if (current_task != NULL) {
        k_thread_suspend(current_task);
    }
    current_task = task_tid;
    k_thread_resume(current_task);
}

void set_idle() {
    if (current_task != NULL) {
        k_thread_suspend(current_task);
    }
    current_task = NULL;
}

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins){
    peripherals_update();
    for(int i = 0; i < MAX_APERIODIC_THREADS; i++){
        if(switches[aperiodicTask[i].switch_number] == aperiodicTask[i].switch_state){
            spawn_aperiodictask(aperiodicTask[i].execution_time, aperiodicTask[i].pin);
            printk("Spawned Aperiodic Task %d\n", i);
        }
    }
}

void run_scheduler(SchedulerType type) {
    int num_tasks = thread_counter;
    scheduler_thread = k_current_get();

    // Setting up the button interrupt
    gpio_pin_interrupt_configure_dt(&button,GPIO_INT_EDGE_RISING);
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    bool finished = false;
    while (true) {
        reset_as();
        k_timeout_t sleep = schedule(type, &task_set, &aperiodictask_set, num_tasks, finished);
        finished = k_sleep(sleep) != 0;
    }
}

k_timeout_t schedule(SchedulerType type, Task *tasks, Task *aperiodicTask,int n, bool finished) {
    // Call the scheduler based on which one is selected
    switch(type){
        case RM:
            return rate_monotonic(tasks, n, finished);
        case EDF:
            return earliest_deadline_first(tasks, n, finished);
        case BS:
            return background_scheduler(tasks, aperiodicTask, n, finished);
        case SIE:
            return schedule_immediately(tasks, aperiodicTask, n, finished);
        default:
            return example_scheduler(tasks, n, finished);
    }
}