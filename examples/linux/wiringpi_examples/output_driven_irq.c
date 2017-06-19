/*
 * button_toggles_led.c:
 *  combines blink.c and isr.c
 *
 *	Wait for Interrupt from button connected to a pull electronic scheme at GPIO pin 23
 *  When interrupt happens it toggles the output to the LED connected to the GPIO pin 22.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <pthread.h>
#include <unistd.h>


// we are using GPIO numbering
#define	BUTTON_GPIO_PIN	24  
#define	LED_GPIO_PIN    22 

// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int globalCounter = 0 ;
static volatile int globalToggleCounter = 0 ;

pthread_t tid[2];

/*
 * myInterrupt:
 *********************************************************************************
 */

void myInterrupt (void)
{
  static int value=1;
  static int timestamp=0;
  ++globalCounter;
  if ( millis() - timestamp > 100 ) {   // debounce button  
    digitalWrite (LED_GPIO_PIN, value) ;
    value=1-value;	
    ++globalToggleCounter ;
    timestamp=millis();
    printf ("  toggle counter: %5d\n", globalToggleCounter) ;
    printf ("  interrupt counter: %5d\n", globalCounter) ;
  }
//    fprintf (stderr, "interrupt happened\n") ;
   
}


/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

void* toggle_blink(void *arg) {
  for (;;)
  {
    digitalWrite (LED_GPIO_PIN, 1) ;	// On
    delay (500) ;		// mS
    digitalWrite (LED_GPIO_PIN, 0) ;	// Off
    delay (500) ;
  }

} 

int main (void)
{
  int err;  
  printf ("starting ...\n") ; fflush (stdout) ;

  //pthread_t self = pthread_self();
  pthread_t blink;

  err= pthread_create(&blink,NULL,&toggle_blink,NULL); 
  if (err != 0 ) {
    fprintf (stderr, "Unable to setup thread  %s\n", strerror (errno)) ;
  }

  if (wiringPiSetupGpio() < 0)
  {
    fprintf (stderr, "Unable to setup wiringPi with GPIO: %s\n", strerror (errno)) ;
    return 1 ;
  }
  
  pinMode (LED_GPIO_PIN, OUTPUT) ;
  pinMode (BUTTON_GPIO_PIN, INPUT) ;  // returns void
  //pullUpDnControl (BUTTON_GPIO_PIN, PUD_UP) ; // returns void

  if (wiringPiISR (BUTTON_GPIO_PIN, INT_EDGE_FALLING, &myInterrupt) < 0)
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }


  printf ("Waiting ...\n") ; fflush (stdout) ;
  for (;;)
  {
    // basicly wait until interrupt has happened
    waitForInterrupt(BUTTON_GPIO_PIN,-1);
  }

  return 0 ;
}
