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

 
 
#define VALUE_MAX 33
static	int fd_in=0;
static	int fd_out_write=0;
static	int fd_out_read=0;


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
        rt_printf("Failed to read  value %d to pin %d  ret %d errno %d\n", value, pin,ret,errsv);
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
	    GPIORead(fd_in,PIN);
		GPIOWrite(fd_out_write,POUT, repeat % 2);
	    //GPIORead(fd_out_write,POUT); -> gives error, instead make also read handle for same pin
	    GPIORead(fd_out_read,POUT);
        //rt_task_sleep(1000000000);
		//rt_timer_spin(1000000000);
        rt_timer_spin(500000000); // spin to guarantee that lower priotity tasks have to wait 
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
	//int waitTime = *(int*)args;
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
//	rt_task_start(&hello_task, &runHello, &period);
	rt_task_start(&hello_task, &runHello, NULL);
	rt_task_start(&blink_task, &runBlink, NULL);
}

int run() {
    // NOTE: initialization code is within linux : none-realtime  => should be done in advance
	fd_in=GPIOReadInit(PIN);
    // configure for single pin both an input as output file handle
    // to be able to both write and read it !! (instead of O_RDWR which doesn't work for reading)  
	fd_out_read=GPIOReadInit(POUT);
	fd_out_write=GPIOWriteInit(POUT, 1);

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
