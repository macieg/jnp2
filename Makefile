debuglevel=0
CPP11=-std=c++11
CFLAGS=-Wall
OPT=-O2
CO=-c
DEBUG=-D DEBUG


all: network.o growingnet.o

network.o: network.h network.cc
	if [ $(debuglevel) = 0 ]; then \
		g++ $(CPP11) $(OPT) $(CO) network.cc -o network.o; \
	else \
		g++ $(CPP11) $(CFLAGS) $(CO) $(DEBUG) network.cc -o network.o; \
	fi \

growingnet.o: growingnet.h growingnet.cc
	if [ $(debuglevel) = 0 ]; then \
		g++ $(CPP11) $(OPT) $(CO) growingnet.cc -o growingnet.o; \
	else \
		g++ $(CPP11) $(CFLAGS) $(CO) growingnet.cc -o growingnet.o; \
	fi \

#to niżej do wywalenia potem
network_test: network_test1.o network_test1

network_test1.o: network_test1.c
	gcc $(CFLAGS) $(OPT) $(CO) network_test1.c -o network_test1.o

network_test1: growingnet.o network.o network_test1.o
	g++ $(CPP11) $(CFLAGS) $(OPT) network_test1.o growingnet.o network.o -o network_test1

clean:
	rm *.o
