#include <cstdio>
#include "pico/stdlib.h"
#include "hardware_pwm.h"

#define PIN_STEP 14
#define PIN_DIRECTION 15
#define TOGGLE_DIRECTION 255

int main()
{
  stdio_init_all();

  gpio_set_function(PIN_STEP, GPIO_FUNC_PWM);
  pwm_set_gpio_level(PIN_STEP, 0);
  uint pwmSlice = pwm_gpio_to_slice_num(PIN_STEP);

  bool forward = false;

  while (true)
  {
    int rawInput = getchar();
    if (rawInput == -1)
    {
      continue;
    }
    char input = (char)rawInput;
    if (input == TOGGLE_DIRECTION)
    {
      forward = !forward;
      gpio_put(PIN_DIRECTION, forward);
    }
    else if (input == 0)
    {
      pwm_set_chan_level(pwmSlice, PWM_CHAN_A, 0);
    }
    else
    {
      pwm_set_chan_level(pwmSlice, PWM_CHAN_A, 1);
      uint16_t offTicks = (uint16_t)((1.0 - ((double)input) / 255.0) * ~(uint16_t)0);
      pwm_set_wrap(pwmSlice, 1 + offTicks);
    }
  }
}
