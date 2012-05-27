CC=gcc
CFLAGS=-c
LDFLAGS= -lpthread
SOURCES=logging.c session.c dynamic_array.c memory_dump.c login_entry.c database.c main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=server.exe

all: clean $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	make --directory=../shared/
	$(CC) -o $@ $(OBJECTS) ../shared/networking.o $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *.exe

test: 
	./$(EXECUTABLE)
