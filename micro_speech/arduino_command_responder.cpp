/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "command_responder.h"

#include "am_bsp.h" // NOLINT


// Leds
#define LED1 9
#define LED2 10
#define LED3 8
#define LED4 14

int32_t leds[] = {LED1, LED2, LED3, LED4};
int32_t level = 0;

// This implementation will light up the LEDs on the board in response to
// different commands.
void RespondToCommand(tflite::ErrorReporter *error_reporter,
                      int32_t current_time, const char *found_command,
                      uint8_t score, bool is_new_command)
{
  static bool is_initialized = false;
  if (!is_initialized)
  {
    // Setup LED's as outputs
    // Blue led heart beat
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_BLUE, g_AM_HAL_GPIO_OUTPUT_12);
    // Level indicator leds
    for (int i = 0; i < 4; i++)
    {
      am_hal_gpio_pinconfig(leds[i], g_AM_HAL_GPIO_OUTPUT_12);
      // initially off
      am_hal_gpio_output_clear(leds[i]);
    }

    is_initialized = true;
  }
  static int count = 0;

  // Toggle the blue LED every time an inference is performed.
  ++count;
  if (count & 1)
  {
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_BLUE);
  }
  else
  {
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_BLUE);
  }

  if (is_new_command)
  {
    error_reporter->Report("Heard %s (%d) @%dms", found_command, score,
                           current_time);
    // Level up if 'up' was heard.
    if (found_command[0] == 'u' && found_command[1] == 'p')
    {
      if (level < 4)
      {
        level++;
      }
      error_reporter->Report("UP level(%d)\n", level);
    }
    // Level down if 'down' was heard.
    if (found_command[0] == 'd' && found_command[1] == 'o')
    {
      if (level > 0)
      {
        level--;
      }
      error_reporter->Report("DOWN level(%d)\n",level);
    }
    if (found_command[0] == 'u' && found_command[1] == 'n')
    {
      error_reporter->Report("\nUNKNOWN");
    }
    // display actual level
    for (int i = 0; i < level; i++)
    {
      am_hal_gpio_output_set(leds[i]);
    }
    for (int i = level; i < 4; i++)
    {
      am_hal_gpio_output_clear(leds[i]);
    }
  }
}
