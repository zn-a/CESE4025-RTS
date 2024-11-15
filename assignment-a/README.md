# Assignment A: Coding Schedulers
In this assignment you will be implementing and testing several schedulers for a real-time system on actual hardware (STM32F4DISCOVERY).

<!-- TODO: Update Lecture links for the latest brightspace lectures -->
You will use Zephyr RTOS to implement RM (rate-monotonic), EDF (earliest-deadline-first) and FP (fixed priority) aperiodic schedulers. RM, EDF and FP servers are scheduling algorithms discussed in the [Lecture 6](https://brightspace.tudelft.nl/d2l/le/content/594635/viewContent/3569466/View), [Lecture 7](https://brightspace.tudelft.nl/d2l/le/content/594635/viewContent/3277146/View), and [Lecture 8](https://brightspace.tudelft.nl/d2l/le/content/594635/viewContent/3277164/View) respectively. In this assignment you will put this theory into practice. The resulting schedules must then be plotted and analyzed by using a logic analyzer.

## Learning Objectives
Considering the learning objectives of the course, this assignment (partially) focuses on the following:
* Discuss advantages and disadvantages of different scheduling policies for a given platform or system
* Identify (reverse engineer) parameters of a scheduling scheme or a task set from output traces of the system
* Evaluate the scheduling overheads of a given implementation (in the lab)
* Implement event-based scheduling policies on a given microcontroller (in the lab)

## Submission Information
***Deadline: 7th December 2024 23:59 (week 4)***

The assignment is submitted by pushing to the `main` branch of this repository. 
The last commit before the deadline will be considered your submission. 
We recommend frequently committing in order to prevent losing progression. Please note that you cannot directly commit to the main branch. You need to make a merge request between a different branch and main.

The report should be submitted as a `.pdf`, in the root of the main branch your GitLab repository. Use a maximum of **6 pages** for your report (A4, standard font size,  single-column, including images). The filename should be formatted as: `assignment_a_report_group_X.pdf` where `X` is your group number. Inside the report you should specify your names and student numbers. Use the following template for the report: [RTS_assignment-a-template](./templates/assignment-a-template.zip)


> Following your assignment's submission, the TAs might conduct oral checks about the assignment. For example, you might be asked to change parts of the code or explain the report. There will be a follow up announcement with more information.

## Using the console
For this assignment, you can use the built-in Zephyr console and use Zephyr's ``printk`` that works exactly the same way as ``printf`` from the standard C library. **Ensure that ``printk`` is not called in a interrupt routine, it can cause unpredicted behaviour.**

When you want to debug, you just have to uncomment the ``CONSOLE_ENABLED`` macro in ``main.c``. Be aware that initialization will block untill you don't connect the computer to the console. 

**WARNING: The use of print statements will mess up the scheduling timings. When measuring with the logic analyzer, be sure that you don't call any print statement.**

## Assignment questions

### I. Understanding the System
<!-- TODO: Chane the file names once the assignment code if refactored -->
Let's take a look at the system you will be working with for this assignment. The scheduler functions are found in `scheduler.c`. It is already set up with a simple
scheduler `example` and a task set `tasks` in `main.c`. New tasks can be added to the schedule by using the `spawn_task` 
function which takes an execution time (ms), period (ms) and a GPIO pin. This task should execute for the given time 
every period, during which it sets the pin to high. The available pins are listed in `gpio.h`.
1. (2 points) Is the `example` scheduler a real-time scheduler? Justify your answer.
2. (1 points) Calculate the hyper-period of `TASK_SET_1` in `main.c`.
3. (2 points) Design two new non-trivial task sets with at least 3 tasks with a minimum hyper-period of 120ms (non-trivial meaning for instance no two or more same tasks, execution times greater than 1). 
   - One with a processor utilization factor of 1 - `TASK_SET_2`
   - One with a processor utilization factor over 1 - `TASK_SET_3`
   
   Calculate and report the processor utilization factors for both task sets and document these in your report. Note that you will be using these later on in the assignment, so do not make them too difficult. 

   Your main.c should resemble this snippet:
   ```c
   #ifdef TASK_SET_1
      //Task set provided
   #elifdef TASK_SET_2
      //Non-trivial task set, U = 1
   #elifdef TASK_SET_3
      //Non-trivial task set, U > 1   
   #elifdef APERIODIC_TASKS
      // Periodic task set provided in question IV
      // Aperiodic tasks you define based on the requirements in question IV
   #endif
   ```

   You can switch which task set is active by changing `#define TASK_SET_1` (at the top of the file) to the task set of your choice. I.e. `#define TASK_SET_2` for task set 2 and `#define TASK_SET_3` for task set 3
   > **Important**:
   > - **At any point in time, only one of these defines must be present**
   > - Note that the code supports a **maximum of 8 tasks**.

### II. Implementing a Rate Monotonic Scheduler
In `scheduler.c`, implement a rate monotonic scheduler in the `rate_monotonic` function. The scheduling algorithm should 
schedule the first pending task with the shortest period.
Some points about helping you implement this:
- To select the scheduler, change the `schedule` function to call the `rate_monotonic` scheduler.
- The `rate_monotonic` function should select the next task to be executed based on the scheduling algorithm, which in this case is the first pending task with the shortest period, this is done with the `set_active_task` function. When there are no pending tasks, `set_idle` needs to be called.
- The `rate_monotonic` function is invoked when a task is finished or when it has delayed itself. It should delay itself until a higher priority task is ready (preemption of the current running task) or until there is a task ready to be scheduled when there were no tasks running. This delay should be calculated and a `k_timeout_t` struct containing the delay time needs to be returned. Learn more about kernel timing here: [Kernel Timing](https://docs.zephyrproject.org/latest/kernel/services/timing/clocks.html). Some of the functions/macros that you might find useful are `K_MSEC`, `K_USEC`, `sys_timepoint_calc`, `sys_timepoint_timeout` and `k_uptime_get`.
- For the following questions you are expected to add fields to the `Task` struct given in `scheduler_impl.h` in order for you to make a rate monotonic scheduler.
- Check the [Zephyr kernel API reference](https://docs.zephyrproject.org/apidoc/latest/group__kernel__apis.html) for more information. We recommend looking at the pages about the [threads](https://docs.zephyrproject.org/apidoc/latest/group__thread__apis.html) and [system_clock](https://docs.zephyrproject.org/apidoc/latest/group__clock__apis.html) APIs.

4. (12 points) Implement a rate monotonic scheduler in the function `rate_monotonic` in `scheduler.c`. 
   Briefly explain how it works and mention what fields you added to the `Task` struct. Report the output of the logic analyzer for the given task set in `main.c`. Verify that the scheduler works correctly.
5. (4 points) Measure the overhead of your RM scheduler with the given task set in `main.c` and calculate the percentage of the hyperperiod that is spent on this overhead. What actions does the scheduler need to do during this time?
6. (5 points) Explain what overloading is and how your rate monotonic scheduler handles it. Use the given task set in `main.c` with a processor utilization < 1, and the two task sets
   that you made in question 3 (U = 1 and U > 1). Compare and report how the task sets with their different utilizations (U < 1, U > 1) behave. Now use the task set with U = 1. How does this task set's behaviour differ from what is expected in theory?

### III. Implementing an Earliest Deadline First Scheduler
In `scheduler.c`, implement the earliest deadline first scheduler in the `earliest_deadline_first` function.
This scheduling algorithm should schedule the first pending task with the earliest nearing deadline.
Similar points about implementing this:
- Change the `schedule` function to call the `earliest_deadline_first` scheduler. The function should perform similar actions as described in the `rate_monotonic` function, but it should schedule with the EDF algorithm.
- Add necessary fields to the `Task` struct in order for you to make an earliest deadline first scheduler.
- Use the `set_active_task` function to select the task to execute. In this case that should be the first pending task with the earliest nearing deadline.

7. <a id="Question7"></a> (10 points) Implement an earliest deadline first scheduler in the function `earliest_deadline_first` in `scheduler.c`.
   Briefly explain how it works and mention what fields you added to the `Task` struct. Report the output of the logic analyzer for the given task set in `main.c`. Verify that the scheduler works correctly.
8. (4 points) How does your EDF scheduler handle overloading? Is this better or worse than RM? Discuss. Perform the same analysis like you did in question 6.

### IV. Implement Fixed Priority Servers 

Fixed Priority Servers are scheduling algorithms for hybrid task-set with both periodic tasks and aperiodic jobs. In this section you will be designing 3 aperiodic jobs with varing periods. These jobs will be mapped to the switches `SW 1`, `SW 2` and `SW 3` and will spawn when you press the `user_button` on the board. Once you have the 3 aperiodic jobs you will implement 2 scheduling schemes, using your EDF implementation, implemented in [Question 7](#Question7). 

<!-- TODO: Need help to explain how the aperiodic jobs are mapped to the hardware -->

In the `main.c` file, you need to decide the periods for 3 aperiodic jobs such that they meet the following constraints. For the periodic tasks, use the below task-set with your EDF implementation.

| Task             | Execution Time (ms)          | Period (ms) | 
|----------------- |------------------------------|-------------|
| Task1            | 10                           | 40          |
| Task2            | 40                           | 80          |
| Task3            | 60                           | 150         |

<!-- TODO: Actually Verify the above task-set -->

9. (3 points) Design 3 jobs that satisfy below requirements. Justify how the job satisfies the requirements. 
   1. Job should never make a periodic task miss it's deadline, regardless of when it is spawned.
   2. Job should always make atleast 1 periodic task miss it's deadline.
   3. Job can cause task to miss deadline, when spawned at a certain point in the schedule.
   

10. (3 points) ***Background Scheduling:*** Any spawned aperiodic job has the least priority. 
      * Schedule the job when no periodic job is active. 
      * Calculate the `Best case execution time` and `Worst case execution time` for all the jobs. 

11. (4 points) ***Schedule it Immediately:*** Aperiodic Jobs have the highest priority. 
      * Jobs are scheduled before any periodic task. 
      * Calculate the `Best case execution time` and `Worst case execution time` for all the jobs. 
      * Detect when a periodic task misses it's deadline, set the Orange LED (`LDX`) when a deadline is missed. 

<!-- TODO: Find a better name for Schedule it immediately -->