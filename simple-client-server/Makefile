CC = gcc
CFLAGS = -g -Wall -Wpointer-arith -Wreturn-type -Wstrict-prototypes
LIBS = -lccn -lcrypto

PROGRAM_CL = client
PROGRAM_SR = server

all: $(PROGRAM_CL) $(PROGRAM_SR)

client: client.o
	$(CC) $(CFLAGS) -o client client.o $(LIBS)

client.o:
	$(CC) $(CFLAGS) -c client.c

server: server.o
	$(CC) $(CFLAGS) -o server server.o $(LIBS)

server.o:
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f *.o
	rm -f $(PROGRAM_CL) $(PROGRAM_SR)

