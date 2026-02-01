#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PIN_STEP 14
#define PIN_DIRECTION 15

static uint pwmSlice;
static uint steps = 0;

void handlePwmWrap()
{
  if (--steps == 0)
  {
    pwm_set_enabled(pwmSlice, false);
  }
}

int main()
{
  stdio_init_all();

  gpio_set_function(PIN_STEP, GPIO_FUNC_PWM);
  gpio_set_function(PIN_DIRECTION, GPIO_FUNC_SIO);
  pwm_set_gpio_level(PIN_STEP, 0);
  pwmSlice = pwm_gpio_to_slice_num(PIN_STEP);
  pwm_set_irq_enabled(pwmSlice, true);
  pwm_set_chan_level(pwmSlice, PWM_CHAN_A, 1);
  pwm_set_enabled(pwmSlice, false);
  irq_set_exclusive_handler(PWM_IRQ_WRAP, handlePwmWrap);

  unsigned char buf[6];

  while (true)
  {
    fread(buf, 1, 6, stdin);
    pwm_set_enabled(pwmSlice, false);
    unsigned char speed = buf[0];
    bool isForward = buf[1];
    steps = buf[2] << 24 | buf[3] << 16 | buf[4] << 8 | buf[5];

    gpio_put(PIN_DIRECTION, isForward);
    if (speed != 0)
    {
      uint16_t offTicks = (uint16_t)((1.0 - ((double)speed) / 255.0) * ~(uint16_t)0);
      if (offTicks == ~(uint16_t)0)
      {
        --offTicks;
      }
      pwm_set_wrap(pwmSlice, 1 + offTicks);
      pwm_set_enabled(pwmSlice, true);
    }
  }
}
