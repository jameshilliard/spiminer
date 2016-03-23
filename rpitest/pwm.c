// gcc pwm.c -lbcm2835

#include <bcm2835.h>
#include <stdio.h>

#define PIN0 RPI_V2_GPIO_P1_12
#define PIN1 RPI_V2_GPIO_P1_35

#define PWM0 0
#define PWM1 1
#define RANGE 2


int main(int argc, char **argv)
{
  if (!bcm2835_init()) {
    printf("bcm2835 init failed\n");
    return 1;
  }
  // Set the output pin to Alt Fun 5, to allow PWM channel 0 to be output there
  bcm2835_gpio_fsel(PIN0, BCM2835_GPIO_FSEL_ALT5);
  bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_2);
  bcm2835_pwm_set_mode(PWM0, 1, 1);
  bcm2835_pwm_set_range(PWM0, RANGE);


  bcm2835_gpio_fsel(PIN1, BCM2835_GPIO_FSEL_ALT5);
  bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_4);
  bcm2835_pwm_set_mode(PWM1, 1, 1);
  bcm2835_pwm_set_range(PWM1, RANGE*2);

  while(1) {
    bcm2835_pwm_set_data(PWM0, 1);
    bcm2835_pwm_set_data(PWM1, 1);
  }
  bcm2835_close();
  return 0;
}


