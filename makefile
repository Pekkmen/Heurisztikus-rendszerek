CC = gcc
CFLAGS = -lm

main: main.c
	$(CC) $^ $(CFLAGS) -o $@

.PHONY: clean
clean:
	rm -f main
