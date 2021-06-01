CC = g++

# find cpp files in subdirectories
#SOURCES := $(shell find . -name '*.cpp')
SOURCES := Crypto/cpp/*.cpp Client/client.cpp

# find headers
#HEADERS := $(shell find . -name '*.h')
OUTPUT := Client/Client 

all:  
	g++ -g -o $(OUTPUT) $(SOURCES)