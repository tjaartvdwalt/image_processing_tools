#Project name and version
PROJ = libimproctools
VERSION = 0.0.1

# The path where application will be installed
PREFIX =/usr/local/lib

# The path where the headers will be installed
H_PREFIX =/usr/local/include

# Search for source files in these dirs
VPATH = ./src

# The source files
SRC = image.cpp colorspace.cpp
H_SRC = image.hpp colorspace.hpp

# Derive object file names from source names
OBJ = ${SRC:.cpp=.o}

# compiler flags
CPPFLAGS = 
INCS = 
LIBS = -l opencv_highgui -l opencv_core -l opencv_imgproc
CFLAGS = -Wall -fPIC ${INCS} ${CPPFLAGS}
LDFLAGS = -shared -g ${LIBS}

# compiler command
CC = g++
