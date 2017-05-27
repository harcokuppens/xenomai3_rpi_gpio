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
 
#define PIN  18  /* P1-18    not used */
#define POUT 17  /* gpio pin 17, physical 11 ,   P1-07 */

RTIME SEC = 1000000000llu;
RTIME MSEC = 1000000llu;
RTIME USEC = 1000llu;

RT_TASK blink_task, task, startTask;

 
 
static int GPIORead(int pin) {
#define VALUE_MAX 33
	char path[VALUE_MAX];
	char value_str[3];
	int fd;
 
	//snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
        snprintf(path, VALUE_MAX, "/dev/rtdm/pinctrl-bcm2835/gpio%d", pin);
	rt_printf("Im opening device readonly: %s   ---\n",path );
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		rt_printf("Failed to open gpio value for reading!\n");
		return(-1);
	}
    int ret = ioctl(fd, GPIO_RTIOC_DIR_IN);
    if (ret) {
        rt_printf("Failed to set input mode to pin %d!\n", pin);
        return ret;
    }
 
	if (-1 == read(fd, value_str, 3)) {
		rt_printf("Failed to read value!\n");
		return(-1);
	}
 
	close(fd);
 
	return(atoi(value_str));
}
 
    typedef unsigned char BYTE;
static	int fd=0;
static int GPIOWriteInit(int pin, int value) {
	static const char s_values_str[] = "01";
 
	char path[VALUE_MAX];
	//int fd;
 
	//snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
        snprintf(path, VALUE_MAX, "/dev/rtdm/pinctrl-bcm2835/gpio%d", pin);
	rt_printf("open device wronly: %s   ---\n",path );
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
        int errsv = errno;
        rt_printf("Failed to failed to gpio pin %d , return %d errno %d\n", pin,fd,errsv);
//		rt_printf("Failed to open gpio %d for writing %d!\n", pin, value);
		return(-1);
	}
    rt_printf("Succes to open pin %d  ret %d errno %d\n", pin,fd,errno);

    BYTE b=1; 
	rt_printf("set device output mode: %s   ---\n",path );
    int ret =ioctl(fd, GPIO_RTIOC_DIR_OUT, &value);
    //int ret =ioctl(fd, GPIO_RTIOC_DIR_OUT, &b);
    //int ret=0 ;
    if (ret) {
        int errsv = errno;
		//rt_printf("Failed to write value %d to pin %d!\n", value, pin);
        rt_printf("Failed to set output mode to pin %d  ret %d errno %d\n", pin,ret,errsv);
        //rt_printf("Failed to set output mode to pin %d error %d!\n", pin,ret);
        return ret;
    }
    rt_printf("Succes to set output mode to pin %d  ret %d errno %d\n", pin,ret,errno);
    
    return 0;
}
static int GPIOWrite(int pin, int value) {
	static const char s_values_str[] = "01";
 
	char path[VALUE_MAX];
 
	//snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
        snprintf(path, VALUE_MAX, "/dev/rtdm/pinctrl-bcm2835/gpio%d", pin);
/*
	rt_printf("open device wronly: %s   ---\n",path );
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		rt_printf("Failed to open gpio %d for writing %d!\n", pin, value);
		return(-1);
	}


	rt_printf("set device output mode: %s   ---\n",path );
    int ret =ioctl(fd, GPIO_RTIOC_DIR_OUT, &value);
    //int ret=0 ;
    if (ret) {
        rt_printf("Failed to set output mode to pin %d error %d!\n", pin,ret);
        return ret;
    }
*/
/*
     rt_printf("Try write  value %d with bytesize  %d\n", value, sizeof(value));
    //ret = write(fd, &value, sizeof(value));
    ret = write(fd, &value, 1);
    if (ret) {
        rt_printf("Failed to write  value %d to pin %d  error %d\n", value, pin,ret);
        return ret;
    }
*/
    rt_printf("write  value %d to pin %d \n", value, pin);
    //int ret = write(fd, &s_values_str[LOW == value ? 0 : 1], 1);
    int ret = write(fd, &value, sizeof(value));
    
     BYTE b = LOW == value ? 0 : 1;
    //int ret = write(fd,&b , 1);
    
    // size_t count = sizeof(b); // how many bytes
    //int ret = write(fd,(void *)&b ,b);
    //ret = write(fd, &value, 1);
        int errsv = errno;
    if (1 != ret ) {
		//rt_printf("Failed to write value %d to pin %d!\n", value, pin);
        rt_printf("Failed to write  value %d to pin %d  ret %d errno %d\n", value, pin,ret,errsv);
		return(-1);
	}
    close(fd);
 
 
///	close(fd);
	return(0);
}

void lblink(int pin, int repeat, int timeout) {
	rt_printf("blink(%d, %d, %d)\n", pin, repeat, timeout);
	do {
		rt_printf("Blink now\n");
		GPIOWrite(pin, repeat % 2);
		sleep(timeout);
	} while (repeat--);
	rt_printf("Done blinking!\n");
}

void blink( int pin, int repeat, int timeout) {
	rt_printf("blink(%d, %d, %d)\n", pin, repeat, timeout);
	do {
		//GPIORead(pin);
		rt_printf("Blink now\n");
		GPIOWrite(pin, repeat % 2);
		//sleep(timeout);
   //     rt_task_sleep(1000000000);
        
		rt_timer_spin(500000000);
	} while (repeat--);
	rt_printf("Done blinking!\n");
}



void runBlink(void *arg) {
	rt_printf("Starting blink...\n");
        // gpio pin POUT  , loop 10 times to toggle between 0 and 1 value  => 5 blinks
        //  sleep 1000 ms   => duration light on or off
	blink(POUT, 10, 1);
}

/* dummy task with higher prio */
void runTask(void *args) {
	int count = 5;
	int waitTime = *(int*)args;
	rt_printf("Starting hello world task... should be printed after blinking done!!\n");
	do {
		//rt_printf("Hello world!\n");
		printf("Hello world!\n");
        rt_task_sleep(500000000);
	//	rt_timer_spin(waitTime);
		//rt_timer_spin(500000000);
        //fflush( stdout ); // ?? realtime
   ///     sleep(1);
	} while (--count);
	rt_printf("Done running!\n");
}

// task to start tasks
void startTasks(void *args) {
	int period = 100;
	rt_task_start(&blink_task, &runBlink, NULL);
	rt_task_start(&task, &runTask, &period);
}

int run() {
	//lblink(POUT, 10, 1);
    //return 0;
		GPIOWriteInit(POUT, 1);

	rt_task_create(&blink_task, "BlinkTask", 0, 50, 0);
	rt_task_create(&task, "Task", 0, 40, 0);
        
        // task to start tasks
	rt_task_create(&startTask, "StartTask", 0, 99, 0);
	rt_task_start(&startTask, &startTasks, NULL);
}
 
int main(int argc, char *argv[]) {
	 	
 	run();
 	getchar();
 	
	return(0);
}
