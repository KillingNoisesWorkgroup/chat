CC=gcc
CFLAGS=-c
LDFLAGS=
SOURCES=networking.c
OBJECTS=$(SOURCES:.c=.o)

all: $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o

