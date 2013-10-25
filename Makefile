debuglevel=0
SOURCES=growingnet.h growingnet.cc network.h network.cc

all: $(SOURCES)
	if [ $(debuglevel) = 0 ]; then \
		g++ -O2 network.cc -o network; \
	else \
		g++ -Wall -D DEBUG network.cc -o network; \
	fi \

#to niżej do wywalenia potem
network_test1.o: network_test1.c
	gcc -Wall -O2 -c network_test1.c -o network_test1.o

network_test1: growingnet.o network.o network_test1.o
	g++ network_test1.o growingnet.o network.o -o network_test1

clean:
	rm *.o
