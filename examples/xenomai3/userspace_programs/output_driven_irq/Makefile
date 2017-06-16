XENO_CONFIG := /usr/xenomai/bin/xeno-config
# below is wrong =>  must not use --cobalt flag => need wrapping
# see: https://xenomai.org/documentation/xenomai-3/html/man1/xeno-config/index.html
#CFLAGS := $(shell $(XENO_CONFIG) --alchemy --cobalt --cflags)
#LDFLAGS := $(shell $(XENO_CONFIG) --alchemy --cobalt --ldflags)


CFLAGS := -marm $(shell $(XENO_CONFIG)   --posix --alchemy  --cflags)
LDFLAGS := -marm $(shell $(XENO_CONFIG)  --posix --alchemy --ldflags)

#CFLAGS := $(shell $(XENO_CONFIG)   --alchemy  --cflags)
#LDFLAGS := $(shell $(XENO_CONFIG)  --alchemy --ldflags)


EXTRA :=
#CC := $(shell $(XENO_CONFIG) --cc)
CC :=arm-linux-gnueabihf-gcc   

EXECUTABLE := output_driven_irq

all: $(EXECUTABLE)

%: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(EXTRA)

clean:
	rm -f $(EXECUTABLE)


run: $(EXECUTABLE)
	bash run.sh

freshrun: 
	make clean
	make run
