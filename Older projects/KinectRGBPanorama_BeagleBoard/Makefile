CC=g++
CFLAGS=-c -g -Wextra -Wall `pkg-config --cflags opencv`   `pkg-config --cflags libfreenect`  
LIBS=`pkg-config --libs opencv` `pkg-config --libs libfreenect` -L/usr/local/lib -lfreenect_sync
LIBINC=-L/usr/local/lib
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=kinect
OPT=-O0

all: $(SOURCES) $(TARGET)
	
debug: CFLAGS += -g -O0 -Wextra
debug: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(LIBINC) -o $@ $(OBJECTS) $(LIBS)

.cpp.o:
	$(CC) $(OPT) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) $(OBJECTS)
	
