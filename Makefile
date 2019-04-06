all: bankingServer bankingClient

bankingServer: bankingServer.c
	gcc -lpthread -o bankingServer bankingServer.c bankingServerLibrary.c

bankingClient: bankingClient.c
	gcc -lpthread -o bankingClient bankingClient.c bankingClientLibrary.c

clean:
	rm -rf bankingClient bankingServer
