CC=g++
CFLAGS=-c -g -Wextra -Wall
LIBS=-lpthread
LIBINC=-L/usr/local/lib
SOURCES=global_controller.c multiwii_driver.c serial.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=controller
OPT=-O0

all: $(SOURCES) $(TARGET)
	
debug: CFLAGS += -g -O0 -Wextra
debug: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(LIBINC) -o $@ $(OBJECTS) $(LIBS)

.o:
	$(CC) $(OPT) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) $(OBJECTS)
	
