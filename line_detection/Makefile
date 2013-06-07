CC=g++
CFLAGS=-c -Wall `pkg-config opencv --cflags`
#LDFLAGS=-L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
LIBS=-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
LIBINC=-L/usr/local/lib
SOURCES=line_detection.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=line_detect
OPT=-O0

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LIBINC) -o $@ $(OBJECTS) $(LIBS)

.cpp.o:
	$(CC) $(OPT) $(CFLAGS) $< -o $@