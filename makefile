all: fork

fork:
	gcc -pthread -o client client.c 
	gcc -D FORK -pthread -o server server.c 
thread:
	gcc -pthread -o client client.c 
	gcc -D THREAD -pthread -o server server.c 
clean:
	rm -f client
	rm -f server