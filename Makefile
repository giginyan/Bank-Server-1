CC = gcc
CFLAGS = -Wall -Werror -g -pthread -lm -fsanitize=address

all: bankingServer bankingClient

bankingServer: bankingServer.c bankingServer.h
	$(CC) $(CFLAGS) -o bankingServer bankingServer.c 

bankingServer.o: bankingServer.c bankingServer.h
	$(CC) -c bankingServer.c

bankingClient: bankingClient.c bankingServer.h
	$(CC) $(CFLAGS) -o bankingClient bankingClient.c 

bankingClient.o: bankingClient.c bankingServer.h
	$(CC) -c bankingClient.c

clean:
	rm -f bankingServer *.o
	rm -f bankingClient *.o
