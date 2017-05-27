/*
 * blink.c:
 *	Standard "blink" program in wiringPi. Blinks an LED connected to the GPIO pin 22.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>

// we are using GPIO numbering
#define	LED_GPIO_PIN 22  // gpio 22 , physical 15 

int main (void)
{
  printf ("Raspberry Pi blink\n") ;

  if (wiringPiSetupGpio() < 0)
  {
    fprintf (stderr, "Unable to setup wiringPi with GPIO: %s\n", strerror (errno)) ;
    return 1 ;
  }

  pinMode (LED_GPIO_PIN, OUTPUT) ;

  for (;;)
  {
    digitalWrite (LED_GPIO_PIN, 1) ;	// On
    delay (500) ;		// mS
    digitalWrite (LED_GPIO_PIN, 0) ;	// Off
    delay (500) ;
  }
  return 0 ;
}
