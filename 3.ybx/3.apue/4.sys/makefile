src = $(wildcard *.c)
targets = $(patsubst %.c, %, $(src))

CC = gcc
CFLAGS =  -D_FILE_OFFSET_BITS=64 -D_XOPEN_SOURCE  -Wall -g -pthread
LDFLAGS+=-lcrypt

all:$(targets)

$(targets):%:%.c
	$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

.PHONY:clean all
clean:
	-rm -rf $(targets) 

