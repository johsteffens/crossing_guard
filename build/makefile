CC      = gcc
CFLAGS  = -Wall -O3 -std=c11 
LDFLAGS = -lm -lpthread

BETH_SRCS    = ../../beth-master/lib
CROSSING_GUARD_SRCS = ../src

BETH_BCORE_SRCS = $(BETH_SRCS)/bcore

INCLUDES = \
    -I $(BETH_BCORE_SRCS)

SRCS = \
    $(wildcard $(BETH_BCORE_SRCS)/*.c) \
    $(wildcard $(CROSSING_GUARD_SRCS)/*.c)
    
HDRS = \
    $(wildcard $(BETH_BCORE_SRCS)/*.h) \
    $(wildcard $(CROSSING_GUARD_SRCS)/*.h)

TARGET = crossing_guard

$(TARGET): $(SRCS) $(HDRS)
	$(CC) -o $@ $(SRCS) $(CFLAGS) $(INCLUDES) $(LDFLAGS) 

.PHONY: clean

clean:
	rm -f $(TARGET)
