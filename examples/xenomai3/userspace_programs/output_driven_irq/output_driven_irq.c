#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>

#include <rtdm/gpio.h>
 
#define IN  0
#define OUT 1
 
#define LOW  0
#define HIGH 1
 
#define POUT 22  
#define PIN  23  

RTIME SEC = 1000000000llu;
RTIME MSEC = 1000000llu;
RTIME USEC = 1000llu;

RT_TASK blink_task, hello_task, startTasks;
RT_TASK isr_task;

 
 
#define VALUE_MAX 33
static	int fd_in=0;
static	int fd_out_write=0;
static	int fd_out_read=0;

static int GPIOInterruptInit(int pin) {
	int fd=0;
	int do_select = 0;
    //int trigger = GPIO_TRIGGER_LEVEL_LOW;
	int	trigger = GPIO_TRIGGER_EDGE_FALLING;
/*
	} trigger_types[] = {
		{ .name = "edge", .flag = GPIO_TRIGGER_EDGE_RISING },
		{ .name = "edge-rising", .flag = GPIO_TRIGGER_EDGE_RISING },
		{ .name = "edge-falling", .flag = GPIO_TRIGGER_EDGE_FALLING },
		{ .name = "edge-both", .flag = GPIO_TRIGGER_EDGE_FALLING|GPIO_TRIGGER_EDGE_RISING },
		{ .name = "level", .flag = GPIO_TRIGGER_LEVEL_LOW },
		{ .name = "level-low", .flag = GPIO_TRIGGER_LEVEL_LOW },
		{ .name = "level-high", .flag = GPIO_TRIGGER_LEVEL_HIGH },
  */
   
	char path[VALUE_MAX];
    snprintf(path, VALUE_MAX, "/dev/rtdm/pinctrl-bcm2835/gpio%d", pin);
	rt_printf("open device rdwr for interrupt: %s   ---\n",path );
	fd = open(path, O_RDWR);
	if (-1 == fd) {
        int errsv = errno;
        rt_printf("Failed to open  gpio pin %d , return %d errno %d [%s]\n", pin,fd,errsv,symerror(-errsv));
//				rt_printf("failed listening to gpio pin %d [%s]\n", pin, symerror(ret));
		return(-1);
	}
    rt_printf("Succes to open pin %d \n", pin);

	int ret = ioctl(fd, GPIO_RTIOC_IRQEN, &trigger);
    if (ret) {
		//ret = -errno;
		//warning("GPIO_RTIOC_IRQEN failed on %s [%s]", device, symerror(ret));
        int errsv = errno;
        rt_printf("GPIO_RTIOC_IRQEN failed on  gpio pin %d , return %d errno %d\n", pin,fd,errsv);
		return ret;
	}

    return fd;
}




void runIsr(void *args) {
    // NOTE: initialization code is within linux : none-realtime  => should be done in advance
    int value;	
    int ret;
    int fd=fd_in;
    int pin=PIN;
    int switch_count=0;
    int count=0;
    RTIME lasttime,now,lastbounce;
    lasttime=0;
    lastbounce=0;
	for (;;) {
     
        // wait for interrupt 
       	ret = read(fd, &value, sizeof(value));
        count++;
		if (ret < 0) {
			ret = -errno;
			rt_printf("failed reading from gpio pin %d [%s]\n", pin, symerror(ret));
			break;
		}
     
        // handle interrupt
        now =rt_timer_read();
        if ( now > lasttime + 300000000) {
            // new interrupt (no bounce)  
    		rt_printf("bounce time=%llu\n",lastbounce-lasttime);
            lasttime =now;
         
            // on new interrupt we assume switch is triggered and we toggle output 
    		rt_printf("\nINTERRUPT: received irq, GPIO state=%d count=%d\n", value,count);
            switch_count++;
        } else {
            // bounce: ignore  bounce interrupts
            lastbounce=now;
   	        printf("BOUNCE: count=%d, time=%llu\n", count, now);
        } 
	}
}


static int GPIOWriteInit(int pin, int value) {
 
	int fd=0;
	char path[VALUE_MAX];
    snprintf(path, VALUE_MAX, "/dev/rtdm/pinctrl-bcm2835/gpio%d", pin);
	rt_printf("open device wronly: %s   ---\n",path );
	fd = open(path, O_WRONLY);
	//fd = open(path, O_RDWR|O_NONBLOCK); // cannot read from filehandle even with O_RDWR
	//fd = open(path, O_RDWR); // cannot read from filehandle even with O_RDWR
	if (-1 == fd) {
        int errsv = errno;
        rt_printf("Failed to failed to gpio pin %d , return %d errno %d\n", pin,fd,errsv);
		return(-1);
	}
    rt_printf("Succes to open pin %d\n", pin);

	rt_printf("set device output mode: %s   ---\n",path );
    int ret =ioctl(fd, GPIO_RTIOC_DIR_OUT, &value);
    int errsv = errno;
    if (ret) {
        rt_printf("Failed to set output mode to pin %d  ret %d errno %d\n", pin,ret,errsv);
        return ret;
    }
    rt_printf("Succes to set output mode to pin %d\n",pin);
    
    return fd;
}

static int GPIOReadInit(int pin) {
	int fd=0;
     
	char path[VALUE_MAX];
    snprintf(path, VALUE_MAX, "/dev/rtdm/pinctrl-bcm2835/gpio%d", pin);
	rt_printf("open device rdonly: %s   ---\n",path );
	fd = open(path, O_RDONLY|O_NONBLOCK);
	if (-1 == fd) {
        int errsv = errno;
        rt_printf("Failed to failed to gpio pin %d , return %d errno %d\n", pin,fd,errsv);
		return(-1);
	}
    rt_printf("Succes to open pin %d \n", pin);

	rt_printf("set device input mode: %s   ---\n",path );
    int ret =ioctl(fd, GPIO_RTIOC_DIR_IN);
    int errsv = errno;
    if (ret) {
        rt_printf("Failed to set input mode to pin %d  ret %d errno %d\n", pin,ret,errsv);
        return ret;
    }
    rt_printf("Succes to set input mode to pin %d\n",pin);
    
    return fd;
}


static int GPIOWrite(int fd, int pin, int value) {

    rt_printf("write  value %d to pin %d \n", value, pin);
    int ret = write(fd, &value, sizeof(value));
    int errsv = errno;
    if (sizeof(value) != ret ) {
        rt_printf("Failed to write  value %d to pin %d  ret %d errno %d\n", value, pin,ret,errsv);
		return(-1);
	}
	return(0);
}

static int GPIORead(int fd, int pin) {
    int value;
    //rt_printf("Try to read input pin %d \n",pin);
    int ret = read(fd, &value, 4);
    int errsv = errno;
	if (-1 == ret ) {
        rt_printf("Failed to read  value from pin %d  ret %d errno %d\n", pin,ret,errsv);
		return(-1);
	}
    //rt_printf("Succes to read input pin %d with value %d\n",pin,value);
    rt_printf("read input pin %d with value %d\n",pin,value);
 
 
	return(0);
}

void blink( int repeat, int timeout) {
	rt_printf("blink(%d, %d)\n", repeat, timeout);
	do {
		rt_printf("Blink now\n");
		GPIOWrite(fd_out_write,POUT, repeat % 2);
	    GPIORead(fd_out_read,POUT);
        rt_timer_spin(500000000);
        //rt_task_sleep(500000000);
	} while (repeat--);
	rt_printf("Done blinking!\n");
}



void runBlink(void *arg) {
	rt_printf("Starting blink...\n");
    // loop 10 times to toggle between 0 and 1 value  => 5 blinks
    // period:  1 second   => duration light on or off
	blink( 10, 1);
}

/* dummy task with higher prio */
void runHello(void *args) {
	int count = 5;
	int waitTime = *(int*)args;
	rt_printf("Starting hello world task... should be printed after blinking done!!\n");
	do {
		rt_printf("Hello world!\n");
        rt_task_sleep(500000000);
	} while (--count);
	rt_printf("Done running!\n");
}

// task to start tasks
void runStartTasks(void *args) {
	int period = 100;
	rt_task_start(&isr_task, &runIsr, NULL);
	rt_task_start(&blink_task, &runBlink, NULL);
	rt_task_start(&hello_task, &runHello, &period);
}

int run() {
    // NOTE: initialization code is within linux : none-realtime  => should be done in advance
	fd_in=GPIOInterruptInit(PIN); 
    // configure for single pin both an input as output file handle
    // to be able to both write and read it !! (instead of O_RDWR which doesn't work for reading)  
	fd_out_read=GPIOReadInit(POUT);
	fd_out_write=GPIOWriteInit(POUT, 1);

    rt_task_create(&isr_task, "IsrTask", 0, 60, 0);
	rt_task_create(&blink_task, "BlinkTask", 0, 50, 0);
	rt_task_create(&hello_task, "Task", 0, 40, 0);
        
        // task to start tasks
	rt_task_create(&startTasks, "StartTask", 0, 99, 0);
	rt_task_start(&startTasks, &runStartTasks, NULL);
}
 
int main(int argc, char *argv[]) {
	 	
 	run();
 	getchar();
 	
    close(fd_in);
    close(fd_out_write);
    close(fd_out_read);
	return(0);
}
