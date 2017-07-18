/* task getting high interrupt priority  */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/io.h>

#include <native/task.h>
#include <native/timer.h>
#include <native/intr.h>

#include  <rtdk.h>

static  RT_INTR myinterrupt;


static RT_TASK IRQHandlerTask;
static RT_TASK SomeTask;


// duration activity in task
static RTIME some_task_activity = 3e9; // in nanoseconds (2900ms)

static int interrupts = 0;      /* number of served interrupts */

#define KEYBOARD_IRQ 1

//  Connect our ISR to the parallel port interrupt.
int setup_ISR(void) {

   // set handler
   // important to propagate interrupts to Linux!
  rt_intr_create(&myinterrupt,"myinterrupt",KEYBOARD_IRQ,I_PROPAGATE);


  // enable the IRQ in the adeos pipeline for Xenomai, so that after this we
  // can expect interupts really be coming!
  rt_printf("enable IRQ\n");
  rt_intr_enable(&myinterrupt);
  // note: by default is keyboard interrupt already enabled,
  //       but we add this line to be sure

  return 0; // no error
}

/*
   IRQHandler increases the interrupts count
 */
void IRQHandler (void *cookie)
{
     int p;
     int number_of_interrupts_waiting;
  RT_TASK *curtask;
  RT_TASK_INFO curtaskinfo;

  // inquire current task
  curtask=rt_task_self();
  rt_task_inquire(curtask,&curtaskinfo);
  rt_printf("Task prio : %d \n", curtaskinfo.cprio);     
  //rt_printf("Task status : %u \n", curtaskinfo.status);     

  for (;;) {
        /* Wait for the next interrupt */
        number_of_interrupts_waiting = rt_intr_wait(&myinterrupt,TM_INFINITE);
        interrupts++;
        rt_printf("at interrupt : interrupts=%d\n",interrupts);
        rt_task_inquire(curtask,&curtaskinfo);
        rt_printf("Task prio : %d \n", curtaskinfo.cprio);     
  }
}

// the some_task Task does some spinning at a high priority
// during the execution of this task the user should press some keys
// at the keyboard to see if the IRQhandler task with lower priority
// will interrupt this task when waiting for an interrupt using rt_intr_wait
void some_task (void *cookie)
{
    int i;

    rt_printf("at start some task : interrupts=%d\n",interrupts);
    rt_timer_spin(some_task_activity);
    rt_printf("at end some task : interrupts=%d\n",interrupts);
   
    return;
}

//startup code
void startup()
{
  int retval;


  rt_printf("start\n");
  rt_timer_set_mode(0);// set clock tick to nanoseconds

  /* setup interupt handler */
  retval=setup_ISR();
  if (retval) {
    rt_printf("\n\n stopped program : problems configuring interrupt handler 
\n\n");
    return;
  }
  // handler with average priority
  rt_task_create(&IRQHandlerTask, "IRQHandlerTask", 0, 50, 0);
  rt_task_start(&IRQHandlerTask, &IRQHandler, NULL);
  // create some other task with a much higher priority than irq handler task!
  rt_task_create(&SomeTask, "SomeTask", 0, 70, 0);
  rt_task_start(&SomeTask, &some_task, NULL);

}

void init_xenomai() {
  /* Avoids memory swapping for this program */
  mlockall(MCL_CURRENT|MCL_FUTURE);

  /* Perform auto-init of rt_print buffers if the task doesn't do so */
  rt_print_auto_init(1);
}

int main(int argc, char* argv[])
{

  sleep(1);
  printf("\nType CTRL-C to end this program\n\n" );
  printf("\nPress some keys to generate interrupts\n" );

  // code to set things to run xenomai
  init_xenomai();

  //startup code
  startup();

  // wait for CTRL-c is typed
  pause();
}
