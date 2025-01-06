/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Switch.hpp"
#include "audio.h"
#include "key.hpp"
#include "leds.h"
#include "peripherals.h"
#include "synth.hpp"
#include "usb.h"
#include <math.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

// TEST

Synthesizer synth;
// initializes a memory slab that has 6 blocks that are 400 bytes long, each of which is aligned to a 4-byte boundary
k_tid_t task3_4 = NULL;
/// Function that checks key presses
void check_keyboard() {
  char character;
  while (usbRead(&character, 1)) {
    auto key = Key::char_to_key(character);
    bool key_pressed = false;
    for (int i = 0; i < MAX_KEYS; i++) {
      if (key == keys[i].key && keys[i].state != IDLE) {
        keys[i].state = PRESSED;
        keys[i].hold_time = sys_timepoint_calc(K_MSEC(500));
        keys[i].release_time = sys_timepoint_calc(K_MSEC(500));
        key_pressed = true;
      }
    }
    // The second loop is necessary to avoid selecting an IDLE key when a
    // PRESSED or RELEASED key is located further away on the array
    if (!key_pressed) {
      for (int i = 0; i < MAX_KEYS; i++) {
        if (keys[i].state == IDLE) {
          keys[i].key = key;
          keys[i].state = PRESSED;
          keys[i].hold_time = sys_timepoint_calc(K_MSEC(500));
          keys[i].release_time = sys_timepoint_calc(K_MSEC(500));
          keys[i].phase1 = 0;
          keys[i].phase2 = 0;
          break;
        }
      }
    }
  }
}

void peripheral_update_thread(void *, void *, void *) {
  printuln("peripheral_update_thread");
  while (true) {
    set_led(&debug_led0);
    peripherals_update();
    reset_led(&debug_led0);
    // 50 is smooth
    k_msleep(50);
  }
}


void check_keyboard_thread(void *, void *, void *) {
  printuln("check_keyboard_thread");
  while (true) {
    set_led(&debug_led1);
    check_keyboard();
    reset_led(&debug_led1);
    k_msleep(50);
  }

}


K_SEM_DEFINE(make_write_synth, 0, 1);

static void audio_timer_callback(struct k_timer *timer_id)
{
  k_sem_give(&make_write_synth);
}
K_TIMER_DEFINE(audio_timer, audio_timer_callback, NULL);


void synth_timer_callback(struct k_timer * timer);

K_TIMER_DEFINE(make_synth_timer, synth_timer_callback, NULL);

// Ensure buffer/block size and timer period align
// Buffer for writing to audio driver
// void *mem_block = allocBlock();
// K_MEM_SLAB_DEFINE(audio_buffer, 400, 2, 4);
void *mem_block_synth = allocBlock();
void *mem_block_write = allocBlock();
[[noreturn]] void make_write_synth_thread(void *, void *, void *) {
  printuln("make_write_synth_thread");
  void *block_ptr_active = mem_block_synth;
  void *block_ptr_inactive = mem_block_write;

  while (true) {
    k_timer_start(&make_synth_timer, K_MSEC(500), K_NO_WAIT);
    k_sem_take(&make_write_synth, K_FOREVER);
    // Make synth sound (Red LED, LD5, Task 3, LogicAnalyzer CH2)
    set_led(&debug_led2);
    // synth.makesynth((uint8_t *)mem_block);
    synth.makesynth((uint8_t *)block_ptr_active);
    reset_led(&debug_led2);

    // Write audio block (Blue LED, LD6, Task 4, LogicAnalyzer CH3)
    set_led(&debug_led3);
    // writeBlock(mem_block);
    writeBlock(block_ptr_active);
    reset_led(&debug_led3);

    void *temp = block_ptr_active;
    block_ptr_active = block_ptr_inactive;
    block_ptr_inactive = temp;
    k_msleep(5);
    k_timer_stop(&make_synth_timer);
  }
}


#define GENERAL_STACK_SIZE 20000
#define T1_PRIORITY (1)
#define T2_PRIORITY (2)
#define T3_4_PRIORITY (3)


K_THREAD_STACK_DEFINE(task_1_stack_area, GENERAL_STACK_SIZE);
struct k_thread task_1_data;
K_THREAD_STACK_DEFINE(task_2_stack_area, GENERAL_STACK_SIZE);
struct k_thread task_2_data;
K_THREAD_STACK_DEFINE(task_3_4_stack_area, GENERAL_STACK_SIZE);
struct k_thread task_3_4_data;
void synth_timer_callback(struct k_timer * timer) {
  printuln("OVERLOAD. Abort.");
  // TODO: SET Overload LED
  k_thread_abort(task3_4);
  printuln("Resuming synth");
  task3_4 = k_thread_create(&task_3_4_data, task_3_4_stack_area,
                                 K_THREAD_STACK_SIZEOF(task_3_4_stack_area),
                                 make_write_synth_thread,
                                 NULL, NULL, NULL,
                                 T3_4_PRIORITY, 0, K_NO_WAIT);

}
int main(void) {
  initUsb();
  waitForUsb();

  printuln("== Initializing... ==");

  init_leds();
  initAudio();
  init_peripherals();

  synth.initialize();



  printuln("== Finished initialization ==");

  int64_t time = k_uptime_get();
  k_timer_start(&audio_timer, K_MSEC(5), K_MSEC(5));

  // Check the peripherals input (Green LED, LD4, Task 1, LogicAnalyzer CH0)
  k_tid_t task1 = k_thread_create(&task_1_data, task_1_stack_area,
                                   K_THREAD_STACK_SIZEOF(task_1_stack_area),
                                   peripheral_update_thread,
                                   NULL, NULL, NULL,
                                   T1_PRIORITY, 0, K_NO_WAIT);
  // Get user input from the keyboard (Orange LED, LD3, Task 2, LogicAnalyzer CH1)
  k_tid_t task2 = k_thread_create(&task_2_data, task_2_stack_area,
                                   K_THREAD_STACK_SIZEOF(task_2_stack_area),
                                   check_keyboard_thread,
                                   NULL, NULL, NULL,
                                   T2_PRIORITY, 0, K_NO_WAIT);
  // Make synth sound (Red LED, LD5, Task 3, LogicAnalyzer CH2) and write audio block (Blue LED, LD6, Task 4, LogicAnalyzer CH3)
 task3_4 = k_thread_create(&task_3_4_data, task_3_4_stack_area,
                                 K_THREAD_STACK_SIZEOF(task_3_4_stack_area),
                                 make_write_synth_thread,
                                 NULL, NULL, NULL,
                                 T3_4_PRIORITY, 0, K_NO_WAIT);

  int state = 0;
  while (1) {

    // Run the superloop slightly faster than once every 50 ms
    if (k_uptime_get() - time > BLOCK_GEN_PERIOD_MS-1) {
      time = k_uptime_get();
      if (state) {
        set_led(&status_led0);
      } else {
        reset_led(&status_led0);
      }
      state = !state;
    }


    k_msleep(10);
  }
}
