
In mygpiotest.c is a copy of the source of the gpiotest programm comming with
xenomai. I include it here so that you can conveniently study its code, and 
optionally build and run it. 

Source remarks
==============

select using FD_CLR,FD_ISSET,FD_SET,and FD_ZERO macros
------------------------------------------------------

When waiting for an interrupt you can either read the device file or  use the
select method to wait of any interrupt within a set of interrupts to be called.
When you use the select method it uses the macros FD_CLR,FD_ISSET,FD_SET,and
FD_ZERO macros to setup the select.

http://www.mkssoftware.com/docs/man3/select.3.asp

 File descriptor masks of type fd_set can be initialized and tested with FD_CLR(), FD_ISSET(), FD_SET(), and FD_ZERO() macros.

FD_CLR(fd, &fdset)

    Clears the bit for the file descriptor fd in the file descriptor set fdset.
    
FD_ISSET(fd, &fdset)

    Returns a non-zero value if the bit for the file descriptor fd is set in the file descriptor set pointed to by fdset, and 0 otherwise.
    
FD_SET(fd, &fdset)

    Sets the bit for the file descriptor fd in the file descriptor set fdset.
    
    
FD_ZERO(&fdset)

    Initializes the file descriptor set fdset to have zero bits for all file descriptors.



