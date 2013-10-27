debuglevel=0

all: network.o growingnet.o 

network.o: network.h network.cc
	if [ $(debuglevel) = 0 ]; then \
		g++ -std=c++11 -O2 -c network.cc -o network.o; \
	else \
		g++ -std=c++11 -Wall -D DEBUG -c network.cc -o network.o; \
	fi \

growingnet.o: growingnet.h growingnet.cc
	if [ $(debuglevel) = 0 ]; then \
		g++ -std=c++11 -O2 -c growingnet.cc -o growingnet.o; \
	else \
		g++ -std=c++11 -Wall -c growingnet.cc -o growingnet.o; \
	fi \

#to niżej do wywalenia potem
network_test: network_test1.o network_test1

network_test1.o: network_test1.c
	gcc -Wall -O2 -c network_test1.c -o network_test1.o

network_test1: growingnet.o network.o network_test1.o
	g++ network_test1.o growingnet.o network.o -o network_test1

clean:
	rm *.o
