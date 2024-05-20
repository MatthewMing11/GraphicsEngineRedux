SYSCONF_LINK = g++
LDFLAGS      = -O3
LIBS         = -lSDL2

DESTDIR = ./
TARGET  = main

OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: 
	g++ main.cpp matrix.cpp model.cpp rasterizer3D.cpp tgaimage.cpp vec3f.cpp $(LIBS)