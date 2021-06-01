CC = g++

# find cpp files in subdirectories
#SOURCES := $(shell find . -name '*.cpp')
SOURCES := Crypto/cpp/*.cpp Client/client.cpp

# find headers
#HEADERS := $(shell find . -name '*.h')
OUTPUT := Client/Client 

server:  
	g++ -g -o Server/server Server/*.cpp Crypto/cpp/*.cpp -pthread 
client:
	g++ -g -o Client/client Client/*.cpp Crypto/cpp/*.cpp