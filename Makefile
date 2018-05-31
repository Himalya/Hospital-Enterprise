

all: hospital

hospital: Hospital_Server.o Hospital_Functions.o
	gcc Hospital_Server.o Hospital_Functions.o -o hospital -lxml2 -lpthread

server.o: Hospital_Server.c Hospital_Functions.o
	gcc -c Hospital_Server.c

functions.o: Hospital_Functions.c
	gcc -c Hospital_Functions.c

clean:
	rm *.o hospital
