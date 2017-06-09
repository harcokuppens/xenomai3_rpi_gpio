XENO_CONFIG := /usr/xenomai/bin/xeno-config
CFLAGS := $(shell $(XENO_CONFIG) --posix --alchemy  --cflags)
#CFLAGS := $(shell $(XENO_CONFIG) --alchemy --cobalt --cflags)
LDFLAGS := $(shell $(XENO_CONFIG) --posix --alchemy  --ldflags) -lsmokey
#LDFLAGS := $(shell $(XENO_CONFIG) --alchemy --cobalt --ldflags) -lsmokey

EXTRA :=
CC := $(shell $(XENO_CONFIG) --cc)

EXECUTABLE := mygpiotest

all: $(EXECUTABLE)

%: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(EXTRA)
