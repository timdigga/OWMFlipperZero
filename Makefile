CXX = g++
CFLAGS = -Wall -std=c++11

all: spoof_temp

spoof_temp: main.cpp
	$(CXX) main.cpp -o spoof_temp $(CFLAGS)

clean:
	rm -f spoof_temp
