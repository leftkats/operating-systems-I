all : client server
client: client.c
	gcc client.c -o client

server: server.c
	gcc -pthread -lm server.c -o server

clean: 
	rm -f client server
