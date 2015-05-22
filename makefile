CC=g++
GCC_OPTIONS=-c -Wall -pedantic -Wno-deprecated

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
GL_OPTIONS=-lglut -lGL -lGLU -lGLEW -lm -lX11 -lXmu -lXi -L/usr/lib64
else
GL_OPTIONS=-framework OpenGL -framework GLUT
endif

OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)
SOURCES=maze.cpp border.cpp gridSpace.cpp ../include/initShader.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=maze

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(GL_OPTIONS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(GCC_OPTIONS) $< -o $@

clean:
	rm *.o maze ../include/initShader.o
