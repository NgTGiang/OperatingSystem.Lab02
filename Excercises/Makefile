CC=gcc
CFLAGS=-pthread -o
TARGETS=sum_serial sum_multi_thread

all: $(TARGETS)

sum_serial: sum_serial.c
	$(CC) sum_serial.c -o sum_serial

sum_multi_thread: sum_multi_thread.c
	$(CC) sum_multi_thread.c -o sum_multi_thread

clean:
	rm -f $(TARGETS)