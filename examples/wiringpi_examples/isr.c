/*
 * isr.c:
 *	Wait for Interrupt test program - ISR method -  button connected to a pull electronic scheme at GPIO pin 23
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>


// we are using GPIO numbering
#define	BUTTON_GPIO_PIN	23   //  gpio 23 , physical 16

// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int globalCounter = 0 ;


/*
 * myInterrupt:
 *********************************************************************************
 */

void myInterrupt (void)
{
  ++globalCounter ;
//    fprintf (stderr, "interrupt happened\n") ;
}


/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int main (void)
{
  int myCounter = 0 ;
  printf ("starting ...\n") ; fflush (stdout) ;
    


  if (wiringPiSetupGpio() < 0)
  {
    fprintf (stderr, "Unable to setup wiringPi with GPIO: %s\n", strerror (errno)) ;
    return 1 ;
  }
  pinMode (BUTTON_GPIO_PIN, INPUT) ;  // returns void
  //pullUpDnControl (BUTTON_GPIO_PIN, PUD_UP) ; // returns void

  if (wiringPiISR (BUTTON_GPIO_PIN, INT_EDGE_FALLING, &myInterrupt) < 0)
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }


  for (;;)
  {
    printf ("Waiting ...\n") ; fflush (stdout) ;

    // basicly wait until interrupt has happened
    while (myCounter == globalCounter)
      delay (100) ;

    printf (" Done. counter: %5d\n", globalCounter) ;
      delay (100) ;
    myCounter = globalCounter ;
  }

  return 0 ;
}
