/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

#include "task_abstraction.h"
#include "gpio.h"
// Uncomment this macro if you want to print to the console.
//
// WARNING: this will mess up the timings, don't use when you are measuring with
// the logic analyzer.
//
// The initialization function will block until the computer opens the USB
// connection.
#define CONSOLE_ENABLED

// Scheduler Configuration
//
// Only uncomment one #define to select the Task Set
//
#define TASK_SET_1
// #define TASK_SET_2
// #define TASK_SET_3
//#define APERIODIC_TASKS

int main() {
#ifdef CONSOLE_ENABLED
  const struct device *usb_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
  uint32_t dtr = 0;
  int ret = 0;

  // No need to initialize, as it happens during OS initialization, just enable it.
  if (usb_enable(NULL)) {
    return 1;
  }

    // This will block until the computer opens the USB
  while (!dtr) {
    uart_line_ctrl_get(usb_dev, UART_LINE_CTRL_DTR, &dtr);
    k_sleep(K_MSEC(100));
  }
#endif

    init_pins();
    init_peripherals();

#ifdef TASK_SET_1
    //Task Set 1
    spawn_task(2, 20, leds[0]);
    spawn_task(10, 40, leds[1]);
    spawn_task(30, 60, leds[2]);
#elif defined TASK_SET_2
    //Task Set 2
    // U = 1
    spawn_task(20, 40, leds[0]);
    spawn_task(24, 60, leds[1]);
    spawn_task(12, 120, leds[2]);
#elif defined TASK_SET_3
    //Task Set 3
    // U > 1
    spawn_task(10, 20, leds[0]);
    spawn_task(15, 30, leds[1]);
    spawn_task(20, 40,  leds[2]);

#elif defined APERIODIC_TASKS
    // Use define_aperiodic_task() to define aperiodic tasks
#endif

    run_scheduler(EDF);
    k_thread_suspend(k_current_get());

    return 0;
}
