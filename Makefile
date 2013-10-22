#TODO jak przekazywać argument do pliku w trakcie kompilacji
debuglevel=0
SOURCES=growingnet.h growingnet.cc network.h network.cc

all: $(SOURCES)
	if [ $(debuglevel)=0 ]; 
	then 
		g++ -Wall -O2 growingnet.cc -o growingnet.o;
		g++ -Wall -O2 network.cc -o network.o;
	else
		g++ growingnet.cc -o growingnet.o;
		g++ -Wall -O2 network.cc -o network.o;
	fi

network_test1.o: network_test1.c
	gcc -Wall -O2 -c network_test1.c -o network_test1.o
		
network_test1: growingnet.o network.o network_test1.o
	g++ network_test1.o growingnet.o network.o -o network_test1

clean:
	rm *.o
