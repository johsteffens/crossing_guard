CC      = gcc
CFLAGS  = -Wall -O3 -std=c11 
LDFLAGS = -lm -lpthread -latomic

BETH_SRCS    = ../../beth/lib
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
ifeq ($(wildcard $(BETH_BCORE_SRCS)/*.c),)
	@echo "Sources of beth-library missing: $(BETH_BCORE_SRCS). Clone from https://github.com/johsteffens/beth.git"
else
	$(CC) -o $@ $(SRCS) $(CFLAGS) $(INCLUDES) $(LDFLAGS) 
endif

.PHONY: clean

clean:
	rm -f $(TARGET)
